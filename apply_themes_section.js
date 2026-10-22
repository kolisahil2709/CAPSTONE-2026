const fs = require('fs');
const path = require('path');

console.log('================================================================');
console.log('  CHRONOS MIS: 5-PRESET THEMES & CUSTOM PALETTE STUDIO IN CONFIG');
console.log('================================================================');

const targetFiles = ['index.html', 'cloud_erp.html'];

// 1. CSS TO INJECT FOR THE 5 THEMES & CUSTOM PALETTE CONTROLS
const themesCssToInject = `
    /* =========================================================================
       CHRONOS MIS: 5 CURATED DASHBOARD THEMES & CUSTOMIZER PALETTE STYLES
       1. Orange - Cyan (Cyber Dual-Neon)
       2. Magenta - Beige (Editorial Luxury Linen)
       3. Emerald - Gold (Executive Prestige Fintech)
       4. Midnight - Indigo (Cyber Enterprise Matrix)
       5. Dark Slate - Amber (High-Contrast Classic)
       ========================================================================= */

    /* 1. ORANGE - CYAN (Cyber Dual-Neon) */
    [data-theme="orange-cyan"], [data-theme="cyan-orange"] {
      --bg: #060e1a;
      --surface: #0b172a;
      --surface-card: rgba(11, 23, 42, 0.88);
      --surface-hover: rgba(16, 36, 64, 0.9);
      --sidebar-bg: #081221;
      --border: rgba(6, 182, 212, 0.22);
      --border-accent: rgba(255, 107, 0, 0.45);
      --text: #f0f9ff;
      --text-muted: #94b8d7;
      --text-dim: #597a99;
      --primary: #ff6b00;
      --primary-hover: #ff8533;
      --primary-glow: rgba(255, 107, 0, 0.45);
      --accent: #06b6d4;
      --accent-hover: #22d3ee;
      --accent-glow: rgba(6, 182, 212, 0.4);
      --cyan: #06b6d4;
      --cyan-glow: rgba(6, 182, 212, 0.35);
      --success: #10b981;
      --warning: #f59e0b;
      --danger: #ef4444;
      --shadow-sm: 0 2px 8px rgba(0, 0, 0, 0.4);
      --shadow-md: 0 8px 24px rgba(0, 0, 0, 0.5);
      --shadow-lg: 0 16px 36px rgba(0, 0, 0, 0.6);
      --shadow-glow: 0 0 28px rgba(255, 107, 0, 0.38);
    }
    [data-theme="orange-cyan"] .card,
    [data-theme="orange-cyan"] .content-view .card,
    [data-theme="orange-cyan"] .weekly-chart-card,
    [data-theme="orange-cyan"] .device-gateway-card,
    [data-theme="orange-cyan"] .device-archive-card,
    [data-theme="orange-cyan"] .daily-summary-tile,
    [data-theme="orange-cyan"] .matrix-card,
    [data-theme="orange-cyan"] .kpi-card,
    [data-theme="orange-cyan"] .daily-control-card,
    [data-theme="orange-cyan"] .config-sidebar {
      background: linear-gradient(145deg, rgba(255, 107, 0, 0.05) 0%, rgba(11, 23, 42, 0.88) 100%) !important;
      border: 1px solid rgba(6, 182, 212, 0.22) !important;
      box-shadow: 0 8px 26px rgba(0, 0, 0, 0.38) !important;
    }
    [data-theme="orange-cyan"] .app-topbar {
      background: rgba(8, 18, 33, 0.92) !important;
      border-bottom: 1px solid rgba(6, 182, 212, 0.22) !important;
      box-shadow: 0 4px 20px rgba(0, 0, 0, 0.3) !important;
    }
    [data-theme="orange-cyan"] .nav-btn.active,
    [data-theme="orange-cyan"] .config-nav-btn.active {
      background: linear-gradient(135deg, rgba(255, 107, 0, 0.22) 0%, rgba(6, 182, 212, 0.1) 100%) !important;
      border-color: #ff6b00 !important;
      color: #ff8533 !important;
    }
    [data-theme="orange-cyan"] .badge-primary {
      background: rgba(255, 107, 0, 0.2) !important;
      color: #ff8533 !important;
      border: 1px solid rgba(255, 107, 0, 0.4) !important;
    }
    [data-theme="orange-cyan"] .bar-track,
    [data-theme="orange-cyan"] .kpi-progress-track {
      background: rgba(6, 182, 212, 0.12) !important;
    }

    /* 2. MAGENTA - BEIGE (Editorial Luxury Linen) */
    [data-theme="magenta-beige"] {
      --bg: #f8f4ec;
      --surface: #ffffff;
      --surface-card: rgba(255, 255, 255, 0.96);
      --surface-hover: #f1e9dc;
      --sidebar-bg: #faf6f0;
      --border: #e8ded0;
      --border-accent: rgba(192, 38, 211, 0.4);
      --text: #2c1a28;
      --text-muted: #6e5867;
      --text-dim: #988292;
      --primary: #c026d3;
      --primary-hover: #a21caf;
      --primary-glow: rgba(192, 38, 211, 0.35);
      --accent: #d946ef;
      --accent-hover: #c026d3;
      --accent-glow: rgba(217, 70, 239, 0.3);
      --cyan: #d97706;
      --cyan-glow: rgba(217, 119, 6, 0.25);
      --success: #15803d;
      --success-glow: rgba(21, 128, 61, 0.25);
      --warning: #d97706;
      --danger: #e11d48;
      --shadow-sm: 0 2px 8px rgba(192, 38, 211, 0.05);
      --shadow-md: 0 8px 24px rgba(192, 38, 211, 0.08);
      --shadow-lg: 0 16px 36px rgba(192, 38, 211, 0.12);
      --shadow-glow: 0 0 24px rgba(192, 38, 211, 0.3);
    }
    [data-theme="magenta-beige"] .card,
    [data-theme="magenta-beige"] .content-view .card,
    [data-theme="magenta-beige"] .weekly-chart-card,
    [data-theme="magenta-beige"] .device-gateway-card,
    [data-theme="magenta-beige"] .device-archive-card,
    [data-theme="magenta-beige"] .daily-summary-tile,
    [data-theme="magenta-beige"] .matrix-card,
    [data-theme="magenta-beige"] .kpi-card,
    [data-theme="magenta-beige"] .daily-control-card,
    [data-theme="magenta-beige"] .config-sidebar {
      background: linear-gradient(145deg, #ffffff 0%, #faf5ee 100%) !important;
      border: 1px solid #e8ded0 !important;
      box-shadow: 0 6px 20px rgba(192, 38, 211, 0.05) !important;
    }
    [data-theme="magenta-beige"] .app-topbar {
      background: rgba(248, 244, 236, 0.94) !important;
      border-bottom: 1px solid #e8ded0 !important;
      box-shadow: 0 4px 16px rgba(192, 38, 211, 0.04) !important;
    }
    [data-theme="magenta-beige"] .nav-btn.active,
    [data-theme="magenta-beige"] .config-nav-btn.active {
      background: linear-gradient(135deg, rgba(192, 38, 211, 0.16) 0%, rgba(192, 38, 211, 0.05) 100%) !important;
      border-color: #c026d3 !important;
      color: #c026d3 !important;
    }
    [data-theme="magenta-beige"] table th,
    [data-theme="magenta-beige"] th {
      background: #f4ecdf !important;
      border-bottom: 1px solid #e8ded0 !important;
      color: #6e5867 !important;
    }
    [data-theme="magenta-beige"] table td {
      border-bottom: 1px solid #ede3d5 !important;
      color: #2c1a28 !important;
    }
    [data-theme="magenta-beige"] table tbody tr:hover {
      background: rgba(244, 230, 245, 0.5) !important;
    }
    [data-theme="magenta-beige"] .form-input,
    [data-theme="magenta-beige"] select,
    [data-theme="magenta-beige"] input[type="text"],
    [data-theme="magenta-beige"] input[type="password"],
    [data-theme="magenta-beige"] input[type="date"],
    [data-theme="magenta-beige"] input[type="month"],
    [data-theme="magenta-beige"] input[type="time"],
    [data-theme="magenta-beige"] .search-input {
      background: #ffffff !important;
      border-color: #dcd0bf !important;
      color: #2c1a28 !important;
    }
    [data-theme="magenta-beige"] .badge-primary {
      background: rgba(192, 38, 211, 0.14) !important;
      color: #c026d3 !important;
      border: 1px solid rgba(192, 38, 211, 0.35) !important;
    }

    /* 3. EMERALD - GOLD (Executive Prestige Fintech) */
    [data-theme="emerald-gold"] {
      --bg: #06120e;
      --surface: #0a1f18;
      --surface-card: rgba(10, 31, 24, 0.85);
      --surface-hover: rgba(15, 46, 36, 0.88);
      --sidebar-bg: #071712;
      --border: rgba(16, 185, 129, 0.22);
      --border-accent: rgba(245, 158, 11, 0.45);
      --text: #f2fbf6;
      --text-muted: #96ccb4;
      --text-dim: #5c957d;
      --primary: #10b981;
      --primary-hover: #34d399;
      --primary-glow: rgba(16, 185, 129, 0.4);
      --accent: #f59e0b;
      --accent-hover: #fbbf24;
      --accent-glow: rgba(245, 158, 11, 0.4);
      --cyan: #10b981;
      --cyan-glow: rgba(16, 185, 129, 0.35);
      --success: #10b981;
      --warning: #f59e0b;
      --danger: #ef4444;
      --shadow-sm: 0 2px 8px rgba(0, 0, 0, 0.4);
      --shadow-md: 0 8px 24px rgba(0, 0, 0, 0.5);
      --shadow-lg: 0 16px 36px rgba(0, 0, 0, 0.6);
      --shadow-glow: 0 0 28px rgba(16, 185, 129, 0.38);
    }
    [data-theme="emerald-gold"] .card,
    [data-theme="emerald-gold"] .content-view .card,
    [data-theme="emerald-gold"] .weekly-chart-card,
    [data-theme="emerald-gold"] .device-gateway-card,
    [data-theme="emerald-gold"] .device-archive-card,
    [data-theme="emerald-gold"] .daily-summary-tile,
    [data-theme="emerald-gold"] .matrix-card,
    [data-theme="emerald-gold"] .kpi-card,
    [data-theme="emerald-gold"] .daily-control-card,
    [data-theme="emerald-gold"] .config-sidebar {
      background: linear-gradient(145deg, rgba(16, 185, 129, 0.05) 0%, rgba(10, 31, 24, 0.85) 100%) !important;
      border: 1px solid rgba(16, 185, 129, 0.22) !important;
      box-shadow: 0 8px 26px rgba(0, 0, 0, 0.38) !important;
    }
    [data-theme="emerald-gold"] .app-topbar {
      background: rgba(7, 23, 18, 0.92) !important;
      border-bottom: 1px solid rgba(16, 185, 129, 0.22) !important;
      box-shadow: 0 4px 20px rgba(0, 0, 0, 0.3) !important;
    }
    [data-theme="emerald-gold"] .nav-btn.active,
    [data-theme="emerald-gold"] .config-nav-btn.active {
      background: linear-gradient(135deg, rgba(16, 185, 129, 0.22) 0%, rgba(245, 158, 11, 0.1) 100%) !important;
      border-color: #10b981 !important;
      color: #34d399 !important;
    }
    [data-theme="emerald-gold"] .badge-primary {
      background: rgba(16, 185, 129, 0.18) !important;
      color: #34d399 !important;
      border: 1px solid rgba(16, 185, 129, 0.35) !important;
    }

    /* 4. MIDNIGHT - INDIGO (Cyber Enterprise Matrix) */
    [data-theme="midnight-indigo"] {
      --bg: #070c18;
      --surface: #0e1626;
      --surface-card: rgba(14, 22, 38, 0.88);
      --surface-hover: rgba(20, 32, 54, 0.9);
      --sidebar-bg: #091020;
      --border: rgba(99, 102, 241, 0.22);
      --border-accent: rgba(56, 189, 248, 0.45);
      --text: #f8fafc;
      --text-muted: #94a3b8;
      --text-dim: #64748b;
      --primary: #6366f1;
      --primary-hover: #818cf8;
      --primary-glow: rgba(99, 102, 241, 0.45);
      --accent: #38bdf8;
      --accent-hover: #7dd3fc;
      --accent-glow: rgba(56, 189, 248, 0.4);
      --cyan: #38bdf8;
      --cyan-glow: rgba(56, 189, 248, 0.35);
      --success: #10b981;
      --warning: #f59e0b;
      --danger: #ef4444;
      --shadow-sm: 0 2px 8px rgba(0, 0, 0, 0.4);
      --shadow-md: 0 8px 24px rgba(0, 0, 0, 0.5);
      --shadow-lg: 0 16px 36px rgba(0, 0, 0, 0.6);
      --shadow-glow: 0 0 28px rgba(99, 102, 241, 0.38);
    }
    [data-theme="midnight-indigo"] .card,
    [data-theme="midnight-indigo"] .content-view .card,
    [data-theme="midnight-indigo"] .weekly-chart-card,
    [data-theme="midnight-indigo"] .device-gateway-card,
    [data-theme="midnight-indigo"] .device-archive-card,
    [data-theme="midnight-indigo"] .daily-summary-tile,
    [data-theme="midnight-indigo"] .matrix-card,
    [data-theme="midnight-indigo"] .kpi-card,
    [data-theme="midnight-indigo"] .daily-control-card,
    [data-theme="midnight-indigo"] .config-sidebar {
      background: linear-gradient(145deg, rgba(99, 102, 241, 0.06) 0%, rgba(14, 22, 38, 0.88) 100%) !important;
      border: 1px solid rgba(99, 102, 241, 0.22) !important;
      box-shadow: 0 8px 26px rgba(0, 0, 0, 0.38) !important;
    }
    [data-theme="midnight-indigo"] .app-topbar {
      background: rgba(9, 16, 32, 0.92) !important;
      border-bottom: 1px solid rgba(99, 102, 241, 0.22) !important;
      box-shadow: 0 4px 20px rgba(0, 0, 0, 0.3) !important;
    }
    [data-theme="midnight-indigo"] .nav-btn.active,
    [data-theme="midnight-indigo"] .config-nav-btn.active {
      background: linear-gradient(135deg, rgba(99, 102, 241, 0.22) 0%, rgba(56, 189, 248, 0.1) 100%) !important;
      border-color: #6366f1 !important;
      color: #818cf8 !important;
    }
    [data-theme="midnight-indigo"] .badge-primary {
      background: rgba(99, 102, 241, 0.2) !important;
      color: #818cf8 !important;
      border: 1px solid rgba(99, 102, 241, 0.4) !important;
    }

    /* 5. DARK SLATE - AMBER (High-Contrast Classic) */
    [data-theme="dark-amber"], [data-theme="dark"] {
      --bg: #070a12;
      --surface: #0d121f;
      --surface-card: rgba(15, 23, 42, 0.88);
      --surface-hover: rgba(30, 41, 59, 0.9);
      --sidebar-bg: #0a0f1d;
      --border: rgba(245, 158, 11, 0.22);
      --border-accent: rgba(245, 158, 11, 0.45);
      --text: #f8fafc;
      --text-muted: #94a3b8;
      --text-dim: #64748b;
      --primary: #f59e0b;
      --primary-hover: #fbbf24;
      --primary-glow: rgba(245, 158, 11, 0.45);
      --accent: #10b981;
      --accent-hover: #34d399;
      --accent-glow: rgba(16, 185, 129, 0.4);
      --cyan: #06b6d4;
      --cyan-glow: rgba(6, 182, 212, 0.35);
      --success: #10b981;
      --warning: #f59e0b;
      --danger: #ef4444;
      --shadow-sm: 0 2px 8px rgba(0, 0, 0, 0.4);
      --shadow-md: 0 8px 24px rgba(0, 0, 0, 0.5);
      --shadow-lg: 0 16px 36px rgba(0, 0, 0, 0.6);
      --shadow-glow: 0 0 28px rgba(245, 158, 11, 0.38);
    }
    [data-theme="dark-amber"] .card,
    [data-theme="dark-amber"] .content-view .card,
    [data-theme="dark-amber"] .weekly-chart-card,
    [data-theme="dark-amber"] .device-gateway-card,
    [data-theme="dark-amber"] .device-archive-card,
    [data-theme="dark-amber"] .daily-summary-tile,
    [data-theme="dark-amber"] .matrix-card,
    [data-theme="dark-amber"] .kpi-card,
    [data-theme="dark-amber"] .daily-control-card,
    [data-theme="dark-amber"] .config-sidebar {
      background: linear-gradient(145deg, rgba(245, 158, 11, 0.05) 0%, rgba(15, 23, 42, 0.88) 100%) !important;
      border: 1px solid rgba(245, 158, 11, 0.22) !important;
      box-shadow: 0 8px 26px rgba(0, 0, 0, 0.38) !important;
    }
    [data-theme="dark-amber"] .app-topbar {
      background: rgba(10, 15, 29, 0.92) !important;
      border-bottom: 1px solid rgba(245, 158, 11, 0.22) !important;
      box-shadow: 0 4px 20px rgba(0, 0, 0, 0.3) !important;
    }
    [data-theme="dark-amber"] .nav-btn.active,
    [data-theme="dark-amber"] .config-nav-btn.active {
      background: linear-gradient(135deg, rgba(245, 158, 11, 0.22) 0%, rgba(16, 185, 129, 0.1) 100%) !important;
      border-color: #f59e0b !important;
      color: #fbbf24 !important;
    }
    [data-theme="dark-amber"] .badge-primary {
      background: rgba(245, 158, 11, 0.18) !important;
      color: #fbbf24 !important;
      border: 1px solid rgba(245, 158, 11, 0.35) !important;
    }

    /* THEME CARDS & CUSTOMIZER STYLES */
    .themes-preset-grid {
      display: grid;
      grid-template-columns: repeat(auto-fit, minmax(220px, 1fr));
      gap: 16px;
      margin-top: 14px;
      margin-bottom: 24px;
    }
    .theme-preset-card {
      position: relative;
      background: var(--surface);
      border: 1.5px solid var(--border);
      border-radius: 14px;
      padding: 16px;
      cursor: pointer;
      transition: all 0.22s cubic-bezier(0.4, 0, 0.2, 1);
      display: flex;
      flex-direction: column;
      gap: 10px;
      user-select: none;
    }
    .theme-preset-card:hover {
      transform: translateY(-3px);
      border-color: var(--primary);
      box-shadow: 0 10px 28px rgba(0, 0, 0, 0.3);
    }
    .theme-preset-card.active {
      border-color: var(--primary) !important;
      box-shadow: 0 0 0 2px var(--primary-glow), 0 8px 24px rgba(0, 0, 0, 0.35) !important;
      background: linear-gradient(145deg, rgba(255, 255, 255, 0.06) 0%, var(--surface) 100%) !important;
    }
    .theme-preset-header {
      display: flex;
      justify-content: space-between;
      align-items: center;
    }
    .theme-preset-swatches {
      display: flex;
      gap: 6px;
    }
    .theme-preset-swatch {
      width: 22px;
      height: 22px;
      border-radius: 50%;
      border: 1.5px solid rgba(255, 255, 255, 0.25);
      box-shadow: 0 2px 6px rgba(0, 0, 0, 0.3);
      display: inline-block;
    }
    .theme-preset-check {
      font-size: 11px;
      font-weight: 700;
      color: #fff;
      background: var(--primary);
      padding: 2px 8px;
      border-radius: 20px;
      display: none;
    }
    .theme-preset-card.active .theme-preset-check {
      display: inline-block;
    }
    .theme-preset-title {
      font-size: 14.5px;
      font-weight: 700;
      color: var(--text);
      display: flex;
      align-items: center;
      gap: 7px;
      margin: 0;
    }
    .theme-preset-desc {
      font-size: 12px;
      color: var(--text-muted);
      line-height: 1.45;
      margin: 0;
      min-height: 36px;
    }
    .theme-preset-badge {
      align-self: flex-start;
      font-size: 10.5px;
      font-weight: 700;
      padding: 3px 9px;
      border-radius: 6px;
      letter-spacing: 0.3px;
    }

    /* CUSTOM PALETTE PICKER GRID */
    .custom-palette-grid {
      display: grid;
      grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
      gap: 16px;
      margin-top: 14px;
    }
    .custom-color-item {
      background: rgba(255, 255, 255, 0.02);
      border: 1px solid var(--border);
      border-radius: 12px;
      padding: 14px;
      display: flex;
      flex-direction: column;
      gap: 10px;
    }
    .custom-color-label {
      font-size: 12px;
      font-weight: 600;
      color: var(--text);
      display: flex;
      align-items: center;
      justify-content: space-between;
    }
    .custom-color-control {
      display: flex;
      align-items: center;
      gap: 10px;
    }
    .custom-color-control input[type="color"] {
      -webkit-appearance: none;
      border: none;
      width: 42px;
      height: 38px;
      border-radius: 8px;
      cursor: pointer;
      background: none;
      padding: 0;
    }
    .custom-color-control input[type="color"]::-webkit-color-swatch-wrapper {
      padding: 0;
    }
    .custom-color-control input[type="color"]::-webkit-color-swatch {
      border: 1.5px solid var(--border);
      border-radius: 8px;
    }
    .custom-color-control input[type="text"] {
      flex: 1;
      height: 38px;
      padding: 0 10px;
      background: rgba(0, 0, 0, 0.2);
      border: 1px solid var(--border);
      border-radius: 8px;
      color: var(--text);
      font-family: var(--font-mono);
      font-size: 12px;
    }
    .palette-live-preview-box {
      margin-top: 20px;
      background: var(--surface);
      border: 1px solid var(--border);
      border-radius: 14px;
      padding: 20px;
      box-shadow: var(--shadow-md);
    }
`;

// 2. NAV BUTTON HTML TO INJECT IN <nav class="config-nav">
const navBtnHtml = `
                <button class="config-nav-btn" data-cfg-pane="themes" onclick="switchConfigTab('themes', this)">
                  <span class="cfg-icon">🎨</span>
                  <span class="cfg-text">Themes</span>
                  <span class="cfg-badge" id="cfg-theme-count" style="background:linear-gradient(135deg, var(--primary), var(--accent)); color:#fff; font-weight:700;">5 Presets</span>
                </button>`;

// 3. COMPLETE CONFIG PANE FOR THEMES & COLOR PALETTE STUDIO
const themesPaneHtml = `
              <!-- ===============================================================
                   SECTION: THEMES & COLOR PALETTE CUSTOMIZER
                   =============================================================== -->
              <div class="config-pane" id="cfg-pane-themes">
                
                <!-- TOP INTRO CARD -->
                <div class="card" style="margin-bottom:20px; background:linear-gradient(135deg, rgba(255,107,0,0.06) 0%, rgba(6,182,212,0.06) 100%); border:1px solid var(--border);">
                  <div style="padding:20px 24px; display:flex; justify-content:space-between; align-items:center; flex-wrap:wrap; gap:14px;">
                    <div style="display:flex; align-items:center; gap:14px;">
                      <span style="font-size:32px;">🎨</span>
                      <div>
                        <div style="font-size:16px; font-weight:800; color:var(--text); letter-spacing:-0.02em;">Master Themes & Color Palette Studio</div>
                        <div style="font-size:12.5px; color:var(--text-muted); margin-top:2px;">Select from 5 curated enterprise dashboard combos or customize every color of the whole website.</div>
                      </div>
                    </div>
                    <div style="display:flex; align-items:center; gap:10px;">
                      <span class="badge badge-primary" id="active-theme-display-badge" style="font-weight:700; font-size:12px; padding:6px 14px; letter-spacing:0.4px;">ACTIVE: Orange - Cyan</span>
                      <button type="button" class="btn btn-outline btn-sm" onclick="toggleTheme()" title="Quick Cycle to Next Preset" style="border-radius:20px; padding:6px 14px; font-size:12px; display:inline-flex; align-items:center; gap:6px;">
                        <span>🔄</span> Cycle Preset
                      </button>
                    </div>
                  </div>
                </div>

                <!-- 5 PRESET CARDS -->
                <div class="card" style="margin-bottom:20px;">
                  <div class="card-header">
                    <div class="card-title-group">
                      <h3 class="card-title">✨ Curated Enterprise Dashboard Palettes (5 Presets)</h3>
                      <p class="card-subtitle">One-click instant application across the entire ERP, Daily Attendance, Muster Rolls & Topbar</p>
                    </div>
                  </div>
                  <div style="padding:22px;">
                    <div class="themes-preset-grid" id="themes-preset-grid">

                      <!-- 1. Orange - Cyan -->
                      <div class="theme-preset-card" data-scheme="orange-cyan" onclick="selectSchemeCard('orange-cyan')">
                        <div class="theme-preset-header">
                          <div class="theme-preset-swatches">
                            <span class="theme-preset-swatch" style="background:#ff6b00;" title="Solar Orange Primary"></span>
                            <span class="theme-preset-swatch" style="background:#06b6d4;" title="Electric Cyan Accent"></span>
                            <span class="theme-preset-swatch" style="background:#060e1a; border-color:rgba(6,182,212,0.5);" title="Deep Space Canvas"></span>
                          </div>
                          <span class="theme-preset-check">✓ Active</span>
                        </div>
                        <h4 class="theme-preset-title">⚡ Orange - Cyan</h4>
                        <p class="theme-preset-desc">Cosmic sci-fi deep navy energized by vibrant solar amber orange and electric cyan.</p>
                        <span class="theme-preset-badge" style="background:rgba(255,107,0,0.18); color:#ff8533; border:1px solid rgba(255,107,0,0.35);">Requested Combo</span>
                      </div>

                      <!-- 2. Magenta - Beige -->
                      <div class="theme-preset-card" data-scheme="magenta-beige" onclick="selectSchemeCard('magenta-beige')">
                        <div class="theme-preset-header">
                          <div class="theme-preset-swatches">
                            <span class="theme-preset-swatch" style="background:#c026d3;" title="Royal Magenta"></span>
                            <span class="theme-preset-swatch" style="background:#f8f4ec; border-color:#dcd0bf;" title="Warm Oatmeal Beige"></span>
                            <span class="theme-preset-swatch" style="background:#e879f9;" title="Orchid Glow"></span>
                          </div>
                          <span class="theme-preset-check">✓ Active</span>
                        </div>
                        <h4 class="theme-preset-title">🌸 Magenta - Beige</h4>
                        <p class="theme-preset-desc">Warm editorial luxury oatmeal linen backdrop paired with vivid royal magenta highlights.</p>
                        <span class="theme-preset-badge" style="background:rgba(192,38,211,0.18); color:#c026d3; border:1px solid rgba(192,38,211,0.35);">Requested Combo</span>
                      </div>

                      <!-- 3. Emerald - Gold -->
                      <div class="theme-preset-card" data-scheme="emerald-gold" onclick="selectSchemeCard('emerald-gold')">
                        <div class="theme-preset-header">
                          <div class="theme-preset-swatches">
                            <span class="theme-preset-swatch" style="background:#10b981;" title="Royal Mint Emerald"></span>
                            <span class="theme-preset-swatch" style="background:#f59e0b;" title="Champagne Gold"></span>
                            <span class="theme-preset-swatch" style="background:#06120e; border-color:rgba(16,185,129,0.4);" title="Obsidian Forest"></span>
                          </div>
                          <span class="theme-preset-check">✓ Active</span>
                        </div>
                        <h4 class="theme-preset-title">👑 Emerald - Gold</h4>
                        <p class="theme-preset-desc">Executive fintech boardroom obsidian illuminated by vibrant emerald and champagne gold.</p>
                        <span class="theme-preset-badge" style="background:rgba(16,185,129,0.18); color:#34d399; border:1px solid rgba(16,185,129,0.35);">Executive Prestige</span>
                      </div>

                      <!-- 4. Midnight - Indigo -->
                      <div class="theme-preset-card" data-scheme="midnight-indigo" onclick="selectSchemeCard('midnight-indigo')">
                        <div class="theme-preset-header">
                          <div class="theme-preset-swatches">
                            <span class="theme-preset-swatch" style="background:#6366f1;" title="Electric Indigo"></span>
                            <span class="theme-preset-swatch" style="background:#38bdf8;" title="Sky Cyan Accent"></span>
                            <span class="theme-preset-swatch" style="background:#070c18; border-color:rgba(99,102,241,0.4);" title="Midnight Starlight"></span>
                          </div>
                          <span class="theme-preset-check">✓ Active</span>
                        </div>
                        <h4 class="theme-preset-title">🔮 Midnight - Indigo</h4>
                        <p class="theme-preset-desc">Modern sleek SaaS enterprise matrix with electric royal indigo and bright ice sky-blue.</p>
                        <span class="theme-preset-badge" style="background:rgba(99,102,241,0.18); color:#818cf8; border:1px solid rgba(99,102,241,0.35);">Cyber Enterprise</span>
                      </div>

                      <!-- 5. Dark Slate - Amber -->
                      <div class="theme-preset-card" data-scheme="dark-amber" onclick="selectSchemeCard('dark-amber')">
                        <div class="theme-preset-header">
                          <div class="theme-preset-swatches">
                            <span class="theme-preset-swatch" style="background:#f59e0b;" title="Solar Amber"></span>
                            <span class="theme-preset-swatch" style="background:#10b981;" title="Mint Green Accent"></span>
                            <span class="theme-preset-swatch" style="background:#070a12; border-color:rgba(255,255,255,0.2);" title="Slate Obsidian"></span>
                          </div>
                          <span class="theme-preset-check">✓ Active</span>
                        </div>
                        <h4 class="theme-preset-title">🌌 Dark Slate - Amber</h4>
                        <p class="theme-preset-desc">Industrial high-contrast dark mode with high-visibility warm amber badges and slate cards.</p>
                        <span class="theme-preset-badge" style="background:rgba(245,158,11,0.18); color:#fbbf24; border:1px solid rgba(245,158,11,0.35);">High-Contrast Classic</span>
                      </div>

                    </div>
                  </div>
                </div>

                <!-- PERSONALIZED CUSTOM COLOR PALETTE BUILDER -->
                <div class="card" style="margin-bottom:20px;">
                  <div class="card-header" style="display:flex; justify-content:space-between; align-items:center; flex-wrap:wrap; gap:10px;">
                    <div class="card-title-group">
                      <h3 class="card-title">🛠️ Custom Color Palette Builder</h3>
                      <p class="card-subtitle">Fine-tune individual colors to match your personal taste or brand identity</p>
                    </div>
                    <div style="display:flex; gap:8px;">
                      <button type="button" class="btn btn-outline btn-sm" onclick="resetCustomPaletteToPreset()">
                        🔄 Reset to Preset
                      </button>
                      <button type="button" class="btn btn-primary btn-sm" onclick="saveCustomPalette()">
                        💾 Save Custom Palette
                      </button>
                    </div>
                  </div>

                  <div style="padding:22px;">
                    <div style="font-size:12px; color:var(--text-muted); margin-bottom:14px;">
                      Adjust the color pickers or enter hex codes below. The whole website will update in real-time as you tweak:
                    </div>

                    <!-- 6 Custom Color Inputs -->
                    <div class="custom-palette-grid">
                      <!-- 1. Primary Accent -->
                      <div class="custom-color-item">
                        <div class="custom-color-label">
                          <span>Primary Brand Color</span>
                          <span style="font-size:10px; color:var(--text-dim); font-family:var(--font-mono);">--primary</span>
                        </div>
                        <div class="custom-color-control">
                          <input type="color" id="pick-primary" value="#ff6b00" oninput="onCustomColorChange('primary', this.value)">
                          <input type="text" id="hex-primary" value="#ff6b00" oninput="onCustomHexChange('primary', this.value)">
                        </div>
                        <div style="font-size:11px; color:var(--text-dim);">Main buttons, active tabs, punch alerts</div>
                      </div>

                      <!-- 2. Secondary Highlight -->
                      <div class="custom-color-item">
                        <div class="custom-color-label">
                          <span>Secondary Accent</span>
                          <span style="font-size:10px; color:var(--text-dim); font-family:var(--font-mono);">--accent</span>
                        </div>
                        <div class="custom-color-control">
                          <input type="color" id="pick-accent" value="#06b6d4" oninput="onCustomColorChange('accent', this.value)">
                          <input type="text" id="hex-accent" value="#06b6d4" oninput="onCustomHexChange('accent', this.value)">
                        </div>
                        <div style="font-size:11px; color:var(--text-dim);">Hover glows, charts, secondary metrics</div>
                      </div>

                      <!-- 3. Canvas Background -->
                      <div class="custom-color-item">
                        <div class="custom-color-label">
                          <span>Background Canvas</span>
                          <span style="font-size:10px; color:var(--text-dim); font-family:var(--font-mono);">--bg</span>
                        </div>
                        <div class="custom-color-control">
                          <input type="color" id="pick-bg" value="#060e1a" oninput="onCustomColorChange('bg', this.value)">
                          <input type="text" id="hex-bg" value="#060e1a" oninput="onCustomHexChange('bg', this.value)">
                        </div>
                        <div style="font-size:11px; color:var(--text-dim);">Base screen backdrop across all views</div>
                      </div>

                      <!-- 4. Surface Card Background -->
                      <div class="custom-color-item">
                        <div class="custom-color-label">
                          <span>Card & Surface Box</span>
                          <span style="font-size:10px; color:var(--text-dim); font-family:var(--font-mono);">--surface</span>
                        </div>
                        <div class="custom-color-control">
                          <input type="color" id="pick-surface" value="#0b172a" oninput="onCustomColorChange('surface', this.value)">
                          <input type="text" id="hex-surface" value="#0b172a" oninput="onCustomHexChange('surface', this.value)">
                        </div>
                        <div style="font-size:11px; color:var(--text-dim);">Glassmorphism cards, modals & tables</div>
                      </div>

                      <!-- 5. Sidebar Background -->
                      <div class="custom-color-item">
                        <div class="custom-color-label">
                          <span>Sidebar & Topbar</span>
                          <span style="font-size:10px; color:var(--text-dim); font-family:var(--font-mono);">--sidebar-bg</span>
                        </div>
                        <div class="custom-color-control">
                          <input type="color" id="pick-sidebar" value="#081221" oninput="onCustomColorChange('sidebar-bg', this.value)">
                          <input type="text" id="hex-sidebar" value="#081221" oninput="onCustomHexChange('sidebar-bg', this.value)">
                        </div>
                        <div style="font-size:11px; color:var(--text-dim);">Left menu rail & sticky header</div>
                      </div>

                      <!-- 6. Typography Color -->
                      <div class="custom-color-item">
                        <div class="custom-color-label">
                          <span>Main Text Color</span>
                          <span style="font-size:10px; color:var(--text-dim); font-family:var(--font-mono);">--text</span>
                        </div>
                        <div class="custom-color-control">
                          <input type="color" id="pick-text" value="#f0f9ff" oninput="onCustomColorChange('text', this.value)">
                          <input type="text" id="hex-text" value="#f0f9ff" oninput="onCustomHexChange('text', this.value)">
                        </div>
                        <div style="font-size:11px; color:var(--text-dim);">Headers, body copy & data numbers</div>
                      </div>
                    </div>

                    <!-- LIVE PREVIEW SANDBOX -->
                    <div class="palette-live-preview-box">
                      <div style="display:flex; justify-content:space-between; align-items:center; margin-bottom:14px; flex-wrap:wrap; gap:8px;">
                        <span style="font-size:13px; font-weight:700; color:var(--text); letter-spacing:-0.01em;">Live Component Sandbox (Dynamic Preview)</span>
                        <span class="badge badge-primary">ACTIVE PREVIEW</span>
                      </div>
                      
                      <div style="display:grid; grid-template-columns:repeat(auto-fit, minmax(240px, 1fr)); gap:16px;">
                        <!-- Sample KPI Widget -->
                        <div class="kpi-card" style="padding:14px;">
                          <div class="kpi-header">
                            <span class="kpi-label">Today Present Count</span>
                            <span class="kpi-icon-badge">⚡</span>
                          </div>
                          <div class="kpi-value" style="color:var(--primary);">28 / 30</div>
                          <div class="kpi-progress-track" style="margin-top:8px;">
                            <div class="kpi-progress-fill" style="width:93.3%; background:linear-gradient(90deg, var(--primary), var(--accent));"></div>
                          </div>
                        </div>

                        <!-- Sample Action Controls -->
                        <div style="display:flex; flex-direction:column; gap:10px; justify-content:center;">
                          <div style="display:flex; gap:8px;">
                            <button type="button" class="btn btn-primary btn-sm" style="flex:1;">Primary Button</button>
                            <button type="button" class="btn btn-outline btn-sm" style="flex:1;">Outline Button</button>
                          </div>
                          <div style="display:flex; gap:6px; flex-wrap:wrap;">
                            <span class="badge badge-primary">Status: Synced</span>
                            <span class="badge badge-accent">ESP32: Online</span>
                            <span class="badge badge-success">Fingerprint: Matched</span>
                          </div>
                        </div>
                      </div>
                    </div>

                  </div>
                </div>

                <div class="config-action-bar">
                  <button type="button" class="btn btn-primary" onclick="saveCustomPalette()">💾 Save & Apply Current Theme</button>
                </div>
              </div>`;

// 4. JAVASCRIPT LOGIC TO INJECT
const themesJsToInject = `
    // =========================================================================
    //  CHRONOS MIS: 5 THEME PRESETS & CUSTOM PALETTE CONTROLLER ENGINE
    // =========================================================================
    var SCHEMES_REGISTRY = [
      {
        id: 'orange-cyan',
        name: 'Orange - Cyan',
        tag: 'Cyber Dual-Neon',
        icon: '⚡',
        primary: '#ff6b00',
        accent: '#06b6d4',
        bg: '#060e1a',
        surface: '#0b172a',
        sidebar: '#081221',
        text: '#f0f9ff'
      },
      {
        id: 'magenta-beige',
        name: 'Magenta - Beige',
        tag: 'Editorial Luxury Linen',
        icon: '🌸',
        primary: '#c026d3',
        accent: '#e879f9',
        bg: '#f8f4ec',
        surface: '#ffffff',
        sidebar: '#faf6f0',
        text: '#2c1a28'
      },
      {
        id: 'emerald-gold',
        name: 'Emerald - Gold',
        tag: 'Executive Prestige',
        icon: '👑',
        primary: '#10b981',
        accent: '#f59e0b',
        bg: '#06120e',
        surface: '#0a1f18',
        sidebar: '#071712',
        text: '#f2fbf6'
      },
      {
        id: 'midnight-indigo',
        name: 'Midnight - Indigo',
        tag: 'Cyber Enterprise',
        icon: '🔮',
        primary: '#6366f1',
        accent: '#38bdf8',
        bg: '#070c18',
        surface: '#0e1626',
        sidebar: '#091020',
        text: '#f8fafc'
      },
      {
        id: 'dark-amber',
        name: 'Dark Slate - Amber',
        tag: 'High-Contrast Classic',
        icon: '🌌',
        primary: '#f59e0b',
        accent: '#10b981',
        bg: '#070a12',
        surface: '#0d121f',
        sidebar: '#0a0f1d',
        text: '#f8fafc'
      }
    ];

    var currentCustomPalette = null;

    function setGlobalScheme(schemeId, showToastAlert) {
      if (!schemeId) schemeId = 'orange-cyan';
      if (schemeId === 'cyan-orange') schemeId = 'orange-cyan';
      if (schemeId === 'dark') schemeId = 'dark-amber';

      var sObj = SCHEMES_REGISTRY.find(function(s) { return s.id === schemeId; });
      if (!sObj) {
        sObj = SCHEMES_REGISTRY[0];
        schemeId = sObj.id;
      }

      // 1. Reset any inline custom CSS overrides so preset rules shine through
      clearCustomCssOverrides();

      // 2. Set data-theme on body
      document.body.setAttribute('data-theme', schemeId);
      localStorage.setItem('erp_theme', schemeId);
      localStorage.removeItem('chronos_custom_palette_active');

      // 3. Synchronize appState
      if (window.appState) {
        if (!window.appState.settings) window.appState.settings = {};
        window.appState.settings.theme = schemeId;
        if (window.appState.config && window.appState.config.system) {
          window.appState.config.system.theme = schemeId;
        }
      }

      // 4. Update preset card active states
      document.querySelectorAll('.theme-preset-card, .scheme-card').forEach(function(card) {
        var cId = card.getAttribute('data-scheme');
        if (cId === schemeId || (schemeId === 'orange-cyan' && cId === 'cyan-orange')) {
          card.classList.add('active');
        } else {
          card.classList.remove('active');
        }
      });

      // 5. Sync customizer color pickers with preset colors
      syncColorPickers(sObj.primary, sObj.accent, sObj.bg, sObj.surface, sObj.sidebar, sObj.text);

      // 6. Update Badges
      var badge = document.getElementById('active-theme-display-badge');
      if (badge) badge.innerText = 'ACTIVE: ' + sObj.name;
      var badge2 = document.getElementById('active-scheme-badge');
      if (badge2) badge2.innerText = 'ACTIVE: ' + sObj.name;

      // 7. Update Topbar Button
      var icon = document.getElementById('theme-btn-icon');
      var label = document.getElementById('theme-btn-label');
      if (icon) icon.innerText = sObj.icon;
      if (label) label.innerText = sObj.name;

      // 8. Toast feedback
      if (showToastAlert && typeof showToast === 'function') {
        showToast("Theme: " + sObj.name + " (" + sObj.tag + ")", "info");
      }
    }

    function selectSchemeCard(schemeId) {
      setGlobalScheme(schemeId, true);
    }

    function toggleTheme() {
      var cur = document.body.getAttribute('data-theme') || 'orange-cyan';
      if (cur === 'cyan-orange') cur = 'orange-cyan';
      if (cur === 'dark') cur = 'dark-amber';
      var idx = SCHEMES_REGISTRY.findIndex(function(s) { return s.id === cur; });
      if (idx === -1) idx = 0;
      var nextIdx = (idx + 1) % SCHEMES_REGISTRY.length;
      setGlobalScheme(SCHEMES_REGISTRY[nextIdx].id, true);
    }

    function previewTheme(mode) {
      setGlobalScheme(mode, true);
    }

    function openSchemeControllerInConfig() {
      openConfigSection('themes');
    }

    // --- CUSTOM PALETTE LOGIC ---
    function syncColorPickers(primary, accent, bg, surface, sidebar, text) {
      if (document.getElementById('pick-primary')) document.getElementById('pick-primary').value = primary;
      if (document.getElementById('hex-primary')) document.getElementById('hex-primary').value = primary;

      if (document.getElementById('pick-accent')) document.getElementById('pick-accent').value = accent;
      if (document.getElementById('hex-accent')) document.getElementById('hex-accent').value = accent;

      if (document.getElementById('pick-bg')) document.getElementById('pick-bg').value = bg;
      if (document.getElementById('hex-bg')) document.getElementById('hex-bg').value = bg;

      if (document.getElementById('pick-surface')) document.getElementById('pick-surface').value = surface;
      if (document.getElementById('hex-surface')) document.getElementById('hex-surface').value = surface;

      if (document.getElementById('pick-sidebar')) document.getElementById('pick-sidebar').value = sidebar;
      if (document.getElementById('hex-sidebar')) document.getElementById('hex-sidebar').value = sidebar;

      if (document.getElementById('pick-text')) document.getElementById('pick-text').value = text;
      if (document.getElementById('hex-text')) document.getElementById('hex-text').value = text;
    }

    function onCustomColorChange(propKey, hexValue) {
      var idMap = {
        'primary': 'hex-primary',
        'accent': 'hex-accent',
        'bg': 'hex-bg',
        'surface': 'hex-surface',
        'sidebar-bg': 'hex-sidebar',
        'text': 'hex-text'
      };
      if (idMap[propKey]) {
        var hEl = document.getElementById(idMap[propKey]);
        if (hEl) hEl.value = hexValue;
      }
      applyCustomCssProperty(propKey, hexValue);
    }

    function onCustomHexChange(propKey, hexValue) {
      if (!/^#[0-9A-Fa-f]{6}$/.test(hexValue)) return;
      var idMap = {
        'primary': 'pick-primary',
        'accent': 'pick-accent',
        'bg': 'pick-bg',
        'surface': 'pick-surface',
        'sidebar-bg': 'pick-sidebar',
        'text': 'pick-text'
      };
      if (idMap[propKey]) {
        var pEl = document.getElementById(idMap[propKey]);
        if (pEl) pEl.value = hexValue;
      }
      applyCustomCssProperty(propKey, hexValue);
    }

    function applyCustomCssProperty(propKey, hexValue) {
      document.documentElement.style.setProperty('--' + propKey, hexValue);

      if (propKey === 'primary') {
        document.documentElement.style.setProperty('--primary-glow', hexValue + '66');
        document.documentElement.style.setProperty('--border-accent', hexValue + '55');
      }
      if (propKey === 'accent') {
        document.documentElement.style.setProperty('--accent-glow', hexValue + '66');
      }
      if (propKey === 'bg') {
        document.body.style.backgroundColor = hexValue;
      }

      // Mark badge as Custom
      var badge = document.getElementById('active-theme-display-badge');
      if (badge) badge.innerText = 'ACTIVE: Personalized Custom Palette';
      document.querySelectorAll('.theme-preset-card').forEach(function(c){ c.classList.remove('active'); });
    }

    function clearCustomCssOverrides() {
      var props = ['primary', 'primary-glow', 'border-accent', 'accent', 'accent-glow', 'bg', 'surface', 'sidebar-bg', 'text'];
      props.forEach(function(p) {
        document.documentElement.style.removeProperty('--' + p);
      });
      document.body.style.backgroundColor = '';
    }

    function saveCustomPalette() {
      var palette = {
        primary: document.getElementById('pick-primary').value,
        accent: document.getElementById('pick-accent').value,
        bg: document.getElementById('pick-bg').value,
        surface: document.getElementById('pick-surface').value,
        sidebar: document.getElementById('pick-sidebar').value,
        text: document.getElementById('pick-text').value
      };

      localStorage.setItem('chronos_custom_palette', JSON.stringify(palette));
      localStorage.setItem('chronos_custom_palette_active', 'true');
      localStorage.setItem('erp_theme', 'custom');

      // Apply to all
      Object.keys(palette).forEach(function(k) {
        var key = (k === 'sidebar') ? 'sidebar-bg' : k;
        applyCustomCssProperty(key, palette[k]);
      });

      if (typeof showToast === 'function') {
        showToast("Personalized color palette saved & applied successfully across the whole website!", "success");
      }
    }

    function resetCustomPaletteToPreset() {
      var curTheme = localStorage.getItem('erp_theme') || 'orange-cyan';
      if (curTheme === 'custom') curTheme = 'orange-cyan';
      setGlobalScheme(curTheme, true);
    }

    function loadInitialTheme() {
      var isCustom = localStorage.getItem('chronos_custom_palette_active') === 'true';
      if (isCustom) {
        var saved = localStorage.getItem('chronos_custom_palette');
        if (saved) {
          try {
            var pal = JSON.parse(saved);
            syncColorPickers(pal.primary, pal.accent, pal.bg, pal.surface, pal.sidebar, pal.text);
            Object.keys(pal).forEach(function(k) {
              var key = (k === 'sidebar') ? 'sidebar-bg' : k;
              applyCustomCssProperty(key, pal[k]);
            });
            var badge = document.getElementById('active-theme-display-badge');
            if (badge) badge.innerText = 'ACTIVE: Personalized Custom Palette';
            return;
          } catch(e) {}
        }
      }

      var savedTheme = localStorage.getItem('erp_theme') || 'orange-cyan';
      setGlobalScheme(savedTheme, false);
    }

    // --- CONFIG TAB SWITCHER ENGINE ---
    function switchConfigTab(paneKey, btnEl) {
      if (!paneKey) paneKey = 'general';
      document.querySelectorAll('.config-nav-btn').forEach(function(b){ b.classList.remove('active'); });
      document.querySelectorAll('.config-pane').forEach(function(p){ p.classList.remove('active'); });

      if (btnEl) {
        btnEl.classList.add('active');
      } else {
        var found = document.querySelector('.config-nav-btn[data-cfg-pane="' + paneKey + '"]');
        if (found) found.classList.add('active');
      }

      var target = document.getElementById('cfg-pane-' + paneKey);
      if (target) {
        target.classList.add('active');
      }

      if (paneKey === 'themes') {
        // Sync customizer inputs with current state
        var curTheme = document.body.getAttribute('data-theme') || 'orange-cyan';
        var sObj = SCHEMES_REGISTRY.find(function(s){ return s.id === curTheme; }) || SCHEMES_REGISTRY[0];
        if (localStorage.getItem('chronos_custom_palette_active') !== 'true') {
          syncColorPickers(sObj.primary, sObj.accent, sObj.bg, sObj.surface, sObj.sidebar, sObj.text);
        }
      }
      if (paneKey === 'branches' && typeof renderConfigBranches === 'function') renderConfigBranches();
      if (paneKey === 'shifts' && typeof renderConfigShifts === 'function') renderConfigShifts();
      if (paneKey === 'leaves' && typeof renderConfigLeaves === 'function') renderConfigLeaves();
      if (paneKey === 'departments' && typeof renderConfigDepartments === 'function') renderConfigDepartments();
      if (paneKey === 'designations' && typeof renderConfigDesignations === 'function') renderConfigDesignations();
      if (paneKey === 'holidays' && typeof renderConfigHolidays === 'function') renderConfigHolidays();
      if (paneKey === 'roles' && typeof renderConfigRolesMatrix === 'function') renderConfigRolesMatrix();
    }
`;

// EXECUTE UPDATES ON TARGET FILES
targetFiles.forEach(file => {
  const filePath = path.join(__dirname, file);
  if (!fs.existsSync(filePath)) {
    console.log(`⚠️ File not found: ${file}`);
    return;
  }

  let content = fs.readFileSync(filePath, 'utf8');

  // 1. Inject Themes CSS before </style>
  if (!content.includes('CHRONOS MIS: 5 CURATED DASHBOARD THEMES')) {
    content = content.replace('</style>', themesCssToInject + '\n  </style>');
    console.log(`✓ [${file}] Injected 5 Curated Dashboard Themes CSS`);
  }

  // 2. Add Themes Nav Button in <nav class="config-nav">
  if (!content.includes('data-cfg-pane="themes"')) {
    const rolesBtnPattern = /<button class="config-nav-btn" data-cfg-pane="roles"[\s\S]*?<\/button>/;
    if (rolesBtnPattern.test(content)) {
      content = content.replace(rolesBtnPattern, (match) => match + '\n' + navBtnHtml);
      console.log(`✓ [${file}] Injected Themes tab into Configuration sidebar`);
    }
  }

  // 3. Add Themes Config Pane before </main> inside #view-configuration
  if (!content.includes('id="cfg-pane-themes"')) {
    const configMainCloseIdx = content.indexOf('</main>', content.indexOf('id="view-configuration"'));
    if (configMainCloseIdx !== -1) {
      content = content.substring(0, configMainCloseIdx) + themesPaneHtml + '\n            ' + content.substring(configMainCloseIdx);
      console.log(`✓ [${file}] Injected Themes & Color Palette Studio Pane`);
    }
  }

  // 4. Inject Themes & Switcher JS before </script>
  if (!content.includes('CHRONOS MIS: 5 THEME PRESETS & CUSTOM PALETTE CONTROLLER ENGINE')) {
    content = content.replace('</script>', themesJsToInject + '\n  </script>');
    console.log(`✓ [${file}] Injected Themes & Custom Palette JS Engine`);
  }

  // 5. Ensure loadInitialTheme() is called upon init
  if (!content.includes('loadInitialTheme();')) {
    if (content.includes('initEnterpriseConfig();')) {
      content = content.replace('initEnterpriseConfig();', 'initEnterpriseConfig();\n      loadInitialTheme();');
    } else if (content.includes('// Initial Dashboard & Attendance Renders')) {
      content = content.replace('// Initial Dashboard & Attendance Renders', 'loadInitialTheme();\n      // Initial Dashboard & Attendance Renders');
    }
    console.log(`✓ [${file}] Hooked loadInitialTheme() into startup lifecycle`);
  }

  fs.writeFileSync(filePath, content, 'utf8');
  console.log(`✓ [${file}] Successfully updated and saved (${content.length} bytes)`);
});

console.log('\n================================================================');
console.log('  THEMES & CUSTOM PALETTE STUDIO SUCCESSFULLY INSTALLED!       ');
console.log('================================================================');
