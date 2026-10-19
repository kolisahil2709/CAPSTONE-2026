const fs = require('fs');
const path = require('path');

const targetFiles = ['index.html', 'cloud_erp.html'];

targetFiles.forEach(file => {
  const filePath = path.join(__dirname, file);
  if (!fs.existsSync(filePath)) {
    console.log('File not found:', file);
    return;
  }

  let html = fs.readFileSync(filePath, 'utf8');
  console.log(`\n================ Processing ${file} (${html.length} bytes) ================`);

  // 1. Ensure Top Bar Last Sync Pill exists
  if (!html.includes('id="last-sync-pill"')) {
    html = html.replace(
      '<div class="clock-pill" id="live-clock">--:--:-- --</div>',
      '<div class="clock-pill" id="live-clock">--:--:-- --</div>\n          <div class="clock-pill" id="last-sync-pill" style="display:inline-flex; align-items:center; gap:6px; font-size:12px;">\n            <span>☁️</span>\n            <span id="last-sync-text">Cloud Sync: Ready</span>\n          </div>'
    );
    console.log('✓ Added last-sync-pill to topbar');
  }

  // 2. Ensure Dashboard Recent Activity Table has Date Column in <thead>
  const oldDashHead = `                  <tr>
                    <th>Time</th>
                    <th>Roll ID</th>
                    <th>Name</th>
                    <th>Department</th>
                    <th>Direction</th>
                    <th>Status</th>
                    <th>Method</th>
                  </tr>`;
  
  const newDashHead = `                  <tr>
                    <th>Time</th>
                    <th>Date</th>
                    <th>Roll ID</th>
                    <th>Name</th>
                    <th>Department</th>
                    <th>Direction</th>
                    <th>Status</th>
                    <th>Method</th>
                  </tr>`;

  if (html.includes(oldDashHead)) {
    html = html.replace(oldDashHead, newDashHead);
    console.log('✓ Added Date column to Dashboard Recent Activity <thead>');
  }

  // Also update colspan from 7 to 8 in dashboard recent tbody
  html = html.replace(
    '<tbody id="dashboard-recent-punches-body">\n                  <tr><td colspan="7"',
    '<tbody id="dashboard-recent-punches-body">\n                  <tr><td colspan="8"'
  );
  html = html.replace(
    '<tbody id="dashboard-recent-punches-body"><tr><td colspan="7"',
    '<tbody id="dashboard-recent-punches-body"><tr><td colspan="8"'
  );

  // 3. Ensure Daily Attendance Table has Date Column in <thead> and Date Badge in Header
  const oldDailyHead = `                  <tr>
                    <th>Roll ID</th>
                    <th>Employee Name</th>
                    <th>Department</th>
                    <th>Shift</th>
                    <th>Punch IN</th>
                    <th>Punch OUT</th>
                    <th>Duration</th>
                    <th>Status</th>
                    <th>Method</th>
                  </tr>`;

  const newDailyHead = `                  <tr>
                    <th>Date</th>
                    <th>Roll ID</th>
                    <th>Employee Name</th>
                    <th>Department</th>
                    <th>Shift</th>
                    <th>Punch IN</th>
                    <th>Punch OUT</th>
                    <th>Duration</th>
                    <th>Status</th>
                    <th>Method</th>
                  </tr>`;

  if (html.includes(oldDailyHead)) {
    html = html.replace(oldDailyHead, newDailyHead);
    console.log('✓ Added Date column to Daily Attendance <thead>');
  }

  // Update colspan for Daily Attendance from 9 to 10
  html = html.replace(
    '<tbody id="daily-att-table-body">\n                  <tr><td colspan="9"',
    '<tbody id="daily-att-table-body">\n                  <tr><td colspan="10"'
  );
  html = html.replace(
    '<tbody id="daily-att-table-body"><tr><td colspan="9"',
    '<tbody id="daily-att-table-body"><tr><td colspan="10"'
  );

  // Add Date Badge & Quick Filter Container to Daily Attendance Card Header if not present
  if (!html.includes('id="daily-att-date-badge"')) {
    html = html.replace(
      '<h3 class="card-title">Daily Attendance Ledger</h3>',
      '<h3 class="card-title">Daily Attendance Ledger</h3>\n                  <div style="display:flex; align-items:center; gap:10px; margin-top:6px; flex-wrap:wrap;">\n                    <span class="badge badge-primary" id="daily-att-date-badge" style="font-size:12px; font-weight:700; padding:4px 10px; background:linear-gradient(135deg, var(--primary), var(--accent)); color:#fff; border:none;">📅 Selected: Today</span>\n                    <div id="daily-att-quick-pills" style="display:inline-flex; gap:6px; flex-wrap:wrap; align-items:center;"></div>\n                  </div>'
    );
    console.log('✓ Added daily-att-date-badge and quick pills to Daily Attendance card header');
  }

  // 4. Update Date Helper Functions
  const dateHelpersOld = `    function normalizeDateToIso(d) {
      if (!d) return '';
      d = String(d).trim();
      var m1 = d.match(/^(\\d{1,2})[-\\/](\\d{1,2})[-\\/](\\d{4})$/);
      if (m1) {
        var day = m1[1].padStart(2, '0');
        var month = m1[2].padStart(2, '0');
        var year = m1[3];
        return year + '-' + month + '-' + day;
      }
      var m2 = d.match(/^(\\d{4})[-\\/](\\d{1,2})[-\\/](\\d{1,2})$/);
      if (m2) {
        var year = m2[1];
        var month = m2[2].padStart(2, '0');
        var day = m2[3].padStart(2, '0');
        return year + '-' + month + '-' + day;
      }
      var m3 = d.match(/^(\\d{4})[-\\/](\\d{1,2})$/);
      if (m3) {
        return m3[1] + '-' + m3[2].padStart(2, '0');
      }
      try {
        var dt = new Date(d);
        if (!isNaN(dt.getTime())) {
          var y = dt.getFullYear();
          var mo = String(dt.getMonth() + 1).padStart(2, '0');
          var da = String(dt.getDate()).padStart(2, '0');
          return y + '-' + mo + '-' + da;
        }
      } catch(e){}
      return d;
    }

    function formatDisplayDate(d) {
      if (!d) return '-';
      var iso = normalizeDateToIso(d);
      var parts = iso.split('-');
      if (parts.length === 3) {
        return parts[2] + '-' + parts[1] + '-' + parts[0];
      }
      return d;
    }`;

  const dateHelpersStandard = `    function normalizeDate(d) {
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

    function normalizeDateToIso(d) {
      return normalizeDate(d);
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

    function formatDisplayDate(d) {
      return formatDateDisplay(d);
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
      return normalizeDate(isoDate);
    }`;

  if (html.includes(dateHelpersOld)) {
    html = html.replace(dateHelpersOld, dateHelpersStandard);
    console.log('✓ Replaced old date helpers with standard suite');
  } else if (!html.includes('function formatDateDisplay(')) {
    // Inject if missing
    html = html.replace('function startApp() {', dateHelpersStandard + '\n\n    function startApp() {');
    console.log('✓ Injected dateHelpersStandard before startApp()');
  }

  // 5. Update renderPunches to display Date in Dashboard Recent Activity & Audit Table
  const oldDashRow = `        if (idx < 5 && dashBody) {
          dashHtml += '<tr>' +
            '<td><code style="color:var(--text);">' + (p.time || '-') + '</code></td>' +
            '<td><code>' + (p.roll || '-') + '</code></td>' +
            '<td><b>' + (p.name || 'Unknown') + '</b></td>' +
            '<td><span class="badge" style="background:rgba(255,255,255,0.06);">' + (p.dept || 'General') + '</span></td>' +
            '<td>' + dirBadge + '</td>' +
            '<td>' + statusBadge + '</td>' +
            '<td><span style="font-size:11px;">' + methodIcon + '</span></td>' +
          '</tr>';
        }`;

  const newDashRow = `        if (idx < 8 && dashBody) {
          dashHtml += '<tr>' +
            '<td><code style="color:var(--text); font-weight:600;">' + (p.time || '-') + '</code></td>' +
            '<td><b style="color:var(--text); font-family:var(--font-mono); font-size:12px;">' + formatDateDisplay(p.date) + '</b></td>' +
            '<td><code>' + (p.roll || '-') + '</code></td>' +
            '<td><b>' + (p.name || 'Unknown') + '</b></td>' +
            '<td><span class="badge" style="background:rgba(255,255,255,0.06);">' + (p.dept || 'General') + '</span></td>' +
            '<td>' + dirBadge + '</td>' +
            '<td>' + statusBadge + '</td>' +
            '<td><span style="font-size:11px;">' + methodIcon + '</span></td>' +
          '</tr>';
        }`;

  if (html.includes(oldDashRow)) {
    html = html.replace(oldDashRow, newDashRow);
    console.log('✓ Updated dashHtml to include Date cell');
  }

  // Update Audit Table Date cell in renderPunches
  html = html.replace(
    "'<td><span style=\"font-size:12px; color:var(--text-dim);\">' + (p.date || '-') + '</span></td>' +",
    "'<td><b style=\"font-size:12px; color:var(--text); font-family:var(--font-mono); font-weight:600;\">' + formatDateDisplay(p.date) + '</b></td>' +"
  );

  // Update empty state colspan for dashboard recent
  html = html.replace(
    'if (dashBody) dashBody.innerHTML = \'<tr><td colspan="7" style="text-align:center; padding:20px; color:var(--text-muted);">No activity today yet.</td></tr>\';',
    'if (dashBody) dashBody.innerHTML = \'<tr><td colspan="8" style="text-align:center; padding:20px; color:var(--text-muted);">No activity recorded yet.</td></tr>\';'
  );

  // 6. Update renderDailyAttendance:
  // - Show date in row
  // - Update date badge and render quick selector pills
  const oldDailyRow = `        rowsHtml += '<tr>' +
          '<td><code>' + roll + '</code></td>' +
          '<td><b>' + name + '</b></td>' +
          '<td><span class="badge" style="background:rgba(255,255,255,0.06);">' + dept + '</span></td>' +
          '<td><span style="font-family:var(--font-mono); font-size:11px;">🕒 ' + shift + '</span></td>' +
          '<td><code style="color:var(--text);">' + inTime + '</code></td>' +
          '<td><code style="color:var(--text);">' + outTime + '</code></td>' +
          '<td><span style="font-family:var(--font-mono);">' + duration + '</span></td>' +
          '<td>' + statusBadge + '</td>' +
          '<td><span style="font-size:11px;">' + method + '</span></td>' +
        '</tr>';`;

  const newDailyRow = `        rowsHtml += '<tr>' +
          '<td><b style="font-family:var(--font-mono); font-size:12px; color:var(--text);">' + formatDateDisplay(targetDate) + '</b></td>' +
          '<td><code>' + roll + '</code></td>' +
          '<td><b>' + name + '</b></td>' +
          '<td><span class="badge" style="background:rgba(255,255,255,0.06);">' + dept + '</span></td>' +
          '<td><span style="font-family:var(--font-mono); font-size:11px;">🕒 ' + shift + '</span></td>' +
          '<td><code style="color:var(--text);">' + inTime + '</code></td>' +
          '<td><code style="color:var(--text);">' + outTime + '</code></td>' +
          '<td><span style="font-family:var(--font-mono);">' + duration + '</span></td>' +
          '<td>' + statusBadge + '</td>' +
          '<td><span style="font-size:11px;">' + method + '</span></td>' +
        '</tr>';`;

  if (html.includes(oldDailyRow)) {
    html = html.replace(oldDailyRow, newDailyRow);
    console.log('✓ Added Date cell to Daily Attendance rows');
  }

  // Update empty state in renderDailyAttendance
  html = html.replace(
    'rowsHtml = \'<tr><td colspan="9" style="text-align:center; padding:24px; color:var(--text-muted);">No records found matching filters for \' + targetDate + \'.</td></tr>\';',
    'rowsHtml = \'<tr><td colspan="10" style="text-align:center; padding:24px; color:var(--text-muted);">No records found matching filters for \' + formatDateDisplay(targetDate) + \'.</td></tr>\';'
  );
  html = html.replace(
    'rowsHtml = \'<tr><td colspan="9" style="text-align:center; padding:24px; color:var(--text-muted);">No records found matching filters for \' + formatDateDisplay(targetDate) + \'.</td></tr>\';',
    'rowsHtml = \'<tr><td colspan="10" style="text-align:center; padding:24px; color:var(--text-muted);">No records found matching filters for \' + formatDateDisplay(targetDate) + \'.</td></tr>\';'
  );

  // Update date badge and pills in renderDailyAttendance
  const oldDateMatchLogic = `      // Map punches for this date
      var datePunches = {};
      appState.punches.forEach(function(p){
        var pDate = normalizeDateToIso(p.date);
        if (pDate === targetDate) {
          var r = String(p.roll);
          if (!datePunches[r]) datePunches[r] = [];
          datePunches[r].push(p);
        }
      });`;

  const newDateMatchLogic = `      // Update Daily Attendance Date Badge & Active Quick-Pills
      var dateBadge = document.getElementById('daily-att-date-badge');
      if (dateBadge) {
        var isToday = isSameDate(targetDate, getTodayFormattedDate());
        dateBadge.innerText = '📅 Selected: ' + formatDateDisplay(targetDate) + (isToday ? ' (Today)' : '');
      }

      var pillsContainer = document.getElementById('daily-att-quick-pills');
      if (pillsContainer) {
        // Collect all unique active punch dates
        var activeDates = {};
        appState.punches.forEach(function(p){
          if (p.date) {
            var n = normalizeDate(p.date);
            activeDates[n] = (activeDates[n] || 0) + 1;
          }
        });
        var sortedDates = Object.keys(activeDates).sort().reverse();
        if (sortedDates.length > 0) {
          var phtml = '<span style="font-size:11px; color:var(--text-dim); margin-right:4px;">Quick Dates:</span>';
          sortedDates.slice(0, 8).forEach(function(dt){
            var isSel = isSameDate(dt, targetDate);
            var bg = isSel ? 'var(--primary)' : 'rgba(255,255,255,0.06)';
            var col = isSel ? '#fff' : 'var(--text-muted)';
            var border = isSel ? '1px solid var(--primary)' : '1px solid var(--border)';
            phtml += '<button type="button" onclick="setDailyDate(\\'' + dt + '\\')" style="padding:2px 8px; font-size:11px; font-family:var(--font-mono); border-radius:4px; background:' + bg + '; color:' + col + '; border:' + border + '; cursor:pointer; font-weight:' + (isSel ? '700' : '400') + ';">' + formatDateDisplay(dt) + ' (' + activeDates[dt] + ')</button>';
          });
          pillsContainer.innerHTML = phtml;
        }
      }

      // Map punches for this date
      var datePunches = {};
      appState.punches.forEach(function(p){
        if (isSameDate(p.date, targetDate)) {
          var r = String(p.roll);
          if (!datePunches[r]) datePunches[r] = [];
          datePunches[r].push(p);
        }
      });`;

  if (html.includes(oldDateMatchLogic)) {
    html = html.replace(oldDateMatchLogic, newDateMatchLogic);
    console.log('✓ Injected date badge and quick date pills into renderDailyAttendance');
  }

  // Ensure setDailyDate helper exists
  if (!html.includes('function setDailyDate(')) {
    const setDailyDateHelper = `    function setDailyDate(dt) {
      var picker = document.getElementById('daily-att-date-picker');
      if (picker) {
        picker.value = normalizeDate(dt);
        renderDailyAttendance();
      }
    }\n`;
    html = html.replace('function renderDailyAttendance() {', setDailyDateHelper + '    function renderDailyAttendance() {');
    console.log('✓ Added setDailyDate helper function');
  }

  // 7. Update updateDeviceStatus to show formatted Last Sync Date
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

  if (html.includes(oldUpdateDev)) {
    html = html.replace(oldUpdateDev, newUpdateDev);
    console.log('✓ Updated updateDeviceStatus with Last Sync time display');
  }

  // 8. Update renderEmployees to display Last Punch Date cleanly
  html = html.replace(
    `          '<td><code style="color:var(--success);">' + (emp.firstIn || '-') + '</code></td>' +
          '<td><code style="color:var(--primary);">' + (emp.lastOut || '-') + '</code></td>' +`,
    `          '<td><code style="color:var(--success);">' + (emp.firstIn || '-') + '</code>' + (emp.lastDateFormatted || emp.lastDate ? ('<br><span style="font-size:10px; color:var(--text-dim); font-family:var(--font-mono);">' + (emp.lastDateFormatted || formatDateDisplay(emp.lastDate)) + '</span>') : '') + '</td>' +
          '<td><code style="color:var(--primary);">' + (emp.lastOut || '-') + '</code>' + (emp.lastDateFormatted || emp.lastDate ? ('<br><span style="font-size:10px; color:var(--text-dim); font-family:var(--font-mono);">' + (emp.lastDateFormatted || formatDateDisplay(emp.lastDate)) + '</span>') : '') + '</td>' +`
  );

  // 9. Update viewEmployeeDossier Recent Punches date formatting
  html = html.replace(
    "'<td>' + p.date + '</td>' +",
    "'<td><b style=\"font-family:var(--font-mono); font-size:12px; font-weight:600; color:var(--text);\">' + formatDateDisplay(p.date) + '</b></td>' +"
  );

  // 10. Update Monthly Attendance date formatting
  html = html.replace(
    "'<td><code>' + dateStr + '</code></td>' +",
    "'<td><b style=\"font-family:var(--font-mono); color:var(--text);\">' + formatDateDisplay(dateStr) + '</b></td>' +"
  );

  // 11. Update fetchFirebaseData & processPunches to re-render active view
  const oldProcessPunchesEnd = `      appState.punches = arr;
      renderPunches();
      updateKpis();
    }`;

  const newProcessPunchesEnd = `      appState.punches = arr;
      renderPunches();
      updateKpis();
      
      // Keep active view refreshed immediately
      var activeView = document.querySelector('.content-view.active');
      if (activeView) {
        var vId = activeView.id.replace('view-', '');
        if (vId === 'att-daily') renderDailyAttendance();
        if (vId === 'att-monthly') renderMonthlyAttendance();
        if (vId === 'att-monthly-all') renderMonthlyAttendanceAll();
        if (vId === 'employees') renderEmployees();
      }
    }`;

  if (html.includes(oldProcessPunchesEnd)) {
    html = html.replace(oldProcessPunchesEnd, newProcessPunchesEnd);
    console.log('✓ Added auto-refresh of active view in processPunches');
  }

  // 12. Robust startDeviceCloudTransfer:
  // Pull /list-users, pull /get-history directly with all dates!
  const oldTransferStep3And5 = `      // STEP 3: Extract employee profiles, shifts & RFID cards
      var emps = transferData.employees || [];
      setStepStatus(3, 'active', 'Step 3: Extracting employee records (' + emps.length + ' found)...');
      document.getElementById('transfer-progress-bar').style.width = '45%';
      logTransfer("👥 Found <b>" + emps.length + "</b> employee records & shift schedules in device flash", "info");

      var authParam = appState.firebaseAuth ? ('?auth=' + encodeURIComponent(appState.firebaseAuth)) : '';
      var empMap = {};
      emps.forEach(function(e) {
        var key = e.roll || ('EMP_' + e.fingerId);
        empMap[key] = {
          roll: e.roll || key,
          name: e.name || 'Staff ' + key,
          role: e.role || '-',
          dept: e.dept || 'General',
          dob: e.dob || '',
          rfid: e.rfid || '',
          fingerId: e.fingerId || '',
          shiftStart: (e.shift && e.shift.start) ? e.shift.start : '09:00',
          shiftEnd: (e.shift && e.shift.end) ? e.shift.end : '17:00',
          shiftGrace: (e.shift && e.shift.grace) ? e.shift.grace : 15,
          state: '0',
          firstIn: '-',
          lastOut: '-'
        };
      });

      try {
        await fetch(fbUrl + '/employees.json' + authParam, {
          method: 'PUT',
          headers: { 'Content-Type': 'application/json' },
          body: JSON.stringify(empMap)
        });
        logTransfer("☁️ <b>" + Object.keys(empMap).length + "</b> employee profiles uploaded to chronosMIS Firebase Cloud", "success");
      } catch(e) {
        logTransfer("⚠️ Notice on employee upload: " + e.message, "warn");
      }
      setStepStatus(3, 'completed', 'Step 3: Extracted & synced ' + emps.length + ' employee profiles');`;

  const newTransferStep3 = `      // STEP 3: Extract employee profiles, shifts & active punch states
      var emps = transferData.employees || [];
      var liveUsers = [];
      try {
        var uResp = await fetch('http://' + ip + '/list-users', { mode: 'cors' });
        if (uResp.ok) {
          liveUsers = await uResp.json();
          logTransfer("👥 Retrieved <b>" + liveUsers.length + "</b> live user profiles from device", "info");
        }
      } catch(ue){}

      if (liveUsers.length > 0 && emps.length === 0) {
        emps = liveUsers;
      }

      setStepStatus(3, 'active', 'Step 3: Extracting employee records (' + emps.length + ' found)...');
      document.getElementById('transfer-progress-bar').style.width = '45%';

      var authParam = appState.firebaseAuth ? ('?auth=' + encodeURIComponent(appState.firebaseAuth)) : '';
      var empMap = {};
      var todayIso = getTodayFormattedDate();
      var todayDisp = formatDateDisplay(todayIso);

      emps.forEach(function(e) {
        var key = e.roll || ('EMP_' + (e.fingerId || e.uid));
        var matchingLive = liveUsers.find(function(u){ return String(u.roll) === String(e.roll); });
        var hasIn = (matchingLive && matchingLive.in && matchingLive.in !== '-') ? matchingLive.in : ((e.in && e.in !== '-') ? e.in : (e.firstIn && e.firstIn !== '-' ? e.firstIn : null));
        var hasOut = (matchingLive && matchingLive.out && matchingLive.out !== '-') ? matchingLive.out : ((e.out && e.out !== '-') ? e.out : (e.lastOut && e.lastOut !== '-' ? e.lastOut : null));
        var state = (matchingLive && matchingLive.state) ? matchingLive.state : (e.state || (hasIn ? '1' : '0'));

        empMap[key] = {
          roll: e.roll || key,
          name: e.name || (matchingLive ? matchingLive.name : ('Staff ' + key)),
          role: e.role || (matchingLive ? matchingLive.role : '-'),
          dept: (e.dept && e.dept !== '-') ? e.dept : ((matchingLive && matchingLive.dept !== '-') ? matchingLive.dept : 'General'),
          dob: e.dob || '',
          rfid: e.rfid || '',
          fingerId: e.fingerId || (e.uid ? String(e.uid).replace('_f','') : ''),
          shiftStart: (e.shift && e.shift.start) ? e.shift.start : (e.shiftStart || '09:00'),
          shiftEnd: (e.shift && e.shift.end) ? e.shift.end : (e.shiftEnd || '17:00'),
          shiftGrace: (e.shift && e.shift.grace) ? e.shift.grace : (e.shiftGrace || 15),
          state: state,
          firstIn: hasIn || '-',
          lastOut: hasOut || '-',
          lastDate: hasIn ? todayIso : (e.lastDate || ''),
          lastDateFormatted: hasIn ? todayDisp : (e.lastDateFormatted || '-'),
          lastSync: Math.floor(Date.now() / 1000)
        };
      });

      try {
        await fetch(fbUrl + '/employees.json' + authParam, {
          method: 'PUT',
          headers: { 'Content-Type': 'application/json' },
          body: JSON.stringify(empMap)
        });
        logTransfer("☁️ <b>" + Object.keys(empMap).length + "</b> employee profiles uploaded to chronosMIS Firebase Cloud", "success");
      } catch(e) {
        logTransfer("⚠️ Notice on employee upload: " + e.message, "warn");
      }
      setStepStatus(3, 'completed', 'Step 3: Extracted & synced ' + Object.keys(empMap).length + ' employee profiles');`;

  if (html.includes(oldTransferStep3And5)) {
    html = html.replace(oldTransferStep3And5, newTransferStep3);
    console.log('✓ Upgraded startDeviceCloudTransfer Step 3');
  }

  // Upgrade Step 5 to extract full historical punch log with all dates
  const oldStep5 = `      // STEP 5: Finalize records in cloud
      setStepStatus(5, 'active', 'Step 5: Finalizing cloud records & punch logs...');
      document.getElementById('transfer-progress-bar').style.width = '90%';

      var punchesToUpload = [];
      if (Array.isArray(transferData.punches) && transferData.punches.length > 0) {
        punchesToUpload = transferData.punches;
      } else if (Array.isArray(transferData.todayLogs) && transferData.todayLogs.length > 0) {
        var tDate = normalizeDateToIso(transferData.todayDate || getTodayFormattedDate());
        transferData.todayLogs.forEach(function(line) {
          if (!line || line.startsWith('Time,') || (line.indexOf(',') === -1 && line.indexOf('|') === -1)) return;
          var parts = line.indexOf(',') !== -1 ? line.split(',') : line.split('|');
          if (parts.length >= 2) {
            punchesToUpload.push({
              time: parts[0] ? parts[0].trim() : '',
              roll: parts[1] ? parts[1].trim() : '',
              name: parts[2] ? parts[2].trim() : '',
              role: parts[3] ? parts[3].trim() : '-',
              dir: parts[4] ? parts[4].trim() : 'In',
              status: parts[5] ? parts[5].trim() : 'Accepted',
              method: parts[6] ? parts[6].trim() : 'Fingerprint',
              date: tDate,
              device: devName
            });
          }
        });
      }`;

  const newStep5 = `      // STEP 5: Extract full multi-date attendance punch logs from device flash
      setStepStatus(5, 'active', 'Step 5: Extracting verified dates & punch history from device...');
      document.getElementById('transfer-progress-bar').style.width = '90%';

      var punchesToUpload = [];
      if (Array.isArray(transferData.punches) && transferData.punches.length > 0) {
        punchesToUpload = transferData.punches;
        logTransfer("📋 Loaded " + punchesToUpload.length + " punches from device gateway transfer buffer", "info");
      }

      // Also fetch full monthly historical punch log directly from device flash memory
      try {
        var nowD = new Date();
        var hMonth = nowD.getMonth() + 1;
        var hYear = nowD.getFullYear();
        var hResp = await fetch('http://' + ip + '/get-history?month=' + hMonth + '&year=' + hYear, { mode: 'cors' });
        if (hResp.ok) {
          var hText = await hResp.text();
          var hLines = hText.split('\\n');
          var curDate = getTodayFormattedDate();
          var parsedCount = 0;

          hLines.forEach(function(hline){
            hline = hline.trim();
            if (!hline || hline.startsWith('Time,')) return;
            if (hline.startsWith('#DATE:')) {
              curDate = normalizeDate(hline.substring(6).trim());
            } else {
              var pParts = hline.split(',');
              if (pParts.length >= 6) {
                var pDate = curDate || getTodayFormattedDate();
                punchesToUpload.push({
                  time: pParts[0].trim(),
                  roll: pParts[1].trim(),
                  name: pParts[2] ? pParts[2].trim() : '',
                  role: pParts[3] ? pParts[3].trim() : 'Staff',
                  dept: (empMap[pParts[1].trim()] && empMap[pParts[1].trim()].dept) ? empMap[pParts[1].trim()].dept : 'General',
                  dir: pParts[4] ? pParts[4].trim() : 'In',
                  status: pParts[5] ? pParts[5].trim() : 'On-Time',
                  method: pParts[6] ? pParts[6].trim() : 'Fingerprint',
                  date: pDate,
                  dateFormatted: formatDateDisplay(pDate),
                  device: devName
                });
                parsedCount++;
              }
            }
          });
          if (parsedCount > 0) {
            logTransfer("📚 Extracted <b>" + parsedCount + "</b> historical attendance punches across verified dates from device flash memory!", "success");
          }
        }
      } catch(he){
        logTransfer("Notice on history fetch: " + he.message, "info");
      }

      // Add today active check-ins if available
      Object.keys(empMap).forEach(function(k){
        var e = empMap[k];
        if (e.firstIn && e.firstIn !== '-') {
          var exists = punchesToUpload.some(function(p){ return String(p.roll) === String(e.roll) && isSameDate(p.date, todayIso) && p.dir === 'In'; });
          if (!exists) {
            punchesToUpload.push({
              time: e.firstIn,
              roll: e.roll,
              name: e.name,
              dept: e.dept,
              dir: 'In',
              status: 'On-Time',
              method: e.fingerId ? 'Fingerprint' : 'RFID Card',
              date: todayIso,
              dateFormatted: todayDisp,
              device: devName
            });
          }
        }
        if (e.lastOut && e.lastOut !== '-' && e.lastOut !== e.firstIn) {
          var existsOut = punchesToUpload.some(function(p){ return String(p.roll) === String(e.roll) && isSameDate(p.date, todayIso) && p.dir === 'Out'; });
          if (!existsOut) {
            punchesToUpload.push({
              time: e.lastOut,
              roll: e.roll,
              name: e.name,
              dept: e.dept,
              dir: 'Out',
              status: 'On-Time',
              method: e.fingerId ? 'Fingerprint' : 'RFID Card',
              date: todayIso,
              dateFormatted: todayDisp,
              device: devName
            });
          }
        }
      });`;

  if (html.includes(oldStep5)) {
    html = html.replace(oldStep5, newStep5);
    console.log('✓ Upgraded startDeviceCloudTransfer Step 5 with full multi-date history extraction');
  }

  fs.writeFileSync(filePath, html, 'utf8');
  console.log(`✓ Saved ${file} (${html.length} bytes) successfully!`);
});
