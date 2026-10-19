const http = require('http');
const https = require('https');

// =========================================================================
// chronosMIS 24/7 Live Biometric Sync Daemon
// Automatically streams every biometric punch from ESP32 to Firebase Cloud
// =========================================================================

const DEVICE_IP = process.argv[2] || process.env.DEVICE_IP || '192.168.0.108';
const FB_URL = process.env.FB_URL || 'https://chronosmis-default-rtdb.firebaseio.com';
const POLL_INTERVAL_MS = 3000; // Check device every 3 seconds

let lastKnownPunchesSignature = '';
let isSyncing = false;
let consecutiveErrors = 0;
let lastHeartbeatTime = 0;

function httpGetRaw(url, timeoutMs = 4000) {
  return new Promise((resolve, reject) => {
    const req = http.get(url, { headers: { Cookie: 'session=admin_active' }, timeout: timeoutMs }, (res) => {
      let data = '';
      res.on('data', chunk => data += chunk);
      res.on('end', () => resolve(data));
    });
    req.on('timeout', () => { req.destroy(); reject(new Error('Device connection timeout')); });
    req.on('error', reject);
  });
}

function httpGetJson(url, timeoutMs = 4000) {
  return httpGetRaw(url, timeoutMs).then(data => {
    try {
      return JSON.parse(data);
    } catch (e) {
      throw new Error(`Invalid JSON response: ${e.message}`);
    }
  });
}

function firebasePut(path, data) {
  return new Promise((resolve, reject) => {
    const payload = JSON.stringify(data);
    const url = new URL(FB_URL + path);
    const req = https.request(url, {
      method: 'PUT',
      headers: {
        'Content-Type': 'application/json',
        'Content-Length': Buffer.byteLength(payload)
      },
      timeout: 8000
    }, (res) => {
      let respData = '';
      res.on('data', chunk => respData += chunk);
      res.on('end', () => {
        if (res.statusCode >= 200 && res.statusCode < 300) {
          resolve(respData);
        } else {
          reject(new Error(`Firebase HTTP ${res.statusCode}: ${respData}`));
        }
      });
    });
    req.on('timeout', () => { req.destroy(); reject(new Error('Firebase connection timeout')); });
    req.on('error', reject);
    req.write(payload);
    req.end();
  });
}

function normalizeDate(d) {
  if (!d) return '';
  d = String(d).trim();
  if (d.indexOf('T') !== -1) d = d.split('T')[0];
  d = d.replace(/[\/\.]/g, '-');
  const parts = d.split('-');
  if (parts.length === 3) {
    if (parts[0].length === 4) return `${parts[0]}-${parts[1].padStart(2, '0')}-${parts[2].padStart(2, '0')}`;
    if (parts[2].length === 4) return `${parts[2]}-${parts[1].padStart(2, '0')}-${parts[0].padStart(2, '0')}`;
  }
  return d;
}

function formatDateDisplay(d) {
  if (!d || d === '-' || d === '00-00-0000') return '-';
  const norm = normalizeDate(d);
  const parts = norm.split('-');
  if (parts.length === 3 && parts[0].length === 4) {
    const months = ['Jan','Feb','Mar','Apr','May','Jun','Jul','Aug','Sep','Oct','Nov','Dec'];
    const mIdx = parseInt(parts[1], 10) - 1;
    const day = parseInt(parts[2], 10);
    const monthName = months[mIdx] || parts[1];
    return `${day < 10 ? '0' + day : day} ${monthName} ${parts[0]}`;
  }
  return d;
}

async function performLiveSync() {
  if (isSyncing) return;
  isSyncing = true;

  try {
    const now = new Date();
    const nowTs = Math.floor(Date.now() / 1000);
    const todayIso = now.toISOString().split('T')[0];
    const todayDisplay = formatDateDisplay(todayIso);

    // 1. Fetch current users list with their daily punches
    const users = await httpGetJson(`http://${DEVICE_IP}/list-users`, 3500);
    consecutiveErrors = 0;

    // Create signature to detect new punches
    let currentSignature = '';
    const todayPunches = [];
    const empMap = {};

    for (const u of users) {
      const key = u.roll || u.uid;
      const isFp = u.uid && u.uid.endsWith('_f');
      const fpSlot = isFp ? u.uid.replace('_f', '') : '';
      const hasIn = (u.in && u.in !== '-');
      const hasOut = (u.out && u.out !== '-');

      currentSignature += `${key}:${u.in || '-'}:${u.out || '-'};`;

      empMap[key] = {
        roll: u.roll || key,
        name: u.name || 'Staff',
        role: u.role || 'Staff',
        dept: (u.dept && u.dept !== '-') ? u.dept : 'General',
        dob: u.dob || '',
        rfid: (u.rfid && u.rfid !== 'NOMATCH') ? u.rfid : '',
        fingerId: fpSlot,
        templateStatus: isFp ? `Enrolled (Slot #${fpSlot})` : 'None',
        shiftId: u.shiftId || 0,
        shiftStart: u.shiftStart || '09:00',
        shiftEnd: u.shiftEnd || '17:00',
        shiftGrace: u.shiftGrace || 15,
        state: u.state || '0',
        firstIn: hasIn ? u.in : '-',
        lastOut: hasOut ? u.out : '-',
        lastDate: hasIn ? todayIso : '',
        lastDateFormatted: hasIn ? todayDisplay : '-',
        lastSync: nowTs
      };

      if (hasIn) {
        todayPunches.push({
          roll: u.roll || key,
          name: u.name || 'Staff',
          dept: (u.dept && u.dept !== '-') ? u.dept : 'General',
          dir: 'In',
          time: u.in,
          date: todayIso,
          dateFormatted: todayDisplay,
          status: (u.in <= '09:15') ? 'On-Time' : 'Late',
          method: isFp ? 'Fingerprint' : 'RFID Card',
          device: 'ESP32 Biometric Device',
          timestamp: nowTs
        });
      }

      if (hasOut) {
        todayPunches.push({
          roll: u.roll || key,
          name: u.name || 'Staff',
          dept: (u.dept && u.dept !== '-') ? u.dept : 'General',
          dir: 'Out',
          time: u.out,
          date: todayIso,
          dateFormatted: todayDisplay,
          status: 'On-Time',
          method: isFp ? 'Fingerprint' : 'RFID Card',
          device: 'ESP32 Biometric Device',
          timestamp: nowTs
        });
      }
    }

    // Check if new punches arrived or first run
    const isFirstRun = (lastKnownPunchesSignature === '');
    const hasPunchChanges = (currentSignature !== lastKnownPunchesSignature);

    if (hasPunchChanges) {
      if (!isFirstRun) {
        console.log(`\n⚡ [LIVE PUNCH DETECTED at ${now.toLocaleTimeString()}] Syncing to chronosMIS...`);
      } else {
        console.log(`[${now.toLocaleTimeString()}] Baseline connected. Enrolled staff: ${users.length}, Today punches: ${todayPunches.length}`);
      }

      // Update Firebase
      await firebasePut('/employees.json', empMap);

      // Upload today's punches
      const punchObject = {};
      todayPunches.forEach((p, idx) => {
        const key = `PUNCH_${normalizeDate(p.date)}_${p.roll}_${p.dir}_${p.time.replace(/:/g, '')}_${idx}`;
        punchObject[key] = p;
      });

      // Fetch history once on start or if punch count changed to ensure audit log is 100% complete
      try {
        const historyRaw = await httpGetRaw(`http://${DEVICE_IP}/get-history?month=${now.getMonth()+1}&year=${now.getFullYear()}`, 4000);
        let currentDate = '';
        for (let line of historyRaw.split('\n')) {
          line = line.trim();
          if (!line || line.startsWith('Time,')) continue;
          if (line.startsWith('#DATE:')) {
            currentDate = line.substring(6).trim();
          } else {
            const parts = line.split(',');
            if (parts.length >= 7) {
              const pDate = currentDate || todayDisplay;
              const isoD = normalizeDate(pDate);
              const key = `PUNCH_${isoD}_${parts[1]}_${parts[4] || 'In'}_${(parts[0] || '').replace(/:/g, '')}`;
              punchObject[key] = {
                time: parts[0],
                roll: parts[1],
                name: parts[2],
                role: parts[3],
                dept: (empMap[parts[1]] && empMap[parts[1]].dept) ? empMap[parts[1]].dept : 'General',
                dir: parts[4] || 'In',
                status: parts[5] || 'On-Time',
                method: parts[6] || 'Fingerprint',
                date: isoD,
                dateFormatted: formatDateDisplay(isoD),
                device: 'ESP32 Biometric Device'
              };
            }
          }
        }
      } catch (e) {
        // Fallback to today punches
      }

      await firebasePut('/punches.json', punchObject);
      lastKnownPunchesSignature = currentSignature;

      if (!isFirstRun) {
        console.log(`✔ [SYNCHRONIZED] Punches successfully beamed to https://chronosmis.web.app in real-time!\n`);
      }
    }

    // Heartbeat every 30 seconds
    if (nowTs - lastHeartbeatTime > 30) {
      lastHeartbeatTime = nowTs;
      let freeHeap = 0;
      let uptime = 0;
      try {
        const sys = await httpGetJson(`http://${DEVICE_IP}/sys-info`, 2000);
        freeHeap = sys.freeHeap || 0;
        uptime = sys.uptime || 0;
      } catch (err) {}

      await firebasePut('/device_status.json', {
        status: 'Online',
        device: 'ESP32 Biometric Device',
        ip: DEVICE_IP,
        freeHeap: freeHeap,
        uptime: uptime,
        lastSeen: nowTs,
        lastSyncDate: todayIso,
        lastSyncFormatted: `${todayDisplay}, ${now.toLocaleTimeString()}`
      });
      process.stdout.write(`.`);
    }

  } catch (err) {
    consecutiveErrors++;
    if (consecutiveErrors === 1 || consecutiveErrors % 20 === 0) {
      console.log(`\n⚠️ [${new Date().toLocaleTimeString()}] Waiting for ESP32 device at http://${DEVICE_IP}/ (${err.message})`);
    }
  } finally {
    isSyncing = false;
  }
}

console.log('================================================================');
console.log('⚡ CHRONOS MIS 24/7 LIVE BIOMETRIC SYNC DAEMON');
console.log('================================================================');
console.log(`📡 Monitoring ESP32 at: http://${DEVICE_IP}`);
console.log(`☁️ Live Firebase Cloud: ${FB_URL}`);
console.log(`⏱️ Polling Frequency: Every ${POLL_INTERVAL_MS / 1000} seconds`);
console.log('----------------------------------------------------------------');
console.log('Scanning for punches... (Keep this window open or running in background)\n');

// Start the live polling loop
performLiveSync();
setInterval(performLiveSync, POLL_INTERVAL_MS);
