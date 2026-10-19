const fs = require('fs');
const path = require('path');

const filesToUpdate = ['index.html', 'cloud_erp.html'];

filesToUpdate.forEach(fileName => {
  const filePath = path.join(__dirname, fileName);
  if (!fs.existsSync(filePath)) return;

  let content = fs.readFileSync(filePath, 'utf8');

  // 1. Add topbar last-sync-pill if not already there
  if (content.indexOf('id="last-sync-pill"') === -1) {
    content = content.replace(
      '<div class="clock-pill" id="live-clock">--:--:-- --</div>',
      '<div class="clock-pill" id="live-clock">--:--:-- --</div>\n          <div class="clock-pill" id="last-sync-pill" style="display:inline-flex; align-items:center; gap:6px; font-size:12px;">\n            <span>☁️</span>\n            <span id="last-sync-text">Cloud Sync: Ready</span>\n          </div>'
    );
  }

  // 2. Replace formatAltDate with full suite of robust date normalizers
  const oldFormatAltDate = `    function formatAltDate(isoDate) {
      if (!isoDate) return '';
      var parts = isoDate.split('-');
      if (parts.length === 3) return parts[2] + '/' + parts[1] + '/' + parts[0];
      return isoDate;
    }`;

  const newDateHelpers = `    function normalizeDate(d) {
      if (!d) return '';
      d = String(d).trim();
      if (d.indexOf('T') !== -1) d = d.split('T')[0];
      d = d.replace(/[\\/\\.]/g, '-');
      var parts = d.split('-');
      if (parts.length === 3) {
        if (parts[0].length === 4) {
          // YYYY-MM-DD
          return parts[0] + '-' + parts[1].padStart(2, '0') + '-' + parts[2].padStart(2, '0');
        } else if (parts[2].length === 4) {
          // DD-MM-YYYY -> YYYY-MM-DD
          return parts[2] + '-' + parts[1].padStart(2, '0') + '-' + parts[0].padStart(2, '0');
        }
      }
      return d;
    }

    function isSameDate(d1, d2) {
      if (!d1 || !d2) return false;
      return normalizeDate(d1) === normalizeDate(d2);
    }

    function formatDateDisplay(d) {
      if (!d || d === '-' || d === '00-00-0000') return '-';
      var norm = normalizeDate(d);
      var parts = norm.split('-');
      if (parts.length === 3 && parts[0].length === 4) {
        var months = ['Jan','Feb','Mar','Apr','May','Jun','Jul','Aug','Sep','Oct','Nov','Dec'];
        var mIdx = parseInt(parts[1], 10) - 1;
        var day = parseInt(parts[2], 10);
        var monthName = months[mIdx] || parts[1];
        return (day < 10 ? '0' + day : day) + ' ' + monthName + ' ' + parts[0];
      }
      return d;
    }

    function formatTimestampDisplay(ts) {
      if (!ts) return '-';
      var ms = ts;
      if (typeof ts === 'number' && ts < 10000000000) ms = ts * 1000;
      else if (!isNaN(ts) && Number(ts) < 10000000000) ms = Number(ts) * 1000;
      var dateObj = new Date(ms);
      if (isNaN(dateObj.getTime())) return String(ts);
      return dateObj.toLocaleDateString('en-GB', { day: '2-digit', month: 'short', year: 'numeric' }) + ', ' +
             dateObj.toLocaleTimeString('en-US', { hour: '2-digit', minute: '2-digit', second: '2-digit' });
    }

    function formatAltDate(isoDate) {
      if (!isoDate) return '';
      return normalizeDate(isoDate);
    }`;

  if (content.indexOf(oldFormatAltDate) !== -1) {
    content = content.replace(oldFormatAltDate, newDateHelpers);
  }

  // 3. Update renderDailyAttendance punch matching
  content = content.replace(
    'if (p.date === targetDate || p.date === formatAltDate(targetDate)) {',
    'if (isSameDate(p.date, targetDate)) {'
  );

  content = content.replace(
    '} else if (targetDate === getTodayFormattedDate() && emp.state && emp.state !== "0" && emp.firstIn && emp.firstIn !== "-") {',
    '} else if (isSameDate(targetDate, getTodayFormattedDate()) && emp.state && emp.state !== "0" && emp.firstIn && emp.firstIn !== "-") {'
  );

  content = content.replace(
    'No records found matching filters for \' + targetDate + \'.',
    'No records found matching filters for \' + formatDateDisplay(targetDate) + \'.'
  );

  // 4. Update renderMonthlyAttendance month & day matching
  content = content.replace(
    `      // Filter punches for this roll and month
      var empPunches = {};
      appState.punches.forEach(function(p){
        if (String(p.roll) === String(roll) && p.date && p.date.startsWith(monthVal)) {
          var dNum = parseInt(p.date.split('-')[2]);
          if (!empPunches[dNum]) empPunches[dNum] = [];
          empPunches[dNum].push(p);
        }
      });`,
    `      // Filter punches for this roll and month
      var empPunches = {};
      appState.punches.forEach(function(p){
        if (String(p.roll) === String(roll) && p.date) {
          var normPDate = normalizeDate(p.date);
          if (normPDate.startsWith(monthVal)) {
            var dNum = parseInt(normPDate.split('-')[2], 10);
            if (!empPunches[dNum]) empPunches[dNum] = [];
            empPunches[dNum].push(p);
          }
        }
      });`
  );

  content = content.replace(
    'if (!isPresent && dateStr === getTodayFormattedDate() && emp.state && emp.state !== "0" && emp.firstIn && emp.firstIn !== "-") {',
    'if (!isPresent && isSameDate(dateStr, getTodayFormattedDate()) && emp.state && emp.state !== "0" && emp.firstIn && emp.firstIn !== "-") {'
  );

  content = content.replace(
    '\'<td><code>\' + dateStr + \'</code></td>\' +',
    '\'<td><b style="font-family:var(--font-mono); color:var(--text);">\' + formatDateDisplay(dateStr) + \'</b></td>\' +'
  );

  // 5. Update calculateAndRenderPayroll (Muster roll)
  content = content.replace(
    `          // Check punches
          var hasPunch = appState.punches.some(function(p){
            return String(p.roll) === String(roll) && (p.date === dateStr || p.date === formatAltDate(dateStr));
          });

          if (!hasPunch && dateStr === getTodayFormattedDate() && emp.state && emp.state !== "0" && emp.firstIn && emp.firstIn !== "-") {
            hasPunch = true;
          }`,
    `          // Check punches
          var hasPunch = appState.punches.some(function(p){
            return String(p.roll) === String(roll) && isSameDate(p.date, dateStr);
          });

          if (!hasPunch && isSameDate(dateStr, getTodayFormattedDate()) && emp.state && emp.state !== "0" && emp.firstIn && emp.firstIn !== "-") {
            hasPunch = true;
          }`
  );

  // 6. Update renderEmployees to display Last Punch Date cleanly
  content = content.replace(
    `          '<td><code style="color:var(--success);">' + (emp.firstIn || '-') + '</code></td>' +
          '<td><code style="color:var(--primary);">' + (emp.lastOut || '-') + '</code></td>' +`,
    `          '<td><code style="color:var(--success);">' + (emp.firstIn || '-') + '</code>' + (emp.lastDateFormatted || emp.lastDate ? ('<br><span style="font-size:10px; color:var(--text-dim);">' + (emp.lastDateFormatted || formatDateDisplay(emp.lastDate)) + '</span>') : '') + '</td>' +
          '<td><code style="color:var(--primary);">' + (emp.lastOut || '-') + '</code>' + (emp.lastDateFormatted || emp.lastDate ? ('<br><span style="font-size:10px; color:var(--text-dim);">' + (emp.lastDateFormatted || formatDateDisplay(emp.lastDate)) + '</span>') : '') + '</td>' +`
  );

  // 7. Update renderPunches date display
  content = content.replace(
    '\'<td><span style="font-size:12px; color:var(--text-dim);">\' + (p.date || \'-\') + \'</span></td>\' +',
    '\'<td><b style="font-size:12px; color:var(--text); font-family:var(--font-mono); font-weight:600;">\' + formatDateDisplay(p.date) + \'</b></td>\' +'
  );

  // 8. Update viewEmployeeDossier date display
  content = content.replace(
    '\'<td>\' + p.date + \'</td>\' +',
    '\'<td><span style="font-family:var(--font-mono); font-size:12px; font-weight:600;">\' + formatDateDisplay(p.date) + \'</span></td>\' +'
  );

  // 9. Update updateDeviceStatus
  const oldUpdateDev = `    function updateDeviceStatus() {
      var dev = appState.deviceInfo;
      if (!dev) return;
      var qIp = document.getElementById('quick-device-ip-display');
      if (qIp && dev.ip) qIp.innerText = dev.ip;
      var gwPill = document.getElementById('gateway-status-text');
      if (gwPill && dev.name) gwPill.innerText = 'Connected: ' + dev.name;
    }`;

  const newUpdateDev = `    function updateDeviceStatus() {
      var dev = appState.deviceInfo;
      if (!dev) return;
      var qIp = document.getElementById('quick-device-ip-display');
      if (qIp && dev.ip) qIp.innerText = dev.ip;
      var gwPill = document.getElementById('gateway-status-text');
      var statusDot = document.querySelector('#gateway-status-pill .status-dot');
      var syncTime = dev.lastSyncFormatted || (dev.lastSync ? formatTimestampDisplay(dev.lastSync) : 'Just now');
      if (gwPill) {
        gwPill.innerHTML = '<b>' + (dev.status || 'Online') + '</b>: ' + (dev.name || dev.deviceName || 'ESP32') + ' · <span style="color:var(--text-muted); font-size:11px;">Last Sync: ' + syncTime + '</span>';
        if (statusDot) statusDot.style.background = 'var(--success)';
      }
      var lastSyncEl = document.getElementById('last-sync-text');
      if (lastSyncEl) {
        lastSyncEl.innerText = 'Last Sync: ' + syncTime;
      }
    }`;

  if (content.indexOf(oldUpdateDev) !== -1) {
    content = content.replace(oldUpdateDev, newUpdateDev);
  }

  // 10. In startDeviceCloudTransfer: save lastDate on employees and sync full monthly history
  const oldEmpExtract = `      emps.forEach(function(e) {
        var key = e.roll || ('EMP_' + e.fingerId);
        empMap[key] = {
          roll: e.roll || key,
          name: e.name || 'Staff',
          role: e.role || 'Staff',
          dept: e.dept || 'General',
          dob: e.dob || '',
          rfid: e.rfid || '',
          fingerId: e.fingerId || '',
          templateStatus: (e.fingerId ? 'Enrolled (Slot #' + e.fingerId + ')' : 'None'),
          shiftId: (e.shift ? e.shift.id : 0),
          shiftStart: (e.shift ? e.shift.start : '09:00'),
          shiftEnd: (e.shift ? e.shift.end : '17:00'),
          shiftGrace: (e.shift ? e.shift.grace : 15),
          state: '0',
          firstIn: '-',
          lastOut: '-',
          lastSync: new Date().toISOString()
        };
      });`;

  const newEmpExtract = `      var now = new Date();
      var todayIso = getTodayFormattedDate();
      var todayDisp = formatDateDisplay(todayIso);
      emps.forEach(function(e) {
        var key = e.roll || ('EMP_' + e.fingerId);
        var hasIn = (e.in && e.in !== '-');
        var hasOut = (e.out && e.out !== '-');
        empMap[key] = {
          roll: e.roll || key,
          name: e.name || 'Staff',
          role: e.role || 'Staff',
          dept: e.dept || 'General',
          dob: e.dob || '',
          rfid: e.rfid || '',
          fingerId: e.fingerId || '',
          templateStatus: (e.fingerId ? 'Enrolled (Slot #' + e.fingerId + ')' : 'None'),
          shiftId: (e.shift ? e.shift.id : (e.shiftId || 0)),
          shiftStart: (e.shift ? e.shift.start : (e.shiftStart || '09:00')),
          shiftEnd: (e.shift ? e.shift.end : (e.shiftEnd || '17:00')),
          shiftGrace: (e.shift ? e.shift.grace : (e.shiftGrace || 15)),
          state: e.state || (hasIn ? '1' : '0'),
          firstIn: hasIn ? e.in : (e.firstIn || '-'),
          lastOut: hasOut ? e.out : (e.lastOut || '-'),
          lastDate: hasIn ? todayIso : (e.lastDate || ''),
          lastDateFormatted: hasIn ? todayDisp : (e.lastDateFormatted || '-'),
          lastSync: Math.floor(Date.now() / 1000)
        };
      });`;

  if (content.indexOf(oldEmpExtract) !== -1) {
    content = content.replace(oldEmpExtract, newEmpExtract);
  }

  fs.writeFileSync(filePath, content, 'utf8');
  console.log('✓ Successfully updated', fileName);
});
