const fs = require('fs');

['index.html', 'cloud_erp.html'].forEach(f => {
  const c = fs.readFileSync(f, 'utf8');
  console.log('\n=== ' + f + ' (' + c.length + ' bytes) ===');
  console.log('✓ Lock Overlay:', c.includes('id="portal-lock-overlay"'));
  console.log('✓ checkPortalAuth:', c.includes('checkPortalAuth()'));
  console.log('✓ Lock button in Topbar:', c.includes('lockPortalSession()'));
  console.log('✓ Command Hub:', c.includes('dashboard-command-hub'));
  console.log('✓ 8-Card KPI Matrix:', c.includes('kpi-terminal-status') && c.includes('kpi-sensor-slots'));
  console.log('✓ Recent Search/Dir Filter:', c.includes('dash-recent-search') && c.includes('dash-recent-dir'));
  console.log('✓ Avatar Initials in dashHtml:', c.includes('avatarInitials'));
  console.log('✓ Security Tab in Settings:', c.includes('settings-pane-security'));
  console.log('✓ Date Column in Dashboard Thead:', c.includes('<th>Date</th>'));
  console.log('✓ Date Column in Daily Thead:', c.includes('<th>Date</th>\n                    <th>Roll ID</th>') || c.includes('<th>Date</th>\r\n                    <th>Roll ID</th>'));
});
