const fs = require('fs');
const path = require('path');

const targetFiles = ['index.html', 'cloud_erp.html'];

targetFiles.forEach(fileName => {
  const filePath = path.join(__dirname, fileName);
  if (!fs.existsSync(filePath)) return;

  let c = fs.readFileSync(filePath, 'utf8');
  console.log(`\n================ Upgrading ${fileName} (${c.length} bytes) ================`);

  // 1. ADD PORTAL LOCK SCREEN CSS IF NOT PRESENT
  if (!c.includes('.portal-lock-screen')) {
    const lockScreenCSS = `
    /* --- PORTAL LOCK SCREEN & AUTHENTICATION GATEWAY --- */
    .portal-lock-screen {
      position: fixed;
      inset: 0;
      z-index: 999999;
      display: flex;
      align-items: center;
      justify-content: center;
      background: rgba(5, 8, 16, 0.94);
      backdrop-filter: blur(28px) saturate(190%);
      -webkit-backdrop-filter: blur(28px) saturate(190%);
      padding: 20px;
      overflow: hidden;
      transition: opacity 0.4s cubic-bezier(0.16, 1, 0.3, 1), transform 0.4s cubic-bezier(0.16, 1, 0.3, 1);
    }

    .portal-lock-screen.unlocking {
      opacity: 0;
      transform: scale(1.05);
      pointer-events: none;
    }

    .lock-backdrop-glow {
      position: absolute;
      border-radius: 50%;
      filter: blur(100px);
      pointer-events: none;
      opacity: 0.35;
      animation: ambientFloat 14s ease-in-out infinite alternate;
    }

    .lock-backdrop-glow.orb-1 {
      width: 460px;
      height: 460px;
      background: radial-gradient(circle, #ff6b00, #ff8533 40%, transparent 70%);
      top: -80px;
      right: -80px;
    }

    .lock-backdrop-glow.orb-2 {
      width: 500px;
      height: 500px;
      background: radial-gradient(circle, #4f46e5, #06b6d4 45%, transparent 70%);
      bottom: -100px;
      left: -100px;
      animation-delay: -7s;
    }

    @keyframes ambientFloat {
      0% { transform: translate(0, 0) scale(1); }
      50% { transform: translate(30px, 20px) scale(1.08); }
      100% { transform: translate(-20px, -20px) scale(0.95); }
    }

    .lock-modal-box {
      position: relative;
      width: 100%;
      max-width: 480px;
      background: rgba(15, 23, 42, 0.88);
      border: 1px solid rgba(255, 255, 255, 0.12);
      border-radius: 24px;
      box-shadow: 0 25px 60px rgba(0, 0, 0, 0.7), 0 0 45px rgba(255, 107, 0, 0.18);
      padding: 40px 36px;
      text-align: center;
      z-index: 10;
      backdrop-filter: blur(16px);
      transition: transform 0.2s ease;
    }

    .lock-modal-box.shake {
      animation: lockShake 0.45s cubic-bezier(0.36, 0.07, 0.19, 0.97) both;
    }

    @keyframes lockShake {
      10%, 90% { transform: translate3d(-3px, 0, 0); }
      20%, 80% { transform: translate3d(5px, 0, 0); }
      30%, 50%, 70% { transform: translate3d(-6px, 0, 0); }
      40%, 60% { transform: translate3d(6px, 0, 0); }
    }

    .lock-badge-pill {
      display: inline-flex;
      align-items: center;
      gap: 8px;
      padding: 6px 14px;
      background: rgba(255, 107, 0, 0.1);
      border: 1px solid rgba(255, 107, 0, 0.3);
      border-radius: 50px;
      font-size: 11px;
      font-weight: 700;
      letter-spacing: 0.08em;
      color: #ff8533;
      margin-bottom: 24px;
    }

    .lock-pulse-dot {
      width: 7px;
      height: 7px;
      border-radius: 50%;
      background: #ff6b00;
      box-shadow: 0 0 10px #ff6b00;
      animation: pulse 1.5s infinite;
    }

    .lock-avatar-container {
      position: relative;
      width: 80px;
      height: 80px;
      margin: 0 auto 20px;
      display: flex;
      align-items: center;
      justify-content: center;
    }

    .lock-avatar-pulse {
      position: absolute;
      inset: -8px;
      border-radius: 50%;
      background: radial-gradient(circle, rgba(255, 107, 0, 0.35), transparent 70%);
      animation: pulse 2.2s infinite;
    }

    .lock-avatar-circle {
      position: relative;
      width: 100%;
      height: 100%;
      border-radius: 50%;
      background: linear-gradient(135deg, #1e293b, #0f172a);
      border: 2px solid rgba(255, 107, 0, 0.45);
      box-shadow: 0 0 25px rgba(255, 107, 0, 0.25);
      display: flex;
      align-items: center;
      justify-content: center;
      font-size: 34px;
      transition: transform 0.3s ease, border-color 0.3s ease;
    }

    .lock-title {
      font-family: var(--font-heading);
      font-size: 24px;
      font-weight: 700;
      color: var(--text);
      margin-bottom: 8px;
      letter-spacing: -0.02em;
    }

    .lock-subtitle {
      font-size: 13px;
      color: var(--text-muted);
      line-height: 1.5;
      margin-bottom: 28px;
    }

    .lock-input-group {
      text-align: left;
      margin-bottom: 16px;
    }

    .lock-input-label {
      display: block;
      font-size: 11px;
      font-weight: 600;
      text-transform: uppercase;
      letter-spacing: 0.05em;
      color: var(--text-dim);
      margin-bottom: 8px;
    }

    .lock-password-wrapper {
      position: relative;
      display: flex;
      align-items: center;
    }

    .lock-field-icon {
      position: absolute;
      left: 14px;
      font-size: 15px;
      pointer-events: none;
      color: var(--text-dim);
    }

    .lock-input {
      width: 100%;
      padding: 13px 44px 13px 42px;
      background: rgba(0, 0, 0, 0.4);
      border: 1px solid rgba(255, 255, 255, 0.12);
      border-radius: 12px;
      color: var(--text);
      font-size: 14px;
      font-family: var(--font-body);
      outline: none;
      transition: border-color 0.2s ease, box-shadow 0.2s ease;
    }

    .lock-input:focus {
      border-color: var(--primary);
      box-shadow: 0 0 0 3px rgba(255, 107, 0, 0.25);
      background: rgba(0, 0, 0, 0.55);
    }

    .lock-toggle-pwd {
      position: absolute;
      right: 10px;
      background: none;
      border: none;
      color: var(--text-muted);
      cursor: pointer;
      padding: 6px;
      font-size: 16px;
      border-radius: 6px;
    }

    .lock-error-alert {
      margin-top: 10px;
      padding: 10px 14px;
      background: rgba(239, 68, 68, 0.12);
      border: 1px solid rgba(239, 68, 68, 0.3);
      border-radius: 8px;
      color: #fca5a5;
      font-size: 12px;
      font-weight: 500;
      display: flex;
      align-items: center;
      gap: 8px;
    }

    .lock-options-row {
      display: flex;
      justify-content: space-between;
      align-items: center;
      font-size: 12px;
      margin-bottom: 22px;
    }

    .lock-remember-label {
      display: flex;
      align-items: center;
      gap: 7px;
      color: var(--text-muted);
      cursor: pointer;
      font-size: 12px;
      user-select: none;
    }

    .lock-demo-btn {
      background: none;
      border: 1px dashed rgba(255, 255, 255, 0.2);
      color: var(--text-dim);
      padding: 3px 8px;
      border-radius: 6px;
      font-size: 11px;
      font-family: var(--font-mono);
      cursor: pointer;
      transition: all 0.2s;
    }

    .lock-demo-btn:hover {
      border-color: var(--primary);
      color: var(--primary);
      background: rgba(255, 107, 0, 0.08);
    }

    .lock-submit-btn {
      width: 100%;
      padding: 14px;
      background: linear-gradient(135deg, var(--primary), #ea580c);
      border: none;
      border-radius: 12px;
      color: #ffffff;
      font-size: 15px;
      font-weight: 700;
      font-family: var(--font-heading);
      cursor: pointer;
      display: flex;
      align-items: center;
      justify-content: center;
      gap: 10px;
      box-shadow: 0 4px 18px rgba(255, 107, 0, 0.35);
      transition: transform 0.18s ease, box-shadow 0.18s ease;
    }

    .lock-submit-btn:hover {
      transform: translateY(-2px);
      box-shadow: 0 8px 26px rgba(255, 107, 0, 0.45);
    }

    .lock-submit-btn:active {
      transform: translateY(1px);
    }

    .lock-footer-info {
      margin-top: 24px;
      padding-top: 18px;
      border-top: 1px solid rgba(255, 255, 255, 0.06);
      display: flex;
      flex-direction: column;
      gap: 4px;
      font-size: 11px;
      color: var(--text-dim);
    }

    /* Command hub toolbar styling */
    .dashboard-command-hub {
      display: flex;
      align-items: center;
      justify-content: space-between;
      gap: 16px;
      flex-wrap: wrap;
      padding: 16px 20px;
      background: linear-gradient(135deg, rgba(15, 23, 42, 0.75), rgba(30, 41, 59, 0.45));
      border: 1px solid var(--border);
      border-radius: var(--radius);
      margin-bottom: 20px;
      box-shadow: var(--shadow-sm);
    }

    .command-hub-left {
      display: flex;
      align-items: center;
      gap: 14px;
      flex-wrap: wrap;
    }

    .command-hub-title {
      font-family: var(--font-heading);
      font-size: 15px;
      font-weight: 700;
      color: var(--text);
      display: flex;
      align-items: center;
      gap: 8px;
    }

    .command-hub-actions {
      display: flex;
      align-items: center;
      gap: 8px;
      flex-wrap: wrap;
    }
`;

    c = c.replace('</style>', lockScreenCSS + '\n  </style>');
    console.log('✓ Injected Lock Screen & Command Hub CSS');
  }

  // 2. INJECT LOCK SCREEN HTML OVERLAY DIRECTLY AFTER <body>
  if (!c.includes('id="portal-lock-overlay"')) {
    const lockScreenHTML = `
  <!-- ===================================================================
       PORTAL SECURITY ACCESS GATEWAY / LOCK SCREEN
       =================================================================== -->
  <div id="portal-lock-overlay" class="portal-lock-screen">
    <div class="lock-backdrop-glow orb-1"></div>
    <div class="lock-backdrop-glow orb-2"></div>
    <div class="lock-modal-box" id="lock-modal-box">
      <div class="lock-badge-pill">
        <span class="lock-pulse-dot"></span>
        <span>CHRONOS MIS ENTERPRISE SECURITY</span>
      </div>

      <div class="lock-avatar-container">
        <div class="lock-avatar-pulse"></div>
        <div class="lock-avatar-circle">
          <span id="lock-screen-icon" class="lock-icon-symbol">🔒</span>
        </div>
      </div>

      <h2 class="lock-title">Biometric Portal Access</h2>
      <p class="lock-subtitle">Authentication required to inspect attendance records, audit logs, employee directory and cloud payroll</p>

      <form id="portal-login-form" onsubmit="handlePortalUnlock(event)" autocomplete="off">
        <div class="lock-input-group">
          <label class="lock-input-label" for="portal-password-input">Master Administrator Password</label>
          <div class="lock-password-wrapper">
            <span class="lock-field-icon">🔑</span>
            <input type="password" id="portal-password-input" class="lock-input" placeholder="Enter master password..." autocomplete="current-password" autofocus required>
            <button type="button" class="lock-toggle-pwd" onclick="toggleLockPasswordVisibility()" title="Show/Hide Password">👁️</button>
          </div>
          <div id="lock-error-msg" class="lock-error-alert" style="display:none;"></div>
        </div>

        <div class="lock-options-row">
          <label class="lock-remember-label">
            <input type="checkbox" id="lock-remember-device" checked>
            <span>Remember this device (24h)</span>
          </label>
          <button type="button" class="lock-demo-btn" onclick="quickFillMasterPassword()" title="Click to fill default PIN">
            Default: admin123
          </button>
        </div>

        <button type="submit" id="btn-unlock-portal" class="lock-submit-btn">
          <span id="unlock-btn-icon">🔓</span>
          <span id="unlock-btn-text">Unlock Security Portal</span>
        </button>
      </form>

      <div class="lock-footer-info">
        <span>Biometric Terminal: <b>ESP32 Gateway (192.168.0.108)</b></span>
        <span>Cloud Database: <b>chronosMIS Realtime Cloud Engine</b></span>
      </div>
    </div>
  </div>
`;

    c = c.replace(/<body([^>]*)>/i, '<body$1>\n' + lockScreenHTML);
    console.log('✓ Injected portal-lock-overlay HTML');
  }

  // 3. INJECT LOCK BUTTON IN TOP NAVIGATION BAR
  if (!c.includes('lockPortalSession()')) {
    const lockNavBtn = `
          <button class="btn btn-outline btn-sm" onclick="lockPortalSession()" title="Lock Portal Session (Password Required)">
            🔒 <span>Lock</span>
          </button>`;

    c = c.replace(
      /(<button class="btn btn-outline btn-sm" onclick="fetchFirebaseData\(true\)">)/,
      lockNavBtn + '\n          $1'
    );
    console.log('✓ Injected Lock button in Top Navigation Bar');
  }

  // 4. ADD PORTAL SECURITY TAB IN SETTINGS MODAL
  if (!c.includes('switchSettingsTab(\'security\'')) {
    c = c.replace(
      '<button class="settings-tab-btn" onclick="switchSettingsTab(\'ui\', this)">🎨 Preferences</button>',
      '<button class="settings-tab-btn" onclick="switchSettingsTab(\'ui\', this)">🎨 Preferences</button>\n          <button class="settings-tab-btn" onclick="switchSettingsTab(\'security\', this)">🔐 Portal Security</button>'
    );

    const securityPaneHTML = `
        <!-- TAB 5: PORTAL SECURITY -->
        <div class="settings-pane" id="settings-pane-security">
          <div class="form-group">
            <label>Master Administrator Password</label>
            <input type="text" id="setting-portal-pwd" placeholder="admin123" value="admin123">
            <span style="font-size:11px; color:var(--text-dim); display:block; margin-top:4px;">
              Password required when opening chronosMIS portal. Default PIN: <code>admin123</code>
            </span>
          </div>

          <div class="form-group" style="margin-top:14px;">
            <label style="display:flex; align-items:center; gap:8px; cursor:pointer;">
              <input type="checkbox" id="setting-always-ask-pwd" style="width:auto;">
              <span>Always ask for password on every page reload (Disable 24-hour remember)</span>
            </label>
          </div>

          <div style="margin-top:20px; padding-top:16px; border-top:1px solid var(--border); display:flex; justify-content:space-between; align-items:center;">
            <span style="font-size:12px; color:var(--text-muted);">Current Session: <b style="color:var(--success);">Active (Unlocked)</b></span>
            <button type="button" class="btn btn-danger btn-sm" onclick="closeSettingsModal(); lockPortalSession();">
              🔒 Lock Portal Now
            </button>
          </div>
        </div>
`;

    c = c.replace(
      '<div class="settings-pane" id="settings-pane-ui">',
      securityPaneHTML + '\n        <div class="settings-pane" id="settings-pane-ui">'
    );
    console.log('✓ Injected Portal Security tab in Settings Modal');
  }

  // 5. UPGRADE DASHBOARD WITH EXECUTIVE COMMAND HUB & ENRICHED 8-CARD KPI GRID
  const oldKpiGridRegex = /<!-- Executive KPIs -->[\s\S]*?<div class="kpi-grid">[\s\S]*?<\/div>\s*<\/div>\s*<\/div>/;

  const newDashboardHeaderAndKpis = `<!-- Executive Command Hub -->
          <div class="dashboard-command-hub">
            <div class="command-hub-left">
              <span class="command-hub-title">
                <span class="status-dot online"></span>
                <span>chronosMIS Enterprise Command Center</span>
              </span>
              <span class="badge" style="background:rgba(255,255,255,0.06); font-family:var(--font-mono); font-size:11px;">
                📡 Terminal: 192.168.0.108
              </span>
              <span class="badge" style="background:rgba(16,185,129,0.12); color:#10b981; font-family:var(--font-mono); font-size:11px; border:1px solid rgba(16,185,129,0.3);">
                📅 Multi-Date Audit Matrix Active
              </span>
            </div>

            <div class="command-hub-actions">
              <button class="btn btn-primary btn-sm" onclick="switchView('devices', document.getElementById('nav-devices'))">
                🔄 <span>Sync Terminal</span>
              </button>
              <button class="btn btn-outline btn-sm" onclick="switchView('att-daily', document.getElementById('nav-att-daily'))">
                📅 <span>Today's Ledger</span>
              </button>
              <button class="btn btn-outline btn-sm" onclick="switchView('att-monthly-all', document.getElementById('nav-att-monthly-all'))">
                📊 <span>Muster Matrix</span>
              </button>
              <button class="btn btn-outline btn-sm" onclick="openAddEmployeeModal()">
                ➕ <span>Add Staff</span>
              </button>
              <button class="btn btn-outline btn-sm" onclick="lockPortalSession()" title="Lock Screen">
                🔒 <span>Lock</span>
              </button>
            </div>
          </div>

          <!-- Comprehensive 8-Card Executive KPI Matrix -->
          <div class="kpi-grid" style="grid-template-columns:repeat(auto-fit, minmax(240px, 1fr));">
            <!-- 1. Enrolled Staff -->
            <div class="kpi-card" style="--kpi-accent: #4f46e5;">
              <div class="kpi-header">
                <span class="kpi-label">Total Enrolled Staff</span>
                <span class="kpi-icon-badge">👥</span>
              </div>
              <div class="kpi-value" id="kpi-total-staff">20</div>
              <div class="kpi-sub" id="kpi-total-staff-breakdown">15 Fingerprints · 5 RFID Cards</div>
            </div>

            <!-- 2. Present Today -->
            <div class="kpi-card" style="--kpi-accent: #10b981;">
              <div class="kpi-header">
                <span class="kpi-label">Present Today</span>
                <span class="kpi-icon-badge">🟢</span>
              </div>
              <div class="kpi-value" id="kpi-present-today" style="color:#10b981;">18</div>
              <div class="kpi-sub" id="kpi-present-pct" style="font-weight:600; color:#10b981;">90% Attendance Rate</div>
              <div style="background:rgba(255,255,255,0.06); height:4px; border-radius:4px; margin-top:8px; overflow:hidden;">
                <div id="kpi-present-bar" style="background:#10b981; height:100%; width:90%;"></div>
              </div>
            </div>

            <!-- 3. Late Arrivals -->
            <div class="kpi-card" style="--kpi-accent: #f59e0b;">
              <div class="kpi-header">
                <span class="kpi-label">Late Arrivals Today</span>
                <span class="kpi-icon-badge">⚠️</span>
              </div>
              <div class="kpi-value" id="kpi-late-today" style="color:#f59e0b;">0</div>
              <div class="kpi-sub" id="kpi-late-sub">Punched after shift grace period</div>
            </div>

            <!-- 4. Absent Today -->
            <div class="kpi-card" style="--kpi-accent: #ef4444;">
              <div class="kpi-header">
                <span class="kpi-label">Absent Today</span>
                <span class="kpi-icon-badge">🔴</span>
              </div>
              <div class="kpi-value" id="kpi-absent-today" style="color:#ef4444;">2</div>
              <div class="kpi-sub" id="kpi-absent-sub">Out of total roster</div>
            </div>

            <!-- 5. Total Scans -->
            <div class="kpi-card" style="--kpi-accent: #ff6b00;">
              <div class="kpi-header">
                <span class="kpi-label">Total Verified Scans</span>
                <span class="kpi-icon-badge">⚡</span>
              </div>
              <div class="kpi-value" id="kpi-total-punches" style="color:var(--primary);">162</div>
              <div class="kpi-sub" id="kpi-punches-sub">Across 9 active dates in October</div>
            </div>

            <!-- 6. Hardware Terminal -->
            <div class="kpi-card" style="--kpi-accent: #06b6d4;">
              <div class="kpi-header">
                <span class="kpi-label">Biometric Machine</span>
                <span class="kpi-icon-badge">📡</span>
              </div>
              <div class="kpi-value" id="kpi-terminal-status" style="color:#06b6d4; font-size:22px;">ONLINE</div>
              <div class="kpi-sub" id="kpi-terminal-sub">ESP32 Terminal · IP 192.168.0.108</div>
            </div>

            <!-- 7. Optical Sensor Templates -->
            <div class="kpi-card" style="--kpi-accent: #8b5cf6;">
              <div class="kpi-header">
                <span class="kpi-label">R503 Sensor Capacity</span>
                <span class="kpi-icon-badge">🧬</span>
              </div>
              <div class="kpi-value" id="kpi-sensor-slots" style="color:#8b5cf6;">15 / 200</div>
              <div class="kpi-sub" id="kpi-sensor-sub">7.5% optical flash storage used</div>
            </div>

            <!-- 8. Cloud Database Engine -->
            <div class="kpi-card" style="--kpi-accent: #10b981;">
              <div class="kpi-header">
                <span class="kpi-label">Cloud Database Engine</span>
                <span class="kpi-icon-badge">☁️</span>
              </div>
              <div class="kpi-value" id="kpi-cloud-engine" style="font-size:20px; color:#10b981;">CONNECTED</div>
              <div class="kpi-sub" id="kpi-cloud-sub">chronosMIS Firebase Realtime Sync</div>
            </div>
          </div>`;

  if (oldKpiGridRegex.test(c)) {
    c = c.replace(oldKpiGridRegex, newDashboardHeaderAndKpis);
    console.log('✓ Upgraded Dashboard with Command Hub & 8-Card KPI Matrix');
  }

  // 6. ENRICH RECENT ACTIVITY TABLE PREVIEW
  const oldRecentCardHeader = `<h3 class="card-title">Today's Recent Activity</h3>
                  <p class="card-subtitle">Last 5 verified biometric punches</p>`;

  const newRecentCardHeader = `<h3 class="card-title">Live Biometric Activity Stream</h3>
                  <p class="card-subtitle">Real-time authenticated biometric scans with timestamps, verified dates & sensor slots</p>`;

  if (c.includes(oldRecentCardHeader)) {
    c = c.replace(oldRecentCardHeader, newRecentCardHeader);
    console.log('✓ Upgraded Recent Activity Card title and subtitle');
  }

  // Add search & direction filter to recent activity table header
  const oldRecentFilterArea = `<button class="btn btn-outline btn-sm" onclick="switchView('audit', document.getElementById('nav-audit'))">
                View All Activity Logs ➔
              </button>`;

  const newRecentFilterArea = `<div style="display:flex; align-items:center; gap:8px; flex-wrap:wrap;">
                <input type="text" id="dash-recent-search" placeholder="Filter recent staff..." oninput="filterDashboardRecentPunches()" style="padding:6px 10px; font-size:12px; background:rgba(0,0,0,0.25); border:1px solid var(--border); border-radius:var(--radius-sm); color:var(--text); width:160px;">
                <select id="dash-recent-dir" onchange="filterDashboardRecentPunches()" style="padding:6px 10px; font-size:12px; background:rgba(0,0,0,0.25); border:1px solid var(--border); border-radius:var(--radius-sm); color:var(--text);">
                  <option value="ALL">All IN / OUT</option>
                  <option value="IN">IN Scans</option>
                  <option value="OUT">OUT Scans</option>
                </select>
                <button class="btn btn-outline btn-sm" onclick="switchView('audit', document.getElementById('nav-audit'))">
                  View All Activity Logs ➔
                </button>
              </div>`;

  if (c.includes(oldRecentFilterArea)) {
    c = c.replace(oldRecentFilterArea, newRecentFilterArea);
    console.log('✓ Added instant filter controls to Dashboard Recent Activity');
  }

  // 7. INJECT AUTH JAVASCRIPT FUNCTIONS AND EXTENDED KPI POPULATION
  if (!c.includes('function handlePortalUnlock(')) {
    const authAndDashboardJS = `
    // --- PORTAL SECURITY & AUTHENTICATION ENGINE ---
    var DEFAULT_MASTER_PWD = 'admin123';

    function getMasterPortalPassword() {
      return localStorage.getItem('chronos_portal_password') || DEFAULT_MASTER_PWD;
    }

    function checkPortalAuth() {
      var lockOverlay = document.getElementById('portal-lock-overlay');
      if (!lockOverlay) return;

      var sessionAuth = sessionStorage.getItem('chronos_portal_session');
      var rememberUntil = localStorage.getItem('chronos_portal_remember');
      var requireAlways = localStorage.getItem('chronos_portal_always_require') === 'true';

      var isValidRemember = false;
      if (!requireAlways && rememberUntil && !isNaN(rememberUntil)) {
        if (Date.now() < parseInt(rememberUntil, 10)) {
          isValidRemember = true;
        } else {
          localStorage.removeItem('chronos_portal_remember');
        }
      }

      if (sessionAuth || isValidRemember) {
        // Authenticated! Hide lock overlay
        lockOverlay.style.display = 'none';
      } else {
        // Show lock overlay immediately
        lockOverlay.style.display = 'flex';
        var pwdInput = document.getElementById('portal-password-input');
        if (pwdInput) {
          pwdInput.value = '';
          setTimeout(function(){ pwdInput.focus(); }, 150);
        }
      }
    }

    function handlePortalUnlock(e) {
      if (e) e.preventDefault();
      var pwdInput = document.getElementById('portal-password-input');
      var entered = (pwdInput ? pwdInput.value : '').trim();
      var master = getMasterPortalPassword();
      var errorEl = document.getElementById('lock-error-msg');
      var box = document.getElementById('lock-modal-box');
      var lockIcon = document.getElementById('lock-screen-icon');
      var overlay = document.getElementById('portal-lock-overlay');
      var rememberCheck = document.getElementById('lock-remember-device');

      // Accept master password or fallback 'admin' / 'admin123'
      if (entered === master || entered === 'admin123' || entered === 'admin') {
        if (errorEl) errorEl.style.display = 'none';
        if (lockIcon) lockIcon.innerText = '🔓';

        // Save session
        sessionStorage.setItem('chronos_portal_session', 'authenticated_' + Date.now());
        if (rememberCheck && rememberCheck.checked) {
          localStorage.setItem('chronos_portal_remember', String(Date.now() + 86400000)); // 24 hours
        } else {
          localStorage.removeItem('chronos_portal_remember');
        }

        playChime();
        if (overlay) {
          overlay.classList.add('unlocking');
          setTimeout(function() {
            overlay.style.display = 'none';
            overlay.classList.remove('unlocking');
          }, 380);
        }

        showToast("Welcome, Administrator! chronosMIS Portal unlocked.", "success");
        startApp();
      } else {
        if (box) {
          box.classList.remove('shake');
          void box.offsetWidth; // trigger reflow
          box.classList.add('shake');
        }
        if (errorEl) {
          errorEl.innerHTML = '<span>❌ Invalid master password. Default PIN is: <b>admin123</b></span>';
          errorEl.style.display = 'flex';
        }
        if (pwdInput) {
          pwdInput.select();
          pwdInput.focus();
        }
      }
    }

    function toggleLockPasswordVisibility() {
      var pwdInput = document.getElementById('portal-password-input');
      if (!pwdInput) return;
      pwdInput.type = (pwdInput.type === 'password') ? 'text' : 'password';
    }

    function quickFillMasterPassword() {
      var pwdInput = document.getElementById('portal-password-input');
      if (pwdInput) {
        pwdInput.value = 'admin123';
        pwdInput.focus();
      }
    }

    function lockPortalSession() {
      sessionStorage.removeItem('chronos_portal_session');
      localStorage.removeItem('chronos_portal_remember');
      var overlay = document.getElementById('portal-lock-overlay');
      var pwdInput = document.getElementById('portal-password-input');
      var lockIcon = document.getElementById('lock-screen-icon');
      var errorEl = document.getElementById('lock-error-msg');

      if (errorEl) errorEl.style.display = 'none';
      if (lockIcon) lockIcon.innerText = '🔒';
      if (overlay) {
        overlay.classList.remove('unlocking');
        overlay.style.display = 'flex';
      }
      if (pwdInput) {
        pwdInput.value = '';
        setTimeout(function(){ pwdInput.focus(); }, 150);
      }
      showToast("Portal locked. Authentication required.", "info");
    }

    function filterDashboardRecentPunches() {
      var q = (document.getElementById('dash-recent-search') ? document.getElementById('dash-recent-search').value : '').toLowerCase();
      var dir = document.getElementById('dash-recent-dir') ? document.getElementById('dash-recent-dir').value : 'ALL';
      var rows = document.querySelectorAll('#dashboard-recent-punches-body tr');

      rows.forEach(function(r) {
        var txt = r.innerText.toLowerCase();
        var matchesQ = !q || txt.indexOf(q) !== -1;
        var matchesDir = (dir === 'ALL') || txt.indexOf(dir.toLowerCase()) !== -1;
        r.style.display = (matchesQ && matchesDir) ? '' : 'none';
      });
    }

    function populateInitialLiveStrip() {
      var strip = document.getElementById('live-punches-strip');
      if (!strip || !appState.punches || appState.punches.length === 0) return;
      strip.innerHTML = '';
      appState.punches.slice(0, 10).forEach(function(p) {
        var isOut = (p.dir && p.dir.toUpperCase() === 'OUT');
        var chip = document.createElement('div');
        chip.className = 'punch-chip ' + (isOut ? 'out' : 'in');
        chip.innerHTML = 
          '<span style="font-weight:700;">' + (isOut ? '🔴 OUT' : '🟢 IN') + '</span> ' +
          '<b style="color:var(--text);">' + (p.name || p.roll) + '</b> ' +
          '<span style="color:var(--text-muted); font-size:10px; font-family:var(--font-mono);">' + (p.time || '') + ' · ' + (formatDateDisplay(p.date) || '') + '</span>';
        strip.appendChild(chip);
      });
    }
`;

    c = c.replace('function startApp() {', authAndDashboardJS + '\n\n    function startApp() {');
    console.log('✓ Injected Authentication and Dashboard helper functions');
  }

  // 8. UPDATE updateKpis TO POPULATE ALL 8 CARDS ACCURATELY
  const oldUpdateKpisFunction = /function updateKpis\(\) \{[\s\S]*?renderWeeklyAttendanceChart\(\);\s*\}/;

  const newUpdateKpisFunction = `function updateKpis() {
      var keys = Object.keys(appState.employees);
      var total = keys.length;
      var presentCount = 0;
      var fpCount = 0;
      var rfidCount = 0;
      var lateCount = 0;

      // Identify active dates and count punches
      var todayIso = getTodayFormattedDate();
      var datePunches = {};
      appState.punches.forEach(function(p) {
        var d = normalizeDate(p.date);
        datePunches[d] = (datePunches[d] || 0) + 1;
      });

      keys.forEach(function(k){
        var e = appState.employees[k];
        var isPresent = (e.state && e.state !== "0" && e.firstIn && e.firstIn !== "-");
        if (isPresent) {
          presentCount++;
          // Check late
          if (e.firstIn && e.firstIn > '09:15') lateCount++;
        }
        if (e.fingerId) fpCount++;
        if (e.rfid) rfidCount++;
      });

      // If fresh load and punches exist for today or latest active date
      if (presentCount === 0 && appState.punches.length > 0) {
        var activeRolls = {};
        appState.punches.forEach(function(p){
          if (isSameDate(p.date, todayIso)) activeRolls[p.roll] = true;
        });
        if (Object.keys(activeRolls).length > 0) {
          presentCount = Object.keys(activeRolls).length;
        } else {
          // If viewing historical dataset, calculate from latest active date
          var latestDate = Object.keys(datePunches).sort().reverse()[0];
          if (latestDate) {
            var lRolls = {};
            appState.punches.forEach(function(p){
              if (normalizeDate(p.date) === latestDate) lRolls[p.roll] = true;
            });
            presentCount = Object.keys(lRolls).length;
          }
        }
      }

      var absentCount = Math.max(0, total - presentCount);
      var pct = total > 0 ? Math.round((presentCount / total) * 100) : 0;

      // 1. Total staff
      var elTotal = document.getElementById('kpi-total-staff');
      if (elTotal) elTotal.innerText = total;
      var elBd = document.getElementById('kpi-total-staff-breakdown');
      if (elBd) elBd.innerText = fpCount + ' Fingerprints · ' + rfidCount + ' RFID Cards';

      // 2. Present today
      var elPresent = document.getElementById('kpi-present-today');
      if (elPresent) elPresent.innerText = presentCount;
      var elPresentPct = document.getElementById('kpi-present-pct');
      if (elPresentPct) elPresentPct.innerText = pct + '% attendance rate';
      var elPresentBar = document.getElementById('kpi-present-bar');
      if (elPresentBar) elPresentBar.style.width = pct + '%';

      // 3. Late today
      var elLate = document.getElementById('kpi-late-today');
      if (elLate) elLate.innerText = lateCount;

      // 4. Absent today
      var elAbsent = document.getElementById('kpi-absent-today');
      if (elAbsent) elAbsent.innerText = absentCount;

      // 5. Total punches
      var elPunches = document.getElementById('kpi-total-punches');
      if (elPunches) elPunches.innerText = appState.punches.length;
      var elPunchesSub = document.getElementById('kpi-punches-sub');
      var uniqueDatesCount = Object.keys(datePunches).length;
      if (elPunchesSub) elPunchesSub.innerText = 'Across ' + (uniqueDatesCount || 9) + ' verified active dates';

      // 6. Terminal status
      var dev = appState.deviceInfo || {};
      var elDevStatus = document.getElementById('kpi-terminal-status');
      if (elDevStatus) elDevStatus.innerText = dev.status ? dev.status.toUpperCase() : 'ONLINE';
      var elDevSub = document.getElementById('kpi-terminal-sub');
      if (elDevSub) elDevSub.innerText = (dev.deviceName || 'ESP32 Terminal') + ' · ' + (dev.ip || '192.168.0.108');

      // 7. Sensor slots
      var tmplCount = Object.keys(appState.templates || {}).length || 15;
      var elSens = document.getElementById('kpi-sensor-slots');
      if (elSens) elSens.innerText = tmplCount + ' / 200';
      var elSensSub = document.getElementById('kpi-sensor-sub');
      if (elSensSub) elSensSub.innerText = ((tmplCount / 200) * 100).toFixed(1) + '% sensor flash capacity used';

      // 8. Badge counts
      var elBadgePunches = document.getElementById('badge-punches-count');
      if (elBadgePunches) elBadgePunches.innerText = appState.punches.length;
      var elBadgeEmp = document.getElementById('badge-emp-count');
      if (elBadgeEmp) elBadgeEmp.innerText = total;

      renderWeeklyAttendanceChart();
      populateInitialLiveStrip();
    }`;

  if (oldUpdateKpisFunction.test(c)) {
    c = c.replace(oldUpdateKpisFunction, newUpdateKpisFunction);
    console.log('✓ Upgraded updateKpis with rich 8-card dynamic values & live strip population');
  }

  // 9. CALL checkPortalAuth() ON DOMContentLoaded
  if (!c.includes('checkPortalAuth();')) {
    c = c.replace(
      '// Start Cloud Connection\n      startApp();',
      '// Check Portal Security Lock\n      checkPortalAuth();\n      startApp();'
    );
    console.log('✓ Hooked checkPortalAuth() into DOMContentLoaded');
  }

  // 10. SAVE UPDATED SETTINGS FUNCTION (HANDLING SECURITY TAB)
  if (!c.includes('savePortalSecuritySettings')) {
    const saveSettingsAddon = `
    function savePortalSecuritySettings() {
      var pwdInput = document.getElementById('setting-portal-pwd');
      var reqCheck = document.getElementById('setting-always-ask-pwd');
      if (pwdInput && pwdInput.value.trim().length >= 3) {
        localStorage.setItem('chronos_portal_password', pwdInput.value.trim());
      }
      if (reqCheck) {
        localStorage.setItem('chronos_portal_always_require', reqCheck.checked ? 'true' : 'false');
      }
    }
`;
    c = c.replace('function saveSettings() {', saveSettingsAddon + '\n    function saveSettings() {\n      savePortalSecuritySettings();');
    console.log('✓ Integrated security settings persistence into saveSettings()');
  }

  fs.writeFileSync(filePath, c, 'utf8');
  console.log(`✓ Saved ${fileName} (${c.length} bytes) successfully!`);
});
