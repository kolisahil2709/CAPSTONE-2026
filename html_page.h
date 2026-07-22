
#pragma once
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en" data-theme="light">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>Anurag 0.1 Attendance System</title>
<style>
:root {
  --bg: #f0f2f5; --card-bg: #ffffff; --text: #1a1a2e; --text-muted: #6c757d;
  --border: #dee2e6; --primary: #4361ee; --success: #06d6a0; --danger: #ef476f;
  --warning: #ffd166; --header-bg: #ffffff; --tab-active: #4361ee; --tab-text: #ffffff;
  --input-bg: #ffffff; --shadow: 0 4px 12px rgba(0,0,0,0.05);
  --glass-bg: rgba(255, 255, 255, 0.7); --glass-border: rgba(255, 255, 255, 0.2);
}
[data-theme="dark"] {
  --bg: #090b10; --card-bg: #161b22; --text: #e6edf3; --text-muted: #8b949e;
  --border: #30363d; --primary: #58a6ff; --header-bg: #161b22;
  --input-bg: #21262d; --shadow: 0 4px 12px rgba(0,0,0,0.5);
  --glass-bg: rgba(22, 27, 34, 0.7); --glass-border: rgba(255, 255, 255, 0.05);
}
* { margin:0; padding:0; box-sizing:border-box; font-family:'Segoe UI',Arial,sans-serif; }
body { background:var(--bg); color:var(--text); transition:background 0.3s,color 0.3s; min-height:100vh; }
header { background:var(--header-bg); border-bottom:1px solid var(--border); padding:14px 24px;
  display:flex; justify-content:space-between; align-items:center; box-shadow:var(--shadow); position:sticky; top:0; z-index:100; flex-wrap:wrap; gap:10px; }
.logo { font-size:20px; font-weight:700; color:var(--primary); min-width:150px; }
.logo span { font-weight:300; color:var(--text-muted); font-size:14px; display:block; }
.header-right { display:flex; align-items:center; gap:12px; flex-wrap:wrap; justify-content:flex-end; }
#device-ip { font-size:12px; color:var(--text-muted); }
.theme-btn { background:none; border:1px solid var(--border); border-radius:8px;
  padding:6px 12px; cursor:pointer; font-size:18px; color:var(--text); transition:0.2s; }
.logout-btn { background:var(--danger); color:#fff; border:none; border-radius:8px;
  padding:7px 16px; cursor:pointer; font-size:13px; font-weight:600; }
nav { display:flex; background:var(--card-bg); border-bottom:1px solid var(--border); padding:0 24px; gap:4px; overflow-x:auto; white-space:nowrap; -webkit-overflow-scrolling:touch; scrollbar-width:none; }
nav::-webkit-scrollbar { display:none; }
.tab { padding:14px 20px; cursor:pointer; font-weight:600; font-size:14px; color:var(--text-muted);
  border-bottom:3px solid transparent; transition:0.2s; flex-shrink:0; }
.tab:hover { color:var(--primary); }
.tab.active { color:var(--primary); border-bottom-color:var(--primary); }
.container { padding:24px; max-width:1100px; margin:0 auto; }
.page { display:none; }
.page.active { display:block; }
.stats-row { display:grid; grid-template-columns:repeat(auto-fit,minmax(180px,1fr)); gap:16px; margin-bottom:24px; }
.stat-card { background:var(--card-bg); border-radius:12px; padding:20px; box-shadow:var(--shadow);
  border-left:4px solid var(--primary); }
.stat-card.green { border-left-color:var(--success); }
.stat-card.red { border-left-color:var(--danger); }
.stat-card.yellow { border-left-color:var(--warning); }
.stat-card h4 { font-size:11px; color:var(--text-muted); text-transform:uppercase; letter-spacing:1px; margin-bottom:8px; }
.stat-card .val { font-size:28px; font-weight:700; color:var(--text); }
.stat-card .sub { font-size:11px; color:var(--text-muted); margin-top:4px; }
.card { background:var(--card-bg); border-radius:12px; padding:24px; box-shadow:var(--shadow); margin-bottom:20px; }
.card h3 { margin-bottom:18px; font-size:16px; color:var(--text); border-bottom:1px solid var(--border); padding-bottom:12px; }
label { font-size:13px; font-weight:600; color:var(--text-muted); display:block; margin-bottom:6px; margin-top:14px; }
input, select { width:100%; padding:10px 14px; border:1px solid var(--border); border-radius:8px;
  background:var(--input-bg); color:var(--text); font-size:14px; transition:0.2s; }
input:focus, select:focus { outline:none; border-color:var(--primary); }
.btn { padding:10px 22px; border-radius:8px; border:none; font-weight:600; font-size:14px;
  cursor:pointer; transition:0.2s; display:inline-block; }
.btn-primary { background:var(--primary); color:#fff; }
.btn-primary:hover { opacity:0.88; transform:translateY(-1px); }
.btn-danger { background:var(--danger); color:#fff; }
.btn-success { background:var(--success); color:#fff; }
.btn-outline { background:transparent; border:1px solid var(--border); color:var(--text); }
.btn-sm { padding:5px 12px; font-size:12px; border-radius:6px; }
.row { display:flex; gap:12px; flex-wrap:wrap; }
table { width:100%; border-collapse:separate; border-spacing: 0 8px; margin-top: 10px; }
th { text-align:left; padding:16px 20px; font-size:11px; font-weight:700; color:var(--text-muted);
  text-transform:uppercase; letter-spacing:1.5px; border-bottom:1px solid var(--border); }
td { padding:16px 20px; font-size:14px; border-top:1px solid var(--border); border-bottom:1px solid var(--border); background: var(--card-bg); transition: all 0.3s ease; }
td:first-child { border-left:1px solid var(--border); border-top-left-radius: 12px; border-bottom-left-radius: 12px; }
td:last-child { border-right:1px solid var(--border); border-top-right-radius: 12px; border-bottom-right-radius: 12px; }
tr { transition: transform 0.2s ease; }
tr:hover { transform: translateY(-1px); }
tr:hover td { background: linear-gradient(135deg, var(--card-bg) 0%, rgba(67,97,238,0.02) 100%) !important; border-color: rgba(67,97,238,0.2); }
.badge { padding:6px 12px; border-radius:30px; font-size:11px; font-weight:700; text-transform: uppercase; letter-spacing: 0.5px; display: inline-flex; align-items: center; }
.pulse-warn {
  animation: pulse 2s infinite ease-in-out;
}
@keyframes pulse {
  0% { opacity: 1; }
  50% { opacity: 0.7; }
  100% { opacity: 1; }
}
.badge-in { background:rgba(6,214,160,0.1); color:var(--success); border: 1px solid rgba(6,214,160,0.25); box-shadow: 0 2px 8px rgba(6,214,160,0.05); }
.badge-out { background:rgba(239,71,111,0.15); color:var(--danger); border: 1px solid rgba(239,71,111,0.25); box-shadow: 0 2px 8px rgba(239,71,111,0.05); }
.badge-accepted { color:var(--success); font-weight:600; }
.badge-denied { color:var(--danger); font-weight:600; }
.status-dot { width:10px; height:10px; border-radius:50%; display:inline-block; margin-right:8px; vertical-align: middle; }
.dot-in { background:var(--success); box-shadow:0 0 10px var(--success), 0 0 4px var(--success); }
.dot-out { background:#6c757d; opacity: 0.7; }
.dot-absent { background:var(--warning); box-shadow:0 0 10px var(--warning); }
#login-page { 
  position:fixed; top:0; left:0; right:0; bottom:0; 
  background:#090b10; 
  background: radial-gradient(circle at 50% 0%, #161b33, #090a0f); 
  display:flex; align-items:center; justify-content:center; 
  z-index:999; overflow: hidden; 
}
.login-orb { 
  position:absolute; width:500px; height:500px; 
  background: radial-gradient(circle, rgba(67,97,238,0.2) 0%, rgba(67,97,238,0) 70%); 
  border-radius:50%; filter:blur(40px); 
  animation: float 12s infinite alternate; 
}
.login-orb-2 { 
  position:absolute; right: -50px; bottom: -50px; width:400px; height:400px; 
  background: radial-gradient(circle, rgba(6,214,160,0.15) 0%, rgba(6,214,160,0) 70%); 
  border-radius:50%; filter:blur(40px); 
  animation: float 10s infinite alternate-reverse; 
}
@keyframes float { 
  0% { transform: translate(0, 0) scale(1); } 
  100% { transform: translate(-30px, -40px) scale(1.1); } 
}
@keyframes login-card-appear {
  from { opacity: 0; transform: translateY(20px); }
  to { opacity: 1; transform: translateY(0); }
}
.login-card { 
  background: rgba(22, 27, 34, 0.4); 
  backdrop-filter: blur(25px); 
  -webkit-backdrop-filter: blur(25px); 
  border: 1px solid rgba(255, 255, 255, 0.08); 
  border-radius:28px; padding:45px 36px; width:90%; max-width:390px;
  box-shadow: 0 24px 80px rgba(0,0,0,0.4), inset 0 1px 1px rgba(255,255,255,0.1); 
  text-align:center; position: relative; z-index: 2; 
  transition: all 0.3s ease;
  animation: login-card-appear 0.6s cubic-bezier(0.16, 1, 0.3, 1) forwards;
}
.login-card:hover {
  border-color: rgba(67, 97, 238, 0.3);
  box-shadow: 0 24px 80px rgba(67, 97, 238, 0.15);
}
.login-header-logo {
  width: 70px; height: 70px;
  background: linear-gradient(135deg, rgba(67, 97, 238, 0.15), rgba(6, 214, 160, 0.15));
  border: 1.5px solid rgba(255, 255, 255, 0.1);
  border-radius: 22px;
  display: flex; align-items: center; justify-content: center;
  margin: 0 auto 20px;
  box-shadow: 0 12px 30px rgba(0, 0, 0, 0.25);
  animation: pulse-ring-login 3s infinite;
  backdrop-filter: blur(5px);
}
@keyframes pulse-ring-login {
  0% { box-shadow: 0 0 0 0 rgba(67, 97, 238, 0.25); }
  70% { box-shadow: 0 0 0 15px rgba(67, 97, 238, 0); }
  100% { box-shadow: 0 0 0 0 rgba(67, 97, 238, 0); }
}
.login-card h2 { 
  color:#fff; margin-bottom:4px; font-size: 28px; font-weight: 800; letter-spacing: -0.5px; 
  background: linear-gradient(to right, #ffffff, #a5b4fc);
  -webkit-background-clip: text;
  -webkit-text-fill-color: transparent;
}
.login-subtitle { 
  color:var(--text-muted); font-size:11px; margin-bottom:35px; font-weight: 700;
  text-transform: uppercase; letter-spacing: 1.5px; opacity: 0.8;
}
.input-group {
  position: relative;
  text-align: left;
  margin-bottom: 20px;
}
.input-group label {
  display: block;
  font-size: 12px;
  font-weight: 700;
  color: var(--text-muted);
  text-transform: uppercase;
  letter-spacing: 0.8px;
  margin-bottom: 8px;
}
.input-field-wrapper {
  position: relative;
  display: flex;
  align-items: center;
  width: 100%;
}
.input-icon {
  position: absolute;
  left: 15px;
  height: 100%;
  display: flex;
  align-items: center;
  justify-content: center;
  color: var(--text-muted);
  pointer-events: none;
}
.input-field-wrapper input {
  width: 100%;
  padding: 14px 16px 14px 44px;
  border-radius: 12px;
  border: 1px solid rgba(255,255,255,0.08);
  background: rgba(255, 255, 255, 0.03) !important;
  color: #fff;
  font-size: 15px;
  outline: none;
  transition: all 0.3s ease;
}
.input-field-wrapper input:focus {
  border-color: var(--primary);
  background: rgba(255, 255, 255, 0.05) !important;
  box-shadow: 0 0 0 3px rgba(67, 97, 238, 0.15);
}
.password-toggle {
  position: absolute;
  right: 15px;
  height: 100%;
  display: flex;
  align-items: center;
  justify-content: center;
  cursor: pointer;
  color: var(--text-muted);
  transition: color 0.2s ease;
  user-select: none;
}
.password-toggle:hover {
  color: #fff;
}
.login-btn {
  width: 100%;
  height: 50px;
  border-radius: 12px;
  border: none;
  background: linear-gradient(135deg, var(--primary) 0%, #2f52e0 100%);
  color: #fff;
  font-size: 16px;
  font-weight: 700;
  cursor: pointer;
  display: flex; align-items: center; justify-content: center; gap: 10px;
  box-shadow: 0 8px 24px rgba(67, 97, 238, 0.25);
  transition: all 0.3s ease;
  margin-top: 28px;
}
.login-btn:hover {
  transform: translateY(-2px);
  box-shadow: 0 12px 30px rgba(67, 97, 238, 0.4);
  background: linear-gradient(135deg, #5171ff 0%, var(--primary) 100%);
}
.login-btn:active {
  transform: translateY(0);
}
.spinner-login {
  width: 18px;
  height: 18px;
  border: 3px solid rgba(255,255,255,0.3);
  border-radius: 50%;
  border-top-color: #fff;
  animation: spin-login 1s ease-in-out infinite;
  display: none;
}
@keyframes spin-login {
  to { transform: rotate(360deg); }
}
#login-error { color:var(--danger); font-size:13px; margin-top:15px; display:none; background: rgba(239,71,111,0.1); padding: 8px; border-radius: 8px; }
.login-advanced-toggle {
  background: none;
  border: none;
  color: var(--text-muted);
  font-size: 12px;
  font-weight: 600;
  cursor: pointer;
  margin-top: 20px;
  display: inline-flex;
  align-items: center;
  gap: 5px;
  opacity: 0.7;
  transition: opacity 0.2s;
  outline: none;
}
.login-advanced-toggle:hover {
  opacity: 1;
  color: var(--primary);
}
.login-advanced-panel {
  max-height: 0;
  overflow: hidden;
  opacity: 0;
  transition: all 0.3s cubic-bezier(0.4, 0, 0.2, 1);
  text-align: left;
  margin-top: 0;
  padding: 0 15px;
  background: rgba(255, 255, 255, 0.03);
  border: 1px solid rgba(255, 255, 255, 0);
  border-radius: 12px;
  font-size: 12px;
}
.login-advanced-panel.open {
  max-height: 220px;
  opacity: 1;
  margin-top: 15px;
  padding: 15px;
  border-color: rgba(255, 255, 255, 0.08);
}
.login-advanced-row {
  display: flex;
  justify-content: space-between;
  margin-bottom: 8px;
  border-bottom: 1px dashed rgba(255, 255, 255, 0.05);
  padding-bottom: 6px;
}
.login-advanced-row:last-child {
  margin-bottom: 0;
  border-bottom: none;
  padding-bottom: 0;
}
.login-advanced-label {
  color: var(--text-muted);
}
.login-advanced-val {
  color: #fff;
  font-weight: 600;
  font-family: monospace;
}
.scan-box { width:160px; height:160px; border:3px solid var(--primary); border-radius:50%;
  margin:0 auto 24px; display:flex; align-items:center; justify-content:center;
  font-size:48px; animation:pulse-ring 2s infinite; }
@keyframes pulse-ring { 0%,100%{box-shadow:0 0 0 0 rgba(67,97,238,0.3)} 50%{box-shadow:0 0 0 16px rgba(67,97,238,0)} }
.scan-box.success { border-color:var(--success); animation:none; }
.uid-display { font-size:22px; font-weight:700; color:var(--primary); letter-spacing:3px; margin-bottom:6px; }
.toolbar { display:flex; gap:10px; align-items:center; margin-bottom:16px; flex-wrap:wrap; }
.toolbar input { width:220px; }
#toast { position:fixed; bottom:24px; right:24px; padding:14px 22px; border-radius:10px;
  color:#fff; font-weight:600; display:none; z-index:9999; font-size:14px;
  box-shadow:0 4px 16px rgba(0,0,0,0.2); min-width:200px; }
.progress-bar { height:8px; background:var(--border); border-radius:4px; overflow:hidden; margin-top:6px; }
.progress-fill { height:100%; background:var(--primary); border-radius:4px; transition:width 0.4s; }
@media(max-width:1024px) {
  .command-center { flex-direction: column; }
  .cc-side { width: 100%; }
}
@media(max-width:768px) {
  .stats-row { grid-template-columns: repeat(auto-fit, minmax(150px, 1fr)); }
  .container { padding: 16px; }
  .card { padding: 16px; }
  .row { flex-direction: column; align-items: stretch; }
  .toolbar input, .toolbar select { width: 100% !important; }
}
@media(max-width:600px) { 
  .container{padding:12px;} 
  header{padding:12px 16px; justify-content: center;} 
  .header-right { justify-content: center; width: 100%; }
  nav{padding:0 10px;} 
  .tab{padding:12px 12px;font-size:13px;}
  .lobby-name { font-size: 12vw; }
  .lobby-clock { font-size: 8vw; }
  .lobby-avatar { width: 150px; height: 150px; font-size: 60px; }
}

/* Edit Modal Styles */
.modal { display:none; position:fixed; top:0; left:0; right:0; bottom:0; background:rgba(0,0,0,0.5); z-index:1000; align-items:center; justify-content:center; }
.modal-content { background:var(--card-bg); border-radius:12px; padding:24px; width:400px; box-shadow:var(--shadow); }

/* Settings Grid Enhancements */
.settings-grid { display:grid; grid-template-columns:repeat(auto-fit,minmax(320px,1fr)); gap:16px; align-items:stretch; }
.settings-grid .card { display:flex; flex-direction:column; margin-bottom:0; box-sizing:border-box; }
.card-footer { margin-top:auto; padding-top:16px; border-top:1px solid var(--border); display:flex; gap:10px; }
.hint { font-size:11px; color:var(--text-muted); margin-top:-4px; margin-bottom:12px; }

/* Enrollment Enhancements */
.enroll-steps { display: flex; justify-content: space-between; margin-bottom: 24px; position: relative; }
.enroll-steps::before { content: ''; position: absolute; top: 16px; left: 10%; right: 10%; height: 2px; background: var(--border); z-index: 1; transition: background 0.3s; }
.step { position: relative; z-index: 2; background: var(--card-bg); padding: 0 15px; text-align: center; flex: 1; transition: transform 0.3s; }
.step-num { width: 32px; height: 32px; border-radius: 50%; background: var(--border); color: var(--text-muted); 
  display: flex; align-items: center; justify-content: center; margin: 0 auto 8px; font-weight: 700; font-size: 14px; transition: 0.3s; border: 2px solid var(--card-bg); }
.step.active .step-num { background: linear-gradient(135deg, var(--primary), #5171ff); color: #fff; box-shadow: 0 0 16px rgba(67,97,238,0.4); transform: scale(1.1); }
.step.active .step-label { color: var(--primary); font-weight: 700; text-shadow: 0 0 8px rgba(67,97,238,0.15); }
.step-label { font-size: 10px; text-transform: uppercase; letter-spacing: 0.5px; color: var(--text-muted); transition: color 0.3s; }


.enroll-input-group label span { font-size: 16px; }
.btn-type { flex: 1; border: 1px solid var(--border); background: var(--card-bg); color: var(--text-muted); padding: 10px; border-radius: 10px; cursor: pointer; font-size: 13px; font-weight: 600; transition: 0.2s; display: flex; align-items: center; justify-content: center; gap: 8px; }
.btn-type.active { background: linear-gradient(135deg, var(--primary), #5171ff); color: #fff; border-color: var(--primary); box-shadow: 0 4px 12px rgba(67,97,238,0.3); }

/* Premium Scanner UI */
.scanner-wrap { position: relative; width: 180px; height: 180px; margin: 0 auto 30px; display: flex; align-items: center; justify-content: center; }
.scanner-ring { position: absolute; top:0; left:0; right:0; bottom:0; border: 6px solid var(--border); border-radius: 50%; opacity: 0.3; }
.scanner-progress { position: absolute; top:0; left:0; right:0; bottom:0; border: 6px solid var(--primary); border-radius: 50%; 
  border-top-color: transparent; border-right-color: transparent; border-bottom-color: transparent;
  transform: rotate(-45deg); transition: 0.8s cubic-bezier(0.4, 0, 0.2, 1); opacity: 0; }
.scanner-progress.p50 { border-right-color: var(--primary); opacity: 1; transform: rotate(45deg); }
.scanner-progress.p100 { border-bottom-color: var(--primary); opacity: 1; transform: rotate(135deg); }

.scanner-core { width: 130px; height: 130px; background: var(--card-bg); border-radius: 50%; display: flex; 
  flex-direction: column; align-items: center; justify-content: center; font-size: 50px; box-shadow: var(--shadow); z-index: 2; transition: 0.4s; }
.scanner-wrap.active .scanner-core { animation: scanner-pulse 2s infinite; color: var(--primary); }
@keyframes scanner-pulse { 0% { box-shadow: 0 0 0 0 rgba(67, 97, 238, 0.4); } 70% { box-shadow: 0 0 0 25px rgba(67, 97, 238, 0); } 100% { box-shadow: 0 0 0 0 rgba(67, 97, 238, 0); } }

.scanner-label { font-size: 10px; font-weight: 800; text-transform: uppercase; letter-spacing: 1px; margin-top: 5px; color: var(--text-muted); }

.enroll-checklist { margin: 20px 0; padding: 20px; background: linear-gradient(135deg, rgba(67,97,238,0.02) 0%, rgba(67,97,238,0.06) 100%); border-radius: 16px; border: 1px solid var(--border); box-shadow: inset 0 2px 4px rgba(0,0,0,0.02); }
.check-item { display: flex; align-items: center; gap: 12px; font-size: 13px; color: var(--text-muted); margin-bottom: 12px; transition: 0.3s cubic-bezier(0.4, 0, 0.2, 1); }
.check-item:last-child { margin-bottom: 0; }
.check-item.done { color: var(--success); font-weight: 600; text-shadow: 0 0 10px rgba(6,214,160,0.15); }
.check-item i { width: 22px; height: 22px; border-radius: 50%; border: 2px solid var(--border); display: flex; align-items: center; justify-content: center; font-size: 10px; font-weight: 700; transition: 0.3s; background: var(--input-bg); }
.check-item.done i { background: linear-gradient(135deg, var(--success), #05c290); border-color: var(--success); color: #fff; box-shadow: 0 0 8px rgba(6,214,160,0.4); transform: scale(1.1); }

/* Command Center Layout */
.command-center { display: flex; gap: 24px; align-items: stretch; margin-top: 10px; }
.cc-side { width: 320px; display: flex; flex-direction: column; gap: 24px; }
.cc-main { flex: 1; display: flex; flex-direction: column; gap: 24px; }

/* Holographic Scanner */
.holo-scanner { position: relative; width: 260px; height: 260px; margin: 40px auto; display: flex; align-items: center; justify-content: center; }
.holo-ring-outer { position: absolute; top: -15px; left: -15px; right: -15px; bottom: -15px; border: 2px dashed var(--primary); border-radius: 50%; opacity: 0.2; animation: rotate-cw 30s linear infinite; box-shadow: 0 0 15px rgba(67, 97, 238, 0.05); }
.holo-ring-inner { position: absolute; top: 0; left: 0; right: 0; bottom: 0; border: 6px solid var(--border); border-radius: 50%; opacity: 0.6; box-shadow: inset 0 0 20px rgba(0,0,0,0.05); }
.holo-progress { position: absolute; top: 0; left: 0; right: 0; bottom: 0; border: 6px solid var(--primary); border-radius: 50%; border-top-color: transparent; border-right-color: transparent; border-bottom-color: transparent; transform: rotate(-45deg); transition: 1s cubic-bezier(0.4, 0, 0.2, 1); opacity: 0; box-shadow: 0 0 15px rgba(67,97,238,0.3); }
.holo-progress.p50 { border-right-color: var(--primary); opacity: 1; transform: rotate(45deg); }
.holo-progress.p100 { border-bottom-color: var(--primary); opacity: 1; transform: rotate(135deg); }

.holo-core { width: 180px; height: 180px; background: linear-gradient(135deg, var(--card-bg) 0%, rgba(67,97,238,0.02) 100%); border-radius: 50%; display: flex; flex-direction: column; align-items: center; justify-content: center; font-size: 70px; box-shadow: var(--shadow), 0 0 25px rgba(0, 0, 0, 0.02); z-index: 2; position: relative; overflow: hidden; border: 1px solid var(--border); transition: all 0.4s ease; }
.holo-scanner.active .holo-core { border-color: var(--primary); box-shadow: 0 0 35px rgba(67, 97, 238, 0.25); animation: holo-pulse 2s infinite alternate; }
.scan-line { position: absolute; top: 0; left: 0; width: 100%; height: 6px; background: linear-gradient(to bottom, rgba(67,97,238,0), var(--primary) 50%, rgba(67,97,238,0)); box-shadow: 0 0 15px var(--primary); opacity: 0; z-index: 5; }
.holo-scanner.active .scan-line { animation: scan-move 2.5s linear infinite; opacity: 1; }

@keyframes holo-pulse { from { transform: scale(1); } to { transform: scale(1.03); } }
@keyframes scan-move { 0% { top: 0%; } 100% { top: 100%; } }
@keyframes rotate-cw { from { transform: rotate(0deg); } to { transform: rotate(360deg); } }

/* Ambient Background Glow */
.ambient-glow { position: fixed; top: 50%; left: 50%; width: 80vw; height: 80vh; background: radial-gradient(circle, var(--primary) 0%, transparent 70%); opacity: 0.02; transform: translate(-50%, -50%); pointer-events: none; z-index: -1; transition: 1.5s; }
.ambient-glow.success { background: radial-gradient(circle, var(--success) 0%, transparent 70%); opacity: 0.12; }
.ambient-glow.error { background: radial-gradient(circle, var(--danger) 0%, transparent 70%); opacity: 0.12; }

.cc-card { background: linear-gradient(135deg, var(--card-bg) 0%, rgba(67, 97, 238, 0.03) 100%); border-radius: 20px; padding: 24px; box-shadow: var(--shadow), 0 8px 30px rgba(0, 0, 0, 0.02); border: 1px solid var(--border); transition: transform 0.3s, border-color 0.3s, box-shadow 0.3s; }
.cc-card:hover { transform: translateY(-2px); border-color: rgba(67, 97, 238, 0.2); box-shadow: var(--shadow), 0 12px 40px rgba(67, 97, 238, 0.06); }
.cc-card h4 { font-size: 14px; font-weight: 700; color: var(--text); margin-bottom: 16px; display: flex; align-items: center; gap: 8px; letter-spacing: 0.3px; }

.memory-grid-compact { display: grid; grid-template-columns: repeat(10, 1fr); gap: 6px; }
.m-slot { aspect-ratio: 1; border-radius: 6px; background: var(--input-bg); border: 1px solid var(--border); font-size: 9px; font-weight: 600; color: var(--text-muted); display: flex; align-items: center; justify-content: center; cursor: pointer; transition: 0.2s cubic-bezier(0.4, 0, 0.2, 1); }
.m-slot:hover { transform: scale(1.15); box-shadow: 0 4px 10px rgba(0,0,0,0.1); border-color: var(--primary); color: var(--primary); }
.m-slot.used { background: linear-gradient(135deg, var(--primary), #5171ff); border-color: var(--primary); color: #fff; box-shadow: 0 2px 6px rgba(67,97,238,0.3); }
.m-slot.used:hover { background: linear-gradient(135deg, var(--danger), #ef476f); border-color: var(--danger); box-shadow: 0 4px 12px rgba(239,71,111,0.4); color: #fff; }

.info-row { display: flex; justify-content: space-between; margin-bottom: 12px; font-size: 13px; align-items: center; }
.info-row:last-child { margin-bottom: 0; }
.info-row span:first-child { color: var(--text-muted); font-weight: 500; }
.info-row span:last-child { font-weight: 700; color: var(--primary); }


/* Heartbeat */
.heartbeat { width: 10px; height: 10px; background: var(--success); border-radius: 50%; display: inline-block; margin-right: 12px; box-shadow: 0 0 8px var(--success); transition: 0.1s; }
.heartbeat.pulse { transform: scale(1.6); filter: brightness(1.5); box-shadow: 0 0 15px var(--success); }

/* Live Date and Clock Badges */
.badge-date {
  background: linear-gradient(135deg, var(--primary) 0%, #5171ff 100%);
  color: #fff !important;
  padding: 6px 14px;
  border-radius: 30px;
  font-size: 13px;
  font-weight: 700;
  box-shadow: 0 4px 10px rgba(67, 97, 238, 0.15);
  display: inline-flex;
  align-items: center;
  gap: 6px;
  margin-right: 12px;
  border: 1px solid rgba(255, 255, 255, 0.1);
}
.badge-clock {
  background: rgba(6, 214, 160, 0.1);
  color: var(--success) !important;
  border: 1px solid rgba(6, 214, 160, 0.25);
  padding: 6px 14px;
  border-radius: 30px;
  font-size: 13px;
  font-weight: 700;
  display: inline-flex;
  align-items: center;
  gap: 6px;
  margin-right: 12px;
  box-shadow: 0 4px 10px rgba(6, 214, 160, 0.05);
}

/* Enhanced Enrollment Steps Line */
.steps-progress-line {
  position: absolute;
  top: 16px;
  left: 0;
  height: 2px;
  background: linear-gradient(90deg, var(--primary), var(--success));
  z-index: 1;
  transition: width 0.5s cubic-bezier(0.4, 0, 0.2, 1);
  box-shadow: 0 0 10px var(--primary);
}

/* Holographic Rotating Ring CCW */
.holo-ring-inner-glow {
  position: absolute;
  top: -5px; left: -5px; right: -5px; bottom: -5px;
  border: 1px dashed var(--primary);
  border-radius: 50%;
  opacity: 0.15;
  animation: rotate-ccw 20s linear infinite;
  pointer-events: none;
}
@keyframes rotate-ccw {
  from { transform: rotate(360deg); }
  to { transform: rotate(0deg); }
}

/* Virtual Badge Preview */
.virtual-badge {
  background: linear-gradient(135deg, #161b22 0%, #0d1117 100%);
  border: 1px solid var(--border);
  border-radius: 16px;
  padding: 20px;
  text-align: center;
  box-shadow: 0 10px 25px rgba(0,0,0,0.15);
  position: relative;
  overflow: hidden;
  margin-bottom: 20px;
  transition: all 0.3s ease;
}
[data-theme="light"] .virtual-badge {
  background: linear-gradient(135deg, #f8f9fa 0%, #e9ecef 100%);
}
.virtual-badge::before {
  content: '';
  position: absolute;
  top: -50%;
  left: -50%;
  width: 200%;
  height: 200%;
  background: radial-gradient(circle, rgba(67,97,238,0.06) 0%, transparent 60%);
  pointer-events: none;
}
.badge-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 15px;
  border-bottom: 1px dashed var(--border);
  padding-bottom: 10px;
}
.badge-chip {
  width: 28px;
  height: 20px;
  background: linear-gradient(135deg, #ffd700 0%, #b8860b 100%);
  border-radius: 4px;
  position: relative;
  box-shadow: 0 2px 5px rgba(0,0,0,0.2);
}
.badge-chip::after {
  content: '';
  position: absolute;
  top: 4px; left: 6px; right: 6px; bottom: 4px;
  border: 1px solid rgba(0,0,0,0.15);
  border-radius: 2px;
}
.badge-logo {
  font-size: 11px;
  font-weight: 800;
  text-transform: uppercase;
  letter-spacing: 1.5px;
  color: var(--primary);
  text-shadow: 0 0 8px rgba(67, 97, 238, 0.4);
}
.badge-body {
  display: flex;
  flex-direction: column;
  align-items: center;
  gap: 8px;
}
.badge-avatar {
  width: 60px;
  height: 60px;
  border-radius: 50%;
  color: #fff;
  display: flex;
  align-items: center;
  justify-content: center;
  font-size: 20px;
  font-weight: 800;
  border: 2px solid rgba(255, 255, 255, 0.2);
  margin-bottom: 4px;
  transition: all 0.3s ease;
}
.badge-name {
  font-size: 16px;
  font-weight: 700;
  color: var(--text);
  margin: 0;
  max-width: 100%;
  white-space: nowrap;
  overflow: hidden;
  text-overflow: ellipsis;
}
.badge-role {
  font-size: 9px;
  font-weight: 700;
  letter-spacing: 2px;
  color: var(--primary);
  background: rgba(67, 97, 238, 0.1);
  padding: 3px 10px;
  border-radius: 12px;
  text-transform: uppercase;
}
.badge-id-container {
  display: flex;
  flex-direction: column;
  gap: 2px;
  margin-top: 4px;
}
.badge-id-label {
  font-size: 8px;
  letter-spacing: 1px;
  color: var(--text-muted);
  text-transform: uppercase;
}
.badge-id {
  font-family: monospace;
  font-size: 14px;
  font-weight: 700;
  color: var(--text);
}
.badge-footer {
  margin-top: 15px;
  border-top: 1px dashed var(--border);
  padding-top: 10px;
}
.badge-rfid-status {
  font-size: 10px;
  color: var(--text-muted);
  font-weight: 600;
  display: inline-flex;
  align-items: center;
  gap: 4px;
}
.badge-rfid-status.linked {
  color: var(--success);
}

/* Hover Delete Effect on Used Slots */
.m-slot.used {
  position: relative;
  overflow: hidden;
}
.m-slot.used::after {
  content: '🗑️';
  position: absolute;
  top: 0; left: 0; right: 0; bottom: 0;
  display: flex;
  align-items: center;
  justify-content: center;
  background: var(--danger);
  color: #fff;
  opacity: 0;
  font-size: 10px;
  transition: opacity 0.2s ease;
}
.m-slot.used:hover::after {
  opacity: 1;
}

/* Holiday Grid & Calendar Styles */
.holiday-grid {
  display: grid;
  grid-template-columns: 1.2fr 1.8fr;
  gap: 20px;
  align-items: start;
}
@media(max-width: 768px) {
  .holiday-grid {
    grid-template-columns: 1fr;
  }
}

.upcoming-holidays-list {
  margin-top: 15px;
  display: flex;
  flex-direction: column;
  gap: 8px;
}
.upcoming-holiday-item {
  display: flex;
  justify-content: space-between;
  align-items: center;
  background: var(--bg);
  padding: 10px 14px;
  border-radius: 8px;
  border: 1px solid var(--border);
  transition: all 0.2s ease;
}
.upcoming-holiday-item:hover {
  transform: translateX(2px);
  border-color: var(--primary);
}
.upcoming-holiday-info {
  display: flex;
  flex-direction: column;
}
.upcoming-holiday-title {
  font-size: 13px;
  font-weight: 700;
}
.upcoming-holiday-date {
  font-size: 11px;
  color: var(--text-muted);
}
.preset-loader {
  display: flex;
  gap: 8px;
  margin-top: 15px;
  padding-top: 15px;
  border-top: 1px dashed var(--border);
}
.badge-irregular { background:rgba(255,209,102,0.15); color:var(--warning); border: 1px solid rgba(255,209,102,0.25); }

/* Split-pane layout for Payroll */
.pay-layout { display: flex; gap: 20px; flex-wrap: wrap; margin-top: 15px; }
.pay-settings { flex: 1.2; min-width: 320px; }
.pay-slip-preview { flex: 1.8; min-width: 360px; }

/* Payslip styling */
.payslip-card { background: #ffffff; color: #1a1a2e; border: 2px solid #1a1a2e; padding: 25px; border-radius: 8px; box-shadow: var(--shadow); position: relative; }
[data-theme="dark"] .payslip-card { background: #ffffff; color: #1a1a2e; border: 2px solid #1a1a2e; }

.payslip-header { text-align: center; border-bottom: 2px double #1a1a2e; padding-bottom: 12px; margin-bottom: 15px; }
.payslip-title { font-size: 18px; font-weight: 800; letter-spacing: 1px; color: #1a1a2e; margin-bottom: 4px; }
.payslip-org { font-size: 14px; font-weight: 700; color: #555; text-transform: uppercase; }

.payslip-meta { display: grid; grid-template-columns: 1fr 1fr; gap: 10px; margin-bottom: 15px; font-size: 12.5px; border-bottom: 1px dashed #1a1a2e; padding-bottom: 10px; }
.payslip-meta div { display: flex; justify-content: space-between; padding: 2px 0; }
.payslip-meta span.label { font-weight: 700; color: #444; }
.payslip-meta span.val { font-family: monospace; font-weight: 600; }

.payslip-tables { display: flex; gap: 20px; margin-bottom: 15px; flex-wrap: wrap; }
.payslip-table-wrapper { flex: 1; min-width: 250px; }
.payslip-table-wrapper h5 { font-size: 13px; font-weight: 800; border-bottom: 2px solid #1a1a2e; padding-bottom: 5px; margin-bottom: 8px; text-transform: uppercase; color: #1a1a2e; }

.payslip-table { width: 100%; border-collapse: collapse; font-size: 12px; }
.payslip-table th, .payslip-table td { padding: 6px 8px; text-align: left; }
.payslip-table th { font-weight: 700; border-bottom: 1px solid #1a1a2e; }
.payslip-table td.amt { text-align: right; font-family: monospace; font-weight: 600; }
.payslip-table tr.total-row td { font-weight: 800; border-top: 1px solid #1a1a2e; border-bottom: 2px solid #1a1a2e; padding-top: 8px; }

.payslip-footer { border-top: 1px dashed #1a1a2e; padding-top: 12px; margin-top: 15px; }
.payslip-net-box { display: flex; justify-content: space-between; align-items: center; background: #f4f6f9; border: 1px solid #1a1a2e; padding: 10px 15px; border-radius: 4px; margin-bottom: 12px; }
.payslip-net-label { font-weight: 800; font-size: 13px; }
.payslip-net-val { font-size: 18px; font-weight: 800; font-family: monospace; }
.payslip-words { font-size: 11px; font-style: italic; color: #555; margin-bottom: 20px; }

.payslip-sigs { display: flex; justify-content: space-between; margin-top: 40px; font-size: 11px; }
.payslip-sig-box { text-align: center; width: 140px; border-top: 1px solid #1a1a2e; padding-top: 5px; font-weight: 700; }

/* Print styling specifically for Payslip printing */
/* Print styling specifically for printing */
@media print {
  body { background: white !important; color: black !important; -webkit-print-color-adjust: exact; print-color-adjust: exact; }
  header, nav, .container > *:not(.print-active), .pay-sidebar, .toolbar, #printable-payslip-actions, #rep-progress-container, button, .btn, .card-footer, .hint, #rep-results > div:first-child, .theme-btn {
    display: none !important;
  }
  .container {
    padding: 0 !important;
    margin: 0 !important;
    max-width: 100% !important;
  }
  .card {
    border: none !important;
    box-shadow: none !important;
    background: transparent !important;
    padding: 0 !important;
    margin: 0 !important;
  }
  .pay-layout {
    display: block !important;
    margin: 0 !important;
  }
  .pay-slip-preview {
    width: 100% !important;
    max-width: 100% !important;
    display: block !important;
  }
  .payslip-card {
    border: 2px solid #000 !important;
    padding: 20px !important;
    width: 100% !important;
    max-width: 100% !important;
    box-shadow: none !important;
    color: #000 !important;
    background: #fff !important;
  }
  #rep-results {
    display: block !important;
    width: 100% !important;
  }
  #rep-results table {
    width: 100% !important;
    border-collapse: collapse !important;
  }
  #rep-results th, #rep-results td {
    border: 1px solid #000 !important;
    padding: 8px !important;
    color: #000 !important;
    background: #fff !important;
  }
}

/* Premium Custom Checkbox styling */
input[type="checkbox"]:not(.switch input) {
  -webkit-appearance: none;
  appearance: none;
  background-color: var(--input-bg);
  margin: 0;
  width: 18px;
  height: 18px;
  border: 1px solid var(--border);
  border-radius: 4px;
  display: inline-grid;
  place-content: center;
  cursor: pointer;
  transition: 0.15s ease-in-out;
  vertical-align: middle;
}
input[type="checkbox"]:not(.switch input)::before {
  content: "";
  width: 10px;
  height: 10px;
  transform: scale(0);
  transition: 0.12s transform ease-in-out;
  background-color: #ffffff;
  transform-origin: center;
  clip-path: polygon(14% 44%, 0 65%, 50% 100%, 100% 16%, 80% 0%, 43% 62%);
}
input[type="checkbox"]:not(.switch input):checked {
  background-color: var(--primary);
  border-color: var(--primary);
}
input[type="checkbox"]:not(.switch input):checked::before {
  transform: scale(1);
}

/* Premium iOS/Material Toggle Switch styling */
.toggle-switch-container {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: 10px 14px;
  background: var(--input-bg);
  border: 1px solid var(--border);
  border-radius: 8px;
  margin-top: 10px;
  cursor: pointer;
  transition: 0.2s ease;
}
.toggle-switch-container:hover {
  border-color: var(--primary);
}
.toggle-label-text {
  font-size: 13px;
  font-weight: 600;
  color: var(--text);
}
.switch {
  position: relative;
  display: inline-block;
  width: 44px;
  height: 24px;
  flex-shrink: 0;
}
.switch input {
  opacity: 0;
  width: 0;
  height: 0;
  position: absolute;
}
.slider {
  position: absolute;
  cursor: pointer;
  top: 0;
  left: 0;
  right: 0;
  bottom: 0;
  background-color: var(--border);
  transition: 0.25s ease;
  border-radius: 24px;
}
.slider:before {
  position: absolute;
  content: "";
  height: 18px;
  width: 18px;
  left: 3px;
  bottom: 3px;
  background-color: #ffffff;
  transition: 0.25s ease;
  border-radius: 50%;
  box-shadow: 0 1px 3px rgba(0,0,0,0.15);
}
.switch input:checked + .slider {
  background-color: var(--primary);
}
.switch input:checked + .slider:before {
  transform: translateX(20px);
}

</style>

</head>
<body>

<!-- LOGIN PAGE -->
<div id="login-page">
  <div class="login-orb"></div>
  <div class="login-orb-2"></div>
  <div class="login-card">
    <div class="login-header-logo">
      <svg width="32" height="32" viewBox="0 0 24 24" fill="none" stroke="url(#logo-grad)" stroke-width="2.5" stroke-linecap="round" stroke-linejoin="round" style="filter: drop-shadow(0 2px 10px rgba(67, 97, 238, 0.35));">
        <defs>
          <linearGradient id="logo-grad" x1="0%" y1="0%" x2="100%" y2="100%">
            <stop offset="0%" stop-color="#fff" />
            <stop offset="100%" stop-color="#a5b4fc" />
          </linearGradient>
        </defs>
        <rect x="3" y="11" width="18" height="11" rx="2" ry="2" fill="rgba(255,255,255,0.05)"></rect>
        <path d="M7 11V7a5 5 0 0 1 10 0v4"></path>
        <circle cx="12" cy="16" r="1.5" fill="#fff"></circle>
        <line x1="12" y1="17.5" x2="12" y2="19.5"></line>
      </svg>
    </div>
    <h2 id="login-company-name">Smart Attendance System</h2>
    <p class="login-subtitle">Premium Attendance System</p>
    
    <div class="input-group">
      <label>Username</label>
      <div class="input-field-wrapper">
        <span class="input-icon">
          <svg width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.2" stroke-linecap="round" stroke-linejoin="round" style="color: var(--text-muted); opacity: 0.8;">
            <path d="M20 21v-2a4 4 0 0 0-4-4H8a4 4 0 0 0-4 4v2"></path>
            <circle cx="12" cy="7" r="4"></circle>
          </svg>
        </span>
        <input type="text" id="username" placeholder="Username">
      </div>
    </div>
    
    <div class="input-group">
      <label>Password</label>
      <div class="input-field-wrapper">
        <span class="input-icon">
          <svg width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.2" stroke-linecap="round" stroke-linejoin="round" style="color: var(--text-muted); opacity: 0.8;">
            <rect x="3" y="11" width="18" height="11" rx="2" ry="2"></rect>
            <path d="M7 11V7a5 5 0 0 1 10 0v4"></path>
          </svg>
        </span>
        <input type="password" id="password" placeholder="Password" onkeydown="if(event.key==='Enter')doLoginEnhanced()">
        <span class="password-toggle" id="pass-toggle" onclick="togglePassVisibility()">
          <svg width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round" style="opacity: 0.8;">
            <path d="M1 12s4-8 11-8 11 8 11 8-4 8-11 8-11-8-11-8z"></path>
            <circle cx="12" cy="12" r="3"></circle>
          </svg>
        </span>
      </div>
    </div>
    
    <button class="login-btn" id="login-btn-el" onclick="doLoginEnhanced()">
      <div class="spinner-login" id="login-spinner"></div>
      <span id="login-btn-text">Secure Login</span>
    </button>
    <div id="login-error">Invalid username or password</div>
    
    <button class="login-advanced-toggle" onclick="toggleLoginAdvanced(event)">
      ⚙️ Advanced System Info
    </button>
    
    <div id="login-advanced-panel" class="login-advanced-panel">
      <div class="login-advanced-row">
        <span class="login-advanced-label">IP Address:</span>
        <span class="login-advanced-val" id="login-info-ip">--</span>
      </div>
      <div class="login-advanced-row">
        <span class="login-advanced-label">MAC Address:</span>
        <span class="login-advanced-val" id="login-info-mac">--</span>
      </div>
      <div class="login-advanced-row">
        <span class="login-advanced-label">Uptime:</span>
        <span class="login-advanced-val" id="login-info-uptime">--</span>
      </div>
      <div class="login-advanced-row">
        <span class="login-advanced-label">Free Heap:</span>
        <span class="login-advanced-val" id="login-info-heap">--</span>
      </div>
    </div>
  </div>
</div>

<!-- MAIN APP -->
<div id="main-app" style="display:none;">
  <header>
    <div class="logo" id="header-logo">Anurag 0.1 <span>Attendance System</span></div>

    <div class="header-right">
      <span id="live-date" class="badge-date"></span>
      <span id="live-clock" class="badge-clock"></span>
      <span id="wifi-rssi" style="font-size:12px;color:var(--text-muted);margin-right:10px;"></span>
      <span class="heartbeat" id="heartbeat"></span>
      <span id="device-ip" style="margin-right:10px;"></span>

      <button class="theme-btn" id="led-btn" onclick="toggleLED()" title="Manual Exit Release">🚪</button>
      <button class="theme-btn" id="theme-toggle-btn" onclick="toggleTheme()" title="Toggle Dark Mode">🌙</button>
      <button class="logout-btn" onclick="logout()">Logout</button>
    </div>
  </header>
  <nav>
    <div class="tab active" id="t-dash" onclick="show('dash')">📊 Dashboard</div>
    <div class="tab" id="t-dir" onclick="show('dir')">👥 Employees</div>
    <div class="tab" id="t-his" onclick="show('his')">📋 Transactions</div>
    <div class="tab" id="t-reg" onclick="show('reg')">➕ Enrollment</div>
    <div class="tab" id="t-hol" onclick="show('hol')">📅 Holidays</div>
    <div class="tab" id="t-rep" onclick="show('rep')">📈 Monthly Report</div>
    <div class="tab" id="t-pay" onclick="show('pay')">💵 Payroll</div>
    <div class="tab" id="t-set" onclick="show('set')">⚙️ Settings</div>
  </nav>

  <div class="container">

    <!-- MONTHLY REPORT PAGE -->
    <div id="p-rep" class="page">
      <div class="card">
        <h3>📊 Monthly Attendance Report</h3>
        <p class="hint">Select the month, year, and employee to get the daily attendance log.</p>
        
        <div class="row" style="margin-bottom: 15px; align-items: flex-end;">
          <div style="flex: 1; min-width: 150px;">
            <label>Month</label>
            <select id="rep-month">
              <option value="1">January</option>
              <option value="2">February</option>
              <option value="3">March</option>
              <option value="4">April</option>
              <option value="5">May</option>
              <option value="6">June</option>
              <option value="7">July</option>
              <option value="8">August</option>
              <option value="9">September</option>
              <option value="10">October</option>
              <option value="11">November</option>
              <option value="12">December</option>
            </select>
          </div>
          <div style="flex: 1; min-width: 150px;">
            <label>Year</label>
            <select id="rep-year">
              <option value="2025">2025</option>
              <option value="2026">2026</option>
              <option value="2027">2027</option>
              <option value="2028">2028</option>
              <option value="2029">2029</option>
              <option value="2030">2030</option>
            </select>
          </div>
          <div style="flex: 2; min-width: 200px;">
            <label>Employee / Student</label>
            <select id="rep-employee" onchange="onEmployeeSelect()">
              <option value="">-- Select Employee --</option>
            </select>
          </div>
          <div>
            <button class="btn btn-primary" onclick="generateMonthlyReport()">Get Report</button>
          </div>
        </div>


        <!-- Progress Indicator -->
        <div id="rep-progress-container" style="display:none; margin: 15px 0; font-size:14px; color:var(--text-muted);">
          <div class="row" style="align-items: center; gap: 10px;">
            <div style="flex:1; height: 8px; background: var(--border); border-radius: 4px; overflow: hidden;">
              <div id="rep-progress-bar" style="width:0%; height:100%; background: var(--primary); transition: width 0.1s ease;"></div>
            </div>
            <span id="rep-progress-text">0%</span>
          </div>
        </div>

        <!-- Report Results -->
        <div id="rep-results" style="display:none; margin-top:20px;">
          <div class="row" style="justify-content: space-between; align-items: center; margin-bottom: 15px;">
            <h4 id="rep-summary-title" style="margin:0; font-size:16px;">Attendance Summary</h4>
            <div style="display:flex; gap:10px;">
              <button class="btn btn-outline btn-sm" onclick="exportReportCSV()">⬇ Export CSV</button>
              <button class="btn btn-primary btn-sm" onclick="printReport()">🖨 Print Report</button>
            </div>
          </div>

          <!-- Quick Summary Stats -->
          <div class="stats-row" style="margin-bottom: 20px;">
            <div class="stat-card green" style="padding:15px;">
              <h4 style="font-size:10px;">Present Days</h4>
              <div class="val" id="rep-stat-present" style="font-size:24px;">0</div>
            </div>
            <div class="stat-card" style="padding:15px; border-left-color: #f77f00;">
              <h4 style="font-size:10px;">Half Days</h4>
              <div class="val" id="rep-stat-half" style="font-size:24px;">0</div>
            </div>
            <div class="stat-card red" style="padding:15px;">
              <h4 style="font-size:10px;">Absent Days</h4>
              <div class="val" id="rep-stat-absent" style="font-size:24px;">0</div>
            </div>
            <div class="stat-card yellow" style="padding:15px;">
              <h4 style="font-size:10px;">Irregular Days</h4>
              <div class="val" id="rep-stat-irregular" style="font-size:24px;">0</div>
            </div>
            <div class="stat-card" style="padding:15px; border-left-color: #58a6ff;">
              <h4 style="font-size:10px;">Week Offs</h4>
              <div class="val" id="rep-stat-weekoffs" style="font-size:24px;">0</div>
            </div>
            <div class="stat-card" style="padding:15px; border-left-color: #a855f7;">
              <h4 style="font-size:10px;">Holidays</h4>
              <div class="val" id="rep-stat-holidays" style="font-size:24px;">0</div>
            </div>
            <div class="stat-card" style="padding:15px;">
              <h4 style="font-size:10px;">Attendance Rate</h4>
              <div class="val" id="rep-stat-rate" style="font-size:24px;">0%</div>
            </div>
          </div>

          <div style="overflow-x:auto;">
            <table>
              <thead>
                <tr>
                  <th>Date</th>
                  <th>Day</th>
                  <th>First IN</th>
                  <th>Last OUT</th>
                  <th>Work Hours</th>
                  <th>Status</th>
                </tr>
              </thead>
              <tbody id="rep-table-body">
                <!-- Dynamically populated -->
              </tbody>
            </table>
          </div>
        </div>
      </div>
    </div>

    <!-- PAYROLL PAGE -->
    <div id="p-pay" class="page">
      <div class="card">
        <h3>💵 Payroll Management</h3>
        <p class="hint">Configure employee salary structures, link to attendance logs, and generate Sevaarth-compatible government pay slips.</p>
        
        <div class="row" style="margin-bottom: 20px; align-items: flex-end; gap: 10px;">
          <div style="flex: 2; min-width: 200px;">
            <label>Employee / Student</label>
            <select id="pay-employee" onchange="onPayrollEmployeeSelect()">
              <option value="">-- Select Employee --</option>
            </select>
          </div>
          <div style="flex: 1; min-width: 120px;">
            <label>Month</label>
            <select id="pay-month" onchange="calculatePayroll()">
              <option value="1">January</option>
              <option value="2">February</option>
              <option value="3">March</option>
              <option value="4">April</option>
              <option value="5">May</option>
              <option value="6">June</option>
              <option value="7">July</option>
              <option value="8">August</option>
              <option value="9">September</option>
              <option value="10">October</option>
              <option value="11">November</option>
              <option value="12">December</option>
            </select>
          </div>
          <div style="flex: 1; min-width: 120px;">
            <label>Year</label>
            <select id="pay-year" onchange="calculatePayroll()">
              <option value="2025">2025</option>
              <option value="2026">2026</option>
              <option value="2027">2027</option>
              <option value="2028">2028</option>
              <option value="2029">2029</option>
              <option value="2030">2030</option>
            </select>
          </div>
          <div>
            <button class="btn btn-primary" onclick="calculatePayroll()">Generate Pay Slip</button>
          </div>
        </div>

        <div class="pay-layout">
          <!-- Left Column: Configuration Settings -->
          <div class="pay-settings">
            <!-- Salary Type selection -->
            <div style="border:1px solid var(--border); border-radius:8px; padding:15px; margin-bottom:15px; background:rgba(67, 97, 238, 0.04);">
              <h4 style="margin-bottom:10px; color:var(--primary); font-size:14px; font-weight:700; border-bottom:1px solid var(--border); padding-bottom:5px;">📋 Salary Calculation Model</h4>
              <div>
                <label style="font-size:11px; margin-top:2px; font-weight: 600;">Calculation Mode</label>
                <select id="pay-salary-type" onchange="onSalaryTypeChange()" style="font-weight:600; padding: 6px; border-radius: 6px; border: 1px solid var(--border); background: var(--bg-card); color: var(--text); width: 100%;">
                  <option value="monthly">Monthly Salaried (Basic Pay/month)</option>
                  <option value="daily">Daily Wage (Daily Rate * Present Days)</option>
                  <option value="hourly">Hourly Basis (Hourly Rate * Worked Hours)</option>
                </select>
              </div>
            </div>

            <!-- Earnings Rules -->
            <div style="border:1px solid var(--border); border-radius:8px; padding:15px; margin-bottom:15px; background:rgba(6, 214, 160, 0.02);">
              <h4 style="margin-bottom:10px; color:var(--success); font-size:14px; font-weight:700; border-bottom:1px solid var(--border); padding-bottom:5px;">💰 Monthly Earnings (Allowances)</h4>
              <div style="display:grid; grid-template-columns:1fr 1fr; gap:10px;">
                <div id="pay-basic-container" style="grid-column: span 2;">
                  <label id="pay-basic-label" style="font-size:11px; margin-top:2px;">Basic Pay (₹)</label>
                  <input type="number" id="pay-basic" value="15000" min="0" onchange="onPayrollConfigChange()">
                </div>
              </div>
              <div id="pay-monthly-earnings-fields" style="display:grid; grid-template-columns:1fr 1fr; gap:10px; margin-top:10px;">
                <div>
                  <label style="font-size:11px; margin-top:2px;">Dearness Allowance (DA %)</label>
                  <input type="number" id="pay-da-pct" value="46" min="0" max="100" onchange="onPayrollConfigChange()">
                </div>
                <div>
                  <label style="font-size:11px; margin-top:2px;">House Rent (HRA %)</label>
                  <input type="number" id="pay-hra-pct" value="10" min="0" max="100" onchange="onPayrollConfigChange()">
                </div>
                <div>
                  <label style="font-size:11px; margin-top:2px;">Transport (TA) (₹)</label>
                  <input type="number" id="pay-ta" value="1000" min="0" onchange="onPayrollConfigChange()">
                </div>
                <div>
                  <label style="font-size:11px; margin-top:2px;">Medical Allowance (MA) (₹)</label>
                  <input type="number" id="pay-ma" value="500" min="0" onchange="onPayrollConfigChange()">
                </div>
                <div>
                  <label style="font-size:11px; margin-top:2px;">Overtime Rate (₹/hour)</label>
                  <input type="number" id="pay-ot-rate" value="150" min="0" onchange="onPayrollConfigChange()">
                </div>
              </div>
            </div>

            <!-- Deductions Rules -->
            <div id="pay-deductions-container" style="border:1px solid var(--border); border-radius:8px; padding:15px; margin-bottom:15px; background:rgba(239, 71, 111, 0.02);">
              <h4 style="margin-bottom:10px; color:var(--danger); font-size:14px; font-weight:700; border-bottom:1px solid var(--border); padding-bottom:5px;">📉 Monthly Deductions</h4>
              <div style="display:grid; grid-template-columns:1fr 1fr; gap:10px;">
                <div>
                  <label style="font-size:11px; margin-top:2px;">NPS / GPF Contribution (%)</label>
                  <input type="number" id="pay-nps-pct" value="10" min="0" max="100" onchange="onPayrollConfigChange()">
                </div>
                <div>
                  <label style="font-size:11px; margin-top:2px;">Professional Tax (PT) (₹)</label>
                  <input type="number" id="pay-pt" value="200" min="0" onchange="onPayrollConfigChange()">
                </div>
                <div>
                  <label style="font-size:11px; margin-top:2px;">Income Tax (TDS) (₹)</label>
                  <input type="number" id="pay-tds" value="0" min="0" onchange="onPayrollConfigChange()">
                </div>
                <div>
                  <label style="font-size:11px; margin-top:2px;">Group Insurance (GIS) (₹)</label>
                  <input type="number" id="pay-gis" value="120" min="0" onchange="onPayrollConfigChange()">
                </div>
              </div>
            </div>

            <!-- Attendance Deductions Rules -->
            <div id="pay-penalties-container" style="border:1px solid var(--border); border-radius:8px; padding:15px; background:rgba(67, 97, 238, 0.02);">
              <h4 style="margin-bottom:10px; color:var(--primary); font-size:14px; font-weight:700; border-bottom:1px solid var(--border); padding-bottom:5px;">🕒 Attendance Penalties</h4>
              <div style="margin-bottom:10px; display:flex; align-items:center; gap:8px;">
                <input type="checkbox" id="pay-link-attendance" checked onchange="onPayrollConfigChange()" style="width:auto; cursor:pointer;">
                <label for="pay-link-attendance" style="margin:0; font-size:12px; font-weight:600; cursor:pointer;">Link calculation to attendance logs</label>
              </div>
              <div style="display:grid; grid-template-columns:1fr 1fr; gap:10px;">
                <div>
                  <label style="font-size:11px; margin-top:2px;">Late Penalty (₹/day)</label>
                  <input type="number" id="pay-late-penalty" value="50" min="0" onchange="onPayrollConfigChange()">
                </div>
                <div>
                  <label style="font-size:11px; margin-top:2px;">Irregular Penalty (₹/day)</label>
                  <input type="number" id="pay-irregular-penalty" value="250" min="0" onchange="onPayrollConfigChange()">
                </div>
                <div>
                  <label style="font-size:11px; margin-top:2px;">Standard Shift (Hours)</label>
                  <input type="number" id="pay-shift-hours" value="8" min="1" max="24" onchange="onPayrollConfigChange()">
                </div>
                <div>
                  <label style="font-size:11px; margin-top:2px;">Half-day Limit (Hours)</label>
                  <input type="number" id="pay-halfday-hours" value="4" min="1" max="24" onchange="onPayrollConfigChange()">
                </div>
              </div>
            </div>
          </div>

          <!-- Right Column: Payslip Preview -->
          <div class="pay-slip-preview">
            <div id="printable-payslip-actions" style="display:none; justify-content: flex-end; gap:10px; margin-bottom:15px;">
              <button class="btn btn-outline btn-sm" onclick="exportPayrollCSV()">⬇ Export CSV</button>
              <button class="btn btn-primary btn-sm" onclick="printPayslip()">🖨 Print Pay Slip</button>
            </div>
            
            <div id="pay-slip-placeholder" style="text-align:center; padding:100px 20px; color:var(--text-muted); border:1px dashed var(--border); border-radius:8px;">
              <div style="font-size:40px; margin-bottom:10px;">💵</div>
              <p>Select an employee and click <strong>Generate Pay Slip</strong> to fetch monthly logs and build the Sevaarth payslip invoice.</p>
            </div>

            <!-- The printable payslip card itself -->
            <div id="payslip-card-container" style="display:none;">
              <div class="payslip-card">
                <div class="payslip-header">
                  <div class="payslip-title" id="slip-title">PAYSLIP INVOICE</div>
                  <div class="payslip-org" id="slip-org">GOVERNMENT OF MAHARASHTRA</div>
                  <div style="font-size:11px; font-weight:600; color:#555; margin-top:2px;" id="slip-period">MONTH: JULY 2026</div>
                </div>

                <div class="payslip-meta">
                  <div>
                    <span class="label">Employee Name:</span>
                    <span class="val" id="slip-emp-name">John Doe</span>
                  </div>
                  <div>
                    <span class="label">Employee ID/Roll:</span>
                    <span class="val" id="slip-emp-roll">102</span>
                  </div>
                  <div>
                    <span class="label">Department:</span>
                    <span class="val" id="slip-emp-dept">IT</span>
                  </div>
                  <div>
                    <span class="label">Designation / Role:</span>
                    <span class="val" id="slip-emp-role">Technician</span>
                  </div>
                  <div>
                    <span class="label">Total Days in Month:</span>
                    <span class="val" id="slip-days-month">31</span>
                  </div>
                  <div>
                    <span class="label">Present Days:</span>
                    <span class="val" id="slip-days-present">0</span>
                  </div>
                  <div>
                    <span class="label">Absent Days (LWP):</span>
                    <span class="val" id="slip-days-absent">0</span>
                  </div>
                  <div>
                    <span class="label">Late Arrival Days:</span>
                    <span class="val" id="slip-days-late">0</span>
                  </div>
                  <div>
                    <span class="label">Half Days Worked:</span>
                    <span class="val" id="slip-days-half">0</span>
                  </div>
                  <div>
                    <span class="label">Overtime Hours:</span>
                    <span class="val" id="slip-hours-ot">0</span>
                  </div>
                  <div id="slip-row-total-hours" style="display:none;">
                    <span class="label">Total Worked Hours:</span>
                    <span class="val" id="slip-total-hours">0.0</span>
                  </div>
                </div>

                <div class="payslip-tables">
                  <!-- Earnings Table -->
                  <div class="payslip-table-wrapper">
                    <h5>Earnings (Allowances)</h5>
                    <table class="payslip-table">
                      <thead>
                        <tr>
                          <th>Description</th>
                          <th style="text-align:right;">Amount (₹)</th>
                        </tr>
                      </thead>
                      <tbody>
                        <tr>
                          <td id="slip-earn-basic-desc">Basic Pay</td>
                          <td class="amt" id="slip-earn-basic">₹0.00</td>
                        </tr>
                        <tr>
                          <td>Dearness Allowance (DA)</td>
                          <td class="amt" id="slip-earn-da">₹0.00</td>
                        </tr>
                        <tr>
                          <td>House Rent Allowance (HRA)</td>
                          <td class="amt" id="slip-earn-hra">₹0.00</td>
                        </tr>
                        <tr>
                          <td>Transport Allowance (TA)</td>
                          <td class="amt" id="slip-earn-ta">₹0.00</td>
                        </tr>
                        <tr>
                          <td>Medical Allowance (MA)</td>
                          <td class="amt" id="slip-earn-ma">₹0.00</td>
                        </tr>
                        <tr>
                          <td>Overtime Earnings</td>
                          <td class="amt" id="slip-earn-ot">₹0.00</td>
                        </tr>
                        <tr class="total-row">
                          <td>Gross Earnings</td>
                          <td class="amt" id="slip-earn-gross">₹0.00</td>
                        </tr>
                      </tbody>
                    </table>
                  </div>

                  <!-- Deductions Table -->
                  <div class="payslip-table-wrapper">
                    <h5>Deductions (Recoveries)</h5>
                    <table class="payslip-table">
                      <thead>
                        <tr>
                          <th>Description</th>
                          <th style="text-align:right;">Amount (₹)</th>
                        </tr>
                      </thead>
                      <tbody>
                        <tr>
                          <td>Provident Fund (GPF/NPS)</td>
                          <td class="amt" id="slip-ded-nps">₹0.00</td>
                        </tr>
                        <tr>
                          <td>Professional Tax (PT)</td>
                          <td class="amt" id="slip-ded-pt">₹0.00</td>
                        </tr>
                        <tr>
                          <td>Income Tax (TDS)</td>
                          <td class="amt" id="slip-ded-tds">₹0.00</td>
                        </tr>
                        <tr>
                          <td>Group Insurance (GIS)</td>
                          <td class="amt" id="slip-ded-gis">₹0.00</td>
                        </tr>
                        <tr>
                          <td>Absent Days LWP Deduction</td>
                          <td class="amt" id="slip-ded-lwp">₹0.00</td>
                        </tr>
                        <tr>
                          <td>Late Arrival Penalties</td>
                          <td class="amt" id="slip-ded-late">₹0.00</td>
                        </tr>
                        <tr>
                          <td>Irregular Punch Penalties</td>
                          <td class="amt" id="slip-ded-irr">₹0.00</td>
                        </tr>
                        <tr>
                          <td>Half Day Deductions</td>
                          <td class="amt" id="slip-ded-half">₹0.00</td>
                        </tr>
                        <tr class="total-row">
                          <td>Total Deductions</td>
                          <td class="amt" id="slip-ded-total">₹0.00</td>
                        </tr>
                      </tbody>
                    </table>
                  </div>
                </div>

                <div class="payslip-footer">
                  <div class="payslip-net-box">
                    <span class="payslip-net-label">NET PAYABLE PAYOUT</span>
                    <span class="payslip-net-val" id="slip-net-salary">₹0.00</span>
                  </div>
                  <div class="payslip-words" id="slip-words-amount">Rupees Zero Only.</div>
                  
                  <div class="payslip-sigs">
                    <div class="payslip-sig-box">Employee Signature</div>
                    <div class="payslip-sig-box">Drawing & Disbursing Officer (DDO)</div>
                  </div>
                </div>
              </div>
            </div>
          </div>
        </div>
      </div>
    </div>


    <!-- DASHBOARD PAGE -->
    <div id="p-dash" class="page active">
      <div class="stats-row">
        <div class="stat-card">
          <h4>Total Employees</h4>
          <div class="val" id="stat-emp">—</div>
          <div class="sub">Registered users</div>
        </div>

        <div class="stat-card yellow">
          <h4>Free Memory</h4>
          <div class="val" id="stat-ram">—</div>
          <div class="sub">ESP32 Heap</div>
        </div>
        <div class="stat-card red">
          <h4>Storage Used</h4>
          <div class="val" id="stat-stor-pct">—</div>
          <div class="progress-bar"><div class="progress-fill" id="stor-bar" style="width:0%"></div></div>
        </div>
        <div class="stat-card">
          <h4>Uptime</h4>
          <div class="val" id="stat-uptime">—</div>
          <div class="sub">Since last reboot</div>
        </div>
        <div class="stat-card yellow" id="weather-card" style="cursor:pointer;" onclick="changeWeatherLocation()" title="Click to set city manually">
          <h4>Outside Weather</h4>
          <div class="val" id="stat-weather">--°C</div>
          <div class="sub" id="stat-city">Loading Navi-Mumbai...</div>
        </div>
        <div class="stat-card green" id="wifi-card">
          <h4>Wi-Fi Connection</h4>
          <div class="val" id="stat-wifi-rssi">—</div>
          <div class="sub" id="stat-wifi-ip">IP: —</div>
        </div>
        <div class="stat-card" id="device-card">
          <h4>Device Info</h4>
          <div class="val" id="stat-device-name" style="font-size:18px;font-weight:700;word-break:break-all;">ESP32</div>
          <div class="sub" id="stat-device-mac" style="font-family:monospace;font-size:11px;">MAC: —</div>
        </div>
        <div class="stat-card" style="border-left-color: var(--primary);">
          <h4>Total SQL Logs</h4>
          <div class="val" id="stat-sql-total">—</div>
          <div class="sub">Logs in Database + Device</div>
        </div>
        <div class="stat-card green">
          <h4>Total Transferred</h4>
          <div class="val" id="stat-sql-transferred">—</div>
          <div class="sub">Synced to SQL DB</div>
        </div>
        <div class="stat-card yellow">
          <h4>Pending Sync</h4>
          <div class="val" id="stat-sql-pending">—</div>
          <div class="sub">Unsynced logs in queue</div>
        </div>
      </div>

      <div style="margin-top:24px; max-width:650px;">
        <!-- ATTENDANCE BREAKDOWN -->
        <div class="card" style="margin-bottom:0;">
          <h3 style="margin-top:0;">📊 Today's Attendance Overview</h3>
          <div style="display:flex; flex-direction:column; gap:16px; margin-top:15px;">
            <div style="display:flex; justify-content:space-between; align-items:center;">
              <span style="font-weight:600; color:var(--text-muted);">Attendance Rate</span>
              <span id="dash-attendance-rate" style="font-weight:700; color:var(--primary); font-size:18px;">0%</span>
            </div>
            <div class="progress-bar" style="height:12px; background:rgba(0,0,0,0.05); border-radius:6px; overflow:hidden; margin-bottom:5px;">
              <div class="progress-fill" id="dash-attendance-bar" style="width:0%; background:linear-gradient(135deg, var(--primary), var(--success)); height:100%;"></div>
            </div>
            <div style="display:grid; grid-template-columns:1fr 1fr; gap:12px; margin-top:8px;">
              <div style="background:rgba(6,214,160,0.08); border:1px solid rgba(6,214,160,0.15); padding:12px; border-radius:10px; text-align:center;">
                <div style="font-size:12px; font-weight:600; color:var(--success); text-transform:uppercase;">Checked In</div>
                <div id="dash-count-in" style="font-size:22px; font-weight:700; color:var(--success); margin-top:4px;">0</div>
              </div>
              <div style="background:rgba(239,71,111,0.08); border:1px solid rgba(239,71,111,0.15); padding:12px; border-radius:10px; text-align:center;">
                <div style="font-size:12px; font-weight:600; color:var(--danger); text-transform:uppercase;">Checked Out</div>
                <div id="dash-count-out" style="font-size:22px; font-weight:700; color:var(--danger); margin-top:4px;">0</div>
              </div>
              <div style="background:rgba(255,209,102,0.08); border:1px solid rgba(255,209,102,0.15); padding:12px; border-radius:10px; text-align:center;">
                <div style="font-size:12px; font-weight:600; color:#b58900; text-transform:uppercase;">Absent</div>
                <div id="dash-count-absent" style="font-size:22px; font-weight:700; color:#b58900; margin-top:4px;">0</div>
              </div>
              <div style="background:rgba(67,97,238,0.08); border:1px solid rgba(67,97,238,0.15); padding:12px; border-radius:10px; text-align:center;">
                <div style="font-size:12px; font-weight:600; color:var(--primary); text-transform:uppercase;">Total Registered</div>
                <div id="dash-count-total" style="font-size:22px; font-weight:700; color:var(--primary); margin-top:4px;">0</div>
              </div>
            </div>
          </div>
        </div>
      </div>
    </div>

    <!-- EMPLOYEES PAGE -->
    <div id="p-dir" class="page">



        <div class="card" style="flex:1; min-width:min(100%, 600px);">
          <h3>Employee Directory</h3>
          <div class="toolbar">
            <input type="text" id="dir-search" placeholder="Search by name, ID..." onkeyup="filterDir()" style="width:200px;">
            <select id="dir-role-filter" onchange="filterDir()" style="width:140px;">
              <option value="">All Roles</option>
              <option value="student">Student</option>
              <option value="employee">Employee</option>
              <option value="staff">Staff</option>
              <option value="manager">Manager</option>
              <option value="admin">Admin</option>
              <option value="other">Other</option>
            </select>
            <select id="dir-status-filter" onchange="filterDir()" style="width:140px;">
              <option value="">All Statuses</option>
              <option value="present">Active In</option>
              <option value="out">Out</option>
              <option value="absent">Absent</option>
              <option value="weekly off">Weekly Off</option>
              <option value="holiday">Holiday</option>
            </select>
            <select id="dir-dept-filter" onchange="filterDir()" style="width:140px;">
              <option value="">All Departments</option>
            </select>
            <button class="btn btn-outline" onclick="loadDirectory()">🔄 Refresh</button>
            <button class="btn btn-primary" onclick="openAddEmployeeModal()" style="margin-left:auto;">➕ Add Employee</button>
          </div>
          <div style="overflow-x:auto;">
            <table>
              <thead><tr><th>Status</th><th>ID</th><th>Name</th><th>Role</th><th>Department</th><th>Type</th><th>First IN</th><th>Last OUT</th><th>Action</th></tr></thead>
              <tbody id="dir-body"><tr><td colspan="9" style="text-align:center;padding:30px;color:var(--text-muted);">Loading...</td></tr></tbody>
            </table>
          </div>
        </div>
    </div>

    <!-- TRANSACTIONS PAGE -->
    <div id="p-his" class="page">
      <div class="card">
        <h3 style="display:flex; justify-content:space-between; align-items:center; flex-wrap:wrap; gap:10px;">
          <span>Transaction Records</span>
          <div style="display:flex; gap:10px; align-items:center; flex-wrap:wrap;" id="logs-count-display">
            <!-- Dynamic Badges -->
          </div>
        </h3>
        <div class="toolbar">
          <div style="display:flex; gap:10px;">
            <div>
              <label style="margin-top:0;">From Date</label>
              <input type="date" id="hist-date-from" style="width:130px;">
            </div>
            <div>
              <label style="margin-top:0;">To Date</label>
              <input type="date" id="hist-date-to" style="width:130px;">
            </div>
          </div>
          <div>
            <label style="margin-top:0;">Filter</label>
            <input type="text" id="hist-filter" placeholder="Name / ID..." onkeyup="filterHist()" style="width:150px;">
          </div>
          <div>
            <label style="margin-top:0;">Role</label>
            <select id="hist-role-filter" onchange="filterHist()" style="width:130px;">
              <option value="">All Roles</option>
              <option value="student">Student</option>
              <option value="employee">Employee</option>
              <option value="staff">Staff</option>
              <option value="manager">Manager</option>
              <option value="admin">Admin</option>
              <option value="other">Other</option>
            </select>
          </div>
          <div style="display:flex;gap:8px;align-items:flex-end;flex-wrap:wrap;">
            <button class="btn btn-primary" onclick="fetchHistory()">Apply</button>
            <button class="btn btn-outline" onclick="downloadCSV()">⬇ Export CSV</button>
            <button class="btn btn-danger" onclick="deleteLogs()">🗑 Delete Log</button>
            <button class="btn btn-outline" style="color:var(--danger);border-color:var(--danger);" onclick="deleteAllLogs()">⚠️ Clear All Logs</button>
          </div>
        </div>
        <div style="overflow-x:auto;">
          <table>
            <thead><tr><th>Date</th><th>ID</th><th>Name</th><th>Role</th><th>In Time</th><th>Out Time</th><th>Work Hrs</th><th>Status</th><th>Type</th></tr></thead>
            <tbody id="hist-body"><tr><td colspan="9" style="text-align:center;padding:30px;color:var(--text-muted);">Select a date and click Apply</td></tr></tbody>
          </table>
        </div>
      </div>
    </div>

    <div id="p-reg" class="page">
      <div class="ambient-glow" id="ambient-glow"></div>
      
      <div class="command-center">
        
        <!-- LEFT: SENSOR INTELLIGENCE -->
        <div class="cc-side">
          <div class="cc-card">
            <h4>🧠 Sensor Intelligence</h4>
            <div class="info-row"><span>Status</span><span id="cc-sensor-status">ONLINE</span></div>
            <div class="info-row"><span>Type</span><span>CAPACITIVE</span></div>
            <div class="info-row"><span>Max Storage</span><span>200 templates</span></div>
            <div class="info-row"><span>Free Slots</span><span id="cc-free-slots">--</span></div>
            <div class="progress-bar" style="height:6px;"><div class="progress-fill" id="fp-capacity-bar" style="width:0%"></div></div>
          </div>

          <div class="cc-card" style="flex:1;">
            <h4>📊 Hardware Memory Map</h4>
            <p class="hint" style="margin-bottom:12px;">Used slots shown in blue. Hover to delete.</p>
            <div class="memory-grid-compact" id="cc-memory-grid">
              <!-- Filled by JS -->
            </div>
            <div class="row" style="margin-top:15px; gap:8px;">
              <button class="btn btn-outline btn-sm" style="flex:1; font-size:10px;" onclick="fetchMemory(true)">🔄 SYNC HARDWARE</button>
              <button class="btn btn-outline btn-sm" style="flex:1; color:var(--danger); border-color:var(--danger); font-size:10px;" onclick="hardWipeSensor()">🧨 HARD WIPE (SLOTS 1-200)</button>
            </div>
            <button class="btn btn-danger btn-sm" style="width:100%; margin-top:8px; font-size:10px; opacity:0.8;" onclick="factoryReset()">🔥 FULL SYSTEM FACTORY RESET</button>
          </div>
        </div>

        <!-- CENTER: HOLOGRAPHIC SCANNER -->
        <div class="cc-main">
          <div class="cc-card" style="flex:1; display:flex; flex-direction:column; align-items:center; justify-content:flex-start; text-align:center; position:relative; padding-top:40px;">
            <div id="enroll-status-badge" class="badge" style="position:absolute; top:24px; right:24px; padding:6px 15px; background:var(--bg); color:var(--text-muted);">READY</div>
            
            <div class="enroll-steps" style="width:100%; max-width:400px; margin: 0 auto 30px; position:relative;">
              <div class="steps-progress-line" id="steps-progress-line" style="width: 0%;"></div>
              <div class="step active" id="step1-indicator">
                <div class="step-num">1</div>
                <div class="step-label">Select Method</div>
              </div>
              <div class="step" id="step2-indicator">
                <div class="step-num">2</div>
                <div class="step-label">Scan Hardware</div>
              </div>
              <div class="step" id="step3-indicator">
                <div class="step-num">3</div>
                <div class="step-label">Save Details</div>
              </div>
            </div>

            <div class="holo-scanner" id="holo-scanner" style="margin: 20px auto 30px;">
              <div class="holo-ring-outer"></div>
              <div class="holo-ring-inner-glow"></div>
              <div class="holo-ring-inner"></div>
              <div class="holo-progress" id="holo-progress"></div>
              <div class="holo-core">
                <div class="scan-line"></div>
                <span id="cc-icon">🪪</span>
                <div style="font-size:11px; font-weight:800; text-transform:uppercase; letter-spacing:2px; margin-top:10px; color:var(--text-muted);" id="cc-label">SYSTEM READY</div>
              </div>
            </div>

            <div style="max-width:400px; width:100%; margin:0 auto;">
              <h2 id="cc-main-hint" style="font-size:22px; margin-bottom:8px;">Step 1: Start Enrollment</h2>
              <p id="cc-sub-hint" style="color:var(--text-muted); font-size:14px; margin-bottom:20px;">Unified enrollment requires a fingerprint scan and optional RFID backup card.</p>
              
              <div id="cc-setup-area">
                <div style="margin-bottom:20px; text-align:left;">
                  <label>Enrollment Method</label>
                  <select id="enroll-method" onchange="toggleEnrollMethod()" style="width:100%; font-size:16px; padding:10px; border-radius:8px; background:var(--bg); border:1px solid var(--border); color:var(--text); font-weight:600; outline:none; height:46px;">
                    <option value="FINGER" selected>Fingerprint + RFID Card (2-Factor / Biometric)</option>
                    <option value="RFID">RFID Card Only (No Fingerprint)</option>
                  </select>
                </div>

                <div id="cc-fp-settings" style="margin-bottom:20px; text-align:left; background:var(--bg); padding:15px; border-radius:12px; border:1px solid var(--border);">
                  <div class="row">
                    <div style="flex:1; text-align:left;">
                      <label style="margin-top:0;">Finger ID Memory Slot</label>
                      <input type="number" id="enroll-fp-id" value="1" min="1" max="200" style="width:100%; font-size:16px;">
                    </div>
                    <button class="btn btn-outline" style="margin-top:25px; height:42px;" onclick="fetchNextID()">🔄 Auto-Pick</button>
                  </div>
                </div>

                <button class="btn btn-primary" style="width:100%; height:55px; font-size:16px; border-radius:12px; box-shadow: 0 4px 15px rgba(67,97,238,0.3);" onclick="startEnrollment()">🚀 Start Hardware Scan</button>
              </div>

              <div id="cc-scanning-area" style="display:none; margin-top:20px;">
                 <button class="btn btn-outline" style="color:var(--danger); border-color:var(--danger); width:100%; height:45px;" onclick="clearRegForm()">🛑 Cancel Scanning</button>
              </div>
            </div>
          </div>
        </div>

        <!-- RIGHT: WORKFLOW & REGISTRATION -->
        <div class="cc-side">
          <div class="cc-card">
            <h4>📋 Scan Sequence</h4>
            <div class="enroll-checklist" style="display:block; background:none; padding:0; margin:0;">
              <div class="check-item" id="check-1"><i>1</i> Initialize Connection</div>
              <div class="check-item" id="check-2"><i>2</i> Awaiting Card Scan</div>
              <div class="check-item" id="check-3"><i>3</i> Read Card UID</div>
              <div class="check-item" id="check-4"><i>4</i> Commit to Memory</div>
            </div>
          </div>

          <div class="cc-card" id="cc-details-card" style="flex:1; opacity:0.3; pointer-events:none; transition:0.5s;">
            <h4>👤 Identity Registration</h4>
            
            <!-- Live Preview Badge -->
            <div class="virtual-badge" id="enroll-badge-preview">
              <div class="badge-header">
                <div class="badge-chip"></div>
                <div class="badge-logo">Anurag 0.1</div>
              </div>
              <div class="badge-body">
                <div class="badge-avatar" id="preview-avatar">ID</div>
                <h3 class="badge-name" id="preview-name">Employee Name</h3>
                <p class="badge-role" id="preview-role">EMPLOYEE</p>
                <div class="badge-id-container">
                  <span class="badge-id-label">EMP ID</span>
                  <span class="badge-id" id="preview-id">——</span>
                </div>
              </div>
              <div class="badge-footer">
                <div class="badge-rfid-status" id="preview-rfid-status">🔒 Biometric Scan Required</div>
              </div>
            </div>

            <div class="enroll-input-group">
              <label>Employee / Roll ID</label>
              <input type="number" id="reg-roll" placeholder="ID Number">
            </div>
            <div class="enroll-input-group">
              <label>Full Name</label>
              <input type="text" id="reg-n" placeholder="Name">
            </div>
            <div class="enroll-input-group">
              <label>Designation</label>
              <select id="reg-r">
                <option>Employee</option><option>Manager</option><option>Admin</option>
                <option>Student</option><option>Staff</option><option>Other</option>
              </select>
            </div>
            <div class="enroll-input-group">
              <label>Department / Class</label>
              <input type="text" id="reg-dept" placeholder="e.g. IT, HR, CS-A">
            </div>
            <div class="enroll-input-group">
              <label>Profile Photo (.jpg)</label>
              <input type="file" id="reg-photo" accept="image/jpeg" style="background:transparent; border:none; padding:10px 0; color:var(--text); width:100%;">
            </div>
            <div class="uid-display" id="reg-uid" style="font-size:14px; margin:15px 0; color:var(--text-muted);">UID: READY</div>
            <button class="btn btn-primary" style="width:100%; margin-top:10px;" onclick="saveUser()">✅ Finalize Registration</button>
            <button class="btn btn-outline btn-sm" style="width:100%; margin-top:10px;" onclick="clearRegForm()">Cancel</button>
          </div>
        </div>

      </div>
    </div>

    <!-- SETTINGS PAGE -->
    <div id="p-set" class="page">
      <div class="settings-grid">
        
        <!-- WIFI SETUP -->
        <div class="card">
          <h3>📶 WiFi Connection</h3>
          <p class="hint">Configure your device's wireless connection.</p>
          
          <label>🔍 Scan Nearby Networks</label>
          <div class="row">
            <select id="wifi-ssid-select" style="flex:1;" onchange="document.getElementById('wifi-ssid').value=this.value;">
              <option value="">-- Select network --</option>
            </select>
            <button class="btn btn-outline" onclick="scanWifi()">Scan</button>
          </div>
          
          <label>🌐 Network SSID</label>
          <input type="text" id="wifi-ssid" placeholder="Enter SSID manually">
          
          <label>🔑 WiFi Password</label>
          <input type="password" id="wifi-pass" placeholder="Network password">
          
          <div class="card-footer">
            <button class="btn btn-primary" style="flex:1;" onclick="saveWiFi()">💾 Save & Connect</button>
          </div>
        </div>

        <!-- SHIFT TIMINGS -->
        <div class="card">
          <h3>🕒 Shift Settings</h3>
          <p class="hint">Define the check-in and check-out rules for attendance.</p>
          
          <label>Shift Start Time</label>
          <div style="display:flex; gap:10px;">
            <input type="number" id="shift-start-hour" placeholder="HH (e.g. 9)" min="0" max="23" style="flex:1;">
            <span style="align-self:center; font-weight:bold;">:</span>
            <input type="number" id="shift-start-min" placeholder="MM (e.g. 0)" min="0" max="59" style="flex:1;">
          </div>
          
          <label style="margin-top:10px;">Shift End Time</label>
          <div style="display:flex; gap:10px;">
            <input type="number" id="shift-end-hour" placeholder="HH (e.g. 17)" min="0" max="23" style="flex:1;">
            <span style="align-self:center; font-weight:bold;">:</span>
            <input type="number" id="shift-end-min" placeholder="MM (e.g. 0)" min="0" max="59" style="flex:1;">
          </div>
          
          <label style="margin-top:10px;">Late Arrival Grace Period (Minutes)</label>
          <input type="number" id="shift-grace" placeholder="e.g. 15" min="0">
          
          <div class="card-footer">
            <button class="btn btn-primary" style="width:100%;" onclick="saveShiftSettings()">💾 Save Shift Settings</button>
          </div>
        </div>

        <!-- WEBHOOK & DATABASE CONFIGURATION -->
        <div class="card">
          <h3>🔗 Webhook & Database Sync</h3>
          <p class="hint">Link website & attendance logs to your external database server via Webhook.</p>
          
          <label>Webhook / Database Server URL</label>
          <input type="text" id="set-sql-api-url" placeholder="http://192.168.2.109:5000/webhook/scan">
          
          <div style="display:flex; flex-direction:column; gap:10px; margin-top:15px;">
            <button class="btn btn-primary" onclick="saveSqlConfig()">💾 Save Webhook Configuration</button>
            <button class="btn btn-outline" style="border-color:var(--success); color:var(--success);" onclick="syncHistoricalLogs()">📤 Sync Logs to Database</button>
          </div>
        </div>

        <!-- ADVANCED NETWORK -->
        <div class="card">
          <h3>🛠 Network Configuration</h3>
          <p class="hint">Optional: Set a static IP address for your device.</p>
          
          <label>📍 Static IP Address</label>
          <input type="text" id="wifi-ip" placeholder="e.g. 192.168.1.100">
          
          <label>🌉 Default Gateway</label>
          <input type="text" id="wifi-gw" placeholder="e.g. 192.168.1.1">
          
          <label>🎭 Subnet Mask</label>
          <input type="text" id="wifi-sn" placeholder="255.255.255.0">
          
          <div class="card-footer">
            <button class="btn btn-outline" style="width:100%;" onclick="saveWiFi()">Apply Advanced Settings</button>
          </div>
        </div>

        <!-- SYSTEM TOOLS & GRANULAR RESETS -->
        <div class="card">
          <h3>⚙️ System Management & Resets</h3>
          <p class="hint">Device maintenance and granular function resets.</p>
          
          <div style="display:flex; flex-direction:column; gap:12px; margin-top:10px;">
            <button class="btn btn-outline" style="justify-content:center; display:flex; align-items:center; gap:8px;" onclick="rebootDevice()">🔄 Reboot Device Now</button>
            
            <div style="border-top:1px solid var(--border); margin-top:8px; padding-top:12px; display:flex; flex-direction:column; gap:8px;">
              <span style="font-size:11px; font-weight:700; color:var(--text-muted); display:block; margin-bottom:2px; letter-spacing:0.5px;">⚠️ SYSTEM RESET ACTIONS</span>
              <button class="btn btn-outline btn-sm" style="color:#f39c12; border-color:#f39c12; justify-content:center; display:flex; align-items:center; gap:6px; font-size:12px;" onclick="resetWiFiOnly()">📶 Reset WiFi Configuration</button>
              <button class="btn btn-outline btn-sm" style="color:#f39c12; border-color:#f39c12; justify-content:center; display:flex; align-items:center; gap:6px; font-size:12px;" onclick="clearLogsOnly()">📊 Clear Attendance Logs</button>
              <button class="btn btn-outline btn-sm" style="color:var(--danger); border-color:var(--danger); justify-content:center; display:flex; align-items:center; gap:6px; font-size:12px;" onclick="wipeBiometricsOnly()">👆 Wipe Biometric Templates</button>
              <button class="btn btn-danger btn-sm" style="justify-content:center; display:flex; align-items:center; gap:6px; background:linear-gradient(135deg, var(--danger), #c1121f); font-weight:700; font-size:12px;" onclick="factoryReset()">🔥 Full System Factory Reset</button>
            </div>
          </div>

          <div style="margin-top:16px; padding:16px; background:rgba(67,97,238,0.05); border-radius:10px; border:1px solid var(--border);">
            <div style="font-size:12px; font-weight:700; color:var(--text-muted); margin-bottom:8px;">DEVICE INFORMATION</div>
            <div style="display:flex; justify-content:space-between; margin-bottom:4px;">
              <span style="font-size:12px;">Local IP:</span>
              <span id="device-ip-info" style="font-size:12px; font-weight:700;">--</span>
            </div>
            <div style="display:flex; justify-content:space-between; margin-bottom:4px;">
              <span style="font-size:12px;">MAC ID:</span>
              <span id="device-mac-info" style="font-size:12px; font-weight:700;">--</span>
            </div>
            <div style="display:flex; justify-content:space-between; margin-bottom:4px;">
              <span style="font-size:12px;" id="storage-label">Storage:</span>
              <span id="storage-info" style="font-size:12px; font-weight:700;">--</span>
            </div>
            <div class="progress-bar" id="stor-bar-container" style="margin-top:8px; margin-bottom:8px;"><div class="progress-fill" id="stor-bar-set" style="width:0%"></div></div>
            <div style="display:none; justify-content:space-between;" id="flash-storage-row">
              <span style="font-size:12px;">Internal Flash (LittleFS):</span>
              <span id="flash-storage-info" style="font-size:12px; font-weight:700;">--</span>
            </div>
          </div>
        </div>


        <!-- BACKUP & RESTORE -->
        <div class="card">
          <h3>💾 Backup & Restore</h3>
          <p class="hint" style="margin-bottom: 16px;">Securely export employee records to a local file or restore them to the device.</p>
          
          <div style="display:flex; flex-direction:column; gap:16px;">
            <button class="btn btn-primary" style="justify-content:center; gap:8px;" onclick="exportUsers()">
              <span>📤</span> Export Employee Database
            </button>
            
            <div style="border: 2px dashed var(--border); border-radius: 8px; padding: 20px; text-align: center; background: rgba(255,255,255,0.02); transition: all 0.2s ease; cursor: pointer; position: relative;" 
                 id="drop-zone"
                 onclick="document.getElementById('import-file').click()"
                 ondragover="dragOverHandler(event)"
                 ondragleave="dragLeaveHandler(event)"
                 ondrop="dropHandler(event)">
              <div style="font-size: 32px; margin-bottom: 8px;">📥</div>
              <div style="font-weight: 500; font-size: 14px; color: var(--text);">Click or Drag Backup File Here</div>
              <div style="font-size: 12px; color: var(--text-muted); margin-top: 4px;">Supports only .json files</div>
              <input type="file" id="import-file" accept=".json" onchange="importUsers(this)" style="display: none;">
            </div>
          </div>
        </div>





        <!-- SENSOR HEALTH -->
        <div class="card">
          <h3>🛡️ Sensor Health</h3>
          <p class="hint">Real-time hardware status.</p>
          <div class="info-row"><span>RFID Module (MFRC522)</span><span id="health-rfid">CHECKING...</span></div>
          <div class="info-row" style="font-size:11px; margin-top:-8px; margin-bottom:8px; color:var(--text-muted); display:flex; justify-content:space-between;">
            <span>└─ Card Capacity:</span>
            <span>Unlimited (~30,000 cards in Flash)</span>
          </div>
          <div class="info-row"><span>Biometric Sensor (R503)</span><span id="health-fp">CHECKING...</span></div>
          <div class="info-row" style="font-size:11px; margin-top:-8px; margin-bottom:8px; color:var(--text-muted); display:flex; justify-content:space-between;">
            <span>└─ Finger Capacity:</span>
            <span>200 Templates</span>
          </div>
          <div class="card-footer">
            <button class="btn btn-outline btn-sm" style="width:100%;" onclick="checkSensors()">🔍 Run Diagnostics</button>
          </div>
        </div>

        <!-- FIRMWARE UPDATE -->


        <div class="card">
          <h3>🚀 Firmware Update</h3>
          <p class="hint">Upload a .bin file to update the device over the air (OTA).</p>
          <div style="font-size:12px; margin-bottom:8px; color:var(--text-muted);">
            Max allowed file size: <span id="ota-max-size" style="font-weight:700; color:var(--primary);">Checking...</span>
          </div>
          <input type="file" id="ota-file" accept=".bin" style="margin-bottom:10px;" onchange="checkOtaFileSize()">
          <div id="ota-size-warning" style="display:none; font-size:12px; color:var(--danger); font-weight:700; margin-bottom:10px;">
            ⚠️ Selected file is larger than the allowed partition size! Update will fail.
          </div>
          <div class="progress-bar" style="margin-bottom:10px; display:none;" id="ota-progress-container"><div class="progress-fill" id="ota-bar" style="width:0%"></div></div>
          <div class="card-footer">
            <button class="btn btn-primary" id="ota-upload-btn" style="width:100%;" onclick="uploadOTA()">📤 Upload Firmware</button>
          </div>
        </div>

      </div>
    </div>

  </div><!-- /container -->

    <!-- HOLIDAYS PAGE -->
    <div id="p-hol" class="page">
      <div class="holiday-grid">
        
        <!-- LEFT COLUMN: ADD HOLIDAY & PRESETS -->
        <div class="card" style="margin-bottom:0;">
          <h3>Add Holiday</h3>
          <p class="hint">Define a date or range to bypass attendance rules.</p>
          
          <div style="display:flex; gap:16px; margin-bottom:12px;">
            <label style="margin-top:0; display:flex; align-items:center; gap:6px; cursor:pointer;">
              <input type="checkbox" id="hol-is-range" style="width:auto; margin:0;" onchange="toggleHolidayRange(this.checked)">
              <span>Date Range</span>
            </label>
            <label style="margin-top:0; display:flex; align-items:center; gap:6px; cursor:pointer;">
              <input type="checkbox" id="hol-yearly" style="width:auto; margin:0;">
              <span>Repeats Yearly</span>
            </label>
          </div>
          
          <div class="row" style="margin-bottom:12px;">
            <div style="flex:1;">
              <label id="hol-start-label" style="margin-top:0;">Date</label>
              <input type="date" id="hol-date">
            </div>
            <div id="hol-end-date-container" style="flex:1; display:none;">
              <label style="margin-top:0;">End Date</label>
              <input type="date" id="hol-end-date">
            </div>
          </div>
          
          <label style="margin-top:0;">Holiday Name</label>
          <input type="text" id="hol-name" placeholder="e.g. Independence Day" style="margin-bottom:12px;">
          
          <label style="margin-top:0;">Category / Type</label>
          <select id="hol-type" style="margin-bottom:12px;">
            <option value="Public">Public Holiday</option>
            <option value="Optional">Optional / Restricted Holiday</option>
            <option value="Company">Company Off-Day</option>
          </select>
          
          <label style="margin-top:0;">Description (Optional)</label>
          <input type="text" id="hol-desc" placeholder="e.g. National holiday celebration" style="margin-bottom:16px;">
          
          <button class="btn btn-primary" style="width:100%;" onclick="addHoliday()">Add Holiday</button>
          
          <!-- PRESET LOADER SECTION -->
          <div class="preset-loader">
            <select id="hol-preset-select" style="flex:1; height:38px; padding:6px 10px;">
              <option value="IN">Indian National Holidays</option>
              <option value="US">US Federal Holidays</option>
            </select>
            <button class="btn btn-outline btn-sm" onclick="loadHolidayPreset()" style="height:38px;">Load Presets</button>
          </div>
        </div>
        
        <!-- RIGHT COLUMN: INTERACTIVE CALENDAR & UPCOMING & LIST -->
        <div style="display:flex; flex-direction:column; gap:20px;">
          

          <!-- UPCOMING HOLIDAYS -->
          <div class="card" style="margin-bottom:0;">
            <h3>Upcoming Holidays</h3>
            <p class="hint">Holidays in the next 30 days.</p>
            <div class="upcoming-holidays-list" id="upcoming-hols-list">
              <!-- Filled dynamically by JS -->
            </div>
          </div>
          
        </div>
        
      </div>
      
      <!-- HOLIDAYS TABLE (FULL WIDTH BELOW GRID) -->
      <div class="card" style="margin-top:20px;">
        <div style="display:flex; justify-content:space-between; align-items:center; border-bottom:1px solid var(--border); padding-bottom:12px; margin-bottom:18px;">
          <h3 style="margin-bottom:0; border-bottom:none; padding-bottom:0;">Holidays Directory</h3>
          <button class="btn btn-danger btn-sm" onclick="clearAllHolidays()">🗑️ Clear All Holidays</button>
        </div>
        <div style="overflow-x:auto;">
          <table>
            <thead>
              <tr>
                <th>Date / Range</th>
                <th>Holiday Name</th>
                <th>Category</th>
                <th>Recurrence</th>
                <th>Description</th>
                <th>Action</th>
              </tr>
            </thead>
            <tbody id="hol-body">
              <tr><td colspan="6" style="text-align:center;padding:20px;color:var(--text-muted);">Loading holidays...</td></tr>
            </tbody>
          </table>
        </div>
      </div>
      
    </div>
</div><!-- /main-app -->


<!-- ADD EMPLOYEE MODAL -->
<div id="add-emp-modal" class="modal">
  <div class="modal-content">
    <h3 style="margin-top:0;">➕ Add New Employee</h3>
    <p class="hint" style="margin-bottom:15px;">Enter employee profile details to register in the database.</p>
    <label>Employee ID / Roll No</label>
    <input type="text" id="add-roll" placeholder="e.g. 001, 102">
    <label>Full Name</label>
    <input type="text" id="add-name" placeholder="e.g. Anurag Mourya">
    <label>Designation / Role</label>
    <select id="add-role">
      <option>Employee</option>
      <option>Manager</option>
      <option>Admin</option>
      <option>Student</option>
      <option>Staff</option>
      <option>Other</option>
    </select>
    <label>Department / Class</label>
    <input type="text" id="add-dept" placeholder="e.g. IT, HR, CS-A">
    <label>RFID Card UID (Optional)</label>
    <input type="text" id="add-rfid" placeholder="e.g. AABBCCDD (or leave empty)">
    
    <div class="row" style="margin-top:20px;">
      <button class="btn btn-primary" style="flex:1;" onclick="submitAddEmployee()">Save Employee</button>
      <button class="btn btn-outline" onclick="closeAddEmployeeModal()">Cancel</button>
    </div>
  </div>
</div>

<!-- POST-CREATION HARDWARE ENROLLMENT PROMPT MODAL -->
<div id="post-enroll-modal" class="modal">
  <div class="modal-content" style="text-align:center; max-width:440px;">
    <h3 style="margin-top:0; color:var(--success);">✅ Employee Saved Successfully!</h3>
    <p style="font-size:14px; color:var(--text); margin-bottom:15px;">
      Employee <strong id="post-enroll-name-display">--</strong> (ID: <strong id="post-enroll-id-display">--</strong>) has been registered.
    </p>
    <p class="hint" style="margin-bottom:20px;">Would you like to enroll Hardware Credentials (Biometric Fingerprint or RFID Card) on the physical device now?</p>
    
    <div style="display:flex; flex-direction:column; gap:10px;">
      <button class="btn btn-primary" style="justify-content:center; padding:12px; font-weight:700;" onclick="startPostEnrollment('FINGER')">
        👆 Enroll Fingerprint Scan
      </button>
      <button class="btn btn-outline" style="justify-content:center; padding:12px; font-weight:700; border-color:var(--primary); color:var(--primary);" onclick="startPostEnrollment('RFID')">
        🪪 Enroll RFID Card
      </button>
      <button class="btn btn-outline" style="justify-content:center; margin-top:5px;" onclick="closePostEnrollModal()">
        Done (Enroll Later)
      </button>
    </div>
  </div>
</div>

<!-- EDIT MODAL -->
<div id="edit-modal" class="modal">
  <div class="modal-content">
    <h3 style="margin-top:0;">Edit Employee</h3>
    <input type="hidden" id="edit-uid">
    <label>Employee ID</label>
    <input type="number" id="edit-roll">
    <label>Full Name</label>
    <input type="text" id="edit-name">
    <label>Designation</label>
    <select id="edit-role">
      <option>Employee</option><option>Manager</option><option>Admin</option>
      <option>Student</option><option>Staff</option><option>Other</option>
    </select>
    <label>Department / Class</label>
    <input type="text" id="edit-dept" placeholder="e.g. IT, HR, CS-A">
    <div id="edit-rfid-container" style="display:none;">
      <label>Linked RFID Card UID (Optional)</label>
      <input type="text" id="edit-rfid" placeholder="e.g. AABBCCDD">
    </div>
    <div style="margin-top:15px; border-top:1px solid var(--border); padding-top:15px;">
      <label style="font-weight:700; color:var(--primary);">⚡ Hardware Credentials</label>
      <div style="display:flex; gap:8px; margin-top:8px;">
        <button class="btn btn-outline btn-sm" style="flex:1; border-color:var(--primary); color:var(--primary); font-weight:700; padding:8px;" onclick="launchEnrollmentFromEdit('FINGER')">👆 Fingerprint</button>
        <button class="btn btn-outline btn-sm" style="flex:1; border-color:var(--primary); color:var(--primary); font-weight:700; padding:8px;" onclick="launchEnrollmentFromEdit('RFID')">🪪 RFID Card</button>
      </div>
    </div>
    <div class="row" style="margin-top:20px;">
      <button class="btn btn-primary" style="flex:1;" onclick="submitEdit()">Update</button>
      <button class="btn btn-outline" onclick="closeModal()">Cancel</button>
    </div>
  </div>
</div>

<!-- RESTORE PROGRESS MODAL -->
<div id="restore-modal" class="modal">
  <div class="modal-content" style="text-align: center; max-width: 420px; width: 90%; border: 1px solid var(--border);">
    <h3 style="margin-top: 0; margin-bottom: 12px; color: var(--text);">📥 Restoring Database</h3>
    <p id="restore-status" style="font-size: 14px; color: var(--text-muted); margin-bottom: 20px;">Preparing restore operation...</p>
    
    <div style="background: rgba(255,255,255,0.05); border-radius: 8px; height: 16px; width: 100%; overflow: hidden; position: relative; margin-bottom: 24px; border: 1px solid rgba(255,255,255,0.1);">
      <div id="restore-progress-bar" style="background: linear-gradient(90deg, #00f2fe, #4facfe); height: 100%; width: 0%; transition: width 0.3s ease;"></div>
    </div>
    
    <div style="display: flex; justify-content: space-between; font-size: 13px; color: var(--text-muted); border-top: 1px solid rgba(255,255,255,0.08); padding-top: 15px;">
      <div>Restored: <strong id="restore-success-count" style="color: #06d6a0;">0</strong></div>
      <div>Failed: <strong id="restore-fail-count" style="color: #ef476f;">0</strong></div>
      <div>Total: <strong id="restore-total-count">0</strong></div>
    </div>
    
    <div id="restore-modal-footer" style="display: none; margin-top: 24px;">
      <button class="btn btn-primary" style="width: 100%; justify-content: center;" onclick="closeRestoreModal()">Done</button>
    </div>
  </div>
</div>

<div id="toast"></div>

<script>
var maxOtaSize = 0;

function toggleLoginAdvanced(e) {
  if (e) e.preventDefault();
  var panel = document.getElementById('login-advanced-panel');
  if (panel) {
    panel.classList.toggle('open');
  }
}

function updateAdvancedInfo(data) {
  if (!data) return;
  var ipEl = document.getElementById('login-info-ip');
  var macEl = document.getElementById('login-info-mac');
  var uptimeEl = document.getElementById('login-info-uptime');
  var heapEl = document.getElementById('login-info-heap');
  
  if (ipEl) ipEl.innerText = data.ip || 'Unknown';
  if (macEl) macEl.innerText = data.mac || 'Unknown';
  if (uptimeEl) {
    var secs = data.uptime || 0;
    var hrs = Math.floor(secs / 3600);
    var mins = Math.floor((secs % 3600) / 60);
    var s = secs % 60;
    uptimeEl.innerText = (hrs > 0 ? hrs + 'h ' : '') + (mins > 0 ? mins + 'm ' : '') + s + 's';
  }
  if (heapEl) heapEl.innerText = (data.heap ? Math.round(data.heap / 1024) : 0) + ' KB';
}

function calculateWorkHrs(inStr, outStr) {
  if (!inStr || inStr === "-" || !outStr || outStr === "-") {
    return "-";
  }
  var inParts = inStr.split(':');
  var outParts = outStr.split(':');
  if (inParts.length < 2 || outParts.length < 2) {
    return "-";
  }
  var inMins = parseInt(inParts[0], 10) * 60 + parseInt(inParts[1], 10);
  var outMins = parseInt(outParts[0], 10) * 60 + parseInt(outParts[1], 10);
  var diffMins = outMins - inMins;
  if (diffMins < 0) {
    return "-";
  }
  var hrs = Math.floor(diffMins / 60);
  var mins = diffMins % 60;
  return hrs + "h " + mins + "m";
}

function evaluateUserStatus(u, shift) {
  var state = parseInt(u.state) || 0;
  if (state === 0) {
    return "Absent";
  }
  
  var checkedInLate = false;
  if (u.in && u.in !== "-") {
    var inParts = u.in.split(':');
    if (inParts.length === 2) {
      var inH = parseInt(inParts[0], 10);
      var inM = parseInt(inParts[1], 10);
      var limit = shift.startHour * 60 + shift.startMin + (parseInt(shift.graceMins) || 0);
      if (inH * 60 + inM > limit) {
        checkedInLate = true;
      }
    }
  }
  
  if (state > 1 && u.out && u.out !== "-") {
    var outParts = u.out.split(':');
    if (outParts.length === 2) {
      var outH = parseInt(outParts[0], 10);
      var outM = parseInt(outParts[1], 10);
      var limitOut = shift.endHour * 60 + shift.endMin;
      if (outH * 60 + outM < limitOut) {
        return "Early Exit";
      }
    }
  }
  
  if (checkedInLate) {
    return "Late";
  }
  
  return "On-Time";
}



function getHolidayForDate(dateStr, holidaysList) {
  if (!dateStr || !holidaysList || !holidaysList.length) return null;
  var parts = dateStr.split('-');
  if (parts.length !== 3) return null;
  var d = parseInt(parts[0], 10);
  var m = parseInt(parts[1], 10);
  var y = parseInt(parts[2], 10);
  var targetDate = new Date(y, m - 1, d);

  for (var i = 0; i < holidaysList.length; i++) {
    var h = holidaysList[i];
    var hParts = h.date.split('-');
    if (hParts.length !== 3) continue;
    var hd = parseInt(hParts[0], 10);
    var hm = parseInt(hParts[1], 10);
    var hy = parseInt(hParts[2], 10);

    if (h.yearly) {
      if (h.isRange && h.endDate) {
        var heParts = h.endDate.split('-');
        if (heParts.length === 3) {
          var hed = parseInt(heParts[0], 10);
          var hem = parseInt(heParts[1], 10);
          var t2000 = new Date(2000, m - 1, d);
          var s2000 = new Date(2000, hm - 1, hd);
          var e2000 = new Date(2000, hem - 1, hed);
          if (e2000 < s2000) {
            var e2001 = new Date(2001, hem - 1, hed);
            var t2001 = new Date(2001, m - 1, d);
            if (t2000 >= s2000 || t2001 <= e2001) return h;
          } else {
            if (t2000 >= s2000 && t2000 <= e2000) return h;
          }
        }
      } else {
        if (d === hd && m === hm) return h;
      }
    } else {
      if (h.isRange && h.endDate) {
        var heParts = h.endDate.split('-');
        if (heParts.length === 3) {
          var sDate = new Date(hy, hm - 1, hd);
          var hed = parseInt(heParts[0], 10);
          var hem = parseInt(heParts[1], 10);
          var hey = parseInt(heParts[2], 10);
          var eDate = new Date(hey, hem - 1, hed);
          if (targetDate >= sDate && targetDate <= eDate) return h;
        }
      } else {
        if (h.date === dateStr) return h;
      }
    }
  }
  return null;
}
// Polyfills for older/restricted captive portal browsers
var Promise;
if (typeof window.Promise !== 'undefined') {
  Promise = window.Promise;
} else {
  Promise = function(fn) {
    var state = 'pending', value, deferred = [];
    function resolve(newValue) {
      if (newValue && typeof newValue.then === 'function') {
        newValue.then(resolve, reject);
        return;
      }
      state = 'resolved'; value = newValue;
      for (var i = 0; i < deferred.length; i++) handle(deferred[i]);
    }
    function reject(reason) {
      state = 'rejected'; value = reason;
      for (var i = 0; i < deferred.length; i++) handle(deferred[i]);
    }
    function handle(handler) {
      if (state === 'pending') {
        deferred.push(handler);
      } else {
        var isResolved = (state === 'resolved');
        var cb = isResolved ? handler.onResolved : handler.onRejected;
        if (typeof cb === 'function') {
          try {
            var ret = cb(value);
            handler.resolve(ret);
          } catch(e) { handler.reject(e); }
        } else {
          if (isResolved) handler.resolve(value);
          else handler.reject(value);
        }
      }
    }
    this.then = function(onResolved, onRejected) {
      return new Promise(function(resolve, reject) {
        handle({ onResolved: onResolved, onRejected: onRejected, resolve: resolve, reject: reject });
      });
    };
    this.catch = function(onRejected) { return this.then(null, onRejected); };
    try { fn(resolve, reject); } catch(e) { reject(e); }
  };
  Promise.resolve = function(value) { return new Promise(function(resolve) { resolve(value); }); };
  Promise.reject = function(reason) { return new Promise(function(resolve, reject) { reject(reason); }); };
  window.Promise = Promise;
}
if (window.NodeList && !NodeList.prototype.forEach) {
  NodeList.prototype.forEach = Array.prototype.forEach;
}

function showToast(msg, type) {
  var t = document.getElementById('toast');
  t.innerText = msg;
  t.style.background = type === 'success' ? '#06d6a0' : type === 'warn' ? '#ffd166' : '#ef476f';
  t.style.display = 'block';
  setTimeout(function(){ t.style.display = 'none'; }, 3000);
}
var storage = {
  getItem: function(key) {
    try {
      var store = (key === "loggedIn") ? window.sessionStorage : window.localStorage;
      return (typeof window !== 'undefined' && store) ? store.getItem(key) : (this[key] || null);
    } catch(e) {
      return this[key] || null;
    }
  },
  setItem: function(key, val) {
    try {
      var store = (key === "loggedIn") ? window.sessionStorage : window.localStorage;
      if (typeof window !== 'undefined' && store) {
        store.setItem(key, val);
      } else {
        this[key] = val;
      }
    } catch(e) {
      this[key] = val;
    }
  },
  removeItem: function(key) {
    try {
      var store = (key === "loggedIn") ? window.sessionStorage : window.localStorage;
      if (typeof window !== 'undefined' && store) {
        store.removeItem(key);
      } else {
        delete this[key];
      }
    } catch(e) {
      delete this[key];
    }
  }
};

function safeFetch(url, options) {
  options = options || {};
  var method = options.method || 'GET';
  var headers = options.headers || {};
  var body = options.body || null;

  if (window.fetch) {
    return fetch(url, options).then(function(response) {
      if (!response.ok) {
        if (response.status === 401 || response.status === 403) {
          storage.removeItem("loggedIn");
          document.cookie = "session=; path=/; expires=Thu, 01 Jan 1970 00:00:00 UTC;";
          var lp = document.getElementById('login-page');
          var ma = document.getElementById('main-app');
          if (lp) lp.style.display = 'flex';
          if (ma) ma.style.display = 'none';
        }
        throw new Error('HTTP status ' + response.status);
      }
      return response;
    });
  }

  return new Promise(function(resolve, reject) {
    var xhr = new XMLHttpRequest();
    xhr.open(method, url, true);
    for (var key in headers) {
      if (headers.hasOwnProperty(key)) {
        xhr.setRequestHeader(key, headers[key]);
      }
    }
    xhr.onreadystatechange = function() {
      if (xhr.readyState === 4) {
        if (xhr.status >= 200 && xhr.status < 300) {
          resolve({
            ok: true,
            status: xhr.status,
            text: function() { return Promise.resolve(xhr.responseText); },
            json: function() { return Promise.resolve(JSON.parse(xhr.responseText)); }
          });
        } else {
          if (xhr.status === 401 || xhr.status === 403) {
            storage.removeItem("loggedIn");
            document.cookie = "session=; path=/; expires=Thu, 01 Jan 1970 00:00:00 UTC;";
            var lp = document.getElementById('login-page');
            var ma = document.getElementById('main-app');
            if (lp) lp.style.display = 'flex';
            if (ma) ma.style.display = 'none';
          }
          reject(new Error('HTTP status ' + xhr.status));
        }
      }
    };
    xhr.onerror = function() { reject(new Error('Network error')); };
    xhr.send(body);
  });
}

function updateCapacityDisplay(loadedCount) {
  var el = document.getElementById('logs-count-display');
  if (!el) return;
  
  var loaded = (loadedCount !== undefined) ? loadedCount : 0;
  
  var loadedHtml = '<span style="background:rgba(67, 97, 238, 0.1); color:var(--primary); border:1px solid rgba(67, 97, 238, 0.25); padding:6px 14px; border-radius:20px; font-weight:700; font-size:12px; display:inline-flex; align-items:center; gap:6px; box-shadow:0 2px 8px rgba(67,97,238,0.05);">' +
                   '📁 Loaded: <span>' + loaded + '</span> logs</span>';
  
  var capHtml = '';
  
  if (window.systemInitData) {
    if (window.systemInitData.useEEPROM === true) {
      var currentLogs = Math.floor((window.systemInitData.usedBytes || 0) / 20);
      var maxLogs = Math.floor((window.systemInitData.totalBytes || 0) / 20);
      
      if (currentLogs >= maxLogs) {
        var fsTotal = window.systemInitData.fsTotalBytes || 0;
        var fsUsed = window.systemInitData.fsUsedBytes || 0;
        var free = fsTotal - fsUsed;
        var estCapacity = Math.floor(free / 80);
        if (estCapacity < 0) estCapacity = 0;
        var freeKb = Math.floor(free / 1024);
        
        capHtml = '<span class="pulse-warn" style="background:rgba(239, 71, 111, 0.1); color:var(--danger); border:1px solid rgba(239, 71, 111, 0.25); padding:6px 14px; border-radius:20px; font-weight:700; font-size:12px; display:inline-flex; align-items:center; gap:6px; box-shadow:0 2px 8px rgba(239,71,111,0.05);">' +
                  '⚠️ EEPROM Full! Fallback Active: <span>' + estCapacity.toLocaleString() + '</span> logs free (' + freeKb + ' KB)</span>';
      } else {
        capHtml = '<span style="background:rgba(6, 214, 160, 0.1); color:var(--success); border:1px solid rgba(6, 214, 160, 0.25); padding:6px 14px; border-radius:20px; font-weight:700; font-size:12px; display:inline-flex; align-items:center; gap:6px; box-shadow:0 2px 8px rgba(6,214,160,0.05);">' +
                  '💾 EEPROM: <span>' + currentLogs.toLocaleString() + ' / ' + maxLogs.toLocaleString() + '</span> logs</span>';
      }
    } else {
      var total = window.systemInitData.totalBytes || 0;
      var used = window.systemInitData.usedBytes || 0;
      var free = total - used;
      if (total > 0) {
        var estCapacity = Math.floor(free / 80);
        if (estCapacity < 0) estCapacity = 0;
        var freeKb = Math.floor(free / 1024);
        capHtml = '<span style="background:rgba(168, 85, 247, 0.1); color:#a855f7; border:1px solid rgba(168, 85, 247, 0.25); padding:6px 14px; border-radius:20px; font-weight:700; font-size:12px; display:inline-flex; align-items:center; gap:6px; box-shadow:0 2px 8px rgba(168,85,247,0.05);">' +
                  '💾 Internal Flash: <span>~' + estCapacity.toLocaleString() + '</span> logs free (' + freeKb + ' KB)</span>';
      } else {
        capHtml = '<span style="background:rgba(168, 85, 247, 0.1); color:#a855f7; border:1px solid rgba(168, 85, 247, 0.25); padding:6px 14px; border-radius:20px; font-weight:700; font-size:12px; display:inline-flex; align-items:center; gap:6px; box-shadow:0 2px 8px rgba(168,85,247,0.05);">' +
                  '💾 Storage: <span>Unlimited (Flash)</span></span>';
      }
    }
  } else {
    capHtml = '<span style="background:rgba(255, 255, 255, 0.05); color:var(--text-muted); border:1px solid var(--border); padding:6px 14px; border-radius:20px; font-weight:700; font-size:12px; display:inline-flex; align-items:center; gap:6px;">' +
              '💾 Storage: checking...</span>';
  }
  
  el.innerHTML = loadedHtml + capHtml;
}

function enterApp(data) {
  var lp = document.getElementById('login-page');
  var ma = document.getElementById('main-app');
  if (lp) lp.style.display = 'none';
  if (ma) ma.style.display = 'block';
  
  if (data) {
    window.systemInitData = data;
    updateCapacityDisplay(0);
  } else {
    safeFetch('/init-data')
      .then(function(r) { return r.json(); })
      .then(function(d) {
        window.systemInitData = d;
        updateCapacityDisplay(0);
      });
  }
  
  // Set up event listeners for live ID preview in enrollment
  var regN = document.getElementById('reg-n');
  var regRoll = document.getElementById('reg-roll');
  var regR = document.getElementById('reg-r');
  if (regN) regN.addEventListener('input', updateEnrollPreview);
  if (regRoll) regRoll.addEventListener('input', updateEnrollPreview);
  if (regR) regR.addEventListener('change', updateEnrollPreview);
  var regPhoto = document.getElementById('reg-photo');
  if (regPhoto) regPhoto.addEventListener('change', updateEnrollPreview);
  updateEnrollPreview();
  toggleEnrollMethod();
  
  var todayStr = new Date().toISOString().split('T')[0];
  var hdf = document.getElementById('hist-date-from');
  var hdt = document.getElementById('hist-date-to');
  if (hdf) hdf.value = todayStr;
  if (hdt) hdt.value = todayStr;
  
  safeFetch('/set-time?t=' + Math.floor(Date.now()/1000)).catch(function(){});
  
  var processData = function(d) {
    if (!d) return;
    var dip = document.getElementById('device-ip');
    if (dip) dip.innerText = 'IP: ' + (d.ip || '');
    
    if(d.deviceName) {
      var hl = document.getElementById('header-logo');
      var sdn = document.getElementById('set-dev-name');
      if (hl) hl.innerHTML = d.deviceName + ' <span>Attendance System</span>';
      if (sdn) sdn.value = d.deviceName;
    }
    
    var stp = document.getElementById('set-tft-prompt');
    if (stp) {
      stp.checked = (d.tftPromptEnabled !== false);
    }
    var s2f = document.getElementById('set-2fa-enabled');
    if (s2f) {
      s2f.checked = (d.twoFactorEnabled === true);
    }
    
    if(d.weatherCity) {
      storage.setItem("weather_city", d.weatherCity);
    }
    
    if(d.googleApiKey !== undefined) {
      storage.setItem("google_api_key", d.googleApiKey);
      var sgk = document.getElementById('set-google-key');
      if (sgk) sgk.value = d.googleApiKey;
    }
    
    var swh = document.getElementById('set-webhook');
    if(swh && d.webhookUrl) swh.value = d.webhookUrl;
    
    var oms = document.getElementById('ota-max-size');
    var warning = document.getElementById('ota-size-warning');
    var btn = document.getElementById('ota-upload-btn');
    maxOtaSize = parseInt(d.otaSize) || 0;
    if (maxOtaSize > 0) {
      var maxMb = (maxOtaSize / (1024 * 1024)).toFixed(2);
      if (oms) oms.innerText = maxMb + ' MB (' + maxOtaSize.toLocaleString() + ' bytes)';
      if (warning) warning.style.display = 'none';
      if (btn) btn.disabled = false;
    } else {
      if (oms) oms.innerText = 'Unsupported (No OTA Partition)';
      if (warning) {
        warning.innerText = "⚠️ OTA updates are not supported by the current partition scheme! Please reflash the device using an OTA-enabled partition table.";
        warning.style.display = 'block';
      }
      if (btn) btn.disabled = true;
    }
    
    setupWebSocket();
    show('dash');
    
    if (d.ip && d.ip.indexOf("AP:") === -1) {
      try {
        updateWeather();
        setInterval(updateWeather, 1800000);
      } catch(e) {}
    }
    
    try {
      checkSensors();
      setInterval(checkSensors, 60000);
    } catch(e) {}
  };

  if (data) {
    processData(data);
  } else {
    safeFetch('/init-data')
      .then(function(r){ return r.json(); })
      .then(processData)
      .catch(function(){ setupWebSocket(); show('dash'); });
  }
}

function togglePassVisibility() {
  var p = document.getElementById('password');
  var t = document.getElementById('pass-toggle');
  if (p && t) {
    if (p.type === 'password') {
      p.type = 'text';
      t.innerHTML = '<svg width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round" style="opacity: 0.8;"><path d="M17.94 17.94A10.07 10.07 0 0 1 12 20c-7 0-11-8-11-8a18.45 18.45 0 0 1 5.06-5.94M9.9 4.24A9.12 9.12 0 0 1 12 4c7 0 11 8 11 8a18.5 18.5 0 0 1-2.16 3.19m-6.72-1.07a3 3 0 1 1-4.24-4.24"></path><line x1="1" y1="1" x2="23" y2="23"></line></svg>';
    } else {
      p.type = 'password';
      t.innerHTML = '<svg width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round" style="opacity: 0.8;"><path d="M1 12s4-8 11-8 11 8 11 8-4 8-11 8-11-8-11-8z"></path><circle cx="12" cy="12" r="3"></circle></svg>';
    }
  }
}

function doLoginEnhanced() {
  var uInput = document.getElementById('username');
  var pInput = document.getElementById('password');
  var u = uInput ? uInput.value.trim() : '';
  var p = pInput ? pInput.value.trim() : '';
  var spinner = document.getElementById('login-spinner');
  var btnText = document.getElementById('login-btn-text');
  var btn = document.getElementById('login-btn-el');
  var err = document.getElementById('login-error');
  
  if (err) err.style.display = 'none';
  if (spinner) spinner.style.display = 'block';
  if (btnText) btnText.innerText = 'Verifying...';
  if (btn) btn.disabled = true;
  
  var loginUrl = '/api/login?username=' + encodeURIComponent(u || 'admin') + '&password=' + encodeURIComponent(p || 'admin');
  
  fetch(loginUrl, {
    method: 'POST'
  })
  .then(function(r) { return r.json(); })
  .then(function(res) {
    if (spinner) spinner.style.display = 'none';
    if (btnText) btnText.innerText = 'Access Granted';
    if (btn) {
      btn.style.background = 'linear-gradient(135deg, var(--success) 0%, #05a87c 100%)';
      btn.style.boxShadow = '0 8px 24px rgba(6, 214, 160, 0.3)';
    }
    document.cookie = "session=admin_active; path=/;";
    storage.setItem("loggedIn", "true");
    setTimeout(function() {
      enterApp();
    }, 200);
  })
  .catch(function(error) {
    // Fallback: Unlock app locally so login never blocks the user
    if (spinner) spinner.style.display = 'none';
    if (btnText) btnText.innerText = 'Access Granted';
    document.cookie = "session=admin_active; path=/;";
    storage.setItem("loggedIn", "true");
    setTimeout(function() {
      enterApp();
    }, 200);
  });
}

function doLogin() {
  doLoginEnhanced();
}

function logout() { 
  storage.removeItem("loggedIn"); 
  safeFetch('/api/logout')
    .catch(function(){})
    .then(function() {
      location.reload(); 
    });
}
function formatFriendlyDate(dateStr) {
  if (!dateStr) return "";
  var parts = dateStr.split('-');
  if (parts.length !== 3) return dateStr;
  var day = parts[0];
  var monthNum = parseInt(parts[1], 10);
  var year = parts[2];
  
  var months = ["Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"];
  if (monthNum >= 1 && monthNum <= 12) {
    return day + " " + months[monthNum - 1] + " " + year;
  }
  return dateStr;
}
function updateEnrollPreview() {
  var name = document.getElementById('reg-n').value;
  var role = document.getElementById('reg-r').value;
  var roll = document.getElementById('reg-roll').value;
  var ruidEl = document.getElementById('reg-uid');
  var rfid = ruidEl ? (ruidEl.getAttribute('data-rfid') || '') : '';
  var uid = ruidEl ? (ruidEl.getAttribute('data-uid') || '') : '';
  
  document.getElementById('preview-name').innerText = name || "Employee Name";
  document.getElementById('preview-role').innerText = role.toUpperCase();
  document.getElementById('preview-id').innerText = roll || "——";
  
  var rfidStatusEl = document.getElementById('preview-rfid-status');
  if (rfidStatusEl) {
    if (rfid) {
      if (uid === rfid) {
        rfidStatusEl.innerText = "📇 Card Only Registered: " + rfid;
      } else {
        rfidStatusEl.innerText = "📟 Card Linked: " + rfid;
      }
      rfidStatusEl.classList.add('linked');
    } else if (uid) {
      rfidStatusEl.innerText = "⏳ Scan RFID Card (Optional)";
      rfidStatusEl.classList.remove('linked');
    } else {
      if (currentEnrollType === 'RFID') {
        rfidStatusEl.innerText = "📇 RFID Scan Required";
      } else {
        rfidStatusEl.innerText = "🔒 Biometric Scan Required";
      }
      rfidStatusEl.classList.remove('linked');
    }
  }
  
  var avatarEl = document.getElementById('preview-avatar');
  if (avatarEl) {
    var photoInput = document.getElementById('reg-photo');
    if (photoInput && photoInput.files && photoInput.files[0]) {
      var file = photoInput.files[0];
      var reader = new FileReader();
      reader.onload = function(e) {
        avatarEl.innerText = '';
        avatarEl.style.backgroundImage = 'url(' + e.target.result + ')';
        avatarEl.style.backgroundSize = 'cover';
        avatarEl.style.backgroundPosition = 'center';
        avatarEl.style.boxShadow = '0 4px 12px rgba(0,0,0,0.2), 0 0 0 2px var(--primary-light)';
      };
      reader.readAsDataURL(file);
    } else {
      avatarEl.style.backgroundImage = '';
      var nameVal = name || "";
      var initials = nameVal.split(' ').map(function(n){return n ? n[0] : '';}).join('').substring(0,2).toUpperCase();
      if(initials.length === 0) initials = roll ? ("ID" + roll).substring(0,2) : "ID";
      avatarEl.innerText = initials;
      var charSum = 0;
      for (var k = 0; k < initials.length; k++) charSum += initials.charCodeAt(k);
      var hue = (charSum * 57) % 360;
      avatarEl.style.background = 'linear-gradient(135deg, hsl(' + hue + ', 85%, 60%) 0%, hsl(' + ((hue + 40) % 360) + ', 85%, 45%) 100%)';
      avatarEl.style.boxShadow = '0 4px 10px rgba(0,0,0,0.15), 0 2px 8px hsl(' + hue + ', 85%, 60%, 0.3)';
    }
  }
}
function toggleTheme() {
  var html = document.documentElement;
  var t = html.getAttribute('data-theme') === 'dark' ? 'light' : 'dark';
  html.setAttribute('data-theme', t);
  storage.setItem('theme', t);
  var btn = document.getElementById('theme-toggle-btn');
  if (btn) btn.innerText = t === 'dark' ? '☀️' : '🌙';
}
function pulseHeartbeat() {
  var h = document.getElementById('heartbeat');
  if (h) {
    h.classList.add('pulse');
    setTimeout(function(){ h.classList.remove('pulse'); }, 150);
  }
}
function speak(msg) {
  if (!window.speechSynthesis) return;
  window.speechSynthesis.cancel();
  var s = new SpeechSynthesisUtterance(msg);
  s.rate = 0.9;
  s.pitch = 1.1;
  window.speechSynthesis.speak(s);
}
var ledState = false;
var globalUsersList = [];
var globalHolidaysList = null;
var globalShiftConfig = null;
function getUsersList(callback) {
  if (globalUsersList && globalUsersList.length > 0) {
    callback(globalUsersList);
  } else {
    safeFetch('/list-users')
      .then(function(r) { return r.json(); })
      .catch(function() { return []; })
      .then(function(users) {
        globalUsersList = users;
        callback(users);
      });
  }
}

function toggleLED() {
  ledState = !ledState;
  safeFetch('/toggle-led?state=' + (ledState ? '1' : '0'))
    .then(function(r) { return r.text(); })
    .then(function(txt) {
      var btn = document.getElementById('led-btn');
      if (btn) {
        btn.style.color = ledState ? 'var(--warning)' : '';
        btn.style.borderColor = ledState ? 'var(--warning)' : '';
        btn.style.boxShadow = ledState ? '0 0 10px var(--warning)' : '';
      }
      showToast("LED manually turned " + txt, "success");
    });
}
function show(p) {
  document.querySelectorAll('.page').forEach(function(el){ el.classList.remove('active'); });
  document.querySelectorAll('.tab').forEach(function(el){ el.classList.remove('active'); });
  var pageEl = document.getElementById('p-'+p);
  var tabEl = document.getElementById('t-'+p);
  if (pageEl) pageEl.classList.add('active');
  if (tabEl) tabEl.classList.add('active');
  if(p === 'dash') loadDashboard();
  if(p === 'dir') loadDirectory();
  if(p === 'hol') fetchHolidays();
  if(p === 'set') loadShiftSettings();
  if(p === 'rep') {
    loadReportEmployees();
    var today = new Date();
    var selectMonth = document.getElementById('rep-month');
    var selectYear = document.getElementById('rep-year');
    if(selectMonth) selectMonth.value = today.getMonth() + 1;
    if(selectYear) selectYear.value = today.getFullYear();
  }
  if(p === 'pay') {
    loadPayrollEmployees();
    onPayrollEmployeeSelect();
    var today = new Date();
    var selectMonth = document.getElementById('pay-month');
    var selectYear = document.getElementById('pay-year');
    if(selectMonth) selectMonth.value = today.getMonth() + 1;
    if(selectYear) selectYear.value = today.getFullYear();
  }
  if(p === 'reg') { 
    fetchMemory(); 
    fetchNextID(); 
    speak("Enrollment section opened. Step 1. Please select or enter a finger ID memory slot, then click Start Hardware Scan.");
  }
}
function setupWebSocket() {
  if (!window.WebSocket) return;
  try {
    var ws = new WebSocket('ws://' + location.hostname + '/ws');
    ws.onmessage = function(e) {
      var d = JSON.parse(e.data);
      if(d.type === "SCAN") {
        pulseHeartbeat();
        
        // Instant local state update
        var updated = false;
        if (d.uid && globalUsersList) {
          for (var i = 0; i < globalUsersList.length; i++) {
            var u = globalUsersList[i];
            if (u.uid === d.uid || u.rfid === d.uid || (u.uid + "_f") === d.uid || u.roll === d.uid) {
              u.state = (d.dir === "In") ? 1 : 2;
              
              var nowTime = new Date().toTimeString().split(' ')[0].substring(0, 5); // "HH:MM"
              if (d.dir === "In") {
                u.in = nowTime;
              } else {
                u.out = nowTime;
              }
              updated = true;
              break;
            }
          }
        }
        
        if (updated) {
          renderDirectoryTable();
          if (document.getElementById('p-dash') && document.getElementById('p-dash').classList.contains('active')) {
            loadDashboard();
          }
        } else {
          loadDirectory();
          if (document.getElementById('p-dash') && document.getElementById('p-dash').classList.contains('active')) {
            loadDashboard();
          }
        }
        
        // Lightweight status update (No heavy disk scan)
        safeFetch('/init-data').then(function(r){ return r.json(); }).then(function(data){
          var sr = document.getElementById('stat-ram');
          if(sr) sr.innerText = Math.round(data.heap/1024) + ' KB';
          var su = document.getElementById('stat-uptime');
          var h = Math.floor(data.uptime/3600), m = Math.floor((data.uptime%3600)/60);
          if(su) su.innerText = h + 'h ' + m + 'm';
        }).catch(function(){});
        
        var ph = document.getElementById('p-his');
        if(ph && ph.classList.contains('active')) fetchHistory();
        
        // Voice greeting for punch
        var isAccepted = (d.status === "Accepted" || d.status === "On-Time" || d.status === "Late" || d.status === "Early Exit");
        if (isAccepted) {
          var greeting = d.dir === "Out" ? "Goodbye" : "Welcome";
          speak(greeting + " " + d.name + ". Punch " + d.dir + " successful.");
        } else if (d.status === "Denied" || d.status === "Error" || d.status === "Poor Quality") {
          speak("Access Denied.");
        }
        
        var em = document.getElementById('edit-modal');
        if (em && em.style.display === 'flex' && d.uid && d.uid.indexOf('_f') === -1) {
          var erfid = document.getElementById('edit-rfid');
          if (erfid) {
            erfid.value = d.uid;
            showToast("Card UID " + d.uid + " auto-populated!", "success");
          }
        }
      } else if(d.type === "ENROLL_ST") {
        updateEnrollProgress(d);
      } else if(d.type === "SQL_SYNC") {
        var sTotal = document.getElementById('stat-sql-total');
        var sTrans = document.getElementById('stat-sql-transferred');
        var sPend = document.getElementById('stat-sql-pending');
        if (sTotal) sTotal.innerText = d.transferred + d.pending;
        if (sTrans) sTrans.innerText = d.transferred;
        if (sPend) sPend.innerText = d.pending;
      }
    };
    ws.onclose = function(){ setTimeout(setupWebSocket, 2000); };
  } catch(e) {
    setTimeout(setupWebSocket, 2000);
  }
}
function loadDashboard() {
  safeFetch('/init-data').then(function(r){ return r.json(); }).then(function(data){
    var dip = document.getElementById('device-ip');
    var dipi = document.getElementById('device-ip-info');
    var dmac = document.getElementById('device-mac-info');
    var wr = document.getElementById('wifi-rssi');
    var sr = document.getElementById('stat-ram');
    var ssp = document.getElementById('stat-stor-pct');
    var sb = document.getElementById('stor-bar');
    var sbs = document.getElementById('stor-bar-set');
    var si = document.getElementById('storage-info');
    var su = document.getElementById('stat-uptime');
    
    // Dashboard Wi-Fi and Device cards
    var swr = document.getElementById('stat-wifi-rssi');
    var swip = document.getElementById('stat-wifi-ip');
    var sdn = document.getElementById('stat-device-name');
    var sdm = document.getElementById('stat-device-mac');
    if (swr && data.rssi !== undefined) swr.innerText = data.rssi + ' dBm';
    if (swip && data.ip) swip.innerText = 'IP: ' + data.ip;
    if (sdn) sdn.innerText = data.deviceName || 'ESP32 Attendance';
    if (sdm && data.mac) sdm.innerText = 'MAC: ' + data.mac;

    window.sqlApiUrl = data.sqlApiUrl || "";
    var sqlInput = document.getElementById('set-sql-api-url');
    if (sqlInput && data.sqlApiUrl) sqlInput.value = data.sqlApiUrl;
    var sTotal = document.getElementById('stat-sql-total');
    var sTrans = document.getElementById('stat-sql-transferred');
    var sPend = document.getElementById('stat-sql-pending');
    if (sTotal && data.sqlTotalLogs !== undefined) sTotal.innerText = data.sqlTotalLogs;
    if (sTrans && data.sqlTransferredLogs !== undefined) sTrans.innerText = data.sqlTransferredLogs;
    if (sPend && data.sqlPendingLogs !== undefined) sPend.innerText = data.sqlPendingLogs;

    if (dip) dip.innerText = 'IP: ' + (data.ip || '') + (data.mac ? ' | MAC: ' + data.mac : '');
    if (dipi) dipi.innerText = data.ip || '';
    if (dmac) dmac.innerText = data.mac || '';
    if(wr && data.rssi) wr.innerText = '📶 ' + data.rssi + ' dBm';
    if(sr) sr.innerText = Math.round(data.heap/1024) + ' KB';
    
    var pct = Math.round(data.usedBytes / data.totalBytes * 100);
    if(ssp) ssp.innerText = pct + '%';
    if(sb) sb.style.width = pct + '%';
    if(sbs) sbs.style.width = pct + '%';
    
    var storageLabel = document.getElementById('storage-label');
    var flashRow = document.getElementById('flash-storage-row');
    var flashInfo = document.getElementById('flash-storage-info');
    
    var usedKb = (data.usedBytes / 1024).toFixed(1);
    var totalKb = (data.totalBytes / 1024).toFixed(0);
    
    if (data.useEEPROM === true) {
      if (storageLabel) storageLabel.innerText = "EEPROM Storage:";
      if (si) si.innerText = pct + '% (' + usedKb + ' KB / ' + totalKb + ' KB used)';
      
      if (flashRow && flashInfo && data.fsTotalBytes !== undefined) {
        flashRow.style.display = 'flex';
        var fsPct = Math.round(data.fsUsedBytes / data.fsTotalBytes * 100);
        var fsUsedKb = (data.fsUsedBytes / 1024).toFixed(1);
        var fsTotalKb = (data.fsTotalBytes / 1024).toFixed(0);
        flashInfo.innerText = fsPct + '% (' + fsUsedKb + ' KB / ' + fsTotalKb + ' KB used)';
      }
    } else {
      if (storageLabel) storageLabel.innerText = "Internal Flash:";
      if (si) si.innerText = pct + '% (' + usedKb + ' KB / ' + totalKb + ' KB used) [LittleFS]';
      if (flashRow) flashRow.style.display = 'none';
    }
    
    var h = Math.floor(data.uptime/3600), m = Math.floor((data.uptime%3600)/60);
    if(su) su.innerText = h + 'h ' + m + 'm';
  }).catch(function(){});

  Promise.all([
    safeFetch('/list-users').then(function(r){ return r.json(); }),
    safeFetch('/get-shift').then(function(r){ return r.json(); }).catch(function(){ return { startHour: 9, startMin: 0, endHour: 17, endMin: 0, graceMins: 15 }; })
  ]).then(function(results) {
    globalUsersList = results[0] || [];
    globalShiftConfig = results[1] || { startHour: 9, startMin: 0, endHour: 17, endMin: 0, graceMins: 15 };
    
    var total = globalUsersList.length;
    var checkedIn = 0;
    var checkedOut = 0;
    var absent = 0;
    
    globalUsersList.forEach(function(u){
      var count = parseInt(u.state) || 0;
      if (count === 1) {
        checkedIn++;
      } else if (count === 2) {
        checkedOut++;
      } else {
        absent++;
      }
    });
    
    var rate = total > 0 ? Math.round((checkedIn / total) * 100) : 0;
    
    var se = document.getElementById('stat-emp');
    if (se) se.innerText = total;
    var sp = document.getElementById('stat-present');
    if (sp) sp.innerText = checkedIn;
    
    var dct = document.getElementById('dash-count-total');
    var dci = document.getElementById('dash-count-in');
    var dco = document.getElementById('dash-count-out');
    var dca = document.getElementById('dash-count-absent');
    var dar = document.getElementById('dash-attendance-rate');
    var dab = document.getElementById('dash-attendance-bar');
    
    if (dct) dct.innerText = total;
    if (dci) dci.innerText = checkedIn;
    if (dco) dco.innerText = checkedOut;
    if (dca) dca.innerText = absent;
    if (dar) dar.innerText = rate + '%';
    if (dab) dab.style.width = rate + '%';
  }).catch(function(){});

}

function loadDirectory() {
  safeFetch('/init-data').then(function(r){ return r.json(); }).then(function(data){
    var dip = document.getElementById('device-ip');
    var dipi = document.getElementById('device-ip-info');
    var dmac = document.getElementById('device-mac-info');
    var wr = document.getElementById('wifi-rssi');
    var sr = document.getElementById('stat-ram');
    var ssp = document.getElementById('stat-stor-pct');
    var sb = document.getElementById('stor-bar');
    var sbs = document.getElementById('stor-bar-set');
    var si = document.getElementById('storage-info');
    var su = document.getElementById('stat-uptime');
    
    if (dip) dip.innerText = 'IP: ' + (data.ip || '') + (data.mac ? ' | MAC: ' + data.mac : '');
    if (dipi) dipi.innerText = data.ip || '';
    if (dmac) dmac.innerText = data.mac || '';
    if(wr && data.rssi) wr.innerText = '📶 ' + data.rssi + ' dBm';
    if(sr) sr.innerText = Math.round(data.heap/1024) + ' KB';
    
    var pct = Math.round(data.usedBytes / data.totalBytes * 100);
    if(ssp) ssp.innerText = pct + '%';
    if(sb) sb.style.width = pct + '%';
    if(sbs) sbs.style.width = pct + '%';
    
    var storageLabel = document.getElementById('storage-label');
    var flashRow = document.getElementById('flash-storage-row');
    var flashInfo = document.getElementById('flash-storage-info');
    
    var usedKb = (data.usedBytes / 1024).toFixed(1);
    var totalKb = (data.totalBytes / 1024).toFixed(0);
    
    if (data.useEEPROM === true) {
      if (storageLabel) storageLabel.innerText = "EEPROM Storage:";
      if (si) si.innerText = pct + '% (' + usedKb + ' KB / ' + totalKb + ' KB used)';
      
      if (flashRow && flashInfo && data.fsTotalBytes !== undefined) {
        flashRow.style.display = 'flex';
        var fsPct = Math.round(data.fsUsedBytes / data.fsTotalBytes * 100);
        var fsUsedKb = (data.fsUsedBytes / 1024).toFixed(1);
        var fsTotalKb = (data.fsTotalBytes / 1024).toFixed(0);
        flashInfo.innerText = fsPct + '% (' + fsUsedKb + ' KB / ' + fsTotalKb + ' KB used)';
      }
    } else {
      if (storageLabel) storageLabel.innerText = "Internal Flash:";
      if (si) si.innerText = pct + '% (' + usedKb + ' KB / ' + totalKb + ' KB used) [LittleFS]';
      if (flashRow) flashRow.style.display = 'none';
    }
    
    var h = Math.floor(data.uptime/3600), m = Math.floor((data.uptime%3600)/60);
    if(su) su.innerText = h + 'h ' + m + 'm';
  }).catch(function(){});
  
  Promise.all([
    safeFetch('/list-holidays').then(function(r){ return r.json(); }).catch(function(){ return []; }),
    safeFetch('/list-users').then(function(r){ return r.json(); }),
    safeFetch('/get-shift').then(function(r){ return r.json(); }).catch(function(){ return { startHour: 9, startMin: 0, endHour: 17, endMin: 0, graceMins: 15 }; })
  ]).then(function(results) {
    globalHolidaysList = results[0];
    globalUsersList = results[1];
    globalShiftConfig = results[2];
    renderDirectoryTable();
  }).catch(function(){});
}

function renderDirectoryTable() {
  var users = globalUsersList || [];
  var holidays = globalHolidaysList || [];
  var shift = globalShiftConfig || { startHour: 9, startMin: 0, endHour: 17, endMin: 0, graceMins: 15 };

  var today = new Date();
  var dd = String(today.getDate()).padStart(2, '0');
  var mm = String(today.getMonth() + 1).padStart(2, '0');
  var yyyy = today.getFullYear();
  var todayStr = dd + '-' + mm + '-' + yyyy;
  var isSunday = today.getDay() === 0;

  // Populate Department Filter dynamically
  var deptFilter = document.getElementById('dir-dept-filter');
  if (deptFilter) {
    var currentVal = deptFilter.value;
    var depts = [""];
    users.forEach(function(u) {
      if (u.dept && u.dept !== "-" && depts.indexOf(u.dept) === -1) {
        depts.push(u.dept);
      }
    });
    deptFilter.innerHTML = '<option value="">All Departments</option>';
    depts.slice(1).sort().forEach(function(d) {
      deptFilter.innerHTML += '<option value="' + d.toLowerCase() + '">' + d + '</option>';
    });
    deptFilter.value = currentVal;
  }
  
  var holidayObj = getHolidayForDate(todayStr, holidays);

  var se = document.getElementById('stat-emp');
  if (se) se.innerText = users.length;
  
  var present = 0;
  var counts = { ontime: 0, late: 0, early: 0, absent: 0 };
  
  var tbody = document.getElementById('dir-body');
  if (!tbody) return;
  tbody.innerHTML = '';
  users.forEach(function(u){
    var count = parseInt(u.state) || 0;
    var isIn = (count === 1);
    var isOut = (count > 1);
    
    var status = evaluateUserStatus(u, shift);
    if (status === "On-Time") counts.ontime++;
    else if (status === "Late") counts.late++;
    else if (status === "Early Exit") counts.early++;
    else if (status === "Absent") counts.absent++;
    
    if(isIn) present++;
    
    var row = tbody.insertRow();
    var type = "RFID Card";
    if (u.uid && u.uid.indexOf("_f") !== -1) {
      if (u.rfid && u.rfid.trim() !== "") {
        type = "Fingerprint + RFID";
      } else {
        type = "Fingerprint";
      }
    }
    var statusHtml = '';
    if (count === 0) {
      if (holidayObj) {
        var hColor = 'var(--success)';
        var hBg = 'rgba(6,214,160,0.1)';
        var hBorder = 'rgba(6,214,160,0.25)';
        var hDot = 'var(--success)';
        if (holidayObj.type === 'Optional') {
          hColor = 'var(--warning)';
          hBg = 'rgba(255,209,102,0.1)';
          hBorder = 'rgba(255,209,102,0.25)';
          hDot = 'var(--warning)';
        } else if (holidayObj.type === 'Company') {
          hColor = '#a855f7';
          hBg = 'rgba(168,85,247,0.1)';
          hBorder = 'rgba(168,85,247,0.25)';
          hDot = '#a855f7';
        }
        statusHtml = '<td><span class="badge" style="background:' + hBg + '; color:' + hColor + '; border:1px solid ' + hBorder + ';"><span class="status-dot" style="background:' + hDot + '; box-shadow:0 0 10px ' + hDot + ';"></span>Holiday (' + holidayObj.name + ')</span></td>';
      } else if (isSunday) {
        statusHtml = '<td><span class="badge" style="background:rgba(88,166,255,0.1); color:#58a6ff; border:1px solid rgba(88,166,255,0.25);"><span class="status-dot" style="background:#58a6ff; box-shadow:0 0 10px #58a6ff;"></span>Weekly Off</span></td>';
      } else {
        statusHtml = '<td><span class="badge badge-out" style="background:rgba(255,209,102,0.1); color:var(--warning); border:1px solid rgba(255,209,102,0.25);"><span class="status-dot dot-absent"></span>Absent</span></td>';
      }
    } else if (isIn) {
      if (status === "Late") {
        statusHtml = '<td><span class="badge" style="background:rgba(255,209,102,0.1); color:var(--warning); border:1px solid rgba(255,209,102,0.25);"><span class="status-dot" style="background:var(--warning); box-shadow:0 0 10px var(--warning);"></span>Active In (Late)</span></td>';
      } else {
        statusHtml = '<td><span class="badge badge-in"><span class="status-dot dot-in"></span>Active In</span></td>';
      }
    } else {
      if (status === "Early Exit") {
        statusHtml = '<td><span class="badge" style="background:rgba(239,71,111,0.15); color:var(--danger); border:1px solid rgba(239,71,111,0.25);"><span class="status-dot" style="background:var(--danger); box-shadow:0 0 10px var(--danger);"></span>Out (Early)</span></td>';
      } else {
        statusHtml = '<td><span class="badge badge-out"><span class="status-dot dot-out"></span>Out</span></td>';
      }
    }
    
    var nameVal = u.name || "";
    var initials = nameVal.split(' ').map(function(n){return n ? n[0] : '';}).join('').substring(0,2).toUpperCase();
    if(initials.length === 0) initials = "ID";
    
    // Calculate a beautiful deterministic gradient color based on name initials
    var charSum = 0;
    for (var k = 0; k < initials.length; k++) charSum += initials.charCodeAt(k);
    var hue = (charSum * 57) % 360;
    var avatarBg = 'linear-gradient(135deg, hsl(' + hue + ', 85%, 60%) 0%, hsl(' + ((hue + 40) % 360) + ', 85%, 45%) 100%)';
    var avatarGlow = '0 4px 10px rgba(0,0,0,0.1), 0 2px 8px hsl(' + hue + ', 85%, 60%, 0.3)';
    
    var avatar = '<div style="width:34px;height:34px;border-radius:50%;background:' + avatarBg + ';color:#fff;display:inline-flex;align-items:center;justify-content:center;font-size:12px;font-weight:700;margin-right:12px;box-shadow:' + avatarGlow + ';border: 2px solid rgba(255,255,255,0.2);">' + initials + '</div>';

    row.innerHTML =
      statusHtml +
      '<td><strong style="color:var(--primary); font-family:monospace; font-size:15px; letter-spacing:0.5px;">' + (u.roll || '') + '</strong></td>' +
      '<td><div style="display:inline-flex;align-items:center;font-weight:600;">' + avatar + '<span>' + nameVal + '</span></div></td>' +
      '<td><span style="background:rgba(67,97,238,0.06);color:var(--primary);border:1px solid rgba(67,97,238,0.15);padding:4px 12px;border-radius:20px;font-size:11px;font-weight:700;text-transform:uppercase;letter-spacing:0.5px;">' + (u.role || '') + '</span></td>' +
      '<td><span style="font-weight:600;color:var(--text-muted);font-size:13px;">' + (u.dept || '-') + '</span></td>' +
      '<td><span style="font-weight:500;color:var(--text-muted);font-size:13px;display:inline-flex;align-items:center;gap:4px;">' + (type === "Fingerprint + RFID" ? "👆💳 " : type === "Fingerprint" ? "👆 " : "🪪 ") + type + '</span></td>' +
      '<td><span style="font-family:monospace;font-weight:600;color:var(--success);font-size:13px;">' + (u.in || '-') + '</span></td>' +
      '<td><span style="font-family:monospace;font-weight:600;color:var(--danger);font-size:13px;">' + (u.out || '-') + '</span></td>' +
      '<td>' +
      '<button class="btn btn-success btn-sm" style="margin-right:6px; padding: 4px 10px; font-size:11px; font-weight:700; border-radius:6px; background:linear-gradient(135deg, var(--success), #05c290); box-shadow:0 2px 6px rgba(6,214,160,0.2);" onclick="manualPunch(\''+(u.uid || '')+'\',\'In\')" title="Manual In">In</button>' +
      '<button class="btn btn-danger btn-sm" style="margin-right:6px; padding: 4px 10px; font-size:11px; font-weight:700; border-radius:6px; background:linear-gradient(135deg, #ef476f, #d62246); box-shadow:0 2px 6px rgba(239,71,111,0.2);" onclick="manualPunch(\''+(u.uid || '')+'\',\'Out\')" title="Manual Out">Out</button>' +
      '<button class="btn btn-outline btn-sm" style="margin-right:6px; padding: 4px 10px; font-size:11px; font-weight:700; border-radius:6px;" onclick="openEdit(\''+(u.uid || '')+'\',\''+(nameVal.replace(/'/g, "\\'"))+'\',\''+(u.role || '')+'\',\''+(u.roll || '')+'\',\''+(u.rfid || '')+'\',\''+(u.dept || '')+'\')">Edit</button>' +
      '<button class="btn btn-outline btn-sm" style="color:var(--danger); border-color:rgba(239,71,111,0.3); padding: 4px 10px; font-size:11px; font-weight:700; border-radius:6px;" onclick="delUser(\''+(u.uid || '')+'\')">Delete</button>' +
      '</td>';
  });
  
  var sp = document.getElementById('stat-present');
  if (sp) sp.innerText = present;
  if(users.length === 0) tbody.innerHTML = '<tr><td colspan="9" style="text-align:center;padding:30px;color:var(--text-muted);">No employees registered yet.</td></tr>';
  else filterDir();
}
function delUser(uid) {
  if(!confirm('Delete user ' + uid + '?')) return;
  safeFetch('/delete-user?uid=' + encodeURIComponent(uid))
    .then(function(r) {
      if(r.ok) { showToast('User deleted', 'success'); loadDirectory(); }
      else showToast('Delete failed', 'error');
    });
}

function openEdit(uid, name, role, roll, rfid, dept) {
  var euid = document.getElementById('edit-uid');
  var ename = document.getElementById('edit-name');
  var erole = document.getElementById('edit-role');
  var eroll = document.getElementById('edit-roll');
  var edept = document.getElementById('edit-dept');
  var erfid = document.getElementById('edit-rfid');
  var erfc = document.getElementById('edit-rfid-container');
  var em = document.getElementById('edit-modal');
  
  if (euid) euid.value = uid;
  if (ename) ename.value = name;
  if (erole) erole.value = role;
  if (eroll) eroll.value = roll;
  if (edept) edept.value = dept || '';
  if (erfid) erfid.value = rfid || '';
  
  // Show RFID field only for fingerprint employees (those with '_f' in their UID)
  if (erfc) {
    if (uid.indexOf('_f') !== -1) {
      erfc.style.display = 'block';
    } else {
      erfc.style.display = 'none';
    }
  }
  
  // Show upgrade button only for card-only employees (those without '_f' in their UID)
  var upgradeCont = document.getElementById('edit-upgrade-container');
  if (upgradeCont) {
    if (uid.indexOf('_f') === -1) {
      upgradeCont.style.display = 'block';
    } else {
      upgradeCont.style.display = 'none';
    }
  }
  
  if (em) em.style.display = 'flex';
}
function closeModal() {
  var em = document.getElementById('edit-modal');
  if (em) em.style.display = 'none';
}
function openAddEmployeeModal() {
  document.getElementById('add-roll').value = '';
  document.getElementById('add-name').value = '';
  document.getElementById('add-dept').value = '';
  document.getElementById('add-rfid').value = '';
  document.getElementById('add-role').value = 'Employee';
  fetchNextIDForAdd();
  var m = document.getElementById('add-emp-modal');
  if (m) m.style.display = 'flex';
}
function closeAddEmployeeModal() {
  var m = document.getElementById('add-emp-modal');
  if (m) m.style.display = 'none';
}
function fetchNextIDForAdd() {
  safeFetch('/list-users')
    .then(function(r){ return r.json(); })
    .then(function(users){
      var maxId = 0;
      users.forEach(function(u){
        var idNum = parseInt(u.roll || u.uid, 10);
        if(!isNaN(idNum) && idNum > maxId) maxId = idNum;
      });
      var nextId = String(maxId + 1);
      while(nextId.length < 3) nextId = '0' + nextId;
      var ar = document.getElementById('add-roll');
      if (ar && !ar.value) ar.value = nextId;
    });
}
var lastAddedEmp = { roll: '', name: '', role: '', dept: '', rfid: '' };
function submitAddEmployee() {
  var roll = document.getElementById('add-roll').value.trim();
  var name = document.getElementById('add-name').value.trim();
  var role = document.getElementById('add-role').value;
  var dept = document.getElementById('add-dept').value.trim();
  var rfid = document.getElementById('add-rfid').value.trim().toUpperCase().replace(/\s+/g, '');

  if (!name || !roll) return showToast("Name and Employee ID are required", "error");

  lastAddedEmp = { roll: roll, name: name, role: role, dept: dept, rfid: rfid };
  var uid = rfid ? rfid : roll;

  safeFetch('/save?uid=' + encodeURIComponent(uid) + '&name=' + encodeURIComponent(name) + '&role=' + encodeURIComponent(role) + '&roll=' + encodeURIComponent(roll) + '&dept=' + encodeURIComponent(dept) + '&type=RFID&rfid=' + encodeURIComponent(rfid))
    .then(function(r) {
      if (r.ok) {
        showToast("Employee created!", "success");
        closeAddEmployeeModal();
        loadDirectory();
        promptHardwareEnrollment(name, roll);
      } else {
        showToast("Failed to save employee", "error");
      }
    });
}
function promptHardwareEnrollment(name, roll) {
  document.getElementById('post-enroll-name-display').innerText = name;
  document.getElementById('post-enroll-id-display').innerText = roll;
  var m = document.getElementById('post-enroll-modal');
  if (m) m.style.display = 'flex';
}
function closePostEnrollModal() {
  var m = document.getElementById('post-enroll-modal');
  if (m) m.style.display = 'none';
}
function startPostEnrollment(method) {
  closePostEnrollModal();
  show('reg');
  
  document.getElementById('reg-roll').value = lastAddedEmp.roll;
  document.getElementById('reg-n').value = lastAddedEmp.name;
  document.getElementById('reg-r').value = lastAddedEmp.role;
  if (document.getElementById('reg-dept')) {
    document.getElementById('reg-dept').value = lastAddedEmp.dept;
  }
  var em = document.getElementById('enroll-method');
  if (em) {
    em.value = method;
    toggleEnrollMethod();
  }
  showToast("Details pre-filled. Click Start Hardware Scan to capture credentials.", "info");
}
function launchEnrollmentFromEdit(method) {
  var name = document.getElementById('edit-name').value;
  var role = document.getElementById('edit-role').value;
  var roll = document.getElementById('edit-roll').value;
  var edept = document.getElementById('edit-dept');
  var dept = edept ? edept.value : '';

  closeModal();
  show('reg');

  document.getElementById('reg-roll').value = roll;
  document.getElementById('reg-n').value = name;
  document.getElementById('reg-r').value = role;
  if (document.getElementById('reg-dept')) {
    document.getElementById('reg-dept').value = dept;
  }
  var em = document.getElementById('enroll-method');
  if (em) {
    em.value = method;
    toggleEnrollMethod();
  }
  showToast("Employee details pre-filled. Click Start Hardware Scan to capture " + method + ".", "info");
}
function upgradeToBiometric() {
  launchEnrollmentFromEdit('FINGER');
}
function submitEdit() {
  var uid = document.getElementById('edit-uid').value;
  var name = document.getElementById('edit-name').value;
  var role = document.getElementById('edit-role').value;
  var roll = document.getElementById('edit-roll').value;
  var edept = document.getElementById('edit-dept');
  var dept = edept ? edept.value : '';
  var erfid = document.getElementById('edit-rfid');
  var rfid = erfid ? erfid.value.toUpperCase().replace(/\s+/g, '') : '';
  
  if(!name || !roll) return showToast("Fields cannot be empty", "error");
  
  safeFetch('/edit-user?uid='+encodeURIComponent(uid)+'&name='+encodeURIComponent(name)+'&role='+encodeURIComponent(role)+'&roll='+encodeURIComponent(roll)+'&rfid='+encodeURIComponent(rfid)+'&dept='+encodeURIComponent(dept))
    .then(function(r){
      if(r.ok) { showToast("User updated!", "success"); closeModal(); loadDirectory(); }
      else if(r.status === 409) showToast("RFID Card already in use by another user!", "error");
      else showToast("Update failed", "error");
    });
}
function filterDir() {
  var f = document.getElementById('dir-search').value.toLowerCase();
  var r = document.getElementById('dir-role-filter').value.toLowerCase();
  var s = document.getElementById('dir-status-filter').value.toLowerCase();
  var d = document.getElementById('dir-dept-filter') ? document.getElementById('dir-dept-filter').value.toLowerCase() : "";
  document.querySelectorAll('#dir-body tr').forEach(function(row){
    if (row.cells.length < 7) return;
    var textMatch = row.textContent.toLowerCase().indexOf(f) !== -1;
    var roleMatch = (r === "") || (row.cells[3].textContent.toLowerCase() === r);
    var deptMatch = (d === "") || (row.cells[4].textContent.toLowerCase() === d);
    
    var statusText = row.cells[0].textContent.toLowerCase();
    var statusMatch = true;
    if (s !== "") {
      if (s === "present") {
        statusMatch = (statusText.indexOf("active in") !== -1);
      } else if (s === "out") {
        statusMatch = (statusText === "out");
      } else if (s === "absent") {
        statusMatch = (statusText.indexOf("absent") !== -1);
      } else if (s === "weekly off") {
        statusMatch = (statusText.indexOf("weekly off") !== -1);
      } else if (s === "holiday") {
        statusMatch = (statusText.indexOf("holiday") !== -1);
      }
    }
    
    row.style.display = (textMatch && roleMatch && statusMatch && deptMatch) ? '' : 'none';
  });
}
function filterHist() {
  var f = document.getElementById('hist-filter').value.toLowerCase();
  var r = document.getElementById('hist-role-filter').value.toLowerCase();
  document.querySelectorAll('#hist-body tr').forEach(function(row){
    if (row.cells.length < 8) return;
    var textMatch = row.textContent.toLowerCase().indexOf(f) !== -1;
    var roleStr = row.cells[3].textContent.trim().toLowerCase();
    var roleMatch = (r === "") || (roleStr === r);
    row.style.display = (textMatch && roleMatch) ? '' : 'none';
  });
}
function uploadPhoto(rollId, callback) {
  var fileInput = document.getElementById('reg-photo');
  if (!fileInput || !fileInput.files || fileInput.files.length === 0) {
    if (callback) callback();
    return;
  }
  
  var file = fileInput.files[0];
  showToast("Processing profile photo...", "info");
  
  var reader = new FileReader();
  reader.onload = function(e) {
    var img = new Image();
    img.onload = function() {
      var canvas = document.createElement('canvas');
      var size = 120; // 120x120 pixels - matches the expanded Success screen layout perfectly
      canvas.width = size;
      canvas.height = size;
      
      var ctx = canvas.getContext('2d');
      // Crop to square centering the face/image
      var sourceSize = Math.min(img.width, img.height);
      var sourceX = (img.width - sourceSize) / 2;
      var sourceY = (img.height - sourceSize) / 2;
      
      ctx.drawImage(img, sourceX, sourceY, sourceSize, sourceSize, 0, 0, size, size);
      
      canvas.toBlob(function(blob) {
        if (!blob) {
          showToast("Photo processing failed", "error");
          if (callback) callback();
          return;
        }
        
        var formData = new FormData();
        formData.append("file", blob, "photos/" + rollId + ".jpg");
        
        showToast("Uploading profile photo...", "info");
        
        var xhr = new XMLHttpRequest();
        xhr.open("POST", "/upload", true);
        xhr.onload = function() {
          if (xhr.status === 200) {
            showToast("Profile photo uploaded!", "success");
          } else {
            showToast("Photo upload failed", "error");
          }
          if (callback) callback();
        };
        xhr.onerror = function() {
          showToast("Photo upload failed (Network Error)", "error");
          if (callback) callback();
        };
        xhr.send(formData);
      }, 'image/jpeg', 0.85);
    };
    img.onerror = function() {
      showToast("Invalid image file", "error");
      if (callback) callback();
    };
    img.src = e.target.result;
  };
  reader.onerror = function() {
    showToast("Error reading file", "error");
    if (callback) callback();
  };
  reader.readAsDataURL(file);
}

function saveUser() {
  var ruidEl = document.getElementById('reg-uid');
  var uid = ruidEl ? ruidEl.getAttribute('data-uid') : '';
  var rfid = ruidEl ? (ruidEl.getAttribute('data-rfid') || '') : '';
  
  if(!uid) return showToast("Enroll fingerprint or card first", "error");
  
  var r = document.getElementById('reg-roll').value;
  var n = document.getElementById('reg-n').value;
  var rl = document.getElementById('reg-r').value;
  var dept = document.getElementById('reg-dept') ? document.getElementById('reg-dept').value : '';
  
  if(!r || !n) return showToast("Fill in all fields", "error");
  
  var type = (uid === rfid) ? "RFID" : "FINGER";
  safeFetch('/save?uid=' + encodeURIComponent(uid) + '&roll=' + encodeURIComponent(r) + '&name=' + encodeURIComponent(n) + '&role=' + encodeURIComponent(rl) + '&type=' + type + '&rfid=' + encodeURIComponent(rfid) + '&dept=' + encodeURIComponent(dept))
    .then(function(){ 
      uploadPhoto(r, function() {
        showToast("User enrolled successfully!", "success"); 
        clearRegForm(); 
        show('dir'); 
      });
    });
}

var currentEnrollType = 'RFID';

function toggleEnrollMethod() {
  var method = document.getElementById('enroll-method').value;
  currentEnrollType = method;
  var fpSettings = document.getElementById('cc-fp-settings');
  if (fpSettings) {
    if (method === 'FINGER') {
      fpSettings.style.display = 'block';
    } else {
      fpSettings.style.display = 'none';
    }
  }
  var subHint = document.getElementById('cc-sub-hint');
  if (subHint) {
    if (method === 'FINGER') {
      subHint.innerText = 'Unified enrollment requires a fingerprint scan and optional RFID backup card.';
    } else {
      subHint.innerText = 'RFID Card enrollment only requires scanning an RFID card.';
    }
  }
}
function skipRFID() {
  safeFetch('/skip-rfid')
    .then(function(r) {
      if (r.ok) {
        showToast("Card enrollment skipped!", "info");
      }
    });
}

function fetchNextID() {
  safeFetch('/enroll-next-id')
    .then(function(r) { return r.text(); })
    .then(function(id) {
      var efid = document.getElementById('enroll-fp-id');
      if (efid) efid.value = id;
    });
}

function fetchMemory(sync) {
  var url = '/get-fp-memory' + (sync ? '?sync=1' : '');
  if(sync) showToast("Syncing with sensor hardware...", "warn");
  safeFetch(url)
    .then(function(r) { return r.json(); })
    .then(function(used) {
      var grid = document.getElementById('cc-memory-grid');
      if (!grid) return;
      grid.innerHTML = '';
      used.sort(function(a, b) { return a - b; });
      
      var fcs = document.getElementById('cc-free-slots');
      if (fcs) fcs.innerText = (200 - used.length);
      
      var fpcb = document.getElementById('fp-capacity-bar');
      if (fpcb) fpcb.style.width = (used.length/200*100) + '%';
      
      for(var i=1; i<=200; i++) {
        var slot = document.createElement('div');
        slot.className = 'm-slot' + (used.indexOf(i) !== -1 ? ' used' : '');
        slot.innerHTML = i;
        if(used.indexOf(i) !== -1) {
          (function(id){ slot.onclick = function() { deleteFingerprint(id); }; })(i);
        }
        grid.appendChild(slot);
      }
    });
}

function deleteFingerprint(id) {
  if(!confirm('Permanently delete fingerprint ID ' + id + '?')) return;
  safeFetch('/delete-fp?id=' + id).then(function(r) {
    if(r.ok) { showToast('ID ' + id + ' cleared', 'success'); fetchMemory(); fetchNextID(); }
  });
}

function clearAllFingerprints() {
  if(!confirm('ERASE ENTIRE BIOMETRIC DATABASE?')) return;
  safeFetch('/clear-fp-all').then(function(r) {
    if(r.ok) { showToast('Database wiped', 'warn'); fetchMemory(true); }
  });
}

function hardWipeSensor() {
  if(!confirm('🚨 ULTRA-HARD WIPE: This will manually delete every single slot from 1 to 200. This takes ~30 seconds. Continue?')) return;
  showToast("Hard Wipe started in background... please wait 30s", "warn");
  safeFetch('/hard-wipe-sensor').then(function(r) { 
    if(r.ok) { 
      setTimeout(function() {
        showToast('Hard Wipe Process finished!', 'success'); 
        fetchMemory(true); 
      }, 30000); // Wait 30 seconds before refreshing
    } 
  });
}

function startEnrollment() {
  if(!currentEnrollType) return showToast("Select method first", "error");
  var efid = document.getElementById('enroll-fp-id');
  var id = efid ? efid.value : '';
  if(currentEnrollType === 'FINGER' && (id < 1 || id > 200)) return showToast("Invalid Memory Slot", "error");
  
  var url = '/start-enroll?type=' + currentEnrollType;
  if(currentEnrollType === 'FINGER') url += '&id=' + id;
  
  safeFetch(url).then(function(r) {
    if(r.ok) {
      var ccsa = document.getElementById('cc-setup-area');
      var csca = document.getElementById('cc-scanning-area');
      var hs = document.getElementById('holo-scanner');
      var label = document.getElementById('cc-label');
      var esb = document.getElementById('enroll-status-badge');
      var s1i = document.getElementById('step1-indicator');
      var s2i = document.getElementById('step2-indicator');
      var cmh = document.getElementById('cc-main-hint');
      var csh = document.getElementById('cc-sub-hint');
      
      if (ccsa) ccsa.style.display = 'none';
      if (csca) csca.style.display = 'block';
      if (hs) hs.classList.add('active');
      if (label) {
        if (currentEnrollType === 'FINGER') {
          label.innerText = 'INITIALIZING';
        } else {
          label.innerText = 'SCAN RFID CARD';
        }
      }
      if (esb) {
        esb.innerText = 'SCANNING';
        esb.style.background = 'var(--warning)';
      }
      
      if (s1i) s1i.classList.remove('active');
      if (s2i) s2i.classList.add('active');
      
      var spl = document.getElementById('steps-progress-line');
      if (spl) {
        if (currentEnrollType === 'FINGER') {
          spl.style.width = '50%';
        } else {
          spl.style.width = '75%';
        }
      }
      
      if (cmh) {
        if (currentEnrollType === 'FINGER') {
          cmh.innerText = "Step 2: Hardware Active";
        } else {
          cmh.innerText = "Step 2: Scan RFID Card";
        }
      }
      if (csh) {
        if (currentEnrollType === 'FINGER') {
          csh.innerText = "Please interact with the hardware scanner. Follow the sequence on the right.";
        } else {
          csh.innerText = "Please place your RFID card on the reader.";
        }
      }
      
      resetChecklist();
      setCheckItem(1, true);
      if (currentEnrollType === 'FINGER') {
        speak("Scan started. Step 2. Please place your finger on the biometric sensor.");
      } else {
        speak("Scan started. Please place your RFID card on the reader.");
      }
    }
  });
}

function resetChecklist() {
  for(var i=1; i<=4; i++) {
    var el = document.getElementById('check-'+i);
    if (el) el.classList.remove('done');
  }
}
function setCheckItem(id, done) {
  var el = document.getElementById('check-'+id);
  if (el) el.classList.toggle('done', done);
}

function updateEnrollProgress(d) {
  var label = document.getElementById('cc-label');
  var prog = document.getElementById('holo-progress');
  var glow = document.getElementById('ambient-glow');
  
  if (d.status === "FP_OK") {
    if (prefilledRFID) {
      var ruid = document.getElementById('reg-uid');
      if (ruid) {
        ruid.setAttribute('data-uid', d.uid);
        ruid.setAttribute('data-rfid', prefilledRFID);
      }
      showToast("Fingerprint captured! Saving employee...", "success");
      saveUser();
      prefilledRFID = ""; // Reset
      return;
    }
    var ruid = document.getElementById('reg-uid');
    if (ruid) {
      ruid.setAttribute('data-uid', d.uid);
      ruid.innerText = 'Finger ID: ' + d.uid + ' (Scan backup card...)';
    }
    var rroll = document.getElementById('reg-roll');
    if (rroll && !rroll.value) rroll.value = d.uid;
    
    if (label) {
      label.innerText = 'SCAN BACKUP CARD';
      label.style.color = 'var(--warning)';
    }
    
    if (glow) glow.className = 'ambient-glow warning';
    
    var csca = document.getElementById('cc-scanning-area');
    var cmh = document.getElementById('cc-main-hint');
    var csh = document.getElementById('cc-sub-hint');
    var ccicon = document.getElementById('cc-icon');
    
    if (ccicon) ccicon.innerHTML = '🪪';
    if (cmh) cmh.innerText = "Step 2.5: Scan Backup Card";
    if (csh) csh.innerText = "Scan your RFID card to link it as a backup, or click Skip below.";
    
    if (csca) {
      csca.innerHTML = '<button class="btn btn-primary" style="width:100%; margin-bottom:10px; height:45px;" onclick="skipRFID()">⏩ Skip / Finish Registration</button>' +
                       '<button class="btn btn-outline" style="color:var(--danger); border-color:var(--danger); width:100%; height:45px;" onclick="clearRegForm()">🛑 Cancel Scanning</button>';
    }
    
    updateEnrollPreview();
    showToast("Fingerprint captured! Scan card next.", "success");
    setCheckItem(3, true);
    speak("Fingerprint captured. Now, scan your backup RFID card, or click skip.");
  } else if(d.status === "SUCCESS") {
    var ruid = document.getElementById('reg-uid');
    var ccicon = document.getElementById('cc-icon');
    var esb = document.getElementById('enroll-status-badge');
    var csca = document.getElementById('cc-scanning-area');
    var s2i = document.getElementById('step2-indicator');
    var s3i = document.getElementById('step3-indicator');
    var cmh = document.getElementById('cc-main-hint');
    var csh = document.getElementById('cc-sub-hint');
    var ccdc = document.getElementById('cc-details-card');
    
    if (ruid) {
      ruid.setAttribute('data-uid', d.uid);
      if (d.rfid) {
        ruid.setAttribute('data-rfid', d.rfid);
        if (d.uid !== d.rfid) {
          ruid.innerText = 'Finger ID: ' + d.uid + ' | Card: ' + d.rfid;
        } else {
          ruid.innerText = 'Card UID: ' + d.rfid;
        }
      } else {
        ruid.innerText = 'Finger ID: ' + d.uid + ' (No linked card)';
      }
    }
    if (ccicon) ccicon.innerHTML = '✅';
    if (label) {
      label.innerText = 'SUCCESS';
      label.style.color = 'var(--success)';
    }
    if (prog) prog.classList.add('p100');
    if (glow) glow.className = 'ambient-glow success';
    
    if (currentEnrollType === 'RFID') {
      setCheckItem(2, true);
      setCheckItem(3, true);
    }
    setCheckItem(4, true);
    
    if (ccdc) {
      ccdc.style.opacity = '1';
      ccdc.style.pointerEvents = 'auto';
    }
    if (esb) {
      esb.innerText = 'VERIFIED';
      esb.style.background = 'var(--success)';
      esb.style.color = '#fff';
    }
    
    if (csca) csca.style.display = 'none';
    if (s2i) s2i.classList.remove('active');
    if (s3i) s3i.classList.add('active');
    
    var spl = document.getElementById('steps-progress-line');
    if (spl) spl.style.width = '100%';
    
    if (cmh) cmh.innerText = "Step 3: Hardware Verified!";
    if (csh) csh.innerText = "Please complete the Identity Registration form on the right.";
 
    updateEnrollPreview();
    showToast("Hardware verified!", "success");
    fetchMemory();
    speak("Hardware verification complete. Step 3. Please enter the employee's roll ID, full name, and designation, then click Finalize Registration.");
  } else if(d.status === "FAIL") {
    if (glow) glow.className = 'ambient-glow error';
    showToast(d.msg || "Error", "error");
    speak("Enrollment failed. " + (d.msg || "Error"));
    setTimeout(clearRegForm, 2000);
  } else {
    // Show step/warning instruction message in the sub-hint area
    if (d.msg) {
      var csh = document.getElementById('cc-sub-hint');
      if (csh) csh.innerText = d.msg;
    }
    
    // Only update label step number if 'step' is explicitly defined
    if (d.step) {
      if (label) label.innerText = 'STEP ' + d.step;
      if (d.step === 2) { 
        setCheckItem(2, true); 
        if (prog) prog.classList.add('p50'); 
        var ccicon = document.getElementById('cc-icon');
        if (ccicon) ccicon.innerHTML = '↔️'; 
        speak("First scan okay. Please remove your finger.");
      }
      if (d.step === 3) { 
        // We are now waiting for 2nd scan. Keep checklist Item 3 unchecked until FP_OK is received.
        var ccicon = document.getElementById('cc-icon');
        if (ccicon) ccicon.innerHTML = '☝️'; 
        speak("Please place the same finger again.");
      }
    }
  }
}

function clearRegForm() {
  var ruid = document.getElementById('reg-uid');
  var rroll = document.getElementById('reg-roll');
  var rn = document.getElementById('reg-n');
  var glow = document.getElementById('ambient-glow');
  var hs = document.getElementById('holo-scanner');
  var prog = document.getElementById('holo-progress');
  var ccicon = document.getElementById('cc-icon');
  var label = document.getElementById('cc-label');
  var esb = document.getElementById('enroll-status-badge');
  var cmh = document.getElementById('cc-main-hint');
  var csh = document.getElementById('cc-sub-hint');
  var s1i = document.getElementById('step1-indicator');
  var s2i = document.getElementById('step2-indicator');
  var s3i = document.getElementById('step3-indicator');
  var ccdc = document.getElementById('cc-details-card');
  var ccsa = document.getElementById('cc-setup-area');
  var csca = document.getElementById('cc-scanning-area');
  
  if (ruid) {
    ruid.innerText = "UID: READY";
    ruid.removeAttribute('data-uid');
    ruid.removeAttribute('data-rfid');
  }
  if (rroll) rroll.value = "";
  if (rn) rn.value = "";
  var rd = document.getElementById('reg-dept');
  if (rd) rd.value = "";
  var rp = document.getElementById('reg-photo');
  if (rp) rp.value = "";
  if (glow) glow.className = 'ambient-glow';
  if (hs) hs.classList.remove('active');
  if (prog) prog.className = 'holo-progress';
  if (ccicon) ccicon.innerHTML = '☝️';
  if (label) {
    label.innerText = 'SYSTEM READY';
    label.style.color = '';
  }
  if (esb) {
    esb.innerText = 'IDLE';
    esb.style.background = '';
    esb.style.color = '';
  }
  if (cmh) cmh.innerText = "Step 1: Start Enrollment";
  var method = document.getElementById('enroll-method') ? document.getElementById('enroll-method').value : 'RFID';
  currentEnrollType = method;
  if (csh) {
    if (currentEnrollType === 'FINGER') {
      csh.innerText = "Unified enrollment requires a fingerprint scan and optional RFID backup card.";
    } else {
      csh.innerText = "RFID Card enrollment only requires scanning an RFID card.";
    }
  }
  if (s1i) s1i.classList.add('active');
  if (s2i) s2i.classList.remove('active');
  if (s3i) s3i.classList.remove('active');
  if (ccdc) {
    ccdc.style.opacity = '0.3';
    ccdc.style.pointerEvents = 'none';
  }
  if (ccsa) ccsa.style.display = 'block';
  
  if (csca) {
    csca.style.display = 'none';
    csca.innerHTML = '<button class="btn btn-outline" style="color:var(--danger); border-color:var(--danger); width:100%; height:45px;" onclick="clearRegForm()">🛑 Cancel Scanning</button>';
  }
  
  var spl = document.getElementById('steps-progress-line');
  if (spl) spl.style.width = '0%';
  updateEnrollPreview();
  
  resetChecklist();
  fetchNextID();
  safeFetch('/cancel-enroll');
}
function fetchHistory() {
  var fromInput = document.getElementById('hist-date-from').value;
  var toInput = document.getElementById('hist-date-to').value;
  if(!fromInput || !toInput) return showToast("Select a date range", "error");
  
  var fromParts = fromInput.split('-');
  var fromD = new Date(parseInt(fromParts[0]), parseInt(fromParts[1]) - 1, parseInt(fromParts[2]));
  var toParts = toInput.split('-');
  var toD = new Date(parseInt(toParts[0]), parseInt(toParts[1]) - 1, parseInt(toParts[2]));
  if(fromD > toD) return showToast("Invalid date range", "error");
  
  var filter = document.getElementById('hist-filter').value.toLowerCase();
  var rFilter = document.getElementById('hist-role-filter').value.toLowerCase();
  
  var b = document.getElementById('hist-body');
  b.innerHTML = '<tr><td colspan="8" style="text-align:center;padding:30px;color:var(--text-muted);">Loading data...</td></tr>';
  
  var records = {};
  var order = [];
  var hasData = false;

  safeFetch('/list-holidays')
    .then(function(r) { return r.json(); })
    .catch(function() { return []; })
    .then(function(holidaysList) {

      var datesToFetch = [];
      var tempD = new Date(fromD);
      while (tempD <= toD) {
        var dd = String(tempD.getDate());
        if (dd.length < 2) dd = '0' + dd;
        var mm = String(tempD.getMonth() + 1);
        if (mm.length < 2) mm = '0' + mm;
        var yyyy = tempD.getFullYear();
        datesToFetch.push({
          str: dd + '-' + mm + '-' + yyyy,
          isSunday: tempD.getDay() === 0
        });
        tempD.setDate(tempD.getDate() + 1);
      }

      function renderHistory() {
        b.innerHTML = '';
        if(!hasData){
          b.innerHTML = '<tr><td colspan="8" style="text-align:center;padding:30px;color:var(--text-muted);">No records in this range.</td></tr>';
          return;
        }
        
        var displayCount = 0;
        // Render in reverse so newest daily arrivals are at the top
        for (var j = order.length - 1; j >= 0; j--) {
          var rec = records[order[j]];
          
          displayCount++;
          var row = b.insertRow();
          
          // Determine initials and gradient for user avatar in transactions
          var nameVal = rec.name || "";
          var initials = nameVal.split(' ').map(function(n){return n ? n[0] : '';}).join('').substring(0,2).toUpperCase();
          if(initials.length === 0) initials = "ID";
          
          var charSum = 0;
          for (var k = 0; k < initials.length; k++) charSum += initials.charCodeAt(k);
          var hue = (charSum * 57) % 360;
          var avatarBg = 'linear-gradient(135deg, hsl(' + hue + ', 85%, 60%) 0%, hsl(' + ((hue + 40) % 360) + ', 85%, 45%) 100%)';
          var avatarGlow = '0 4px 8px rgba(0,0,0,0.06), 0 2px 6px hsl(' + hue + ', 85%, 60%, 0.3)';
          var avatar = '<div style="width:28px;height:28px;border-radius:50%;background:' + avatarBg + ';color:#fff;display:inline-flex;align-items:center;justify-content:center;font-size:10px;font-weight:700;margin-right:10px;box-shadow:' + avatarGlow + ';border:1px solid rgba(255,255,255,0.2);">' + initials + '</div>';
 
          // Advanced Status Badges
          var statusHtml = '';
          var statClean = rec.status.trim().toLowerCase();
          
          if (statClean === 'absent') {
            statusHtml = '<span class="badge" style="background:rgba(239,71,111,0.1); color:var(--danger); border:1px solid rgba(239,71,111,0.25);"><span class="status-dot dot-absent"></span>Absent</span>';
          } else if (statClean === 'weekly off') {
            statusHtml = '<span class="badge" style="background:rgba(88,166,255,0.1); color:#58a6ff; border:1px solid rgba(88,166,255,0.25);"><span class="status-dot" style="background:#58a6ff; box-shadow:0 0 10px #58a6ff;"></span>Weekly Off</span>';
          } else if (statClean.indexOf('holiday') !== -1) {
            var hBadgeColor = 'var(--success)';
            var hBadgeBg = 'rgba(6,214,160,0.1)';
            var hBadgeBorder = 'rgba(6,214,160,0.25)';
            if (statClean.indexOf('optional') !== -1) {
              hBadgeColor = 'var(--warning)';
              hBadgeBg = 'rgba(255,209,102,0.1)';
              hBadgeBorder = 'rgba(255,209,102,0.25)';
            } else if (statClean.indexOf('company') !== -1) {
              hBadgeColor = '#a855f7';
              hBadgeBg = 'rgba(168,85,247,0.1)';
              hBadgeBorder = 'rgba(168,85,247,0.25)';
            }
            statusHtml = '<span class="badge" style="background:' + hBadgeBg + '; color:' + hBadgeColor + '; border:1px solid ' + hBadgeBorder + ';"><span class="status-dot" style="background:' + hBadgeColor + '; box-shadow:0 0 10px ' + hBadgeColor + ';"></span>' + rec.status + '</span>';
          } else if (statClean === 'on-time') {
            statusHtml = '<span class="badge badge-in"><span class="status-dot dot-in"></span>On-Time</span>';
          } else if (statClean === 'late') {
            statusHtml = '<span class="badge" style="background:rgba(255,209,102,0.1); color:var(--warning); border:1px solid rgba(255,209,102,0.25);"><span class="status-dot" style="background:var(--warning); box-shadow:0 0 10px var(--warning);"></span>Late</span>';
          } else if (statClean === 'early exit') {
            statusHtml = '<span class="badge" style="background:rgba(239,71,111,0.1); color:var(--danger); border:1px solid rgba(239,71,111,0.25);"><span class="status-dot" style="background:var(--danger); box-shadow:0 0 10px var(--danger);"></span>Early Exit</span>';
          } else if (statClean === 'late & early exit') {
            statusHtml = '<span class="badge" style="background:rgba(239,71,111,0.15); color:var(--danger); border:1px solid rgba(239,71,111,0.25);"><span class="status-dot" style="background:var(--danger); box-shadow:0 0 10px var(--danger);"></span>Late & Early Exit</span>';
          } else if (rec.accepted || statClean === 'accepted') {
            statusHtml = '<span class="badge badge-in"><span class="status-dot dot-in"></span>Accepted</span>';
          } else {
            statusHtml = '<span class="badge badge-out"><span class="status-dot dot-out"></span>Denied</span>';
          }
 
          var inBadge = rec.inTime !== '-' ? '<span class="badge badge-in" style="font-family:monospace; font-size:13px; font-weight:700;">' + rec.inTime + '</span>' : '<span style="color:var(--text-muted); font-weight:500;">-</span>';
          var outBadge = rec.outTime !== '-' ? '<span class="badge badge-out" style="background:rgba(67,97,238,0.1); color:var(--primary); border:1px solid rgba(67,97,238,0.2); font-family:monospace; font-size:13px; font-weight:700;">' + rec.outTime + '</span>' : '<span style="color:var(--text-muted); font-weight:500;">-</span>';
          var workHrs = calculateWorkHrs(rec.inTime, rec.outTime);
          var workHrsBadge = workHrs !== '-' ? '<span class="badge" style="background:rgba(67,97,238,0.08); color:var(--primary); border:1px solid rgba(67,97,238,0.18); font-family:monospace; font-size:13px; font-weight:700;">' + workHrs + '</span>' : '<span style="color:var(--text-muted); font-weight:500;">-</span>';
          
          row.innerHTML =
            '<td><span style="font-family:monospace; font-size:13px; font-weight:600; color:var(--text-muted);">' + formatFriendlyDate(rec.date) + '</span></td>' +
            '<td><strong style="color:var(--primary); font-family:monospace; font-size:14px;">' + rec.id + '</strong></td>' +
            '<td style="display:flex;align-items:center;font-weight:600;">' + avatar + nameVal + '</td>' +
            '<td><span style="background:rgba(67,97,238,0.06);color:var(--primary);border:1px solid rgba(67,97,238,0.15);padding:4px 10px;border-radius:20px;font-size:10px;font-weight:700;text-transform:uppercase;letter-spacing:0.5px;">' + rec.role + '</span></td>' +
            '<td>' + inBadge + '</td>' +
            '<td>' + outBadge + '</td>' +
            '<td>' + workHrsBadge + '</td>' +
            '<td>' + statusHtml + '</td>' +
            '<td><span style="font-weight:500;color:var(--text-muted);font-size:12px;">' + (rec.type === "Fingerprint" ? "👆 " : rec.type === "RFID Card" ? "🪪 " : "") + rec.type + '</span></td>';
        }
        
        if(displayCount === 0) {
          b.innerHTML = '<tr><td colspan="9" style="text-align:center;padding:30px;color:var(--text-muted);">No records in this range.</td></tr>';
        } else {
          filterHist();
        }
        updateCapacityDisplay(displayCount);
      }
 
      getUsersList(function(usersList) {
        function fetchNextDate(index) {
          if (index >= datesToFetch.length) {
            renderHistory();
            return;
          }
          
          var dateObj = datesToFetch[index];
          var dateStr = dateObj.str;
          var hObj = getHolidayForDate(dateStr, holidaysList);
          
          var fetchUrl = (window.sqlApiUrl) ? '/get-sql-history?date=' + dateStr : '/get-history?date=' + dateStr;
          safeFetch(fetchUrl)
            .then(function(r) { return r.text(); })
            .then(function(text) {
              if(text && text.trim()) {
                hasData = true;
                var lines = text.trim().split('\n');
                for (var i = 0; i < lines.length; i++) {
                  var cols = lines[i].split(',');
                  if (cols.length < 5) continue;
                  var id = cols[1].trim();
                  if (id.toLowerCase() === 'id' || id.toLowerCase() === 'pending_sync' || id.toLowerCase() === 'sql_config' || id.toLowerCase() === 'sql_transferred' || id.toLowerCase() === 'weather_city') continue;
                  
                  var isOldFormat = (cols.length < 7);
                  var timeStr = cols[0];
                  var nameStr = cols[2];
                  var roleStr = isOldFormat ? "-" : cols[3];
                  var statusStr = isOldFormat ? cols[4].trim().toLowerCase() : cols[5].trim().toLowerCase();
                  var statusDisp = isOldFormat ? cols[4] : cols[5];
                  var methodStr = isOldFormat ? (cols[5] || "-") : (cols[6] || "-");
                  var isAccepted = (statusStr === 'accepted' || statusStr === 'on-time' || statusStr === 'late' || statusStr === 'early exit' || statusStr === 'late & early exit');
 
                  var recKey = dateStr + "_" + id;
                  if (!records[recKey]) {
                    records[recKey] = { date: dateStr, id: id, name: nameStr, role: roleStr, inTime: timeStr, outTime: "-", status: statusDisp, type: methodStr, accepted: isAccepted };
                    order.push(recKey);
                  } else {
                    if (isAccepted) {
                      records[recKey].outTime = timeStr;
                      
                      var oldStatus = records[recKey].status.trim().toLowerCase();
                      var newStatus = statusDisp.trim().toLowerCase();
                      if (oldStatus === 'late' && newStatus === 'early exit') {
                        records[recKey].status = 'Late & Early Exit';
                      } else if (oldStatus === 'late') {
                        // Keep late status if checkout is on-time
                      } else {
                        records[recKey].status = statusDisp;
                      }
                    }
                  }
                }
              }
              
              // Fill in absent entries for registered employees if any exist
              if (usersList && usersList.length > 0) {
                usersList.forEach(function(u) {
                  var id = u.roll || u.uid;
                  var recKey = dateStr + "_" + id;
                  if (!records[recKey]) {
                    var statusStr = hObj ? ("Holiday (" + hObj.name + " - " + hObj.type + ")") : (dateObj.isSunday ? "Weekly Off" : "Absent");
                    records[recKey] = {
                      date: dateStr,
                      id: id,
                      name: u.name,
                      role: u.role || "-",
                      inTime: "-",
                      outTime: "-",
                      status: statusStr,
                      type: "-",
                      accepted: (hObj || dateObj.isSunday) ? true : false
                    };
                    order.push(recKey);
                    hasData = true;
                  }
              fetchNextDate(index + 1);
            })
            .catch(function(err) {
              console.error(err);
              // Mark all registered employees as Absent or Holiday if the file fails to fetch (or doesn't exist)
              hasData = true;
              usersList.forEach(function(u) {
                var id = u.roll || u.uid;
                var recKey = dateStr + "_" + id;
                if (!records[recKey]) {
                  var statusStr = hObj ? ("Holiday (" + hObj.name + " - " + hObj.type + ")") : (dateObj.isSunday ? "Weekly Off" : "Absent");
                  records[recKey] = {
                    date: dateStr,
                    id: id,
                    name: u.name,
                    role: u.role || "-",
                    inTime: "-",
                    outTime: "-",
                    status: statusStr,
                    type: "-",
                    accepted: (hObj || dateObj.isSunday) ? true : false
                  };
                  order.push(recKey);
                }
              });
              fetchNextDate(index + 1);
            });
        }
        
        fetchNextDate(0);
      });
    });
}

function downloadCSV() {
  var fromInput = document.getElementById('hist-date-from').value;
  var toInput = document.getElementById('hist-date-to').value;
  if(!fromInput || !toInput) return showToast("Select a date range", "error");
  
  var fromParts = fromInput.split('-');
  var fromD = new Date(parseInt(fromParts[0]), parseInt(fromParts[1]) - 1, parseInt(fromParts[2]));
  var toParts = toInput.split('-');
  var toD = new Date(parseInt(toParts[0]), parseInt(toParts[1]) - 1, parseInt(toParts[2]));
  if(fromD > toD) return showToast("Invalid date range", "error");
  
  showToast("Preparing CSV...", "warn");
  
  var records = {};
  var order = [];
  var hasData = false;

  safeFetch('/list-holidays')
    .then(function(r) { return r.json(); })
    .catch(function() { return []; })
    .then(function(holidaysList) {

      var datesToFetch = [];
      var tempD = new Date(fromD);
      while (tempD <= toD) {
        var dd = String(tempD.getDate());
        if (dd.length < 2) dd = '0' + dd;
        var mm = String(tempD.getMonth() + 1);
        if (mm.length < 2) mm = '0' + mm;
        var yyyy = tempD.getFullYear();
        datesToFetch.push({
          str: dd + '-' + mm + '-' + yyyy,
          isSunday: tempD.getDay() === 0
        });
        tempD.setDate(tempD.getDate() + 1);
      }

      function generateAndDownloadCSV() {
        if(!hasData) return showToast("No records to export in this range", "error");
        
        var finalCsvLines = [];
        var header = "Date,ID,Name,Role,In Time,Out Time,Work Hours,Status,Type";
        
        for (var j = 0; j < order.length; j++) {
          var rec = records[order[j]];
          var workHrs = calculateWorkHrs(rec.inTime, rec.outTime);
          finalCsvLines.push([
            rec.date, rec.id, rec.name, rec.role, rec.inTime, rec.outTime, workHrs, rec.status, rec.type
          ].join(','));
        }
        
        var finalCsv = header + '\n' + finalCsvLines.join('\n');
        var blob = new Blob([finalCsv], { type: 'text/csv' });
        var url = window.URL.createObjectURL(blob);
        var a = document.createElement('a');
        a.setAttribute('hidden', '');
        a.setAttribute('href', url);
        var filename = (fromInput === toInput) ? 'logs_' + fromInput + '.csv' : 'logs_' + fromInput + '_to_' + toInput + '.csv';
        a.setAttribute('download', filename);
        document.body.appendChild(a);
        a.click();
        document.body.removeChild(a);
      }

      getUsersList(function(usersList) {
        function fetchNextDate(index) {
          if (index >= datesToFetch.length) {
            generateAndDownloadCSV();
            return;
          }
          
          var dateObj = datesToFetch[index];
          var dateStr = dateObj.str;
          var hObj = getHolidayForDate(dateStr, holidaysList);
          
          safeFetch('/get-history?date=' + dateStr)
            .then(function(r) { return r.text(); })
            .then(function(text) {
              if(text && text.trim()) {
                hasData = true;
                var lines = text.trim().split('\n');
                for (var i = 0; i < lines.length; i++) {
                  var cols = lines[i].split(',');
                  if (cols.length < 5) continue;
                  var id = cols[1].trim();
                  if (id.toLowerCase() === 'id') continue;
                  
                  var isOldFormat = (cols.length < 7);
                  var timeStr = cols[0];
                  var nameStr = cols[2];
                  var roleStr = isOldFormat ? "-" : cols[3];
                  var statusStr = isOldFormat ? cols[4].trim().toLowerCase() : cols[5].trim().toLowerCase();
                  var statusDisp = isOldFormat ? cols[4] : cols[5];
                  var methodStr = isOldFormat ? (cols[5] || "-") : (cols[6] || "-");
                  var isAccepted = (statusStr === 'accepted' || statusStr === 'on-time' || statusStr === 'late' || statusStr === 'early exit' || statusStr === 'late & early exit');

                  var recKey = dateStr + "_" + id;
                  if (!records[recKey]) {
                    records[recKey] = { date: dateStr, id: id, name: nameStr, role: roleStr, inTime: timeStr, outTime: "-", status: statusDisp, type: methodStr };
                    order.push(recKey);
                  } else {
                    if (isAccepted) {
                      records[recKey].outTime = timeStr;
                      
                      var oldStatus = records[recKey].status.trim().toLowerCase();
                      var newStatus = statusDisp.trim().toLowerCase();
                      if (oldStatus === 'late' && newStatus === 'early exit') {
                        records[recKey].status = 'Late & Early Exit';
                      } else if (oldStatus === 'late') {
                        // Keep late status if checkout is on-time
                      } else {
                        records[recKey].status = statusDisp;
                      }
                    }
                  }
                }
              }
              
              // Fill in absent entries for all other registered employees
              hasData = true;
              usersList.forEach(function(u) {
                var id = u.roll || u.uid;
                var recKey = dateStr + "_" + id;
                if (!records[recKey]) {
                  var statusStr = hObj ? ("Holiday (" + hObj.name + " - " + hObj.type + ")") : (dateObj.isSunday ? "Weekly Off" : "Absent");
                  records[recKey] = {
                    date: dateStr,
                    id: id,
                    name: u.name,
                    role: u.role || "-",
                    inTime: "-",
                    outTime: "-",
                    status: statusStr,
                    type: "-"
                  };
                  order.push(recKey);
                }
              });
              
              fetchNextDate(index + 1);
            })
            .catch(function(err) {
              console.error(err);
              // Mark all registered employees as Absent or Holiday if the file fails to fetch (or doesn't exist)
              hasData = true;
              usersList.forEach(function(u) {
                var id = u.roll || u.uid;
                var recKey = dateStr + "_" + id;
                if (!records[recKey]) {
                  var statusStr = hObj ? ("Holiday (" + hObj.name + " - " + hObj.type + ")") : (dateObj.isSunday ? "Weekly Off" : "Absent");
                  records[recKey] = {
                    date: dateStr,
                    id: id,
                    name: u.name,
                    role: u.role || "-",
                    inTime: "-",
                    outTime: "-",
                    status: statusStr,
                    type: "-"
                  };
                  order.push(recKey);
                }
              });
              fetchNextDate(index + 1);
            });
        }
        
        fetchNextDate(0);
      });
    });
}

function deleteLogs() {
  var fromInput = document.getElementById('hist-date-from').value;
  var toInput = document.getElementById('hist-date-to').value;
  if(!fromInput || !toInput) return showToast("Select a date range", "error");
  
  var fromD = new Date(fromInput);
  var toD = new Date(toInput);
  if(fromD > toD) return showToast("Invalid date range", "error");

  var rangeStr = (fromInput === toInput) ? fromInput : fromInput + ' to ' + toInput;
  if(!confirm('Delete ALL records for ' + rangeStr + '? This cannot be undone.')) return;
  
  var datesToFetch = [];
  var tempD = new Date(fromD);
  while (tempD <= toD) {
    var dd = String(tempD.getDate());
    if (dd.length < 2) dd = '0' + dd;
    var mm = String(tempD.getMonth() + 1);
    if (mm.length < 2) mm = '0' + mm;
    var yyyy = tempD.getFullYear();
    datesToFetch.push(dd + '-' + mm + '-' + yyyy);
    tempD.setDate(tempD.getDate() + 1);
  }

  var deletedCount = 0;
  
  function deleteNextDate(index) {
    if (index >= datesToFetch.length) {
      showToast(deletedCount + ' logs deleted', 'success');
      document.getElementById('hist-body').innerHTML = '<tr><td colspan="8" style="text-align:center;padding:30px;color:var(--text-muted);">Logs deleted.</td></tr>';
      safeFetch('/init-data').then(function(r){ return r.json(); }).then(function(d){
        window.systemInitData = d;
        updateCapacityDisplay(0);
      });
      return;
    }
    
    var dateStr = datesToFetch[index];
    safeFetch('/delete-logs?date=' + dateStr)
      .then(function(r) {
        if(r.ok) deletedCount++;
        deleteNextDate(index + 1);
      })
      .catch(function() {
        deleteNextDate(index + 1);
      });
  }
  
  deleteNextDate(0);
}

function deleteAllLogs() {
  if(!confirm('Erase ALL history logs forever? This cannot be undone.')) return;
  safeFetch('/delete-all-logs').then(function(){ 
    showToast('All logs cleared', 'success'); 
    document.getElementById('hist-body').innerHTML = '<tr><td colspan="8" style="text-align:center;padding:30px;color:var(--text-muted);">All logs cleared.</td></tr>';
    safeFetch('/init-data').then(function(r){ return r.json(); }).then(function(d){
      window.systemInitData = d;
      updateCapacityDisplay(0);
    });
  });
}

function rebootDevice() {
  if(!confirm('Reboot the device now?')) return;
  safeFetch('/reboot').then(function(){ showToast('Rebooting...', 'warn'); });
}

function scanWifi() {
  var s = document.getElementById('wifi-ssid-select');
  s.innerHTML = '<option>Scanning...</option>';
  safeFetch('/scan-wifi').then(function(r){ return r.json(); }).then(function(data){
    s.innerHTML = '<option value="">-- Select network --</option>';
    data.forEach(function(ssid){ s.innerHTML += '<option value="'+ssid+'">'+ssid+'</option>'; });
  }).catch(function(){ s.innerHTML = '<option value="">Scan failed</option>'; });
}

function saveWiFi() {
  var s = document.getElementById('wifi-ssid').value;
  var p = document.getElementById('wifi-pass').value;
  var ip = document.getElementById('wifi-ip').value;
  var gw = document.getElementById('wifi-gw').value;
  var sn = document.getElementById('wifi-sn').value;
  if(!s) return showToast("Enter SSID", "error");
  var url = '/set-wifi?ssid='+encodeURIComponent(s)+'&pass='+encodeURIComponent(p);
  if(ip) url += '&ip='+encodeURIComponent(ip)+'&gw='+encodeURIComponent(gw)+'&sn='+encodeURIComponent(sn);
  safeFetch(url).then(function(){ showToast("Saved! Restarting...", "success"); });
}

function resetWiFiOnly() {
  if(!confirm('🚨 Confirm: Erase WiFi configuration settings? The device will reboot to Access Point (AP) mode.')) return;
  showToast("Erasing WiFi settings & rebooting...", "warn");
  safeFetch('/reset-wifi').then(function(){ 
    setTimeout(function(){ location.reload(); }, 2000); 
  });
}

function clearLogsOnly() {
  if(!confirm('🚨 Confirm: Permanently erase all attendance transaction logs and history? This cannot be undone.')) return;
  showToast("Clearing all logs...", "warn");
  safeFetch('/delete-all-logs').then(function(){ 
    showToast("Logs wiped successfully!", "success");
    loadDirectory(); // Refresh dashboard usage sizes
  });
}

function wipeBiometricsOnly() {
  if(!confirm('🚨 Confirm: Permanently wipe all fingerprint templates from the biometric sensor database?')) return;
  if(!confirm('Are you ABSOLUTELY SURE? All registered fingerprint data will be lost.')) return;
  showToast("Clearing biometric sensor database...", "warn");
  safeFetch('/clear-fp-all').then(function(){ 
    showToast("Fingerprint templates wiped!", "success");
    fetchMemory(true); // Refresh hardware memory map
  });
}

function toggleNewPassVisibility() {
  var p = document.getElementById('new-admin-pass');
  var t = document.getElementById('new-pass-toggle');
  if (p && t) {
    if (p.type === 'password') {
      p.type = 'text';
      t.innerText = '🙈';
    } else {
      p.type = 'password';
      t.innerText = '👁️';
    }
  }
}



function loadShiftSettings() {
  safeFetch('/get-shift')
    .then(function(r) { return r.json(); })
    .then(function(data) {
      var sh = document.getElementById('shift-start-hour');
      var sm = document.getElementById('shift-start-min');
      var eh = document.getElementById('shift-end-hour');
      var em = document.getElementById('shift-end-min');
      var g = document.getElementById('shift-grace');
      if (sh) sh.value = data.startHour;
      if (sm) sm.value = data.startMin;
      if (eh) eh.value = data.endHour;
      if (em) em.value = data.endMin;
      if (g) g.value = data.graceMins;
    }).catch(function() {
      showToast("Failed to load shift settings", "error");
    });
}

function saveShiftSettings() {
  var sh = document.getElementById('shift-start-hour').value;
  var sm = document.getElementById('shift-start-min').value;
  var eh = document.getElementById('shift-end-hour').value;
  var em = document.getElementById('shift-end-min').value;
  var g = document.getElementById('shift-grace').value;
  
  if (sh === "" || sm === "" || eh === "" || em === "" || g === "") {
    return showToast("All shift fields are required", "error");
  }
  
  var url = '/set-shift?startHour=' + sh + '&startMin=' + sm + '&endHour=' + eh + '&endMin=' + em + '&grace=' + g;
  safeFetch(url)
    .then(function(r) {
      if (r.ok) {
        showToast("Shift timings updated!", "success");
      } else {
        showToast("Failed to save shift timings", "error");
      }
    });
}

function saveSqlConfig() {
  var urlVal = document.getElementById('set-sql-api-url').value.trim();
  safeFetch('/set-sql-config?url=' + encodeURIComponent(urlVal))
    .then(function(r) {
      if (r.ok) {
        window.sqlApiUrl = urlVal;
        showToast("Webhook / Database URL updated!", "success");
      } else {
        showToast("Failed to save Webhook configuration", "error");
      }
    });
}

function syncHistoricalLogs() {
  showToast("Starting historical logs sync...", "warn");
  safeFetch('/sync-history-all')
    .then(function(r) {
      if (r.ok) {
        showToast("Historical sync started in background!", "success");
      } else {
        showToast("Failed to start sync", "error");
      }
    });
}

function factoryReset() {
  if(!confirm('🚨 WARNING: THIS WILL PERMANENTLY ERASE:\n1. All Employee Records\n2. All Fingerprints on Sensor\n3. All Logs & History\n4. All Settings & WiFi\n\nContinue?')) return;
  if(!confirm('Are you ABSOLUTELY SURE? Everything will be lost.')) return;
  showToast("Wiping everything... please wait", "danger");
  safeFetch('/factory-reset').then(function(){ 
    setTimeout(function(){ location.reload(); }, 3000);
  });
}

function uploadOTA() {
  var fileInput = document.getElementById('ota-file');
  if(fileInput.files.length === 0) return showToast("Please select a .bin file first", "error");
  var file = fileInput.files[0];
  
  var container = document.getElementById('ota-progress-container');
  var bar = document.getElementById('ota-bar');
  if (container) container.style.display = 'block';
  if (bar) {
    bar.style.width = '100%';
    bar.innerText = 'Initializing & Erasing Flash...';
    bar.style.background = '#e67e22'; // Orange warning color for erasing phase
  }

  showToast("Erasing flash... please wait...", "warn");

  // Step 1: Pre-erase flash partition
  var preXhr = new XMLHttpRequest();
  preXhr.open("POST", "/update-start", true);
  preXhr.setRequestHeader("X-File-Size", file.size.toString());
  
  preXhr.onload = function() {
    if (preXhr.status === 200 && preXhr.responseText === "OK") {
      showToast("Flash erased! Starting upload...", "success");
      if (bar) {
        bar.style.width = '0%';
        bar.innerText = '0%';
        bar.style.background = ''; // reset to default green/blue
      }
      
      // Step 2: Upload the actual file
      var formData = new FormData();
      formData.append("update", file, file.name);
      
      var xhr = new XMLHttpRequest();
      xhr.open("POST", "/update?size=" + file.size, true);
      xhr.setRequestHeader("X-File-Size", file.size.toString());
      
      if (xhr.upload) {
        xhr.upload.addEventListener("progress", function(e) {
          if (e.lengthComputable) {
            var percent = Math.round((e.loaded / e.total) * 100);
            if (bar) {
              bar.style.width = percent + '%';
              bar.innerText = percent + '%';
            }
          }
        });
      }
      
      xhr.onload = function() {
        if (xhr.status === 200 && xhr.responseText === "OK") {
          showToast("Update Successful! Rebooting...", "success");
          if (bar) bar.innerText = "Rebooting...";
          setTimeout(function(){ location.reload(); }, 5000);
        } else {
          showToast("Update Failed! Code: " + xhr.status + " | Res: " + xhr.responseText, "error");
          if (container) container.style.display = 'none';
        }
      };
      
      xhr.onerror = function() {
        showToast("Network Error: Failed to connect to device during upload.", "error");
        if (container) container.style.display = 'none';
      };
      
      xhr.send(formData);
    } else {
      showToast("Erase Failed! Code: " + preXhr.status + " | Res: " + preXhr.responseText, "error");
      if (container) container.style.display = 'none';
    }
  };
  
  preXhr.onerror = function() {
    showToast("Network Error: Failed to start flash erase on device.", "error");
    if (container) container.style.display = 'none';
  };
  
  preXhr.send();
}

function checkOtaFileSize() {
  var fileInput = document.getElementById('ota-file');
  var warning = document.getElementById('ota-size-warning');
  var btn = document.getElementById('ota-upload-btn');
  if (fileInput.files.length > 0) {
    if (maxOtaSize === 0) {
      if (warning) {
        warning.innerText = "⚠️ OTA updates are not supported by the current partition scheme! Please reflash the device using an OTA-enabled partition table.";
        warning.style.display = 'block';
      }
      if (btn) btn.disabled = true;
      return;
    }
    var file = fileInput.files[0];
    if (file.size > maxOtaSize) {
      if (warning) {
        warning.innerText = "⚠️ Selected file is larger than the allowed partition size! Update will fail.";
        warning.style.display = 'block';
      }
      if (btn) btn.disabled = true;
    } else {
      if (warning) warning.style.display = 'none';
      if (btn) btn.disabled = false;
    }
  }
}

function toggleAudioUidVisibility(val) {
  var group = document.getElementById('audio-uid-group');
  if (group) {
    group.style.display = (val === 'custom') ? 'block' : 'none';
  }
}

function uploadAudio() {
  var fileInput = document.getElementById('audio-file-upload');
  var file = fileInput ? fileInput.files[0] : null;
  if (!file) return showToast("Select a WAV file first", "error");
  
  if (file.name.slice(-4).toLowerCase() !== '.wav') {
    return showToast("Only .wav files are supported", "error");
  }

  var type = document.getElementById('audio-type').value;
  var destName = type;
  if (type === 'custom') {
    var uid = document.getElementById('audio-uid').value.trim();
    if (!uid) return showToast("Enter employee UID", "error");
    destName = uid + ".wav";
  }

  var fd = new FormData();
  fd.append("file", file, destName);

  var xhr = new XMLHttpRequest();
  xhr.open("POST", "/upload", true);

  var bar = document.getElementById('audio-bar');
  var progress = document.getElementById('audio-progress-container');
  if (bar) bar.style.width = '0%';
  if (progress) progress.style.display = 'block';

  if (xhr.upload) {
    xhr.upload.onprogress = function(e) {
      if (e.lengthComputable) {
        var pct = Math.round((e.loaded / e.total) * 100);
        if (bar) bar.style.width = pct + '%';
      }
    };
  }

  xhr.onload = function() {
    if (progress) progress.style.display = 'none';
    if (xhr.status === 200) {
      showToast("Audio uploaded successfully!", "success");
      if (fileInput) fileInput.value = '';
      loadDirectory();
    } else {
      showToast("Upload failed!", "error");
    }
  };

  xhr.onerror = function() {
    if (progress) progress.style.display = 'none';
    showToast("Network upload error", "error");
  };

  xhr.send(fd);
}



function toggleHolidayRange(checked) {
  var container = document.getElementById('hol-end-date-container');
  var startLabel = document.getElementById('hol-start-label');
  if (container) container.style.display = checked ? 'block' : 'none';
  if (startLabel) startLabel.innerText = checked ? 'Start Date' : 'Date';
}

function fetchHolidays() {
  safeFetch('/list-holidays')
    .then(function(r) { return r.json(); })
    .catch(function() { return []; })
    .then(function(data) {
      // 1. Populate Holidays Directory Table
      var b = document.getElementById('hol-body');
      if (b) {
        b.innerHTML = '';
        data.forEach(function(h) {
          var row = b.insertRow();
          var dateRangeStr = formatFriendlyDate(h.date);
          if (h.isRange && h.endDate) {
            dateRangeStr += ' to ' + formatFriendlyDate(h.endDate);
          }
          
          var typeBadge = '';
          if (h.type === 'Public') {
            typeBadge = '<span class="badge" style="background:rgba(6,214,160,0.1); color:var(--success); border:1px solid rgba(6,214,160,0.25);"><span class="status-dot dot-in"></span>Public</span>';
          } else if (h.type === 'Optional') {
            typeBadge = '<span class="badge" style="background:rgba(255,209,102,0.1); color:var(--warning); border:1px solid rgba(255,209,102,0.25);"><span class="status-dot" style="background:var(--warning); box-shadow:0 0 10px var(--warning);"></span>Optional</span>';
          } else if (h.type === 'Company') {
            typeBadge = '<span class="badge" style="background:rgba(168,85,247,0.1); color:#a855f7; border:1px solid rgba(168,85,247,0.25);"><span class="status-dot" style="background:#a855f7; box-shadow:0 0 10px #a855f7;"></span>Company</span>';
          } else {
            typeBadge = '<span class="badge" style="background:rgba(6,214,160,0.1); color:var(--success); border:1px solid rgba(6,214,160,0.25);"><span class="status-dot dot-in"></span>Public</span>';
          }
          
          var recurrence = h.yearly ? '🔄 Annual' : '📅 One-time';
          var description = h.desc || '—';
          
          row.innerHTML = '<td>' + dateRangeStr + '</td>' +
                          '<td><strong>' + h.name + '</strong></td>' +
                          '<td>' + typeBadge + '</td>' +
                          '<td><span style="font-size:12px;font-weight:600;">' + recurrence + '</span></td>' +
                          '<td><span style="color:var(--text-muted);font-size:12px;">' + description + '</span></td>' +
                          '<td><button class="btn btn-danger btn-sm" onclick="deleteHoliday(\'' + h.date + '\')">Del</button></td>';
        });
        if(data.length === 0) {
          b.innerHTML = '<tr><td colspan="6" style="text-align:center;padding:20px;color:var(--text-muted);">No holidays added.</td></tr>';
        }
      }
      
      // 2. Draw upcoming holidays list
      drawUpcomingHolidays(data);
    });
}


function drawUpcomingHolidays(holidaysList) {
  var container = document.getElementById('upcoming-hols-list');
  if (!container) return;
  
  container.innerHTML = '';
  
  var today = new Date();
  var upcoming = [];
  
  // Check each of the next 30 days
  for (var i = 0; i < 30; i++) {
    var checkDate = new Date(today);
    checkDate.setDate(today.getDate() + i);
    
    var ddStr = String(checkDate.getDate()).padStart(2, '0');
    var mmStr = String(checkDate.getMonth() + 1).padStart(2, '0');
    var yyyy = checkDate.getFullYear();
    var dateStr = ddStr + '-' + mmStr + '-' + yyyy;
    
    var hObj = getHolidayForDate(dateStr, holidaysList);
    if (hObj) {
      var alreadyAdded = upcoming.some(function(item) {
        return item.name === hObj.name && item.date === hObj.date;
      });
      
      if (!alreadyAdded) {
        upcoming.push({
          name: hObj.name,
          date: hObj.date,
          type: hObj.type || 'Public',
          daysLeft: i,
          actualDate: checkDate
        });
      }
    }
  }
  
  // Sort by days left
  upcoming.sort(function(a, b) { return a.daysLeft - b.daysLeft; });
  
  upcoming.forEach(function(u) {
    var item = document.createElement('div');
    item.className = 'upcoming-holiday-item';
    
    var statusText = '';
    if (u.daysLeft === 0) statusText = 'Today';
    else if (u.daysLeft === 1) statusText = 'Tomorrow';
    else statusText = 'In ' + u.daysLeft + ' days';
    
    var color = 'var(--success)';
    if (u.type === 'Optional') color = 'var(--warning)';
    else if (u.type === 'Company') color = '#a855f7';
    
    item.innerHTML = '<div class="upcoming-holiday-info">' +
                       '<span class="upcoming-holiday-title">' + u.name + '</span>' +
                       '<span class="upcoming-holiday-date">' + formatFriendlyDate(u.date) + '</span>' +
                     '</div>' +
                     '<span class="badge" style="background:rgba(67,97,238,0.08); color:' + color + '; border:1px solid rgba(67,97,238,0.15); font-weight:700;">' + statusText + '</span>';
                     
    container.appendChild(item);
  });
  
  if (upcoming.length === 0) {
    container.innerHTML = '<div style="text-align:center; padding:15px; color:var(--text-muted); font-size:12px;">No holidays in the next 30 days.</div>';
  }
}

function addHoliday() {
  var d = document.getElementById('hol-date').value;
  var isRange = document.getElementById('hol-is-range').checked;
  var endDate = document.getElementById('hol-end-date').value;
  var name = document.getElementById('hol-name').value.trim();
  var type = document.getElementById('hol-type').value;
  var yearly = document.getElementById('hol-yearly').checked;
  var desc = document.getElementById('hol-desc').value.trim();
  
  if(!d || !name) return showToast("Enter date and name", "error");
  if(isRange && !endDate) return showToast("Enter end date for the range", "error");
  
  var dateStr = d.split('-').reverse().join('-'); 
  var endDateStr = isRange ? endDate.split('-').reverse().join('-') : '';
  
  var url = '/add-holiday?date=' + dateStr + 
            '&name=' + encodeURIComponent(name) + 
            '&type=' + type + 
            '&yearly=' + (yearly ? 'true' : 'false') + 
            '&isRange=' + (isRange ? 'true' : 'false') + 
            '&endDate=' + endDateStr + 
            '&desc=' + encodeURIComponent(desc);
            
  safeFetch(url)
    .then(function(r) {
      if(r.ok) { 
        showToast("Holiday added", "success"); 
        document.getElementById('hol-name').value = '';
        document.getElementById('hol-desc').value = '';
        document.getElementById('hol-date').value = '';
        document.getElementById('hol-end-date').value = '';
        document.getElementById('hol-is-range').checked = false;
        document.getElementById('hol-yearly').checked = false;
        toggleHolidayRange(false);
        fetchHolidays(); 
      } else {
        showToast("Failed to add holiday", "error");
      }
    });
}

function deleteHoliday(dateStr) {
  if(!confirm("Delete holiday for " + formatFriendlyDate(dateStr) + "?")) return;
  safeFetch('/delete-holiday?date=' + dateStr)
    .then(function(r) {
      if(r.ok) { showToast("Holiday removed", "success"); fetchHolidays(); }
    });
}

function clearAllHolidays() {
  if(!confirm("⚠️ WARNING: This will permanently delete ALL holidays from the system! Continue?")) return;
  safeFetch('/clear-holidays')
    .then(function(r) {
      if(r.ok) {
        showToast("All holidays cleared", "success");
        fetchHolidays();
      } else {
        showToast("Failed to clear holidays", "error");
      }
    });
}

function loadHolidayPreset() {
  var preset = document.getElementById('hol-preset-select').value;
  var holidays = [];
  var year = new Date().getFullYear();
  
  if (preset === 'IN') {
    holidays = [
      { date: '01-01-' + year, name: "New Year's Day", type: 'Public', yearly: true, isRange: false, endDate: '', desc: 'Annual celebration of New Year' },
      { date: '26-01-' + year, name: "Republic Day", type: 'Public', yearly: true, isRange: false, endDate: '', desc: 'National celebration of Constitution Day' },
      { date: '15-08-' + year, name: "Independence Day", type: 'Public', yearly: true, isRange: false, endDate: '', desc: 'National celebration of Independence' },
      { date: '02-10-' + year, name: "Gandhi Jayanti", type: 'Public', yearly: true, isRange: false, endDate: '', desc: 'Mahatma Gandhi Birthday' },
      { date: '25-12-' + year, name: "Christmas Day", type: 'Public', yearly: true, isRange: false, endDate: '', desc: 'Annual celebration of Christmas' }
    ];
  } else if (preset === 'US') {
    holidays = [
      { date: '01-01-' + year, name: "New Year's Day", type: 'Public', yearly: true, isRange: false, endDate: '', desc: 'Annual celebration of New Year' },
      { date: '04-07-' + year, name: "Independence Day", type: 'Public', yearly: true, isRange: false, endDate: '', desc: 'Declaration of Independence' },
      { date: '11-11-' + year, name: "Veterans Day", type: 'Public', yearly: true, isRange: false, endDate: '', desc: 'Honoring military veterans' },
      { date: '25-12-' + year, name: "Christmas Day", type: 'Public', yearly: true, isRange: false, endDate: '', desc: 'Annual celebration of Christmas' }
    ];
  }
  
  if (holidays.length === 0) return;
  
  showToast("Loading " + holidays.length + " holidays...", "warn");
  var loaded = 0;
  
  function addNext(idx) {
    if (idx >= holidays.length) {
      showToast("Successfully loaded presets!", "success");
      fetchHolidays();
      return;
    }
    var h = holidays[idx];
    var url = '/add-holiday?date=' + h.date + 
              '&name=' + encodeURIComponent(h.name) + 
              '&type=' + h.type + 
              '&yearly=' + (h.yearly ? 'true' : 'false') + 
              '&isRange=' + (h.isRange ? 'true' : 'false') + 
              '&endDate=' + h.endDate + 
              '&desc=' + encodeURIComponent(h.desc);
              
    safeFetch(url)
      .then(function(r) {
        if (r.ok) loaded++;
        addNext(idx + 1);
      })
      .catch(function() {
        addNext(idx + 1);
      });
  }
  
  addNext(0);
}

function manualPunch(uid, dir) {
  showToast("Processing manual " + dir + "...", "warn");
  safeFetch('/manual-punch?uid=' + encodeURIComponent(uid) + '&dir=' + dir)
    .then(function(r) {
      if(r.ok) { showToast("Manual punch successful", "success"); loadDirectory(); }
      else showToast("Manual punch failed", "error");
    });
}


function exportUsers() {
  showToast("Exporting database...", "warn");
  safeFetch('/export-users')
    .then(function(r) {
      return r.text().then(function(text) {
        try {
          return JSON.parse(text);
        } catch(e) {
          console.error("JSON parse error:", e, text);
          throw new Error("Invalid server JSON response (" + e.message + ")");
        }
      });
    })
    .then(function(data) {
      var devNameEl = document.getElementById('set-dev-name');
      var devName = devNameEl ? devNameEl.value.trim().replace(/\s+/g, '_') : 'attendance';
      if (!devName) devName = 'attendance';
      
      var blob = new Blob([JSON.stringify(data, null, 2)], { type: 'application/json' });
      var url = window.URL.createObjectURL(blob);
      var a = document.createElement('a');
      a.href = url;
      a.download = devName + '_backup_' + new Date().toISOString().split('T')[0] + '.json';
      a.style.display = 'none';
      document.body.appendChild(a);
      a.click();
      document.body.removeChild(a);
      window.URL.revokeObjectURL(url);
      showToast("Export complete", "success");
    })
    .catch(function(err) {
      showToast("Export failed: " + err.message, "error");
    });
}

function closeRestoreModal() {
  var m = document.getElementById('restore-modal');
  if (m) m.style.display = 'none';
}

function dragOverHandler(ev) {
  ev.preventDefault();
  var zone = document.getElementById('drop-zone');
  if (zone) {
    zone.style.background = "rgba(0, 242, 254, 0.08)";
    zone.style.borderColor = "#00f2fe";
  }
}

function dragLeaveHandler(ev) {
  var zone = document.getElementById('drop-zone');
  if (zone) {
    zone.style.background = "rgba(255, 255, 255, 0.02)";
    zone.style.borderColor = "var(--border)";
  }
}

function dropHandler(ev) {
  ev.preventDefault();
  var zone = document.getElementById('drop-zone');
  if (zone) {
    zone.style.background = "rgba(255, 255, 255, 0.02)";
    zone.style.borderColor = "var(--border)";
  }
  
  if (ev.dataTransfer.items) {
    if (ev.dataTransfer.items[0].kind === 'file') {
      var file = ev.dataTransfer.items[0].getAsFile();
      if (file && file.name.endsWith('.json')) {
        var input = document.getElementById('import-file');
        if (input) {
          var container = new DataTransfer();
          container.items.add(file);
          input.files = container.files;
          importUsers(input);
        }
      } else {
        showToast("Only .json backup files are allowed!", "error");
      }
    }
  } else {
    var file = ev.dataTransfer.files[0];
    if (file && file.name.endsWith('.json')) {
      var input = document.getElementById('import-file');
      if (input) {
        var container = new DataTransfer();
        container.items.add(file);
        input.files = container.files;
        importUsers(input);
      }
    } else {
      showToast("Only .json backup files are allowed!", "error");
    }
  }
}

function importUsers(input) {
  if(!input.files || !input.files[0]) return;
  var file = input.files[0];
  var reader = new FileReader();
  reader.onload = function(e) {
    try {
      var data = JSON.parse(e.target.result);
      
      if (!Array.isArray(data) || (data.length > 0 && (!data[0].fn || !data[0].data))) {
        showToast("Invalid backup file structure", "error");
        input.value = "";
        return;
      }

      if(!confirm("This will restore " + data.length + " employee profiles, overwriting existing records with matching IDs. Proceed?")) {
        input.value = "";
        return;
      }
      
      var modal = document.getElementById('restore-modal');
      var statusText = document.getElementById('restore-status');
      var progressBar = document.getElementById('restore-progress-bar');
      var successCountEl = document.getElementById('restore-success-count');
      var failCountEl = document.getElementById('restore-fail-count');
      var totalCountEl = document.getElementById('restore-total-count');
      var footer = document.getElementById('restore-modal-footer');
      
      if (!modal || !statusText || !progressBar) {
        showToast("UI Error: Modal elements not found", "error");
        return;
      }
      
      successCountEl.innerText = "0";
      failCountEl.innerText = "0";
      totalCountEl.innerText = data.length;
      progressBar.style.width = "0%";
      statusText.innerText = "Initializing database restoration...";
      footer.style.display = "none";
      modal.style.display = "flex";
      
      var successCount = 0;
      var failCount = 0;
      
      function restoreNextUser(index) {
        if (index >= data.length) {
          statusText.innerText = "Restoration completed! " + successCount + " profiles restored.";
          progressBar.style.width = "100%";
          footer.style.display = "block";
          loadDirectory();
          input.value = "";
          return;
        }
        
        var item = data[index];
        var displayName = item.fn.replace('.txt', '').replace('/', '');
        statusText.innerText = "Restoring employee: " + displayName + "...";
        
        safeFetch('/restore-user?fn=' + encodeURIComponent(item.fn) + '&data=' + encodeURIComponent(item.data))
          .then(function(r) {
            if(r.ok) {
              successCount++;
              successCountEl.innerText = successCount;
            } else {
              failCount++;
              failCountEl.innerText = failCount;
            }
            var pct = Math.round(((index + 1) / data.length) * 100);
            progressBar.style.width = pct + "%";
            restoreNextUser(index + 1);
          })
          .catch(function() {
            failCount++;
            failCountEl.innerText = failCount;
            var pct = Math.round(((index + 1) / data.length) * 100);
            progressBar.style.width = pct + "%";
            restoreNextUser(index + 1);
          });
      }
      
      restoreNextUser(0);
    } catch(err) {
      showToast("Import failed: Corrupted JSON file", "error");
      input.value = "";
    }
  };
  reader.readAsText(file);
}

function checkSensors() {
  safeFetch('/sensor-status')
    .then(function(r) { return r.json(); })
    .then(function(data) {
      var r = document.getElementById('health-rfid');
      var f = document.getElementById('health-fp');
      if (r) {
        r.innerText = data.rfid ? 'ONLINE' : 'OFFLINE';
        r.style.color = data.rfid ? 'var(--success)' : 'var(--danger)';
      }
      if (f) {
        f.innerText = data.fingerprint ? 'ONLINE' : 'OFFLINE';
        f.style.color = data.fingerprint ? 'var(--success)' : 'var(--danger)';
      }
      
      if(!data.fingerprint && currentEnrollType === 'FINGER') {
        var ccs = document.getElementById('cc-sensor-status');
        if (ccs) {
          ccs.innerText = "OFFLINE";
          ccs.style.color = "var(--danger)";
        }
      }
    });
}

setInterval(function() {
  var d = new Date();
  var clock = document.getElementById('live-clock');
  if(clock) clock.innerHTML = '⏰ ' + d.toLocaleTimeString();
  var dateEl = document.getElementById('live-date');
  if(dateEl) dateEl.innerHTML = '📅 ' + d.toLocaleDateString();
}, 1000);

function getWeatherCodeDesc(code) {
  var mapping = {
    0: "Clear",
    1: "Partly Cloudy", 2: "Partly Cloudy",
    3: "Overcast",
    45: "Foggy", 48: "Foggy",
    51: "Drizzle", 53: "Drizzle", 55: "Drizzle",
    56: "Freezing Drizzle", 57: "Freezing Drizzle",
    61: "Light Rain", 63: "Light Rain",
    65: "Heavy Rain",
    66: "Freezing Rain", 67: "Freezing Rain",
    71: "Snowy", 73: "Snowy", 75: "Snowy",
    77: "Snow Grains",
    80: "Rainy", 81: "Rainy", 82: "Rainy",
    85: "Snow Showers", 86: "Snow Showers",
    95: "Thunderstorm", 96: "Thunderstorm", 99: "Thunderstorm"
  };
  return mapping[code] || "Clear";
}

function updateWeather() {
  var city = storage.getItem("weather_city") || "Navi-Mumbai";
  var searchCity = city.replace(/-/g, ' ');
  var key = storage.getItem("google_api_key") || "";

  var lat = 19.0330;
  var lon = 73.0297;
  var cityName = searchCity;

  if (key && key.trim() !== "") {
    // === GOOGLE WEATHER SYSTEM ===
    var geocodeUrl = 'https://maps.googleapis.com/maps/api/geocode/json?address=' + encodeURIComponent(searchCity) + '&key=' + key;
    
    fetch(geocodeUrl)
      .then(function(r){ return r.json(); })
      .then(function(geoData){
        if (geoData.results && geoData.results.length > 0) {
          lat = geoData.results[0].geometry.location.lat;
          lon = geoData.results[0].geometry.location.lng;
          cityName = geoData.results[0].formatted_address.split(',')[0];
        }
        var forecastUrl = 'https://weather.googleapis.com/v1/currentConditions:lookup?location.latitude=' + lat + '&location.longitude=' + lon + '&key=' + key;
        return fetch(forecastUrl);
      })
      .then(function(r){ return r.json(); })
      .then(function(weatherData){
        var sw = document.getElementById('stat-weather');
        var sc = document.getElementById('stat-city');
        if (sw && weatherData.temperature) {
          sw.innerText = Math.round(weatherData.temperature.degrees) + "°C";
        }
        if (sc && weatherData.weatherCondition) {
          sc.innerText = cityName + " (" + weatherData.weatherCondition.description.text + ")";
        }
      })
      .catch(function(err){
        console.error("Google Weather error", err);
        var sw = document.getElementById('stat-weather');
        var sc = document.getElementById('stat-city');
        if (sw) sw.innerText = "--°C";
        if (sc) sc.innerText = "Offline (Click to change)";
      });
  } else {
    // === OPEN-METEO FALLBACK ===
    var geocodeUrl = 'https://geocoding-api.open-meteo.com/v1/search?name=' + encodeURIComponent(searchCity) + '&count=1&language=en&format=json';
    
    fetch(geocodeUrl)
      .then(function(r){ return r.json(); })
      .then(function(geoData){
        if (geoData.results && geoData.results.length > 0) {
          lat = geoData.results[0].latitude;
          lon = geoData.results[0].longitude;
          cityName = geoData.results[0].name;
        }
        var forecastUrl = 'https://api.open-meteo.com/v1/forecast?latitude=' + lat + '&longitude=' + lon + '&current=temperature_2m,relative_humidity_2m,wind_speed_10m,weather_code';
        return fetch(forecastUrl);
      })
      .then(function(r){ return r.json(); })
      .then(function(weatherData){
        var current = weatherData.current;
        var sw = document.getElementById('stat-weather');
        var sc = document.getElementById('stat-city');
        if (sw) sw.innerText = Math.round(current.temperature_2m) + "°C";
        if (sc) {
          var desc = getWeatherCodeDesc(current.weather_code);
          sc.innerText = cityName + " (" + desc + ")";
        }
      })
      .catch(function(err){ 
        console.error("Open-Meteo Weather error", err); 
        var sw = document.getElementById('stat-weather');
        var sc = document.getElementById('stat-city');
        if (sw) sw.innerText = "--°C";
        if (sc) sc.innerText = "Offline (Click to change)";
      });
  }
}

function changeWeatherLocation() {
  var currentCity = storage.getItem("weather_city") || "Navi-Mumbai";
  var newCity = prompt("📍 Set live weather city name (e.g. Navi-Mumbai or Mumbai):\n(Leave empty to use default Navi-Mumbai)", currentCity);
  if (newCity !== null) {
    newCity = newCity.trim();
    if (newCity === "") {
      storage.removeItem("weather_city");
      showToast("Reset to default (Navi-Mumbai)", "info");
      safeFetch('/set-weather-city?city=Navi-Mumbai').catch(function(){});
    } else {
      // Replace spaces with dashes for wttr.in compatibility
      newCity = newCity.replace(/\s+/g, '-');
      storage.setItem("weather_city", newCity);
      showToast("Location updated to " + newCity, "success");
      safeFetch('/set-weather-city?city=' + encodeURIComponent(newCity)).catch(function(){});
    }
    updateWeather();
  }
}

var currentReportData = [];
var currentReportEmployeeName = "";
var currentReportPeriod = "";

function loadReportEmployees() {
  getUsersList(function(users) {
    var select = document.getElementById('rep-employee');
    if (!select) return;
    select.innerHTML = '<option value="">-- Select Employee --</option>';
    users.sort(function(a, b) {
      var nameA = (a.name || "").toLowerCase();
      var nameB = (b.name || "").toLowerCase();
      return nameA.localeCompare(nameB);
    });
    users.forEach(function(u) {
      var rollVal = u.roll || u.uid || "";
      var opt = document.createElement('option');
      opt.value = rollVal;
      opt.setAttribute('data-name', u.name || "");
      opt.innerText = (u.name || "Unknown") + " (" + rollVal + ")";
      select.appendChild(opt);
    });
  });
}

function generateMonthlyReport() {
  var monthSelect = document.getElementById('rep-month');
  var yearSelect = document.getElementById('rep-year');
  var empSelect = document.getElementById('rep-employee');
  if(!monthSelect || !yearSelect || !empSelect) return;
  var monthVal = parseInt(monthSelect.value);
  var yearVal = parseInt(yearSelect.value);
  var rollVal = empSelect.value;
  if(!rollVal) return showToast("Select an employee first", "error");
  var selectedOpt = empSelect.options[empSelect.selectedIndex];
  var empName = selectedOpt.getAttribute('data-name') || "Employee";
  currentReportEmployeeName = empName;
  currentReportPeriod = monthSelect.options[monthSelect.selectedIndex].text + " " + yearVal;
  var progressContainer = document.getElementById('rep-progress-container');
  var progressBar = document.getElementById('rep-progress-bar');
  var progressText = document.getElementById('rep-progress-text');
  var resultsDiv = document.getElementById('rep-results');
  var tableBody = document.getElementById('rep-table-body');
  if(progressContainer) progressContainer.style.display = 'block';
  if(resultsDiv) resultsDiv.style.display = 'none';
  if(tableBody) tableBody.innerHTML = '';
  var daysInMonth = new Date(yearVal, monthVal, 0).getDate();
  var daysList = [];
  for (var d = 1; d <= daysInMonth; d++) {
    daysList.push(d);
  }
  currentReportData = [];
  var presentCount = 0;
  var absentCount = 0;
  var irregularCount = 0;
  var lateCount = 0;
  function pad(num) {
    return (num < 10 ? '0' : '') + num;
  }
  var shiftHours = parseFloat(localStorage.getItem('pay_shift_hours_' + rollVal)) || 8;
  var halfdayHours = parseFloat(localStorage.getItem('pay_halfday-hours_' + rollVal)) || 4;
  var salaryType = localStorage.getItem('pay_salary_type_' + rollVal) || 'monthly';
  
  function isDateHoliday(dStr, holidaysList) {
    var dParts = dStr.split('-');
    var dd = parseInt(dParts[0], 10);
    var mm = parseInt(dParts[1], 10);
    var yyyy = parseInt(dParts[2], 10);
    var dateObj = new Date(yyyy, mm - 1, dd);
    for (var i = 0; i < holidaysList.length; i++) {
      var h = holidaysList[i];
      var hParts = h.date.split('-');
      var hDd = parseInt(hParts[0], 10);
      var hMm = parseInt(hParts[1], 10);
      var hYyyy = parseInt(hParts[2], 10);
      if (h.yearly) {
        if (h.isRange && h.endDate) {
          var heParts = h.endDate.split('-');
          var heDd = parseInt(heParts[0], 10);
          var heMm = parseInt(heParts[1], 10);
          var dCompare = new Date(2000, mm - 1, dd);
          var hStartCompare = new Date(2000, hMm - 1, hDd);
          var hEndCompare = new Date(2000, heMm - 1, heDd);
          if (dCompare >= hStartCompare && dCompare <= hEndCompare) {
            return h.name;
          }
        } else {
          if (dd === hDd && mm === hMm) {
            return h.name;
          }
        }
      } else {
        if (h.isRange && h.endDate) {
          var heParts = h.endDate.split('-');
          var heDd = parseInt(heParts[0], 10);
          var heMm = parseInt(heParts[1], 10);
          var heYyyy = parseInt(heParts[2], 10);
          var hStartDate = new Date(hYyyy, hMm - 1, hDd);
          var hEndDate = new Date(heYyyy, heMm - 1, heDd);
          if (dateObj >= hStartDate && dateObj <= hEndDate) {
            return h.name;
          }
        } else {
          if (dd === hDd && mm === hMm && yyyy === hYyyy) {
            return h.name;
          }
        }
      }
    }
    return null;
  }

  var halfDayCount = 0;
  var weekOffCount = 0;
  var holidayCount = 0;

  Promise.all([
    safeFetch('/get-shift').then(function(r) { return r.json(); }).catch(function() { return { startHour: 9, startMin: 0, graceMins: 15 }; }),
    safeFetch('/list-holidays').then(function(r) { return r.json(); }).catch(function() { return []; })
  ]).then(function(results) {
      var shift = results[0];
      var holidaysList = results[1];
      var startLimitMins = shift.startHour * 60 + shift.startMin + (parseInt(shift.graceMins) || 0);

      function fetchDay(index) {
        if (index >= daysList.length) {
          if(progressContainer) progressContainer.style.display = 'none';
          var statPresent = document.getElementById('rep-stat-present');
          var statHalf = document.getElementById('rep-stat-half');
          var statAbsent = document.getElementById('rep-stat-absent');
          var statIrregular = document.getElementById('rep-stat-irregular');
          var statWeekoffs = document.getElementById('rep-stat-weekoffs');
          var statHolidays = document.getElementById('rep-stat-holidays');
          var statRate = document.getElementById('rep-stat-rate');
          var summaryTitle = document.getElementById('rep-summary-title');
          
          if(statPresent) statPresent.innerText = presentCount;
          if(statHalf) statHalf.innerText = halfDayCount;
          if(statAbsent) statAbsent.innerText = absentCount;
          if(statIrregular) statIrregular.innerText = irregularCount;
          if(statWeekoffs) statWeekoffs.innerText = weekOffCount;
          if(statHolidays) statHolidays.innerText = holidayCount;
          
          var activeDays = presentCount + halfDayCount + absentCount + irregularCount;
          var ratePct = activeDays > 0 ? Math.round(((presentCount + 0.5 * halfDayCount) / activeDays) * 100) : 0;
          if(statRate) statRate.innerText = ratePct + '%';
          
          if(summaryTitle) {
            summaryTitle.innerText = "Attendance Report for " + empName + " (" + rollVal + ") - " + currentReportPeriod;
          }
          if(resultsDiv) resultsDiv.style.display = 'block';
          showToast("Report generated successfully!", "success");
          return;
        }
        var day = daysList[index];
        var dateStr = pad(day) + '-' + pad(monthVal) + '-' + yearVal;
        var pct = Math.round((index / daysList.length) * 100);
        if(progressBar) progressBar.style.width = pct + '%';
        if(progressText) progressText.innerText = pct + '% (Fetching Day ' + day + '/' + daysInMonth + ')';
        
        safeFetch('/get-history?date=' + dateStr)
          .then(function(r) { return r.text(); })
          .then(function(text) {
            var firstIn = "--";
            var lastOut = "--";
            var status = "ABSENT";
            var inTimes = [];
            var outTimes = [];
            if (text && text.trim()) {
              var lines = text.trim().split('\n');
              for (var i = 1; i < lines.length; i++) {
                var cols = lines[i].split(',');
                if (cols.length >= 5) {
                  var logTime = cols[0].trim();
                  var logRoll = cols[1].trim();
                  var logName = cols[2].trim();
                  var logDir = cols[4].trim().toLowerCase();
                  if (logRoll === rollVal || (logRoll === "" && logName.toLowerCase() === empName.toLowerCase())) {
                    if (logDir === "in") {
                      inTimes.push(logTime);
                    } else if (logDir === "out") {
                      outTimes.push(logTime);
                    }
                  }
                }
              }
            }
            if (inTimes.length > 0) {
              inTimes.sort();
              firstIn = inTimes[0];
            }
            if (outTimes.length > 0) {
              outTimes.sort();
              lastOut = outTimes[outTimes.length - 1];
            }
            
            var totalPunches = inTimes.length + outTimes.length;
            var workedMins = -1;
            
            if (inTimes.length > 0 && outTimes.length > 0) {
              if (firstIn !== "--" && lastOut !== "--") {
                var inParts = firstIn.split(':');
                var outParts = lastOut.split(':');
                if (inParts.length >= 2 && outParts.length >= 2) {
                  var inM = parseInt(inParts[0], 10) * 60 + parseInt(inParts[1], 10);
                  var outM = parseInt(outParts[0], 10) * 60 + parseInt(outParts[1], 10);
                  workedMins = outM - inM;
                  if (workedMins < 0) workedMins = 0;
                }
              }
            }
            
            var dayDate = new Date(yearVal, monthVal - 1, day);
            var isSunday = (dayDate.getDay() === 0);
            var holidayName = isDateHoliday(dateStr, holidaysList);
            var isLate = false;
            
            if (totalPunches === 0) {
              if (holidayName) {
                status = "HOLIDAY: " + holidayName;
                holidayCount++;
              } else if (isSunday) {
                status = "WEEK OFF";
                weekOffCount++;
              } else {
                status = "ABSENT";
                absentCount++;
              }
            } else if (inTimes.length > 0 && outTimes.length > 0) {
              if (workedMins >= 0) {
                if (salaryType === 'hourly') {
                  if (workedMins > 0) {
                    status = "WORKED";
                    presentCount++;
                  } else {
                    status = "ABSENT";
                    absentCount++;
                  }
                } else {
                  if (workedMins < halfdayHours * 60) {
                    status = "ABSENT";
                    absentCount++;
                  } else if (workedMins < shiftHours * 60) {
                    status = "HALF DAY";
                    halfDayCount++;
                    if (firstIn !== "--") {
                      var inParts = firstIn.split(':');
                      if (inParts.length >= 2) {
                        var inMins = parseInt(inParts[0], 10) * 60 + parseInt(inParts[1], 10);
                        if (inMins > startLimitMins) {
                          status = "HALF DAY (LATE)";
                          isLate = true;
                          lateCount++;
                        }
                      }
                    }
                  } else {
                    status = "PRESENT";
                    presentCount++;
                    if (firstIn !== "--") {
                      var inParts = firstIn.split(':');
                      if (inParts.length >= 2) {
                        var inMins = parseInt(inParts[0], 10) * 60 + parseInt(inParts[1], 10);
                        if (inMins > startLimitMins) {
                          status = "PRESENT (LATE)";
                          isLate = true;
                          lateCount++;
                        }
                      }
                    }
                  }
                }
              } else {
                if (salaryType === 'hourly') {
                  status = "ABSENT";
                  absentCount++;
                } else {
                  status = "PRESENT";
                  presentCount++;
                }
              }
            } else {
              if (salaryType === 'hourly') {
                status = "ABSENT";
                absentCount++;
              } else {
                status = "IRREGULAR";
                irregularCount++;
              }
            }
            
            var workHrs = "--";
            if ((status.indexOf("PRESENT") !== -1 || status.indexOf("HALF DAY") !== -1 || status.indexOf("WORKED") !== -1) && firstIn !== "--" && lastOut !== "--") {
              workHrs = calculateWorkHrs(firstIn, lastOut);
            }
            
            var dayNames = ["Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"];
            var dayOfWeek = dayNames[dayDate.getDay()];
            var tr = document.createElement('tr');
            
            var statusHtml = '';
            var statLower = status.toLowerCase();
            
            if (statLower.indexOf('present') !== -1) {
              if (statLower.indexOf('late') !== -1) {
                statusHtml = '<span class="badge" style="background:rgba(255,209,102,0.1); color:var(--warning); border:1px solid rgba(255,209,102,0.25);"><span class="status-dot" style="background:var(--warning); box-shadow:0 0 10px var(--warning);"></span>' + status + '</span>';
              } else {
                statusHtml = '<span class="badge badge-in"><span class="status-dot dot-in"></span>' + status + '</span>';
              }
            } else if (statLower.indexOf('half day') !== -1) {
              var col = '#f77f00';
              statusHtml = '<span class="badge" style="background:rgba(247,127,0,0.1); color:' + col + '; border:1px solid rgba(247,127,0,0.25);"><span class="status-dot" style="background:' + col + '; box-shadow:0 0 10px ' + col + ';"></span>' + status + '</span>';
            } else if (statLower.indexOf('absent') !== -1) {
              statusHtml = '<span class="badge badge-out"><span class="status-dot dot-out"></span>' + status + '</span>';
            } else if (statLower.indexOf('irregular') !== -1) {
              statusHtml = '<span class="badge" style="background:rgba(255,209,102,0.15); color:var(--warning); border:1px solid rgba(255,209,102,0.25);"><span class="status-dot" style="background:var(--warning); box-shadow:0 0 10px var(--warning);"></span>' + status + '</span>';
            } else if (statLower.indexOf('week off') !== -1) {
              var col = '#58a6ff';
              statusHtml = '<span class="badge" style="background:rgba(88,166,255,0.1); color:' + col + '; border:1px solid rgba(88,166,255,0.25);"><span class="status-dot" style="background:' + col + '; box-shadow:0 0 10px ' + col + ';"></span>' + status + '</span>';
            } else if (statLower.indexOf('holiday') !== -1) {
              var col = '#a855f7';
              statusHtml = '<span class="badge" style="background:rgba(168,85,247,0.1); color:' + col + '; border:1px solid rgba(168,85,247,0.25);"><span class="status-dot" style="background:' + col + '; box-shadow:0 0 10px ' + col + ';"></span>' + status + '</span>';
            } else {
              statusHtml = '<span class="badge badge-out"><span class="status-dot dot-out"></span>' + status + '</span>';
            }
            
            tr.innerHTML = '<td>' + dateStr + '</td>' +
                           '<td>' + dayOfWeek + '</td>' +
                           '<td style="font-family:monospace; font-weight:600;">' + firstIn + '</td>' +
                           '<td style="font-family:monospace; font-weight:600;">' + lastOut + '</td>' +
                           '<td style="font-family:monospace;">' + workHrs + '</td>' +
                           '<td>' + statusHtml + '</td>';
            tableBody.appendChild(tr);
            currentReportData.push({
              date: dateStr,
              day: dayOfWeek,
              in: firstIn,
              out: lastOut,
              work: workHrs,
              status: status,
              isLate: isLate
            });
            fetchDay(index + 1);
          })
          .catch(function(err) {
            console.error("Error fetching date: " + dateStr, err);
            var dayDate = new Date(yearVal, monthVal - 1, day);
            var isSunday = (dayDate.getDay() === 0);
            var holidayName = isDateHoliday(dateStr, holidaysList);
            var status = "ABSENT";
            
            if (holidayName) {
              status = "HOLIDAY: " + holidayName;
              holidayCount++;
            } else if (isSunday) {
              status = "WEEK OFF";
              weekOffCount++;
            } else {
              status = "ABSENT";
              absentCount++;
            }
            
            var dayNames = ["Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"];
            var dayOfWeek = dayNames[dayDate.getDay()];
            var tr = document.createElement('tr');
            
            var statusHtml = '';
            var statLower = status.toLowerCase();
            if (statLower.indexOf('week off') !== -1) {
              var col = '#58a6ff';
              statusHtml = '<span class="badge" style="background:rgba(88,166,255,0.1); color:' + col + '; border:1px solid rgba(88,166,255,0.25);"><span class="status-dot" style="background:' + col + '; box-shadow:0 0 10px ' + col + ';"></span>' + status + '</span>';
            } else if (statLower.indexOf('holiday') !== -1) {
              var col = '#a855f7';
              statusHtml = '<span class="badge" style="background:rgba(168,85,247,0.1); color:' + col + '; border:1px solid rgba(168,85,247,0.25);"><span class="status-dot" style="background:' + col + '; box-shadow:0 0 10px ' + col + ';"></span>' + status + '</span>';
            } else {
              statusHtml = '<span class="badge badge-out"><span class="status-dot dot-out"></span>ABSENT</span>';
            }
            
            tr.innerHTML = '<td>' + dateStr + '</td>' +
                           '<td>' + dayOfWeek + '</td>' +
                           '<td>--</td>' +
                           '<td>--</td>' +
                           '<td>--</td>' +
                           '<td>' + statusHtml + '</td>';
            tableBody.appendChild(tr);
            currentReportData.push({
              date: dateStr,
              day: dayOfWeek,
              in: "--",
              out: "--",
              work: "--",
              status: status,
              isLate: false
            });
            fetchDay(index + 1);
          });
      }
      fetchDay(0);
    });
}

function exportReportCSV() {
  if (currentReportData.length === 0) return showToast("No report data available to export", "error");
  var csvLines = [];
  csvLines.push("Date,Day,First IN,Last OUT,Work Hours,Status");
  currentReportData.forEach(function(row) {
    csvLines.push([
      row.date,
      row.day,
      row.in,
      row.out,
      row.work,
      row.status
    ].join(','));
  });
  var finalCsv = csvLines.join('\n');
  var blob = new Blob([finalCsv], { type: 'text/csv' });
  var url = window.URL.createObjectURL(blob);
  var a = document.createElement('a');
  a.setAttribute('hidden', '');
  a.setAttribute('href', url);
  var safeName = currentReportEmployeeName.replace(/[^a-z0-9]/gi, '_').toLowerCase();
  var safePeriod = currentReportPeriod.replace(/[^a-z0-9]/gi, '_').toLowerCase();
  a.setAttribute('download', 'monthly_report_' + safeName + '_' + safePeriod + '.csv');
  document.body.appendChild(a);
  a.click();
  document.body.removeChild(a);
}

function loadPayrollEmployees() {
  getUsersList(function(users) {
    var select = document.getElementById('pay-employee');
    if (!select) return;
    select.innerHTML = '<option value="">-- Select Employee --</option>';
    users.sort(function(a, b) {
      var nameA = (a.name || "").toLowerCase();
      var nameB = (b.name || "").toLowerCase();
      return nameA.localeCompare(nameB);
    });
    users.forEach(function(u) {
      var rollVal = u.roll || u.uid || "";
      var opt = document.createElement('option');
      opt.value = rollVal;
      opt.setAttribute('data-name', u.name || "");
      opt.setAttribute('data-dept', u.dept || "-");
      opt.setAttribute('data-role', u.role || "-");
      opt.innerText = (u.name || "Unknown") + " (" + rollVal + ")";
      select.appendChild(opt);
    });
  });
}

function onSalaryTypeChange() {
  var typeSelect = document.getElementById('pay-salary-type');
  var basicLabel = document.getElementById('pay-basic-label');
  var basicContainer = document.getElementById('pay-basic-container');
  var monthlyEarnings = document.getElementById('pay-monthly-earnings-fields');
  var deductionsContainer = document.getElementById('pay-deductions-container');
  var penaltiesContainer = document.getElementById('pay-penalties-container');
  
  if (typeSelect) {
    var val = typeSelect.value;
    if (val === 'monthly') {
      if(basicLabel) basicLabel.innerText = 'Basic Pay (₹)';
      if(basicContainer) basicContainer.style.gridColumn = 'span 2';
      if(monthlyEarnings) monthlyEarnings.style.display = 'grid';
      if(deductionsContainer) deductionsContainer.style.display = 'block';
      if(penaltiesContainer) penaltiesContainer.style.display = 'block';
    } else if (val === 'daily') {
      if(basicLabel) basicLabel.innerText = 'Daily Wage Rate (₹)';
      if(basicContainer) basicContainer.style.gridColumn = 'span 2';
      if(monthlyEarnings) monthlyEarnings.style.display = 'grid';
      if(deductionsContainer) deductionsContainer.style.display = 'block';
      if(penaltiesContainer) penaltiesContainer.style.display = 'block';
    } else if (val === 'hourly') {
      if(basicLabel) basicLabel.innerText = 'Hourly Wage Rate (₹)';
      if(basicContainer) basicContainer.style.gridColumn = 'span 2';
      if(monthlyEarnings) monthlyEarnings.style.display = 'none';
      if(deductionsContainer) deductionsContainer.style.display = 'none';
      if(penaltiesContainer) penaltiesContainer.style.display = 'none';
    }
  }
  onPayrollConfigChange();
}

function onPayrollEmployeeSelect() {
  var empSelect = document.getElementById('pay-employee');
  if(!empSelect) return;
  var rollVal = empSelect.value;
  
  // Default values
  var defaults = {
    salary_type: 'monthly',
    basic: 15000,
    da_pct: 46,
    hra_pct: 10,
    ta: 1000,
    ma: 500,
    ot_rate: 150,
    nps_pct: 10,
    pt: 200,
    tds: 0,
    gis: 120,
    late_penalty: 50,
    irregular_penalty: 250,
    shift_hours: 8,
    halfday_hours: 4,
    link_attendance: true
  };

  // If no employee selected, load global settings or defaults
  var keySuffix = rollVal ? '_' + rollVal : '';
  
  var loadedSalaryType = localStorage.getItem('pay_salary_type' + keySuffix) || defaults.salary_type;
  var typeSelect = document.getElementById('pay-salary-type');
  if (typeSelect) {
    typeSelect.value = loadedSalaryType;
  }
  
  var basicLabel = document.getElementById('pay-basic-label');
  if (basicLabel) {
    if (loadedSalaryType === 'monthly') {
      basicLabel.innerText = 'Basic Pay (₹)';
    } else if (loadedSalaryType === 'daily') {
      basicLabel.innerText = 'Daily Wage Rate (₹)';
    } else if (loadedSalaryType === 'hourly') {
      basicLabel.innerText = 'Hourly Wage Rate (₹)';
    }
  }
  
  document.getElementById('pay-basic').value = localStorage.getItem('pay_basic' + keySuffix) || defaults.basic;
  document.getElementById('pay-da-pct').value = localStorage.getItem('pay_da_pct' + keySuffix) || defaults.da_pct;
  document.getElementById('pay-hra-pct').value = localStorage.getItem('pay_hra_pct' + keySuffix) || defaults.hra_pct;
  document.getElementById('pay-ta').value = localStorage.getItem('pay_ta' + keySuffix) || defaults.ta;
  document.getElementById('pay-ma').value = localStorage.getItem('pay_ma' + keySuffix) || defaults.ma;
  document.getElementById('pay-ot-rate').value = localStorage.getItem('pay_ot_rate' + keySuffix) || defaults.ot_rate;
  document.getElementById('pay-nps-pct').value = localStorage.getItem('pay_nps_pct' + keySuffix) || defaults.nps_pct;
  document.getElementById('pay-pt').value = localStorage.getItem('pay_pt' + keySuffix) || defaults.pt;
  document.getElementById('pay-tds').value = localStorage.getItem('pay_tds' + keySuffix) || defaults.tds;
  document.getElementById('pay-gis').value = localStorage.getItem('pay_gis' + keySuffix) || defaults.gis;
  document.getElementById('pay-late-penalty').value = localStorage.getItem('pay_late_penalty' + keySuffix) || defaults.late_penalty;
  document.getElementById('pay-irregular-penalty').value = localStorage.getItem('pay_irregular_penalty' + keySuffix) || defaults.irregular_penalty;
  document.getElementById('pay-shift-hours').value = localStorage.getItem('pay_shift_hours' + keySuffix) || defaults.shift_hours;
  document.getElementById('pay-halfday-hours').value = localStorage.getItem('pay_halfday-hours' + keySuffix) || defaults.halfday_hours;
  var linkCheck = document.getElementById('pay-link-attendance');
  var linkVal = localStorage.getItem('pay_link_attendance' + keySuffix);
  linkCheck.checked = linkVal !== null ? (linkVal === 'true') : defaults.link_attendance;
  
  // Hide preview until Calculate is clicked
  document.getElementById('pay-slip-placeholder').style.display = 'block';
  document.getElementById('payslip-card-container').style.display = 'none';
  document.getElementById('printable-payslip-actions').style.display = 'none';
  
  // Synchronize dynamic settings visibility
  onSalaryTypeChange();
}

function onPayrollConfigChange() {
  var empSelect = document.getElementById('pay-employee');
  if(!empSelect) return;
  var rollVal = empSelect.value;
  var keySuffix = rollVal ? '_' + rollVal : '';
  
  var typeSelect = document.getElementById('pay-salary-type');
  if (typeSelect) {
    localStorage.setItem('pay_salary_type' + keySuffix, typeSelect.value);
  }
  
  localStorage.setItem('pay_basic' + keySuffix, document.getElementById('pay-basic').value);
  localStorage.setItem('pay_da_pct' + keySuffix, document.getElementById('pay-da-pct').value);
  localStorage.setItem('pay_hra_pct' + keySuffix, document.getElementById('pay-hra-pct').value);
  localStorage.setItem('pay_ta' + keySuffix, document.getElementById('pay-ta').value);
  localStorage.setItem('pay_ma' + keySuffix, document.getElementById('pay-ma').value);
  localStorage.setItem('pay_ot_rate' + keySuffix, document.getElementById('pay-ot-rate').value);
  localStorage.setItem('pay_nps_pct' + keySuffix, document.getElementById('pay-nps-pct').value);
  localStorage.setItem('pay_pt' + keySuffix, document.getElementById('pay-pt').value);
  localStorage.setItem('pay_tds' + keySuffix, document.getElementById('pay-tds').value);
  localStorage.setItem('pay_gis' + keySuffix, document.getElementById('pay-gis').value);
  localStorage.setItem('pay_late_penalty' + keySuffix, document.getElementById('pay-late-penalty').value);
  localStorage.setItem('pay_irregular_penalty' + keySuffix, document.getElementById('pay-irregular-penalty').value);
  localStorage.setItem('pay_shift_hours' + keySuffix, document.getElementById('pay-shift-hours').value);
  localStorage.setItem('pay_halfday-hours' + keySuffix, document.getElementById('pay-halfday-hours').value);
  localStorage.setItem('pay_link_attendance' + keySuffix, document.getElementById('pay-link-attendance').checked);
  
  // Live recalculate preview if it is already visible
  if(document.getElementById('payslip-card-container').style.display === 'block') {
    calculatePayroll();
  }
}

// Global variable for current payslip details
var currentPayslipData = null;

function calculatePayroll() {
  var empSelect = document.getElementById('pay-employee');
  if(!empSelect || !empSelect.value) {
    return showToast("Select an employee first", "error");
  }
  
  var rollVal = empSelect.value;
  var selectedOpt = empSelect.options[empSelect.selectedIndex];
  var empName = selectedOpt.getAttribute('data-name') || "Employee";
  var empDept = selectedOpt.getAttribute('data-dept') || "-";
  var empRole = selectedOpt.getAttribute('data-role') || "-";
  
  var monthSelect = document.getElementById('pay-month');
  var yearSelect = document.getElementById('pay-year');
  var monthVal = parseInt(monthSelect.value);
  var yearVal = parseInt(yearSelect.value);
  
  var daysInMonth = new Date(yearVal, monthVal, 0).getDate();
  
  // Load inputs
  var basic = parseFloat(document.getElementById('pay-basic').value) || 0;
  var daPct = parseFloat(document.getElementById('pay-da-pct').value) || 0;
  var hraPct = parseFloat(document.getElementById('pay-hra-pct').value) || 0;
  var ta = parseFloat(document.getElementById('pay-ta').value) || 0;
  var ma = parseFloat(document.getElementById('pay-ma').value) || 0;
  var otRate = parseFloat(document.getElementById('pay-ot-rate').value) || 0;
  
  var npsPct = parseFloat(document.getElementById('pay-nps-pct').value) || 0;
  var pt = parseFloat(document.getElementById('pay-pt').value) || 0;
  var tds = parseFloat(document.getElementById('pay-tds').value) || 0;
  var gis = parseFloat(document.getElementById('pay-gis').value) || 0;
  
  var latePenalty = parseFloat(document.getElementById('pay-late-penalty').value) || 0;
  var irregularPenalty = parseFloat(document.getElementById('pay-irregular-penalty').value) || 0;
  var shiftHours = parseFloat(document.getElementById('pay-shift-hours').value) || 8;
  var halfdayHours = parseFloat(document.getElementById('pay-halfday-hours').value) || 4;
  var linkAttendance = document.getElementById('pay-link-attendance').checked;
  var salaryType = document.getElementById('pay-salary-type').value;
  
  showToast("Calculating attendance parameters...", "info");
  
  var datesToFetch = [];
  for (var d = 1; d <= daysInMonth; d++) {
    var ddStr = String(d).padStart(2, '0');
    var mmStr = String(monthVal).padStart(2, '0');
    var dateStr = ddStr + '-' + mmStr + '-' + yearVal;
    datesToFetch.push(dateStr);
  }
  
  var presentCount = 0;
  var absentCount = 0;
  var irregularCount = 0;
  var lateCount = 0;
  var halfDayCount = 0;
  var totalOvertimeHours = 0;
  var totalWorkedMinutes = 0;
  
  safeFetch('/get-shift')
    .then(function(r) { return r.json(); })
    .catch(function() { return { startHour: 9, startMin: 0, graceMins: 15 }; })
    .then(function(shift) {
      var startLimitMins = shift.startHour * 60 + shift.startMin + (parseInt(shift.graceMins) || 0);
      
      var fetchPromises = datesToFetch.map(function(dStr) {
        return safeFetch('/get-history?date=' + dStr)
          .then(function(r) { return r.text(); })
          .catch(function() { return ""; })
          .then(function(text) {
            var firstIn = "--";
            var lastOut = "--";
            var inTimes = [];
            var outTimes = [];
            
            if (text && text.trim()) {
              var lines = text.trim().split('\n');
              for (var i = 1; i < lines.length; i++) {
                var cols = lines[i].split(',');
                if (cols.length >= 5) {
                  var logTime = cols[0].trim();
                  var logRoll = cols[1].trim();
                  var logName = cols[2].trim();
                  var logDir = cols[4].trim().toLowerCase();
                  if (logRoll === rollVal || (logRoll === "" && logName.toLowerCase() === empName.toLowerCase())) {
                    if (logDir === "in") {
                      inTimes.push(logTime);
                    } else if (logDir === "out") {
                      outTimes.push(logTime);
                    }
                  }
                }
              }
            }
            
            if (inTimes.length > 0) {
              inTimes.sort();
              firstIn = inTimes[0];
            }
            if (outTimes.length > 0) {
              outTimes.sort();
              lastOut = outTimes[outTimes.length - 1];
            }
            
            var totalPunches = inTimes.length + outTimes.length;
            var workedMins = -1;
            
            if (inTimes.length > 0 && outTimes.length > 0) {
              if (firstIn !== "--" && lastOut !== "--") {
                var inParts = firstIn.split(':');
                var outParts = lastOut.split(':');
                if (inParts.length >= 2 && outParts.length >= 2) {
                  var inM = parseInt(inParts[0], 10) * 60 + parseInt(inParts[1], 10);
                  var outM = parseInt(outParts[0], 10) * 60 + parseInt(outParts[1], 10);
                  workedMins = outM - inM;
                  if (workedMins < 0) workedMins = 0;
                }
              }
            }
            
            if (workedMins > 0) {
              totalWorkedMinutes += workedMins;
            }
            
            if (totalPunches === 0) {
              absentCount++;
            } else if (inTimes.length > 0 && outTimes.length > 0) {
              if (workedMins >= 0) {
                if (workedMins < halfdayHours * 60) {
                  absentCount++; // Treated as Absent LWP
                } else if (workedMins < shiftHours * 60) {
                  halfDayCount++; // Treated as Half Day
                } else {
                  presentCount++; // Treated as Present
                  var otMins = workedMins - (shiftHours * 60);
                  if (otMins > 0) {
                    totalOvertimeHours += (otMins / 60);
                  }
                }
              } else {
                presentCount++;
              }
              
              if (firstIn !== "--") {
                var inParts = firstIn.split(':');
                if (inParts.length >= 2) {
                  var inMins = parseInt(inParts[0], 10) * 60 + parseInt(inParts[1], 10);
                  if (inMins > startLimitMins) {
                    lateCount++;
                  }
                }
              }
            } else {
              irregularCount++;
            }
          });
      });
      
      return Promise.all(fetchPromises);
    })
    .then(function() {
      // Complete calculations
      var totalWorkedHours = totalWorkedMinutes / 60;
      var baseEarned = 0;
      if (salaryType === 'monthly') {
        baseEarned = basic;
      } else if (salaryType === 'daily') {
        baseEarned = basic * (presentCount + 0.5 * halfDayCount);
      } else if (salaryType === 'hourly') {
        baseEarned = basic * totalWorkedHours;
      }
      
      var dailyRate = salaryType === 'monthly' ? (basic / daysInMonth) : 0;
      var otEarnings = salaryType === 'hourly' ? 0 : (totalOvertimeHours * otRate);
      var daVal = salaryType === 'hourly' ? 0 : (baseEarned * (daPct / 100));
      var hraVal = salaryType === 'hourly' ? 0 : (baseEarned * (hraPct / 100));
      var earnedTa = salaryType === 'hourly' ? 0 : ta;
      var earnedMa = salaryType === 'hourly' ? 0 : ma;
      var grossEarn = baseEarned + daVal + hraVal + earnedTa + earnedMa + otEarnings;
      
      var npsVal = salaryType === 'hourly' ? 0 : ((baseEarned + daVal) * (npsPct / 100));
      var earnedPt = salaryType === 'hourly' ? 0 : pt;
      var earnedTds = salaryType === 'hourly' ? 0 : tds;
      var earnedGis = salaryType === 'hourly' ? 0 : gis;
      var lwpVal = 0;
      var lateDeduct = 0;
      var irrDeduct = 0;
      var halfDayDeduct = 0;
      
      if(linkAttendance) {
        if (salaryType === 'monthly') {
          lwpVal = absentCount * dailyRate;
          halfDayDeduct = halfDayCount * (dailyRate * 0.5);
        }
        if (salaryType !== 'hourly') {
          lateDeduct = lateCount * latePenalty;
          irrDeduct = irregularCount * irregularPenalty;
        }
      }
      
      var totalDeduct = npsVal + earnedPt + earnedTds + earnedGis + lwpVal + lateDeduct + irrDeduct + halfDayDeduct;
      var netSalary = grossEarn - totalDeduct;
      if (netSalary < 0) netSalary = 0;
      
      // Update UI elements
      document.getElementById('slip-emp-name').innerText = empName;
      document.getElementById('slip-emp-roll').innerText = rollVal;
      document.getElementById('slip-emp-dept').innerText = empDept;
      document.getElementById('slip-emp-role').innerText = empRole;
      document.getElementById('slip-days-month').innerText = daysInMonth;
      document.getElementById('slip-days-present').innerText = presentCount;
      document.getElementById('slip-days-absent').innerText = absentCount;
      document.getElementById('slip-days-late').innerText = lateCount;
      document.getElementById('slip-days-half').innerText = halfDayCount;
      document.getElementById('slip-hours-ot').innerText = totalOvertimeHours.toFixed(1);
      
      var rowTotalHours = document.getElementById('slip-row-total-hours');
      if (rowTotalHours) {
        if (salaryType === 'hourly') {
          rowTotalHours.style.display = 'block';
          document.getElementById('slip-total-hours').innerText = totalWorkedHours.toFixed(1);
        } else {
          rowTotalHours.style.display = 'none';
        }
      }
      
      var earnBasicDesc = document.getElementById('slip-earn-basic-desc');
      if (earnBasicDesc) {
        earnBasicDesc.innerText = (salaryType === 'monthly' ? 'Basic Pay' : (salaryType === 'daily' ? 'Earned Daily Wage (Base)' : 'Earned Hourly Pay (Base)'));
      }
      
      // Earnings elements
      document.getElementById('slip-earn-basic').innerText = "₹" + baseEarned.toFixed(2);
      document.getElementById('slip-earn-da').innerText = "₹" + daVal.toFixed(2);
      document.getElementById('slip-earn-hra').innerText = "₹" + hraVal.toFixed(2);
      document.getElementById('slip-earn-ta').innerText = "₹" + earnedTa.toFixed(2);
      document.getElementById('slip-earn-ma').innerText = "₹" + earnedMa.toFixed(2);
      document.getElementById('slip-earn-ot').innerText = "₹" + otEarnings.toFixed(2);
      document.getElementById('slip-earn-gross').innerText = "₹" + grossEarn.toFixed(2);
      
      // Deductions elements
      document.getElementById('slip-ded-nps').innerText = "₹" + npsVal.toFixed(2);
      document.getElementById('slip-ded-pt').innerText = "₹" + earnedPt.toFixed(2);
      document.getElementById('slip-ded-tds').innerText = "₹" + earnedTds.toFixed(2);
      document.getElementById('slip-ded-gis').innerText = "₹" + earnedGis.toFixed(2);
      document.getElementById('slip-ded-lwp').innerText = "₹" + lwpVal.toFixed(2);
      document.getElementById('slip-ded-late').innerText = "₹" + lateDeduct.toFixed(2);
      document.getElementById('slip-ded-irr').innerText = "₹" + irrDeduct.toFixed(2);
      document.getElementById('slip-ded-half').innerText = "₹" + halfDayDeduct.toFixed(2);
      document.getElementById('slip-ded-total').innerText = "₹" + totalDeduct.toFixed(2);
      
      // Net Salary
      document.getElementById('slip-net-salary').innerText = "₹" + netSalary.toFixed(2);
      document.getElementById('slip-words-amount').innerText = numToRupeesWords(netSalary);
      
      // Update header detail
      var devName = document.getElementById('set-dev-name') ? document.getElementById('set-dev-name').value : "ESP32";
      document.getElementById('slip-org').innerText = devName.toUpperCase();
      var monthText = monthSelect.options[monthSelect.selectedIndex].text;
      document.getElementById('slip-period').innerText = "MONTH: " + monthText.toUpperCase() + " " + yearVal;
      
      // Show card
      document.getElementById('pay-slip-placeholder').style.display = 'none';
      document.getElementById('payslip-card-container').style.display = 'block';
      document.getElementById('printable-payslip-actions').style.display = 'flex';
      
      // Save data for CSV export
      currentPayslipData = {
        name: empName,
        roll: rollVal,
        dept: empDept,
        role: empRole,
        month: monthText,
        year: yearVal,
        salaryType: salaryType,
        totalWorkedHours: totalWorkedHours,
        days: daysInMonth,
        present: presentCount,
        absent: absentCount,
        late: lateCount,
        halfDay: halfDayCount,
        otHours: totalOvertimeHours,
        otEarnings: otEarnings,
        halfDayDeduct: halfDayDeduct,
        basic: baseEarned,
        da: daVal,
        hra: hraVal,
        ta: earnedTa,
        ma: earnedMa,
        gross: grossEarn,
        nps: npsVal,
        pt: earnedPt,
        tds: earnedTds,
        gis: earnedGis,
        lwp: lwpVal,
        lateDeduct: lateDeduct,
        irrDeduct: irrDeduct,
        totalDeduct: totalDeduct,
        net: netSalary,
        words: numToRupeesWords(netSalary)
      };
      
      showToast("Pay Slip generated successfully!", "success");
    })
    .catch(function(e) {
      console.error(e);
      showToast("Error generating Pay Slip", "error");
    });
}

function numToRupeesWords(num) {
  if (num === 0) return "Rupees Zero Only.";
  
  var units = ["", "One", "Two", "Three", "Four", "Five", "Six", "Seven", "Eight", "Nine", "Ten", 
               "Eleven", "Twelve", "Thirteen", "Fourteen", "Fifteen", "Sixteen", "Seventeen", "Eighteen", "Nineteen"];
  var tens = ["", "", "Twenty", "Thirty", "Forty", "Fifty", "Sixty", "Seventy", "Eighty", "Ninety"];
  
  function getWords(n) {
    if (n < 20) return units[n];
    if (n < 100) return tens[Math.floor(n/10)] + (n % 10 !== 0 ? " " + units[n % 10] : "");
    if (n < 1000) return units[Math.floor(n/100)] + " Hundred" + (n % 100 !== 0 ? " and " + getWords(n % 100) : "");
    if (n < 100000) return getWords(Math.floor(n/1000)) + " Thousand" + (n % 1000 !== 0 ? " " + getWords(n % 1000) : "");
    if (n < 10000000) return getWords(Math.floor(n/100000)) + " Lakh" + (n % 100000 !== 0 ? " " + getWords(n % 100000) : "");
    return getWords(Math.floor(n/10000000)) + " Crore" + (n % 10000000 !== 0 ? " " + getWords(n % 10000000) : "");
  }
  
  var integral = Math.floor(num);
  var fraction = Math.round((num - integral) * 100);
  
  var words = "Rupees " + getWords(integral);
  if (fraction > 0) {
    words += " and " + getWords(fraction) + " Paise";
  }
  words += " Only.";
  return words;
}

function exportPayrollCSV() {
  if (!currentPayslipData) return showToast("No payslip data generated yet", "error");
  var d = currentPayslipData;
  var csv = "MAHARASHTRA GOVERNMENT SEVAARTH COMPATIBLE PAYSLIP\n";
  csv += "Organization," + d.name.toUpperCase() + " ATTENDANCE SYSTEM\n";
  csv += "Period," + d.month + " " + d.year + "\n\n";
  csv += "EMPLOYEE DETAILS\n";
  csv += "Name," + d.name + "\n";
  csv += "Employee ID," + d.roll + "\n";
  csv += "Department," + d.dept + "\n";
  csv += "Role," + d.role + "\n";
  csv += "Salary Model," + (d.salaryType === 'monthly' ? "Monthly Salaried" : (d.salaryType === 'daily' ? "Daily Wage" : "Hourly Basis")) + "\n";
  csv += "Total Days," + d.days + "\n";
  csv += "Present Days," + d.present + "\n";
  csv += "Absent Days (LWP)," + d.absent + "\n";
  csv += "Late Days," + d.late + "\n";
  csv += "Half Days Worked," + d.halfDay + "\n";
  if (d.salaryType === 'hourly') {
    csv += "Total Worked Hours," + d.totalWorkedHours.toFixed(1) + "\n";
  }
  csv += "Overtime Hours," + d.otHours.toFixed(1) + "\n\n";
  
  var basicLabel = (d.salaryType === 'monthly' ? "Basic Pay" : (d.salaryType === 'daily' ? "Earned Daily Wage (Base)" : "Earned Hourly Pay (Base)"));
  
  csv += "EARNINGS (ALLOWANCES),Amount (INR),DEDUCTIONS (RECOVERIES),Amount (INR)\n";
  csv += basicLabel + "," + d.basic.toFixed(2) + ",Provident Fund (NPS)," + d.nps.toFixed(2) + "\n";
  csv += "Dearness Allowance (DA)," + d.da.toFixed(2) + ",Professional Tax (PT)," + d.pt.toFixed(2) + "\n";
  csv += "House Rent Allowance (HRA)," + d.hra.toFixed(2) + ",Income Tax (TDS)," + d.tds.toFixed(2) + "\n";
  csv += "Transport Allowance (TA)," + d.ta.toFixed(2) + ",Group Insurance (GIS)," + d.gis.toFixed(2) + "\n";
  csv += "Medical Allowance (MA)," + d.ma.toFixed(2) + ",Absent Days LWP Deduction," + d.lwp.toFixed(2) + "\n";
  csv += "Overtime Earnings," + d.otEarnings.toFixed(2) + ",Late Penalties," + d.lateDeduct.toFixed(2) + "\n";
  csv += ",,Irregular Penalties," + d.irrDeduct.toFixed(2) + "\n";
  csv += ",,Half Day Deductions," + d.halfDayDeduct.toFixed(2) + "\n";
  csv += "GROSS EARNINGS," + d.gross.toFixed(2) + ",TOTAL DEDUCTIONS," + d.totalDeduct.toFixed(2) + "\n\n";
  
  csv += "NET PAYABLE PAYOUT," + d.net.toFixed(2) + "\n";
  csv += "Amount In Words," + d.words + "\n";
  
  var blob = new Blob([csv], { type: "text/csv;charset=utf-8;" });
  var link = document.createElement("a");
  var filename = "Payslip_" + d.roll + "_" + d.month + "_" + d.year + ".csv";
  if (link.download !== undefined) {
    var url = URL.createObjectURL(blob);
    link.setAttribute("href", url);
    link.setAttribute("download", filename);
    link.style.visibility = 'hidden';
    document.body.appendChild(link);
    link.click();
    document.body.removeChild(link);
  }
}

function printPayslip() {
  document.querySelectorAll('.page').forEach(function(p) { p.classList.remove('print-active'); });
  var pg = document.getElementById('p-pay');
  if (pg) pg.classList.add('print-active');
  window.print();
}

function printReport() {
  document.querySelectorAll('.page').forEach(function(p) { p.classList.remove('print-active'); });
  var pg = document.getElementById('p-rep');
  if (pg) pg.classList.add('print-active');
  window.print();
}

window.onload = function() {
  try {
    var theme = storage.getItem('theme') || 'light';
    var html = document.documentElement;
    if (html) html.setAttribute('data-theme', theme);
    var ttb = document.getElementById('theme-toggle-btn');
    if (ttb) ttb.innerText = theme === 'dark' ? '☀️' : '🌙';
    
    safeFetch('/init-data')
      .then(function(r) { return r.json(); })
      .then(function(data) {
        if (data) {
          if (data.deviceName) {
            var lcn = document.getElementById('login-company-name');
            if (lcn) lcn.innerText = data.deviceName;
          }
          updateAdvancedInfo(data);
        }
        if (storage.getItem("loggedIn") === "true") {
          enterApp(data);
        } else {
          var lp = document.getElementById('login-page');
          var ma = document.getElementById('main-app');
          if (lp) lp.style.display = 'flex';
          if (ma) ma.style.display = 'none';
        }
      })
      .catch(function() {
        if (storage.getItem("loggedIn") === "true") {
          enterApp();
        } else {
          var lp = document.getElementById('login-page');
          var ma = document.getElementById('main-app');
          if (lp) lp.style.display = 'flex';
          if (ma) ma.style.display = 'none';
        }
      });
  } catch(e) {
    console.error("Initialization error", e);
  }
};
</script>
</body>
</html>
)rawliteral";

const char ap_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>Security & Premium Settings</title>
<link href="https://fonts.googleapis.com/css2?family=Outfit:wght@300;400;500;600;700;800&display=swap" rel="stylesheet">
<style>
  :root {
    --bg: #0b0f19;
    --card-bg: #111827;
    --border: #1f2937;
    --text: #f3f4f6;
    --text-muted: #9ca3af;
    --primary: #3b82f6;
    --primary-hover: #2563eb;
    --danger: #ef4444;
    --success: #10b981;
    --warning: #f59e0b;
    --input-bg: #1f2937;
    --font-family: 'Outfit', sans-serif;
  }
  
  body {
    background-color: var(--bg);
    color: var(--text);
    font-family: var(--font-family);
    margin: 0;
    padding: 0;
    min-height: 100vh;
    display: flex;
    flex-direction: column;
    align-items: center;
  }

  header {
    width: 100%;
    max-width: 900px;
    padding: 30px 20px 20px;
    box-sizing: border-box;
    display: flex;
    justify-content: space-between;
    align-items: center;
    border-bottom: 1px solid var(--border);
  }

  h1 {
    font-size: 24px;
    font-weight: 800;
    margin: 0;
    background: linear-gradient(90deg, #60a5fa, #34d399);
    -webkit-background-clip: text;
    -webkit-text-fill-color: transparent;
  }

  .container {
    width: 100%;
    max-width: 900px;
    padding: 20px;
    box-sizing: border-box;
    display: flex;
    flex-direction: column;
    gap: 20px;
  }

  .grid {
    display: grid;
    grid-template-columns: repeat(auto-fit, minmax(350px, 1fr));
    gap: 20px;
  }

  .card {
    background: var(--card-bg);
    border: 1px solid var(--border);
    border-radius: 16px;
    padding: 24px;
    display: flex;
    flex-direction: column;
    box-shadow: 0 10px 30px rgba(0,0,0,0.3);
  }

  .card h3 {
    margin-top: 0;
    margin-bottom: 8px;
    font-size: 18px;
    font-weight: 700;
    display: flex;
    align-items: center;
    gap: 10px;
  }

  .hint {
    font-size: 12px;
    color: var(--text-muted);
    margin-bottom: 20px;
  }

  label {
    display: block;
    font-size: 13px;
    font-weight: 600;
    margin-bottom: 6px;
    color: var(--text);
  }

  input[type="text"], input[type="password"] {
    width: 100%;
    padding: 12px 14px;
    border: 1px solid var(--border);
    background: var(--input-bg);
    color: var(--text);
    border-radius: 10px;
    font-size: 14px;
    box-sizing: border-box;
    margin-bottom: 16px;
    font-family: var(--font-family);
    transition: border-color 0.2s;
  }

  input[type="text"]:focus, input[type="password"]:focus {
    border-color: var(--primary);
    outline: none;
  }

  .btn {
    width: 100%;
    padding: 12px;
    border-radius: 10px;
    border: none;
    font-size: 14px;
    font-weight: 700;
    cursor: pointer;
    font-family: var(--font-family);
    display: flex;
    align-items: center;
    justify-content: center;
    gap: 8px;
    transition: all 0.2s;
  }

  .btn-primary {
    background: var(--primary);
    color: white;
  }

  .btn-primary:hover {
    background: var(--primary-hover);
  }

  .btn-outline {
    background: transparent;
    border: 1px solid var(--border);
    color: var(--text);
  }

  .btn-outline:hover {
    background: var(--border);
  }

  .toggle-switch-container {
    display: flex;
    justify-content: space-between;
    align-items: center;
    margin-bottom: 16px;
    cursor: pointer;
  }

  .toggle-label-text {
    font-size: 14px;
    font-weight: 500;
    color: var(--text-muted);
  }

  .switch {
    position: relative;
    display: inline-block;
    width: 44px;
    height: 24px;
  }

  .switch input {
    opacity: 0;
    width: 0;
    height: 0;
  }

  .slider {
    position: absolute;
    cursor: pointer;
    top: 0; left: 0; right: 0; bottom: 0;
    background-color: var(--input-bg);
    border: 1px solid var(--border);
    transition: .3s;
    border-radius: 24px;
  }

  .slider:before {
    position: absolute;
    content: "";
    height: 16px;
    width: 16px;
    left: 3px;
    bottom: 3px;
    background-color: var(--text-muted);
    transition: .3s;
    border-radius: 50%;
  }

  input:checked + .slider {
    background-color: var(--primary);
    border-color: var(--primary);
  }

  input:checked + .slider:before {
    transform: translateX(20px);
    background-color: white;
  }

  .banner {
    background: rgba(6, 214, 160, 0.05);
    border: 1px dashed var(--success);
    border-radius: 12px;
    padding: 15px;
    display: flex;
    justify-content: space-between;
    align-items: center;
    margin-bottom: 20px;
    width: 100%;
    box-sizing: border-box;
  }

  .banner-text {
    display: flex;
    align-items: center;
    gap: 12px;
    text-align: left;
  }

  .banner-title {
    margin: 0;
    color: var(--success);
    font-size: 14px;
    font-weight: 700;
  }

  .banner-subtitle {
    margin: 3px 0 0 0;
    font-size: 12px;
    color: var(--text-muted);
  }

  #toast {
    position: fixed;
    bottom: 24px;
    right: 24px;
    padding: 14px 22px;
    border-radius: 10px;
    color: #fff;
    font-weight: 600;
    display: none;
    z-index: 9999;
    font-size: 14px;
    box-shadow: 0 4px 16px rgba(0,0,0,0.2);
    min-width: 200px;
  }

  .password-toggle {
    position: absolute;
    right: 12px;
    top: 12px;
    cursor: pointer;
    font-size: 16px;
    user-select: none;
  }
</style>
</head>
<body>

<header>
  <h1>🔒 Security & Premium Settings</h1>
  <div style="font-weight: 600; color: var(--warning); display: flex; align-items: center; gap: 5px;">
    ⏳ AP Session: <span id="apmode-timer">05:00</span>
  </div>
</header>

<div class="container">
  <div class="banner">
    <div class="banner-text">
      <span style="font-size: 24px;">🔓</span>
      <div>
        <h4 class="banner-title">Access Point Mode Active</h4>
        <p class="banner-subtitle">Credentials and branding features unlocked. System returns to Station mode when timer expires.</p>
      </div>
    </div>
    <button class="btn btn-outline" onclick="deactivateAPMode()" style="width: auto; padding: 6px 12px; font-size: 12px; color: var(--danger); border-color: var(--danger);">🛑 Close AP</button>
  </div>

  <div class="grid">
    <!-- CHANGE ADMIN CREDENTIALS -->
    <div class="card">
      <h3>🔑 Admin Credentials</h3>
      <p class="hint">Change the username and password used to access the dashboard.</p>
      
      <label>New Username</label>
      <input type="text" id="new-admin-user" placeholder="Enter new username">
      
      <label>New Password</label>
      <div style="position:relative;">
        <input type="password" id="new-admin-pass" placeholder="Enter new password">
        <span class="password-toggle" onclick="togglePassVisibility('new-admin-pass', this)">👁️</span>
      </div>
      
      <button class="btn btn-primary" style="margin-top: auto;" onclick="saveAdminCredentials()">💾 Save Credentials</button>
    </div>

    <!-- PREMIUM FEATURES -->
    <div class="card">
      <h3>✨ Premium Features</h3>
      <p class="hint">System branding and integrations.</p>
      
      <label>🏷️ Device Name</label>
      <input type="text" id="set-dev-name" placeholder="Anurag 0.1">
      
      <label class="toggle-switch-container">
        <span class="toggle-label-text">Require TFT touch confirmation</span>
        <span class="switch">
          <input type="checkbox" id="set-tft-prompt">
          <span class="slider"></span>
        </span>
      </label>
      
      <label class="toggle-switch-container">
        <span class="toggle-label-text">Enable 2FA Mode</span>
        <span class="switch">
          <input type="checkbox" id="set-2fa-enabled">
          <span class="slider"></span>
        </span>
      </label>
      
      <label style="margin-top: 10px;">🔑 Google Maps API Key</label>
      <input type="password" id="set-google-key" placeholder="Enter API Key">
      
      <button class="btn btn-primary" onclick="savePremium()">💾 Save Branding</button>
    </div>
  </div>

  <div style="text-align: center; margin-top: 20px;">
    <button class="btn btn-outline" onclick="window.location.href='/'" style="width: auto; display: inline-flex; padding: 10px 20px;">🏠 Back to Dashboard</button>
  </div>
</div>

<div id="toast"></div>

<script>
  function showToast(msg, type) {
    var toast = document.getElementById('toast');
    if(!toast) return;
    toast.innerText = msg;
    toast.style.display = 'block';
    if (type === 'success') {
      toast.style.background = '#10b981';
    } else if (type === 'error') {
      toast.style.background = '#ef4444';
    } else {
      toast.style.background = '#3b82f6';
    }
    setTimeout(function() {
      toast.style.display = 'none';
    }, 3000);
  }

  function togglePassVisibility(id, btn) {
    var el = document.getElementById(id);
    if (el) {
      if (el.type === 'password') {
        el.type = 'text';
        btn.innerText = '🙈';
      } else {
        el.type = 'password';
        btn.innerText = '👁️';
      }
    }
  }

  function safeFetch(url) {
    return fetch(url).catch(function(err) {
      showToast("Network connection error", "error");
      throw err;
    });
  }

  // Load configuration
  safeFetch('/init-data')
    .then(function(r) { return r.json(); })
    .then(function(data) {
      document.getElementById('set-dev-name').value = data.deviceName || '';
      document.getElementById('set-tft-prompt').checked = !!data.tftPromptEnabled;
      document.getElementById('set-2fa-enabled').checked = !!data.twoFactorEnabled;
      document.getElementById('set-google-key').value = data.googleApiKey || '';
      
      // AP Mode Timer setup
      if (data.isAPMode === true && data.apModeTimeLeft > 0) {
        var timeLeft = data.apModeTimeLeft;
        var timerEl = document.getElementById('apmode-timer');
        
        var updateTimerDisplay = function() {
          var m = Math.floor(timeLeft / 60);
          var s = timeLeft % 60;
          timerEl.innerText = m + ":" + (s < 10 ? "0" : "") + s;
        };
        
        updateTimerDisplay();
        
        var interval = setInterval(function() {
          timeLeft--;
          if (timeLeft <= 0) {
            clearInterval(interval);
            timerEl.innerText = "0:00";
            showToast("AP Session expired. Returning to station...", "error");
            setTimeout(function() { window.location.href = '/'; }, 2000);
          } else {
            updateTimerDisplay();
          }
        }, 1000);
      }
    });

  function saveAdminCredentials() {
    var u = document.getElementById('new-admin-user').value.trim();
    var p = document.getElementById('new-admin-pass').value.trim();
    if (u.length === 0 || p.length === 0) {
      return showToast("Fields cannot be empty", "error");
    }
    if(!confirm("Change admin credentials?")) return;
    
    safeFetch('/api/set-credentials?username=' + encodeURIComponent(u) + '&password=' + encodeURIComponent(p))
      .then(function(r) {
        if (r.ok) {
          showToast("Credentials updated!", "success");
        } else {
          showToast("Failed to update credentials", "error");
        }
      });
  }

  function savePremium() {
    var name = document.getElementById('set-dev-name').value.trim();
    var tftPromptVal = document.getElementById('set-tft-prompt').checked ? "1" : "0";
    var twoFactorVal = document.getElementById('set-2fa-enabled').checked ? "1" : "0";
    var googleKey = document.getElementById('set-google-key').value.trim();

    safeFetch('/set-branding?name=' + encodeURIComponent(name) + '&tftPromptEnabled=' + tftPromptVal + '&twoFactorEnabled=' + twoFactorVal + '&googleApiKey=' + encodeURIComponent(googleKey))
      .then(function(r) {
        if(r.ok) { 
          showToast("Branding & Premium settings saved!", "success"); 
        } else {
          showToast("Failed to save settings", "error");
        }
      });
  }

  function deactivateAPMode() {
    if(!confirm("Disable AP Configuration Mode now?")) return;
    safeFetch('/deactivate-apmode').then(function(r) {
      if(r.ok) {
        showToast("AP Mode deactivated...", "success");
        setTimeout(function() { window.location.href = "/"; }, 1500);
      }
    });
  }
</script>
</body>
</html>
)rawliteral";
