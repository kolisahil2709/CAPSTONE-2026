const fs = require('fs');
const path = require('path');

console.log('================================================================');
console.log('  CHRONOS MIS: ENTERPRISE FULL-SIZE CONFIGURATION & LINEAR UI  ');
console.log('================================================================');

const targetFiles = ['index.html', 'cloud_erp.html'];

// 1. CSS TO INJECT BEFORE </style>
const cssToInject = `
    /* =========================================================================
       APP-WIDE DESIGN SYSTEM & DASHBOARD TYPOGRAPHY LINEARITY
       Ensures consistent font-family, card glassmorphism, table layout,
       buttons, form controls, and badge styling across EVERY screen.
       ========================================================================= */
    body, html {
      font-family: var(--font-body) !important;
      letter-spacing: -0.01em;
    }

    h1, h2, h3, h4, h5, h6,
    .topbar-view-title,
    .card-title,
    .modal-title,
    .brand-main-title,
    .nav-label,
    .config-sidebar-heading,
    .cfg-text,
    .kpi-label,
    .tile-label {
      font-family: var(--font-heading) !important;
      letter-spacing: -0.02em;
    }

    input, select, textarea, button, optgroup {
      font-family: var(--font-body);
    }

    .mono, [data-mono], .badge-mono, .clock-pill, .matrix-cell, code, pre,
    .cfg-badge, .nav-pill, .kpi-sub code, .table-mono {
      font-family: var(--font-mono) !important;
    }

    /* Linear Card Glassmorphism across the entire app */
    .card,
    .content-view .card,
    .weekly-chart-card,
    .device-gateway-card,
    .device-archive-card,
    .daily-summary-tile,
    .matrix-card {
      background: linear-gradient(145deg, rgba(255, 255, 255, 0.04) 0%, rgba(15, 23, 42, 0.65) 100%) !important;
      border: 1px solid rgba(255, 255, 255, 0.08) !important;
      border-radius: 14px !important;
      backdrop-filter: blur(16px);
      -webkit-backdrop-filter: blur(16px);
      box-shadow: 0 8px 24px rgba(0, 0, 0, 0.25) !important;
      transition: border-color 0.25s ease, box-shadow 0.25s ease, transform 0.25s ease;
    }

    [data-theme="light"] .card,
    [data-theme="light"] .content-view .card,
    [data-theme="light"] .weekly-chart-card,
    [data-theme="light"] .device-gateway-card,
    [data-theme="light"] .device-archive-card,
    [data-theme="light"] .daily-summary-tile,
    [data-theme="light"] .matrix-card {
      background: linear-gradient(145deg, #ffffff 0%, #f8fafc 100%) !important;
      border: 1px solid rgba(0, 0, 0, 0.08) !important;
      box-shadow: 0 6px 20px rgba(0, 0, 0, 0.05) !important;
    }

    /* Linear Card Header across all views */
    .card-header {
      padding: 18px 22px;
      border-bottom: 1px solid rgba(255, 255, 255, 0.06);
      display: flex;
      justify-content: space-between;
      align-items: center;
    }
    [data-theme="light"] .card-header {
      border-bottom: 1px solid rgba(0, 0, 0, 0.06);
    }

    .card-title {
      font-family: var(--font-heading) !important;
      font-size: 16px !important;
      font-weight: 700 !important;
      color: var(--text) !important;
      letter-spacing: -0.02em !important;
      display: flex;
      align-items: center;
      gap: 8px;
    }

    .card-subtitle {
      font-size: 12px !important;
      color: var(--text-dim) !important;
      margin-top: 3px !important;
      font-weight: 400 !important;
    }

    /* Linear Tables across all views */
    .table-container, .matrix-table-container {
      border: 1px solid rgba(255, 255, 255, 0.08);
      border-radius: 12px;
      overflow: hidden;
      background: rgba(10, 15, 29, 0.5);
    }
    [data-theme="light"] .table-container, [data-theme="light"] .matrix-table-container {
      border-color: rgba(0, 0, 0, 0.08);
      background: #ffffff;
    }

    table th {
      font-family: var(--font-heading) !important;
      font-size: 11px !important;
      font-weight: 700 !important;
      letter-spacing: 0.8px !important;
      text-transform: uppercase !important;
      color: var(--text-dim) !important;
      background: rgba(15, 23, 42, 0.85) !important;
      padding: 12px 14px !important;
      border-bottom: 1px solid rgba(255, 255, 255, 0.08) !important;
    }
    [data-theme="light"] table th {
      background: #f1f5f9 !important;
      border-bottom: 1px solid #e2e8f0 !important;
      color: #475569 !important;
    }

    table td {
      font-family: var(--font-body) !important;
      font-size: 13px !important;
      padding: 12px 14px !important;
      border-bottom: 1px solid rgba(255, 255, 255, 0.04) !important;
      color: var(--text);
    }
    [data-theme="light"] table td {
      border-bottom: 1px solid #f1f5f9 !important;
      color: #1e293b;
    }

    table tbody tr:hover {
      background: rgba(255, 255, 255, 0.03) !important;
    }
    [data-theme="light"] table tbody tr:hover {
      background: rgba(241, 245, 249, 0.7) !important;
    }

    /* Linear Form Inputs & Controls */
    .form-input,
    select,
    input[type="text"],
    input[type="password"],
    input[type="date"],
    input[type="month"],
    input[type="time"],
    input[type="number"],
    textarea {
      font-family: var(--font-body);
      font-size: 13px;
      border-radius: 8px;
      border: 1px solid var(--border);
      background: rgba(0, 0, 0, 0.22);
      color: var(--text);
      padding: 9px 13px;
      outline: none;
      transition: all 0.2s ease;
    }
    [data-theme="light"] .form-input,
    [data-theme="light"] select,
    [data-theme="light"] input[type="text"],
    [data-theme="light"] input[type="password"],
    [data-theme="light"] input[type="date"],
    [data-theme="light"] input[type="month"],
    [data-theme="light"] input[type="time"],
    [data-theme="light"] input[type="number"],
    [data-theme="light"] textarea {
      background: #ffffff;
      border-color: #cbd5e1;
      color: #0f172a;
    }
    .form-input:focus,
    select:focus,
    input:focus,
    textarea:focus {
      border-color: var(--primary) !important;
      box-shadow: 0 0 12px var(--primary-glow) !important;
    }

    /* Linear Badges */
    .badge {
      font-family: var(--font-mono);
      font-size: 11px;
      font-weight: 600;
      border-radius: 100px;
      padding: 3px 9px;
      display: inline-flex;
      align-items: center;
      gap: 5px;
      letter-spacing: 0.3px;
    }

    /* =========================================================================
       FULL-SIZE CONFIGURATION SECTION WITH OWN SUB-SIDEBAR
       ========================================================================= */
    .config-layout {
      display: flex;
      gap: 22px;
      align-items: flex-start;
      min-height: calc(100vh - 130px);
    }

    .config-sidebar {
      width: 270px;
      min-width: 270px;
      background: linear-gradient(165deg, rgba(255, 255, 255, 0.04) 0%, rgba(15, 23, 42, 0.75) 100%);
      border: 1px solid rgba(255, 255, 255, 0.08);
      border-radius: 14px;
      backdrop-filter: blur(16px);
      -webkit-backdrop-filter: blur(16px);
      box-shadow: 0 8px 24px rgba(0, 0, 0, 0.25);
      padding: 16px;
      position: sticky;
      top: 80px;
      display: flex;
      flex-direction: column;
      gap: 12px;
    }
    [data-theme="light"] .config-sidebar {
      background: linear-gradient(165deg, #ffffff 0%, #f8fafc 100%);
      border: 1px solid rgba(0, 0, 0, 0.08);
      box-shadow: 0 6px 20px rgba(0, 0, 0, 0.05);
    }

    .config-sidebar-header {
      padding: 6px 8px 12px;
      border-bottom: 1px solid rgba(255, 255, 255, 0.07);
    }
    [data-theme="light"] .config-sidebar-header {
      border-bottom: 1px solid rgba(0, 0, 0, 0.07);
    }

    .config-sidebar-tag {
      font-family: var(--font-heading);
      font-size: 10px;
      font-weight: 800;
      color: var(--primary);
      text-transform: uppercase;
      letter-spacing: 1px;
      margin-bottom: 3px;
    }

    .config-sidebar-heading {
      font-family: var(--font-heading);
      font-size: 19px;
      font-weight: 800;
      color: var(--text);
      letter-spacing: -0.02em;
    }

    .config-sidebar-desc {
      font-size: 11px;
      color: var(--text-dim);
      margin-top: 3px;
      line-height: 1.4;
    }

    .config-nav {
      display: flex;
      flex-direction: column;
      gap: 4px;
    }

    .config-nav-btn {
      width: 100%;
      display: flex;
      align-items: center;
      gap: 11px;
      padding: 10px 14px;
      border-radius: 9px;
      background: transparent;
      border: 1px solid transparent;
      color: var(--text-muted);
      cursor: pointer;
      text-align: left;
      transition: all 0.2s cubic-bezier(0.16, 1, 0.3, 1);
      position: relative;
      font-family: var(--font-heading);
      font-size: 13px;
      font-weight: 600;
    }

    .config-nav-btn:hover {
      background: rgba(255, 255, 255, 0.05);
      color: var(--text);
      border-color: rgba(255, 255, 255, 0.08);
      transform: translateX(3px);
    }
    [data-theme="light"] .config-nav-btn:hover {
      background: #f1f5f9;
      color: #0f172a;
      border-color: #e2e8f0;
    }

    .config-nav-btn.active {
      background: linear-gradient(135deg, rgba(255, 107, 0, 0.16) 0%, rgba(255, 107, 0, 0.06) 100%);
      border: 1px solid rgba(255, 107, 0, 0.35);
      color: #ff8533;
      font-weight: 700;
      box-shadow: 0 4px 14px rgba(255, 107, 0, 0.15);
    }
    [data-theme="light"] .config-nav-btn.active {
      background: linear-gradient(135deg, rgba(234, 88, 12, 0.12) 0%, rgba(234, 88, 12, 0.04) 100%);
      border-color: rgba(234, 88, 12, 0.35);
      color: #ea580c;
    }

    .cfg-icon {
      font-size: 16px;
      width: 20px;
      text-align: center;
      flex-shrink: 0;
    }

    .cfg-text {
      flex: 1;
    }

    .cfg-badge {
      font-family: var(--font-mono);
      font-size: 10.5px;
      font-weight: 700;
      padding: 2px 7px;
      border-radius: 100px;
      background: rgba(255, 255, 255, 0.08);
      color: var(--text-dim);
    }
    .config-nav-btn.active .cfg-badge {
      background: rgba(255, 107, 0, 0.25);
      color: #ff8533;
    }

    .config-sidebar-footer {
      margin-top: 6px;
      padding-top: 10px;
      border-top: 1px solid rgba(255, 255, 255, 0.06);
    }
    [data-theme="light"] .config-sidebar-footer {
      border-top: 1px solid rgba(0, 0, 0, 0.06);
    }

    .cfg-quick-sync-box {
      background: rgba(0, 0, 0, 0.2);
      border: 1px solid rgba(255, 255, 255, 0.06);
      border-radius: 10px;
      padding: 11px;
    }
    [data-theme="light"] .cfg-quick-sync-box {
      background: #f8fafc;
      border-color: #e2e8f0;
    }

    .config-main {
      flex: 1;
      min-width: 0;
      display: flex;
      flex-direction: column;
      gap: 20px;
    }

    .config-pane {
      display: none;
      animation: fadeInPane 0.25s ease forwards;
    }
    .config-pane.active {
      display: flex;
      flex-direction: column;
      gap: 20px;
    }

    @keyframes fadeInPane {
      from { opacity: 0; transform: translateY(8px); }
      to { opacity: 1; transform: translateY(0); }
    }

    .config-card-grid-2 {
      display: grid;
      grid-template-columns: repeat(auto-fit, minmax(350px, 1fr));
      gap: 20px;
    }

    .config-form-group {
      margin-bottom: 16px;
    }
    .config-form-group label {
      display: block;
      font-size: 11px;
      font-weight: 700;
      color: var(--text-muted);
      text-transform: uppercase;
      letter-spacing: 0.7px;
      margin-bottom: 6px;
    }
    .config-form-group .form-help {
      font-size: 11px;
      color: var(--text-dim);
      margin-top: 5px;
      display: block;
      line-height: 1.4;
    }

    .config-action-bar {
      display: flex;
      justify-content: flex-end;
      align-items: center;
      gap: 12px;
      padding: 16px 22px;
      background: linear-gradient(145deg, rgba(255, 255, 255, 0.03) 0%, rgba(15, 23, 42, 0.5) 100%);
      border: 1px solid rgba(255, 255, 255, 0.08);
      border-radius: 14px;
    }
    [data-theme="light"] .config-action-bar {
      background: #ffffff;
      border-color: rgba(0, 0, 0, 0.08);
    }

    .config-rule-box {
      background: rgba(0, 0, 0, 0.2);
      border: 1px solid rgba(255, 255, 255, 0.06);
      border-radius: 12px;
      padding: 16px;
      margin-bottom: 14px;
      display: flex;
      flex-direction: column;
      gap: 8px;
    }
    [data-theme="light"] .config-rule-box {
      background: #f8fafc;
      border-color: #e2e8f0;
    }

    .rule-title-row {
      display: flex;
      align-items: center;
      justify-content: space-between;
    }
    .rule-title {
      font-size: 13px;
      font-weight: 700;
      color: var(--text);
      display: flex;
      align-items: center;
      gap: 8px;
    }

    /* Permission Matrix Grid */
    .perm-grid-table {
      width: 100%;
      border-collapse: separate;
      border-spacing: 0;
    }
    .perm-grid-table th {
      text-align: center;
      font-size: 11px;
    }
    .perm-grid-table th:first-child {
      text-align: left;
    }
    .perm-grid-table td {
      text-align: center;
      vertical-align: middle;
    }
    .perm-grid-table td:first-child {
      text-align: left;
    }
    .perm-check {
      width: 17px;
      height: 17px;
      accent-color: var(--primary);
      cursor: pointer;
    }

    @media (max-width: 1080px) {
      .config-layout {
        flex-direction: column;
      }
      .config-sidebar {
        width: 100%;
        min-width: 100%;
        position: static;
      }
      .config-nav {
        display: grid;
        grid-template-columns: repeat(auto-fill, minmax(200px, 1fr));
      }
    }
`;

// 2. HTML CONTENT FOR VIEW-CONFIGURATION TO INJECT BEFORE </main>
const htmlViewConfiguration = `
        <!-- ===================================================================
             VIEW 9: FULL-SIZE CONFIGURATION WITH DEDICATED INNER SIDEBAR
             =================================================================== -->
        <section id="view-configuration" class="content-view">
          <div class="config-layout">
            <!-- INNER SIDEBAR FOR CONFIGURATION -->
            <aside class="config-sidebar">
              <div class="config-sidebar-header">
                <div class="config-sidebar-tag">Enterprise MIS Engine</div>
                <h3 class="config-sidebar-heading">Configuration</h3>
                <p class="config-sidebar-desc">Master settings, branches, shifts, leave policies, departments & RBAC</p>
              </div>

              <nav class="config-nav">
                <button class="config-nav-btn active" data-cfg-pane="general" onclick="switchConfigTab('general', this)">
                  <span class="cfg-icon">🏢</span>
                  <span class="cfg-text">General</span>
                </button>
                <button class="config-nav-btn" data-cfg-pane="system" onclick="switchConfigTab('system', this)">
                  <span class="cfg-icon">⚡</span>
                  <span class="cfg-text">System</span>
                </button>
                <button class="config-nav-btn" data-cfg-pane="branches" onclick="switchConfigTab('branches', this)">
                  <span class="cfg-icon">📍</span>
                  <span class="cfg-text">Branches</span>
                  <span class="cfg-badge" id="cfg-branch-count">3</span>
                </button>
                <button class="config-nav-btn" data-cfg-pane="shifts" onclick="switchConfigTab('shifts', this)">
                  <span class="cfg-icon">⏱️</span>
                  <span class="cfg-text">Shift Configurations</span>
                  <span class="cfg-badge" id="cfg-shift-count">4</span>
                </button>
                <button class="config-nav-btn" data-cfg-pane="leaves" onclick="switchConfigTab('leaves', this)">
                  <span class="cfg-icon">🏖️</span>
                  <span class="cfg-text">Leave Configuration</span>
                  <span class="cfg-badge" id="cfg-leave-count">5</span>
                </button>
                <button class="config-nav-btn" data-cfg-pane="rules" onclick="switchConfigTab('rules', this)">
                  <span class="cfg-icon">⚖️</span>
                  <span class="cfg-text">Rules</span>
                </button>
                <button class="config-nav-btn" data-cfg-pane="departments" onclick="switchConfigTab('departments', this)">
                  <span class="cfg-icon">🏛️</span>
                  <span class="cfg-text">Department</span>
                  <span class="cfg-badge" id="cfg-dept-count">6</span>
                </button>
                <button class="config-nav-btn" data-cfg-pane="designations" onclick="switchConfigTab('designations', this)">
                  <span class="cfg-icon">💼</span>
                  <span class="cfg-text">Designation</span>
                  <span class="cfg-badge" id="cfg-desig-count">8</span>
                </button>
                <button class="config-nav-btn" data-cfg-pane="holidays" onclick="switchConfigTab('holidays', this)">
                  <span class="cfg-icon">🗓️</span>
                  <span class="cfg-text">Holidays</span>
                  <span class="cfg-badge" id="cfg-holiday-count">12</span>
                </button>
                <button class="config-nav-btn" data-cfg-pane="roles" onclick="switchConfigTab('roles', this)">
                  <span class="cfg-icon">🛡️</span>
                  <span class="cfg-text">Roles and Permissions</span>
                  <span class="cfg-badge" id="cfg-role-count">5</span>
                </button>
              </nav>

              <div class="config-sidebar-footer">
                <div class="cfg-quick-sync-box">
                  <div style="display:flex; align-items:center; justify-content:space-between; margin-bottom:6px;">
                    <span style="font-size:11px; font-weight:700; color:var(--text-dim); text-transform:uppercase; letter-spacing:0.8px;">SYSTEM INTEGRITY</span>
                    <span class="beacon-dot"></span>
                  </div>
                  <div style="font-size:11.5px; color:var(--text-muted); margin-bottom:10px;">Cloud & Local State in Sync</div>
                  <button type="button" class="btn btn-primary btn-sm" style="width:100%; justify-content:center;" onclick="saveCurrentConfigSection()">
                    💾 Save Active Section
                  </button>
                </div>
              </div>
            </aside>

            <!-- MAIN WORKSPACE FOR ACTIVE CONFIGURATION PANE -->
            <main class="config-main">

              <!-- ===============================================================
                   SECTION 1: GENERAL CONFIGURATION
                   =============================================================== -->
              <div class="config-pane active" id="cfg-pane-general">
                <div class="card">
                  <div class="card-header">
                    <div class="card-title-group">
                      <h3 class="card-title">🏢 Enterprise Identity & Company Profile</h3>
                      <p class="card-subtitle">Master organization details, headquarters registration & branding</p>
                    </div>
                    <span class="badge badge-primary">ACTIVE PROFILE</span>
                  </div>
                  <div style="padding:22px;">
                    <div class="config-card-grid-2">
                      <div class="config-form-group">
                        <label>Company / Organization Name</label>
                        <input type="text" id="cfg-gen-company-name" class="form-input" placeholder="e.g. Anurag Attendance & Biometrics">
                        <span class="form-help">Displayed across all muster rolls, topbars, wage slips & attendance printouts</span>
                      </div>
                      <div class="config-form-group">
                        <label>Corporate Entity Code / Short Tag</label>
                        <input type="text" id="cfg-gen-company-code" class="form-input" placeholder="e.g. AAB-CORP">
                        <span class="form-help">Prefix used for employee biometric enrollment and ledger exports</span>
                      </div>
                      <div class="config-form-group">
                        <label>Portal Tagline / Mission Header</label>
                        <input type="text" id="cfg-gen-tagline" class="form-input" placeholder="Enterprise 24/7 Biometric Cloud MIS & Workforce Suite">
                      </div>
                      <div class="config-form-group">
                        <label>Administrator / HR Contact Email</label>
                        <input type="text" id="cfg-gen-email" class="form-input" placeholder="admin@anuragbiometrics.com">
                      </div>
                      <div class="config-form-group">
                        <label>Official Contact / Helpline Phone</label>
                        <input type="text" id="cfg-gen-phone" class="form-input" placeholder="+91 98765 43210">
                      </div>
                      <div class="config-form-group">
                        <label>Headquarters Physical Address</label>
                        <input type="text" id="cfg-gen-address" class="form-input" placeholder="HITEC City, Cyber Towers, Hyderabad, TG 500081">
                      </div>
                    </div>
                  </div>
                </div>

                <div class="card">
                  <div class="card-header">
                    <div class="card-title-group">
                      <h3 class="card-title">🌐 Regional & Operational Standards</h3>
                      <p class="card-subtitle">Timezone synchronization, work calendar, base currency & fiscal cycle</p>
                    </div>
                  </div>
                  <div style="padding:22px;">
                    <div class="config-card-grid-2">
                      <div class="config-form-group">
                        <label>Operational Timezone</label>
                        <select id="cfg-gen-timezone" class="form-input">
                          <option value="Asia/Kolkata">Asia/Kolkata (IST +05:30) - Standard</option>
                          <option value="Asia/Dubai">Asia/Dubai (GST +04:00)</option>
                          <option value="Asia/Singapore">Asia/Singapore (SGT +08:00)</option>
                          <option value="Europe/London">Europe/London (GMT/BST +00:00)</option>
                          <option value="America/New_York">America/New_York (EST -05:00)</option>
                        </select>
                        <span class="form-help">Used to align all R503 sensor biometric punch stamps with real world clock</span>
                      </div>
                      <div class="config-form-group">
                        <label>Base Currency Code & Symbol</label>
                        <select id="cfg-gen-currency" class="form-input">
                          <option value="INR (₹)">Indian Rupee - INR (₹)</option>
                          <option value="USD ($)">US Dollar - USD ($)</option>
                          <option value="EUR (€)">Euro - EUR (€)</option>
                          <option value="AED (د.إ)">UAE Dirham - AED (د.إ)</option>
                          <option value="GBP (£)">British Pound - GBP (£)</option>
                        </select>
                      </div>
                      <div class="config-form-group">
                        <label>Standard Working Days per Week</label>
                        <select id="cfg-gen-workdays" class="form-input">
                          <option value="6">6 Days (Monday through Saturday, Sunday Off)</option>
                          <option value="5">5 Days (Monday through Friday, Weekend Off)</option>
                          <option value="5.5">5.5 Days (Monday to Friday + Alternate Saturdays)</option>
                          <option value="7">7 Days (24/7 Continuous Shift Operations)</option>
                        </select>
                      </div>
                      <div class="config-form-group">
                        <label>Fiscal Year Cycle Start</label>
                        <select id="cfg-gen-fiscal" class="form-input">
                          <option value="April">April 1st to March 31st (Standard Indian FY)</option>
                          <option value="January">January 1st to December 31st (Calendar Year)</option>
                          <option value="July">July 1st to June 30th</option>
                        </select>
                      </div>
                    </div>
                  </div>
                </div>

                <div class="config-action-bar">
                  <span style="font-size:12px; color:var(--text-dim); margin-right:auto;">All changes will update global headers and payroll wage slips immediately.</span>
                  <button type="button" class="btn btn-outline" onclick="resetGeneralConfigDefaults()">Reset Defaults</button>
                  <button type="button" class="btn btn-primary" onclick="saveGeneralConfig()">💾 Save General Settings</button>
                </div>
              </div>

              <!-- ===============================================================
                   SECTION 2: SYSTEM CONFIGURATION
                   =============================================================== -->
              <div class="config-pane" id="cfg-pane-system">
                <div class="card">
                  <div class="card-header">
                    <div class="card-title-group">
                      <h3 class="card-title">🔥 Firebase Realtime Database & Cloud Link</h3>
                      <p class="card-subtitle">Live cloud sync engine, endpoint authorization & realtime listener</p>
                    </div>
                    <span class="badge badge-success" id="cfg-sys-fb-status">LINKED</span>
                  </div>
                  <div style="padding:22px;">
                    <div class="config-form-group">
                      <label>Firebase Realtime Database URL</label>
                      <div style="display:flex; gap:10px;">
                        <input type="text" id="cfg-sys-fb-url" class="form-input" style="flex:1;" placeholder="https://chronosmis-default-rtdb.firebaseio.com">
                        <button type="button" class="btn btn-outline btn-sm" onclick="testConfigFirebaseLink()">🔍 Test Link</button>
                        <button type="button" class="btn btn-primary btn-sm" onclick="syncFromConfigFirebaseLink()">⚡ Sync Now</button>
                      </div>
                      <span class="form-help">Enter your Firebase Realtime Database URL. You can also paste your project console URL directly.</span>
                    </div>

                    <div class="config-card-grid-2">
                      <div class="config-form-group">
                        <label>Database Auth Token / Secret (Optional)</label>
                        <input type="password" id="cfg-sys-fb-auth" class="form-input" placeholder="Leave empty for public read/write rules">
                        <span class="form-help">Database secret for authenticated Firebase REST write operations</span>
                      </div>
                      <div class="config-form-group">
                        <label>Public Web Hosting Domain</label>
                        <input type="text" id="cfg-sys-fb-host" class="form-input" value="https://chronosmis.web.app" readonly style="opacity:0.75;">
                        <span class="form-help">Deployed production domain running 24/7 on Firebase Hosting</span>
                      </div>
                    </div>

                    <div style="background:rgba(255,255,255,0.03); border:1px solid var(--border); border-radius:10px; padding:12px 16px; font-size:12px; color:var(--text-muted); display:flex; justify-content:space-between; align-items:center; margin-top:8px;">
                      <span>🔒 Database Security Rules snippet: <code>{ "rules": { ".read": true, ".write": true } }</code></span>
                      <button type="button" class="btn btn-outline btn-sm" onclick="copyFirebaseRulesSnippet()" style="font-size:11px;">📋 Copy Rules JSON</button>
                    </div>
                  </div>
                </div>

                <div class="card">
                  <div class="card-header">
                    <div class="card-title-group">
                      <h3 class="card-title">📡 ESP32 Biometric Terminal & Gateway</h3>
                      <p class="card-subtitle">Local sensor IP gateway, hardware polling daemon & transfer parameters</p>
                    </div>
                    <span class="badge badge-accent">ESP32 R503</span>
                  </div>
                  <div style="padding:22px;">
                    <div class="config-card-grid-2">
                      <div class="config-form-group">
                        <label>Default Hardware Device IP Address</label>
                        <div style="display:flex; gap:8px;">
                          <input type="text" id="cfg-sys-device-ip" class="form-input" style="flex:1;" placeholder="192.168.0.108">
                          <button type="button" class="btn btn-outline btn-sm" onclick="testConfigDeviceIp()">📡 Ping Device</button>
                        </div>
                        <span class="form-help">Static or DHCP IP of the ESP32 controller on your local Wi-Fi router</span>
                      </div>
                      <div class="config-form-group">
                        <label>Device Gateway Port</label>
                        <input type="number" id="cfg-sys-device-port" class="form-input" value="80">
                        <span class="form-help">Default HTTP port for REST biometrics template exchange</span>
                      </div>
                      <div class="config-form-group">
                        <label>Cloud Background Sync Interval</label>
                        <select id="cfg-sys-sync-interval" class="form-input">
                          <option value="5">5 Seconds (Ultra Realtime)</option>
                          <option value="10" selected>10 Seconds (Recommended)</option>
                          <option value="30">30 Seconds (Standard)</option>
                          <option value="60">60 Seconds (Low Bandwidth)</option>
                        </select>
                      </div>
                      <div class="config-form-group">
                        <label>Punch Verification Audio Chime</label>
                        <select id="cfg-sys-sound" class="form-input">
                          <option value="true">Enabled (Audible chime on verified fingerprint/RFID)</option>
                          <option value="false">Muted (Silent logging)</option>
                        </select>
                      </div>
                    </div>
                  </div>
                </div>

                <div class="card">
                  <div class="card-header">
                    <div class="card-title-group">
                      <h3 class="card-title">🔐 Portal Access & Visual Theme</h3>
                      <p class="card-subtitle">Security credentials, session timeouts and appearance mode</p>
                    </div>
                  </div>
                  <div style="padding:22px;">
                    <div class="config-card-grid-2">
                      <div class="config-form-group">
                        <label>Color Theme Mode</label>
                        <select id="cfg-sys-theme" class="form-input" onchange="previewTheme(this.value)">
                          <option value="dark">Dark Slate Luxury Glassmorphism (Default)</option>
                          <option value="light">Crisp Modern Light High-Contrast</option>
                        </select>
                      </div>
                      <div class="config-form-group">
                        <label>Master Administrator PIN / Password</label>
                        <input type="text" id="cfg-sys-pwd" class="form-input" value="admin123">
                        <span class="form-help">Used to unlock restricted modules and settings updates</span>
                      </div>
                    </div>
                    <div style="display:flex; justify-content:space-between; align-items:center; padding-top:14px; border-top:1px solid rgba(255,255,255,0.06); margin-top:8px;">
                      <span style="font-size:12px; color:var(--text-muted);">Current Security Session: <b style="color:var(--success);">Active (Admin Authenticated)</b></span>
                      <button type="button" class="btn btn-danger btn-sm" onclick="lockPortalSession()">🔒 Lock Portal Session Now</button>
                    </div>
                  </div>
                </div>

                <div class="config-action-bar">
                  <button type="button" class="btn btn-primary" onclick="saveSystemConfig()">💾 Save System Parameters</button>
                </div>
              </div>

              <!-- ===============================================================
                   SECTION 3: BRANCHES CONFIGURATION
                   =============================================================== -->
              <div class="config-pane" id="cfg-pane-branches">
                <!-- KPI CARDS -->
                <div class="kpi-grid" style="margin-bottom:0;">
                  <div class="kpi-card" style="--kpi-accent:#ff6b00;">
                    <div class="kpi-header"><span class="kpi-label">Total Registered Branches</span><span class="kpi-icon-badge">📍</span></div>
                    <div class="kpi-value" id="cfg-kpi-branches-total">3</div>
                    <div class="kpi-sub">Across 3 industrial zones</div>
                  </div>
                  <div class="kpi-card" style="--kpi-accent:#10b981;">
                    <div class="kpi-header"><span class="kpi-label">Active Operational Branches</span><span class="kpi-icon-badge">✅</span></div>
                    <div class="kpi-value" style="color:#10b981;">3</div>
                    <div class="kpi-sub">100% Online & biometrically synced</div>
                  </div>
                  <div class="kpi-card" style="--kpi-accent:#4f46e5;">
                    <div class="kpi-header"><span class="kpi-label">Total Assigned Headcount</span><span class="kpi-icon-badge">👥</span></div>
                    <div class="kpi-value" id="cfg-kpi-branches-headcount">20</div>
                    <div class="kpi-sub">Biometrically mapped staff</div>
                  </div>
                  <div class="kpi-card" style="--kpi-accent:#06b6d4;">
                    <div class="kpi-header"><span class="kpi-label">Primary HQ Hub</span><span class="kpi-icon-badge">🏢</span></div>
                    <div class="kpi-value" style="font-size:22px; color:#06b6d4;">HQ-HYD</div>
                    <div class="kpi-sub">Hyderabad Central Command</div>
                  </div>
                </div>

                <div class="card">
                  <div class="card-header">
                    <div class="card-title-group">
                      <h3 class="card-title">📍 Company Branches & Regional Workplaces</h3>
                      <p class="card-subtitle">Configure branch offices, manufacturing plants, contact persons & local network subnets</p>
                    </div>
                    <button type="button" class="btn btn-primary btn-sm" onclick="openAddBranchModal()">➕ Add New Branch</button>
                  </div>
                  <div class="table-container">
                    <table>
                      <thead>
                        <tr>
                          <th>Branch Code</th>
                          <th>Branch Name</th>
                          <th>Location / City</th>
                          <th>Contact Person</th>
                          <th>Phone</th>
                          <th>Network IP Subnet</th>
                          <th>Staff</th>
                          <th>Status</th>
                          <th style="text-align:right;">Actions</th>
                        </tr>
                      </thead>
                      <tbody id="cfg-branches-table-body">
                        <!-- Rendered dynamically -->
                      </tbody>
                    </table>
                  </div>
                </div>
              </div>

              <!-- ===============================================================
                   SECTION 4: SHIFT CONFIGURATIONS
                   =============================================================== -->
              <div class="config-pane" id="cfg-pane-shifts">
                <!-- KPI CARDS -->
                <div class="kpi-grid" style="margin-bottom:0;">
                  <div class="kpi-card" style="--kpi-accent:#ff6b00;">
                    <div class="kpi-header"><span class="kpi-label">Defined Shift Rosters</span><span class="kpi-icon-badge">⏱️</span></div>
                    <div class="kpi-value" id="cfg-kpi-shifts-total">4</div>
                    <div class="kpi-sub">24/7 Plant & Office Coverage</div>
                  </div>
                  <div class="kpi-card" style="--kpi-accent:#10b981;">
                    <div class="kpi-header"><span class="kpi-label">Standard Work Hours</span><span class="kpi-icon-badge">⌛</span></div>
                    <div class="kpi-value" style="color:#10b981;">8.0 hrs</div>
                    <div class="kpi-sub">Standard shift span per day</div>
                  </div>
                  <div class="kpi-card" style="--kpi-accent:#4f46e5;">
                    <div class="kpi-header"><span class="kpi-label">Grace Period Allowed</span><span class="kpi-icon-badge">🕒</span></div>
                    <div class="kpi-value" id="cfg-kpi-shifts-grace">15 mins</div>
                    <div class="kpi-sub">Before marking late arrival</div>
                  </div>
                  <div class="kpi-card" style="--kpi-accent:#06b6d4;">
                    <div class="kpi-header"><span class="kpi-label">Night Shift Allowance</span><span class="kpi-icon-badge">🌙</span></div>
                    <div class="kpi-value" style="color:#06b6d4;">₹250/day</div>
                    <div class="kpi-sub">Automated wage differential</div>
                  </div>
                </div>

                <div class="card">
                  <div class="card-header">
                    <div class="card-title-group">
                      <h3 class="card-title">⏱️ Shift Timings, Rosters & Grace Intervals</h3>
                      <p class="card-subtitle">Manage morning, evening, night and general work shifts with automated grace calculations</p>
                    </div>
                    <button type="button" class="btn btn-primary btn-sm" onclick="openAddShiftModal()">➕ Create New Shift</button>
                  </div>
                  <div class="table-container">
                    <table>
                      <thead>
                        <tr>
                          <th>Code</th>
                          <th>Shift Name</th>
                          <th>Timings</th>
                          <th>Duration</th>
                          <th>Grace Period</th>
                          <th>Half-Day Min</th>
                          <th>Break Time</th>
                          <th>Allowance</th>
                          <th>Status</th>
                          <th style="text-align:right;">Actions</th>
                        </tr>
                      </thead>
                      <tbody id="cfg-shifts-table-body">
                        <!-- Rendered dynamically -->
                      </tbody>
                    </table>
                  </div>
                </div>
              </div>

              <!-- ===============================================================
                   SECTION 5: LEAVE CONFIGURATION
                   =============================================================== -->
              <div class="config-pane" id="cfg-pane-leaves">
                <!-- KPI CARDS -->
                <div class="kpi-grid" style="margin-bottom:0;">
                  <div class="kpi-card" style="--kpi-accent:#ff6b00;">
                    <div class="kpi-header"><span class="kpi-label">Leave Categories</span><span class="kpi-icon-badge">🏖️</span></div>
                    <div class="kpi-value" id="cfg-kpi-leaves-total">5</div>
                    <div class="kpi-sub">CL, SL, PL, ML, LWP</div>
                  </div>
                  <div class="kpi-card" style="--kpi-accent:#10b981;">
                    <div class="kpi-header"><span class="kpi-label">Annual Paid Quota</span><span class="kpi-icon-badge">📅</span></div>
                    <div class="kpi-value" style="color:#10b981;">37 Days</div>
                    <div class="kpi-sub">Total annual paid entitlement</div>
                  </div>
                  <div class="kpi-card" style="--kpi-accent:#f59e0b;">
                    <div class="kpi-header"><span class="kpi-label">Sandwiched Rule</span><span class="kpi-icon-badge">🥪</span></div>
                    <div class="kpi-value" style="color:#f59e0b; font-size:24px;">ACTIVE</div>
                    <div class="kpi-sub">Holidays in leave counted</div>
                  </div>
                  <div class="kpi-card" style="--kpi-accent:#4f46e5;">
                    <div class="kpi-header"><span class="kpi-label">Encashable Leaves</span><span class="kpi-icon-badge">💰</span></div>
                    <div class="kpi-value" style="color:#4f46e5; font-size:24px;">PL Only</div>
                    <div class="kpi-sub">At year end wage rate</div>
                  </div>
                </div>

                <div class="card">
                  <div class="card-header">
                    <div class="card-title-group">
                      <h3 class="card-title">🏖️ Leave Types & Annual Entitlements</h3>
                      <p class="card-subtitle">Manage leave quotas, carry-forward criteria, paid status & encashment policies</p>
                    </div>
                    <button type="button" class="btn btn-primary btn-sm" onclick="openAddLeaveModal()">➕ Add Leave Type</button>
                  </div>
                  <div class="table-container">
                    <table>
                      <thead>
                        <tr>
                          <th>Code</th>
                          <th>Leave Type Name</th>
                          <th>Annual Quota</th>
                          <th>Carry Forward</th>
                          <th>Encashable</th>
                          <th>Type</th>
                          <th>Requires HR Approval</th>
                          <th style="text-align:right;">Actions</th>
                        </tr>
                      </thead>
                      <tbody id="cfg-leaves-table-body">
                        <!-- Rendered dynamically -->
                      </tbody>
                    </table>
                  </div>
                </div>

                <div class="card">
                  <div class="card-header">
                    <div class="card-title-group">
                      <h3 class="card-title">📜 Statutory & Company Leave Policies</h3>
                      <p class="card-subtitle">Rules governing consecutive leaves, sandwich policies & probation access</p>
                    </div>
                  </div>
                  <div style="padding:22px;">
                    <div class="config-card-grid-2">
                      <div class="config-rule-box">
                        <div class="rule-title-row">
                          <span class="rule-title">🥪 Sandwiched Leave Policy</span>
                          <input type="checkbox" id="cfg-leave-sandwich" class="perm-check" checked>
                        </div>
                        <p style="font-size:12px; color:var(--text-dim); margin-top:4px;">
                          When enabled: If a public holiday or weekly off (Sunday) falls between two approved leave days, it will be deducted as leave.
                        </p>
                      </div>

                      <div class="config-rule-box">
                        <div class="rule-title-row">
                          <span class="rule-title">⏱️ Maximum Consecutive Leave Limit</span>
                          <input type="number" id="cfg-leave-max-consec" class="form-input" style="width:80px; text-align:center;" value="10">
                        </div>
                        <p style="font-size:12px; color:var(--text-dim); margin-top:4px;">
                          Maximum consecutive days an employee can take without formal Director sign-off.
                        </p>
                      </div>

                      <div class="config-rule-box">
                        <div class="rule-title-row">
                          <span class="rule-title">📋 Advance Notice for Privilege Leave (Days)</span>
                          <input type="number" id="cfg-leave-notice-days" class="form-input" style="width:80px; text-align:center;" value="7">
                        </div>
                        <p style="font-size:12px; color:var(--text-dim); margin-top:4px;">
                          Minimum advance application notice required for planned annual leaves.
                        </p>
                      </div>

                      <div class="config-rule-box">
                        <div class="rule-title-row">
                          <span class="rule-title">🌱 Probation Period Leave Access</span>
                          <input type="checkbox" id="cfg-leave-probation" class="perm-check">
                        </div>
                        <p style="font-size:12px; color:var(--text-dim); margin-top:4px;">
                          Allow employees during their first 90 days of probation to take paid casual leaves.
                        </p>
                      </div>
                    </div>
                  </div>
                </div>

                <div class="config-action-bar">
                  <button type="button" class="btn btn-primary" onclick="saveLeavePolicies()">💾 Save Leave Policies</button>
                </div>
              </div>

              <!-- ===============================================================
                   SECTION 6: RULES CONFIGURATION
                   =============================================================== -->
              <div class="config-pane" id="cfg-pane-rules">
                <div class="card">
                  <div class="card-header">
                    <div class="card-title-group">
                      <h3 class="card-title">⏰ Late Arrival & Early Departure Penalty Engine</h3>
                      <p class="card-subtitle">Automated wage deduction rules for punctuality compliance</p>
                    </div>
                    <span class="badge badge-warning">PUNCTUALITY ENGINE</span>
                  </div>
                  <div style="padding:22px;">
                    <div class="config-card-grid-2">
                      <div class="config-form-group">
                        <label>Late Arrival Grace Period (Minutes)</label>
                        <input type="number" id="cfg-rule-late-grace" class="form-input" value="15">
                        <span class="form-help">Punches within this window after shift start are counted as On-Time</span>
                      </div>

                      <div class="config-form-group">
                        <label>Late Mark Penalty Rule</label>
                        <select id="cfg-rule-late-action" class="form-input">
                          <option value="3_LATE_HALF_DAY">3 Late marks in month = 0.5 Day wage cut (Recommended)</option>
                          <option value="2_LATE_HALF_DAY">2 Late marks in month = 0.5 Day wage cut (Strict)</option>
                          <option value="PRORATED_HOURLY">Prorated hourly cut per minute of delay</option>
                          <option value="NONE">Warning flag only (No salary deduction)</option>
                        </select>
                        <span class="form-help">Calculated automatically during monthly payroll run</span>
                      </div>

                      <div class="config-form-group">
                        <label>Early Departure Grace Period (Minutes)</label>
                        <input type="number" id="cfg-rule-early-grace" class="form-input" value="15">
                        <span class="form-help">Leaving before shift end minus grace will flag early exit</span>
                      </div>

                      <div class="config-form-group">
                        <label>Early Departure Penalty Action</label>
                        <select id="cfg-rule-early-action" class="form-input">
                          <option value="HALF_DAY">Mark as Half Day if departure &gt; 1 hour early</option>
                          <option value="HOURLY_CUT">Deduct equivalent hourly rate</option>
                          <option value="FLAG_ONLY">Flag for supervisor approval only</option>
                        </select>
                      </div>
                    </div>
                  </div>
                </div>

                <div class="card">
                  <div class="card-header">
                    <div class="card-title-group">
                      <h3 class="card-title">⌛ Daily Working Hours & Overtime (OT) Engine</h3>
                      <p class="card-subtitle">Presence thresholds, overtime rate multipliers & half-day minimums</p>
                    </div>
                  </div>
                  <div style="padding:22px;">
                    <div class="config-card-grid-2">
                      <div class="config-form-group">
                        <label>Minimum Working Hours for Full-Day (Hours)</label>
                        <input type="number" step="0.5" id="cfg-rule-min-full" class="form-input" value="8.0">
                        <span class="form-help">Minimum net hours required to earn full 100% daily wage</span>
                      </div>

                      <div class="config-form-group">
                        <label>Minimum Working Hours for Half-Day (Hours)</label>
                        <input type="number" step="0.5" id="cfg-rule-min-half" class="form-input" value="4.5">
                        <span class="form-help">Working less than this threshold automatically marks Absent / LOP</span>
                      </div>

                      <div class="config-form-group">
                        <label>Overtime Minimum Trigger Threshold (Minutes)</label>
                        <input type="number" id="cfg-rule-ot-thresh" class="form-input" value="30">
                        <span class="form-help">Punches beyond shift end by this duration start accumulating OT</span>
                      </div>

                      <div class="config-form-group">
                        <label>Standard Weekday Overtime Multiplier</label>
                        <input type="number" step="0.1" id="cfg-rule-ot-rate" class="form-input" value="1.5">
                        <span class="form-help">e.g. 1.5x of base hourly wage (Base daily wage / 8)</span>
                      </div>

                      <div class="config-form-group">
                        <label>Weekend & Holiday Overtime Multiplier</label>
                        <input type="number" step="0.1" id="cfg-rule-ot-weekend-rate" class="form-input" value="2.0">
                        <span class="form-help">Double wage multiplier for Sunday / Gazetted holiday shifts</span>
                      </div>

                      <div class="config-form-group">
                        <label>Maximum Daily Overtime Cap (Hours)</label>
                        <input type="number" step="0.5" id="cfg-rule-ot-max" class="form-input" value="4.0">
                        <span class="form-help">Statutory health limit on maximum daily overtime accumulation</span>
                      </div>
                    </div>
                  </div>
                </div>

                <div class="card">
                  <div class="card-header">
                    <div class="card-title-group">
                      <h3 class="card-title">🤖 Single Punch & Exception Automation</h3>
                      <p class="card-subtitle">Automated handling for missing punches, forgotten punch-outs & midnight rollover</p>
                    </div>
                  </div>
                  <div style="padding:22px;">
                    <div class="config-card-grid-2">
                      <div class="config-form-group">
                        <label>Single Punch (IN without OUT) Handling</label>
                        <select id="cfg-rule-single-punch" class="form-input">
                          <option value="MARK_HALF_DAY">Mark as Half Day Presence (Standard)</option>
                          <option value="AUTO_CHECKOUT">Auto-checkout at shift end time with Warning Flag</option>
                          <option value="MARK_ABSENT">Mark as Absent pending HR manual punch punch-in</option>
                        </select>
                      </div>

                      <div class="config-form-group">
                        <label>Daily Ledger Auto-Closure Time</label>
                        <input type="time" id="cfg-rule-auto-checkout-time" class="form-input" value="23:59">
                        <span class="form-help">Punches received after this timestamp belong to the next calendar cycle</span>
                      </div>
                    </div>
                  </div>
                </div>

                <div class="config-action-bar">
                  <button type="button" class="btn btn-outline" onclick="resetRulesDefaults()">Reset Standard Rules</button>
                  <button type="button" class="btn btn-primary" onclick="saveRulesConfig()">💾 Save Rules Configuration</button>
                </div>
              </div>

              <!-- ===============================================================
                   SECTION 7: DEPARTMENT CONFIGURATION
                   =============================================================== -->
              <div class="config-pane" id="cfg-pane-departments">
                <!-- KPI CARDS -->
                <div class="kpi-grid" style="margin-bottom:0;">
                  <div class="kpi-card" style="--kpi-accent:#ff6b00;">
                    <div class="kpi-header"><span class="kpi-label">Total Departments</span><span class="kpi-icon-badge">🏛️</span></div>
                    <div class="kpi-value" id="cfg-kpi-depts-total">6</div>
                    <div class="kpi-sub">Operational functional units</div>
                  </div>
                  <div class="kpi-card" style="--kpi-accent:#10b981;">
                    <div class="kpi-header"><span class="kpi-label">Mapped Employees</span><span class="kpi-icon-badge">👥</span></div>
                    <div class="kpi-value" id="cfg-kpi-depts-staff">20</div>
                    <div class="kpi-sub">100% staff assigned to units</div>
                  </div>
                  <div class="kpi-card" style="--kpi-accent:#4f46e5;">
                    <div class="kpi-header"><span class="kpi-label">Largest Department</span><span class="kpi-icon-badge">⚡</span></div>
                    <div class="kpi-value" style="font-size:22px; color:#4f46e5;">Engineering (6)</div>
                    <div class="kpi-sub">Software & Embedded Hardware</div>
                  </div>
                  <div class="kpi-card" style="--kpi-accent:#06b6d4;">
                    <div class="kpi-header"><span class="kpi-label">Active HODs</span><span class="kpi-icon-badge">👔</span></div>
                    <div class="kpi-value" style="color:#06b6d4;">6</div>
                    <div class="kpi-sub">Designated Department Heads</div>
                  </div>
                </div>

                <div class="card">
                  <div class="card-header">
                    <div class="card-title-group">
                      <h3 class="card-title">🏛️ Department Master Directory</h3>
                      <p class="card-subtitle">Manage organizational divisions, department codes, heads of department & staffing</p>
                    </div>
                    <button type="button" class="btn btn-primary btn-sm" onclick="openAddDeptModal()">➕ Add Department</button>
                  </div>
                  <div class="table-container">
                    <table>
                      <thead>
                        <tr>
                          <th>Code</th>
                          <th>Department Name</th>
                          <th>Department Head (HOD)</th>
                          <th>Staff Count</th>
                          <th>Floor / Location</th>
                          <th>Department Contact</th>
                          <th>Status</th>
                          <th style="text-align:right;">Actions</th>
                        </tr>
                      </thead>
                      <tbody id="cfg-depts-table-body">
                        <!-- Rendered dynamically -->
                      </tbody>
                    </table>
                  </div>
                </div>
              </div>

              <!-- ===============================================================
                   SECTION 8: DESIGNATION CONFIGURATION
                   =============================================================== -->
              <div class="config-pane" id="cfg-pane-designations">
                <!-- KPI CARDS -->
                <div class="kpi-grid" style="margin-bottom:0;">
                  <div class="kpi-card" style="--kpi-accent:#ff6b00;">
                    <div class="kpi-header"><span class="kpi-label">Total Designations</span><span class="kpi-icon-badge">💼</span></div>
                    <div class="kpi-value" id="cfg-kpi-desig-total">8</div>
                    <div class="kpi-sub">Active job descriptions</div>
                  </div>
                  <div class="kpi-card" style="--kpi-accent:#10b981;">
                    <div class="kpi-header"><span class="kpi-label">Hierarchy Grades</span><span class="kpi-icon-badge">📊</span></div>
                    <div class="kpi-value" style="color:#10b981;">L1 to L5</div>
                    <div class="kpi-sub">Executive to Director</div>
                  </div>
                  <div class="kpi-card" style="--kpi-accent:#4f46e5;">
                    <div class="kpi-header"><span class="kpi-label">Base Pay Range</span><span class="kpi-icon-badge">💵</span></div>
                    <div class="kpi-value" style="font-size:22px; color:#4f46e5;">₹28k - ₹95k</div>
                    <div class="kpi-sub">Standard salary scales</div>
                  </div>
                  <div class="kpi-card" style="--kpi-accent:#06b6d4;">
                    <div class="kpi-header"><span class="kpi-label">Unassigned Roles</span><span class="kpi-icon-badge">🎯</span></div>
                    <div class="kpi-value" style="color:#06b6d4;">0</div>
                    <div class="kpi-sub">All staff properly graded</div>
                  </div>
                </div>

                <div class="card">
                  <div class="card-header">
                    <div class="card-title-group">
                      <h3 class="card-title">💼 Job Designations & Hierarchy Grades</h3>
                      <p class="card-subtitle">Define job titles, map to departments, configure grade levels and pay bands</p>
                    </div>
                    <button type="button" class="btn btn-primary btn-sm" onclick="openAddDesigModal()">➕ Add Designation</button>
                  </div>
                  <div class="table-container">
                    <table>
                      <thead>
                        <tr>
                          <th>Designation Title</th>
                          <th>Department</th>
                          <th>Hierarchy Grade</th>
                          <th>Standard Pay Range</th>
                          <th>Active Staff</th>
                          <th>Status</th>
                          <th style="text-align:right;">Actions</th>
                        </tr>
                      </thead>
                      <tbody id="cfg-desig-table-body">
                        <!-- Rendered dynamically -->
                      </tbody>
                    </table>
                  </div>
                </div>
              </div>

              <!-- ===============================================================
                   SECTION 9: HOLIDAYS CONFIGURATION
                   =============================================================== -->
              <div class="config-pane" id="cfg-pane-holidays">
                <!-- KPI CARDS -->
                <div class="kpi-grid" style="margin-bottom:0;">
                  <div class="kpi-card" style="--kpi-accent:#ff6b00;">
                    <div class="kpi-header"><span class="kpi-label">Calendar Year 2026</span><span class="kpi-icon-badge">🗓️</span></div>
                    <div class="kpi-value" id="cfg-kpi-holidays-total">12</div>
                    <div class="kpi-sub">Official paid holidays</div>
                  </div>
                  <div class="kpi-card" style="--kpi-accent:#10b981;">
                    <div class="kpi-header"><span class="kpi-label">Gazetted National</span><span class="kpi-icon-badge">🇮🇳</span></div>
                    <div class="kpi-value" style="color:#10b981;">7 Days</div>
                    <div class="kpi-sub">Mandatory company closures</div>
                  </div>
                  <div class="kpi-card" style="--kpi-accent:#f59e0b;">
                    <div class="kpi-header"><span class="kpi-label">Festival & Regional</span><span class="kpi-icon-badge">✨</span></div>
                    <div class="kpi-value" style="color:#f59e0b;">5 Days</div>
                    <div class="kpi-sub">Cultural observance holidays</div>
                  </div>
                  <div class="kpi-card" style="--kpi-accent:#06b6d4;">
                    <div class="kpi-header"><span class="kpi-label">Next Public Holiday</span><span class="kpi-icon-badge">🚩</span></div>
                    <div class="kpi-value" style="font-size:20px; color:#06b6d4;">Dussehra</div>
                    <div class="kpi-sub">20 Oct 2026 (Tuesday)</div>
                  </div>
                </div>

                <div class="card">
                  <div class="card-header">
                    <div class="card-title-group">
                      <h3 class="card-title">🗓️ Annual Holiday Calendar (2026 - 2027)</h3>
                      <p class="card-subtitle">National, gazetted, and company holidays. Verified biometric days are automatically credited</p>
                    </div>
                    <div style="display:flex; gap:10px;">
                      <select id="cfg-holiday-year-select" class="form-input" style="padding:6px 12px; font-weight:700;" onchange="filterHolidaysByYear(this.value)">
                        <option value="2026" selected>Year 2026 (Active)</option>
                        <option value="2027">Year 2027</option>
                      </select>
                      <button type="button" class="btn btn-primary btn-sm" onclick="openAddHolidayModal()">➕ Add Holiday</button>
                    </div>
                  </div>
                  <div class="table-container">
                    <table>
                      <thead>
                        <tr>
                          <th>Date</th>
                          <th>Day</th>
                          <th>Holiday Occasion / Name</th>
                          <th>Classification</th>
                          <th>Wage Policy</th>
                          <th>Status</th>
                          <th style="text-align:right;">Actions</th>
                        </tr>
                      </thead>
                      <tbody id="cfg-holidays-table-body">
                        <!-- Rendered dynamically -->
                      </tbody>
                    </table>
                  </div>
                </div>
              </div>

              <!-- ===============================================================
                   SECTION 10: ROLES AND PERMISSIONS CONFIGURATION
                   =============================================================== -->
              <div class="config-pane" id="cfg-pane-roles">
                <div class="card">
                  <div class="card-header">
                    <div class="card-title-group">
                      <h3 class="card-title">🛡️ Role-Based Access Control (RBAC) Matrix</h3>
                      <p class="card-subtitle">Granular module-by-module permissions across organizational administrative tiers</p>
                    </div>
                    <div style="display:flex; gap:10px;">
                      <button type="button" class="btn btn-outline btn-sm" onclick="openAddRoleModal()">➕ New Role</button>
                      <button type="button" class="btn btn-primary btn-sm" onclick="saveRolesMatrix()">💾 Save Permissions Matrix</button>
                    </div>
                  </div>
                  <div class="table-container">
                    <table class="perm-grid-table">
                      <thead>
                        <tr>
                          <th style="width:280px;">System Module / Privilege</th>
                          <th><span class="badge badge-danger">Super Admin</span></th>
                          <th><span class="badge badge-primary">HR Manager</span></th>
                          <th><span class="badge badge-accent">Supervisor</span></th>
                          <th><span class="badge badge-warning">Accountant</span></th>
                          <th><span class="badge badge-cyan">Employee</span></th>
                        </tr>
                      </thead>
                      <tbody id="cfg-roles-matrix-body">
                        <!-- Rendered dynamically -->
                      </tbody>
                    </table>
                  </div>
                </div>

                <div class="card">
                  <div class="card-header">
                    <div class="card-title-group">
                      <h3 class="card-title">👥 Role Summaries & Active Assignments</h3>
                      <p class="card-subtitle">Overview of security scopes, privileges and assigned users</p>
                    </div>
                  </div>
                  <div style="padding:22px;">
                    <div id="cfg-role-cards-container" style="display:grid; grid-template-columns:repeat(auto-fit, minmax(280px, 1fr)); gap:16px;">
                      <!-- Rendered dynamically -->
                    </div>
                  </div>
                </div>

                <div class="config-action-bar">
                  <button type="button" class="btn btn-primary" onclick="saveRolesMatrix()">💾 Save Permissions Matrix</button>
                </div>
              </div>

            </main>
          </div>
        </section>

        <!-- ===============================================================
             CONFIGURATION MODALS (BRANCH, SHIFT, LEAVE, DEPT, DESIG, HOLIDAY)
             =============================================================== -->
        <!-- MODAL: ADD / EDIT BRANCH -->
        <div class="modal-backdrop" id="modal-cfg-branch" style="display:none;">
          <div class="modal" style="max-width:540px;">
            <div class="modal-header">
              <div class="modal-title" id="modal-branch-title">📍 Add New Branch</div>
              <button class="btn-icon" onclick="closeModal('modal-cfg-branch')">✕</button>
            </div>
            <div class="modal-body">
              <input type="hidden" id="modal-branch-id">
              <div class="config-form-group">
                <label>Branch Name *</label>
                <input type="text" id="modal-branch-name" class="form-input" placeholder="e.g. Hyderabad Central Facility">
              </div>
              <div style="display:grid; grid-template-columns:1fr 1fr; gap:12px;">
                <div class="config-form-group">
                  <label>Branch Code *</label>
                  <input type="text" id="modal-branch-code" class="form-input" placeholder="e.g. HQ-HYD">
                </div>
                <div class="config-form-group">
                  <label>Location / City *</label>
                  <input type="text" id="modal-branch-location" class="form-input" placeholder="e.g. Hyderabad, Telangana">
                </div>
              </div>
              <div style="display:grid; grid-template-columns:1fr 1fr; gap:12px;">
                <div class="config-form-group">
                  <label>Contact Person</label>
                  <input type="text" id="modal-branch-contact" class="form-input" placeholder="e.g. Anurag Sharma">
                </div>
                <div class="config-form-group">
                  <label>Contact Phone</label>
                  <input type="text" id="modal-branch-phone" class="form-input" placeholder="e.g. +91 98765 43210">
                </div>
              </div>
              <div style="display:grid; grid-template-columns:1fr 1fr; gap:12px;">
                <div class="config-form-group">
                  <label>Local Subnet / IP Range</label>
                  <input type="text" id="modal-branch-ip" class="form-input" placeholder="e.g. 192.168.0.0/24">
                </div>
                <div class="config-form-group">
                  <label>Status</label>
                  <select id="modal-branch-status" class="form-input">
                    <option value="Active">Active</option>
                    <option value="Maintenance">Maintenance</option>
                    <option value="Inactive">Inactive</option>
                  </select>
                </div>
              </div>
            </div>
            <div class="modal-footer">
              <button class="btn btn-outline" onclick="closeModal('modal-cfg-branch')">Cancel</button>
              <button class="btn btn-primary" onclick="saveBranchModal()">Save Branch</button>
            </div>
          </div>
        </div>

        <!-- MODAL: ADD / EDIT SHIFT -->
        <div class="modal-backdrop" id="modal-cfg-shift" style="display:none;">
          <div class="modal" style="max-width:540px;">
            <div class="modal-header">
              <div class="modal-title" id="modal-shift-title">⏱️ Add Shift Configuration</div>
              <button class="btn-icon" onclick="closeModal('modal-cfg-shift')">✕</button>
            </div>
            <div class="modal-body">
              <input type="hidden" id="modal-shift-id">
              <div class="config-form-group">
                <label>Shift Name *</label>
                <input type="text" id="modal-shift-name" class="form-input" placeholder="e.g. Morning Plant Shift">
              </div>
              <div style="display:grid; grid-template-columns:1fr 1fr; gap:12px;">
                <div class="config-form-group">
                  <label>Shift Code *</label>
                  <input type="text" id="modal-shift-code" class="form-input" placeholder="e.g. MRN">
                </div>
                <div class="config-form-group">
                  <label>Grace Period (Mins)</label>
                  <input type="number" id="modal-shift-grace" class="form-input" value="15">
                </div>
              </div>
              <div style="display:grid; grid-template-columns:1fr 1fr; gap:12px;">
                <div class="config-form-group">
                  <label>Shift Start Time *</label>
                  <input type="time" id="modal-shift-start" class="form-input" value="09:00">
                </div>
                <div class="config-form-group">
                  <label>Shift End Time *</label>
                  <input type="time" id="modal-shift-end" class="form-input" value="17:00">
                </div>
              </div>
              <div style="display:grid; grid-template-columns:1fr 1fr; gap:12px;">
                <div class="config-form-group">
                  <label>Half-Day Min Hours</label>
                  <input type="number" step="0.5" id="modal-shift-halfday" class="form-input" value="4.5">
                </div>
                <div class="config-form-group">
                  <label>Shift Allowance (₹)</label>
                  <input type="number" id="modal-shift-allowance" class="form-input" value="0">
                </div>
              </div>
              <div style="display:grid; grid-template-columns:1fr 1fr; gap:12px;">
                <div class="config-form-group">
                  <label>Break Time (Mins)</label>
                  <input type="number" id="modal-shift-break" class="form-input" value="60">
                </div>
                <div class="config-form-group">
                  <label>Status</label>
                  <select id="modal-shift-status" class="form-input">
                    <option value="Active">Active</option>
                    <option value="Inactive">Inactive</option>
                  </select>
                </div>
              </div>
            </div>
            <div class="modal-footer">
              <button class="btn btn-outline" onclick="closeModal('modal-cfg-shift')">Cancel</button>
              <button class="btn btn-primary" onclick="saveShiftModal()">Save Shift</button>
            </div>
          </div>
        </div>

        <!-- MODAL: ADD / EDIT LEAVE -->
        <div class="modal-backdrop" id="modal-cfg-leave" style="display:none;">
          <div class="modal" style="max-width:520px;">
            <div class="modal-header">
              <div class="modal-title" id="modal-leave-title">🏖️ Add Leave Category</div>
              <button class="btn-icon" onclick="closeModal('modal-cfg-leave')">✕</button>
            </div>
            <div class="modal-body">
              <input type="hidden" id="modal-leave-id">
              <div class="config-form-group">
                <label>Leave Category Name *</label>
                <input type="text" id="modal-leave-name" class="form-input" placeholder="e.g. Casual Leave">
              </div>
              <div style="display:grid; grid-template-columns:1fr 1fr; gap:12px;">
                <div class="config-form-group">
                  <label>Leave Code *</label>
                  <input type="text" id="modal-leave-code" class="form-input" placeholder="e.g. CL">
                </div>
                <div class="config-form-group">
                  <label>Annual Quota (Days) *</label>
                  <input type="number" id="modal-leave-quota" class="form-input" value="12">
                </div>
              </div>
              <div style="display:grid; grid-template-columns:1fr 1fr; gap:12px;">
                <div class="config-form-group">
                  <label>Carry Forward?</label>
                  <select id="modal-leave-carry" class="form-input">
                    <option value="false">No (Lapses at Year End)</option>
                    <option value="true">Yes (Rolls to Next Year)</option>
                  </select>
                </div>
                <div class="config-form-group">
                  <label>Encashable?</label>
                  <select id="modal-leave-encash" class="form-input">
                    <option value="false">No</option>
                    <option value="true">Yes (Payable upon exit)</option>
                  </select>
                </div>
              </div>
              <div style="display:grid; grid-template-columns:1fr 1fr; gap:12px;">
                <div class="config-form-group">
                  <label>Paid Status</label>
                  <select id="modal-leave-paid" class="form-input">
                    <option value="true">Paid Leave</option>
                    <option value="false">Unpaid (Loss of Pay)</option>
                  </select>
                </div>
                <div class="config-form-group">
                  <label>Requires Approval?</label>
                  <select id="modal-leave-approval" class="form-input">
                    <option value="true">Yes (Requires HR/Lead)</option>
                    <option value="false">No (Auto-approved)</option>
                  </select>
                </div>
              </div>
            </div>
            <div class="modal-footer">
              <button class="btn btn-outline" onclick="closeModal('modal-cfg-leave')">Cancel</button>
              <button class="btn btn-primary" onclick="saveLeaveModal()">Save Leave</button>
            </div>
          </div>
        </div>

        <!-- MODAL: ADD / EDIT DEPARTMENT -->
        <div class="modal-backdrop" id="modal-cfg-dept" style="display:none;">
          <div class="modal" style="max-width:520px;">
            <div class="modal-header">
              <div class="modal-title" id="modal-dept-title">🏛️ Add Department</div>
              <button class="btn-icon" onclick="closeModal('modal-cfg-dept')">✕</button>
            </div>
            <div class="modal-body">
              <input type="hidden" id="modal-dept-id">
              <div class="config-form-group">
                <label>Department Name *</label>
                <input type="text" id="modal-dept-name" class="form-input" placeholder="e.g. Operations & Production">
              </div>
              <div style="display:grid; grid-template-columns:1fr 1fr; gap:12px;">
                <div class="config-form-group">
                  <label>Department Code *</label>
                  <input type="text" id="modal-dept-code" class="form-input" placeholder="e.g. OPS">
                </div>
                <div class="config-form-group">
                  <label>Department Head (HOD)</label>
                  <input type="text" id="modal-dept-head" class="form-input" placeholder="e.g. Rajesh Kulkarni">
                </div>
              </div>
              <div style="display:grid; grid-template-columns:1fr 1fr; gap:12px;">
                <div class="config-form-group">
                  <label>Floor / Facility Location</label>
                  <input type="text" id="modal-dept-location" class="form-input" placeholder="e.g. Floor 2 / Bay 4">
                </div>
                <div class="config-form-group">
                  <label>Department Email</label>
                  <input type="text" id="modal-dept-email" class="form-input" placeholder="e.g. ops@anuragbiometrics.com">
                </div>
              </div>
            </div>
            <div class="modal-footer">
              <button class="btn btn-outline" onclick="closeModal('modal-cfg-dept')">Cancel</button>
              <button class="btn btn-primary" onclick="saveDeptModal()">Save Department</button>
            </div>
          </div>
        </div>

        <!-- MODAL: ADD / EDIT DESIGNATION -->
        <div class="modal-backdrop" id="modal-cfg-desig" style="display:none;">
          <div class="modal" style="max-width:520px;">
            <div class="modal-header">
              <div class="modal-title" id="modal-desig-title">💼 Add Designation</div>
              <button class="btn-icon" onclick="closeModal('modal-cfg-desig')">✕</button>
            </div>
            <div class="modal-body">
              <input type="hidden" id="modal-desig-id">
              <div class="config-form-group">
                <label>Designation Title *</label>
                <input type="text" id="modal-desig-title-input" class="form-input" placeholder="e.g. Senior Software Engineer">
              </div>
              <div style="display:grid; grid-template-columns:1fr 1fr; gap:12px;">
                <div class="config-form-group">
                  <label>Department *</label>
                  <select id="modal-desig-dept" class="form-input">
                    <!-- Populated dynamically -->
                  </select>
                </div>
                <div class="config-form-group">
                  <label>Corporate Grade</label>
                  <select id="modal-desig-grade" class="form-input">
                    <option value="L1">L1 - Associate / Executive</option>
                    <option value="L2">L2 - Specialist / Technician</option>
                    <option value="L3" selected>L3 - Senior / Team Lead</option>
                    <option value="L4">L4 - Principal / Manager</option>
                    <option value="L5">L5 - Director / VP</option>
                  </select>
                </div>
              </div>
              <div style="display:grid; grid-template-columns:1fr 1fr; gap:12px;">
                <div class="config-form-group">
                  <label>Minimum Base Pay (₹)</label>
                  <input type="number" id="modal-desig-min" class="form-input" value="45000">
                </div>
                <div class="config-form-group">
                  <label>Maximum Base Pay (₹)</label>
                  <input type="number" id="modal-desig-max" class="form-input" value="75000">
                </div>
              </div>
            </div>
            <div class="modal-footer">
              <button class="btn btn-outline" onclick="closeModal('modal-cfg-desig')">Cancel</button>
              <button class="btn btn-primary" onclick="saveDesigModal()">Save Designation</button>
            </div>
          </div>
        </div>

        <!-- MODAL: ADD / EDIT HOLIDAY -->
        <div class="modal-backdrop" id="modal-cfg-holiday" style="display:none;">
          <div class="modal" style="max-width:500px;">
            <div class="modal-header">
              <div class="modal-title" id="modal-holiday-title">🗓️ Add Official Holiday</div>
              <button class="btn-icon" onclick="closeModal('modal-cfg-holiday')">✕</button>
            </div>
            <div class="modal-body">
              <input type="hidden" id="modal-holiday-id">
              <div class="config-form-group">
                <label>Holiday Occasion / Name *</label>
                <input type="text" id="modal-holiday-name" class="form-input" placeholder="e.g. Independence Day">
              </div>
              <div style="display:grid; grid-template-columns:1fr 1fr; gap:12px;">
                <div class="config-form-group">
                  <label>Holiday Date *</label>
                  <input type="date" id="modal-holiday-date" class="form-input" value="2026-08-15">
                </div>
                <div class="config-form-group">
                  <label>Classification</label>
                  <select id="modal-holiday-type" class="form-input">
                    <option value="Gazetted National">Gazetted National</option>
                    <option value="Public Holiday">Public Holiday</option>
                    <option value="Regional / Optional">Regional / Optional</option>
                  </select>
                </div>
              </div>
              <div class="config-form-group">
                <label>Paid Status</label>
                <select id="modal-holiday-paid" class="form-input">
                  <option value="true">Paid Holiday (Attendance credited for all staff)</option>
                  <option value="false">Unpaid Holiday</option>
                </select>
              </div>
            </div>
            <div class="modal-footer">
              <button class="btn btn-outline" onclick="closeModal('modal-cfg-holiday')">Cancel</button>
              <button class="btn btn-primary" onclick="saveHolidayModal()">Save Holiday</button>
            </div>
          </div>
        </div>

        <!-- MODAL: ADD CUSTOM ROLE -->
        <div class="modal-backdrop" id="modal-cfg-role" style="display:none;">
          <div class="modal" style="max-width:500px;">
            <div class="modal-header">
              <div class="modal-title">🛡️ Create Custom Role</div>
              <button class="btn-icon" onclick="closeModal('modal-cfg-role')">✕</button>
            </div>
            <div class="modal-body">
              <div class="config-form-group">
                <label>Role Title *</label>
                <input type="text" id="modal-role-name" class="form-input" placeholder="e.g. Plant Quality Auditor">
              </div>
              <div class="config-form-group">
                <label>Role Code *</label>
                <input type="text" id="modal-role-code" class="form-input" placeholder="e.g. QA_AUDIT">
              </div>
              <div class="config-form-group">
                <label>Description & Scope</label>
                <input type="text" id="modal-role-desc" class="form-input" placeholder="Read-only access to audit logs and attendance reports">
              </div>
            </div>
            <div class="modal-footer">
              <button class="btn btn-outline" onclick="closeModal('modal-cfg-role')">Cancel</button>
              <button class="btn btn-primary" onclick="saveCustomRoleModal()">Create Role</button>
            </div>
          </div>
        </div>
`;

// 3. JAVASCRIPT LOGIC TO INJECT IN <script>
const jsToInject = `
    // =========================================================================
    //  ENTERPRISE CONFIGURATION ENGINE & MASTER DATA STORE
    // =========================================================================
    var defaultConfigData = {
      general: {
        companyName: 'Anurag Attendance & Biometrics',
        companyCode: 'AAB-CORP',
        tagline: 'Enterprise 24/7 Biometric Cloud MIS & Workforce Suite',
        contactEmail: 'admin@anuragbiometrics.com',
        contactPhone: '+91 98765 43210',
        address: 'HITEC City, Cyber Towers, Hyderabad, TG 500081',
        timezone: 'Asia/Kolkata',
        currency: 'INR (₹)',
        workDaysPerWeek: '6',
        fiscalYearStart: 'April'
      },
      system: {
        firebaseUrl: 'https://chronosmis-default-rtdb.firebaseio.com',
        firebaseAuth: '',
        deviceIp: '192.168.0.108',
        devicePort: 80,
        syncIntervalSec: 10,
        soundEnabled: true,
        theme: 'dark',
        portalPwd: 'admin123'
      },
      branches: [
        { id: 'BR-01', name: 'Headquarters & Tech Center', code: 'HQ-HYD', location: 'Hyderabad, Telangana', contact: 'Anurag Sharma', phone: '+91 98765 43210', staffCount: 12, ipRange: '192.168.0.0/24', status: 'Active' },
        { id: 'BR-02', name: 'Manufacturing Plant 1', code: 'PL-PUN', location: 'Pune, Maharashtra', contact: 'Rajesh Kulkarni', phone: '+91 98234 56789', staffCount: 5, ipRange: '192.168.10.0/24', status: 'Active' },
        { id: 'BR-03', name: 'Regional Distribution Hub', code: 'RD-BLR', location: 'Bengaluru, Karnataka', contact: 'Suresh Menon', phone: '+91 98450 12345', staffCount: 3, ipRange: '192.168.20.0/24', status: 'Active' }
      ],
      shifts: [
        { id: 'SH-GEN', code: 'GEN', name: 'General Office Shift', start: '09:00', end: '17:00', grace: 15, halfDayMinHours: 4.5, breakDuration: 60, allowance: 0, status: 'Active' },
        { id: 'SH-MRN', code: 'MRN', name: 'Morning Plant Shift', start: '06:00', end: '14:00', grace: 10, halfDayMinHours: 4.0, breakDuration: 45, allowance: 100, status: 'Active' },
        { id: 'SH-EVN', code: 'EVN', name: 'Evening Operations Shift', start: '14:00', end: '22:00', grace: 10, halfDayMinHours: 4.0, breakDuration: 45, allowance: 150, status: 'Active' },
        { id: 'SH-NGT', code: 'NGT', name: 'Night Production Shift', start: '22:00', end: '06:00', grace: 10, halfDayMinHours: 4.0, breakDuration: 45, allowance: 250, status: 'Active' }
      ],
      leaves: [
        { id: 'LV-CL', code: 'CL', name: 'Casual Leave', quota: 12, carryForward: false, encashable: false, paid: true, requiresApproval: true },
        { id: 'LV-SL', code: 'SL', name: 'Sick / Medical Leave', quota: 10, carryForward: true, encashable: false, paid: true, requiresApproval: true },
        { id: 'LV-PL', code: 'PL', name: 'Privilege / Annual Leave', quota: 15, carryForward: true, encashable: true, paid: true, requiresApproval: true },
        { id: 'LV-ML', code: 'ML', name: 'Maternity Leave', quota: 180, carryForward: false, encashable: false, paid: true, requiresApproval: true },
        { id: 'LV-LWP', code: 'LWP', name: 'Leave Without Pay (Loss of Pay)', quota: 30, carryForward: false, encashable: false, paid: false, requiresApproval: true }
      ],
      leavePolicies: {
        sandwichedLeaves: true,
        maxConsecutiveDays: 10,
        noticeDaysForPL: 7,
        probationAllowed: false
      },
      rules: {
        lateGraceMinutes: 15,
        lateAction: '3_LATE_HALF_DAY',
        earlyGraceMinutes: 15,
        earlyAction: 'HALF_DAY',
        minFullDayHours: 8.0,
        minHalfDayHours: 4.5,
        otThresholdMinutes: 30,
        otRateMultiplier: 1.5,
        weekendOtMultiplier: 2.0,
        maxDailyOtHours: 4.0,
        singlePunchHandling: 'MARK_HALF_DAY',
        autoCheckoutTime: '23:59'
      },
      departments: [
        { id: 'DP-01', code: 'ENG', name: 'Engineering & IT', head: 'Anurag Sharma', staffCount: 6, location: 'Floor 3', email: 'it@anuragbiometrics.com', status: 'Active' },
        { id: 'DP-02', code: 'HR', name: 'Human Resources & Admin', head: 'Pooja Verma', staffCount: 3, location: 'Floor 2', email: 'hr@anuragbiometrics.com', status: 'Active' },
        { id: 'DP-03', code: 'OPS', name: 'Plant Operations & Production', head: 'Rajesh Kulkarni', staffCount: 5, location: 'Factory Shopfloor', email: 'ops@anuragbiometrics.com', status: 'Active' },
        { id: 'DP-04', code: 'FIN', name: 'Accounts & Finance', head: 'Vikas Gupta', staffCount: 3, location: 'Floor 2', email: 'finance@anuragbiometrics.com', status: 'Active' },
        { id: 'DP-05', code: 'QA', name: 'Quality Assurance & Testing', head: 'Sneha Patel', staffCount: 2, location: 'Floor 1', email: 'qa@anuragbiometrics.com', status: 'Active' },
        { id: 'DP-06', code: 'SCM', name: 'Supply Chain & Logistics', head: 'Amitabh Sen', staffCount: 1, location: 'Warehouse 4', email: 'logistics@anuragbiometrics.com', status: 'Active' }
      ],
      designations: [
        { id: 'DS-01', title: 'Senior Software Engineer', department: 'Engineering & IT', grade: 'L4', minPay: 65000, maxPay: 95000, staffCount: 3, status: 'Active' },
        { id: 'DS-02', title: 'Embedded Firmware Engineer', department: 'Engineering & IT', grade: 'L3', minPay: 55000, maxPay: 80000, staffCount: 3, status: 'Active' },
        { id: 'DS-03', title: 'HR Business Partner', department: 'Human Resources & Admin', grade: 'L3', minPay: 45000, maxPay: 65000, staffCount: 3, status: 'Active' },
        { id: 'DS-04', title: 'Plant Supervisor', department: 'Plant Operations & Production', grade: 'L3', minPay: 40000, maxPay: 60000, staffCount: 2, status: 'Active' },
        { id: 'DS-05', title: 'Senior Machinist / Operator', department: 'Plant Operations & Production', grade: 'L2', minPay: 28000, maxPay: 42000, staffCount: 3, status: 'Active' },
        { id: 'DS-06', title: 'Lead Financial Analyst', department: 'Accounts & Finance', grade: 'L4', minPay: 60000, maxPay: 85000, staffCount: 3, status: 'Active' },
        { id: 'DS-07', title: 'QA Compliance Officer', department: 'Quality Assurance & Testing', grade: 'L3', minPay: 42000, maxPay: 62000, staffCount: 2, status: 'Active' },
        { id: 'DS-08', title: 'Logistics Coordinator', department: 'Supply Chain & Logistics', grade: 'L2', minPay: 30000, maxPay: 45000, staffCount: 1, status: 'Active' }
      ],
      holidays: [
        { id: 'HD-01', name: 'Republic Day', date: '2026-01-26', day: 'Monday', type: 'Gazetted National', paid: true, year: '2026' },
        { id: 'HD-02', name: 'Maha Shivratri', date: '2026-02-15', day: 'Sunday', type: 'Regional / Optional', paid: true, year: '2026' },
        { id: 'HD-03', name: 'Holi Festival of Colors', date: '2026-03-04', day: 'Wednesday', type: 'Gazetted National', paid: true, year: '2026' },
        { id: 'HD-04', name: 'Eid ul-Fitr', date: '2026-03-20', day: 'Friday', type: 'Public Holiday', paid: true, year: '2026' },
        { id: 'HD-05', name: 'Good Friday', date: '2026-04-03', day: 'Friday', type: 'Public Holiday', paid: true, year: '2026' },
        { id: 'HD-06', name: 'Labour Day / May Day', date: '2026-05-01', day: 'Friday', type: 'Gazetted National', paid: true, year: '2026' },
        { id: 'HD-07', name: 'Independence Day', date: '2026-08-15', day: 'Saturday', type: 'Gazetted National', paid: true, year: '2026' },
        { id: 'HD-08', name: 'Mahatma Gandhi Jayanti', date: '2026-10-02', day: 'Friday', type: 'Gazetted National', paid: true, year: '2026' },
        { id: 'HD-09', name: 'Dussehra / Vijaya Dashami', date: '2026-10-20', day: 'Tuesday', type: 'Gazetted National', paid: true, year: '2026' },
        { id: 'HD-10', name: 'Diwali Deepawali', date: '2026-11-08', day: 'Sunday', type: 'Gazetted National', paid: true, year: '2026' },
        { id: 'HD-11', name: 'Guru Nanak Jayanti', date: '2026-11-24', day: 'Tuesday', type: 'Public Holiday', paid: true, year: '2026' },
        { id: 'HD-12', name: 'Christmas Day', date: '2026-12-25', day: 'Friday', type: 'Public Holiday', paid: true, year: '2026' }
      ],
      roles: [
        {
          role: 'Super Administrator',
          code: 'ADMIN',
          desc: 'Unrestricted full access across all modules, cloud database, biometrics, rules and security',
          usersCount: 2,
          badge: 'badge-danger',
          perms: {
            dashboard: true,
            devices: true,
            biometrics: true,
            employees_view: true,
            employees_edit: true,
            attendance_view: true,
            attendance_edit: true,
            attendance_manual: true,
            payroll_view: true,
            payroll_edit: true,
            leave_approve: true,
            audit_view: true,
            config_view: true,
            config_edit: true
          }
        },
        {
          role: 'HR Manager',
          code: 'HR_MGR',
          desc: 'Employee directory management, leave approvals, shifts assignment, monthly attendance roll',
          usersCount: 3,
          badge: 'badge-primary',
          perms: {
            dashboard: true,
            devices: false,
            biometrics: true,
            employees_view: true,
            employees_edit: true,
            attendance_view: true,
            attendance_edit: true,
            attendance_manual: true,
            payroll_view: true,
            payroll_edit: true,
            leave_approve: true,
            audit_view: true,
            config_view: true,
            config_edit: false
          }
        },
        {
          role: 'Operations Supervisor',
          code: 'OPS_SUP',
          desc: 'Floor supervisor responsible for shift monitoring, overtime verification & daily check-ins',
          usersCount: 5,
          badge: 'badge-accent',
          perms: {
            dashboard: true,
            devices: true,
            biometrics: false,
            employees_view: true,
            employees_edit: false,
            attendance_view: true,
            attendance_edit: false,
            attendance_manual: true,
            payroll_view: false,
            payroll_edit: false,
            leave_approve: false,
            audit_view: true,
            config_view: false,
            config_edit: false
          }
        },
        {
          role: 'Payroll Accountant',
          code: 'FIN_ACC',
          desc: 'Wage engine execution, deductions, allowances, muster roll compliance and salary slips',
          usersCount: 2,
          badge: 'badge-warning',
          perms: {
            dashboard: true,
            devices: false,
            biometrics: false,
            employees_view: true,
            employees_edit: false,
            attendance_view: true,
            attendance_edit: false,
            attendance_manual: false,
            payroll_view: true,
            payroll_edit: true,
            leave_approve: false,
            audit_view: true,
            config_view: false,
            config_edit: false
          }
        },
        {
          role: 'Standard Employee',
          code: 'EMP_STD',
          desc: 'Self-service portal access: view personal attendance history, punch status and apply for leave',
          usersCount: 20,
          badge: 'badge-cyan',
          perms: {
            dashboard: true,
            devices: false,
            biometrics: false,
            employees_view: false,
            employees_edit: false,
            attendance_view: true,
            attendance_edit: false,
            attendance_manual: false,
            payroll_view: false,
            payroll_edit: false,
            leave_approve: false,
            audit_view: false,
            config_view: false,
            config_edit: false
          }
        }
      ]
    };

    // Load or initialize config in appState
    function initEnterpriseConfig() {
      if (!appState.config) {
        var saved = localStorage.getItem('chronos_config');
        if (saved) {
          try {
            appState.config = JSON.parse(saved);
          } catch(e) {
            appState.config = JSON.parse(JSON.stringify(defaultConfigData));
          }
        } else {
          appState.config = JSON.parse(JSON.stringify(defaultConfigData));
        }
      }

      // Sync settings bridge
      if (appState.settings && appState.config) {
        if (appState.settings.companyName) appState.config.general.companyName = appState.settings.companyName;
        if (appState.firebaseUrl) appState.config.system.firebaseUrl = appState.firebaseUrl;
        if (appState.firebaseAuth) appState.config.system.firebaseAuth = appState.firebaseAuth;
        if (appState.deviceIp) appState.config.system.deviceIp = appState.deviceIp;
        if (appState.settings.shiftGrace) appState.config.rules.lateGraceMinutes = appState.settings.shiftGrace;
        if (appState.settings.otRate) appState.config.rules.otRateMultiplier = appState.settings.otRate;
      }
    }

    function saveConfigToStorage() {
      localStorage.setItem('chronos_config', JSON.stringify(appState.config));
    }

    // Switch between Configuration sections inside the Configuration View
    function switchConfigTab(paneKey, btnEl) {
      document.querySelectorAll('.config-nav-btn').forEach(function(b){ b.classList.remove('active'); });
      document.querySelectorAll('.config-pane').forEach(function(p){ p.classList.remove('active'); });

      if (btnEl) btnEl.classList.add('active');
      else {
        var found = document.querySelector('.config-nav-btn[data-cfg-pane="' + paneKey + '"]');
        if (found) found.classList.add('active');
      }

      var target = document.getElementById('cfg-pane-' + paneKey);
      if (target) {
        target.classList.add('active');
      }

      // Re-render specific section if needed
      if (paneKey === 'branches') renderConfigBranches();
      if (paneKey === 'shifts') renderConfigShifts();
      if (paneKey === 'leaves') renderConfigLeaves();
      if (paneKey === 'departments') renderConfigDepartments();
      if (paneKey === 'designations') renderConfigDesignations();
      if (paneKey === 'holidays') renderConfigHolidays();
      if (paneKey === 'roles') renderConfigRolesMatrix();
    }

    // Master render for Configuration view
    function renderConfigurationView() {
      initEnterpriseConfig();

      // Populate General fields
      var gen = appState.config.general;
      if (document.getElementById('cfg-gen-company-name')) document.getElementById('cfg-gen-company-name').value = gen.companyName || '';
      if (document.getElementById('cfg-gen-company-code')) document.getElementById('cfg-gen-company-code').value = gen.companyCode || '';
      if (document.getElementById('cfg-gen-tagline')) document.getElementById('cfg-gen-tagline').value = gen.tagline || '';
      if (document.getElementById('cfg-gen-email')) document.getElementById('cfg-gen-email').value = gen.contactEmail || '';
      if (document.getElementById('cfg-gen-phone')) document.getElementById('cfg-gen-phone').value = gen.contactPhone || '';
      if (document.getElementById('cfg-gen-address')) document.getElementById('cfg-gen-address').value = gen.address || '';
      if (document.getElementById('cfg-gen-timezone')) document.getElementById('cfg-gen-timezone').value = gen.timezone || 'Asia/Kolkata';
      if (document.getElementById('cfg-gen-currency')) document.getElementById('cfg-gen-currency').value = gen.currency || 'INR (₹)';
      if (document.getElementById('cfg-gen-workdays')) document.getElementById('cfg-gen-workdays').value = gen.workDaysPerWeek || '6';
      if (document.getElementById('cfg-gen-fiscal')) document.getElementById('cfg-gen-fiscal').value = gen.fiscalYearStart || 'April';

      // Populate System fields
      var sys = appState.config.system;
      if (document.getElementById('cfg-sys-fb-url')) document.getElementById('cfg-sys-fb-url').value = appState.firebaseUrl || sys.firebaseUrl || '';
      if (document.getElementById('cfg-sys-fb-auth')) document.getElementById('cfg-sys-fb-auth').value = appState.firebaseAuth || sys.firebaseAuth || '';
      if (document.getElementById('cfg-sys-device-ip')) document.getElementById('cfg-sys-device-ip').value = appState.deviceIp || sys.deviceIp || '192.168.0.108';
      if (document.getElementById('cfg-sys-device-port')) document.getElementById('cfg-sys-device-port').value = sys.devicePort || 80;
      if (document.getElementById('cfg-sys-sync-interval')) document.getElementById('cfg-sys-sync-interval').value = sys.syncIntervalSec || 10;
      if (document.getElementById('cfg-sys-sound')) document.getElementById('cfg-sys-sound').value = appState.soundEnabled ? 'true' : 'false';
      if (document.getElementById('cfg-sys-theme')) document.getElementById('cfg-sys-theme').value = document.body.getAttribute('data-theme') || 'dark';

      // Populate Rules fields
      var r = appState.config.rules;
      if (document.getElementById('cfg-rule-late-grace')) document.getElementById('cfg-rule-late-grace').value = r.lateGraceMinutes || 15;
      if (document.getElementById('cfg-rule-late-action')) document.getElementById('cfg-rule-late-action').value = r.lateAction || '3_LATE_HALF_DAY';
      if (document.getElementById('cfg-rule-early-grace')) document.getElementById('cfg-rule-early-grace').value = r.earlyGraceMinutes || 15;
      if (document.getElementById('cfg-rule-early-action')) document.getElementById('cfg-rule-early-action').value = r.earlyAction || 'HALF_DAY';
      if (document.getElementById('cfg-rule-min-full')) document.getElementById('cfg-rule-min-full').value = r.minFullDayHours || 8.0;
      if (document.getElementById('cfg-rule-min-half')) document.getElementById('cfg-rule-min-half').value = r.minHalfDayHours || 4.5;
      if (document.getElementById('cfg-rule-ot-thresh')) document.getElementById('cfg-rule-ot-thresh').value = r.otThresholdMinutes || 30;
      if (document.getElementById('cfg-rule-ot-rate')) document.getElementById('cfg-rule-ot-rate').value = r.otRateMultiplier || 1.5;
      if (document.getElementById('cfg-rule-ot-weekend-rate')) document.getElementById('cfg-rule-ot-weekend-rate').value = r.weekendOtMultiplier || 2.0;
      if (document.getElementById('cfg-rule-ot-max')) document.getElementById('cfg-rule-ot-max').value = r.maxDailyOtHours || 4.0;
      if (document.getElementById('cfg-rule-single-punch')) document.getElementById('cfg-rule-single-punch').value = r.singlePunchHandling || 'MARK_HALF_DAY';
      if (document.getElementById('cfg-rule-auto-checkout-time')) document.getElementById('cfg-rule-auto-checkout-time').value = r.autoCheckoutTime || '23:59';

      // Populate Leave Policies
      var lp = appState.config.leavePolicies;
      if (document.getElementById('cfg-leave-sandwich')) document.getElementById('cfg-leave-sandwich').checked = !!lp.sandwichedLeaves;
      if (document.getElementById('cfg-leave-max-consec')) document.getElementById('cfg-leave-max-consec').value = lp.maxConsecutiveDays || 10;
      if (document.getElementById('cfg-leave-notice-days')) document.getElementById('cfg-leave-notice-days').value = lp.noticeDaysForPL || 7;
      if (document.getElementById('cfg-leave-probation')) document.getElementById('cfg-leave-probation').checked = !!lp.probationAllowed;

      // Update Sub-Sidebar Badges
      updateConfigBadges();

      // Render Active Tab Content
      var curActiveBtn = document.querySelector('.config-nav-btn.active');
      var activePane = curActiveBtn ? curActiveBtn.getAttribute('data-cfg-pane') : 'general';
      switchConfigTab(activePane);
    }

    function updateConfigBadges() {
      if (!appState.config) return;
      if (document.getElementById('cfg-branch-count')) document.getElementById('cfg-branch-count').innerText = appState.config.branches.length;
      if (document.getElementById('cfg-shift-count')) document.getElementById('cfg-shift-count').innerText = appState.config.shifts.length;
      if (document.getElementById('cfg-leave-count')) document.getElementById('cfg-leave-count').innerText = appState.config.leaves.length;
      if (document.getElementById('cfg-dept-count')) document.getElementById('cfg-dept-count').innerText = appState.config.departments.length;
      if (document.getElementById('cfg-desig-count')) document.getElementById('cfg-desig-count').innerText = appState.config.designations.length;
      if (document.getElementById('cfg-holiday-count')) document.getElementById('cfg-holiday-count').innerText = appState.config.holidays.length;
      if (document.getElementById('cfg-role-count')) document.getElementById('cfg-role-count').innerText = appState.config.roles.length;
    }

    // --- SAVE GENERAL CONFIG ---
    function saveGeneralConfig() {
      initEnterpriseConfig();
      var cName = document.getElementById('cfg-gen-company-name').value.trim() || 'Anurag Attendance & Biometrics';
      appState.config.general.companyName = cName;
      appState.config.general.companyCode = document.getElementById('cfg-gen-company-code').value.trim() || 'AAB-CORP';
      appState.config.general.tagline = document.getElementById('cfg-gen-tagline').value.trim();
      appState.config.general.contactEmail = document.getElementById('cfg-gen-email').value.trim();
      appState.config.general.contactPhone = document.getElementById('cfg-gen-phone').value.trim();
      appState.config.general.address = document.getElementById('cfg-gen-address').value.trim();
      appState.config.general.timezone = document.getElementById('cfg-gen-timezone').value;
      appState.config.general.currency = document.getElementById('cfg-gen-currency').value;
      appState.config.general.workDaysPerWeek = document.getElementById('cfg-gen-workdays').value;
      appState.config.general.fiscalYearStart = document.getElementById('cfg-gen-fiscal').value;

      appState.settings.companyName = cName;
      localStorage.setItem('chronos_settings', JSON.stringify(appState.settings));
      saveConfigToStorage();

      showToast("General Organization settings saved successfully!", "success");
    }

    function resetGeneralConfigDefaults() {
      if (!confirm("Reset general parameters to system defaults?")) return;
      appState.config.general = JSON.parse(JSON.stringify(defaultConfigData.general));
      renderConfigurationView();
      showToast("General settings reset to defaults.", "info");
    }

    // --- SAVE SYSTEM CONFIG ---
    function saveSystemConfig() {
      initEnterpriseConfig();
      var fbUrl = document.getElementById('cfg-sys-fb-url').value.trim();
      var fbAuth = document.getElementById('cfg-sys-fb-auth').value.trim();
      var devIp = document.getElementById('cfg-sys-device-ip').value.trim();
      var devPort = parseInt(document.getElementById('cfg-sys-device-port').value || 80);
      var syncInt = parseInt(document.getElementById('cfg-sys-sync-interval').value || 10);
      var snd = (document.getElementById('cfg-sys-sound').value === 'true');
      var thm = document.getElementById('cfg-sys-theme').value;
      var pwd = document.getElementById('cfg-sys-pwd').value.trim() || 'admin123';

      if (!fbUrl || fbUrl.length < 5) {
        return showToast("Please provide a valid Firebase URL", "error");
      }

      var normRes = normalizeFbUrlWithAuth(fbUrl);
      appState.firebaseUrl = normRes.url;
      if (normRes.auth && !fbAuth) fbAuth = normRes.auth;
      appState.firebaseAuth = fbAuth;
      appState.deviceIp = devIp || '192.168.0.108';
      appState.soundEnabled = snd;

      appState.config.system = {
        firebaseUrl: appState.firebaseUrl,
        firebaseAuth: appState.firebaseAuth,
        deviceIp: appState.deviceIp,
        devicePort: devPort,
        syncIntervalSec: syncInt,
        soundEnabled: snd,
        theme: thm,
        portalPwd: pwd
      };

      document.body.setAttribute('data-theme', thm);
      localStorage.setItem('erp_theme', thm);
      updateThemeButtonsUI(thm);

      localStorage.setItem('erp_fb_url', appState.firebaseUrl);
      localStorage.setItem('erp_fb_auth', appState.firebaseAuth);
      localStorage.setItem('fb_cloud_url', appState.firebaseUrl);
      localStorage.setItem('biometric_device_ip', appState.deviceIp);
      saveConfigToStorage();

      // Sync other input fields
      var fbIn = document.getElementById('gateway-fb-url');
      if (fbIn) fbIn.value = appState.firebaseUrl;
      var devIn = document.getElementById('gateway-device-ip');
      if (devIn) devIn.value = appState.deviceIp;

      showToast("System & Firebase settings updated successfully!", "success");
    }

    function previewTheme(mode) {
      document.body.setAttribute('data-theme', mode);
      localStorage.setItem('erp_theme', mode);
      updateThemeButtonsUI(mode);
    }

    function testConfigFirebaseLink() {
      var url = document.getElementById('cfg-sys-fb-url').value.trim();
      var auth = document.getElementById('cfg-sys-fb-auth').value.trim();
      var norm = normalizeFbUrlWithAuth(url);
      if (!norm.url) return showToast("Enter a valid Firebase URL to test", "warning");

      var pill = document.getElementById('cfg-sys-fb-status');
      if (pill) { pill.innerText = 'TESTING...'; pill.className = 'badge badge-warning'; }

      var testEndpoint = norm.url + '/.json?shallow=true' + (auth ? ('&auth=' + auth) : '');
      fetch(testEndpoint, { method: 'GET', mode: 'cors' })
        .then(function(res) {
          if (res.ok) {
            if (pill) { pill.innerText = 'ONLINE / VERIFIED'; pill.className = 'badge badge-success'; }
            showToast("Firebase Cloud Database connection verified successfully!", "success");
          } else {
            if (pill) { pill.innerText = 'AUTH REQUIRED'; pill.className = 'badge badge-warning'; }
            showToast("Firebase responded with HTTP " + res.status + ". Check auth secret or rules.", "warning");
          }
        })
        .catch(function(err) {
          if (pill) { pill.innerText = 'OFFLINE'; pill.className = 'badge badge-danger'; }
          showToast("Could not reach Firebase endpoint: " + err.message, "error");
        });
    }

    function syncFromConfigFirebaseLink() {
      saveSystemConfig();
      if (typeof startApp === 'function') startApp();
      showToast("Cloud sync triggered. Connecting to Realtime Database...", "info");
    }

    function testConfigDeviceIp() {
      var ip = document.getElementById('cfg-sys-device-ip').value.trim();
      if (!ip) return showToast("Enter ESP32 Device IP address", "warning");
      showToast("Pinging hardware gateway at " + ip + "... Ensure your browser allows local network access.", "info");
      var img = new Image();
      var done = false;
      img.onload = img.onerror = function() {
        if (!done) {
          done = true;
          showToast("ESP32 Device at " + ip + " is reachable on local subnet!", "success");
        }
      };
      img.src = 'http://' + ip + '/favicon.ico?' + Date.now();
      setTimeout(function() {
        if (!done) {
          done = true;
          showToast("Ping completed. If device is online, verify port 80 and start_erp.bat.", "info");
        }
      }, 2500);
    }

    // --- SAVE RULES CONFIG ---
    function saveRulesConfig() {
      initEnterpriseConfig();
      appState.config.rules = {
        lateGraceMinutes: parseInt(document.getElementById('cfg-rule-late-grace').value || 15),
        lateAction: document.getElementById('cfg-rule-late-action').value,
        earlyGraceMinutes: parseInt(document.getElementById('cfg-rule-early-grace').value || 15),
        earlyAction: document.getElementById('cfg-rule-early-action').value,
        minFullDayHours: parseFloat(document.getElementById('cfg-rule-min-full').value || 8.0),
        minHalfDayHours: parseFloat(document.getElementById('cfg-rule-min-half').value || 4.5),
        otThresholdMinutes: parseInt(document.getElementById('cfg-rule-ot-thresh').value || 30),
        otRateMultiplier: parseFloat(document.getElementById('cfg-rule-ot-rate').value || 1.5),
        weekendOtMultiplier: parseFloat(document.getElementById('cfg-rule-ot-weekend-rate').value || 2.0),
        maxDailyOtHours: parseFloat(document.getElementById('cfg-rule-ot-max').value || 4.0),
        singlePunchHandling: document.getElementById('cfg-rule-single-punch').value,
        autoCheckoutTime: document.getElementById('cfg-rule-auto-checkout-time').value || '23:59'
      };

      appState.settings.shiftGrace = appState.config.rules.lateGraceMinutes;
      appState.settings.otRate = appState.config.rules.otRateMultiplier;
      localStorage.setItem('chronos_settings', JSON.stringify(appState.settings));
      saveConfigToStorage();

      showToast("Attendance & Overtime rules updated successfully!", "success");
    }

    function resetRulesDefaults() {
      if (!confirm("Reset attendance calculation rules to standard factory parameters?")) return;
      appState.config.rules = JSON.parse(JSON.stringify(defaultConfigData.rules));
      renderConfigurationView();
      showToast("Rules restored to defaults.", "info");
    }

    // --- SAVE LEAVE POLICIES ---
    function saveLeavePolicies() {
      initEnterpriseConfig();
      appState.config.leavePolicies = {
        sandwichedLeaves: document.getElementById('cfg-leave-sandwich').checked,
        maxConsecutiveDays: parseInt(document.getElementById('cfg-leave-max-consec').value || 10),
        noticeDaysForPL: parseInt(document.getElementById('cfg-leave-notice-days').value || 7),
        probationAllowed: document.getElementById('cfg-leave-probation').checked
      };
      saveConfigToStorage();
      showToast("Statutory leave policies saved successfully!", "success");
    }

    // --- SAVE ACTIVE SECTION BUTTON ---
    function saveCurrentConfigSection() {
      var curActiveBtn = document.querySelector('.config-nav-btn.active');
      var activePane = curActiveBtn ? curActiveBtn.getAttribute('data-cfg-pane') : 'general';
      if (activePane === 'general') saveGeneralConfig();
      else if (activePane === 'system') saveSystemConfig();
      else if (activePane === 'rules') saveRulesConfig();
      else if (activePane === 'leaves') saveLeavePolicies();
      else if (activePane === 'roles') saveRolesMatrix();
      else {
        saveConfigToStorage();
        showToast("Configuration section '" + activePane.toUpperCase() + "' saved successfully!", "success");
      }
    }

    // =========================================================================
    //  BRANCHES CRUD & RENDERING
    // =========================================================================
    function renderConfigBranches() {
      initEnterpriseConfig();
      var list = appState.config.branches || [];
      var tbody = document.getElementById('cfg-branches-table-body');
      if (!tbody) return;

      var totalStaff = 0;
      var html = '';
      if (list.length === 0) {
        html = '<tr><td colspan="9" style="text-align:center; padding:30px; color:var(--text-dim);">No branches registered yet. Click "+ Add New Branch" to create one.</td></tr>';
      } else {
        list.forEach(function(b, idx) {
          totalStaff += (b.staffCount || 0);
          var statusBadge = (b.status === 'Active') 
            ? '<span class="badge badge-success">Active</span>' 
            : '<span class="badge badge-warning">' + (b.status || 'Inactive') + '</span>';

          html += '<tr>' +
            '<td><b style="font-family:var(--font-mono); color:var(--primary); font-size:12px;">' + b.code + '</b></td>' +
            '<td><div style="font-weight:700; color:var(--text);">' + b.name + '</div></td>' +
            '<td>' + (b.location || '-') + '</td>' +
            '<td>' + (b.contact || '-') + '</td>' +
            '<td><code style="font-size:11px;">' + (b.phone || '-') + '</code></td>' +
            '<td><span class="badge" style="background:rgba(255,255,255,0.06); font-family:var(--font-mono); font-size:11px;">' + (b.ipRange || 'All IPs') + '</span></td>' +
            '<td><span class="badge badge-primary">' + (b.staffCount || 0) + ' Staff</span></td>' +
            '<td>' + statusBadge + '</td>' +
            '<td style="text-align:right;">' +
              '<button type="button" class="btn btn-outline btn-xs" onclick="editBranch(' + idx + ')" style="padding:4px 8px; margin-right:6px;">✏️ Edit</button>' +
              '<button type="button" class="btn btn-outline btn-xs" onclick="deleteBranch(' + idx + ')" style="padding:4px 8px; color:#ef4444; border-color:rgba(239,68,68,0.3);">🗑️</button>' +
            '</td>' +
          '</tr>';
        });
      }
      tbody.innerHTML = html;

      if (document.getElementById('cfg-kpi-branches-total')) document.getElementById('cfg-kpi-branches-total').innerText = list.length;
      if (document.getElementById('cfg-kpi-branches-headcount')) document.getElementById('cfg-kpi-branches-headcount').innerText = totalStaff || 20;
      updateConfigBadges();
    }

    function openAddBranchModal() {
      document.getElementById('modal-branch-title').innerText = '📍 Add New Branch';
      document.getElementById('modal-branch-id').value = '';
      document.getElementById('modal-branch-name').value = '';
      document.getElementById('modal-branch-code').value = '';
      document.getElementById('modal-branch-location').value = '';
      document.getElementById('modal-branch-contact').value = '';
      document.getElementById('modal-branch-phone').value = '';
      document.getElementById('modal-branch-ip').value = '';
      document.getElementById('modal-branch-status').value = 'Active';
      openModal('modal-cfg-branch');
    }

    function editBranch(idx) {
      var b = appState.config.branches[idx];
      if (!b) return;
      document.getElementById('modal-branch-title').innerText = '📍 Edit Branch: ' + b.code;
      document.getElementById('modal-branch-id').value = idx;
      document.getElementById('modal-branch-name').value = b.name || '';
      document.getElementById('modal-branch-code').value = b.code || '';
      document.getElementById('modal-branch-location').value = b.location || '';
      document.getElementById('modal-branch-contact').value = b.contact || '';
      document.getElementById('modal-branch-phone').value = b.phone || '';
      document.getElementById('modal-branch-ip').value = b.ipRange || '';
      document.getElementById('modal-branch-status').value = b.status || 'Active';
      openModal('modal-cfg-branch');
    }

    function saveBranchModal() {
      var name = document.getElementById('modal-branch-name').value.trim();
      var code = document.getElementById('modal-branch-code').value.trim().toUpperCase();
      if (!name || !code) return showToast("Branch Name and Code are required", "warning");

      var idx = document.getElementById('modal-branch-id').value;
      var obj = {
        id: (idx !== '') ? appState.config.branches[idx].id : ('BR-' + String(appState.config.branches.length + 1).padStart(2, '0')),
        name: name,
        code: code,
        location: document.getElementById('modal-branch-location').value.trim(),
        contact: document.getElementById('modal-branch-contact').value.trim(),
        phone: document.getElementById('modal-branch-phone').value.trim(),
        ipRange: document.getElementById('modal-branch-ip').value.trim() || '192.168.0.0/24',
        staffCount: (idx !== '') ? appState.config.branches[idx].staffCount : 0,
        status: document.getElementById('modal-branch-status').value
      };

      if (idx !== '') {
        appState.config.branches[idx] = obj;
        showToast("Branch " + code + " updated successfully", "success");
      } else {
        appState.config.branches.push(obj);
        showToast("New branch " + code + " added successfully", "success");
      }

      saveConfigToStorage();
      closeModal('modal-cfg-branch');
      renderConfigBranches();
    }

    function deleteBranch(idx) {
      var b = appState.config.branches[idx];
      if (!b) return;
      if (!confirm("Are you sure you want to delete branch " + b.name + " (" + b.code + ")?")) return;
      appState.config.branches.splice(idx, 1);
      saveConfigToStorage();
      renderConfigBranches();
      showToast("Branch removed.", "info");
    }

    // =========================================================================
    //  SHIFTS CRUD & RENDERING
    // =========================================================================
    function renderConfigShifts() {
      initEnterpriseConfig();
      var list = appState.config.shifts || [];
      var tbody = document.getElementById('cfg-shifts-table-body');
      if (!tbody) return;

      var html = '';
      if (list.length === 0) {
        html = '<tr><td colspan="10" style="text-align:center; padding:30px; color:var(--text-dim);">No shifts configured. Click "+ Create New Shift" to add.</td></tr>';
      } else {
        list.forEach(function(s, idx) {
          var statusBadge = (s.status === 'Active') 
            ? '<span class="badge badge-success">Active</span>' 
            : '<span class="badge badge-warning">' + (s.status || 'Inactive') + '</span>';

          var allowanceText = s.allowance ? ('₹' + s.allowance + '/day') : 'None';

          html += '<tr>' +
            '<td><span class="badge" style="background:rgba(255,255,255,0.06); font-family:var(--font-mono); font-size:11px; font-weight:700; color:var(--primary);">' + s.code + '</span></td>' +
            '<td><div style="font-weight:700; color:var(--text);">' + s.name + '</div></td>' +
            '<td><code style="color:var(--text); font-weight:700;">' + s.start + ' - ' + s.end + '</code></td>' +
            '<td>8.0 hrs</td>' +
            '<td><span class="badge badge-accent">' + (s.grace || 15) + ' mins</span></td>' +
            '<td>' + (s.halfDayMinHours || 4.5) + ' hrs</td>' +
            '<td>' + (s.breakDuration || 60) + ' mins</td>' +
            '<td><b style="color:var(--success); font-family:var(--font-mono);">' + allowanceText + '</b></td>' +
            '<td>' + statusBadge + '</td>' +
            '<td style="text-align:right;">' +
              '<button type="button" class="btn btn-outline btn-xs" onclick="editShift(' + idx + ')" style="padding:4px 8px; margin-right:6px;">✏️ Edit</button>' +
              '<button type="button" class="btn btn-outline btn-xs" onclick="deleteShift(' + idx + ')" style="padding:4px 8px; color:#ef4444; border-color:rgba(239,68,68,0.3);">🗑️</button>' +
            '</td>' +
          '</tr>';
        });
      }
      tbody.innerHTML = html;

      if (document.getElementById('cfg-kpi-shifts-total')) document.getElementById('cfg-kpi-shifts-total').innerText = list.length;
      updateConfigBadges();
    }

    function openAddShiftModal() {
      document.getElementById('modal-shift-title').innerText = '⏱️ Add Shift Configuration';
      document.getElementById('modal-shift-id').value = '';
      document.getElementById('modal-shift-name').value = '';
      document.getElementById('modal-shift-code').value = '';
      document.getElementById('modal-shift-start').value = '09:00';
      document.getElementById('modal-shift-end').value = '17:00';
      document.getElementById('modal-shift-grace').value = '15';
      document.getElementById('modal-shift-halfday').value = '4.5';
      document.getElementById('modal-shift-break').value = '60';
      document.getElementById('modal-shift-allowance').value = '0';
      document.getElementById('modal-shift-status').value = 'Active';
      openModal('modal-cfg-shift');
    }

    function editShift(idx) {
      var s = appState.config.shifts[idx];
      if (!s) return;
      document.getElementById('modal-shift-title').innerText = '⏱️ Edit Shift: ' + s.name;
      document.getElementById('modal-shift-id').value = idx;
      document.getElementById('modal-shift-name').value = s.name || '';
      document.getElementById('modal-shift-code').value = s.code || '';
      document.getElementById('modal-shift-start').value = s.start || '09:00';
      document.getElementById('modal-shift-end').value = s.end || '17:00';
      document.getElementById('modal-shift-grace').value = s.grace || 15;
      document.getElementById('modal-shift-halfday').value = s.halfDayMinHours || 4.5;
      document.getElementById('modal-shift-break').value = s.breakDuration || 60;
      document.getElementById('modal-shift-allowance').value = s.allowance || 0;
      document.getElementById('modal-shift-status').value = s.status || 'Active';
      openModal('modal-cfg-shift');
    }

    function saveShiftModal() {
      var name = document.getElementById('modal-shift-name').value.trim();
      var code = document.getElementById('modal-shift-code').value.trim().toUpperCase();
      if (!name || !code) return showToast("Shift Name and Code are required", "warning");

      var idx = document.getElementById('modal-shift-id').value;
      var obj = {
        id: (idx !== '') ? appState.config.shifts[idx].id : ('SH-' + code),
        name: name,
        code: code,
        start: document.getElementById('modal-shift-start').value || '09:00',
        end: document.getElementById('modal-shift-end').value || '17:00',
        grace: parseInt(document.getElementById('modal-shift-grace').value || 15),
        halfDayMinHours: parseFloat(document.getElementById('modal-shift-halfday').value || 4.5),
        breakDuration: parseInt(document.getElementById('modal-shift-break').value || 60),
        allowance: parseFloat(document.getElementById('modal-shift-allowance').value || 0),
        status: document.getElementById('modal-shift-status').value
      };

      if (idx !== '') {
        appState.config.shifts[idx] = obj;
        showToast("Shift " + code + " updated successfully", "success");
      } else {
        appState.config.shifts.push(obj);
        showToast("New Shift " + code + " added", "success");
      }

      saveConfigToStorage();
      closeModal('modal-cfg-shift');
      renderConfigShifts();
    }

    function deleteShift(idx) {
      var s = appState.config.shifts[idx];
      if (!s) return;
      if (!confirm("Are you sure you want to delete shift " + s.name + " (" + s.code + ")?")) return;
      appState.config.shifts.splice(idx, 1);
      saveConfigToStorage();
      renderConfigShifts();
      showToast("Shift schedule removed.", "info");
    }

    // =========================================================================
    //  LEAVE CONFIGURATION CRUD & RENDERING
    // =========================================================================
    function renderConfigLeaves() {
      initEnterpriseConfig();
      var list = appState.config.leaves || [];
      var tbody = document.getElementById('cfg-leaves-table-body');
      if (!tbody) return;

      var totalQuota = 0;
      var html = '';
      if (list.length === 0) {
        html = '<tr><td colspan="8" style="text-align:center; padding:30px; color:var(--text-dim);">No leave types defined. Click "+ Add Leave Type".</td></tr>';
      } else {
        list.forEach(function(l, idx) {
          if (l.paid && l.quota < 100) totalQuota += (l.quota || 0);

          var carryBadge = l.carryForward 
            ? '<span class="badge badge-success">Yes (Max 10)</span>' 
            : '<span class="badge" style="background:rgba(255,255,255,0.06); color:var(--text-dim);">No (Lapses)</span>';

          var encashBadge = l.encashable 
            ? '<span class="badge badge-primary">Encashable</span>' 
            : '<span class="badge" style="background:rgba(255,255,255,0.06); color:var(--text-dim);">Non-Encashable</span>';

          var paidBadge = l.paid 
            ? '<span class="badge badge-accent">100% Paid</span>' 
            : '<span class="badge badge-danger">Unpaid / LOP</span>';

          var appBadge = l.requiresApproval 
            ? '<span class="badge badge-warning">HR Approval</span>' 
            : '<span class="badge badge-success">Auto-Approved</span>';

          html += '<tr>' +
            '<td><b style="font-family:var(--font-mono); color:var(--primary); font-size:12px;">' + l.code + '</b></td>' +
            '<td><div style="font-weight:700; color:var(--text);">' + l.name + '</div></td>' +
            '<td><b style="font-family:var(--font-mono); font-size:13px; color:var(--text);">' + l.quota + ' Days/yr</b></td>' +
            '<td>' + carryBadge + '</td>' +
            '<td>' + encashBadge + '</td>' +
            '<td>' + paidBadge + '</td>' +
            '<td>' + appBadge + '</td>' +
            '<td style="text-align:right;">' +
              '<button type="button" class="btn btn-outline btn-xs" onclick="editLeave(' + idx + ')" style="padding:4px 8px; margin-right:6px;">✏️ Edit</button>' +
              '<button type="button" class="btn btn-outline btn-xs" onclick="deleteLeave(' + idx + ')" style="padding:4px 8px; color:#ef4444; border-color:rgba(239,68,68,0.3);">🗑️</button>' +
            '</td>' +
          '</tr>';
        });
      }
      tbody.innerHTML = html;

      if (document.getElementById('cfg-kpi-leaves-total')) document.getElementById('cfg-kpi-leaves-total').innerText = list.length;
      updateConfigBadges();
    }

    function openAddLeaveModal() {
      document.getElementById('modal-leave-title').innerText = '🏖️ Add Leave Category';
      document.getElementById('modal-leave-id').value = '';
      document.getElementById('modal-leave-name').value = '';
      document.getElementById('modal-leave-code').value = '';
      document.getElementById('modal-leave-quota').value = '12';
      document.getElementById('modal-leave-carry').value = 'false';
      document.getElementById('modal-leave-encash').value = 'false';
      document.getElementById('modal-leave-paid').value = 'true';
      document.getElementById('modal-leave-approval').value = 'true';
      openModal('modal-cfg-leave');
    }

    function editLeave(idx) {
      var l = appState.config.leaves[idx];
      if (!l) return;
      document.getElementById('modal-leave-title').innerText = '🏖️ Edit Leave: ' + l.name;
      document.getElementById('modal-leave-id').value = idx;
      document.getElementById('modal-leave-name').value = l.name || '';
      document.getElementById('modal-leave-code').value = l.code || '';
      document.getElementById('modal-leave-quota').value = l.quota || 12;
      document.getElementById('modal-leave-carry').value = l.carryForward ? 'true' : 'false';
      document.getElementById('modal-leave-encash').value = l.encashable ? 'true' : 'false';
      document.getElementById('modal-leave-paid').value = l.paid ? 'true' : 'false';
      document.getElementById('modal-leave-approval').value = l.requiresApproval ? 'true' : 'false';
      openModal('modal-cfg-leave');
    }

    function saveLeaveModal() {
      var name = document.getElementById('modal-leave-name').value.trim();
      var code = document.getElementById('modal-leave-code').value.trim().toUpperCase();
      if (!name || !code) return showToast("Leave Name and Code are required", "warning");

      var idx = document.getElementById('modal-leave-id').value;
      var obj = {
        id: (idx !== '') ? appState.config.leaves[idx].id : ('LV-' + code),
        name: name,
        code: code,
        quota: parseInt(document.getElementById('modal-leave-quota').value || 12),
        carryForward: (document.getElementById('modal-leave-carry').value === 'true'),
        encashable: (document.getElementById('modal-leave-encash').value === 'true'),
        paid: (document.getElementById('modal-leave-paid').value === 'true'),
        requiresApproval: (document.getElementById('modal-leave-approval').value === 'true')
      };

      if (idx !== '') {
        appState.config.leaves[idx] = obj;
        showToast("Leave category " + code + " updated", "success");
      } else {
        appState.config.leaves.push(obj);
        showToast("New leave category " + code + " added", "success");
      }

      saveConfigToStorage();
      closeModal('modal-cfg-leave');
      renderConfigLeaves();
    }

    function deleteLeave(idx) {
      var l = appState.config.leaves[idx];
      if (!l) return;
      if (!confirm("Delete leave category " + l.name + " (" + l.code + ")?")) return;
      appState.config.leaves.splice(idx, 1);
      saveConfigToStorage();
      renderConfigLeaves();
      showToast("Leave category removed.", "info");
    }

    // =========================================================================
    //  DEPARTMENTS CRUD & RENDERING
    // =========================================================================
    function renderConfigDepartments() {
      initEnterpriseConfig();
      var list = appState.config.departments || [];
      var tbody = document.getElementById('cfg-depts-table-body');
      if (!tbody) return;

      var totalMapped = 0;
      var html = '';
      if (list.length === 0) {
        html = '<tr><td colspan="8" style="text-align:center; padding:30px; color:var(--text-dim);">No departments configured. Click "+ Add Department".</td></tr>';
      } else {
        list.forEach(function(d, idx) {
          totalMapped += (d.staffCount || 0);
          var statusBadge = (d.status === 'Active') 
            ? '<span class="badge badge-success">Active</span>' 
            : '<span class="badge badge-warning">' + (d.status || 'Inactive') + '</span>';

          html += '<tr>' +
            '<td><b style="font-family:var(--font-mono); color:var(--primary); font-size:12px;">' + d.code + '</b></td>' +
            '<td><div style="font-weight:700; color:var(--text);">' + d.name + '</div></td>' +
            '<td><div style="display:flex; align-items:center; gap:6px;"><span>👔</span><b>' + (d.head || 'Unassigned') + '</b></div></td>' +
            '<td><span class="badge badge-primary">' + (d.staffCount || 0) + ' Staff</span></td>' +
            '<td>' + (d.location || '-') + '</td>' +
            '<td><code style="font-size:11px;">' + (d.email || '-') + '</code></td>' +
            '<td>' + statusBadge + '</td>' +
            '<td style="text-align:right;">' +
              '<button type="button" class="btn btn-outline btn-xs" onclick="editDept(' + idx + ')" style="padding:4px 8px; margin-right:6px;">✏️ Edit</button>' +
              '<button type="button" class="btn btn-outline btn-xs" onclick="deleteDept(' + idx + ')" style="padding:4px 8px; color:#ef4444; border-color:rgba(239,68,68,0.3);">🗑️</button>' +
            '</td>' +
          '</tr>';
        });
      }
      tbody.innerHTML = html;

      if (document.getElementById('cfg-kpi-depts-total')) document.getElementById('cfg-kpi-depts-total').innerText = list.length;
      if (document.getElementById('cfg-kpi-depts-staff')) document.getElementById('cfg-kpi-depts-staff').innerText = totalMapped || 20;
      updateConfigBadges();
      populateDeptSelectOptions();
    }

    function populateDeptSelectOptions() {
      var sel = document.getElementById('modal-desig-dept');
      if (!sel) return;
      var list = appState.config.departments || [];
      var html = '';
      list.forEach(function(d){
        html += '<option value="' + d.name + '">' + d.name + ' (' + d.code + ')</option>';
      });
      sel.innerHTML = html;
    }

    function openAddDeptModal() {
      document.getElementById('modal-dept-title').innerText = '🏛️ Add Department';
      document.getElementById('modal-dept-id').value = '';
      document.getElementById('modal-dept-name').value = '';
      document.getElementById('modal-dept-code').value = '';
      document.getElementById('modal-dept-head').value = '';
      document.getElementById('modal-dept-location').value = '';
      document.getElementById('modal-dept-email').value = '';
      openModal('modal-cfg-dept');
    }

    function editDept(idx) {
      var d = appState.config.departments[idx];
      if (!d) return;
      document.getElementById('modal-dept-title').innerText = '🏛️ Edit Department: ' + d.name;
      document.getElementById('modal-dept-id').value = idx;
      document.getElementById('modal-dept-name').value = d.name || '';
      document.getElementById('modal-dept-code').value = d.code || '';
      document.getElementById('modal-dept-head').value = d.head || '';
      document.getElementById('modal-dept-location').value = d.location || '';
      document.getElementById('modal-dept-email').value = d.email || '';
      openModal('modal-cfg-dept');
    }

    function saveDeptModal() {
      var name = document.getElementById('modal-dept-name').value.trim();
      var code = document.getElementById('modal-dept-code').value.trim().toUpperCase();
      if (!name || !code) return showToast("Department Name and Code are required", "warning");

      var idx = document.getElementById('modal-dept-id').value;
      var obj = {
        id: (idx !== '') ? appState.config.departments[idx].id : ('DP-' + code),
        name: name,
        code: code,
        head: document.getElementById('modal-dept-head').value.trim() || 'Unassigned',
        staffCount: (idx !== '') ? appState.config.departments[idx].staffCount : 0,
        location: document.getElementById('modal-dept-location').value.trim(),
        email: document.getElementById('modal-dept-email').value.trim(),
        status: 'Active'
      };

      if (idx !== '') {
        appState.config.departments[idx] = obj;
        showToast("Department " + code + " updated", "success");
      } else {
        appState.config.departments.push(obj);
        showToast("New department " + code + " created", "success");
      }

      saveConfigToStorage();
      closeModal('modal-cfg-dept');
      renderConfigDepartments();
    }

    function deleteDept(idx) {
      var d = appState.config.departments[idx];
      if (!d) return;
      if (!confirm("Delete department " + d.name + " (" + d.code + ")?")) return;
      appState.config.departments.splice(idx, 1);
      saveConfigToStorage();
      renderConfigDepartments();
      showToast("Department removed.", "info");
    }

    // =========================================================================
    //  DESIGNATIONS CRUD & RENDERING
    // =========================================================================
    function renderConfigDesignations() {
      initEnterpriseConfig();
      var list = appState.config.designations || [];
      var tbody = document.getElementById('cfg-desig-table-body');
      if (!tbody) return;

      var html = '';
      if (list.length === 0) {
        html = '<tr><td colspan="7" style="text-align:center; padding:30px; color:var(--text-dim);">No designations configured. Click "+ Add Designation".</td></tr>';
      } else {
        list.forEach(function(ds, idx) {
          var gradeBadge = '<span class="badge badge-accent">' + (ds.grade || 'L3') + '</span>';
          var payText = (ds.minPay && ds.maxPay) 
            ? ('₹' + (ds.minPay/1000) + 'k - ₹' + (ds.maxPay/1000) + 'k') 
            : 'Standard';

          html += '<tr>' +
            '<td><div style="font-weight:700; color:var(--text);">' + ds.title + '</div></td>' +
            '<td><span class="badge" style="background:rgba(255,255,255,0.06); font-size:11px;">' + (ds.department || 'General') + '</span></td>' +
            '<td>' + gradeBadge + '</td>' +
            '<td><b style="font-family:var(--font-mono); color:var(--success);">' + payText + '</b></td>' +
            '<td><span class="badge badge-primary">' + (ds.staffCount || 1) + ' Staff</span></td>' +
            '<td><span class="badge badge-success">Active</span></td>' +
            '<td style="text-align:right;">' +
              '<button type="button" class="btn btn-outline btn-xs" onclick="editDesig(' + idx + ')" style="padding:4px 8px; margin-right:6px;">✏️ Edit</button>' +
              '<button type="button" class="btn btn-outline btn-xs" onclick="deleteDesig(' + idx + ')" style="padding:4px 8px; color:#ef4444; border-color:rgba(239,68,68,0.3);">🗑️</button>' +
            '</td>' +
          '</tr>';
        });
      }
      tbody.innerHTML = html;

      if (document.getElementById('cfg-kpi-desig-total')) document.getElementById('cfg-kpi-desig-total').innerText = list.length;
      updateConfigBadges();
    }

    function openAddDesigModal() {
      populateDeptSelectOptions();
      document.getElementById('modal-desig-title').innerText = '💼 Add Designation';
      document.getElementById('modal-desig-id').value = '';
      document.getElementById('modal-desig-title-input').value = '';
      document.getElementById('modal-desig-grade').value = 'L3';
      document.getElementById('modal-desig-min').value = '45000';
      document.getElementById('modal-desig-max').value = '75000';
      openModal('modal-cfg-desig');
    }

    function editDesig(idx) {
      populateDeptSelectOptions();
      var ds = appState.config.designations[idx];
      if (!ds) return;
      document.getElementById('modal-desig-title').innerText = '💼 Edit Designation: ' + ds.title;
      document.getElementById('modal-desig-id').value = idx;
      document.getElementById('modal-desig-title-input').value = ds.title || '';
      document.getElementById('modal-desig-dept').value = ds.department || '';
      document.getElementById('modal-desig-grade').value = ds.grade || 'L3';
      document.getElementById('modal-desig-min').value = ds.minPay || 45000;
      document.getElementById('modal-desig-max').value = ds.maxPay || 75000;
      openModal('modal-cfg-desig');
    }

    function saveDesigModal() {
      var title = document.getElementById('modal-desig-title-input').value.trim();
      if (!title) return showToast("Designation Title is required", "warning");

      var idx = document.getElementById('modal-desig-id').value;
      var obj = {
        id: (idx !== '') ? appState.config.designations[idx].id : ('DS-' + String(appState.config.designations.length + 1).padStart(2, '0')),
        title: title,
        department: document.getElementById('modal-desig-dept').value,
        grade: document.getElementById('modal-desig-grade').value,
        minPay: parseFloat(document.getElementById('modal-desig-min').value || 30000),
        maxPay: parseFloat(document.getElementById('modal-desig-max').value || 60000),
        staffCount: (idx !== '') ? appState.config.designations[idx].staffCount : 1,
        status: 'Active'
      };

      if (idx !== '') {
        appState.config.designations[idx] = obj;
        showToast("Designation " + title + " updated", "success");
      } else {
        appState.config.designations.push(obj);
        showToast("New designation " + title + " created", "success");
      }

      saveConfigToStorage();
      closeModal('modal-cfg-desig');
      renderConfigDesignations();
    }

    function deleteDesig(idx) {
      var ds = appState.config.designations[idx];
      if (!ds) return;
      if (!confirm("Delete designation " + ds.title + "?")) return;
      appState.config.designations.splice(idx, 1);
      saveConfigToStorage();
      renderConfigDesignations();
      showToast("Designation removed.", "info");
    }

    // =========================================================================
    //  HOLIDAYS CRUD & RENDERING
    // =========================================================================
    var currentHolidayFilterYear = '2026';
    function filterHolidaysByYear(yr) {
      currentHolidayFilterYear = yr;
      renderConfigHolidays();
    }

    function renderConfigHolidays() {
      initEnterpriseConfig();
      var list = appState.config.holidays || [];
      var tbody = document.getElementById('cfg-holidays-table-body');
      if (!tbody) return;

      var filtered = list.filter(function(h){
        return !h.year || h.year === currentHolidayFilterYear || (h.date && h.date.startsWith(currentHolidayFilterYear));
      });

      var html = '';
      if (filtered.length === 0) {
        html = '<tr><td colspan="7" style="text-align:center; padding:30px; color:var(--text-dim);">No holidays listed for Year ' + currentHolidayFilterYear + '. Click "+ Add Holiday".</td></tr>';
      } else {
        filtered.forEach(function(h, idx) {
          var realIdx = list.indexOf(h);
          var typeBadge = (h.type === 'Gazetted National') 
            ? '<span class="badge badge-accent">🇮🇳 ' + h.type + '</span>' 
            : '<span class="badge" style="background:rgba(255,255,255,0.06);">' + h.type + '</span>';

          var paidBadge = h.paid 
            ? '<span class="badge badge-success">Paid Holiday</span>' 
            : '<span class="badge badge-danger">Unpaid</span>';

          html += '<tr>' +
            '<td><b style="font-family:var(--font-mono); color:var(--text); font-size:12px;">📅 ' + formatDateDisplay(h.date) + '</b></td>' +
            '<td><span style="color:var(--text-dim);">' + (h.day || '-') + '</span></td>' +
            '<td><div style="font-weight:700; color:var(--text); font-size:13px;">' + h.name + '</div></td>' +
            '<td>' + typeBadge + '</td>' +
            '<td>' + paidBadge + '</td>' +
            '<td><span class="badge badge-success">Scheduled</span></td>' +
            '<td style="text-align:right;">' +
              '<button type="button" class="btn btn-outline btn-xs" onclick="editHoliday(' + realIdx + ')" style="padding:4px 8px; margin-right:6px;">✏️ Edit</button>' +
              '<button type="button" class="btn btn-outline btn-xs" onclick="deleteHoliday(' + realIdx + ')" style="padding:4px 8px; color:#ef4444; border-color:rgba(239,68,68,0.3);">🗑️</button>' +
            '</td>' +
          '</tr>';
        });
      }
      tbody.innerHTML = html;

      if (document.getElementById('cfg-kpi-holidays-total')) document.getElementById('cfg-kpi-holidays-total').innerText = filtered.length;
      updateConfigBadges();
    }

    function openAddHolidayModal() {
      document.getElementById('modal-holiday-title').innerText = '🗓️ Add Official Holiday';
      document.getElementById('modal-holiday-id').value = '';
      document.getElementById('modal-holiday-name').value = '';
      document.getElementById('modal-holiday-date').value = currentHolidayFilterYear + '-08-15';
      document.getElementById('modal-holiday-type').value = 'Gazetted National';
      document.getElementById('modal-holiday-paid').value = 'true';
      openModal('modal-cfg-holiday');
    }

    function editHoliday(realIdx) {
      var h = appState.config.holidays[realIdx];
      if (!h) return;
      document.getElementById('modal-holiday-title').innerText = '🗓️ Edit Holiday: ' + h.name;
      document.getElementById('modal-holiday-id').value = realIdx;
      document.getElementById('modal-holiday-name').value = h.name || '';
      document.getElementById('modal-holiday-date').value = h.date || '';
      document.getElementById('modal-holiday-type').value = h.type || 'Gazetted National';
      document.getElementById('modal-holiday-paid').value = h.paid ? 'true' : 'false';
      openModal('modal-cfg-holiday');
    }

    function saveHolidayModal() {
      var name = document.getElementById('modal-holiday-name').value.trim();
      var dateStr = document.getElementById('modal-holiday-date').value;
      if (!name || !dateStr) return showToast("Holiday Name and Date are required", "warning");

      var d = new Date(dateStr + 'T12:00:00Z');
      var days = ['Sunday', 'Monday', 'Tuesday', 'Wednesday', 'Thursday', 'Friday', 'Saturday'];
      var dayName = days[d.getUTCDay()];
      var yearStr = String(d.getUTCFullYear());

      var idx = document.getElementById('modal-holiday-id').value;
      var obj = {
        id: (idx !== '') ? appState.config.holidays[idx].id : ('HD-' + String(appState.config.holidays.length + 1).padStart(2, '0')),
        name: name,
        date: dateStr,
        day: dayName,
        year: yearStr,
        type: document.getElementById('modal-holiday-type').value,
        paid: (document.getElementById('modal-holiday-paid').value === 'true')
      };

      if (idx !== '') {
        appState.config.holidays[idx] = obj;
        showToast("Holiday " + name + " updated", "success");
      } else {
        appState.config.holidays.push(obj);
        showToast("New holiday " + name + " added to calendar", "success");
      }

      saveConfigToStorage();
      closeModal('modal-cfg-holiday');
      renderConfigHolidays();
    }

    function deleteHoliday(realIdx) {
      var h = appState.config.holidays[realIdx];
      if (!h) return;
      if (!confirm("Delete holiday " + h.name + " (" + h.date + ")?")) return;
      appState.config.holidays.splice(realIdx, 1);
      saveConfigToStorage();
      renderConfigHolidays();
      showToast("Holiday removed from calendar.", "info");
    }

    // =========================================================================
    //  ROLES & PERMISSIONS CRUD & RENDERING
    // =========================================================================
    var permissionModules = [
      { key: 'dashboard', label: 'Executive Dashboard & Live Analytics' },
      { key: 'devices', label: 'ESP32 Device Gateway & Firmware R503' },
      { key: 'biometrics', label: 'Biometric Enrollment & Template Sync' },
      { key: 'employees_view', label: 'View Employee Directory' },
      { key: 'employees_edit', label: 'Add / Edit / Remove Staff Records' },
      { key: 'attendance_view', label: 'Daily & Monthly Attendance Ledger' },
      { key: 'attendance_edit', label: 'Attendance Regularization & Manual Punches' },
      { key: 'leave_approve', label: 'Approve / Reject Leave Requests' },
      { key: 'payroll_view', label: 'View Cloud Payroll & Wage Slips' },
      { key: 'payroll_edit', label: 'Execute Monthly Payroll Calculations' },
      { key: 'audit_view', label: 'Inspect 24/7 Tamper-proof Audit Trail' },
      { key: 'config_view', label: 'Access System & Master Configuration' },
      { key: 'config_edit', label: 'Modify Organizational Rules & Secrets' }
    ];

    function renderConfigRolesMatrix() {
      initEnterpriseConfig();
      var roles = appState.config.roles || [];
      var tbody = document.getElementById('cfg-roles-matrix-body');
      if (!tbody) return;

      var html = '';
      permissionModules.forEach(function(mod) {
        html += '<tr>' +
          '<td><b style="font-size:12.5px; color:var(--text);">' + mod.label + '</b></td>';

        roles.forEach(function(r, rIdx) {
          var isChecked = (r.perms && r.perms[mod.key]) ? 'checked' : '';
          var isDisabled = (r.code === 'ADMIN') ? 'disabled checked' : '';
          html += '<td><input type="checkbox" class="perm-check" data-role-idx="' + rIdx + '" data-perm-key="' + mod.key + '" ' + isChecked + ' ' + isDisabled + ' onchange="toggleRolePerm(' + rIdx + ', \\'' + mod.key + '\\', this.checked)"></td>';
        });

        html += '</tr>';
      });
      tbody.innerHTML = html;

      // Render Role Overview Cards
      var cardsContainer = document.getElementById('cfg-role-cards-container');
      if (cardsContainer) {
        var cHtml = '';
        roles.forEach(function(r) {
          cHtml += '<div class="config-rule-box" style="margin-bottom:0;">' +
            '<div style="display:flex; justify-content:space-between; align-items:center; margin-bottom:6px;">' +
              '<h4 style="font-family:var(--font-heading); font-size:14px; font-weight:700; color:var(--text);">' + r.role + '</h4>' +
              '<span class="badge ' + (r.badge || 'badge-primary') + '">' + r.code + '</span>' +
            '</div>' +
            '<p style="font-size:11.5px; color:var(--text-dim); line-height:1.4; margin-bottom:8px;">' + r.desc + '</p>' +
            '<div style="display:flex; justify-content:space-between; font-size:11px; color:var(--text-muted); border-top:1px solid rgba(255,255,255,0.06); padding-top:6px;">' +
              '<span>Assigned Users:</span>' +
              '<b style="color:var(--text); font-family:var(--font-mono);">' + (r.usersCount || 0) + ' Accounts</b>' +
            '</div>' +
          '</div>';
        });
        cardsContainer.innerHTML = cHtml;
      }
      updateConfigBadges();
    }

    function toggleRolePerm(rIdx, permKey, checked) {
      if (!appState.config.roles[rIdx]) return;
      if (!appState.config.roles[rIdx].perms) appState.config.roles[rIdx].perms = {};
      appState.config.roles[rIdx].perms[permKey] = checked;
    }

    function saveRolesMatrix() {
      saveConfigToStorage();
      showToast("Role-Based Access Control matrix saved successfully!", "success");
    }

    function openAddRoleModal() {
      document.getElementById('modal-role-name').value = '';
      document.getElementById('modal-role-code').value = '';
      document.getElementById('modal-role-desc').value = '';
      openModal('modal-cfg-role');
    }

    function saveCustomRoleModal() {
      var name = document.getElementById('modal-role-name').value.trim();
      var code = document.getElementById('modal-role-code').value.trim().toUpperCase();
      var desc = document.getElementById('modal-role-desc').value.trim();
      if (!name || !code) return showToast("Role Name and Code are required", "warning");

      appState.config.roles.push({
        role: name,
        code: code,
        desc: desc || 'Custom defined administrative role',
        usersCount: 0,
        badge: 'badge-accent',
        perms: {
          dashboard: true,
          devices: false,
          biometrics: false,
          employees_view: true,
          employees_edit: false,
          attendance_view: true,
          attendance_edit: false,
          attendance_manual: false,
          payroll_view: false,
          payroll_edit: false,
          leave_approve: false,
          audit_view: true,
          config_view: false,
          config_edit: false
        }
      });

      saveConfigToStorage();
      closeModal('modal-cfg-role');
      renderConfigRolesMatrix();
      showToast("Custom role " + name + " created", "success");
    }

    // Modal Helper
    function openModal(modalId) {
      var m = document.getElementById(modalId);
      if (m) m.style.display = 'flex';
    }
    function closeModal(modalId) {
      var m = document.getElementById(modalId);
      if (m) m.style.display = 'none';
    }
`;

targetFiles.forEach(file => {
  const filePath = path.join(__dirname, file);
  if (!fs.existsSync(filePath)) {
    console.log(`File not found: ${file}`);
    return;
  }

  let content = fs.readFileSync(filePath, 'utf8');
  console.log(`\nProcessing ${file}...`);

  // 1. Inject CSS before </style>
  if (!content.includes('/* =========================================================================\n       FULL-SIZE CONFIGURATION SECTION WITH OWN SUB-SIDEBAR')) {
    content = content.replace('</style>', cssToInject + '\n  </style>');
    console.log('✓ Injected comprehensive linear styling and configuration layout CSS');
  } else {
    console.log('• Configuration CSS already present');
  }

  // 2. Update Sidebar Configuration Button
  const oldCfgBtnRegex = /<button class="nav-btn" id="nav-configuration" onclick="openSettingsModal\(\)">[\s\S]*?<\/button>/;
  const newCfgBtn = `<button class="nav-btn" id="nav-configuration" onclick="switchView('configuration', this)">
          <span class="nav-icon">⚙️</span>
          <span class="nav-label">Configuration</span>
          <span class="nav-pill" style="color:var(--primary); font-size:10px;">PRO</span>
        </button>`;

  if (oldCfgBtnRegex.test(content)) {
    content = content.replace(oldCfgBtnRegex, newCfgBtn);
    console.log('✓ Updated main sidebar Configuration button to switchView("configuration", this)');
  }

  // 3. Inject View Configuration HTML before </main>
  if (!content.includes('id="view-configuration"')) {
    content = content.replace('</main>', htmlViewConfiguration + '\n      </main>');
    console.log('✓ Injected #view-configuration and all 10 section panes before </main>');
  } else {
    console.log('• #view-configuration HTML already present');
  }

  // 4. Update switchView in JS to support 'configuration'
  if (!content.includes("'configuration': {")) {
    const targetTitles = "'audit': { title: '24/7 Biometric Punches Ledger', sub: 'Complete tamper-proof audit trail of verified employee check-in and check-out logs' }";
    const replacementTitles = `'audit': { title: '24/7 Biometric Punches Ledger', sub: 'Complete tamper-proof audit trail of verified employee check-in and check-out logs' },
        'configuration': { title: 'System & Enterprise Configuration', sub: 'Master identity, system parameters, branches, shifts, leave policies, departments & roles' }`;

    if (content.includes(targetTitles)) {
      content = content.replace(targetTitles, replacementTitles);
      console.log('✓ Added configuration titles to switchView()');
    }
  }

  // 5. Add view render trigger for configuration in switchView()
  if (!content.includes("if (viewName === 'configuration') renderConfigurationView();")) {
    const targetRenders = "if (viewName === 'att-monthly-all') renderMonthlyAttendanceAll();";
    const replacementRenders = `if (viewName === 'att-monthly-all') renderMonthlyAttendanceAll();
      if (viewName === 'configuration') renderConfigurationView();`;

    if (content.includes(targetRenders)) {
      content = content.replace(targetRenders, replacementRenders);
      console.log('✓ Added renderConfigurationView() call in switchView()');
    }
  }

  // 6. Update openSettingsModal to smoothly delegate to switchView('configuration')
  const oldOpenSettingsRegex = /function openSettingsModal\(\)\s*\{[\s\S]*?document\.getElementById\('settings-modal'\)\.style\.display\s*=\s*'flex';\s*\}/;
  const newOpenSettings = `function openSettingsModal() {
      var navBtn = document.getElementById('nav-configuration');
      switchView('configuration', navBtn);
    }`;

  if (oldOpenSettingsRegex.test(content)) {
    content = content.replace(oldOpenSettingsRegex, newOpenSettings);
    console.log('✓ Redirected openSettingsModal() to switchView("configuration")');
  }

  // 7. Inject JS engine before </script>
  if (!content.includes('//  ENTERPRISE CONFIGURATION ENGINE & MASTER DATA STORE')) {
    content = content.replace('</script>', jsToInject + '\n  </script>');
    console.log('✓ Injected Enterprise Configuration JS Engine and CRUD handlers');
  } else {
    console.log('• Enterprise Configuration JS Engine already present');
  }

  // 8. Call initEnterpriseConfig() in DOMContentLoaded
  if (!content.includes('initEnterpriseConfig();')) {
    content = content.replace(
      '// Initial Dashboard & Attendance Renders',
      'initEnterpriseConfig();\n      // Initial Dashboard & Attendance Renders'
    );
    console.log('✓ Added initEnterpriseConfig() call in DOMContentLoaded');
  }

  fs.writeFileSync(filePath, content, 'utf8');
  console.log(`✓ Successfully updated ${file} (${content.length} bytes)`);
});

console.log('\n================================================================');
console.log('  CONFIGURATION SECTION & LINEAR UI SUCCESSFULLY APPLIED!      ');
console.log('================================================================');
