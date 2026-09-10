# ESP32-S3 Biometric & RFID Attendance System
## Hardware Engineering Manual & KiCad Schematic Reference

**Target Platform:** ESP32-S3 Dual-Core Xtensa LX7 @ 240MHz  
**Project Files:** KiCad 6 / 7 / 8 Compatible  
**Revision:** v1.0  
**Date:** August 20, 2026  

---

## 1. Master System Pinout & Net Mapping Table

All peripheral modules are mapped to the ESP32-S3 GPIO layout strictly adhering to the technical specification:

| ESP32-S3 GPIO | Direction | Connected Subsystem | Module Pin | Schematic Net / Function | Circuit Implementation Details |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **GPIO 1** | Output | 3.5" TFT LCD | `BL` | `TFT_BL` | Active-High Backlight control via N-MOSFET (2N7002 / S8050) with 10kΩ gate pull-down to keep backlight OFF during boot |
| **GPIO 2** | Output | Green Status LED | Anode (`+`) | `LED_GREEN` | Authorized Access / Success punch indicator via 220Ω series current-limiting resistor |
| **GPIO 3** | Output | MFRC522 RFID | `RST` | `RFID_RST` | Hardware Reset line to initialize/re-sync RFID card reader |
| **GPIO 4** | Output | Red Status LED | Anode (`+`) | `LED_RED` | Access Denied / Error alert indicator via 220Ω series current-limiting resistor |
| **GPIO 5** | Output | Active Piezo Buzzer | Positive (`+`)| `BUZZER_CTRL` | Audible swipe confirmation tone driven via S8050 NPN BJT + 1kΩ base resistor + 1N4148 flyback clamp diode |
| **GPIO 6** | Output | 3.5" TFT LCD | `RST` | `TFT_RST` | ILI9488 Display Hardware Reset line |
| **GPIO 7** | Output | 3.5" TFT LCD | `RS` / `DC` | `TFT_DC` | Display Data / Command Register Select control line |
| **GPIO 8** | Bi-dir | 24C512 EEPROM | `SDA` (Pin 5) | `I2C_SDA` | I2C Serial Data line for offline audit logs with dedicated 4.7kΩ hardware pull-up to +3.3V |
| **GPIO 9** | Output | 24C512 EEPROM | `SCL` (Pin 6) | `I2C_SCL` | I2C Serial Clock line (400kHz Fast-Mode) with dedicated 4.7kΩ hardware pull-up to +3.3V |
| **GPIO 10** | Output | MFRC522 RFID | `SDA` (`CS`)| `RFID_CS` | Dedicated Active-Low SPI Chip Select for RFID Reader |
| **GPIO 11** | Output | Shared SPI Bus | `MOSI` / `SDI`| `SPI_MOSI` | Master-Out Slave-In shared between MFRC522 RFID, W5500 Ethernet & ILI9488 TFT Display |
| **GPIO 12** | Output | Shared SPI Bus | `SCK` / `CLK` | `SPI_SCK` | Serial Clock shared between RFID (4MHz), W5500 (15MHz-33MHz), TFT Display (15MHz) & Touch Controller |
| **GPIO 13** | Input | Shared SPI Bus | `MISO` / `T_DO`| `SPI_MISO` | Master-In Slave-Out shared between MFRC522 RFID, W5500 Ethernet & XPT2046 Touch Screen Data Out |
| **GPIO 14** | Output | W5500 Ethernet | `SCS` (`CS`)| `ETH_CS` | Dedicated Active-Low SPI Chip Select for W5500 Ethernet LAN controller |
| **GPIO 15** | Output | 3.5" TFT LCD | `CS` | `TFT_CS` | Dedicated Active-Low SPI Chip Select for ILI9488 Display Controller |
| **GPIO 16** | Input | R503 Fingerprint | `TXD` (White) | `FP_TX_ESP_RX` | Hardware UART2 RX line receiving 57600 baud biometric packet streams from R503 sensor |
| **GPIO 17** | Output | R503 Fingerprint | `RXD` (Green) | `FP_RX_ESP_TX` | Hardware UART2 TX line transmitting enrollment and comparison command packets |
| **GPIO 18** | Output | XPT2046 Touch | `T_CS` | `TOUCH_CS` | Dedicated Active-Low SPI Chip Select for Resistive Touch Screen Controller |
| **GPIO 21** | Input | XPT2046 Touch | `T_IRQ` | `TOUCH_IRQ` | Active-Low Pen Interrupt request input line with 10kΩ pull-up to +3.3V |
| **GPIO 19** | Bi-dir | USB Type-C | `D-` | `USB_D_N` | ESP32-S3 Native USB 2.0 Full-Speed Data- line for OTA, flashing, and JTAG debugging |
| **GPIO 20** | Bi-dir | USB Type-C | `D+` | `USB_D_P` | ESP32-S3 Native USB 2.0 Full-Speed Data+ line |
| **GPIO 0** | Input | Boot Switch | `BOOT` | `ESP_BOOT` | Boot mode selection tactile switch with 10kΩ pull-up to +3.3V |
| **CHIP_PU** | Input | Reset Switch | `EN` | `ESP_EN` | Hardware Reset with 10kΩ pull-up and 1.0µF RC power-on delay capacitor to GND |
| **5V / VBUS** | Power | USB / DC Input | `VBUS` / `VIN`| `+5V` | Main high-power 5.0V supply rail for TFT display backlight, NeoPixels, and LDO regulator input |
| **3.3V** | Power | System Logic | `3V3` / `VCC` | `+3V3` | High-current low-noise 3.3V power rail powering ESP32-S3, R503, W5500, RFID, and 24C512 EEPROM |
| **GND** | Ground | Common Ground | `GND` | `GND` | Low-impedance common ground plane |

---

## 2. Power Infrastructure & Transient Peak Mitigation

### Transient Load Profile Analysis:
- **W5500 SPI Ethernet:** ~150mA current consumption during active TCP/IP transmission and frame buffering.
- **R503 Biometric Sensor:** ~120mA peak current surge during capacitive surface image acquisition and flash template matching.
- **3.5" TFT Backlight:** ~150mA continuous current when active.
- **ESP32-S3 RF Transceiver:** ~350mA–500mA transient bursts during Wi-Fi beacon association and RF packet bursts.
- **Total Peak Requirement:** **~1.0A to 1.5A peak load**.

### Power Circuit Topology:
1. **Dual Power Inputs:**
   - **Primary Input:** USB Type-C receptacle (`J1`) with CC1/CC2 5.1kΩ pull-downs, TVS ESD protection diode (`D1`), and a 1.5A/2.0A PTC resettable fuse (`F1`).
   - **Secondary Input:** 2-Pin Screw Terminal (`J2`) with an SS34 Schottky Barrier Diode (`D2`) for reverse-polarity protection.
2. **High-Current 3.3V Low-Dropout Regulator (`U2`):**
   - AMS1117-3.3 / AP2112K-3.3 capable of delivering continuous 1.0A to 1.5A peak.
   - Input Stage: 100µF 16V Tantalum (`C1`) + 10µF Ceramic (`C2`) + 100nF Ceramic (`C3`).
   - Output Stage: 100µF 10V Tantalum (`C4`) + 22µF Ceramic (`C5`) + 100nF Ceramic (`C6`).
   - Local Decoupling: Dedicated 10µF + 100nF pairs located directly adjacent to the power pins of the W5500 module (`J5`) and the R503 sensor (`J3`).
3. **Firmware-Controlled Inrush Staggering:**
   - Backlight line `TFT_BL` is held LOW during power-up by gate pull-down resistor `R7` (10kΩ). Backlight is only turned ON after Wi-Fi and Ethernet initialization routines finish.

---

## 3. High-Speed Shared SPI Bus Architecture

Four peripherals share the high-speed SPI bus (`SPI2_HOST`):

```
                   ESP32-S3 Microcontroller
          +-----------------------------------------+
          | GPIO 11 (MOSI) --------------------+    |
          | GPIO 12 (SCK)  ---------------+    |    |
          | GPIO 13 (MISO) -----------+   |    |    |
          | GPIO 10 (RFID_CS) -----+  |   |    |    |
          | GPIO 14 (ETH_CS) ---+  |  |   |    |    |
          | GPIO 15 (TFT_CS) -+ |  |  |   |    |    |
          | GPIO 18 (T_CS) -+ | |  |  |   |    |    |
          +-----------------|-|-|--|--|---|----|----+
                            | | |  |  |   |    |
       +--------------------+ | |  |  |   |    |
       | +--------------------+ |  |  |   |    |
       | | +--------------------+  |  |   |    |
       | | | +---------------------+  |   |    |
       v v v v                        v   v    v
     +------------+              +-------------------+
     |   MFRC522  | <--- CS:10---|  Shared SPI Bus   |
     | RFID (4MHz)|              |  MOSI  : GPIO 11  |
     +------------+              |  SCK   : GPIO 12  |
     +------------+              |  MISO  : GPIO 13  |
     |   W5500    | <--- CS:14---|                   |
     | Ethernet   |              +-------------------+
     +------------+                       |
     +------------+                       |
     |  ILI9488   | <--- CS:15------------+
     | TFT (15MHz)|
     +------------+
     +------------+
     |  XPT2046   | <--- CS:18------------+
     |   Touch    |
     +------------+
```

### Bus Timing & Conflict Resolution:
- **MFRC522 RFID:** Runs at 4.0 MHz (SPI Mode 0).
- **W5500 Ethernet:** Runs up to 15–33 MHz (SPI Mode 0).
- **ILI9488 TFT LCD:** Runs at 15.0 MHz (SPI Mode 0 via LovyanGFX).
- **XPT2046 Touch:** Runs at 2.5 MHz (SPI Mode 0).
- **Bus Locking Strategy:** Firmware wraps all SPI transactions with mutex locks (`spi_device_acquire_bus()` or `SPI.beginTransaction()`) ensuring strict isolation.

---

## 4. 24C512 I2C EEPROM Storage Subsystem

- **Memory Capacity:** 64 Kbytes (512 Kbits) organized as 65,536 × 8 bits.
- **I2C Bus Address:** `0x50` (A0, A1, A2 pins hardwired to GND).
- **Write Protect (WP):** Hardwired to GND to permit local logging writes.
- **Pull-up Resistors:** Dedicated 4.7kΩ pull-up resistors (`R3`, `R4`) on `I2C_SDA` (GPIO 8) and `I2C_SCL` (GPIO 9) to ensure 400kHz fast-mode operation without rise-time distortion.
- **Log Entry Struct Format (20 Bytes Aligned):**
  - `timestamp`: 4 Bytes (`uint32_t` Unix epoch)
  - `uid`: 12 Bytes (`char[12]` RFID UID or Biometric template index)
  - `direction`: 1 Byte (`uint8_t` 1 = In, 2 = Out)
  - `status`: 1 Byte (`uint8_t` 1 = On-Time, 2 = Late, 3 = Early, 4 = Accepted, 5 = Denied)
  - `method`: 1 Byte (`uint8_t` 1 = Fingerprint, 2 = RFID, 3 = Manual)
  - `padding`: 1 Byte (`uint8_t` memory alignment)
- **Ring Buffer Capacity:** Up to **3,276 log records** can be stored offline before requiring compaction.

---

## 5. File Manifest & How to Open in KiCad

The complete project is ready to open directly in KiCad 6, 7, or 8:

| Filename | Purpose |
| :--- | :--- |
| **`ESP32_S3_Attendance_System.kicad_pro`** | KiCad Project master file with net class clearances and schematic settings |
| **`ESP32_S3_Attendance_System.kicad_sch`** | Complete A3 schematic diagram containing all 8 functional circuit blocks, fully embedded symbols, nets, and labels |
| **`ESP32_S3_Attendance_System.kicad_pcb`** | Starter PCB layout linked to the schematic netlist |
| **`BOM_ESP32_S3_Attendance_System.csv`** | Comprehensive Bill of Materials with 37 line items, manufacturer references, values, and package footprints |

### Instructions to Open:
1. Launch **KiCad 6, 7, or 8**.
2. Click **File -> Open Project...**
3. Select `ESP32_S3_Attendance_System.kicad_pro` located in `c:\Users\om dagdu bamane\pcb\`.
4. Double-click `ESP32_S3_Attendance_System.kicad_sch` to view and edit the schematic.
5. All symbols are embedded self-contained within the schematic file — no external libraries or broken link issues!
