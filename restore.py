import os

ino_path = r"C:\Users\Anurag Mourya\Desktop\Attendance_System\Attendance_System.ino"

with open(ino_path, "r", encoding="utf-8") as f:
    content = f.read()

# Let's target the exact boundary of the cut.
# The cut starts right after "tft.fillRect(x - 12, y - 4, 25, 10, 0x7BEF);" inside the rainy icon block.
# And ends with "uint16_t bg = 0x10A2;" which is the start of updateTftClock.
target = "tft.fillRect(x - 12, y - 4, 25, 10, 0x7BEF);\n  uint16_t bg = 0x10A2;"

# Let's define the replacement string containing the rainy streaks and all missing functions
replacement = """tft.fillRect(x - 12, y - 4, 25, 10, 0x7BEF);
    
    tft.drawLine(x - 10, y + 8, x - 13, y + 17, TFT_CYAN);
    tft.drawLine(x, y + 10, x - 3, y + 19, TFT_CYAN);
    tft.drawLine(x + 10, y + 8, x + 7, y + 17, TFT_CYAN);
  } 
  else if (condition.indexOf("thunder") != -1 || condition.indexOf("storm") != -1) {
    // Thunderstorm Icon: Very dark clouds with yellow lightning bolt
    tft.fillCircle(x - 12, y - 4, 10, 0x4208);
    tft.fillCircle(x, y - 14, 15, 0x528A);
    tft.fillCircle(x + 12, y - 4, 10, 0x4208);
    tft.fillRect(x - 12, y - 4, 25, 10, 0x4208);
    
    tft.drawLine(x + 3, y + 4, x - 3, y + 13, TFT_YELLOW);
    tft.drawLine(x - 3, y + 13, x + 6, y + 13, TFT_YELLOW);
    tft.drawLine(x + 6, y + 13, x, y + 22, TFT_YELLOW);
  } 
  else if (condition.indexOf("snow") != -1 || condition.indexOf("ice") != -1 || condition.indexOf("sleet") != -1 || condition.indexOf("hail") != -1) {
    // Snowy Icon: Grey/white cloud with falling snow dots
    tft.fillCircle(x - 12, y - 4, 10, 0xD69A);
    tft.fillCircle(x, y - 14, 15, TFT_WHITE);
    tft.fillCircle(x + 12, y - 4, 10, 0xD69A);
    tft.fillRect(x - 12, y - 4, 25, 10, 0xD69A);
    
    tft.fillCircle(x - 8, y + 10, 3, TFT_WHITE);
    tft.fillCircle(x + 1, y + 13, 3, TFT_WHITE);
    tft.fillCircle(x + 10, y + 9, 3, TFT_WHITE);
  } 
  else if (condition.indexOf("cloud") != -1 || condition.indexOf("overcast") != -1 || condition.indexOf("mist") != -1 || condition.indexOf("fog") != -1 || condition.indexOf("haze") != -1) {
    // Cloudy Icon: Fluffy grey and white clouds
    tft.fillCircle(x - 12, y + 3, 10, 0xD69A);
    tft.fillCircle(x, y - 8, 15, TFT_WHITE);
    tft.fillCircle(x + 12, y + 3, 10, 0xD69A);
    tft.fillRect(x - 12, y + 3, 25, 10, 0xD69A);
  } 
  else {
    // Sunny (Day) or Crescent Moon (Night)
    if (isNight) {
      tft.fillCircle(x - 3, y, 16, TFT_YELLOW);
      tft.fillCircle(x + 4, y - 2, 15, bg);
    } else {
      tft.fillCircle(x, y, 13, TFT_YELLOW);
      for (int angle = 0; angle < 360; angle += 45) {
        float rad = angle * 0.0174532925f; // DEG_TO_RAD
        int x1 = x + 17 * cos(rad);
        int y1 = y + 17 * sin(rad);
        int x2 = x + 24 * cos(rad);
        int y2 = y + 24 * sin(rad);
        tft.drawLine(x1, y1, x2, y2, TFT_YELLOW);
      }
    }
  }
}

void drawTftDefaultScreen() {
  tft.fillScreen(0x0821); // Custom premium dark navy-blue background

  // 1. Draw Sleek Header Bar with Underline Glow
  tft.fillRect(0, 0, 320, 48, 0x018C); // Premium Teal header bar background
  tft.drawFastHLine(0, 48, 320, 0x03EF); // Glowing teal bottom underline
  drawAutoScaledString(deviceName.c_str(), 160, 24, 4, 1.3f, 300, TFT_WHITE);

  // 2. Draw Top Card: Network Info (with Drop Shadow and Double Border Glow)
  // Shifted and expanded slightly: y = 55..165 (height 110)
  tft.fillRoundRect(18, 58, 290, 110, 8, 0x0410); // Dark drop shadow
  tft.fillRoundRect(15, 55, 290, 110, 8, 0x10A2); // Lighter navy card body
  tft.drawRoundRect(15, 55, 290, 110, 8, 0x028A); // Outer border
  tft.drawRoundRect(16, 56, 288, 108, 8, 0x03EF);  // Inner glow border
  
  // Section Header (Network)
  drawAutoScaledString("NETWORK INFO", 160, 70, 2, 1.5f, 270, TFT_CYAN, 0x10A2, true);
  tft.drawFastHLine(25, 86, 270, 0x028A);

  // 3. Draw Middle Card: Time & Status (with Shadow, Glow, and Clock Icon)
  // Shifted and expanded: y = 175..410 (height 235)
  tft.fillRoundRect(18, 178, 290, 235, 8, 0x0410); // Dark drop shadow
  tft.fillRoundRect(15, 175, 290, 235, 8, 0x10A2); // Card body
  tft.drawRoundRect(15, 175, 290, 235, 8, 0x028A); // Outer border
  tft.drawRoundRect(16, 176, 288, 233, 8, 0x03EF);  // Inner glow border
  
  // Section Header (Time)
  drawAutoScaledString("TIME & STATUS", 160, 190, 2, 1.5f, 240, TFT_CYAN, 0x10A2, true);
  tft.drawFastHLine(25, 205, 270, 0x028A);

  // Draw mini analog clock icon at the header
  tft.drawCircle(265, 190, 7, TFT_CYAN);
  tft.drawLine(265, 190, 268, 190, TFT_CYAN);
  tft.drawLine(265, 190, 265, 186, TFT_CYAN);

  // Weather panel vertical separator
  tft.drawFastVLine(100, 295, 105, 0x028A);

  // 4. Draw Footer Standby Banner (with Drop Shadow)
  // Shifted and resized: y = 420..470 (height 50)
  tft.fillRoundRect(18, 423, 290, 50, 6, 0x0410); // Dark drop shadow
  tft.fillRoundRect(15, 420, 290, 50, 6, 0x018C); // Teal banner body
  tft.drawRoundRect(15, 420, 290, 50, 6, 0x03EF); // Glow border
  drawAutoScaledString("ATTENDANCE SYSTEM", 160, 445, 4, 1.3f, 300, TFT_WHITE);

  // Initial update of clock & network status
  tftForceClockRedraw = true;
  updateTftClock();
}

void onBrandingUpdated() {
  if (currentTftState == TFT_IDLE) {
    drawTftDefaultScreen();
  }
}

void drawTftDirectionSelectionScreen(String name) {
  currentTftState = TFT_DIRECTION_SELECT;
  tft.fillScreen(0x0821); // Dark navy background
  
  // Outer frame
  tft.drawRoundRect(15, 15, 290, 450, 12, 0x03EF); // Teal border
  tft.drawRoundRect(16, 16, 288, 448, 12, 0x03EF);
  
  drawAutoScaledString("SELECT DIRECTION", 160, 60, 4, 1.4f, 280, TFT_WHITE);
  tft.drawFastHLine(35, 85, 250, 0x028A);
  
  // Greeting name
  drawAutoScaledString(name.c_str(), 160, 125, 4, 1.4f, 270, TFT_CYAN);
  
  // Draw "PUNCH IN" Button (with shadow & subtitle)
  tft.fillRoundRect(43, 178, 240, 80, 12, 0x0410); // Shadow
  tft.fillRoundRect(40, 175, 240, 80, 12, 0x03A4); // Green card
  tft.drawRoundRect(40, 175, 240, 80, 12, TFT_GREEN);
  drawAutoScaledString("PUNCH IN  >>", 160, 203, 4, 1.3f, 220, TFT_WHITE, 0x03A4, true);
  drawAutoScaledString("(Start Shift)", 160, 233, 2, 1.0f, 220, TFT_GREEN, 0x03A4, true);
  
  // Draw "PUNCH OUT" Button (with shadow & subtitle)
  tft.fillRoundRect(43, 278, 240, 80, 12, 0x0410); // Shadow
  tft.fillRoundRect(40, 275, 240, 80, 12, 0x018C); // Teal/Blue card
  tft.drawRoundRect(40, 275, 240, 80, 12, TFT_CYAN);
  drawAutoScaledString("<<  PUNCH OUT", 160, 303, 4, 1.3f, 220, TFT_WHITE, 0x018C, true);
  drawAutoScaledString("(End Shift)", 160, 333, 2, 1.0f, 220, TFT_CYAN, 0x018C, true);
  
  // Cancel message at bottom
  drawAutoScaledString("Tap outside or wait to cancel", 160, 415, 2, 1.5f, 270, TFT_YELLOW);
}

void updateTftClock() {
  uint16_t bg = 0x10A2;"""

if target in content:
    content = content.replace(target, replacement)
    with open(ino_path, "w", encoding="utf-8") as f:
        f.write(content)
    print("SUCCESS: File successfully patched and functions restored!")
else:
    # Try finding with double spacing or slightly different content
    print("ERROR: Target signature not found in file!")
    # Let's inspect the lines around "fillRect(x - 12, y - 4, 25, 10, 0x7BEF);"
    idx = content.find("fillRect(x - 12, y - 4, 25, 10, 0x7BEF);")
    if idx != -1:
        print("Found matching start of rain block at index", idx)
        # Let's find "bg = 0x10A2;" following it
        idx2 = content.find("bg = 0x10A2;", idx)
        if idx2 != -1:
            print("Found matching end of clock variable at index", idx2)
            # Find the start of the line for bg = 0x10A2;
            line_start = content.rfind("\n", idx, idx2)
            if line_start != -1:
                # We can replace from idx + len("fillRect...") to idx2 + len("bg = 0x10a2;") or similar
                # Let's replace the whole range
                # Wait, the start is tft.fillRect(x - 12, y - 4, 25, 10, 0x7BEF);
                # The end is void updateTftClock() { or void updateTftClock( ...
                # Let's replace from idx + len("fillRect(x - 12, y - 4, 25, 10, 0x7BEF);\n") to the next "uint16_t bg = 0x10A2;" line start
                # E.g. we want to replace the range between idx and the line index of uint16_t bg = 0x10A2;
                bg_line_start = content.rfind("uint16_t bg = 0x10A2;", idx, idx2 + 10)
                if bg_line_start != -1:
                    print("Found bg_line_start at", bg_line_start)
                    # Reconstruct
                    new_content = content[:idx] + "tft.fillRect(x - 12, y - 4, 25, 10, 0x7BEF);\n" + replacement.split("tft.fillRect(x - 12, y - 4, 25, 10, 0x7BEF);\n")[1] + content[bg_line_start + len("uint16_t bg = 0x10A2;"):]
                    with open(ino_path, "w", encoding="utf-8") as f:
                        f.write(new_content)
                    print("SUCCESS: Substring-based replacement successful!")
