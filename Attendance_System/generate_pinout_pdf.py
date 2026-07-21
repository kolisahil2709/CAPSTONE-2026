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

def build_pdf(filename):
    # Setup document with 0.5 inch (36 pt) margins for max print area and clean styling
    doc = SimpleDocTemplate(
        filename,
        pagesize=letter,
        rightMargin=36,
        leftMargin=36,
        topMargin=36,
        bottomMargin=36
    )
    
    styles = getSampleStyleSheet()
    
    # Custom styles with a premium color palette (Slate Blue & Teal theme)
    c_primary = colors.HexColor("#1A365D")  # Deep Navy
    c_secondary = colors.HexColor("#0D9488")  # Teal
    c_dark = colors.HexColor("#1F2937")  # Charcoal
    c_light = colors.HexColor("#F3F4F6")  # Light Grey
    c_accent = colors.HexColor("#DC2626")  # Alert Red
    
    title_style = ParagraphStyle(
        'DocTitle',
        parent=styles['Heading1'],
        fontName='Helvetica-Bold',
        fontSize=24,
        leading=28,
        textColor=colors.white,
        alignment=1, # Center
        spaceAfter=15
    )
    
    subtitle_style = ParagraphStyle(
        'DocSubtitle',
        parent=styles['Normal'],
        fontName='Helvetica-Oblique',
        fontSize=12,
        leading=16,
        textColor=colors.white,
        alignment=1,
        spaceAfter=10
    )
    
    section_title = ParagraphStyle(
        'SectionTitle',
        parent=styles['Heading2'],
        fontName='Helvetica-Bold',
        fontSize=14,
        leading=18,
        textColor=c_primary,
        spaceBefore=12,
        spaceAfter=6,
        keepWithNext=True
    )
    
    subsection_title = ParagraphStyle(
        'SubSectionTitle',
        parent=styles['Heading3'],
        fontName='Helvetica-Bold',
        fontSize=11,
        leading=14,
        textColor=c_secondary,
        spaceBefore=8,
        spaceAfter=4,
        keepWithNext=True
    )
    
    body_style = ParagraphStyle(
        'Body',
        parent=styles['Normal'],
        fontName='Helvetica',
        fontSize=9.5,
        leading=13,
        textColor=c_dark,
        spaceAfter=6
    )
    
    bullet_style = ParagraphStyle(
        'Bullet',
        parent=body_style,
        leftIndent=15,
        firstLineIndent=-10,
        spaceAfter=3
    )
    
    warning_style = ParagraphStyle(
        'Warning',
        parent=body_style,
        textColor=c_accent,
        fontName='Helvetica-Bold',
        spaceAfter=6
    )
    
    table_header_style = ParagraphStyle(
        'TableHeader',
        parent=styles['Normal'],
        fontName='Helvetica-Bold',
        fontSize=9,
        leading=11,
        textColor=colors.white
    )
    
    table_cell_style = ParagraphStyle(
        'TableCell',
        parent=styles['Normal'],
        fontName='Helvetica',
        fontSize=8.5,
        leading=11,
        textColor=c_dark
    )
    
    table_cell_bold_style = ParagraphStyle(
        'TableCellBold',
        parent=table_cell_style,
        fontName='Helvetica-Bold'
    )
    
    story = []
    
    # --- HEADER BANNER ---
    header_data = [
        [Paragraph("ESP32-S3 ATTENDANCE SYSTEM", title_style)],
        [Paragraph("Hardware Pinout & Wiring Specification Guide", subtitle_style)]
    ]
    header_table = Table(header_data, colWidths=[540])
    header_table.setStyle(TableStyle([
        ('BACKGROUND', (0,0), (-1,-1), c_primary),
        ('ALIGN', (0,0), (-1,-1), 'CENTER'),
        ('VALIGN', (0,0), (-1,-1), 'MIDDLE'),
        ('TOPPADDING', (0,0), (-1,-1), 15),
        ('BOTTOMPADDING', (0,0), (-1,-1), 15),
        ('BOTTOMMARGIN', (0,0), (-1,-1), 10),
    ]))
    story.append(header_table)
    story.append(Spacer(1, 10))
    
    # --- OVERVIEW ---
    story.append(Paragraph("System Overview", section_title))
    story.append(Paragraph(
        "This guide maps the complete hardware connections of the **ESP32-S3 Smart Attendance System**. "
        "The architecture contains dual input mechanisms (SPI RFID Reader & UART Fingerprint Sensor), "
        "information display via SPI TFT & Touch Display, dynamic status lights, an active buzzer, "
        "and robust network integration (W5500 SPI Ethernet and Wi-Fi).",
        body_style
    ))
    
    # --- MASTER PINOUT TABLE ---
    story.append(Paragraph("ESP32-S3 Master Pinout Sheet", section_title))
    
    # Table headers
    pinout_data = [[
        Paragraph("ESP32-S3 GPIO", table_header_style),
        Paragraph("Direction", table_header_style),
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
        ("GPIO 8", "Bi-directional", "24C512 EEPROM", "SDA / Pin 5", "I2C Serial Data line for logs"),
        ("GPIO 9", "Output", "24C512 EEPROM", "SCL / Pin 6", "I2C Serial Clock line for logs"),
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
        ("5V / VBUS", "Power Out/In", "TFT Display", "VCC / VIN", "High-power rail for TFT display and backlight"),
        ("3.3V / 3V3", "Power Out", "RFID, ETH, R503, EEPROM", "VCC / Pin 8", "Operating voltage rail for logical circuits"),
        ("GND", "Ground", "All Components", "GND / Pin 4", "Common electrical ground reference for the whole system"),
    ]
    
    for row in connections:
        pinout_data.append([
            Paragraph(row[0], table_cell_bold_style),
            Paragraph(row[1], table_cell_style),
            Paragraph(row[2], table_cell_bold_style),
            Paragraph(row[3], table_cell_style),
            Paragraph(row[4], table_cell_style)
        ])
    
    # Define widths adding up to 540 pt (7.5 inches printable area)
    col_widths = [75, 60, 120, 75, 210]
    
    pinout_table = Table(pinout_data, colWidths=col_widths, repeatRows=1)
    
    t_style = TableStyle([
        ('BACKGROUND', (0,0), (-1,0), c_primary),
        ('ALIGN', (0,0), (-1,-1), 'LEFT'),
        ('VALIGN', (0,0), (-1,-1), 'MIDDLE'),
        ('TOPPADDING', (0,0), (-1,-1), 5),
        ('BOTTOMPADDING', (0,0), (-1,-1), 5),
        ('LEFTPADDING', (0,0), (-1,-1), 6),
        ('RIGHTPADDING', (0,0), (-1,-1), 6),
        ('GRID', (0,0), (-1,-1), 0.5, colors.HexColor("#D1D5DB")),
    ])
    
    # Alternating row background colors
    for i in range(1, len(pinout_data)):
        bg = c_light if i % 2 == 1 else colors.white
        t_style.add('BACKGROUND', (0, i), (-1, i), bg)
        
    pinout_table.setStyle(t_style)
    story.append(pinout_table)
    
    # Add page break to keep interface details and layout on Page 2 cleanly
    story.append(PageBreak())
    
    # --- PAGE 2 HEADER (Simple text) ---
    page2_header_data = [[
        Paragraph("ESP32-S3 Smart Attendance System — Interface & Power Specifications", ParagraphStyle(
            'P2Header', parent=styles['Normal'], fontName='Helvetica-Bold', fontSize=10, textColor=c_primary
        ))
    ]]
    p2_header_table = Table(page2_header_data, colWidths=[540])
    p2_header_table.setStyle(TableStyle([
        ('LINEBELOW', (0,0), (-1,-1), 1, c_secondary),
        ('BOTTOMPADDING', (0,0), (-1,-1), 4),
        ('BOTTOMMARGIN', (0,0), (-1,-1), 10),
    ]))
    story.append(p2_header_table)
    
    # --- INTERFACE SPECIFIC GROUPS ---
    story.append(Paragraph("Detailed Interface Wiring & Protocols", section_title))
    
    # SPI Bus
    story.append(Paragraph("1. Shared SPI Bus (RFID, Ethernet, TFT & Touch)", subsection_title))
    story.append(Paragraph(
        "To save GPIO pins, the **MFRC522 RFID Card Reader**, **W5500 SPI Ethernet Module**, **TFT Display**, and **Touch Controller** share the main clock and data lines in parallel:",
        body_style
    ))
    story.append(Paragraph("• <b>Clock (SCK):</b> Connected to <b>GPIO 12</b> on all SPI modules.", bullet_style))
    story.append(Paragraph("• <b>Master-In-Slave-Out (MISO / T_DOUT):</b> Connected to <b>GPIO 13</b>. Used by RFID, Ethernet, and Touch (T_DOUT).", bullet_style))
    story.append(Paragraph("• <b>Master-Out-Slave-In (MOSI):</b> Connected to <b>GPIO 11</b> on all SPI modules.", bullet_style))
    story.append(Paragraph("• <b>Chip Selects (CS):</b> Dedicated lines. RFID is selected via <b>GPIO 10</b>; Ethernet is selected via <b>GPIO 14</b>; TFT CS is selected via <b>GPIO 15</b>; Touch CS is selected via <b>GPIO 18</b>.", bullet_style))
    
    # TFT & Touch
    story.append(Paragraph("2. TFT Display & Touch Controller Interface", subsection_title))
    story.append(Paragraph(
        "The system incorporates a 3.5\" TFT Display with Touch Controller for dynamic status visualization and interactive feedback:",
        body_style
    ))
    story.append(Paragraph("• <b>TFT CS:</b> <b>GPIO 15</b> - Selects the TFT display controller.", bullet_style))
    story.append(Paragraph("• <b>TFT RS/DC:</b> <b>GPIO 7</b> - Register Select / Data-Command control line.", bullet_style))
    story.append(Paragraph("• <b>TFT RST:</b> <b>GPIO 6</b> - Hardware reset pin.", bullet_style))
    story.append(Paragraph("• <b>TFT BL:</b> <b>GPIO 1</b> - Backlight control line (Active-High).", bullet_style))
    story.append(Paragraph("• <b>Touch CS (T_CS):</b> <b>GPIO 18</b> - Selects the Touch controller.", bullet_style))
    story.append(Paragraph("• <b>Touch IRQ (T_IRQ):</b> <b>GPIO 21</b> - Touch Interrupt request line.", bullet_style))
    
    # Serial & I2C
    story.append(Paragraph("3. Fingerprint (UART) & EEPROM (I2C)", subsection_title))
    story.append(Paragraph("• <b>R503 Fingerprint Sensor:</b> Connected via Serial Channel 2. Sensor <b>TXD</b> (White) to ESP32 RXD2 (<b>GPIO 16</b>); Sensor <b>RXD</b> (Green) to ESP32 TXD2 (<b>GPIO 17</b>). Powered by clean <b>3.3V</b>.", bullet_style))
    story.append(Paragraph("• <b>External I2C EEPROM (24C512):</b> Used for offline logging. SDA connects to <b>GPIO 8</b>; SCL connects to <b>GPIO 9</b>. WP pin is grounded for writing, and address lines A0, A1 are grounded (address 0x50).", bullet_style))
    
    # --- POWER CRITICAL STRATEGY ---
    story.append(Spacer(1, 10))
    
    warning_box_data = [[
        Paragraph("⚡ CRITICAL SYSTEM DESIGN & POWER SUPPLY REMINDERS", ParagraphStyle(
            'WarnTitle', parent=styles['Normal'], fontName='Helvetica-Bold', fontSize=10, textColor=colors.HexColor("#991B1B"), spaceAfter=4
        )),
        Paragraph(
            "<b>1. Transient Current Peak Handling:</b> The W5500 SPI Ethernet module draws up to 150mA during active network transmissions. The R503 Fingerprint Sensor spikes up to 120mA when capturing images. Additionally, the TFT 3.5\" Display Backlight draws up to 150mA when active.<br/>"
            "<b>2. USB Power Source:</b> The system MUST be powered by a high-quality external power adapter or a USB port capable of delivering at least <b>1.0A to 1.5A continuous current</b>. Standard computer USB 2.0 ports (500mA max) may experience brownouts.<br/>"
            "<b>3. Logic Level Translation:</b> Ensure that shared SPI devices like the RFID and W5500 modules are strictly operating on 3.3V to prevent cross-talk and permanent chip damage.",
            body_style
        )
    ]]
    
    warning_table = Table(warning_box_data, colWidths=[540])
    warning_table.setStyle(TableStyle([
        ('BACKGROUND', (0,0), (-1,-1), colors.HexColor("#FEF2F2")),
        ('BORDER', (0,0), (-1,-1), 1.5, colors.HexColor("#FCA5A5")),
        ('VALIGN', (0,0), (-1,-1), 'TOP'),
        ('TOPPADDING', (0,0), (-1,-1), 10),
        ('BOTTOMPADDING', (0,0), (-1,-1), 10),
        ('LEFTPADDING', (0,0), (-1,-1), 10),
        ('RIGHTPADDING', (0,0), (-1,-1), 10),
    ]))
    
    story.append(KeepTogether([warning_table]))
    
    # Build PDF
    doc.build(story)
    print(f"Successfully generated PDF: {filename}")

if __name__ == "__main__":
    pdf_filename = "ESP32-S3_Attendance_System_Pinout.pdf"
    build_pdf(pdf_filename)
