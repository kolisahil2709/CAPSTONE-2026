# ESP32-S3 Biometric Attendance & Access Controller

> **Professional-grade PCB** for a dual 2FA smart attendance system using capacitive fingerprint scanning + RFID + Ethernet + TFT touchscreen display.

---

## 🔑 Key Features

| Feature | Details |
|---|---|
| **MCU** | ESP32-S3-WROOM-1 (Dual-core 240MHz, WiFi, BT 5.0) |
| **Fingerprint** | R503 Capacitive Fingerprint Sensor (UART) |
| **RFID** | MFRC522 (SPI, 13.56 MHz, ISO14443A) |
| **Display** | 3.5" TFT ILI9488 with XPT2046 Resistive Touch (SPI) |
| **Ethernet** | W5500 Hardwired TCP/IP SPI Ethernet |
| **Storage** | 24C512 64KB I2C EEPROM for attendance logs |
| **Power** | USB-C 5V input → AMS1117-3.3 LDO (1.5A) |
| **Feedback** | RGB Status LEDs + Active Buzzer |
| **Access** | Relay-controlled door lock via MOSFET driver |

---

## 📐 Schematic Overview

```
USB-C (5V)
   │
   ├─► AMS1117-3.3 LDO ──► +3V3 Rail
   │
   ├─► F1 Fuse (500mA)
   │
ESP32-S3-WROOM-1
   ├─ UART2 ──────────────► R503 Fingerprint (J3)
   ├─ SPI (IO11/12/13)
   │    ├─ CS IO14 ─────► MFRC522 RFID (J4)
   │    ├─ CS IO5  ─────► W5500 Ethernet (J5)
   │    └─ CS IO15 ─────► ILI9488 TFT + XPT2046 Touch (J6)
   ├─ I2C (IO39/IO40) ──► 24C512 EEPROM (U3)
   ├─ GPIO38 ───────────► Relay/Lock MOSFET (Q1 + D5)
   ├─ GPIO4  ───────────► Buzzer BJT driver (Q2)
   ├─ GPIO5,6,7 ────────► RGB Status LEDs (D1/D2/D3)
   ├─ GPIO0  ───────────► BOOT button (SW2)
   └─ EN ───────────────► RESET button (SW1)
```

---

## 📂 Repository Structure

```
├── hardware/
│   ├── ESP32_S3_Attendance_System.kicad_pro   ← KiCad project
│   ├── ESP32_S3_Attendance_System.kicad_sch   ← Schematic
│   ├── ESP32_S3_Attendance_System.kicad_pcb   ← PCB layout
│   └── ESP32_S3_Attendance_System.net         ← Netlist
├── docs/
│   ├── ESP32_S3_Attendance_System_Schematic.pdf ← Schematic PDF
│   ├── BOM_ESP32_S3_Attendance_System.csv       ← Bill of Materials
│   └── SCHEMATIC_MANUAL_AND_PINOUT.md           ← Full pinout reference
└── firmware/                                  ← ESP32 Arduino/IDF code (coming soon)
```

---

## 🧰 Bill of Materials (Summary)

| Qty | Component | Description |
|-----|-----------|-------------|
| 1 | ESP32-S3-WROOM-1 | Main microcontroller module |
| 1 | AMS1117-3.3 | 3.3V LDO regulator (SOT-223) |
| 1 | 24C512 | 64KB I2C EEPROM (DIP-8) |
| 1 | W5500 | Hardwired Ethernet SPI module |
| 1 | MFRC522 | RFID reader module (SPI) |
| 1 | R503 | Capacitive fingerprint sensor |
| 1 | ILI9488 3.5" | TFT LCD + XPT2046 touch |
| 2 | 2N7002 | N-channel MOSFET (SOT-23) |
| 1 | S8050 | NPN BJT buzzer driver (SOT-23) |
| 37 | Various | Resistors, capacitors, LEDs, connectors |

Full BOM: [`docs/BOM_ESP32_S3_Attendance_System.csv`](docs/BOM_ESP32_S3_Attendance_System.csv)

---

## ⚡ ERC Status

| Check | Result |
|-------|--------|
| Errors | **0** ✅ |
| Warnings | 102 (lib_symbol_mismatch — cosmetic only) |
| Netlist | Exported cleanly ✅ |

---

## 🚀 Getting Started

### Hardware
1. Open `hardware/ESP32_S3_Attendance_System.kicad_pro` in **KiCad 10**
2. Review schematic: `File → Open Schematic`
3. Start PCB layout: `Tools → Update PCB from Schematic (F8)`

### Firmware (Coming Soon)
```bash
# Clone this repo
git clone https://github.com/YOUR_USERNAME/esp32-s3-biometric-attendance.git

# Open firmware/ in VS Code with PlatformIO or Arduino IDE
```

---

## 📄 License

This hardware design is open source under the **CERN-OHL-S v2** license.

---

*Designed with KiCad 10 | ESP32-S3 Biometric Attendance System v1.0*
