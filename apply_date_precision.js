const fs = require('fs');
const path = require('path');

const targetFiles = ['index.html', 'cloud_erp.html'];

targetFiles.forEach(fileName => {
  const filePath = path.join(__dirname, fileName);
  if (!fs.existsSync(filePath)) {
    console.log('Skipping missing file:', fileName);
    return;
  }

  let c = fs.readFileSync(filePath, 'utf8');
  console.log(`\n================ Updating ${fileName} (${c.length} bytes) ================`);

  // 1. Dashboard Recent Activity <thead>: Add Date column
  if (!c.includes('<th>Time</th>\r\n                    <th>Date</th>') && !c.includes('<th>Time</th>\n                    <th>Date</th>')) {
    c = c.replace(
      /(<th>Time<\/th>\s*)(<th>Roll ID<\/th>)/,
      '$1<th>Date</th>\n                    $2'
    );
    console.log('✓ Added <th>Date</th> to Dashboard Recent Activity <thead>');
  }

  // 2. Dashboard Recent Activity tbody colspan: 7 -> 8
  c = c.replace(
    /(<tbody id="dashboard-recent-punches-body">\s*<tr><td colspan=")7(")/g,
    '$18$2'
  );
  console.log('✓ Updated dashboard-recent-punches-body colspan to 8');

  // 3. Daily Attendance <thead>: Add Date column
  const dailyHeadRegex = /(<section id="view-att-daily"[\s\S]*?<thead>\s*<tr>\s*)(<th>Roll ID<\/th>)/;
  if (dailyHeadRegex.test(c)) {
    c = c.replace(dailyHeadRegex, '$1<th>Date</th>\n                    $2');
    console.log('✓ Added <th>Date</th> to Daily Attendance <thead>');
  }

  // 4. Daily Attendance tbody colspan: 9 -> 10
  c = c.replace(
    /(<tbody id="daily-att-table-body">\s*<tr><td colspan=")9(")/g,
    '$110$2'
  );
  console.log('✓ Updated daily-att-table-body colspan to 10');

  // 5. Update renderPunches dashHtml to render Date cell
  const dashRowRegex = /if\s*\(\s*idx\s*<\s*5\s*&&\s*dashBody\s*\)\s*\{[\s\S]*?dashHtml\s*\+=\s*'<tr>'\s*\+\s*'<td><code style="color:var\(--text\);">' \+ \(p\.time \|\| '-'\) \+ '<\/code><\/td>'\s*\+/;
  if (dashRowRegex.test(c)) {
    c = c.replace(
      dashRowRegex,
      "if (idx < 8 && dashBody) {\n          dashHtml += '<tr>' +\n            '<td><code style=\"color:var(--text); font-weight:600;\">' + (p.time || '-') + '</code></td>' +\n            '<td><b style=\"color:var(--text); font-family:var(--font-mono); font-size:12px;\">' + formatDateDisplay(p.date) + '</b></td>' +"
    );
    console.log('✓ Updated dashHtml to include Date cell in rows');
  }

  // Update empty state in renderPunches for dashBody: colspan 7 -> 8
  c = c.replace(
    /if\s*\(dashBody\)\s*dashBody\.innerHTML\s*=\s*'<tr><td colspan="7" style="text-align:center; padding:20px; color:var\(--text-muted\);">No activity today yet\.<\/td><\/tr>';/,
    "if (dashBody) dashBody.innerHTML = '<tr><td colspan=\"8\" style=\"text-align:center; padding:20px; color:var(--text-muted);\">No activity recorded yet.</td></tr>';"
  );

  // 6. Update renderDailyAttendance to render Date cell in rows
  const dailyRowRegex = /(rowsHtml\s*\+=\s*'<tr>'\s*\+\s*)(['"]<td><code>['"]\s*\+\s*roll\s*\+\s*['"]<\/code><\/td>['"])/;
  if (dailyRowRegex.test(c)) {
    c = c.replace(
      dailyRowRegex,
      "$1'<td><b style=\"font-family:var(--font-mono); font-size:12px; color:var(--text);\">' + formatDateDisplay(targetDate) + '</b></td>' +\n          $2"
    );
    console.log('✓ Added Date cell to Daily Attendance rows');
  }

  // Update empty state in renderDailyAttendance: colspan 9 -> 10 and formatDateDisplay(targetDate)
  c = c.replace(
    /rowsHtml\s*=\s*'<tr><td colspan="9" style="text-align:center; padding:24px; color:var\(--text-muted\);">No records found matching filters for ' \+ targetDate \+ '\.<\/td><\/tr>';/,
    "rowsHtml = '<tr><td colspan=\"10\" style=\"text-align:center; padding:24px; color:var(--text-muted);\">No records found matching filters for ' + formatDateDisplay(targetDate) + '.</td></tr>';"
  );
  c = c.replace(
    /rowsHtml\s*=\s*'<tr><td colspan="9" style="text-align:center; padding:24px; color:var\(--text-muted\);">No records found matching filters for ' \+ formatDateDisplay\(targetDate\) \+ '\.<\/td><\/tr>';/,
    "rowsHtml = '<tr><td colspan=\"10\" style=\"text-align:center; padding:24px; color:var(--text-muted);\">No records found matching filters for ' + formatDateDisplay(targetDate) + '.</td></tr>';"
  );

  // 7. Inject Date Badge & Quick Selector Pills in renderDailyAttendance
  const datePillsCode = `      // Update Daily Attendance Date Badge & Active Quick-Pills
      var dateBadge = document.getElementById('daily-att-date-badge');
      if (dateBadge) {
        var isToday = isSameDate(targetDate, getTodayFormattedDate());
        dateBadge.innerText = '📅 Selected: ' + formatDateDisplay(targetDate) + (isToday ? ' (Today)' : '');
      }

      var pillsContainer = document.getElementById('daily-att-quick-pills');
      if (pillsContainer) {
        var activeDates = {};
        appState.punches.forEach(function(p){
          if (p.date) {
            var n = normalizeDate(p.date);
            activeDates[n] = (activeDates[n] || 0) + 1;
          }
        });
        var sortedDates = Object.keys(activeDates).sort().reverse();
        if (sortedDates.length > 0) {
          var phtml = '<span style="font-size:11px; color:var(--text-dim); margin-right:4px;">Active Dates:</span>';
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

      // Map punches for this date`;

  if (!c.includes('daily-att-date-badge') || !c.includes('daily-att-quick-pills')) {
    c = c.replace(
      /\/\/\s*Map punches for this date/,
      datePillsCode
    );
    console.log('✓ Injected date badge and quick pills into renderDailyAttendance');
  }

  // 8. Update renderEmployees First In / Last Out to show Last Punch Date
  const empDateCellRegex = /('<td><code style="color:var\(--success\);">' \+ \(emp\.firstIn \|\| '-'\) \+ '<\/code>)(<\/td>')\s*\+\s*('<td><code style="color:var\(--primary\);">' \+ \(emp\.lastOut \|\| '-'\) \+ '<\/code>)(<\/td>')/;
  if (empDateCellRegex.test(c)) {
    c = c.replace(
      empDateCellRegex,
      "$1' + (emp.lastDateFormatted || emp.lastDate ? ('<br><span style=\"font-size:10px; color:var(--text-dim); font-family:var(--font-mono);\">' + (emp.lastDateFormatted || formatDateDisplay(emp.lastDate)) + '</span>') : '') + '$2 +\n          $3' + (emp.lastDateFormatted || emp.lastDate ? ('<br><span style=\"font-size:10px; color:var(--text-dim); font-family:var(--font-mono);\">' + (emp.lastDateFormatted || formatDateDisplay(emp.lastDate)) + '</span>') : '') + '$4"
    );
    console.log('✓ Added Last Punch Date display to Employee Directory');
  }

  // 9. Update startDeviceCloudTransfer Step 3: Extract from /list-users to keep dates intact
  const step3TargetRegex = /\/\/\s*STEP 3: Extract employee profiles[\s\S]*?setStepStatus\(3,\s*'completed'[\s\S]*?\);/;
  const newStep3Code = `// STEP 3: Extract employee profiles, shifts & live punch states
      var emps = transferData.employees || [];
      var liveUsers = [];
      try {
        var uResp = await fetch('http://' + ip + '/list-users', { mode: 'cors' });
        if (uResp.ok) {
          liveUsers = await uResp.json();
          logTransfer("👥 Extracted <b>" + liveUsers.length + "</b> live user profiles from device", "info");
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

  if (step3TargetRegex.test(c)) {
    c = c.replace(step3TargetRegex, newStep3Code);
    console.log('✓ Upgraded startDeviceCloudTransfer Step 3 with live user punch state & dates');
  }

  // 10. Update startDeviceCloudTransfer Step 5: Extract full multi-date history from /get-history
  const step5TargetRegex = /\/\/\s*STEP 5: Finalize records in cloud[\s\S]*?(?=\s*if\s*\(\s*punchesToUpload\.length\s*>\s*0\s*\))/;
  const newStep5Code = `// STEP 5: Extract full multi-date attendance punch logs from device flash memory
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
                var pTime = pParts[0].trim();
                var pRoll = pParts[1].trim();
                var pDir = pParts[4] ? pParts[4].trim() : 'In';

                // Check duplicates
                var existsInArr = punchesToUpload.some(function(ep){
                  return String(ep.roll) === pRoll && isSameDate(ep.date, pDate) && ep.time === pTime && ep.dir === pDir;
                });

                if (!existsInArr) {
                  punchesToUpload.push({
                    time: pTime,
                    roll: pRoll,
                    name: pParts[2] ? pParts[2].trim() : '',
                    role: pParts[3] ? pParts[3].trim() : 'Staff',
                    dept: (empMap[pRoll] && empMap[pRoll].dept) ? empMap[pRoll].dept : 'General',
                    dir: pDir,
                    status: pParts[5] ? pParts[5].trim() : 'On-Time',
                    method: pParts[6] ? pParts[6].trim() : 'Fingerprint',
                    date: pDate,
                    dateFormatted: formatDateDisplay(pDate),
                    device: devName
                  });
                  parsedCount++;
                }
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
      });\n\n`;

  if (step5TargetRegex.test(c)) {
    c = c.replace(step5TargetRegex, newStep5Code);
    console.log('✓ Upgraded startDeviceCloudTransfer Step 5 with full multi-date flash history');
  }

  fs.writeFileSync(filePath, c, 'utf8');
  console.log(`✓ Successfully written ${fileName} (${c.length} bytes)`);
});
