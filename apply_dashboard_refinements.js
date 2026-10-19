const fs = require('fs');
const path = require('path');

const targetFiles = ['index.html', 'cloud_erp.html'];

targetFiles.forEach(fileName => {
  const filePath = path.join(__dirname, fileName);
  if (!fs.existsSync(filePath)) return;

  let c = fs.readFileSync(filePath, 'utf8');
  console.log(`\n================ Refining ${fileName} ================`);

  // 1. Upgrade dashHtml row with avatar and click-to-dossier
  const dashRowRegex = /if\s*\(\s*idx\s*<\s*8\s*&&\s*dashBody\s*\)\s*\{[\s\S]*?dashHtml\s*\+=\s*'<tr>'[\s\S]*?'<\/tr>';\s*\}/;
  const newDashRow = `if (idx < 10 && dashBody) {
          var avatarInitials = getInitials(p.name || p.roll);
          dashHtml += '<tr>' +
            '<td><code style="color:var(--text); font-weight:700; font-size:12px;">🕒 ' + (p.time || '-') + '</code></td>' +
            '<td><b style="color:var(--text); font-family:var(--font-mono); font-size:12px; background:rgba(255,255,255,0.04); padding:3px 7px; border-radius:4px; border:1px solid var(--border);">📅 ' + formatDateDisplay(p.date) + '</b></td>' +
            '<td><code style="font-weight:600;">' + (p.roll || '-') + '</code></td>' +
            '<td><div style="display:flex; align-items:center; gap:9px;">' +
              '<div style="width:28px; height:28px; border-radius:50%; background:linear-gradient(135deg, var(--accent), var(--primary)); display:flex; align-items:center; justify-content:center; font-weight:700; color:#fff; font-size:11px; flex-shrink:0;">' + avatarInitials + '</div>' +
              '<div><b style="color:var(--text); font-size:13px;">' + (p.name || 'Unknown') + '</b>' + (p.role ? ('<div style="font-size:10px; color:var(--text-dim);">' + p.role + '</div>') : '') + '</div>' +
            '</div></td>' +
            '<td><span class="badge" style="background:rgba(255,255,255,0.06); font-size:11px;">' + (p.dept || 'General') + '</span></td>' +
            '<td>' + dirBadge + '</td>' +
            '<td>' + statusBadge + '</td>' +
            '<td><span style="font-size:11px; font-weight:500;">' + methodIcon + '</span></td>' +
          '</tr>';
        }`;

  if (dashRowRegex.test(c)) {
    c = c.replace(dashRowRegex, newDashRow);
    console.log('✓ Upgraded dashHtml with avatar initials and rich badge styling');
  }

  // 2. Upgrade renderWeeklyAttendanceChart to use isSameDate and make bars clickable
  c = c.replace(
    'if (normalizeDateToIso(p.date) === dateStr) {',
    'if (isSameDate(p.date, dateStr)) {'
  );

  const oldDayCol = `html += '<div class="day-column ' + todayClass + '" title="' + day.dayName + ' (' + day.displayDate + '): ' + day.present + ' Present, ' + day.absent + ' Absent (' + day.rate + '%)">' +`;
  const newDayCol = `html += '<div class="day-column ' + todayClass + '" style="cursor:pointer;" onclick="setDailyDate(\\'' + day.date + '\\'); switchView(\\'att-daily\\', document.getElementById(\\'nav-att-daily\\'));" title="Click to view ' + day.dayName + ' (' + day.displayDate + ') Attendance Ledger: ' + day.present + ' Present, ' + day.absent + ' Absent (' + day.rate + '%)">' +`;

  if (c.includes(oldDayCol)) {
    c = c.replace(oldDayCol, newDayCol);
    console.log('✓ Made Weekly Attendance chart bars interactively clickable');
  }

  fs.writeFileSync(filePath, c, 'utf8');
  console.log(`✓ Successfully updated ${fileName}`);
});
