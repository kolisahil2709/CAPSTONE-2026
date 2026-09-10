import os
import sys
import subprocess

# Automatically install reportlab if not available
try:
    import reportlab
except ImportError:
    print("reportlab package not found. Installing now...")
    subprocess.check_call([sys.executable, "-m", "pip", "install", "reportlab"])
    import reportlab

from reportlab.lib.pagesizes import letter
from reportlab.platypus import SimpleDocTemplate, Paragraph, Spacer, Table, TableStyle, KeepTogether, PageBreak
from reportlab.lib.styles import getSampleStyleSheet, ParagraphStyle
from reportlab.lib import colors
from reportlab.pdfgen import canvas

class NumberedCanvas(canvas.Canvas):
    """
    Two-pass canvas to calculate total page count and draw consistent
    running headers and footers with page numbers.
    """
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self._saved_page_states = []

    def showPage(self):
        self._saved_page_states.append(dict(self.__dict__))
        self._startPage()

    def save(self):
        num_pages = len(self._saved_page_states)
        for state in self._saved_page_states:
            self.__dict__.update(state)
            self.draw_page_decorations(num_pages)
            super().showPage()
        super().save()

    def draw_page_decorations(self, page_count):
        self.saveState()
        self.setFont("Helvetica", 8)
        self.setFillColor(colors.HexColor("#4B5563")) # Slate/Gray-600
        
        # Skip header and footer on the first page (cover page)
        if self._pageNumber > 1:
            # Running Header
            self.drawString(36, 754, "ESP32-S3 Smart Attendance System — Project Documentation")
            self.setStrokeColor(colors.HexColor("#0D9488")) # Teal Accent
            self.setLineWidth(0.5)
            self.line(36, 748, 576, 748)
            
            # Running Footer
            self.drawRightString(576, 28, f"Page {self._pageNumber} of {page_count}")
            self.drawString(36, 28, "CONFIDENTIAL — TECHNICAL SPECIFICATION REPORT")
            self.setStrokeColor(colors.HexColor("#D1D5DB")) # Light gray divider
            self.setLineWidth(0.5)
            self.line(36, 38, 576, 38)
            
        self.restoreState()


def build_project_pdf(filename):
    # Setup document with 0.5 inch (36 pt) side margins and 54 pt (0.75 in) top/bottom margins
    doc = SimpleDocTemplate(
        filename,
        pagesize=letter,
        rightMargin=36,
        leftMargin=36,
        topMargin=54,
        bottomMargin=54
    )
    
    styles = getSampleStyleSheet()
    
    # Custom color palette matching the UI system
    c_primary = colors.HexColor("#1A365D")    # Deep Navy Blue
    c_secondary = colors.HexColor("#0D9488")  # Teal Accent
    c_dark = colors.HexColor("#1F2937")       # Charcoal Body Text
    c_light = colors.HexColor("#F3F4F6")      # Soft Gray Background
    c_accent_bg = colors.HexColor("#FEF3C7")  # Soft Amber Background (for warnings/notes)
    c_accent_border = colors.HexColor("#F59E0B") # Amber Border
    
    # Title & Header typography
    title_style = ParagraphStyle(
        'DocTitle',
        parent=styles['Heading1'],
        fontName='Helvetica-Bold',
        fontSize=22,
        leading=26,
        textColor=colors.white,
        alignment=1, # Center
        spaceAfter=8
    )
    
    subtitle_style = ParagraphStyle(
        'DocSubtitle',
        parent=styles['Normal'],
        fontName='Helvetica-Oblique',
        fontSize=11,
        leading=15,
        textColor=colors.HexColor("#E2E8F0"), # Slate 200
        alignment=1,
        spaceAfter=4
    )
    
    section_title = ParagraphStyle(
        'SectionTitle',
        parent=styles['Heading2'],
        fontName='Helvetica-Bold',
        fontSize=13,
        leading=16,
        textColor=c_primary,
        spaceBefore=10,
        spaceAfter=5,
        keepWithNext=True
    )
    
    subsection_title = ParagraphStyle(
        'SubSectionTitle',
        parent=styles['Heading3'],
        fontName='Helvetica-Bold',
        fontSize=10,
        leading=13,
        textColor=c_secondary,
        spaceBefore=6,
        spaceAfter=3,
        keepWithNext=True
    )
    
    body_style = ParagraphStyle(
        'Body',
        parent=styles['Normal'],
        fontName='Helvetica',
        fontSize=9,
        leading=12.5,
        textColor=c_dark,
        spaceAfter=5
    )
    
    body_bold = ParagraphStyle(
        'BodyBold',
        parent=body_style,
        fontName='Helvetica-Bold'
    )
    
    bullet_style = ParagraphStyle(
        'Bullet',
        parent=body_style,
        leftIndent=15,
        firstLineIndent=-10,
        spaceAfter=3
    )
    
    table_header_style = ParagraphStyle(
        'TableHeader',
        parent=styles['Normal'],
        fontName='Helvetica-Bold',
        fontSize=8.5,
        leading=10,
        textColor=colors.white
    )
    
    table_cell_style = ParagraphStyle(
        'TableCell',
        parent=styles['Normal'],
        fontName='Helvetica',
        fontSize=8,
        leading=10.5,
        textColor=c_dark
    )
    
    table_cell_bold = ParagraphStyle(
        'TableCellBold',
        parent=table_cell_style,
        fontName='Helvetica-Bold'
    )

    table_cell_code = ParagraphStyle(
        'TableCellCode',
        parent=table_cell_style,
        fontName='Courier',
        fontSize=7.5,
        leading=9
    )
    
    story = []
    
    # ==========================================
    # PAGE 1: COVER & EXECUTIVE OVERVIEW
    # ==========================================
    
    # Header Banner
    header_data = [
        [Paragraph("ESP32-S3 SMART ATTENDANCE SYSTEM", title_style)],
        [Paragraph("Full-Stack Biometric & RFID Access Controller Project Documentation", subtitle_style)]
    ]
    header_table = Table(header_data, colWidths=[540])
    header_table.setStyle(TableStyle([
        ('BACKGROUND', (0,0), (-1,-1), c_primary),
        ('ALIGN', (0,0), (-1,-1), 'CENTER'),
        ('VALIGN', (0,0), (-1,-1), 'MIDDLE'),
        ('TOPPADDING', (0,0), (-1,-1), 16),
        ('BOTTOMPADDING', (0,0), (-1,-1), 16),
    ]))
    story.append(header_table)
    story.append(Spacer(1, 15))
    
    # 1. Executive Summary
    story.append(Paragraph("1. Executive Summary & Overview", section_title))
    story.append(Paragraph(
        "The <b>ESP32-S3 Smart Attendance System</b> is a premium, edge-computing embedded device designed to manage "
        "employee and student attendance in corporate or educational environments. It combines high-performance "
        "microcontroller capabilities with robust physical security protocols, dynamic visual feedback, and "
        "comprehensive software administration.",
        body_style
    ))
    story.append(Paragraph(
        "By utilizing an <b>ESP32-S3 Dual-Core SoC</b>, the system operates as a stand-alone biometric door controller "
        "and logging terminal. It supports contactless card authentication, high-accuracy capacitive fingerprint scanning, "
        "and a 2-Factor Authentication (2FA) mode where card swipes are combined with biometric validation. "
        "The device features both <b>Wi-Fi</b> and <b>W5500 SPI Ethernet</b> for communication, an external "
        "<b>64 KB (24C512) EEPROM</b> for secure local offline log storage, a <b>3.5\" TFT Display with Touch Screen</b> "
        "running a custom state-driven user interface, and a built-in asynchronous HTTP server that hosts a fully compressed "
        "dashboard UI for administrative management, reports, user profiles, shift timers, and OTA firmware updates.",
        body_style
    ))
    
    # 2. High-Level System Architecture
    story.append(Paragraph("2. System Architecture & Block Diagram", section_title))
    story.append(Paragraph(
        "The system's modular architecture separates hardware inputs (sensors), feedback mechanisms (TFT, buzzer, LEDs), "
        "and external storage and networking interfaces. The shared SPI bus integrates the RFID Reader, Ethernet controller, "
        "TFT Display panel, and TFT Touch controller simultaneously, using dedicated chip select lines.",
        body_style
    ))
    
    # Text-based architecture diagram table
    arch_data = [
        [
            Paragraph("<b>Biometric/RFID Inputs</b>", table_cell_bold),
            Paragraph("<b>Processor & Core Logic</b>", table_cell_bold),
            Paragraph("<b>Network & Storage</b>", table_cell_bold)
        ],
        [
            Paragraph("• MFRC522 RFID Reader (SPI)<br/>• R503 Fingerprint Sensor (UART)", table_cell_style),
            Paragraph("<b>ESP32-S3 Xtensa LX7</b><br/>• Dual Core @ 240MHz<br/>• LittleFS Local Flash Storage<br/>• 2-Factor Auth State Machine", table_cell_style),
            Paragraph("• W5500 SPI Ethernet (LAN)<br/>• 2.4GHz Wi-Fi (AP/STA)<br/>• 24C512 EEPROM (I2C Logs)", table_cell_style)
        ],
        [
            Paragraph("<b>Visual & Audio Output</b>", table_cell_bold),
            Paragraph("<b>User Interactions</b>", table_cell_bold),
            Paragraph("<b>Web Dash Management</b>", table_cell_bold)
        ],
        [
            Paragraph("• 3.5\" TFT Display (ILI9488)<br/>• RGB NeoPixel & LEDs<br/>• Active Piezo Buzzer", table_cell_style),
            Paragraph("• XPT2046 Touch Screen Panel<br/>• Standby Clock & Live Weather<br/>• Direction: In / Out selector", table_cell_style),
            Paragraph("• Async Web Server (Port 80)<br/>• Restful API & OTA Uploads<br/>• Shift & Holiday Configurator", table_cell_style)
        ]
    ]
    arch_table = Table(arch_data, colWidths=[175, 190, 175])
    arch_table.setStyle(TableStyle([
        ('BACKGROUND', (0,0), (-1,0), c_secondary),
        ('BACKGROUND', (0,2), (-1,2), c_secondary),
        ('TEXTCOLOR', (0,0), (-1,0), colors.white),
        ('TEXTCOLOR', (0,2), (-1,2), colors.white),
        ('ALIGN', (0,0), (-1,-1), 'LEFT'),
        ('VALIGN', (0,0), (-1,-1), 'TOP'),
        ('GRID', (0,0), (-1,-1), 0.5, colors.HexColor("#CBD5E1")),
        ('TOPPADDING', (0,0), (-1,-1), 6),
        ('BOTTOMPADDING', (0,0), (-1,-1), 6),
        ('LEFTPADDING', (0,0), (-1,-1), 8),
        ('RIGHTPADDING', (0,0), (-1,-1), 8),
    ]))
    
    # Update text colors in header rows for clarity in PDF
    for r in [0, 2]:
        for c in range(3):
            arch_data[r][c].style.textColor = colors.white
            
    story.append(arch_table)
    
    # Page break to start pinout details on Page 2
    story.append(PageBreak())
    
    # ==========================================
    # PAGE 2: HARDWARE WIRING & POWER STRATEGY
    # ==========================================
    
    story.append(Paragraph("3. Hardware Pinout & Interface Wiring", section_title))
    story.append(Paragraph(
        "All peripheral modules are integrated into the ESP32-S3 GPIO layout as follows. The SPI bus operates "
        "cooperatively among the display, touch screen, RFID, and Ethernet controllers.",
        body_style
    ))
    
    # Table headers
    pinout_data = [[
        Paragraph("ESP32-S3 GPIO", table_header_style),
        Paragraph("I/O", table_header_style),
        Paragraph("Connected Module", table_header_style),
        Paragraph("Module Pin", table_header_style),
        Paragraph("Function & Description", table_header_style)
    ]]
    
    # Connection details
    connections = [
        ("GPIO 1", "Output", "TFT Display", "BL", "TFT Backlight control line (Active-High)"),
        ("GPIO 2", "Output", "Green LED", "Anode (+)", "Authorized access success indicator (via 220Ω resistor)"),
        ("GPIO 3", "Output", "MFRC522 RFID", "RST", "Hardware Reset pin to reinitialize the card reader"),
        ("GPIO 4", "Output", "Red LED", "Anode (+)", "Access denied alert indicator (via 220Ω resistor)"),
        ("GPIO 5", "Output", "Active Buzzer", "Positive (+)", "Audible beep feedback for swipe confirmation"),
        ("GPIO 6", "Output", "TFT Display", "RST", "TFT Reset line to initialize the display"),
        ("GPIO 7", "Output", "TFT Display", "RS (DC)", "TFT Register Select (Data/Command select line)"),
        ("GPIO 8", "Bi-dir", "24C512 EEPROM", "SDA / Pin 5", "I2C Serial Data line for offline logs"),
        ("GPIO 9", "Output", "24C512 EEPROM", "SCL / Pin 6", "I2C Serial Clock line for offline logs"),
        ("GPIO 10", "Output", "MFRC522 RFID", "SDA (CS)", "Dedicated SPI Chip Select line (Active-Low)"),
        ("GPIO 11", "Output", "RFID, ETH & TFT", "MOSI", "Shared SPI bus Master-Out Slave-In line"),
        ("GPIO 12", "Output", "RFID, ETH & TFT", "SCK", "Shared SPI bus Serial Clock line"),
        ("GPIO 13", "Input", "RFID, ETH & Touch", "MISO / T_DOUT", "Shared SPI bus Master-In Slave-Out / Touch Data Out"),
        ("GPIO 14", "Output", "W5500 Ethernet", "SCS (CS)", "Dedicated SPI Chip Select line (Active-Low)"),
        ("GPIO 15", "Output", "TFT Display", "CS", "Dedicated SPI Chip Select line (Active-Low)"),
        ("GPIO 16", "Input", "R503 Fingerprint", "TXD (White)", "UART RX2 channel to receive data from sensor"),
        ("GPIO 17", "Output", "R503 Fingerprint", "RXD (Green)", "UART TX2 channel to send commands to sensor"),
        ("GPIO 18", "Output", "TFT Touch", "T_CS", "Dedicated SPI Touch Chip Select line (Active-Low)"),
        ("GPIO 21", "Input", "TFT Touch", "T_IRQ", "TFT Touch Interrupt request input line"),
        ("5V / VBUS", "Power Out", "TFT Display", "VCC / VIN", "High-power rail for TFT display and backlight"),
        ("3.3V / 3V3", "Power Out", "RFID, ETH, R503, EEPROM", "VCC / Pin 8", "Operating voltage rail for logical circuits"),
        ("GND", "Ground", "All Components", "GND / Pin 4", "Common electrical ground reference for the whole system"),
    ]
    
    for row in connections:
        pinout_data.append([
            Paragraph(row[0], table_cell_bold),
            Paragraph(row[1], table_cell_style),
            Paragraph(row[2], table_cell_bold),
            Paragraph(row[3], table_cell_style),
            Paragraph(row[4], table_cell_style)
        ])
    
    pinout_table = Table(pinout_data, colWidths=[75, 45, 115, 75, 230], repeatRows=1)
    
    t_style = TableStyle([
        ('BACKGROUND', (0,0), (-1,0), c_primary),
        ('ALIGN', (0,0), (-1,-1), 'LEFT'),
        ('VALIGN', (0,0), (-1,-1), 'MIDDLE'),
        ('TOPPADDING', (0,0), (-1,-1), 4),
        ('BOTTOMPADDING', (0,0), (-1,-1), 4),
        ('LEFTPADDING', (0,0), (-1,-1), 6),
        ('RIGHTPADDING', (0,0), (-1,-1), 6),
        ('GRID', (0,0), (-1,-1), 0.5, colors.HexColor("#D1D5DB")),
    ])
    
    # Alternating row backgrounds
    for i in range(1, len(pinout_data)):
        bg = c_light if i % 2 == 1 else colors.white
        t_style.add('BACKGROUND', (0, i), (-1, i), bg)
        
    pinout_table.setStyle(t_style)
    story.append(pinout_table)
    story.append(Spacer(1, 10))
    
    # 4. Power Infrastructure Strategy
    story.append(Paragraph("4. Power Infrastructure & Electrical Strategy", section_title))
    story.append(Paragraph(
        "Integrating high-draw peripherals such as a 3.5\" LCD backlight, a capacitive biometrics sensor, and an "
        "Ethernet driver demands specific power management measures to avoid system brownouts, resets, or serial packet losses:",
        body_style
    ))
    
    story.append(Paragraph(
        "• <b>Transient Current Demands:</b> The W5500 SPI Ethernet module draws up to 150mA during active network transmissions. "
        "The R503 Fingerprint Sensor spikes to 120mA during imaging scans. Additionally, the TFT 3.5\" Display Backlight draws up to 150mA. "
        "To accommodate these transient current peaks, the system requires a high-quality external power adapter or a USB port capable "
        "of delivering at least <b>1.0A to 1.5A continuous current</b>. Connecting the device to standard computer USB 2.0 ports (500mA max) "
        "may cause immediate brownout resets when multiple modules are activated.",
        bullet_style
    ))
    story.append(Paragraph(
        "• <b>Hardware Start-Up Control:</b> To mitigate inrush current during booting, the TFT backlight (TFT_BL) is wired to <b>GPIO 1</b> "
        "and is configured as a manual, active-high digital pin. The firmware initializes it ONLY after the core modules, Wi-Fi, "
        "and Ethernet hardware have finished drawing their initial start-up power spikes, preventing bootloop conditions.",
        bullet_style
    ))
    story.append(Paragraph(
        "• <b>SPI Bus Clock Segregation:</b> The display panel and touchscreen share the SPI bus (SPI2_HOST) with the MFRC522 RFID reader "
        "and W5500 Ethernet. The Arduino SPI object (used by MFRC522) operates at 4MHz (Mode 0), whereas the display controller (LovyanGFX) "
        "operates at 15MHz. To prevent bus conflicts and data corruption, the firmware enforces strict bus locking and reconfigures the "
        "SPI parameters before accessing the RFID module.",
        bullet_style
    ))
    
    # Page break to start software details on Page 3
    story.append(PageBreak())
    
    # ==========================================
    # PAGE 3: FIRMWARE & SOFTWARE CAPABILITIES
    # ==========================================
    
    story.append(Paragraph("5. Embedded Software & Firmware Capabilities", section_title))
    
    story.append(Paragraph("A. Non-Blocking TFT State-Driven UI", subsection_title))
    story.append(Paragraph(
        "The firmware implements a non-blocking state machine using the high-performance <b>LovyanGFX</b> graphics library. "
        "This design ensures that time-consuming tasks (such as searching for Wi-Fi networks, establishing network handshakes, "
        "or awaiting fingerprint scans) do not block display redraws or screen transitions. The system state progresses through "
        "the following defined modes: <i>TFT_BOOT</i>, <i>TFT_IDLE</i>, <i>TFT_PUNCH_SUCCESS</i>, <i>TFT_PUNCH_DENIED</i>, "
        "<i>TFT_DIRECTION_SELECT</i>, <i>TFT_WAITING_2FA_FINGER</i>, <i>TFT_ENROLL_SCANNING</i>, and <i>TFT_ENROLL_CONFIRMED</i>. "
        "Additionally, the touchscreen enables interactive, tactile controls for logging attendance direction (Punch-In vs. Punch-Out).",
        body_style
    ))
    
    story.append(Paragraph("B. Two-Factor Authentication (2FA) Workflow", subsection_title))
    story.append(Paragraph(
        "For areas requiring enhanced security, the system features a 2FA workflow (Card + Fingerprint). "
        "When an employee swipes their RFID Card, the display reads their user profile, transitions to the "
        "<i>TFT_WAITING_2FA_FINGER</i> screen, and prompts the user to place their finger on the sensor within a 10-second window. "
        "Access is granted only when both identifiers match, preventing card-sharing or credential fraud.",
        body_style
    ))
    
    story.append(Paragraph("C. External EEPROM Logs & Compact Deletion", subsection_title))
    story.append(Paragraph(
        "The system records all transactions locally to the 24C512 external EEPROM, organized as a ring buffer. "
        "This design provides physical, non-volatile data persistence that is unaffected by firmware updates or main flash formatting.",
        body_style
    ))
    
    # Draw EEPROMLogEntry Struct Table
    struct_data = [
        [
            Paragraph("<b>Field Name</b>", table_header_style),
            Paragraph("<b>Data Type</b>", table_header_style),
            Paragraph("<b>Size</b>", table_header_style),
            Paragraph("<b>Description / Mapping</b>", table_header_style)
        ],
        [
            Paragraph("timestamp", table_cell_code),
            Paragraph("uint32_t", table_cell_style),
            Paragraph("4 Bytes", table_cell_style),
            Paragraph("Unix Epoch timestamp (seconds since 1970)", table_cell_style)
        ],
        [
            Paragraph("uid", table_cell_code),
            Paragraph("char[12]", table_cell_style),
            Paragraph("12 Bytes", table_cell_style),
            Paragraph("RFID UID string or Fingerprint ID template index", table_cell_style)
        ],
        [
            Paragraph("direction", table_cell_code),
            Paragraph("uint8_t", table_cell_style),
            Paragraph("1 Byte", table_cell_style),
            Paragraph("Punch Direction: 1 = Punch In (Shift Start), 2 = Punch Out (Shift End)", table_cell_style)
        ],
        [
            Paragraph("status", table_cell_code),
            Paragraph("uint8_t", table_cell_style),
            Paragraph("1 Byte", table_cell_style),
            Paragraph("Attendance Status: 1=On-Time, 2=Late, 3=Early Exit, 4=Accepted, 5=Denied", table_cell_style)
        ],
        [
            Paragraph("method", table_cell_code),
            Paragraph("uint8_t", table_cell_style),
            Paragraph("1 Byte", table_cell_style),
            Paragraph("Verification Method: 1 = Fingerprint, 2 = RFID Card, 3 = Manual Web Punch", table_cell_style)
        ],
        [
            Paragraph("padding", table_cell_code),
            Paragraph("uint8_t", table_cell_style),
            Paragraph("1 Byte", table_cell_style),
            Paragraph("Structure alignment padding (aligns struct size to exactly 20 bytes)", table_cell_style)
        ],
    ]
    struct_table = Table(struct_data, colWidths=[80, 75, 65, 320])
    struct_table.setStyle(TableStyle([
        ('BACKGROUND', (0,0), (-1,0), c_primary),
        ('ALIGN', (0,0), (-1,-1), 'LEFT'),
        ('VALIGN', (0,0), (-1,-1), 'MIDDLE'),
        ('TOPPADDING', (0,0), (-1,-1), 3),
        ('BOTTOMPADDING', (0,0), (-1,-1), 3),
        ('GRID', (0,0), (-1,-1), 0.5, colors.HexColor("#D1D5DB")),
    ]))
    for i in range(1, len(struct_data)):
        bg = c_light if i % 2 == 1 else colors.white
        struct_table.setStyle(TableStyle([('BACKGROUND', (0, i), (-1, i), bg)]))
        
    story.append(struct_table)
    story.append(Spacer(1, 5))
    
    story.append(Paragraph(
        "<b>In-Place Date-Based Log Compaction:</b> Since the EEPROM is a raw block memory device, deleting logs for a "
        "specific date requires custom wear-leveling management. The firmware copies all logs <i>excluding</i> the target date "
        "into a temporary file on the local flash storage (LittleFS), clears the EEPROM storage, and writes back the remaining logs "
        "sequentially, rewriting the index headers to ensure zero fragmented memory gaps.",
        body_style
    ))
    
    story.append(Paragraph("D. Web Dashboard, Provisioning Portal & OTA Updates", subsection_title))
    story.append(Paragraph(
        "An asynchronous web server runs on Port 80, serving an administration dashboard. If Wi-Fi credentials are "
        "unconfigured or inaccessible, the device initiates a captive portal (AP Mode) at 192.168.4.1. "
        "Administrators can download records in CSV format, add/edit users, manage biometric templates, and upload compiled "
        "firmware binaries (`.bin`) via an OTA interface to update the system remotely.",
        body_style
    ))
    
    story.append(Paragraph("E. Dynamic Online Weather Display", subsection_title))
    story.append(Paragraph(
        "When connected to the internet, the device queries meteorological APIs to fetch local parameters (temperature, wind, and humidity). "
        "The standby screen displays these values dynamically alongside a custom <b>vector weather icon</b> drawn pixel-by-pixel, "
        "representing Sunny, Crescent Moon (night), Cloudy, Rainy, Thunderstorm, or Snowy conditions.",
        body_style
    ))
    
    # Page break to start API routes and project layout details on Page 4
    story.append(PageBreak())
    
    # ==========================================
    # PAGE 4: WEB API & FILE STRUCTURE REFERENCE
    # ==========================================
    
    story.append(Paragraph("6. Web Dashboard HTTP API Reference", section_title))
    story.append(Paragraph(
        "The asynchronous web backend exposes a set of RESTful API endpoints used by the dashboard interface:",
        body_style
    ))
    
    api_data = [
        [
            Paragraph("<b>Endpoint Route</b>", table_header_style),
            Paragraph("<b>Method</b>", table_header_style),
            Paragraph("<b>Request Parameters & Functionality Description</b>", table_header_style)
        ],
        [
            Paragraph("/api/login", table_cell_code),
            Paragraph("POST", table_cell_bold),
            Paragraph("Logs user/admin in, validating credentials stored in auth configuration.", table_cell_style)
        ],
        [
            Paragraph("/list-users", table_cell_code),
            Paragraph("GET", table_cell_bold),
            Paragraph("Retrieves JSON list of all enrolled user profile files in LittleFS.", table_cell_style)
        ],
        [
            Paragraph("/save", table_cell_code),
            Paragraph("GET", table_cell_bold),
            Paragraph("Creates or updates a user profile file (parameters: <i>uid, name, role, roll</i>).", table_cell_style)
        ],
        [
            Paragraph("/delete-user", table_cell_code),
            Paragraph("GET", table_cell_bold),
            Paragraph("Deletes profile text file (parameter: <i>uid</i>) and wipes associated FP template.", table_cell_style)
        ],
        [
            Paragraph("/get-history", table_cell_code),
            Paragraph("GET", table_cell_bold),
            Paragraph("Queries log history stored in EEPROM and returns JSON for dashboard tables.", table_cell_style)
        ],
        [
            Paragraph("/download-logs", table_cell_code),
            Paragraph("GET", table_cell_bold),
            Paragraph("Generates and streams a downloadable comma-separated values (CSV) log file.", table_cell_style)
        ],
        [
            Paragraph("/delete-logs", table_cell_code),
            Paragraph("GET", table_cell_bold),
            Paragraph("Compact deletion of logs belonging to a specific date (parameter: <i>date</i>).", table_cell_style)
        ],
        [
            Paragraph("/start-enroll", table_cell_code),
            Paragraph("GET", table_cell_bold),
            Paragraph("Initiates biometrics sensor recording sequence for a given template ID.", table_cell_style)
        ],
        [
            Paragraph("/set-wifi", table_cell_code),
            Paragraph("GET", table_cell_bold),
            Paragraph("Receives new target SSID and Password and attempts to connect in Station mode.", table_cell_style)
        ],
        [
            Paragraph("/set-shift", table_cell_code),
            Paragraph("GET", table_cell_bold),
            Paragraph("Saves corporate shift work hours (parameters: <i>start, late, end</i>) to LittleFS.", table_cell_style)
        ],
        [
            Paragraph("/upload", table_cell_code),
            Paragraph("POST", table_cell_bold),
            Paragraph("Accepts compiled binary file upload and triggers ESP32 OTA flash write.", table_cell_style)
        ],
    ]
    
    api_table = Table(api_data, colWidths=[110, 50, 380])
    api_table.setStyle(TableStyle([
        ('BACKGROUND', (0,0), (-1,0), c_primary),
        ('ALIGN', (0,0), (-1,-1), 'LEFT'),
        ('VALIGN', (0,0), (-1,-1), 'MIDDLE'),
        ('TOPPADDING', (0,0), (-1,-1), 3),
        ('BOTTOMPADDING', (0,0), (-1,-1), 3),
        ('GRID', (0,0), (-1,-1), 0.5, colors.HexColor("#D1D5DB")),
    ]))
    for i in range(1, len(api_data)):
        bg = c_light if i % 2 == 1 else colors.white
        api_table.setStyle(TableStyle([('BACKGROUND', (0, i), (-1, i), bg)]))
        
    story.append(api_table)
    story.append(Spacer(1, 10))
    
    # 7. Project File Structure
    story.append(Paragraph("7. Project Directory Layout & File Manifest", section_title))
    
    manifest = [
        ("Attendance_System.ino", "The main sketch initializing variables, LovyanGFX display classes, hardware interfaces, RFID interrupt handlers, 2FA workflows, and core operation loop."),
        ("web_routes.ino", "Implements AsyncWebServer callback routes, REST JSON endpoints, network scanning operations, OTA integration, and captive portal redirections."),
        ("eeprom_logger.ino", "Implements raw I2C block read/write operations, log index headers loading, ring-buffer queue insertions, and date-based database compaction."),
        ("eeprom_logger.h", "Declares the structured log format structure (`EEPROMLogEntry`), address layout definitions, and public function signatures."),
        ("html_page_gz.h", "Contains the minified, gzipped, raw hexadecimal byte-array of the HTML, CSS, and JS web dashboard for rapid page delivery."),
        ("compress_html.py", "A helper script to automatically compress the raw frontend assets, converting them to ESP32-compatible header file formats."),
    ]
    
    for item in manifest:
        story.append(Paragraph(f"• <b>{item[0]}:</b> {item[1]}", bullet_style))
        
    story.append(Spacer(1, 8))
    
    # 8. Maintenance Warnings & Operational Notes
    story.append(Paragraph("8. System Warnings & Maintenance Guidelines", section_title))
    
    warning_box_data = [[
        Paragraph("⚡ CRITICAL OPERATIONAL NOTES", ParagraphStyle(
            'WarnTitle', parent=styles['Normal'], fontName='Helvetica-Bold', fontSize=8.5, textColor=colors.HexColor("#991B1B"), spaceAfter=3
        )),
        Paragraph(
            "<b>1. Physical Bus Hardware Pullups:</b> The I2C EEPROM requires hardware pull-up resistors (4.7kΩ) connected to the SDA (GPIO 8) and SCL (GPIO 9) lines. Relying solely on internal ESP32 pull-ups may lead to bus failure at 400kHz speeds.<br/>"
            "<b>2. Watchdog Safeguard Configuration:</b> The ESP32 task watchdog (TWDT) is active during attendance routines. Any operations blocking the main loop for more than 5 seconds will trigger an automatic hardware reboot. In-place EEPROM compaction pauses the watchdog to prevent triggers during flash operations.",
            body_style
        )
    ]]
    
    warning_table = Table(warning_box_data, colWidths=[540])
    warning_table.setStyle(TableStyle([
        ('BACKGROUND', (0,0), (-1,-1), c_accent_bg),
        ('BORDER', (0,0), (-1,-1), 1.2, c_accent_border),
        ('VALIGN', (0,0), (-1,-1), 'TOP'),
        ('TOPPADDING', (0,0), (-1,-1), 8),
        ('BOTTOMPADDING', (0,0), (-1,-1), 8),
        ('LEFTPADDING', (0,0), (-1,-1), 8),
        ('RIGHTPADDING', (0,0), (-1,-1), 8),
    ]))
    
    story.append(KeepTogether([warning_table]))
    
    # Build PDF using our custom NumberedCanvas for clean header/footer layouts
    doc.build(story, canvasmaker=NumberedCanvas)
    print(f"Successfully generated PDF: {filename}")

if __name__ == "__main__":
    pdf_filename = "ESP32-S3_Smart_Attendance_System_Project_Report.pdf"
    build_project_pdf(pdf_filename)
