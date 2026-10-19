# chronosMIS - 24/7 Biometric Cloud ERP & Firebase Link

This folder contains the **chronosMIS Firebase Cloud Web Portal** for the Biometric Attendance System. It is completely isolated from the ESP32 hardware firmware (`Attendance_System`), ensuring that changes to the cloud link/ERP software and microcontroller firmware do not conflict.

---

## 📁 Folder Contents
* **`index.html`**: The complete 24/7 **chronosMIS** Cloud ERP web portal (with live punch stream, employee directory, R503 optical sensor slot map, attendance logs, and Device IP Gateway).
* **`cloud_erp.html`**: Standalone HTML copy for direct browser opening.
* **`firebase.json`**: Firebase Hosting (configured for site `chronosmis`) and Realtime Database rules.
* **`.firebaserc`**: Project configuration pointing to `chronosmis`.
* **`database.rules.json`**: Firebase Realtime Database security rules for 24/7 live access.
* **`serve.py`**: Lightweight local server (prevents any browser mixed-content restrictions).
* **`start_erp.bat`**: Double-click batch file to launch chronosMIS portal instantly.
* **`deploy_online.bat`**: One-click script to deploy chronosMIS online to Firebase Hosting (`https://chronosmis.web.app/`).

---

## 🚀 How to Run chronosMIS Online & Locally

### Option 1: Run Online via Firebase Hosting (Access Anywhere 24/7)
1. Double-click **`deploy_online.bat`** in this folder (or run `firebase deploy --only hosting`).
2. Your app is immediately published online at:
   - **`https://chronosmis.web.app/`**
   - **`https://chronosmis.firebaseapp.com/`**
3. Open this link from anywhere in the world on mobile, tablet, or PC to monitor biometric punches in real-time.

### Option 2: Double-Click Local Server (Recommended for On-Premise Gateway)
1. Double-click **`start_erp.bat`** (or run `python serve.py`).
2. Your browser will automatically open to `http://localhost:5000/`.
3. Enter your **Device IP Address** (e.g., `192.168.1.100`) and your **chronosMIS Firebase Realtime Database URL** (`https://chronosmis-default-rtdb.firebaseio.com`).
4. Click **`⚡ Start Data & Template Transfer`**.
   * Connects to your ESP32 attendance device.
   * Auto-configures device firmware with chronosMIS Firebase URL for 24/7 live streaming.
   * Extracts and backs up all employee profiles, RFID cards, and R503 fingerprint sensor templates to Firebase Cloud!

### Option 3: Double-Click `index.html` or `cloud_erp.html`
You can double-click **`index.html`** in this folder to open it directly in Chrome, Edge, Safari, or Firefox without running any server.

---

## 🔒 Firebase Security Rules
In your Firebase Console (**Realtime Database > Rules**), publish the contents of `database.rules.json`:
```json
{
  "rules": {
    ".read": true,
    ".write": true
  }
}
```
