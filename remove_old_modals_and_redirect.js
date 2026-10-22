const fs = require('fs');
const path = require('path');

const files = ['index.html', 'cloud_erp.html'];

files.forEach(file => {
  const p = path.join(__dirname, file);
  if (!fs.existsSync(p)) return;

  let content = fs.readFileSync(p, 'utf8');

  // 1. Update sidebar-status-card to open full config system tab
  content = content.replace(
    'onclick="openFirebaseLinkModal()"',
    'onclick="openConfigSection(\'system\')"'
  );

  // 2. Add openConfigSection helper & redirect openFirebaseLinkModal and openSettingsModal
  const oldOpenSettings = `function openSettingsModal() {
      var navBtn = document.getElementById('nav-configuration');
      switchView('configuration', navBtn);
    }`;

  const newOpenConfigSection = `function openConfigSection(sectionKey) {
      var navBtn = document.getElementById('nav-configuration');
      switchView('configuration', navBtn);
      if (typeof switchConfigTab === 'function') {
        switchConfigTab(sectionKey || 'general');
      }
    }

    function openSettingsModal() {
      openConfigSection('general');
    }

    function openFirebaseLinkModal() {
      openConfigSection('system');
    }`;

  if (content.includes(oldOpenSettings)) {
    content = content.replace(oldOpenSettings, newOpenConfigSection);
    console.log(`✓ Updated openConfigSection, openSettingsModal and openFirebaseLinkModal in ${file}`);
  }

  // 3. Ensure settings-modal and firebase-link-modal are never displayed (set style="display:none !important;")
  content = content.replace(
    'id="settings-modal">',
    'id="settings-modal" style="display:none !important;">'
  );
  content = content.replace(
    'id="firebase-link-modal" style="display:none;">',
    'id="firebase-link-modal" style="display:none !important;">'
  );

  fs.writeFileSync(p, content, 'utf8');
  console.log(`✓ Processed ${file} (${content.length} bytes)`);
});
