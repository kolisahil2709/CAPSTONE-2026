const http = require('http');
const https = require('https');

const DEVICE_IP = '192.168.0.108';
const FB_URL = 'https://chronosmis-default-rtdb.firebaseio.com';

function httpGetRaw(url) {
  return new Promise((resolve, reject) => {
    http.get(url, { headers: { Cookie: 'session=admin_active' } }, (res) => {
      let data = '';
      res.on('data', chunk => data += chunk);
      res.on('end', () => resolve(data));
    }).on('error', reject);
  });
}

function httpGetJson(url) {
  return httpGetRaw(url).then(data => {
    try {
      return JSON.parse(data);
    } catch (e) {
      throw new Error(`Failed to parse JSON from ${url}: ${e.message} (Data: ${data.slice(0, 100)}...)`);
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
      }
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
    if (parts[0].length === 4) {
      // YYYY-MM-DD
      return `${parts[0]}-${parts[1].padStart(2, '0')}-${parts[2].padStart(2, '0')}`;
    } else if (parts[2].length === 4) {
      // DD-MM-YYYY -> YYYY-MM-DD
      return `${parts[2]}-${parts[1].padStart(2, '0')}-${parts[0].padStart(2, '0')}`;
    }
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

async function run() {
  console.log('====================================================');
  console.log('🚀 chronosMIS Complete Multi-Date Cloud Sync Engine');
  console.log('📡 Device: http://' + DEVICE_IP);
  console.log('☁️ Firebase: ' + FB_URL);
  console.log('====================================================\n');

  const now = new Date();
  const nowTs = Math.floor(Date.now() / 1000);
  const todayIso = now.toISOString().split('T')[0]; // "2026-10-16"
  const todayDisplay = formatDateDisplay(todayIso);

  // 1. Fetch system info
  console.log('--- 1. Fetching Device Diagnostics ---');
  let sys = {};
  try {
    sys = await httpGetJson(`http://${DEVICE_IP}/sys-info`);
    console.log(`✓ Device Responsive! Free Heap: ${sys.freeHeap}B | Uptime: ${sys.uptime}s`);
  } catch(e) {
    console.log(`Notice on sys-info: ${e.message}`);
  }

  // 2. Publish Live Device Status
  console.log('\n--- 2. Publishing Live Device Status to Firebase Cloud ---');
  await firebasePut('/device_status.json', {
    status: 'Online',
    device: 'ESP32 Biometric Device',
    uptime: sys.uptime || 0,
    freeHeap: sys.freeHeap || 0,
    ip: DEVICE_IP,
    rssi: sys.wifiRssi || 0,
    lastSeen: nowTs,
    lastSyncDate: todayIso,
    lastSyncFormatted: `${todayDisplay}, ${now.toLocaleTimeString()}`
  });
  await firebasePut('/device_info.json', {
    deviceName: 'ESP32 Biometric Device',
    status: 'Online',
    ip: DEVICE_IP,
    freeHeap: sys.freeHeap || 0,
    uptime: sys.uptime || 0,
    lastSync: nowTs,
    lastSyncDate: todayIso,
    lastSyncFormatted: `${todayDisplay}, ${now.toLocaleTimeString()}`
  });
  console.log(`✓ Device status published: ONLINE (Last Sync: ${todayDisplay}, ${now.toLocaleTimeString()})`);

  // 3. Fetch Users
  console.log('\n--- 3. Extracting All Employee Profiles ---');
  const users = await httpGetJson(`http://${DEVICE_IP}/list-users`);
  console.log(`✓ Retrieved ${users.length} employee records from device flash!`);

  const empMap = {};
  const activeFpSlots = [];
  const todayPunches = [];

  for (const u of users) {
    const key = u.roll || u.uid;
    const isFp = u.uid && u.uid.endsWith('_f');
    let fpSlot = '';
    if (isFp) {
      fpSlot = u.uid.replace('_f', '');
      if (!isNaN(fpSlot)) {
        activeFpSlots.push(parseInt(fpSlot));
      }
    }

    const hasIn = (u.in && u.in !== '-');
    const hasOut = (u.out && u.out !== '-');

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

    // If punched today, add IN punch
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

    // If punched OUT today, add OUT punch
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

  await firebasePut('/employees.json', empMap);
  console.log(`✓ Synchronized ${Object.keys(empMap).length} employee profiles to chronosMIS Cloud!`);

  // 4. Fetch Full Monthly Punch History from ESP32 Flash Memory
  console.log('\n--- 4. Extracting Full History Punch Logs from ESP32 Flash ---');
  let historyRaw = '';
  try {
    historyRaw = await httpGetRaw(`http://${DEVICE_IP}/get-history?month=10&year=2026`);
  } catch(e) {
    console.log(`Notice on history: ${e.message}`);
  }

  const allPunches = [];
  let currentDate = '';
  const lines = historyRaw.split('\n');

  for (let line of lines) {
    line = line.trim();
    if (!line || line.startsWith('Time,')) continue;
    if (line.startsWith('#DATE:')) {
      currentDate = line.substring(6).trim();
    } else {
      const parts = line.split(',');
      if (parts.length >= 7) {
        const pDate = currentDate || '16-10-2026';
        const isoD = normalizeDate(pDate);
        allPunches.push({
          time: parts[0],
          roll: parts[1],
          name: parts[2],
          role: parts[3],
          dept: (empMap[parts[1]] && empMap[parts[1]].dept) ? empMap[parts[1]].dept : 'General',
          dir: parts[4] || 'In',
          status: parts[5] || 'On-Time',
          method: parts[6] || 'Fingerprint',
          date: isoD,
          dateRaw: pDate,
          dateFormatted: formatDateDisplay(isoD),
          device: 'ESP32 Biometric Device'
        });
      }
    }
  }

  // Add today's active punches
  for (const tp of todayPunches) {
    allPunches.push(tp);
  }

  // Sort descending by date and time
  allPunches.sort((a, b) => {
    const da = `${normalizeDate(a.date)} ${a.time}`;
    const db = `${normalizeDate(b.date)} ${b.time}`;
    return db.localeCompare(da);
  });

  console.log(`✓ Extracted ${allPunches.length} total punch records across ${[...new Set(allPunches.map(p => p.dateFormatted))].length} active dates!`);

  // Upload to Firebase
  const punchObject = {};
  allPunches.forEach((p, idx) => {
    const key = `PUNCH_${normalizeDate(p.date)}_${p.roll}_${p.dir}_${p.time.replace(/:/g, '')}_${idx}`;
    punchObject[key] = p;
  });

  await firebasePut('/punches.json', punchObject);
  console.log(`✓ Uploaded ${Object.keys(punchObject).length} normalized punch logs to chronosMIS Firebase!`);

  // 5. Extract Fingerprint Templates (if not already synced)
  console.log(`\n--- 5. Verifying R503 Optical Sensor Templates ---`);
  activeFpSlots.sort((a, b) => a - b);
  let tCount = 0;
  for (const slot of activeFpSlots) {
    try {
      const tData = await httpGetJson(`http://${DEVICE_IP}/api/get-fp-template?id=${slot}`);
      if (tData.status === 'ok' && tData.template && tData.template.length > 0) {
        const matchingEmp = users.find(u => u.uid === `${slot}_f`);
        await firebasePut(`/templates/${slot}.json`, {
          slot: slot,
          name: matchingEmp ? matchingEmp.name : '',
          roll: matchingEmp ? matchingEmp.roll : '',
          bytes: tData.bytes || 512,
          hex: tData.template,
          syncedAt: new Date().toISOString()
        });
        tCount++;
      }
    } catch(err){}
    await new Promise(r => setTimeout(r, 40));
  }
  console.log(`✓ Fingerprint Templates: ${tCount}/${activeFpSlots.length} confirmed in Cloud!`);

  console.log('\n====================================================');
  console.log(`🎉 ALL DATES & RECORDS SYNCHRONIZED PROPERLY!`);
  console.log(`📅 Dates Loaded: ${[...new Set(allPunches.map(p => p.dateFormatted))].join(', ')}`);
  console.log(`👥 Employees: ${Object.keys(empMap).length} | Total Punches: ${allPunches.length}`);
  console.log('====================================================');
}

run().catch(err => {
  console.error('\nSync error:', err);
  process.exit(1);
});
