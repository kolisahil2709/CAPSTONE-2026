#include "eeprom_logger.h"
#include "time.h"
#include <Adafruit_Fingerprint.h>
#include <Arduino.h>
#include <AsyncTCP.h>
#include <WiFiClientSecure.h>
#include <DNSServer.h>
#include <ESPAsyncWebServer.h>
#include <ETH.h>
#include <HTTPClient.h>
#include <LittleFS.h>
#include <MFRC522.h>
#include <SPI.h>
#include <Update.h>
#include <WiFi.h>
#include <esp_task_wdt.h>
#include <driver/spi_master.h>

// SPI device handles removed as manual ESP-IDF bus locking is not needed
size_t fsTotalBytes = 0;
size_t fsUsedBytes = 0;

void updateFsSizes() {
  fsTotalBytes = LittleFS.totalBytes();
  fsUsedBytes = LittleFS.usedBytes();
}

// MFRC522 uses the Arduino SPI object at 4MHz, Mode 0.
// LovyanGFX drives the same bus at 27MHz via the ESP-IDF native driver.
// After any TFT draw the Arduino SPI transaction settings are left at
// 27MHz which corrupts all RFID reads/writes.
// Wrap every RFID SPI access with this macro to restore the correct settings.
#define RFID_SPI_SETTINGS SPISettings(4000000, MSBFIRST, SPI_MODE0)

struct EEPROMLogEntry;
class AsyncWebServerRequest;
extern float outsideTemp;
extern String deviceName;
extern String weatherCond;
extern String weatherHum;
extern String weatherWind;
extern bool tftForceClockRedraw;

void drawTftDefaultScreen();
void updateTftClock();
void drawTftSuccessScreen(String name, String dir, String status, String time, String roll);
void drawTftDeniedScreen(String name, String reason);
void drawTftDirectionSelectionScreen(String name, String roll);
void drawTft2FAPromptScreen(String name, String roll);
void drawTftEnrollScanningScreen(String type, int id, int step, String msg, bool forceRedraw = false);
void drawTftEnrollConfirmedScreen(String name, String roll, String role, String type);
void drawProfilePhoto(String roll, int x, int y, int w, int h, uint16_t bgColor = 0x10A2);
void savePunchRecord(String id, bool isFinger, String selectedDir);
void onBrandingUpdated();
void loadShiftConfig();
void loadAuthConfig();
void saveAuthConfig();
void saveFpCache();
void loadFpCache();
void triggerDeniedLed();
void triggerBuzzer(int durationMs);
void processAccess(String id, bool isFinger);
void autoCleanLogs();
void disableWatchdog();
void setup();
void handleEnrollment();
void loop();

// Debug & Compatibility Configurations
#define DISABLE_RGB_LED                                                        \
  0 // Set to 1 if the onboard RGB NeoPixel causes restarts/crashes

// W5500 SPI Ethernet Configuration
#define ETH_CS_PIN 14  // Chip Select pin for W5500 (Adjust if needed)
#define ETH_INT_PIN -1 // Interrupt pin (optional, -1 if not used)
#define ETH_RST_PIN -1 // Reset pin (optional, -1 if not used)
#include <Wire.h>
#include <SPI.h>

// I2C Configuration for external EEPROM
#define LCD_SDA_PIN 8
#define LCD_SCL_PIN 9

// --- Display Driver Selection ---
// Uncomment the line for your specific TFT display controller:
// #define USE_ILI9486
#define USE_ILI9488 // Default for 3.5" display
// #define USE_ST7796

// TFT Display & Touch Pins Configuration
#define TFT_CS    15 // TFT Chip Select
#define TFT_RS     7 // TFT Register Select (Data/Command)
#define TFT_RST    6 // TFT Reset
#define TFT_BL     1 // TFT Backlight control pin
#define TFT_MOSI  11 // Shared SPI MOSI
#define TFT_SCLK  12 // Shared SPI SCLK
#define TFT_MISO  13 // Shared SPI MISO / T_DOUT
#define T_CS      18 // Touch CS
#define T_IRQ     21 // Touch IRQ

#define LGFX_USE_V1
#include <LovyanGFX.hpp>

class LGFX : public lgfx::LGFX_Device {
  lgfx::Bus_SPI        _bus_instance;
#if defined(USE_ILI9486)
  lgfx::Panel_ILI9486  _panel_instance;
#elif defined(USE_ST7796)
  lgfx::Panel_ST7796   _panel_instance;
#else
  lgfx::Panel_ILI9488  _panel_instance; // Default ILI9488
#endif
  lgfx::Light_PWM      _light_instance;
  lgfx::Touch_XPT2046  _touch_instance;

public:
  LGFX(void) {
    {
      auto cfg = _bus_instance.config();
      cfg.spi_host = SPI2_HOST;     // Use FSPI (SPI2_HOST)
      cfg.spi_mode = 0;
      cfg.freq_write = 15000000;    // 15MHz write speed (eliminates ghosting/signal reflections on ILI9488)
      cfg.freq_read  = 16000000;    // 16MHz read speed
      cfg.pin_sclk = TFT_SCLK;      // Shared SPI Clock
      cfg.pin_mosi = TFT_MOSI;      // Shared SPI MOSI
      cfg.pin_miso = TFT_MISO;      // Shared SPI MISO (T_DOUT)
      cfg.pin_dc   = TFT_RS;        // TFT RS/DC (GPIO 7) - Configured on the bus
      cfg.dma_channel = 0;          // Disable DMA to prevent background transfers from colliding with Ethernet driver on the shared SPI bus
      _bus_instance.config(cfg);
      _panel_instance.setBus(&_bus_instance);
    }
    {
      auto cfg = _panel_instance.config();
      cfg.pin_cs           = TFT_CS;  // TFT CS (GPIO 15)
      cfg.pin_rst          = TFT_RST; // TFT RST (GPIO 6)
      cfg.panel_width      = 320;     // Physical panel width
      cfg.panel_height     = 480;     // Physical panel height
      cfg.offset_x         = 0;
      cfg.offset_y         = 0;
      cfg.readable         = false;
      cfg.invert           = false;
      cfg.rgb_order        = false;   // Set to true/false depending on panel color order
      cfg.bus_shared       = true;    // Enable SPI bus sharing lock for the display panel
      _panel_instance.config(cfg);
    }
    // Backlight configuration commented out to enable 100% manual control
    // of the TFT_BL pin. This prevents the TFT backlight from drawing startup
    // current during initial power-up.
    /*
    {
      auto cfg = _light_instance.config();
      cfg.pin_bl = TFT_BL;          // TFT BL (GPIO 1)
      cfg.freq   = 12000;           // PWM frequency
      cfg.pwm_channel = 1;          // PWM channel
      cfg.invert = false;           // Active High
      _light_instance.config(cfg);
      _panel_instance.setLight(&_light_instance);
    }
    */
    {
      auto cfg = _touch_instance.config();
      cfg.x_min      = 300;
      cfg.x_max      = 3900;
      cfg.y_min      = 200;
      cfg.y_max      = 3800;
      cfg.pin_int    = T_IRQ;       // T_IRQ (GPIO 21)
      cfg.bus_shared = true;        // Shares the SPI bus with the panel
      cfg.spi_host   = SPI2_HOST;
      cfg.pin_sclk   = TFT_SCLK;    // Corrected from pin_scl
      cfg.pin_mosi   = TFT_MOSI;    // Corrected from pin_sda
      cfg.pin_miso   = TFT_MISO;    // T_DOUT
      cfg.pin_cs     = T_CS;        // T_CS (GPIO 18)
      cfg.freq       = 1000000;     // SPI speed for touch controller (1MHz)
      _touch_instance.config(cfg);
      _panel_instance.setTouch(&_touch_instance);
    }
    setPanel(&_panel_instance);
  }
};

LGFX tft;

// Non-blocking TFT state variables
unsigned long tftMessageUntil = 0; // Holds high-priority punch screen
enum TftState { TFT_BOOT, TFT_IDLE, TFT_PUNCH_SUCCESS, TFT_PUNCH_DENIED, TFT_DIRECTION_SELECT, TFT_WAITING_2FA_FINGER, TFT_ENROLL_SCANNING, TFT_ENROLL_CONFIRMED };
TftState currentTftState = TFT_BOOT;
bool tftForceClockRedraw = false;
volatile bool tftWasPowerCycled = false;

// Pending punch state variables for touchscreen selection
bool hasPendingPunch = false;
String pendingPunchId = "";
bool pendingPunchIsFinger = false;
unsigned long pendingPunchTimeout = 0;

// 2FA (Card + Fingerprint) State Variables
bool twoFactorEnabled = false;
String pending2FAFingerId = "";
String pending2FACardUid = "";
unsigned long pending2FATimeout = 0;
String pending2FAName = "";
String pending2FARoll = "";
String pendingPunchName = "";
String pendingPunchRoll = "";

// Helper to retrieve the current active network IP address (prioritizing LAN
// over WiFi)
String getActiveIPAddress() {
  bool ethConnected = false;
#if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0)
  ethConnected = (ETH.handle() != NULL) && ETH.linkUp();
#else
  ethConnected = (ETH.localIP() != IPAddress(0, 0, 0, 0));
#endif

  if (ethConnected) {
    return ETH.localIP().toString();
  } else if (WiFi.status() == WL_CONNECTED) {
    return WiFi.localIP().toString();
  } else {
    return WiFi.softAPIP().toString();
  }
}

// Draw a large calendar icon (28x28 px) at (centerX, centerY)
void drawCalendarIcon(int centerX, int centerY, uint16_t color) {
  // Calendar base plate (white card with rounded corners)
  tft.fillRoundRect(centerX - 14, centerY - 14, 28, 28, 4, TFT_WHITE);
  tft.drawRoundRect(centerX - 14, centerY - 14, 28, 28, 4, 0x528A); // Darker border
  
  // Red calendar header bar
  tft.fillRoundRect(centerX - 14, centerY - 14, 28, 8, 2, TFT_RED);
  tft.fillRect(centerX - 14, centerY - 8, 28, 2, TFT_RED); // fill bottom rounded edge
  
  // Binder loops (metallic look using silver/light grey color)
  tft.fillRoundRect(centerX - 9, centerY - 17, 3, 6, 1, 0xBDD7);
  tft.fillRoundRect(centerX + 6, centerY - 17, 3, 6, 1, 0xBDD7);
  
  // Grid lines/dots representing dates (Blue-grey color)
  uint16_t gridColor = 0x528A;
  tft.fillRect(centerX - 9, centerY - 1, 3, 3, gridColor);
  tft.fillRect(centerX - 2, centerY - 1, 3, 3, gridColor);
  tft.fillRect(centerX + 5, centerY - 1, 3, 3, gridColor);
  
  tft.fillRect(centerX - 9, centerY + 5, 3, 3, gridColor);
  tft.fillRect(centerX - 2, centerY + 5, 3, 3, gridColor);
  tft.fillRect(centerX + 5, centerY + 5, 3, 3, gridColor);

  tft.fillRect(centerX - 9, centerY + 11, 3, 3, gridColor);
  tft.fillRect(centerX - 2, centerY + 11, 3, 3, gridColor);
  tft.fillRect(centerX + 5, centerY + 11, 3, 3, gridColor);
}

// Draw a large premium network icon (36x36 px) at (centerX, centerY)
void drawNetworkIcon(int centerX, int centerY, uint16_t color, bool isWiFi) {
  uint16_t bg = 0x10A2;
  // Clear the icon bounding box (36x36 px centered at centerX, centerY)
  tft.fillRect(centerX - 18, centerY - 18, 36, 36, bg);
  
  if (isWiFi) {
    // Large premium Wi-Fi Logo (Concentric Arcs pointing upwards)
    // Using fillArc for clean, thick arcs instead of thin lines
    int base_y = centerY + 12;
    tft.fillCircle(centerX, base_y, 3, color);
    tft.fillArc(centerX, base_y, 7, 10, 225, 315, color);
    tft.fillArc(centerX, base_y, 14, 17, 225, 315, color);
    tft.fillArc(centerX, base_y, 21, 24, 225, 315, color);
  } else {
    // Large premium Ethernet RJ45 socket / plug logo (with gold pins detail)
    tft.drawRoundRect(centerX - 14, centerY - 14, 28, 28, 4, color);
    tft.drawRect(centerX - 10, centerY - 10, 20, 20, color);
    tft.fillRoundRect(centerX - 8, centerY - 6, 16, 12, 2, color);
    // Gold contact pins
    for (int i = -6; i <= 6; i += 3) {
      tft.drawFastVLine(centerX + i, centerY - 4, 4, 0xFDA0);
    }
  }
}

// Draw a large padlock vector at the center of the Denied Screen frame
void drawLockVector(int centerX, int centerY) {
  // Shackle (Arc)
  tft.drawCircle(centerX, centerY - 6, 12, TFT_WHITE);
  tft.drawCircle(centerX, centerY - 6, 11, TFT_WHITE);
  tft.fillRect(centerX - 12, centerY, 24, 8, 0x2000); // Mask bottom half of shackle
  
  // Padlock Body
  tft.fillRoundRect(centerX - 18, centerY, 36, 28, 4, 0xD000); // Premium dark red body
  tft.drawRoundRect(centerX - 18, centerY, 36, 28, 4, TFT_RED);
  
  // Keyhole
  tft.fillCircle(centerX, centerY + 8, 3, TFT_BLACK);
  tft.fillRect(centerX - 1, centerY + 10, 3, 8, TFT_BLACK);
}

// Draw a large thermometer icon (24x36 px) at (centerX, centerY)
void drawTempIcon(int centerX, int centerY, uint16_t color, float temp, uint16_t bg) {
  // Clear the icon area (24x36 px centered at centerX, centerY)
  tft.fillRect(centerX - 12, centerY - 18, 24, 36, bg);

  // Outer glass tube frame (rounded cap at top)
  tft.fillRoundRect(centerX - 5, centerY - 14, 10, 24, 5, color);
  tft.fillRoundRect(centerX - 3, centerY - 12, 6, 20, 3, bg);
  
  tft.fillCircle(centerX, centerY + 10, 8, color);
  tft.fillCircle(centerX, centerY + 10, 6, TFT_RED); // Bulb body (red)
  
  // Connect bulb and tube
  tft.fillRect(centerX - 2, centerY + 4, 4, 3, TFT_RED);
  
  // Calculate dynamic mercury height (range 0 to 45°C mapped to 0 to 14 pixels)
  int mercuryHeight = (int)((temp / 45.0f) * 14.0f);
  if (mercuryHeight < 0) mercuryHeight = 0;
  if (mercuryHeight > 14) mercuryHeight = 14;
  
  // Draw mercury in the tube
  if (mercuryHeight > 0) {
    tft.fillRect(centerX - 2, centerY + 4 - mercuryHeight, 4, mercuryHeight, TFT_RED);
  }

  // Ticks on left side representing temperature scale
  tft.drawFastHLine(centerX - 9, centerY - 6, 3, color);
  tft.drawFastHLine(centerX - 9, centerY, 3, color);
  tft.drawFastHLine(centerX - 9, centerY + 6, 3, color);
}

void drawAutoScaledString(const String& text, int centerX, int centerY, int font, float maxScale, int maxWidth, uint16_t color, uint16_t bg = 0, bool useBg = false) {
  if (font == 1) tft.setFont(&fonts::Font0);
  else if (font == 2) tft.setFont(&fonts::Font2);
  else if (font == 4) tft.setFont(&fonts::Font4);
  else if (font == 6) tft.setFont(&fonts::Font6);
  
  int intScale = (int)maxScale;
  if (intScale < 1) intScale = 1;
  
  tft.setTextSize((float)intScale);
  int w = tft.textWidth(text.c_str());
  
  while (w > maxWidth && intScale > 1) {
    intScale--;
    tft.setTextSize((float)intScale);
    w = tft.textWidth(text.c_str());
  }
  
  tft.setTextDatum(MC_DATUM);
  if (useBg) {
    tft.setTextColor(color, bg);
  } else {
    tft.setTextColor(color);
  }
  tft.drawString(text.c_str(), centerX, centerY);
  tft.setTextSize(1.0f); // Reset text size scale
}

// Helper function to draw a line of centered text inside a card, clearing its previous text background first
int drawCardLine(const String& text, int x, int y, int font, uint16_t color, uint16_t bg, int fontHeight) {
  if (font == 1) tft.setFont(&fonts::Font0);
  else if (font == 2) tft.setFont(&fonts::Font2);
  else if (font == 4) tft.setFont(&fonts::Font4);
  else if (font == 6) tft.setFont(&fonts::Font6);
  
  int intScale = 1;
  tft.setTextSize((float)intScale);
  int w = tft.textWidth(text.c_str());
  
  int clearHeight = fontHeight * intScale + 6;
  // Clear the text area with background color to prevent text overlapping/ghosting.
  // We use 270 width to ensure complete coverage of the scaled text.
  tft.fillRect(x - 135, y - clearHeight / 2, 270, clearHeight, bg);
  tft.setTextColor(color, bg);
  tft.setTextDatum(MC_DATUM);
  tft.drawString(text.c_str(), x, y);
  tft.setTextSize(1.0f); // Reset text size scale
  return w;
}

// Helper function to draw left-aligned text inside a card, clearing its previous text background first
void drawCardLineLeft(const String& text, int x, int y, int font, float maxScale, uint16_t color, uint16_t bg, int fontHeight) {
  if (font == 1) tft.setFont(&fonts::Font0);
  else if (font == 2) tft.setFont(&fonts::Font2);
  else if (font == 4) tft.setFont(&fonts::Font4);
  else if (font == 6) tft.setFont(&fonts::Font6);
  
  int intScale = (int)maxScale;
  if (intScale < 1) intScale = 1;
  
  tft.setTextSize((float)intScale);
  int w = tft.textWidth(text.c_str());
  int maxWidth = 305 - 10 - x; // Clear margin boundary
  
  while (w > maxWidth && intScale > 1) {
    intScale--;
    tft.setTextSize((float)intScale);
    w = tft.textWidth(text.c_str());
  }
  
  int clearHeight = fontHeight * intScale + 6;
  tft.fillRect(x, y - clearHeight / 2, maxWidth, clearHeight, bg); // Clear up to card right edge margin
  tft.setTextColor(color, bg);
  tft.setTextDatum(ML_DATUM); // Middle Left alignment
  tft.drawString(text.c_str(), x, y);
  tft.setTextSize(1.0f); // Reset text size scale
}

// Helper function to draw vector weather icons (Sunny, Moon, Cloudy, Rainy, Thunderstorm, Snowy)
void drawWeatherIcon(int centerX, int centerY, String condition, bool isNight) {
  uint16_t bg = 0x10A2; // Navy-blue card body color
  // Clear the icon area first (64x64 bounding box centered at centerX, centerY)
  tft.fillRect(centerX - 32, centerY - 32, 64, 64, bg);

  condition.toLowerCase();

  if (condition.indexOf("local") != -1 || condition.indexOf("offline") != -1 || condition.equals("")) {
    // Draw a premium local mode / offline icon: a clean wireless router with a slash
    // Base/stand
    tft.fillRoundRect(centerX - 16, centerY + 12, 32, 6, 2, 0x528A);
    // Router body
    tft.fillRoundRect(centerX - 20, centerY + 4, 40, 8, 3, 0xBDF7);
    // Small LED lights on router
    tft.fillCircle(centerX - 12, centerY + 8, 1, 0x07FF); // Cyan LED
    tft.fillCircle(centerX - 6, centerY + 8, 1, 0x07FF);
    tft.fillCircle(centerX, centerY + 8, 1, 0x07FF);
    tft.fillCircle(centerX + 12, centerY + 8, 1, 0xF800); // Red error LED
    
    // Antennas
    tft.drawLine(centerX - 15, centerY + 4, centerX - 18, centerY - 8, 0x528A);
    tft.drawLine(centerX + 15, centerY + 4, centerX + 18, centerY - 8, 0x528A);
    
    // Disconnected Slash (Red)
    tft.drawLine(centerX - 18, centerY + 18, centerX + 18, centerY - 18, 0xF800);
    tft.drawLine(centerX - 17, centerY + 18, centerX + 19, centerY - 18, 0xF800);
  }
  else if (condition.indexOf("rain") != -1 || condition.indexOf("drizzle") != -1 || condition.indexOf("shower") != -1) {
    // Rainy Icon: Layered 3D clouds + detailed teardrop raindrops + peeking Sun/Moon
    if (isNight) {
      // Crescent Moon peeking from behind
      tft.fillCircle(centerX + 14, centerY - 12, 12, 0xFEA0); // Gold moon
      tft.fillCircle(centerX + 20, centerY - 14, 10, bg);
    } else {
      // Glowing Sun peeking from behind
      tft.fillCircle(centerX + 12, centerY - 12, 14, 0xFD20); // Orange outer
      tft.fillCircle(centerX + 12, centerY - 12, 10, 0xFFE0); // Yellow inner
    }
    
    // 3D Layered Cloud (Back dark shadow cloud + Front bright cloud)
    // Back cloud
    tft.fillCircle(centerX - 10, centerY - 4, 12, 0x3186);
    tft.fillCircle(centerX + 8, centerY - 4, 10, 0x3186);
    tft.fillRect(centerX - 10, centerY - 4, 18, 11, 0x3186);
    
    // Front cloud body (Light slate/blue)
    tft.fillCircle(centerX - 14, centerY + 6, 12, 0xBDF7);
    tft.fillCircle(centerX + 14, centerY + 6, 10, 0xBDF7);
    tft.fillRect(centerX - 14, centerY + 6, 28, 11, 0xBDF7);
    // Front cloud center puff (White highlight)
    tft.fillCircle(centerX + 2, centerY - 4, 16, 0xFFFF);
    tft.fillRect(centerX - 14, centerY - 4, 32, 21, 0xFFFF);
    // Redraw bottom part of cloud to merge cleanly
    tft.fillCircle(centerX - 14, centerY + 6, 12, 0xBDF7);
    tft.fillCircle(centerX + 14, centerY + 6, 10, 0xBDF7);
    tft.fillRect(centerX - 14, centerY + 6, 28, 11, 0xBDF7);

    // High-Fidelity Teardrop Raindrops (Cyan)
    // Raindrop 1 (Left)
    tft.fillCircle(centerX - 12, centerY + 24, 2, 0x07FF);
    tft.fillTriangle(centerX - 12, centerY + 20, centerX - 14, centerY + 24, centerX - 10, centerY + 24, 0x07FF);
    // Raindrop 2 (Center)
    tft.fillCircle(centerX, centerY + 26, 2, 0x07FF);
    tft.fillTriangle(centerX, centerY + 22, centerX - 2, centerY + 26, centerX + 2, centerY + 26, 0x07FF);
    // Raindrop 3 (Right)
    tft.fillCircle(centerX + 12, centerY + 24, 2, 0x07FF);
    tft.fillTriangle(centerX + 12, centerY + 20, centerX + 10, centerY + 24, centerX + 14, centerY + 24, 0x07FF);
  } 
  else if (condition.indexOf("thunder") != -1 || condition.indexOf("storm") != -1) {
    // Thunderstorm Icon: Very dark slate storm clouds + solid jagged lightning bolt
    tft.fillCircle(centerX - 10, centerY - 4, 12, 0x2104); // Dark purple-grey
    tft.fillCircle(centerX + 8, centerY - 4, 10, 0x2104);
    tft.fillRect(centerX - 10, centerY - 4, 18, 11, 0x2104);

    tft.fillCircle(centerX - 14, centerY + 6, 12, 0x3186);
    tft.fillCircle(centerX + 2, centerY - 4, 16, 0x528A);
    tft.fillCircle(centerX + 14, centerY + 6, 10, 0x3186);
    tft.fillRect(centerX - 14, centerY + 6, 28, 11, 0x3186);
    
    // Slanted rain lines
    tft.drawLine(centerX - 10, centerY + 18, centerX - 13, centerY + 26, 0x07FF);
    tft.drawLine(centerX + 10, centerY + 18, centerX + 7, centerY + 26, 0x07FF);

    // Thick Jagged Lightning Bolt (Glowing Yellow-Orange with White Core)
    // Outer glow
    tft.fillTriangle(centerX + 5, centerY + 5, centerX - 9, centerY + 20, centerX + 1, centerY + 15, 0xFD20);
    tft.fillTriangle(centerX - 5, centerY + 19, centerX + 7, centerY + 15, centerX - 9, centerY + 31, 0xFD20);
    // Inner body
    tft.fillTriangle(centerX + 4, centerY + 6, centerX - 8, centerY + 20, centerX + 0, centerY + 16, 0xFFE0);
    tft.fillTriangle(centerX - 4, centerY + 18, centerX + 6, centerY + 16, centerX - 8, centerY + 30, 0xFFE0);
    // Core white line
    tft.drawLine(centerX + 2, centerY + 9, centerX - 4, centerY + 18, 0xFFFF);
    tft.drawLine(centerX - 4, centerY + 18, centerX - 6, centerY + 26, 0xFFFF);
  } 
  else if (condition.indexOf("snow") != -1 || condition.indexOf("ice") != -1 || condition.indexOf("sleet") != -1 || condition.indexOf("hail") != -1) {
    // Snowy Icon: Soft slate blue cloud + falling snowflakes (detailed asterisks)
    tft.fillCircle(centerX - 14, centerY + 4, 12, 0xBDF7);
    tft.fillCircle(centerX + 2, centerY - 6, 16, 0xFFFF);
    tft.fillCircle(centerX + 14, centerY + 4, 10, 0xBDF7);
    tft.fillRect(centerX - 14, centerY + 4, 28, 11, 0xBDF7);
    
    // Snowflake 1
    int s1x = centerX - 10, s1y = centerY + 18;
    tft.drawLine(s1x - 3, s1y, s1x + 3, s1y, 0xFFFF);
    tft.drawLine(s1x, s1y - 3, s1x, s1y + 3, 0xFFFF);
    tft.drawLine(s1x - 2, s1y - 2, s1x + 2, s1y + 2, 0xFFFF);
    tft.drawLine(s1x + 2, s1y - 2, s1x - 2, s1y + 2, 0xFFFF);
    
    // Snowflake 2
    int s2x = centerX + 2, s2y = centerY + 24;
    tft.drawLine(s2x - 3, s2y, s2x + 3, s2y, 0xFFFF);
    tft.drawLine(s2x, s2y - 3, s2x, s2y + 3, 0xFFFF);
    tft.drawLine(s2x - 2, s2y - 2, s2x + 2, s2y + 2, 0xFFFF);
    tft.drawLine(s2x + 2, s2y - 2, s2x - 2, s2y + 2, 0xFFFF);

    // Snowflake 3
    int s3x = centerX + 10, s3y = centerY + 18;
    tft.drawLine(s3x - 3, s3y, s3x + 3, s3y, 0xFFFF);
    tft.drawLine(s3x, s3y - 3, s3x, s3y + 3, 0xFFFF);
    tft.drawLine(s3x - 2, s3y - 2, s3x + 2, s3y + 2, 0xFFFF);
    tft.drawLine(s3x + 2, s3y - 2, s3x - 2, s3y + 2, 0xFFFF);
  } 
  else if (condition.indexOf("cloud") != -1 || condition.indexOf("overcast") != -1 || condition.indexOf("mist") != -1 || condition.indexOf("fog") != -1 || condition.indexOf("haze") != -1) {
    // Cloudy Icon: Layered 3D clouds
    // Back cloud (Dark greyish-blue)
    tft.fillCircle(centerX - 10, centerY - 6, 14, 0x3186);
    tft.fillCircle(centerX + 10, centerY - 6, 12, 0x3186);
    tft.fillRect(centerX - 10, centerY - 6, 20, 13, 0x3186);

    // Front cloud (Light greyish-blue and white)
    tft.fillCircle(centerX - 14, centerY + 6, 12, 0xBDF7);
    tft.fillCircle(centerX + 14, centerY + 6, 10, 0xBDF7);
    tft.fillRect(centerX - 14, centerY + 6, 28, 11, 0xBDF7);
    
    tft.fillCircle(centerX + 2, centerY - 4, 18, 0xFFFF);
    tft.fillRect(centerX - 14, centerY - 4, 32, 21, 0xFFFF);
    
    // Bottom blending
    tft.fillCircle(centerX - 14, centerY + 6, 12, 0xBDF7);
    tft.fillCircle(centerX + 14, centerY + 6, 10, 0xBDF7);
    tft.fillRect(centerX - 14, centerY + 6, 28, 11, 0xBDF7);
    
    if (condition.indexOf("mist") != -1 || condition.indexOf("fog") != -1 || condition.indexOf("haze") != -1) {
      tft.drawFastHLine(centerX - 22, centerY + 20, 44, 0xBDF7);
      tft.drawFastHLine(centerX - 14, centerY + 24, 28, 0xBDF7);
    }
  } 
  else {
    // Sunny or Moon (glowing light effects)
    if (isNight) {
      // Glow rings around Moon
      tft.fillCircle(centerX - 3, centerY - 2, 22, 0x2104); // Outer soft glow ring
      tft.fillCircle(centerX - 3, centerY - 2, 18, 0xFEA0); // Moon body (golden gold)
      tft.fillCircle(centerX + 6, centerY - 5, 17, bg);     // Cut-out to make crescent moon
      
      // Twinkling 4-point star 1 (White & Cyan)
      int sx = centerX + 12, sy = centerY + 10;
      tft.drawLine(sx, sy - 5, sx, sy + 5, 0x07FF);
      tft.drawLine(sx - 5, sy, sx + 5, sy, 0x07FF);
      tft.fillCircle(sx, sy, 2, 0xFFFF);
      
      // Twinkling star 2 (Tiny white dot)
      tft.fillCircle(centerX - 14, centerY + 14, 1, 0xFFFF);
      // Twinkling star 3
      tft.fillCircle(centerX - 12, centerY - 14, 1, 0xFFFF);
    } else {
      // Glowing Radial Sun
      tft.fillCircle(centerX, centerY, 24, 0x4100); // Very soft deep orange-red glow ring
      tft.fillCircle(centerX, centerY, 20, 0x9180); // Mid orange glow ring
      tft.fillCircle(centerX, centerY, 15, 0xFD20); // Orange body
      tft.fillCircle(centerX, centerY, 11, 0xFFE0); // Yellow core
      tft.fillCircle(centerX, centerY, 6, 0xFFFF);  // Bright white center core!
      
      // Sun rays (Beautiful thick lines)
      for (int angle = 0; angle < 360; angle += 45) {
         float rad = angle * 0.0174532925f;
         int x1 = centerX + 17 * cos(rad);
         int y1 = centerY + 17 * sin(rad);
         int x2 = centerX + 23 * cos(rad);
         int y2 = centerY + 23 * sin(rad);
         tft.drawLine(x1, y1, x2, y2, 0xFFE0);
      }
    }
  }
}

void drawTftDefaultScreen() {
  if (tftWasPowerCycled) return;
  currentTftState = TFT_IDLE;
  tft.fillScreen(0x0821); // Custom premium dark navy-blue background

  // 1. Draw Sleek Header Bar with Underline Glow
  tft.fillRect(0, 0, 320, 48, 0x018C); // Premium Teal header bar background
  tft.drawFastHLine(0, 48, 320, 0x03EF); // Glowing teal bottom underline
  drawAutoScaledString(deviceName.c_str(), 160, 24, 4, 1.0f, 300, TFT_WHITE, 0x018C, true);

  // 2. Draw Top Card: Network Info (with Double Border Glow)
  // Shifted and expanded slightly: y = 55..165 (height 110)
  tft.fillRoundRect(15, 55, 290, 110, 8, 0x10A2); // Lighter navy card body
  tft.drawRoundRect(15, 55, 290, 110, 8, 0x028A); // Outer border
  tft.drawRoundRect(16, 56, 288, 108, 8, 0x03EF);  // Inner glow border
  
  // Section Header (Network)
  drawAutoScaledString("NETWORK INFO", 160, 70, 2, 1.0f, 270, TFT_CYAN, 0x10A2, true);
  tft.drawFastHLine(25, 86, 270, 0x028A);

  // Network panel vertical separator
  tft.drawFastVLine(100, 90, 60, 0x028A);

  // 3. Draw Middle Card: Time & Status (with Glow, and Clock Icon)
  // Shifted and expanded: y = 175..410 (height 235)
  tft.fillRoundRect(15, 175, 290, 235, 8, 0x10A2); // Card body
  tft.drawRoundRect(15, 175, 290, 235, 8, 0x028A); // Outer border
  tft.drawRoundRect(16, 176, 288, 233, 8, 0x03EF);  // Inner glow border
  
  // Section Header (Time)
  drawAutoScaledString("TIME & STATUS", 160, 190, 2, 1.0f, 240, TFT_CYAN, 0x10A2, true);
  tft.drawFastHLine(25, 205, 270, 0x028A);

  // Draw mini analog clock icon at the header
  tft.drawCircle(265, 190, 7, TFT_CYAN);
  tft.drawLine(265, 190, 268, 190, TFT_CYAN);
  tft.drawLine(265, 190, 265, 186, TFT_CYAN);

  // 4. Draw Footer Standby Banner
  // Shifted and resized: y = 420..470 (height 50)
  tft.fillRoundRect(15, 420, 290, 50, 6, 0x018C); // Teal banner body
  tft.drawRoundRect(15, 420, 290, 50, 6, 0x03EF); // Glow border
  drawAutoScaledString("ATTENDANCE SYSTEM", 160, 445, 4, 1.0f, 300, TFT_WHITE, 0x018C, true);

  // Initial update of clock & network status
  tftForceClockRedraw = true;
  updateTftClock();
}

void onBrandingUpdated() {
  if (currentTftState == TFT_IDLE) {
    drawTftDefaultScreen();
  }
}
void drawTftDirectionSelectionScreen(String name, String roll) {
  if (tftWasPowerCycled) return;
  currentTftState = TFT_DIRECTION_SELECT;
  tft.fillScreen(TFT_BLACK);
  
  // Outer frame
  tft.drawRoundRect(15, 15, 290, 450, 12, 0x03EF); // Teal border
  tft.drawRoundRect(16, 16, 288, 448, 12, 0x03EF);
  
  drawAutoScaledString("SELECT DIRECTION", 160, 40, 4, 1.3f, 280, TFT_WHITE, TFT_BLACK, true);
  tft.drawFastHLine(35, 62, 250, 0x2104); // Subtle divider
  
  // Center Card for Employee Profile (Premium Glassmorphism Look)
  uint16_t cardBg = 0x1082; // Deep charcoal
  tft.fillRoundRect(35, 78, 250, 122, 10, cardBg); // Card Body
  tft.drawRoundRect(35, 78, 250, 122, 10, 0x2104); // Border
  tft.drawRoundRect(36, 79, 248, 120, 10, 0x03EF); // Glow
  
  // Centered greeting, name & roll ID inside the card (Clean, no-photo version)
  drawAutoScaledString("Welcome,", 160, 105, 4, 1.0f, 230, TFT_CYAN, cardBg, true);
  drawAutoScaledString(name, 160, 138, 4, 1.0f, 230, TFT_WHITE, cardBg, true);
  drawAutoScaledString("ID: " + roll, 160, 171, 4, 1.0f, 230, 0x07E0, cardBg, true);

  // Draw "PUNCH IN" Button (drawn at y = 220..296)
  tft.fillRoundRect(43, 223, 240, 76, 12, 0x0102); // Dark shadow
  tft.fillRoundRect(40, 220, 240, 76, 12, 0x03A4); // Rich Green body
  tft.drawRoundRect(40, 220, 240, 76, 12, 0x07E0); // Neon green border
  // Custom design icon: arrow pointing right into bracket
  tft.drawRect(52, 246, 14, 24, TFT_WHITE);
  tft.fillRect(49, 250, 5, 16, 0x03A4); // Open bracket opening
  tft.drawLine(47, 258, 61, 258, TFT_WHITE); // Arrow line
  tft.drawLine(56, 253, 61, 258, TFT_WHITE); // Arrow head
  tft.drawLine(56, 263, 61, 258, TFT_WHITE);
  // Text
  drawAutoScaledString("PUNCH IN  >>", 175, 245, 4, 1.3f, 160, TFT_WHITE, 0x03A4, true);
  drawAutoScaledString("(Start Shift)", 175, 275, 2, 1.0f, 160, 0x07E0, 0x03A4, true);
  
  // Draw "PUNCH OUT" Button (drawn at y = 310..386)
  tft.fillRoundRect(43, 313, 240, 76, 12, 0x0102); // Dark shadow
  tft.fillRoundRect(40, 310, 240, 76, 12, 0x7800); // Rich Red/Maroon body
  tft.drawRoundRect(40, 310, 240, 76, 12, TFT_RED); // Border
  // Custom design icon: arrow pointing left out of bracket
  tft.drawRect(52, 336, 14, 24, TFT_WHITE);
  tft.fillRect(49, 340, 5, 16, 0x7800);
  tft.drawLine(45, 348, 59, 348, TFT_WHITE); // Arrow line
  tft.drawLine(45, 348, 50, 343, TFT_WHITE); // Arrow head
  tft.drawLine(45, 348, 50, 353, TFT_WHITE);
  // Text
  drawAutoScaledString("<<  PUNCH OUT", 175, 335, 4, 1.3f, 160, TFT_WHITE, 0x7800, true);
  drawAutoScaledString("(End Shift)", 175, 365, 2, 1.0f, 160, TFT_RED, 0x7800, true);
  
  // Cancel message at bottom
  drawAutoScaledString("Tap outside or wait to cancel", 160, 425, 2, 1.5f, 270, TFT_YELLOW, TFT_BLACK, true);
}

void drawTft2FAPromptScreen(String name, String roll) {
  if (tftWasPowerCycled) return;
  currentTftState = TFT_WAITING_2FA_FINGER;
  tft.fillScreen(TFT_BLACK);
  
  uint16_t cardBg = 0x1082; // Deep charcoal
  uint16_t accentColor = 0xFDC0; // Gold/Amber pending color
  uint16_t infoColor = 0x5AEB; // High-tech active cyan-blue
  
  // Outer frame
  tft.drawRoundRect(15, 15, 290, 450, 12, accentColor);
  tft.drawRoundRect(16, 16, 288, 448, 12, accentColor);
  
  // Header
  drawAutoScaledString("2-FACTOR AUTH", 160, 40, 4, 1.0f, 280, TFT_WHITE, TFT_BLACK, true);
  tft.drawFastHLine(35, 62, 250, 0x2104); // Dark subtle divider
  
  // Status message
  drawAutoScaledString("CARD VERIFIED", 160, 95, 4, 1.0f, 240, 0x07E0, TFT_BLACK, true); // Neon green for success card read
  
  // Employee Profile Card (Centered, clean)
  tft.fillRoundRect(35, 125, 250, 122, 10, cardBg); // Card Body
  tft.drawRoundRect(35, 125, 250, 122, 10, 0x2104); // Border
  tft.drawRoundRect(36, 126, 248, 120, 10, infoColor); // Glow
  
  // Centered greeting, name & roll ID inside the card (Clean, no-photo version)
  drawAutoScaledString("Welcome,", 160, 152, 4, 1.0f, 230, TFT_CYAN, cardBg, true);
  drawAutoScaledString(name, 160, 185, 4, 1.0f, 230, TFT_WHITE, cardBg, true);
  drawAutoScaledString("ID: " + roll, 160, 218, 4, 1.0f, 230, 0x07E0, cardBg, true);

  // Biometric Instruction Card
  tft.fillRoundRect(35, 265, 250, 140, 12, cardBg);
  tft.drawRoundRect(35, 265, 250, 140, 12, 0x2104);
  tft.drawRoundRect(36, 266, 248, 138, 12, accentColor);
  
  // Action Text - Clean spacing and wording
  drawAutoScaledString("SCAN BIOMETRIC", 160, 310, 4, 1.0f, 230, TFT_WHITE, cardBg, true);
  drawAutoScaledString("Place finger on sensor to complete", 160, 355, 2, 1.0f, 230, accentColor, cardBg, true);

  // Timeout warning at the bottom
  drawAutoScaledString("Timeout in 10s", 160, 438, 2, 1.0f, 270, TFT_RED, TFT_BLACK, true);
}

void drawTftEnrollScanningScreen(String type, int id, int step, String msg, bool forceRedraw) {
  if (tftWasPowerCycled) return;
  
  static String lastMsg = "";
  static int lastStep = -1;
  static String lastType = "";
  
  if (forceRedraw || currentTftState != TFT_ENROLL_SCANNING) {
    lastMsg = "";
    lastStep = -1;
    lastType = "";
  }
  
  if (!forceRedraw && msg == lastMsg && step == lastStep && type == lastType) {
    return;
  }
  
  lastMsg = msg;
  lastStep = step;
  lastType = type;
  
  currentTftState = TFT_ENROLL_SCANNING;
  tft.fillScreen(TFT_BLACK);
  
  uint16_t cardBg = 0x1082; // Deep charcoal
  uint16_t accentColor = 0xFDC0; // Gold/Amber pending color
  uint16_t infoColor = 0x5AEB; // High-tech active cyan-blue
  
  // Outer frame
  tft.drawRoundRect(15, 15, 290, 450, 12, accentColor);
  tft.drawRoundRect(16, 16, 288, 448, 12, accentColor);
  
  // Header
  drawAutoScaledString("ENROLLING EMP", 160, 40, 4, 1.2f, 280, TFT_WHITE, TFT_BLACK, true);
  tft.drawFastHLine(35, 62, 250, 0x2104); // Dark subtle divider
  
  // Enrollment Mode Title
  String typeStr = (type == "FINGER") ? "FINGERPRINT" : "RFID CARD";
  drawAutoScaledString(typeStr, 160, 95, 4, 1.2f, 240, infoColor, TFT_BLACK, true);
  
  // Enrollment Details Card
  tft.fillRoundRect(35, 125, 250, 122, 10, cardBg); // Card Body
  tft.drawRoundRect(35, 125, 250, 122, 10, 0x2104); // Border
  tft.drawRoundRect(36, 126, 248, 120, 10, infoColor); // Glow
  
  drawAutoScaledString("Status Details:", 160, 145, 2, 0.9f, 230, TFT_CYAN, cardBg, true);
  if (type == "FINGER") {
    drawAutoScaledString("Finger ID: " + String(id), 160, 175, 4, 1.1f, 230, TFT_WHITE, cardBg, true);
    drawAutoScaledString("Scan Step: " + String(step) + " of 3", 160, 208, 4, 1.0f, 230, TFT_GREEN, cardBg, true);
  } else {
    drawAutoScaledString("Waiting for Card...", 160, 175, 4, 1.1f, 230, TFT_WHITE, cardBg, true);
    if (id != -1) {
      drawAutoScaledString("Linking to FP: " + String(id), 160, 208, 4, 1.0f, 230, TFT_GREEN, cardBg, true);
    } else {
      drawAutoScaledString("New Card Scan", 160, 208, 4, 1.0f, 230, TFT_GREEN, cardBg, true);
    }
  }

  // Instruction Box
  tft.fillRoundRect(35, 265, 250, 140, 12, cardBg);
  tft.drawRoundRect(35, 265, 250, 140, 12, 0x2104);
  tft.drawRoundRect(36, 266, 248, 138, 12, accentColor);
  
  drawAutoScaledString("INSTRUCTION", 160, 290, 4, 1.0f, 230, TFT_WHITE, cardBg, true);
  drawAutoScaledString(msg, 160, 335, 2, 1.0f, 230, accentColor, cardBg, true);
  if (type == "FINGER") {
    if (step == 1) {
      drawAutoScaledString("Place finger on sensor", 160, 365, 2, 0.9f, 230, TFT_CYAN, cardBg, true);
    } else if (step == 2) {
      drawAutoScaledString("Remove finger from sensor", 160, 365, 2, 0.9f, 230, TFT_CYAN, cardBg, true);
    } else if (step == 3) {
      drawAutoScaledString("Place same finger again", 160, 365, 2, 0.9f, 230, TFT_CYAN, cardBg, true);
    }
  } else {
    drawAutoScaledString("Place card on reader", 160, 365, 2, 0.9f, 230, TFT_CYAN, cardBg, true);
  }

  // Cancel warning at the bottom
  drawAutoScaledString("Cancel from browser dashboard", 160, 438, 2, 0.8f, 270, TFT_RED, TFT_BLACK, true);
}

void drawTftEnrollConfirmedScreen(String name, String roll, String role, String type) {
  if (tftWasPowerCycled) return;
  currentTftState = TFT_ENROLL_CONFIRMED;
  tft.fillScreen(TFT_BLACK);
  
  uint16_t cardBg = 0x1082; // Deep charcoal
  uint16_t accentColor = 0x07E0; // Neon success green
  
  tft.fillRoundRect(18, 18, 290, 450, 12, 0x0410); // Shadow
  tft.fillRoundRect(15, 15, 290, 450, 12, cardBg); // Card Body
  tft.drawRoundRect(15, 15, 290, 450, 12, accentColor); // Border
  tft.drawRoundRect(16, 16, 288, 448, 12, accentColor);
  
  drawAutoScaledString("CONFIRMED!", 160, 70, 4, 1.8f, 260, accentColor, cardBg, true);
  tft.drawFastHLine(35, 110, 250, 0x2104); // Elegant divider
  
  // User Profile Name
  drawAutoScaledString(name.c_str(), 160, 155, 4, 1.1f, 270, TFT_WHITE, cardBg, true);
  tft.drawFastHLine(40, 190, 240, 0x2104);
  
  // Roll ID
  drawAutoScaledString("EMPLOYEE ID", 160, 210, 2, 1.0f, 270, 0x05E5, cardBg, true);
  drawAutoScaledString(roll.c_str(), 160, 235, 4, 1.1f, 270, TFT_WHITE, cardBg, true);
  tft.drawFastHLine(40, 270, 240, 0x2104);
  
  // Designation / Role
  drawAutoScaledString("DESIGNATION", 160, 290, 2, 1.0f, 270, 0x05E5, cardBg, true);
  drawAutoScaledString(role.c_str(), 160, 315, 4, 1.0f, 270, TFT_WHITE, cardBg, true);
  tft.drawFastHLine(40, 350, 240, 0x2104);
  
  // Type of registration
  drawAutoScaledString("CREDENTIAL TYPE", 160, 370, 2, 1.0f, 270, 0x05E5, cardBg, true);
  String credType = (type == "FINGER") ? "Fingerprint" : "RFID Card";
  drawAutoScaledString(credType.c_str(), 160, 395, 4, 1.0f, 270, TFT_CYAN, cardBg, true);
  tft.drawFastHLine(40, 430, 240, 0x2104);
}

// Helper function to shorten and clean up long weather condition descriptions from wttr.in
String shortenWeatherCond(String cond) {
  cond.trim();
  String lower = cond;
  lower.toLowerCase();
  
  if (lower.indexOf("thunder") != -1 || lower.indexOf("storm") != -1) {
    return "Thunderstorm";
  }
  if (lower.indexOf("heavy rain") != -1 || lower.indexOf("moderate or heavy") != -1) {
    return "Heavy Rain";
  }
  if (lower.indexOf("light rain") != -1 || lower.indexOf("patchy rain") != -1) {
    return "Light Rain";
  }
  if (lower.indexOf("rain") != -1 || lower.indexOf("shower") != -1) {
    return "Rainy";
  }
  if (lower.indexOf("drizzle") != -1) {
    return "Drizzle";
  }
  if (lower.indexOf("snow") != -1 || lower.indexOf("sleet") != -1 || lower.indexOf("hail") != -1) {
    return "Snowy";
  }
  if (lower.indexOf("partly cloudy") != -1) {
    return "Partly Cloudy";
  }
  if (lower.indexOf("overcast") != -1) {
    return "Overcast";
  }
  if (lower.indexOf("cloud") != -1) {
    return "Cloudy";
  }
  if (lower.indexOf("mist") != -1 || lower.indexOf("fog") != -1 || lower.indexOf("haze") != -1) {
    return "Misty";
  }
  if (lower.indexOf("clear") != -1 || lower.indexOf("sunny") != -1) {
    return "Clear";
  }
  
  // If nothing matches, return original trimmed string, truncated to 16 chars if too long
  if (cond.length() > 16) {
    return cond.substring(0, 14) + "..";
  }
  return cond;
}

void updateTftClock() {
  if (tftWasPowerCycled) return;
  extern bool isAPMode;
  uint16_t bg = 0x10A2;
  tft.setTextDatum(MC_DATUM);

  // 1. Update Network Status Card
  bool ethConnected = false;
#if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0)
  ethConnected = (ETH.handle() != NULL) && ETH.linkUp();
#else
  ethConnected = (ETH.localIP() != IPAddress(0, 0, 0, 0));
#endif

  String ipStr = "";
  String netMode = "";
  uint16_t ipColor = TFT_GREEN;

  if (ethConnected) {
    netMode = "MODE : LAN";
    ipStr = "IP: " + ETH.localIP().toString();
    ipColor = TFT_GREEN;
  } else if (WiFi.status() == WL_CONNECTED) {
    netMode = "MODE : WIFI";
    ipStr = "IP: " + WiFi.localIP().toString();
    ipColor = TFT_CYAN;
  } else {
    netMode = "MODE: AP CONFIG";
    ipStr = "IP: 192.168.4.1";
    ipColor = TFT_ORANGE;
  }

  // Draw connection status lines only if changed
  static String lastNetMode = "";
  static String lastIpStr = "";
  static uint16_t lastIpColor = 0;
  
  if (tftForceClockRedraw) {
    lastNetMode = "";
    lastIpStr = "";
    lastIpColor = 0;
  }

  if (lastNetMode != netMode || lastIpStr != ipStr || lastIpColor != ipColor) {
    lastNetMode = netMode;
    lastIpStr = ipStr;
    lastIpColor = ipColor;
    
    // Clear left side icon area and text area completely to prevent overlap
    tft.fillRect(20, 92, 78, 66, bg);
    tft.fillRect(102, 92, 198, 66, bg);
    
    // Draw single, large network icon centered at x = 60, y = 125
    drawNetworkIcon(60, 125, ipColor, !ethConnected);

    if (isAPMode && !ethConnected && WiFi.status() != WL_CONNECTED) {
      // Draw three detailed lines for AP Hotspot configuration
      drawCardLineLeft("AP: Anurag_0.1_AP", 104, 102, 2, 1.2f, TFT_WHITE, bg, 16);
      drawCardLineLeft("Pass: admin123", 104, 124, 2, 1.2f, TFT_WHITE, bg, 16);
      drawCardLineLeft("IP: 192.168.4.1", 104, 146, 2, 1.2f, ipColor, bg, 16);
    } else {
      // Draw three balanced lines for connected mode
      if (ethConnected) {
        drawCardLineLeft(netMode, 104, 102, 2, 1.2f, TFT_WHITE, bg, 16);
        drawCardLineLeft("SSID: Ethernet", 104, 124, 2, 1.2f, TFT_WHITE, bg, 16);
        drawCardLineLeft(ipStr, 104, 146, 2, 1.2f, ipColor, bg, 16);
      } else {
        String ssidStr = "SSID: " + WiFi.SSID();
        if (WiFi.SSID().length() == 0) {
          ssidStr = "SSID: Connecting..";
        }
        drawCardLineLeft(netMode, 104, 102, 2, 1.2f, TFT_WHITE, bg, 16);
        drawCardLineLeft(ssidStr, 104, 124, 2, 1.2f, TFT_WHITE, bg, 16);
        drawCardLineLeft(ipStr, 104, 146, 2, 1.2f, ipColor, bg, 16);
      }
    }

    // Connection status light dot
    tft.fillCircle(285, 70, 8, bg);
    tft.fillCircle(285, 70, 4, ipColor);
    tft.drawCircle(285, 70, 6, ipColor);
  }

  // 2. Update Time & Weather Card
  struct tm ti;
  char timeBuf[12] = "12:00:00";
  char dateBuf[15] = "00-00-0000";
  bool gotTime = getLocalTime(&ti, 50);

  int hour12 = 12;
  const char* ampm = "AM";
  if (gotTime) {
    hour12 = ti.tm_hour % 12;
    if (hour12 == 0) hour12 = 12;
    ampm = (ti.tm_hour >= 12) ? "PM" : "AM";
    sprintf(timeBuf, "%02d:%02d:%02d", hour12, ti.tm_min, ti.tm_sec);
    strftime(dateBuf, sizeof(dateBuf), "%d-%m-%Y", &ti);
  }

  // Measure time string width under scaled Font 4
  tft.setFont(&fonts::Font4);
  tft.setTextSize(2.0f);
  int timeW = tft.textWidth(timeBuf);

  // Measure AM/PM string width under Font 4 scale 1.0f
  tft.setFont(&fonts::Font4);
  tft.setTextSize(1.0f);
  int ampmW = tft.textWidth(ampm);

  int totalW = timeW + 8 + ampmW;
  int startX = 160 - (totalW / 2);

  // Clear only the left and right margins outside the text bounds to prevent flickering
  tft.fillRect(20, 208, startX - 20, 60, bg);
  tft.fillRect(startX + totalW, 208, 300 - (startX + totalW), 60, bg);

  // Draw main time (Large yellow text with background color for overwrite)
  tft.setFont(&fonts::Font4);
  tft.setTextSize(2.0f);
  tft.setTextColor(TFT_YELLOW, bg);
  tft.setTextDatum(ML_DATUM);
  tft.drawString(timeBuf, startX, 238);

  // Draw AM/PM superscript (Teal text with background color)
  tft.setFont(&fonts::Font4);
  tft.setTextSize(1.0f);
  tft.setTextColor(TFT_CYAN, bg);
  tft.setTextDatum(ML_DATUM);
  tft.drawString(ampm, startX + timeW + 8, 225);

  tft.setTextSize(1.0f); // Reset text size scale

  // Other fields are drawn only if changed
  static String lastDrawnDate = "";
  static String lastDrawnTemp = "";
  static String lastDrawnHumWind = "";
  static String lastDrawnShift = "";
  static String lastDrawnCond = "";
  static bool lastDrawnNight = false;

  int hour = gotTime ? ti.tm_hour : 12;
  bool isNight = (hour >= 19 || hour < 5);

  if (tftForceClockRedraw) {
    lastDrawnDate = "";
    lastDrawnTemp = "";
    lastDrawnHumWind = "";
    lastDrawnShift = "";
    lastDrawnCond = "";
    lastDrawnNight = false;
    tftForceClockRedraw = false; // Reset the redraw flag at the end
  }

  if (lastDrawnDate != String(dateBuf)) {
    lastDrawnDate = String(dateBuf);
    drawCardLineLeft("DATE: " + lastDrawnDate, 110, 275, 2, 1.3f, TFT_WHITE, bg, 16);
    
    // Clear and draw Calendar Icon (28x28 centered at 60, 275)
    tft.fillRect(45, 260, 30, 30, bg);
    drawCalendarIcon(60, 275, TFT_WHITE);
  }

  // Draw temperature & condition
  // Draw temperature (Large Font 4)
  String tempStr = "";
  float tempVal = 0.0f;
  if (outsideTemp > -50.0) {
    tempStr = String(outsideTemp, 1) + " C";
    tempVal = outsideTemp;
  } else {
    float tempC = temperatureRead();
    if (isnan(tempC)) tempC = 0.0;
    tempStr = String(tempC, 1) + " C (Int)";
    tempVal = tempC;
  }

  if (lastDrawnTemp != tempStr) {
    lastDrawnTemp = tempStr;
    drawCardLineLeft(tempStr, 110, 312, 4, 1.4f, TFT_YELLOW, bg, 26);
    
    // Clear and draw Temperature Icon (24x36 centered at 60, 312)
    tft.fillRect(45, 292, 30, 40, bg);
    drawTempIcon(60, 312, TFT_WHITE, tempVal, bg);
  }

  // Draw weather condition (Font 2)
  String condStr = "";
  if (outsideTemp > -50.0) {
    condStr = shortenWeatherCond(weatherCond);
  } else {
    condStr = "Local Mode";
  }

  static String lastDrawnCondStr = "";
  if (tftForceClockRedraw) {
    lastDrawnCondStr = "";
  }
  if (lastDrawnCondStr != condStr) {
    lastDrawnCondStr = condStr;
    drawCardLineLeft(condStr, 110, 352, 2, 1.3f, TFT_WHITE, bg, 16);
  }

  // Draw humidity & wind (Font 2)
  String humWindStr = "";
  if (outsideTemp > -50.0) {
    humWindStr = "Hum: " + weatherHum + " | Wind: " + weatherWind;
  } else {
    humWindStr = "Hum: -- | Wind: --";
  }

  if (lastDrawnHumWind != humWindStr) {
    lastDrawnHumWind = humWindStr;
    drawCardLineLeft(humWindStr, 110, 376, 2, 1.1f, 0xBDD7, bg, 16);
  }

  // Draw Shift / Status message (Font 2)
  String shiftMsg = "Access Dashboard";
  if (gotTime) {
    if (hour >= 5 && hour < 12) {
      shiftMsg = "Good Morning!";
    } else if (hour >= 12 && hour < 17) {
      shiftMsg = "Good Afternoon!";
    } else {
      shiftMsg = "Good Evening!";
    }
  }

  if (lastDrawnShift != shiftMsg) {
    lastDrawnShift = shiftMsg;
    drawCardLineLeft(shiftMsg, 110, 400, 2, 1.3f, TFT_CYAN, bg, 16);
  }

  // Draw weather icon only if condition or night status changed
  if (lastDrawnCond != condStr || lastDrawnNight != isNight) {
    lastDrawnCond = condStr;
    lastDrawnNight = isNight;
    // Drawn centered at x = 60, y = 364, which spans y = 332..396
    drawWeatherIcon(60, 364, condStr, isNight);
  }
}

void drawProfilePhoto(String roll, int x, int y, int w, int h, uint16_t bgColor) {
  if (tftWasPowerCycled) return;
  // Clear area with card background color
  tft.fillRect(x, y, w, h, bgColor);
  
  String jpgPath = "/photos/" + roll + ".jpg";
  bool imageDrawn = false;
  
  if (LittleFS.exists(jpgPath)) {
    float scale = 1.0f;
    if (w <= 64) {
      scale = 0.5f; // Decode at 1/2 size natively for 120x120 -> 60x60
    }
    // drawJpgFile returns true on success
    if (tft.drawJpgFile(LittleFS, jpgPath.c_str(), x, y, w, h, 0, 0, scale, scale)) {
      imageDrawn = true;
    }
  }
  
  // Draw double borders around the profile picture slot
  tft.drawRoundRect(x - 2, y - 2, w + 4, h + 4, 6, TFT_WHITE);
  tft.drawRoundRect(x - 1, y - 1, w + 2, h + 2, 5, TFT_WHITE);
  
  if (!imageDrawn) {
    // Draw the high-quality futuristic fallback profile avatar scaled to the box size
    int cx = x + w / 2;
    int cy = y + h / 2;
    int headRadius = w / 8 + 4; // Scales head based on width
    int headY = cy - h / 12;    // Scales head position vertically
    tft.fillCircle(cx, headY, headRadius, 0x05E5);
    tft.drawCircle(cx, headY, headRadius, TFT_WHITE);
    
    int shoulderRx = w / 5 + 4; // Scales shoulder width
    int shoulderRy = h / 11;    // Scales shoulder thickness
    int shoulderY = cy + h / 5; // Scales shoulder position vertically
    tft.fillEllipse(cx, shoulderY, shoulderRx, shoulderRy, 0x05E5);
    tft.drawEllipse(cx, shoulderY, shoulderRx, shoulderRy, TFT_WHITE);
  }
}

void drawTftSuccessScreen(String name, String dir, String status, String time, String roll) {
  if (tftWasPowerCycled) return;
  // Pure Black background
  tft.fillScreen(TFT_BLACK);

  // Card background: deep charcoal/slate (0x1082)
  uint16_t cardBg = 0x1082;
  uint16_t accentColor = 0x07E0; // Neon success green
  
  tft.fillRoundRect(15, 15, 290, 450, 12, cardBg); // Card Body
  tft.drawRoundRect(15, 15, 290, 450, 12, accentColor); // Border
  tft.drawRoundRect(16, 16, 288, 448, 12, accentColor);

  String headerText = "SUCCESS";
  if (dir.equalsIgnoreCase("In")) {
    headerText = "WELCOME";
  } else if (dir.equalsIgnoreCase("Out")) {
    headerText = "GOOD BYE";
  }
  // Premium bold centered header (scale 2)
  drawAutoScaledString(headerText, 160, 70, 4, 2.0f, 260, accentColor, cardBg, true);
  tft.drawFastHLine(35, 110, 250, 0x2104); // Elegant divider

  // User Name
  drawAutoScaledString(name.c_str(), 160, 155, 4, 1.0f, 270, TFT_WHITE, cardBg, true);
  tft.drawFastHLine(40, 190, 240, 0x2104);

  // Direction: IN / OUT
  drawAutoScaledString("DIRECTION", 160, 210, 2, 1.0f, 270, 0x05E5, cardBg, true);
  drawAutoScaledString(dir.c_str(), 160, 235, 4, 1.0f, 270, TFT_WHITE, cardBg, true);
  tft.drawFastHLine(40, 270, 240, 0x2104);

  // Time details
  drawAutoScaledString("TIME", 160, 290, 2, 1.0f, 270, 0x05E5, cardBg, true);
  drawAutoScaledString(time.c_str(), 160, 315, 4, 1.0f, 270, TFT_WHITE, cardBg, true);
  tft.drawFastHLine(40, 350, 240, 0x2104);

  // Status (Late / On-Time)
  uint16_t statusColor = 0x07E0;
  if (status == "Late" || status == "Early Exit") {
    statusColor = TFT_ORANGE;
  }
  drawAutoScaledString("STATUS", 160, 370, 2, 1.0f, 270, 0x05E5, cardBg, true);
  drawAutoScaledString(status.c_str(), 160, 395, 4, 1.0f, 270, statusColor, cardBg, true);
  tft.drawFastHLine(40, 430, 240, 0x2104);
}

void drawTftDeniedScreen(String name, String reason) {
  if (tftWasPowerCycled) return;
  // OLED Pure Black background
  tft.fillScreen(TFT_BLACK);

  // Card background: deep charcoal/slate (0x1082)
  uint16_t cardBg = 0x1082;
  uint16_t accentColor = 0xF800; // Neon red
  
  tft.fillRoundRect(15, 15, 290, 450, 12, cardBg); // Card Body
  tft.drawRoundRect(15, 15, 290, 450, 12, accentColor); // Border
  tft.drawRoundRect(16, 16, 288, 448, 12, accentColor);

  // Premium bold centered header (scale 2)
  drawAutoScaledString("ACCESS DENIED", 160, 70, 4, 2.0f, 260, accentColor, cardBg, true);
  tft.drawFastHLine(35, 110, 250, 0x2104); // Elegant divider

  // User Name
  drawAutoScaledString(name.c_str(), 160, 155, 4, 1.0f, 270, TFT_WHITE, cardBg, true);
  tft.drawFastHLine(40, 190, 240, 0x2104);

  // Reason
  drawAutoScaledString("REASON", 160, 220, 2, 1.0f, 270, TFT_YELLOW, cardBg, true);
  drawAutoScaledString(reason.c_str(), 160, 250, 4, 1.0f, 270, TFT_WHITE, cardBg, true);
  tft.drawFastHLine(40, 290, 240, 0x2104);

  // Action message
  drawAutoScaledString("ACTION REQUIRED", 160, 330, 2, 1.0f, 270, TFT_YELLOW, cardBg, true);
  drawAutoScaledString("PLEASE TRY AGAIN", 160, 365, 4, 1.0f, 270, accentColor, cardBg, true);
  tft.drawFastHLine(40, 410, 240, 0x2104);
}

// Safe WDT reset helper (compatible with Arduino Core v2 and v3)
#ifndef CONFIG_ESP_TASK_WDT_EN
#define WDT_RESET() esp_task_wdt_reset()
#else
#define WDT_RESET() esp_task_wdt_reset()
#endif

// Hardware Pins
#define SS_PIN 10
#define RST_PIN 3
#define FP_RX 16
#define FP_TX 17
#define LED_PIN 2
#define DENIED_LED_PIN 4
#define BUZZER_PIN                                                             \
  5 // GPIO pin for Buzzer (can be changed to any free GPIO pin)

#define WDT_TIMEOUT 60

MFRC522 rfid(SS_PIN, RST_PIN);
HardwareSerial mySerial(2);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
SemaphoreHandle_t fpMutex;

const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 19800;

// Login Credentials - Change these if you want
char loginUser[32] = "admin";
char loginPass[32] = "admin123";

// Fallback WiFi Credentials (used if wifi.conf is wiped during sketch upload)
// Enter your default router credentials here:
const char *DEFAULT_SSID = "YOUR_WIFI_SSID";
const char *DEFAULT_PASS = "YOUR_WIFI_PASSWORD";

// Shift Timing Configurations (default 09:00 to 17:00, 15 minutes grace)
int shiftStartHour = 9;
int shiftStartMin = 0;
int shiftEndHour = 17;
int shiftEndMin = 0;
int graceMins = 15;

void saveShiftConfig() {
  File f = LittleFS.open("/shift.sys", FILE_WRITE);
  if (f) {
    f.println(shiftStartHour);
    f.println(shiftStartMin);
    f.println(shiftEndHour);
    f.println(shiftEndMin);
    f.println(graceMins);
    f.close();
    Serial.println("✓ Shift configuration saved");
  }
}

void loadShiftConfig() {
  if (LittleFS.exists("/shift.sys")) {
    File f = LittleFS.open("/shift.sys", FILE_READ);
    if (f) {
      shiftStartHour = f.readStringUntil('\n').toInt();
      shiftStartMin = f.readStringUntil('\n').toInt();
      shiftEndHour = f.readStringUntil('\n').toInt();
      shiftEndMin = f.readStringUntil('\n').toInt();
      graceMins = f.readStringUntil('\n').toInt();
      f.close();
      Serial.printf(
          "✓ Shift config loaded: %02d:%02d to %02d:%02d (Grace: %d min)\n",
          shiftStartHour, shiftStartMin, shiftEndHour, shiftEndMin, graceMins);
    }
  }
}

void saveAuthConfig() {
  File f = LittleFS.open("/auth.sys", FILE_WRITE);
  if (f) {
    f.println(loginUser);
    f.println(loginPass);
    f.close();
    Serial.println("✓ Auth credentials saved");
  }
}

void loadAuthConfig() {
  if (LittleFS.exists("/auth.sys")) {
    File f = LittleFS.open("/auth.sys", FILE_READ);
    if (f) {
      String u = f.readStringUntil('\n');
      String p = f.readStringUntil('\n');
      u.trim();
      p.trim();
      if (u.length() > 0) {
        strncpy(loginUser, u.c_str(), sizeof(loginUser) - 1);
        loginUser[sizeof(loginUser) - 1] = '\0';
      }
      if (p.length() > 0) {
        strncpy(loginPass, p.c_str(), sizeof(loginPass) - 1);
        loginPass[sizeof(loginPass) - 1] = '\0';
      }
      f.close();
      Serial.println("✓ Auth credentials loaded");
    }
  }
}

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
DNSServer dnsServer;

bool isAPMode = false;
unsigned long apModeStartTime = 0;
bool manualLedOn = false;
volatile bool isOtaUpdating = false;
volatile unsigned long lastOtaChunkTime = 0;
volatile bool isDeniedLedRunning = false;

unsigned long lastScanTime = 0;
String lastScanUID = "";
unsigned long restartTime = 0;
String webhookUrl = "";
String sqlApiUrl = "http://192.168.2.109:5000/webhook/scan";
uint32_t sqlTotalLogs = 0;
uint32_t sqlTransferredLogs = 0;
uint32_t sqlPendingLogs = 0;
String deviceName = "Anurag 0.1";
String googleApiKey = "";
bool tftPromptEnabled = true;
String weatherCity = "Navi-Mumbai";
float outsideTemp = -99.0;
String weatherCond = "Clear";
String weatherHum = "0%";
String weatherWind = "0km/h";
TaskHandle_t weatherTaskHandle = NULL;

// I2S audio driver config removed

// Enrollment State
int enrollID = -1;
bool isEnrollMode = false;
int enrollStep = 0;
String enrollType = "";
unsigned long enrollTimer = 0;
bool wasEnrollActive = false;

#define MAX_FP 200
bool fpUsed[MAX_FP + 1] = {false};

void saveFpCache() {
  File f = LittleFS.open("/fp_cache.bin", FILE_WRITE);
  if (f) {
    f.write((uint8_t *)fpUsed, sizeof(fpUsed));
    f.close();
    Serial.println("✓ FP Cache saved to storage");
  }
}

void loadFpCache() {
  if (LittleFS.exists("/fp_cache.bin")) {
    File f = LittleFS.open("/fp_cache.bin", FILE_READ);
    if (f) {
      f.read((uint8_t *)fpUsed, sizeof(fpUsed));
      f.close();
      Serial.println("✓ FP Cache loaded from storage");
    }
  } else {
    Serial.println("ℹ No FP Cache found, first sync required");
  }
}

void syncFingerprintDatabase() {
  Serial.println("Syncing fingerprint database with local storage...");
  int loadedCount = 0;
  
  // Set all to false first
  for (int i = 1; i <= MAX_FP; i++) {
    fpUsed[i] = false;
  }

  // We only load models for profiles that actually exist in LittleFS.
  // This avoids scanning the entire 200 slots on the sensor and deleting orphans,
  // which causes brownouts/resets due to high write power consumption.
  for (int i = 1; i <= MAX_FP; i++) {
    esp_task_wdt_reset(); // Pet the watchdog during sync
    String filename = "/" + String(i) + "_f.txt";
    
    if (LittleFS.exists(filename)) {
      uint8_t result = finger.loadModel(i);
      if (result == FINGERPRINT_OK) {
        fpUsed[i] = true;
        loadedCount++;
      } else {
        Serial.printf("⚠ Profile for ID %d exists in storage, but template is missing on sensor!\n", i);
      }
    }
    // Small delay to yield to scheduler
    delay(5);
  }
  Serial.printf("✓ Fingerprint database synced. Active templates: %d\n", loadedCount);
}

// --- Helper Functions (UNCHANGED) ---
String getTodayFileName() {
  struct tm ti;
  if (!getLocalTime(&ti, 200))
    return "/logs_error.csv";
  char buf[30];
  strftime(buf, sizeof(buf), "/logs_%d-%m-%Y.csv", &ti);
  return String(buf);
}

String getTodayDate() {
  struct tm ti;
  if (!getLocalTime(&ti, 200))
    return "00-00-0000";
  char buf[15];
  strftime(buf, sizeof(buf), "%d-%m-%Y", &ti);
  return String(buf);
}

String getNow() {
  struct tm ti;
  if (!getLocalTime(&ti, 200)) // Increased timeout from 10 to 200ms
    return "00:00:00";
  char buf[10];
  strftime(buf, sizeof(buf), "%H:%M:%S", &ti);
  return String(buf);
}

// Audio functions and setup removed

// Helper function to control the ESP32-S3 onboard LED (handles both RGB
// NeoPixel and standard LED)
void controlOnboardLed(bool on, uint8_t r = 0, uint8_t g = 0, uint8_t b = 64) {
#if !DISABLE_RGB_LED
#ifdef RGB_BUILTIN
  if (on) {
    neopixelWrite(RGB_BUILTIN, r, g, b);
  } else {
    neopixelWrite(RGB_BUILTIN, 0, 0, 0);
  }
#elif defined(LED_BUILTIN)
  digitalWrite(LED_BUILTIN, on ? HIGH : LOW);
#endif
#else
  Serial.println(
      "ℹ️ Onboard RGB LED control is disabled by user configuration.");
#endif
}

// Non-blocking task to beep the buzzer 3 times on denied access (crisp 80ms beeps)
void triggerDeniedBuzzer() {
  xTaskCreate(
      [](void *p) {
        for (int j = 0; j < 3; j++) {
          digitalWrite(BUZZER_PIN, HIGH);
          vTaskDelay(pdMS_TO_TICKS(80));
          digitalWrite(BUZZER_PIN, LOW);
          if (j < 2) {
            vTaskDelay(pdMS_TO_TICKS(80));
          }
        }
        vTaskDelete(NULL);
      },
      "denied_buzzer_task", 2048, NULL, 1, NULL);
}

// Non-blocking task to blink Denied LED 2 times
void triggerDeniedLed() {
  // Always trigger the buzzer alerts for every denied punch
  triggerDeniedBuzzer();

  if (isDeniedLedRunning) {
    Serial.println("[Denied Alert] denied_led_task is already running. "
                   "Skipping duplicate LED blink task spawn.");
    return;
  }
  isDeniedLedRunning = true;
  Serial.println("[Denied Alert] Spawning denied_led_task...");

  BaseType_t result = xTaskCreate(
      [](void *p) {
        Serial.println("[Denied Alert Task] Task started.");

        // 1. Turn on Red LED immediately for 2 seconds (first blink)
        digitalWrite(DENIED_LED_PIN, HIGH);
        vTaskDelay(pdMS_TO_TICKS(2000));

        // 2. First blink finished, turn off Red LED
        digitalWrite(DENIED_LED_PIN, LOW);
        vTaskDelay(pdMS_TO_TICKS(500)); // Off time between blinks

        // 3. Second blink of Red LED (2 seconds)
        digitalWrite(DENIED_LED_PIN, HIGH);
        vTaskDelay(pdMS_TO_TICKS(2000));
        digitalWrite(DENIED_LED_PIN, LOW);

        Serial.println("[Denied Alert Task] Task finished, self-deleting.");
        isDeniedLedRunning = false;
        vTaskDelete(NULL);
      },
      "denied_led_task", 4096, NULL, 1, NULL);

  if (result != pdPASS) {
    isDeniedLedRunning = false;
    Serial.println(
        "[Denied Alert] Error: Failed to create LED task due to low memory!");
  }
}

// Helper function to extract a float/numeric value from JSON key
float extractJsonFloat(const String& json, const String& key, int startPos = 0) {
  int keyIndex = json.indexOf("\"" + key + "\"", startPos);
  if (keyIndex == -1) return -999.0;
  
  int colonIndex = json.indexOf(':', keyIndex);
  if (colonIndex == -1) return -999.0;
  
  // Find where the value starts (skip spaces)
  int startIdx = colonIndex + 1;
  while (startIdx < json.length() && (json.charAt(startIdx) == ' ' || json.charAt(startIdx) == '\r' || json.charAt(startIdx) == '\n')) {
    startIdx++;
  }
  
  // Find where the value ends
  int endIdx = startIdx;
  while (endIdx < json.length() && 
         json.charAt(endIdx) != ',' && 
         json.charAt(endIdx) != '}' && 
         json.charAt(endIdx) != ']' && 
         json.charAt(endIdx) != ' ' && 
         json.charAt(endIdx) != '\r' && 
         json.charAt(endIdx) != '\n') {
    endIdx++;
  }
  
  String valStr = json.substring(startIdx, endIdx);
  valStr.trim();
  return valStr.toFloat();
}

// Helper function to extract a string value from JSON key
String extractJsonString(const String& json, const String& key, int startPos = 0) {
  int keyIndex = json.indexOf("\"" + key + "\"", startPos);
  if (keyIndex == -1) return "";
  
  int colonIndex = json.indexOf(':', keyIndex);
  if (colonIndex == -1) return "";
  
  // Find the opening quote of the string value
  int quoteStart = json.indexOf('"', colonIndex + 1);
  if (quoteStart == -1) return "";
  
  // Find the closing quote
  int quoteEnd = json.indexOf('"', quoteStart + 1);
  if (quoteEnd == -1) return "";
  
  return json.substring(quoteStart + 1, quoteEnd);
}

void weatherTask(void *p) {
  while (true) {
    extern volatile bool isOtaUpdating;
    if (isOtaUpdating) {
      vTaskDelay(pdMS_TO_TICKS(1000));
      continue;
    }

    bool netConnected = false;
#if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0)
    bool ethActive = (ETH.handle() != NULL);
    netConnected = (ethActive && ETH.linkUp()) || (WiFi.status() == WL_CONNECTED);
#else
    netConnected = (ETH.localIP() != IPAddress(0, 0, 0, 0)) ||
                   (WiFi.status() == WL_CONNECTED);
#endif

    if (netConnected) {
      extern String googleApiKey;
      if (googleApiKey.length() > 0) {
        // === GOOGLE MAPS WEATHER API ===
        float lat = 19.0330; // Default Navi-Mumbai
        float lon = 73.0297;
        bool geocodeOk = false;

        String searchCity = weatherCity;
        searchCity.replace("-", "%20");
        searchCity.trim();

        WiFiClientSecure secureClient;
        secureClient.setInsecure(); // Skip certificate verification

        HTTPClient http;
        String geocodeUrl = "https://maps.googleapis.com/maps/api/geocode/json?address=" + searchCity + "&key=" + googleApiKey;
        Serial.print("🌡️ Google Weather: Geocoding city: ");
        Serial.println(weatherCity);
        
        http.begin(secureClient, geocodeUrl);
        http.setTimeout(8000); // 8 seconds timeout
        int httpCode = http.GET();
        if (httpCode == 200) {
          String resp = http.getString();
          int locBlock = resp.indexOf("\"location\"");
          if (locBlock != -1) {
            float parsedLat = extractJsonFloat(resp, "lat", locBlock);
            float parsedLon = extractJsonFloat(resp, "lng", locBlock);
            if (parsedLat != -999.0 && parsedLon != -999.0) {
              lat = parsedLat;
              lon = parsedLon;
              geocodeOk = true;
              Serial.printf("🌡️ Google Weather: Geocoding success: Lat %.5f, Lon %.5f\n", lat, lon);
            }
          }
        }
        if (!geocodeOk) {
          Serial.println("⚠ Google Weather: Geocoding failed or coordinates missing. Using default Navi-Mumbai.");
        }
        http.end();

        // Fetch forecast from Google Weather API
        String forecastUrl = "https://weather.googleapis.com/v1/currentConditions:lookup?location.latitude=" + String(lat, 5) + 
                             "&location.longitude=" + String(lon, 5) + "&key=" + googleApiKey;
        
        Serial.print("🌡️ Google Weather: Fetching conditions for Lat: ");
        Serial.print(lat, 4);
        Serial.print(", Lon: ");
        Serial.println(lon, 4);

        http.begin(secureClient, forecastUrl);
        http.setTimeout(8000); // 8 seconds timeout
        httpCode = http.GET();
        
        if (httpCode == 200) {
          String resp = http.getString();
          
          int tempBlock = resp.indexOf("\"temperature\"");
          float temp = extractJsonFloat(resp, "degrees", tempBlock);
          
          float hum = extractJsonFloat(resp, "relativeHumidity");
          
          int windBlock = resp.indexOf("\"wind\"");
          float wind = extractJsonFloat(resp, "value", windBlock);
          
          int descBlock = resp.indexOf("\"description\"");
          String cond = "";
          if (descBlock != -1) {
            cond = extractJsonString(resp, "text", descBlock);
          }
          
          if (temp != -999.0 && hum != -999.0 && wind != -999.0) {
            outsideTemp = temp;
            weatherHum = String((int)hum) + "%";
            weatherWind = String(wind, 1) + " km/h";
            if (cond.length() > 0) {
              weatherCond = cond;
            } else {
              weatherCond = "Clear";
            }
            
            Serial.printf("🌡️ Google Weather: Updated: %.1f C | Hum: %s | Wind: %s | Cond: %s\n",
                          outsideTemp, weatherHum.c_str(), weatherWind.c_str(), weatherCond.c_str());

            // Save to LittleFS
            File f = LittleFS.open("/last_weather.txt", FILE_WRITE);
            if (f) {
              f.println(outsideTemp);
              f.println(weatherCond);
              f.println(weatherHum);
              f.println(weatherWind);
              f.close();
            }
            tftForceClockRedraw = true;
          } else {
            Serial.println("❌ Google Weather: Parsing failed (some fields missing in JSON).");
          }
        } else {
          Serial.printf("❌ Google Weather: HTTP lookup failed: %d\n", httpCode);
        }
        http.end();
      } else {
        // === OPEN-METEO FALLBACK ===
        float lat = 19.0330; // Default Navi-Mumbai
        float lon = 73.0297;
        bool geocodeOk = false;

        String searchCity = weatherCity;
        searchCity.replace("-", "%20");
        searchCity.trim();

        HTTPClient http;
        String geocodeUrl = "http://geocoding-api.open-meteo.com/v1/search?name=" + searchCity + "&count=1&language=en&format=json";
        Serial.print("🌡️ Weather Task (Open-Meteo): Geocoding city: ");
        Serial.println(weatherCity);
        
        http.begin(geocodeUrl);
        http.setTimeout(8000); // 8 seconds timeout
        int httpCode = http.GET();
        if (httpCode == 200) {
          String resp = http.getString();
          float parsedLat = extractJsonFloat(resp, "latitude");
          float parsedLon = extractJsonFloat(resp, "longitude");
          if (parsedLat != -999.0 && parsedLon != -999.0) {
            lat = parsedLat;
            lon = parsedLon;
            geocodeOk = true;
            Serial.printf("🌡️ Weather Task (Open-Meteo): Geocoding success: Lat %.5f, Lon %.5f\n", lat, lon);
          } else {
            Serial.println("⚠ Weather Task (Open-Meteo): Geocoding coordinates missing in JSON. Using default Navi-Mumbai.");
          }
        } else {
          Serial.printf("❌ Weather Task (Open-Meteo): Geocoding HTTP failed: %d. Using default Navi-Mumbai.\n", httpCode);
        }
        http.end();

        // Now fetch forecast for lat/lon
        String forecastUrl = "http://api.open-meteo.com/v1/forecast?latitude=" + String(lat, 5) + 
                              "&longitude=" + String(lon, 5) + 
                              "&current=temperature_2m,relative_humidity_2m,wind_speed_10m,weather_code";
        
        Serial.print("🌡️ Weather Task (Open-Meteo): Fetching forecast for Lat: ");
        Serial.print(lat, 4);
        Serial.print(", Lon: ");
        Serial.println(lon, 4);

        http.begin(forecastUrl);
        http.setTimeout(8000); // 8 seconds timeout
        httpCode = http.GET();
        
        if (httpCode == 200) {
          String resp = http.getString();
          int currentBlock = resp.indexOf("\"current\"");
          if (currentBlock == -1) currentBlock = 0;
          float temp = extractJsonFloat(resp, "temperature_2m", currentBlock);
          float hum = extractJsonFloat(resp, "relative_humidity_2m", currentBlock);
          float wind = extractJsonFloat(resp, "wind_speed_10m", currentBlock);
          float wcode = extractJsonFloat(resp, "weather_code", currentBlock);
          
          if (temp != -999.0 && hum != -999.0 && wind != -999.0 && wcode != -999.0) {
            outsideTemp = temp;
            weatherHum = String((int)hum) + "%";
            weatherWind = String(wind, 1) + " km/h";
            
            switch ((int)wcode) {
              case 0: weatherCond = "Clear"; break;
              case 1: case 2: weatherCond = "Partly Cloudy"; break;
              case 3: weatherCond = "Overcast"; break;
              case 45: case 48: weatherCond = "Foggy"; break;
              case 51: case 53: case 55: weatherCond = "Drizzle"; break;
              case 56: case 57: weatherCond = "Freezing Drizzle"; break;
              case 61: case 63: weatherCond = "Light Rain"; break;
              case 65: weatherCond = "Heavy Rain"; break;
              case 66: case 67: weatherCond = "Freezing Rain"; break;
              case 71: case 73: case 75: weatherCond = "Snowy"; break;
              case 77: weatherCond = "Snow Grains"; break;
              case 80: case 81: case 82: weatherCond = "Rainy"; break;
              case 85: case 86: weatherCond = "Snow Showers"; break;
              case 95: case 96: case 99: weatherCond = "Thunderstorm"; break;
              default: weatherCond = "Clear"; break;
            }
            
            Serial.printf("🌡️ Weather Task (Open-Meteo): Updated: %.1f C | Hum: %s | Wind: %s | Cond: %s\n",
                          outsideTemp, weatherHum.c_str(), weatherWind.c_str(), weatherCond.c_str());

            // Save to LittleFS
            File f = LittleFS.open("/last_weather.txt", FILE_WRITE);
            if (f) {
              f.println(outsideTemp);
              f.println(weatherCond);
              f.println(weatherHum);
              f.println(weatherWind);
              f.close();
            }
            tftForceClockRedraw = true;
          } else {
            Serial.println("❌ Weather Task (Open-Meteo): Parsing failed (some fields missing in JSON).");
          }
        } else {
          Serial.printf("❌ Weather Task (Open-Meteo): Forecast HTTP failed: %d\n", httpCode);
        }
        http.end();
      }
      
      ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(1800000)); // Sleep 30 mins or until notified
    } else {
      ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(10000)); // Sleep 10 seconds or until notified
    }
  }
}

// Non-blocking task to turn on the buzzer for a specified duration in
// milliseconds
void triggerBuzzer(int durationMs) {
  xTaskCreate(
      [](void *p) {
        int dur = (int)(intptr_t)p;
        digitalWrite(BUZZER_PIN, HIGH);
        vTaskDelay(pdMS_TO_TICKS(dur));
        digitalWrite(BUZZER_PIN, LOW);
        vTaskDelete(NULL);
      },
      "buzzer_task", 2048, (void *)(intptr_t)durationMs, 1, NULL);
}

void sendWebhook(String json) {
  if (webhookUrl.length() < 10)
    return;
  if (WiFi.status() != WL_CONNECTED)
    return;

  // Create a copy of the json string on the heap to pass to the task
  String *payload = new String(json);

  xTaskCreate(
      [](void *p) {
        String *data = (String *)p;
        HTTPClient http;
        http.begin(webhookUrl);
        http.addHeader("Content-Type", "application/json");
        int code = http.POST(*data);
        http.end();
        delete data;
        vTaskDelete(NULL);
      },
      "webhook_task", 4096, payload, 1, NULL);
}

void processAccess(String id, bool isFinger) {
  // If card is scanned, check if it's a linked backup card
  String originalScannedId = id;
  bool wasCard = !isFinger;
  String linkedFingerId = "";

  if (wasCard) {
    String cardPath = "/" + id + ".txt";
    if (LittleFS.exists(cardPath)) {
      File f = LittleFS.open(cardPath, FILE_READ);
      if (f) {
        String content = f.readString();
        f.close();
        if (content.startsWith("LINK|")) {
          linkedFingerId = content.substring(5);
          linkedFingerId.trim();
        }
      }
    }
  }

  if (wasCard) {
    if (twoFactorEnabled) {
      if (linkedFingerId.length() > 0) {
        // Setup 2FA State
        pending2FAFingerId = linkedFingerId;
        pending2FACardUid = originalScannedId;
        pending2FATimeout = millis() + 10000; // 10 seconds timeout

        // Fetch User Info for the screen
        String fProfile = "/" + linkedFingerId + "_f.txt";
        String name = "Employee";
        String roll = linkedFingerId;
        if (LittleFS.exists(fProfile)) {
          File pf = LittleFS.open(fProfile, FILE_READ);
          if (pf) {
            String c = pf.readString();
            pf.close();
            int pipes[8];
            int pCount = 0;
            int lastPos = -1;
            for (int i = 0; i < 8; i++) {
              pipes[i] = c.indexOf('|', lastPos + 1);
              if (pipes[i] != -1) {
                lastPos = pipes[i];
                pCount++;
              }
            }
            if (pCount >= 3) {
              name = c.substring(0, pipes[0]);
              roll = c.substring(pipes[1] + 1, pipes[2]);
            }
          }
        }
        pending2FAName = name;
        pending2FARoll = roll;

        triggerBuzzer(150);
        drawTft2FAPromptScreen(name, roll);
        return;
      } else {
        triggerDeniedLed();
        currentTftState = TFT_PUNCH_DENIED;
        drawTftDeniedScreen("Card Not Linked", "2-Factor Required");
        tftMessageUntil = millis() + 1500;
        lastScanUID = id;
        lastScanTime = millis();
        return;
      }
    } else {
      // Normal flow (No 2FA)
      if (linkedFingerId.length() > 0) {
        id = linkedFingerId;
        isFinger = true;
      }
    }
  }

  if (millis() - lastScanTime < 1000 && id == lastScanUID)
    return;

  // Ignore scans for fingers that are registered on the sensor but do not have
  // a profile yet (pending registration)
  if (isFinger) {
    if (twoFactorEnabled) {
      if (millis() < pending2FATimeout && id == pending2FAFingerId) {
        // Match! Reset 2FA
        pending2FAFingerId = "";
        pending2FACardUid = "";
        pending2FATimeout = 0;
      } else {
        triggerDeniedLed();
        currentTftState = TFT_PUNCH_DENIED;
        if (pending2FAFingerId.length() > 0) {
          drawTftDeniedScreen(pending2FAName, "Wrong Finger 2FA");
        } else {
          drawTftDeniedScreen("Scan Card First", "2-Factor Sequence");
        }
        pending2FAFingerId = "";
        pending2FACardUid = "";
        pending2FATimeout = 0;
        tftMessageUntil = millis() + 1500;
        lastScanUID = id;
        lastScanTime = millis();
        return;
      }
    }

    int fpId = id.toInt();
    if (fpId >= 1 && fpId <= MAX_FP && fpUsed[fpId] &&
        !LittleFS.exists("/" + id + "_f.txt")) {
      Serial.printf(
          "ℹ️ Ignoring scan for ID %d: User profile registration is pending.\n",
          fpId);
      lastScanUID = id;
      lastScanTime = millis();
      return;
    }
  }

  String filename = "/" + id + (isFinger ? "_f.txt" : ".txt");
  String name = "Employee";
  String roll = id;
  bool accepted = false;
  String c = "";

  if (LittleFS.exists(filename)) {
    File f = LittleFS.open(filename, FILE_READ);
    if (f) {
      c = f.readString();
      f.close();
      
      int pipes[8];
      int pCount = 0;
      int lastPos = -1;
      for (int i = 0; i < 8; i++) {
        pipes[i] = c.indexOf('|', lastPos + 1);
        if (pipes[i] != -1) {
          lastPos = pipes[i];
          pCount++;
        } else {
          pipes[i] = -1;
        }
      }

      if (pCount >= 3) {
        name = c.substring(0, pipes[0]);
        roll = c.substring(pipes[1] + 1, pipes[2]);
        accepted = true;
      }
    }
  }

  if (accepted) {
    // Beep briefly to confirm scanner read
    triggerBuzzer(150);

    if (tftPromptEnabled) {
      // Save pending state and trigger direction selection screen
      hasPendingPunch = true;
      pendingPunchId = id;
      pendingPunchIsFinger = isFinger;
      pendingPunchTimeout = millis() + 8000; // 8 seconds timeout
      pendingPunchName = name;
      pendingPunchRoll = roll;

      drawTftDirectionSelectionScreen(name, roll);
    } else {
      // Auto direction selection!
      int pipes[8];
      int pCount = 0;
      int lastPos = -1;
      for (int i = 0; i < 8; i++) {
        pipes[i] = c.indexOf('|', lastPos + 1);
        if (pipes[i] != -1) {
          lastPos = pipes[i];
          pCount++;
        } else {
          pipes[i] = -1;
        }
      }
      
      String stateStr = (pCount >= 4) ? c.substring(pipes[2] + 1, pipes[3]) : "0";
      String lastDate = (pCount >= 5) ? c.substring(pipes[3] + 1, pipes[4]) : "";
      stateStr.trim();
      lastDate.trim();
      
      String today = getTodayDate();
      String autoDir = "In";
      
      if (lastDate == today) {
        autoDir = "Out";
      } else {
        autoDir = "In"; // First punch of the day
      }
      
      savePunchRecord(id, isFinger, autoDir);
    }
  } else {
    // Non-enrolled scan denied immediately
    // Update lastScanUID/lastScanTime so the 2-second guard works correctly
    // on re-scans of the same denied card.
    lastScanUID = id;
    lastScanTime = millis();
    triggerDeniedLed();
    ws.textAll("{\"type\":\"SCAN\",\"uid\":\"" + id + "\",\"status\":\"Denied\"}");
    currentTftState = TFT_PUNCH_DENIED;
    drawTftDeniedScreen("Not Enrolled", "ACCESS DENIED");
    tftMessageUntil = millis() + 1500;
  }
}

void savePunchRecord(String id, bool isFinger, String selectedDir) {
  String method = isFinger ? "Fingerprint" : "RFID Card";
  String filename = "/" + id + (isFinger ? "_f.txt" : ".txt");
  
  struct tm ti;
  bool gotTime = getLocalTime(&ti, 100);
  
  // Format current time "HH:MM:SS"
  char tBuf[12] = "00:00:00";
  if (gotTime) {
    strftime(tBuf, sizeof(tBuf), "%H:%M:%S", &ti);
  }
  String t = String(tBuf);
  
  // Format today's date "DD-MM-YYYY"
  char dateBuf[16] = "00-00-0000";
  if (gotTime) {
    strftime(dateBuf, sizeof(dateBuf), "%d-%m-%Y", &ti);
  }
  String today = String(dateBuf);

  // Format log filename "/logs_DD-MM-YYYY.csv"
  char fileBuf[35] = "/logs_error.csv";
  if (gotTime) {
    strftime(fileBuf, sizeof(fileBuf), "/logs_%d-%m-%Y.csv", &ti);
  }
  String logFilename = String(fileBuf);

  String name = "Unknown", role = "-", roll = id, dir = selectedDir;
  bool accepted = false;

  if (LittleFS.exists(filename)) {
    digitalWrite(LED_PIN, HIGH);
    File f = LittleFS.open(filename, FILE_READ);
    if (f) {
      String c = f.readString();
      f.close();

      // Robust Parsing: find up to 8 pipe positions (9 fields)
      int pipes[8];
      int pCount = 0;
      int lastPos = -1;
      for (int i = 0; i < 8; i++) {
        pipes[i] = c.indexOf('|', lastPos + 1);
        if (pipes[i] != -1) {
          lastPos = pipes[i];
          pCount++;
        } else {
          pipes[i] = -1;
        }
      }

      if (pCount >= 3) {
        name = c.substring(0, pipes[0]);
        role = c.substring(pipes[0] + 1, pipes[1]);
        roll = c.substring(pipes[1] + 1, pipes[2]);

        String stateStr =
            (pCount >= 4) ? c.substring(pipes[2] + 1, pipes[3]) : "0";
        String lastDate =
            (pCount >= 5) ? c.substring(pipes[3] + 1, pipes[4]) : "";

        String firstTime = "-";
        String lastTime = "-";
        String rfidVal = "";
        String deptVal = "-";

        if (filename.endsWith("_f.txt")) {
          // Fingerprint user profile
          if (pCount >= 8) {
            firstTime = c.substring(pipes[4] + 1, pipes[5]);
            lastTime = c.substring(pipes[5] + 1, pipes[6]);
            rfidVal = c.substring(pipes[6] + 1, pipes[7]);
            deptVal = c.substring(pipes[7] + 1);
          } else if (pCount >= 7) {
            firstTime = c.substring(pipes[4] + 1, pipes[5]);
            lastTime = c.substring(pipes[5] + 1, pipes[6]);
            rfidVal = c.substring(pipes[6] + 1);
          } else if (pCount >= 6) {
            firstTime = c.substring(pipes[4] + 1, pipes[5]);
            lastTime = c.substring(pipes[5] + 1);
          } else if (pCount >= 5) {
            firstTime = c.substring(pipes[4] + 1);
          }
        } else {
          // Plain RFID card user profile
          if (pCount >= 7) {
            firstTime = c.substring(pipes[4] + 1, pipes[5]);
            lastTime = c.substring(pipes[5] + 1, pipes[6]);
            deptVal = c.substring(pipes[6] + 1);
          } else if (pCount >= 6) {
            firstTime = c.substring(pipes[4] + 1, pipes[5]);
            lastTime = c.substring(pipes[5] + 1);
          } else if (pCount >= 5) {
            firstTime = c.substring(pipes[4] + 1);
          }
        }

        stateStr.trim();
        lastDate.trim();
        firstTime.trim();
        lastTime.trim();
        rfidVal.trim();
        deptVal.trim();

        int punchesToday = stateStr.toInt();
        if (lastDate != today) {
          punchesToday = 1;
          firstTime = t;
          lastTime = "";
        } else {
          punchesToday++;
          lastTime = t; // Update departure time
        }

        dir = selectedDir;
        accepted = true;

        File fw = LittleFS.open(filename, FILE_WRITE);
        if (fw) {
          if (filename.endsWith("_f.txt")) {
            fw.print(name + "|" + role + "|" + roll + "|" +
                     String(punchesToday) + "|" + today + "|" + firstTime +
                     "|" + lastTime + "|" + rfidVal + "|" + deptVal);
          } else {
            fw.print(name + "|" + role + "|" + roll + "|" +
                     String(punchesToday) + "|" + today + "|" + firstTime +
                     "|" + lastTime + "|" + deptVal);
          }
          fw.close();
        }
      } else {
        name = "Corrupted Record";
      }
    }
  } else {
    triggerDeniedLed();
    name = "Not Enrolled";
  }

  String punchStatus = "Accepted"; // default fallback
  if (accepted) {
    if (gotTime) {
      int curTotalMins = ti.tm_hour * 60 + ti.tm_min;
      if (dir == "In") {
        int startTotalMins = shiftStartHour * 60 + shiftStartMin;
        if (curTotalMins > (startTotalMins + graceMins)) {
          punchStatus = "Late";
        } else {
          punchStatus = "On-Time";
        }
      } else if (dir == "Out") {
        int endTotalMins = shiftEndHour * 60 + shiftEndMin;
        if (curTotalMins < endTotalMins) {
          punchStatus = "Early Exit";
        } else {
          punchStatus = "On-Time";
        }
      }
    } else {
      punchStatus = "On-Time";
    }

    bool eepromFull = (eepromTotalLogs >= MAX_EEPROM_LOGS);
    if (useEEPROM && !eepromFull) {
      EEPROMLogEntry entry;
      time_t nowTime;
      time(&nowTime);
      entry.timestamp = nowTime;

      memset(entry.uid, 0, sizeof(entry.uid));
      strncpy(entry.uid, id.c_str(), sizeof(entry.uid) - 1);

      entry.direction = (dir == "In") ? 1 : 2;

      if (punchStatus == "On-Time")
        entry.status = 1;
      else if (punchStatus == "Late")
        entry.status = 2;
      else if (punchStatus == "Early Exit")
        entry.status = 3;
      else if (punchStatus == "Accepted")
        entry.status = 4;
      else if (punchStatus == "Denied")
        entry.status = 5;
      else
        entry.status = 4;

      if (method == "Fingerprint")
        entry.method = 1;
      else if (method == "RFID Card")
        entry.method = 2;
      else if (method == "Manual")
        entry.method = 3;
      else
        entry.method = 2;

      addEEPROMLog(entry);
    } else {
      // Circular Log Check: Ensure space is available (approx 100KB buffer)
      if (fsTotalBytes - fsUsedBytes < 102400) {
        Serial.println("Storage low, clearing error logs...");
        LittleFS.remove("/logs_error.csv");
      }

      bool newFile = !LittleFS.exists(logFilename);
      File logFile = LittleFS.open(logFilename, FILE_APPEND);
      if (!logFile)
        logFile = LittleFS.open(logFilename, FILE_WRITE);
      if (logFile) {
        if (newFile) {
          logFile.println("Time,ID,Name,Role,Direction,Status,Type");
        }
        logFile.println(t + "," + roll + "," + name + "," + role + "," + dir +
                        "," + punchStatus + "," + method);
        logFile.close();
      }
    }
  }

  lastScanUID = id;
  lastScanTime = millis();

  // Enhanced WebSocket Payload with detailed punchStatus
  String wsData = "{\"type\":\"SCAN\",\"uid\":\"" + id + "\",\"name\":\"" +
                  name + "\",\"role\":\"" + role + "\",\"dir\":\"" + dir +
                  "\",\"status\":\"" + (accepted ? punchStatus : "Denied") +
                  "\"}";
  ws.textAll(wsData);

  if (accepted) {
    extern void queueSqlSync(String id, String name, String role, String dir, String status, String type);
    queueSqlSync(id, name, role, dir, punchStatus, method);
  }

  // Trigger Buzzer Alert
  if (accepted) {
    triggerBuzzer(1000);
  }

  // Display scan results on TFT
  if (accepted) {
    currentTftState = TFT_PUNCH_SUCCESS;
    drawTftSuccessScreen(name, dir, punchStatus, t, roll);
  } else {
    currentTftState = TFT_PUNCH_DENIED;
    drawTftDeniedScreen(name, "ACCESS DENIED");
  }
  tftMessageUntil = millis() + 1200;
}

void autoCleanLogs() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo, 200))
    return; // Wait until time is synced

  time_t now = mktime(&timeinfo);
  double max_seconds = 30.0 * 24 * 60 * 60; // 30 days threshold

  File root = LittleFS.open("/");
  File file = root.openNextFile();
  while (file) {
    String fn = file.name();
    if (fn.startsWith("logs_") && fn.endsWith(".csv")) {
      int d = fn.substring(5, 7).toInt();
      int m = fn.substring(8, 10).toInt();
      int y = fn.substring(11, 15).toInt();

      if (y > 2000) { // Basic validation
        struct tm filetm = {0};
        filetm.tm_mday = d;
        filetm.tm_mon = m - 1;
        filetm.tm_year = y - 1900;
        time_t filetime = mktime(&filetm);

        if (difftime(now, filetime) > max_seconds) {
          Serial.println("Auto-deleting old log: " + fn);
          LittleFS.remove("/" + fn);
        }
      }
    }
    file = root.openNextFile();
  }
}

// --- Dashboard HTML (stored in separate header to avoid Arduino preprocessor
// issues) ---
#include "html_page_gz.h"

void disableWatchdog() {
  Serial.println(
      "🛑 Temporarily increasing Task Watchdog timeout to 5 minutes...");
#if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0)
  esp_task_wdt_config_t wdt_config = {
      .timeout_ms = 300000, // 300 seconds (5 minutes)
      .idle_core_mask = (1 << portNUM_PROCESSORS) - 1,
      .trigger_panic = true,
  };
  esp_task_wdt_reconfigure(&wdt_config);
  esp_task_wdt_add(NULL); // Ensure current task is subscribed
#else
  esp_task_wdt_init(300, true);
  esp_task_wdt_add(NULL);
#endif
}

void enableWatchdog(); // Forward declaration for setup
void enableWatchdog() {
  Serial.println("🛡️ Re-enabling standard 60-second Task Watchdog...");
#if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0)
  esp_task_wdt_config_t wdt_config = {
      .timeout_ms = WDT_TIMEOUT * 1000, // 60 seconds
      .idle_core_mask = (1 << portNUM_PROCESSORS) - 1,
      .trigger_panic = true,
  };
  esp_task_wdt_reconfigure(&wdt_config);
  esp_task_wdt_add(NULL); // Ensure current task is subscribed
#else
  esp_task_wdt_init(WDT_TIMEOUT, true);
  esp_task_wdt_add(NULL);
#endif
}

bool checkW5500Link() {
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
  
  pinMode(ETH_CS_PIN, OUTPUT);
  digitalWrite(ETH_CS_PIN, LOW);
  
  SPI.transfer(0x00); // Address High (0x00)
  SPI.transfer(0x2E); // Address Low (0x2E)
  SPI.transfer(0x00); // Control Byte (Read, Block 0 (Common), Variable length)
  uint8_t phycfgr = SPI.transfer(0x00); // Read register data
  
  digitalWrite(ETH_CS_PIN, HIGH);
  
  SPI.endTransaction();

  Serial.printf("🔍 W5500 PHYCFGR read via SPI library: 0x%02X\n", phycfgr);

  if (phycfgr == 0x00 || phycfgr == 0xFF) {
    return false;
  }

  // Bit 0: Link status (1 = Link up, 0 = Link down)
  return (phycfgr & 0x01) == 0x01;
}

// Helper function to draw premium vector network status screens (Ethernet/WiFi DHCP initialization)
void drawNetworkInitScreen(const String& title, const String& subtitle, const String& details, uint16_t titleColor, uint16_t subtitleColor, bool showAnim = true, int animOffset = 0) {
  if (tftWasPowerCycled) return;
  uint16_t bg = 0x10A2;
  
  if (currentTftState != TFT_BOOT) {
    tft.fillScreen(0x0821);
    
    // Outer frame
    tft.drawRoundRect(10, 10, 300, 460, 12, 0x03EF);
    
    // Center Card
    tft.fillRoundRect(28, 113, 264, 254, 8, 0x0410); // Shadow
    tft.fillRoundRect(25, 110, 264, 254, 8, bg);     // Body
    tft.drawRoundRect(25, 110, 264, 254, 8, 0x028A); // Border
    tft.drawRoundRect(26, 111, 262, 252, 8, 0x03EF); // Glow
    
    currentTftState = TFT_BOOT;
  }
  
  // Clear only the inside of the card (preventing full screen flickering)
  tft.fillRect(27, 112, 260, 250, bg);

  // Connection symbol (centered at 160, 175)
  if (showAnim) {
    tft.drawCircle(160, 175, 26, 0x028A);
    tft.drawCircle(160, 175, 20, 0x03EF);
    // Draw loading rotating dots
    float rad = (animOffset * 45) * 0.0174532925f;
    tft.fillCircle(160 + 20 * cos(rad), 175 + 20 * sin(rad), 4, TFT_WHITE);
  } else {
    // Solid success circle with checkmark
    tft.fillCircle(160, 175, 26, 0x018C);
    tft.drawCircle(160, 175, 26, TFT_GREEN);
    tft.drawLine(150, 175, 157, 182, TFT_WHITE);
    tft.drawLine(157, 182, 172, 167, TFT_WHITE);
  }

  drawAutoScaledString(title, 160, 235, 4, 1.0f, 240, titleColor, bg, true);
  drawAutoScaledString(subtitle, 160, 275, 2, 1.5f, 240, subtitleColor, bg, true);
  drawAutoScaledString(details, 160, 315, 2, 1.1f, 240, TFT_WHITE, bg, true);
}


void setup() {
  // 1. Immediately drive all SPI CS pins HIGH to prevent floating state collisions
  // and minimize signal-leakage power consumption on startup.
  pinMode(TFT_CS, OUTPUT);
  digitalWrite(TFT_CS, HIGH);
  pinMode(T_CS, OUTPUT);
  digitalWrite(T_CS, HIGH);
  pinMode(SS_PIN, OUTPUT);
  digitalWrite(SS_PIN, HIGH);
  pinMode(ETH_CS_PIN, OUTPUT);
  digitalWrite(ETH_CS_PIN, HIGH);

  // 3. Keep the TFT backlight OFF initially to reduce boot current by ~100-150mA.
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, LOW);

  // 4. Add a 1000ms startup delay to let the power supply voltage fully stabilize.
  delay(1000);

  // 5. Perform clean hardware reset on TFT screen while voltage is stable
  pinMode(TFT_RST, OUTPUT);
  digitalWrite(TFT_RST, LOW);   // Active low reset
  delay(100);
  digitalWrite(TFT_RST, HIGH);  // Release reset
  delay(150);

  Serial.begin(115200);
  
  esp_reset_reason_t reason = esp_reset_reason();
  Serial.print("\nℹ️ ESP32 Reset Reason: ");
  switch (reason) {
    case ESP_RST_POWERON:   Serial.println("Power-on reset"); break;
    case ESP_RST_EXT:       Serial.println("External pin reset (e.g. Reset Button)"); break;
    case ESP_RST_SW:        Serial.println("Software reset (ESP.restart())"); break;
    case ESP_RST_PANIC:     Serial.println("Panic / Crash reset"); break;
    case ESP_RST_INT_WDT:   Serial.println("Interrupt Watchdog reset"); break;
    case ESP_RST_TASK_WDT:  Serial.println("Task Watchdog reset"); break;
    case ESP_RST_WDT:       Serial.println("Other Watchdog reset"); break;
    case ESP_RST_DEEPSLEEP: Serial.println("Deep sleep wake-up"); break;
    case ESP_RST_BROWNOUT:  Serial.println("Brownout reset (Voltage drop - check power supply!)"); break;
    case ESP_RST_SDIO:      Serial.println("SDIO reset"); break;
    default:                Serial.printf("Unknown reset reason (%d)\n", reason); break;
  }

  disableWatchdog(); // Temporarily increase watchdog timeout during long setup/sync phase

  // Initialize LCD Display & Shared I2C Bus
  Wire.begin(LCD_SDA_PIN, LCD_SCL_PIN);

  // Test and initialize External EEPROM
  Wire.beginTransmission(0x50);
  if (Wire.endTransmission() == 0) {
    useEEPROM = true;
    loadEEPROMHeader();
  } else {
    useEEPROM = false;
    Serial.println("⚠ AT24C512 EEPROM NOT detected. Scanning I2C bus...");
    byte error, address;
    int nDevices = 0;
    for (address = 1; address < 127; address++) {
      Wire.beginTransmission(address);
      error = Wire.endTransmission();
      if (error == 0) {
        Serial.printf("  ℹ️ Found I2C device at address: 0x%02X\n", address);
        nDevices++;
      }
    }
    if (nDevices == 0) {
      Serial.println(
          "  ❌ No I2C devices responded. Check your SDA/SCL wiring!");
    }
    Serial.println("Falling back to LittleFS logging.");
  }

  // Initialize standard Arduino SPI first so the SPI2_HOST bus driver is set up
  SPI.begin(12, 13, 11, -1);

  // Initialize TFT Display which will attach itself to the existing SPI2_HOST bus
  tft.init();

  tft.setRotation(4); // Portrait Mirrored (use 4 or 6 for vertical depending on cable direction)
  
  // Display Premium Booting screen (Drawn with backlight still off to prevent white screen flash)
  tft.fillScreen(0x0821); // Custom premium dark navy-blue background
  tft.drawRoundRect(10, 10, 300, 460, 12, 0x03EF); // Outer frame glow
  
  uint16_t bootBg = 0x10A2;
  tft.fillRoundRect(28, 113, 264, 254, 8, 0x0410); // Shadow
  tft.fillRoundRect(25, 110, 264, 254, 8, bootBg);     // Body
  tft.drawRoundRect(25, 110, 264, 254, 8, 0x028A); // Border
  tft.drawRoundRect(26, 111, 262, 252, 8, 0x03EF); // Glow
  
  // Loading graphic animation: Concentric rings and glowing dots
  tft.drawCircle(160, 175, 26, 0x028A);
  tft.drawCircle(160, 175, 20, 0x03EF);
  tft.fillCircle(160, 155, 4, TFT_WHITE); // Glowing top dot
  tft.fillCircle(160 + 17, 175 - 10, 2, TFT_CYAN);
  tft.fillCircle(160 + 17, 175 + 10, 2, TFT_CYAN);
  
  // Booting texts
  drawAutoScaledString("ATTENDANCE SYSTEM", 160, 235, 4, 1.0f, 240, TFT_WHITE, bootBg, true);
  drawAutoScaledString("System Booting...", 160, 275, 2, 1.5f, 240, TFT_YELLOW, bootBg, true);
  drawAutoScaledString("Initializing Hardware...", 160, 315, 2, 1.0f, 240, TFT_CYAN, bootBg, true);
  tft.setTextSize(1.0f); // Reset

  // Turn on the TFT Backlight now that the initial screen has been fully drawn
  digitalWrite(TFT_BL, HIGH);

  pinMode(T_IRQ, INPUT_PULLUP); // Configure Touch Interrupt pin as input with pull-up to monitor screen power status
  pinMode(LED_PIN, OUTPUT);
  pinMode(DENIED_LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  digitalWrite(DENIED_LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);

#ifdef LED_BUILTIN
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
#endif
#ifdef RGB_BUILTIN
  neopixelWrite(RGB_BUILTIN, 0, 0, 0); // Turn off NeoPixel on startup
#endif


  Serial.println("🔍 [DEBUG] Initializing MFRC522 RFID via Arduino SPI...");
  rfid.PCD_Init();
  delay(50); // MFRC522 datasheet: 50ms required after soft-reset before first read
  Serial.println("🔍 [DEBUG] MFRC522 RFID Initialization completed!");

  // Register Network Event Handler for Ethernet and Wi-Fi using lambda to avoid
  // IDE prototype bugs
  WiFi.onEvent([](WiFiEvent_t event) {
    switch (event) {
#if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0)
    case ARDUINO_EVENT_ETH_START:
      Serial.println("📡 Ethernet: Interface Started");
      break;
    case ARDUINO_EVENT_ETH_CONNECTED:
      Serial.println("📡 Ethernet: Cable Connected!");
      break;
    case ARDUINO_EVENT_ETH_GOT_IP:
      Serial.print("📡 Ethernet IP: ");
      Serial.println(ETH.localIP());
      drawTftDefaultScreen();
      if (weatherTaskHandle != NULL) {
        xTaskNotifyGive(weatherTaskHandle);
      }
      break;
    case ARDUINO_EVENT_ETH_DISCONNECTED:
      Serial.println("📡 Ethernet: Cable Disconnected!");
      drawTftDefaultScreen();
      break;
    case ARDUINO_EVENT_ETH_STOP:
      Serial.println("📡 Ethernet: Interface Stopped");
      break;
#else
    case SYSTEM_EVENT_ETH_START:
      Serial.println("📡 Ethernet: Interface Started");
      break;
    case SYSTEM_EVENT_ETH_CONNECTED:
      Serial.println("📡 Ethernet: Cable Connected!");
      break;
    case SYSTEM_EVENT_ETH_GOT_IP:
      Serial.print("📡 Ethernet IP: ");
      Serial.println(ETH.localIP());
      drawTftDefaultScreen();
      if (weatherTaskHandle != NULL) {
        xTaskNotifyGive(weatherTaskHandle);
      }
      break;
    case SYSTEM_EVENT_ETH_DISCONNECTED:
      Serial.println("📡 Ethernet: Cable Disconnected!");
      drawTftDefaultScreen();
      break;
    case SYSTEM_EVENT_ETH_STOP:
      Serial.println("📡 Ethernet: Interface Stopped");
      break;
#endif
    default:
      break;
    }
  });


  // Initialize Fingerprint Serial Communication
  mySerial.begin(57600, SERIAL_8N1, FP_RX, FP_TX);

  // Create Mutex for Thread-Safe Fingerprint Access
  fpMutex = xSemaphoreCreateMutex();
  if (fpMutex == NULL) {
    Serial.println("ERROR: Failed to create semaphore!");
  }

  Serial.println("🔍 [DEBUG] Mounting LittleFS...");
  // Initialize File System first
  if (!LittleFS.begin(true)) {
    Serial.println("⚠ LittleFS Mount Failed");
  } else {
    Serial.println("✓ LittleFS initialized");
    if (!LittleFS.exists("/photos")) {
      LittleFS.mkdir("/photos");
      Serial.println("📁 Created /photos directory");
    }
    updateFsSizes();

    // One-time auto-clean of any leftover ghost profiles from storage
    File root = LittleFS.open("/");
    File file = root.openNextFile();
    int cleaned = 0;
    while (file) {
      String fn = file.name();
      if (fn.endsWith("_f.txt")) {
        String c = file.readString();
        if (c.startsWith("Restored ID")) {
          String fullPath = "/" + fn;
          file.close(); // Close before deleting
          LittleFS.remove(fullPath);
          cleaned++;
          file = root.openNextFile();
          continue;
        }
      }
      file.close();
      file = root.openNextFile();
    }
    if (cleaned > 0) {
      Serial.printf(
          "✨ Automatically cleaned up %d leftover ghost files from storage.\n",
          cleaned);
    }

    // Audio setup removed
  }

  Serial.println("🔍 [DEBUG] Initializing Fingerprint Sensor...");
  // Initialize Fingerprint Sensor
  finger.begin(57600);

  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
    uint8_t paramError = finger.getParameters();
    if (paramError == FINGERPRINT_OK) {
      Serial.print("✓ Sensor parameters loaded. Capacity: ");
      Serial.println(finger.capacity);
    } else {
      Serial.printf("⚠ Failed to get sensor parameters, error: %d\n",
                    paramError);
    }
    if (finger.capacity == 0) {
      finger.capacity = 162; // Safe default fallback
      Serial.println("ℹ Using fallback capacity of 162 templates");
    }
    syncFingerprintDatabase();
  } else {
    Serial.println("Did not find fingerprint sensor :(");
  }

  // Load Configuration from Storage
  if (LittleFS.exists("/config.sys")) {
    File f = LittleFS.open("/config.sys", FILE_READ);
    if (f) {
      deviceName = f.readStringUntil('\n');
      webhookUrl = f.readStringUntil('\n');
      deviceName.trim();
      webhookUrl.trim();
      if (f.available()) {
        String pEnabled = f.readStringUntil('\n');
        pEnabled.trim();
        if (pEnabled == "0" || pEnabled == "false") {
          tftPromptEnabled = false;
        } else {
          tftPromptEnabled = true;
        }
      } else {
        tftPromptEnabled = true;
      }
      if (f.available()) {
        String tfaEnabled = f.readStringUntil('\n');
        tfaEnabled.trim();
        if (tfaEnabled == "1" || tfaEnabled == "true") {
          twoFactorEnabled = true;
        } else {
          twoFactorEnabled = false;
        }
      } else {
        twoFactorEnabled = false;
      }
      f.close();
      Serial.printf("✓ Config loaded - Device: %s, TFT Prompt: %s, 2FA: %s\n", deviceName.c_str(), tftPromptEnabled ? "Enabled" : "Disabled", twoFactorEnabled ? "Enabled" : "Disabled");
    }
  }

  // Load weather city configuration
  if (LittleFS.exists("/weather_city.txt")) {
    File f = LittleFS.open("/weather_city.txt", FILE_READ);
    if (f) {
      weatherCity = f.readString();
      weatherCity.trim();
      Serial.println("✓ Loaded weather city: " + weatherCity);
      f.close();
    }
  }

  // Load Google Maps API Key
  if (LittleFS.exists("/google_key.txt")) {
    File f = LittleFS.open("/google_key.txt", FILE_READ);
    if (f) {
      googleApiKey = f.readString();
      googleApiKey.trim();
      Serial.println("✓ Loaded Google API Key: " + (googleApiKey.length() > 0 ? googleApiKey.substring(0, 5) + "..." : "EMPTY"));
      f.close();
    }
  }
  // Load last known weather from storage
  if (LittleFS.exists("/last_weather.txt")) {
    File f = LittleFS.open("/last_weather.txt", FILE_READ);
    if (f) {
      String tStr = f.readStringUntil('\n');
      String cStr = f.readStringUntil('\n');
      String hStr = f.readStringUntil('\n');
      String wStr = f.readStringUntil('\n');
      tStr.trim();
      cStr.trim();
      hStr.trim();
      wStr.trim();
      if (tStr.length() > 0) outsideTemp = tStr.toFloat();
      if (cStr.length() > 0) weatherCond = cStr;
      if (hStr.length() > 0) weatherHum = hStr;
      if (wStr.length() > 0) weatherWind = wStr;
      Serial.printf("✓ Loaded last known weather: %.1f C | Cond: %s | Hum: %s | Wind: %s\n",
                    outsideTemp, weatherCond.c_str(), weatherHum.c_str(), weatherWind.c_str());
      f.close();
    }
  } else if (LittleFS.exists("/last_temp.txt")) {
    File f = LittleFS.open("/last_temp.txt", FILE_READ);
    if (f) {
      String tempStr = f.readString();
      tempStr.trim();
      if (tempStr.length() > 0) {
        outsideTemp = tempStr.toFloat();
        Serial.printf("✓ Loaded last known outside temperature: %.1f C\n",
                      outsideTemp);
      }
      f.close();
    }
  }



  loadShiftConfig();
  loadAuthConfig();

  // Initialize W5500 SPI Ethernet Module ONLY if a physical cable connection is detected.
  // This avoids deadlocks between WiFi and Ethernet LwIP stacks when no Ethernet cable is plugged in.
  bool connected = false;
  isAPMode = false;

  Serial.println("⏳ Checking W5500 Ethernet cable connection status...");
  bool ethCableConnected = checkW5500Link();

  if (ethCableConnected) {
    Serial.println("🔌 W5500 Ethernet cable connection DETECTED! Initializing W5500...");
#if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0)
    // Initialize W5500 with native ESP-IDF SPI host to ensure thread-safe transaction serialization on the shared bus
    if (!ETH.begin(ETH_PHY_W5500, 0, ETH_CS_PIN, ETH_INT_PIN, ETH_RST_PIN, SPI2_HOST, TFT_SCLK, TFT_MISO, TFT_MOSI, 12)) {
      Serial.println("❌ W5500 Ethernet Initialization Failed!");
      ethCableConnected = false;
    } else {
      Serial.println("✅ W5500 Ethernet Initialized Successfully!");
    }
#else
    Serial.println("ℹ️ W5500 LwIP Ethernet is natively supported on ESP32 Arduino Core v3.0.0+");
#endif
  } else {
    Serial.println("⏳ W5500 Ethernet cable NOT DETECTED. Skipping Ethernet driver initialization to prevent WiFi deadlock.");
  }

  if (ethCableConnected) {
    Serial.println("🔌 Ethernet link active. Waiting for DHCP IP...");

    int attempts = 0;
    while (attempts < 20) { // Wait up to 10 seconds for IP
      esp_task_wdt_reset(); // Pet the watchdog
      
      // Update screen with dynamic loading dot
      drawNetworkInitScreen("Ethernet Cable", "Connecting...", "DHCP Request...", TFT_WHITE, TFT_YELLOW, true, attempts);
      
      if (ETH.localIP() != IPAddress(0, 0, 0, 0)) {
        connected = true;
        break;
      }
      delay(500); // delay() automatically yields to the FreeRTOS scheduler to feed the Idle watchdog
      attempts++;
    }

    if (connected) {
      Serial.print("✓ Ethernet Connected! IP: ");
      Serial.println(ETH.localIP());
      drawNetworkInitScreen("LAN Connected!", "Connected!", ETH.localIP().toString(), TFT_GREEN, TFT_WHITE, false);

      if (weatherTaskHandle != NULL) {
        xTaskNotifyGive(weatherTaskHandle);
      }

      // Ethernet is active; disconnect WiFi STA/AP and set mode to OFF to prevent event conflicts
      WiFi.disconnect(true);
      WiFi.mode(WIFI_OFF);
      delay(2000);
    } else {
      Serial.println("❌ Ethernet DHCP failed. Falling back to WiFi/AP.");
    }
  }

  // Load WiFi Configuration & attempt connection if Ethernet is not connected
  if (!connected) {
    String ssid = "", pass = "", static_ip = "", static_gw = "", static_sn = "";

    if (LittleFS.exists("/wifi.conf")) {
      File f = LittleFS.open("/wifi.conf", FILE_READ);
      if (f) {
        ssid = f.readStringUntil('\n');
        pass = f.readStringUntil('\n');
        static_ip = f.readStringUntil('\n');
        static_gw = f.readStringUntil('\n');
        static_sn = f.readStringUntil('\n');

        ssid.trim();
        pass.trim();
        static_ip.trim();
        static_gw.trim();
        static_sn.trim();
        f.close();
      }
    }

    // Use hardcoded fallback credentials if LittleFS configuration is empty
    if (ssid.length() == 0 && String(DEFAULT_SSID).length() > 0) {
      ssid = DEFAULT_SSID;
      pass = DEFAULT_PASS;
      Serial.println("ℹ Using hardcoded fallback WiFi credentials.");
    }

    if (ssid.length() > 0) {
      WiFi.mode(WIFI_STA);
      Serial.printf("\n📡 Attempting to connect to: %s\n", ssid.c_str());

      // Configure Static IP if available
      if (static_ip.length() > 0 && static_gw.length() > 0 &&
          static_sn.length() > 0) {
        IPAddress ip, gw, sn;
        IPAddress dns(8, 8, 8, 8);

        if (ip.fromString(static_ip) && gw.fromString(static_gw) &&
            sn.fromString(static_sn)) {
          WiFi.config(ip, gw, sn, dns);
          Serial.printf("📍 Static IP: %s\n", static_ip.c_str());
        }
      }

      WiFi.begin(ssid.c_str(), pass.c_str());

      int attempts = 0;
      while (WiFi.status() != WL_CONNECTED && attempts < 40) {
        esp_task_wdt_reset(); // Pet the watchdog
        
        // Dynamic loading screen with rotating dot
        drawNetworkInitScreen("WiFi Connecting", ssid.substring(0, 16), "Connecting...", TFT_WHITE, TFT_CYAN, true, attempts);
        
        delay(500);
        Serial.print(".");
        attempts++;
      }
      Serial.println();

      if (WiFi.status() == WL_CONNECTED) {
        connected = true;
        Serial.println("\n✓ WiFi Connected!");
        Serial.print("  IP Address: ");
        Serial.println(WiFi.localIP());
        drawNetworkInitScreen("WiFi Connected!", "Connected!", WiFi.localIP().toString(), TFT_GREEN, TFT_WHITE, false);

        if (weatherTaskHandle != NULL) {
          xTaskNotifyGive(weatherTaskHandle);
        }

        delay(2000);
        
        // Start AP Mode simultaneously at boot (runs for 5 minutes)
        isAPMode = true;
        apModeStartTime = millis();
        WiFi.mode(WIFI_AP_STA);
        IPAddress apIP(192, 168, 4, 1);
        WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
        WiFi.softAP("Anurag_0.1_AP", "admin123");
        dnsServer.start(53, "*", apIP);
        Serial.println("📡 AP Mode started simultaneously at boot (active for 5 minutes)");
      } else {
        Serial.println("✗ WiFi connection failed");
        WiFi.disconnect();
      }
    }
  }

  // Start AP Mode if still not connected
  if (!connected) {
    isAPMode = true;
    apModeStartTime = millis();
    WiFi.mode(WIFI_AP);
    Serial.println("\n========================================");
    Serial.println("    WIFI CONFIGURATION MODE (AP)");
    Serial.println("========================================");

    IPAddress apIP(192, 168, 4, 1);
    WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
    WiFi.softAP("Anurag_0.1_AP", "admin123");

    dnsServer.start(53, "*", apIP);
    Serial.println("1. Connect to: Anurag_0.1_AP");
    Serial.println("   Password: admin123");
    Serial.print("2. Open: http://");
    Serial.println(WiFi.softAPIP());
    Serial.println("========================================\n");

    tft.fillScreen(0x0821); // Premium dark navy background
    
    // Header Bar
    tft.fillRect(0, 0, 320, 48, 0x018C);
    tft.drawFastHLine(0, 48, 320, 0x03EF);
    drawAutoScaledString("DEVICE CONFIG MODE", 160, 24, 4, 1.0f, 300, TFT_WHITE, 0x018C, true);

    // Main Card
    uint16_t apBg = 0x10A2;
    tft.fillRoundRect(18, 73, 290, 330, 8, 0x0410); // Shadow
    tft.fillRoundRect(15, 70, 290, 330, 8, apBg);     // Body
    tft.drawRoundRect(15, 70, 290, 330, 8, 0x028A); // Border
    tft.drawRoundRect(16, 71, 288, 328, 8, 0x03EF); // Glow

    // Section title
    drawAutoScaledString("HOTSPOT SYSTEM ACTIVE", 160, 92, 2, 1.0f, 260, TFT_ORANGE, apBg, true);
    tft.drawFastHLine(25, 108, 270, 0x028A);

    // Draw Wi-Fi Hotspot Icon in center (centered at x = 160, y = 145)
    tft.fillCircle(160, 155, 4, TFT_ORANGE); // Base dot
    tft.drawCircle(160, 155, 10, TFT_ORANGE);
    tft.drawCircle(160, 155, 16, TFT_ORANGE);
    tft.drawCircle(160, 155, 22, TFT_ORANGE);
    tft.fillRect(135, 157, 50, 25, apBg); // Mask bottom half

    // AP Details (SSID, PASS, IP)
    drawAutoScaledString("Connect to Wi-Fi Network:", 160, 185, 2, 0.9f, 260, 0x9DFD, apBg, true);
    drawAutoScaledString("Anurag_0.1_AP", 160, 208, 4, 1.0f, 260, TFT_WHITE, apBg, true);

    drawAutoScaledString("Password:", 160, 238, 2, 0.9f, 260, 0x9DFD, apBg, true);
    drawAutoScaledString("admin123", 160, 258, 4, 1.0f, 260, TFT_WHITE, apBg, true);

    drawAutoScaledString("Open Web Browser Page:", 160, 292, 2, 0.9f, 260, 0x9DFD, apBg, true);
    drawAutoScaledString("http://192.168.4.1", 160, 316, 4, 1.1f, 260, TFT_GREEN, apBg, true);
    drawAutoScaledString("(Portal automatically redirects)", 160, 342, 2, 0.8f, 260, TFT_CYAN, apBg, true);

    // Bottom Banner
    tft.fillRoundRect(18, 418, 290, 50, 6, 0x0410); // Shadow
    tft.fillRoundRect(15, 415, 290, 50, 6, 0x018C); // Body
    tft.drawRoundRect(15, 415, 290, 50, 6, 0x03EF); // Glow
    drawAutoScaledString("CONFIGURE DEVICE SETTINGS", 160, 440, 2, 1.1f, 270, TFT_WHITE, 0x018C, true);
    
    delay(2000);
  }

  // Initialize Watchdog Timer (Compatible with Arduino Core v2 and v3)
  #if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0)
  esp_task_wdt_config_t wdt_config = {
      .timeout_ms = WDT_TIMEOUT * 1000,
      .idle_core_mask = (1 << portNUM_PROCESSORS) - 1,
      .trigger_panic = true,
  };
  esp_task_wdt_reconfigure(&wdt_config);
  #else
  esp_task_wdt_init(WDT_TIMEOUT, true);
  #endif
  esp_task_wdt_add(NULL);
  Serial.println("✓ Watchdog timer initialized");

  // Configure NTP Time Synchronization
  configTime(gmtOffset_sec, 0, ntpServer);
  Serial.println("📡 NTP time sync enabled");

  // Initialize Web Server and Dashboard
  setupDashboard();
  Serial.println("✓ Web server started\n");

  // Spawn weather background task now that network configuration is complete
  xTaskCreate(weatherTask, "weather_task", 4096, NULL, 1, &weatherTaskHandle);

  // Load SQL sync config and stats
  if (LittleFS.exists("/sql_config.txt")) {
    File f = LittleFS.open("/sql_config.txt", FILE_READ);
    if (f) {
      sqlApiUrl = f.readString();
      sqlApiUrl.trim();
      f.close();
    }
  }
  if (LittleFS.exists("/sql_transferred.txt")) {
    File f = LittleFS.open("/sql_transferred.txt", FILE_READ);
    if (f) {
      String countStr = f.readString();
      countStr.trim();
      if (countStr.length() > 0) sqlTransferredLogs = countStr.toInt();
      f.close();
    }
  }
  // Recalculate pending count by counting lines in /pending_sync.txt
  sqlPendingLogs = 0;
  if (LittleFS.exists("/pending_sync.txt")) {
    File f = LittleFS.open("/pending_sync.txt", FILE_READ);
    if (f) {
      while (f.available()) {
        String line = f.readStringUntil('\n');
        line.trim();
        if (line.length() > 0) {
          sqlPendingLogs++;
        }
      }
      f.close();
    }
  }
  sqlTotalLogs = sqlTransferredLogs + sqlPendingLogs;

  // Spawn SQL sync background task
  xTaskCreate([](void *p) {
    while (true) {
      extern void performSqlSync();
      extern String sqlApiUrl;
      if (sqlApiUrl.length() > 10 && WiFi.status() == WL_CONNECTED && LittleFS.exists("/pending_sync.txt")) {
        performSqlSync();
      }
      vTaskDelay(pdMS_TO_TICKS(10000)); // check every 10 seconds
    }
  }, "sql_sync_task", 4096, NULL, 1, NULL);

  drawTftDefaultScreen();
}

void handleEnrollment() {
  if (!isEnrollMode)
    return;

  static unsigned long lastEnrollCheck = 0;
  if (millis() - lastEnrollCheck < 150) {
    return;
  }
  lastEnrollCheck = millis();

  if (!wasEnrollActive) {
    wasEnrollActive = true;
    currentTftState = TFT_ENROLL_SCANNING;
    if (enrollType == "FINGER") {
      drawTftEnrollScanningScreen("FINGER", enrollID, 1, "Place finger on sensor");
    } else {
      drawTftEnrollScanningScreen("RFID", -1, 0, "Place card on reader");
    }
  }

  if (enrollType == "RFID") {
    bool cardPresent = false;
    if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
      cardPresent = true;
    }

    if (cardPresent) {
      String uid = "";
      for (byte i = 0; i < rfid.uid.size; i++) {
        uid += String(rfid.uid.uidByte[i] < 0x10 ? "0" : "");
        uid += String(rfid.uid.uidByte[i], HEX);
      }
      uid.toUpperCase();
      if (LittleFS.exists("/" + uid + ".txt") ||
          LittleFS.exists("/" + uid + "_f.txt")) {
        ws.textAll("{\"type\":\"ENROLL_ST\",\"status\":\"FAIL\",\"msg\":\"Card "
                   "already enrolled!\"}");
        isEnrollMode = false;
        lastScanTime = millis();
        triggerDeniedLed();
        
        currentTftState = TFT_PUNCH_DENIED;
        drawTftDeniedScreen("Card Enroll", "Already Enrolled");
        tftMessageUntil = millis() + 3000;
      } else {
        // Successfully scanned backup RFID card! Link it to current fingerprint session
        if (enrollID != -1) {
          ws.textAll("{\"type\":\"ENROLL_ST\",\"status\":\"SUCCESS\",\"uid\":\"" +
                     String(enrollID) + "\",\"rfid\":\"" + uid +
                     "\",\"msg\":\"Fingerprint & Card captured successfully!\"}");
          drawTftEnrollScanningScreen("RFID", enrollID, 0, "Captured! Save in browser.");
        } else {
          ws.textAll("{\"type\":\"ENROLL_ST\",\"status\":\"SUCCESS\",\"uid\":\"" +
                     uid + "\",\"rfid\":\"" + uid +
                     "\",\"msg\":\"RFID Card captured successfully!\"}");
          drawTftEnrollScanningScreen("RFID", -1, 0, "Card Captured! Save in browser.");
        }
        isEnrollMode = false;
        wasEnrollActive = false; // Prevent automatic loop reset to idle
        lastScanTime = millis();
        triggerBuzzer(5000);

        // Turn off R503 sensor LED
        if (xSemaphoreTake(fpMutex, pdMS_TO_TICKS(500))) {
          finger.LEDcontrol(4, 0, 1);
          xSemaphoreGive(fpMutex);
        }
      }
      rfid.PICC_HaltA();
      rfid.PCD_StopCrypto1();
    }
  } else if (enrollType == "FINGER") {
    if (xSemaphoreTake(fpMutex, pdMS_TO_TICKS(100))) {
      if (enrollStep == 1) {
        int p = finger.getImage();
        if (p == FINGERPRINT_OK) {
          if (finger.image2Tz(1) == FINGERPRINT_OK) {
            if (finger.fingerSearch() == FINGERPRINT_OK) {
              ws.textAll("{\"type\":\"ENROLL_ST\",\"status\":\"FAIL\",\"msg\":"
                         "\"Finger already exists as ID " +
                         String(finger.fingerID) + "!\"}");
              isEnrollMode = false;
              triggerDeniedLed();
              
              currentTftState = TFT_PUNCH_DENIED;
              drawTftDeniedScreen("Finger Enroll", "Already Exists");
              tftMessageUntil = millis() + 3000;
            } else {
              enrollStep = 2;
              ws.textAll(
                  "{\"type\":\"ENROLL_ST\",\"step\":2,\"msg\":\"1st Scan "
                  "OK. Remove finger.\"}");
              finger.LEDcontrol(2, 25, 3); // Flashing Purple
              triggerBuzzer(100);
              drawTftEnrollScanningScreen("FINGER", enrollID, 2, "1st Scan OK. Remove finger.");
            }
          } else {
            ws.textAll("{\"type\":\"ENROLL_ST\",\"msg\":\"Image too messy, try "
                       "again.\"}");
            drawTftEnrollScanningScreen("FINGER", enrollID, 1, "Messy print. Try again.");
          }
        }
      } else if (enrollStep == 2) {
        if (finger.getImage() == FINGERPRINT_NOFINGER) {
          enrollStep = 3;
          ws.textAll("{\"type\":\"ENROLL_ST\",\"step\":3,\"msg\":\"Place SAME "
                     "finger again.\"}");
          finger.LEDcontrol(1, 100, 2); // Breathing Blue
          triggerBuzzer(100);
          drawTftEnrollScanningScreen("FINGER", enrollID, 3, "Place finger again.");
        }
      } else if (enrollStep == 3) {
        int p = finger.getImage();
        if (p == FINGERPRINT_OK) {
          if (finger.image2Tz(2) == FINGERPRINT_OK) {
            if (finger.createModel() == FINGERPRINT_OK) {
              if (finger.storeModel(enrollID) == FINGERPRINT_OK) {
                fpUsed[enrollID] = true;
                saveFpCache();

                // Fingerprint successfully enrolled! Now transition to RFID linking stage
                enrollType = "RFID";
                enrollStep = 0; // Reset steps for RFID stage
                enrollTimer = millis(); // Reset timer for card scan (gives another 45s)

                ws.textAll(
                    "{\"type\":\"ENROLL_ST\",\"status\":\"FP_OK\",\"uid\":\"" +
                    String(enrollID) +
                    "\",\"msg\":\"Fingerprint Captured! Scan backup RFID card "
                    "next.\"}");

                // Guide user to scan card next
                triggerBuzzer(200);
                drawTftEnrollScanningScreen("RFID", enrollID, 0, "FP Ok. Scan backup card.");
              } else {
                ws.textAll("{\"type\":\"ENROLL_ST\",\"status\":\"FAIL\","
                           "\"msg\":\"Sensor memory full or error.\"}");
                isEnrollMode = false;
                lastScanTime = millis();
                triggerDeniedLed();
                
                currentTftState = TFT_PUNCH_DENIED;
                drawTftDeniedScreen("Finger Enroll", "Memory full / error");
                tftMessageUntil = millis() + 3000;
              }
            } else {
              ws.textAll("{\"type\":\"ENROLL_ST\",\"status\":\"FAIL\",\"msg\":"
                         "\"Fingerprints did not match.\"}");
              isEnrollMode = false;
              lastScanTime = millis();
              finger.LEDcontrol(3, 0, 1);
              triggerDeniedLed();
              
              currentTftState = TFT_PUNCH_DENIED;
              drawTftDeniedScreen("Finger Enroll", "Prints did not match");
              tftMessageUntil = millis() + 3000;
            }
          } else {
            ws.textAll("{\"type\":\"ENROLL_ST\",\"msg\":\"2nd Scan messy, try "
                       "again.\"}");
            drawTftEnrollScanningScreen("FINGER", enrollID, 3, "2nd Scan messy. Try again.");
          }
        }
      }
      xSemaphoreGive(fpMutex);
    }
  }

  if (millis() - enrollTimer > 45000) { // Increased to 45s
    ws.textAll("{\"type\":\"ENROLL_ST\",\"status\":\"FAIL\",\"msg\":"
               "\"Enrollment timed out.\"}");
    isEnrollMode = false;
    lastScanTime = millis();
    if (xSemaphoreTake(fpMutex, pdMS_TO_TICKS(500))) {
      finger.LEDcontrol(4, 0, 1);
      xSemaphoreGive(fpMutex);
    }
    triggerDeniedLed();
    
    currentTftState = TFT_PUNCH_DENIED;
    drawTftDeniedScreen("Enrollment", "Timed Out");
    tftMessageUntil = millis() + 3000;
  }
}

void loop() {
  // Pet the watchdog
  esp_task_wdt_reset();

  // Monitor TFT board power status using the T_IRQ (Touch Interrupt) pin.
  // When the TFT board is powered off, T_IRQ is pulled LOW. When powered on (and not touched), it reads HIGH.
  static unsigned long lastPowerCheck = 0;
  static unsigned long lowStartTime = 0;
  
  if (millis() - lastPowerCheck > 500) {
    lastPowerCheck = millis();
    int irqState = digitalRead(T_IRQ);
    
    if (irqState == LOW) {
      if (lowStartTime == 0) {
        lowStartTime = millis();
      } else if (millis() - lowStartTime > 3000) {
        // Consistently LOW for 3 seconds indicates the TFT board has lost power.
        // We configure display pins as INPUT to prevent parasitic power leakage.
        if (!tftWasPowerCycled) {
          tftWasPowerCycled = true;
          Serial.println("🔌 TFT board power loss detected! Reconfiguring pins to INPUT to prevent parasitic power...");
          
          pinMode(TFT_CS, INPUT);
          pinMode(T_CS, INPUT);
          pinMode(TFT_RS, INPUT);
          pinMode(TFT_RST, INPUT);
          pinMode(TFT_BL, INPUT);
        }
      }
    } else {
      // T_IRQ is HIGH (TFT board is powered and not touched)
      if (tftWasPowerCycled) {
        Serial.println("🔌 TFT board power restore detected! Restoring pins and re-initializing display registers...");
        
        // Restore pin configurations
        pinMode(TFT_CS, OUTPUT);
        digitalWrite(TFT_CS, HIGH);
        pinMode(T_CS, OUTPUT);
        digitalWrite(T_CS, HIGH);
        pinMode(TFT_BL, OUTPUT);
        digitalWrite(TFT_BL, LOW); // Keep backlight off initially
        pinMode(TFT_RS, OUTPUT);
        pinMode(TFT_RST, OUTPUT);
        
        // Let the TFT board's voltage stabilize
        delay(200);
        
        // Perform clean hardware reset sequence on the TFT display
        digitalWrite(TFT_RST, LOW);
        delay(100);
        digitalWrite(TFT_RST, HIGH);
        delay(150);
        
        // Re-initialize the display registers in LovyanGFX
        tft.init();
        tft.setRotation(4);
        
        tftWasPowerCycled = false; // Must set false BEFORE drawing so early returns don't trigger
        
        // Redraw the current active screen buffer
        if (currentTftState == TFT_IDLE) {
          drawTftDefaultScreen();
        } else if (currentTftState == TFT_DIRECTION_SELECT) {
          drawTftDirectionSelectionScreen(pendingPunchName, pendingPunchRoll);
        } else if (currentTftState == TFT_WAITING_2FA_FINGER) {
          drawTft2FAPromptScreen(pending2FAName, pending2FARoll);
        } else if (currentTftState == TFT_ENROLL_SCANNING) {
          String tempMsg = "Place card/finger to continue";
          if (enrollType == "FINGER") {
            if (enrollStep == 1) tempMsg = "Place finger on sensor";
            else if (enrollStep == 2) tempMsg = "Remove finger from sensor";
            else if (enrollStep == 3) tempMsg = "Place same finger again";
          } else {
            tempMsg = "Place card on reader";
          }
          drawTftEnrollScanningScreen(enrollType, enrollID, enrollStep, tempMsg, true);
        } else if (currentTftState == TFT_ENROLL_CONFIRMED) {
          drawTftEnrollConfirmedScreen("Employee", "", "", enrollType);
        } else {
          drawTftDefaultScreen();
        }
        
        // Turn backlight pin back HIGH now that the screen is drawn
        digitalWrite(TFT_BL, HIGH);
        
        Serial.println("✓ Display successfully re-initialized after power cycle!");
      }
      lowStartTime = 0;
    }
  }

  // Check and fallback to AP Mode if WiFi connection is lost
  static unsigned long lastNetCheck = 0;
  if (millis() - lastNetCheck > 2000) {
    lastNetCheck = millis();

    bool ethActive = false;
#if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0)
    ethActive = (ETH.handle() != NULL);
#endif

    if (!ethActive) {
      // If we are not connected to WiFi STA and AP mode is not active, start AP Mode
      if (WiFi.status() != WL_CONNECTED && !isAPMode) {
        Serial.println("📡 No active connection detected. Starting AP Mode...");
        isAPMode = true;
        apModeStartTime = millis();
        WiFi.mode(WIFI_AP);
        IPAddress apIP(192, 168, 4, 1);
        WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
        WiFi.softAP("Anurag_0.1_AP", "admin123");
        dnsServer.start(53, "*", apIP);
        drawTftDefaultScreen();
      }
    }
  }

  static bool otaActive = false;
  if (isOtaUpdating) {
    if (!otaActive) {
      otaActive = true;
      disableWatchdog();
    }

    // Safety timeout: if no chunk received for 15 seconds, abort OTA
    if (millis() - lastOtaChunkTime > 15000) {
      Serial.println("⚠ OTA Update Timed Out: No data received for 15 seconds. "
                     "Aborting...");
      Update.abort();
      isOtaUpdating = false;
    }

    delay(200);
    return;
  } else {
    if (otaActive) {
      otaActive = false;
      enableWatchdog();
    }
  }

  // Non-blocking TFT state machine
  unsigned long nowMs = millis();
  if (currentTftState == TFT_DIRECTION_SELECT) {
    // Check timeout for pending selection screen
    if (hasPendingPunch && nowMs > pendingPunchTimeout) {
      hasPendingPunch = false;
      currentTftState = TFT_IDLE;
      drawTftDefaultScreen();
    } else if (hasPendingPunch) {
      // Read touch screen interaction with noise filtering and debouncing
      int32_t tx = 0, ty = 0;
      if (tft.getTouch(&tx, &ty)) {
        // Debounce: verify touch stability and filter coordinates over 3 samples (12ms total)
        int32_t sumX = tx;
        int32_t sumY = ty;
        int32_t validCount = 1;
        
        for (int i = 0; i < 3; i++) {
          delay(4); // 4ms sampling interval
          int32_t cx = 0, cy = 0;
          if (tft.getTouch(&cx, &cy)) {
            // Keep only coordinates close to the initial touch to filter out wild jumps/release spikes
            if (abs(cx - tx) < 35 && abs(cy - ty) < 35) {
              sumX += cx;
              sumY += cy;
              validCount++;
            }
          }
        }
        
        // We only process if we got at least 2 matching stable samples
        if (validCount >= 2) {
          tx = sumX / validCount;
          ty = sumY / validCount;
          
          Serial.printf("🎯 STABLE TOUCH DETECTED! Averaged coordinates: x = %d, y = %d\n", tx, ty);
          
          bool isClickedIn = false;
          bool isClickedOut = false;
          
          // --- Calibrated touch detection for the display offset ---
          // Top button (PUNCH IN): reports ty in 195..300
          // Bottom button (PUNCH OUT): reports ty in 90..190
          if (tx >= 20 && tx <= 300) {
            if (ty >= 195 && ty <= 300) {
              isClickedIn = true;
            } else if (ty >= 90 && ty <= 190) {
              isClickedOut = true;
            }
          }
          
          // Process final decision with visual button highlight feedback
          if (isClickedIn) {
            // Visual feedback: redraw IN button in bright green highlighted state
            tft.fillRoundRect(40, 220, 240, 76, 12, 0x07E0); // Lighter bright green
            tft.drawRoundRect(40, 220, 240, 76, 12, TFT_WHITE);
            drawAutoScaledString("PUNCH IN  >>", 175, 245, 4, 1.3f, 160, TFT_WHITE, 0x07E0, true);
            drawAutoScaledString("(Start Shift)", 175, 275, 2, 1.0f, 160, TFT_WHITE, 0x07E0, true);
            delay(80); // Linger for feedback
            
            hasPendingPunch = false;
            savePunchRecord(pendingPunchId, pendingPunchIsFinger, "In");
          } else if (isClickedOut) {
            // Visual feedback: redraw OUT button in bright red highlighted state
            tft.fillRoundRect(40, 310, 240, 76, 12, 0xF800); // Lighter bright red
            tft.drawRoundRect(40, 310, 240, 76, 12, TFT_WHITE);
            drawAutoScaledString("<<  PUNCH OUT", 175, 335, 4, 1.3f, 160, TFT_WHITE, 0xF800, true);
            drawAutoScaledString("(End Shift)", 175, 365, 2, 1.0f, 160, TFT_WHITE, 0xF800, true);
            delay(80); // Linger for feedback
            
            hasPendingPunch = false;
            savePunchRecord(pendingPunchId, pendingPunchIsFinger, "Out");
          } else {
            // If they click too far top or bottom, or somewhere else, treat as cancel
            hasPendingPunch = false;
            currentTftState = TFT_IDLE;
            drawTftDefaultScreen();
            delay(100); // Debounce touch
          }
        }
      }
    }
  } else if (currentTftState == TFT_WAITING_2FA_FINGER) {
    if (nowMs > pending2FATimeout) {
      pending2FAFingerId = "";
      pending2FACardUid = "";
      pending2FATimeout = 0;
      currentTftState = TFT_IDLE;
      drawTftDefaultScreen();
    } else {
      // Dynamic countdown update on TFT screen
      static unsigned long lastCountdownUpdate = 0;
      int secLeft = (int)((pending2FATimeout - nowMs) / 1000) + 1;
      if (secLeft < 0) secLeft = 0;
      if (secLeft > 10) secLeft = 10;
      if (nowMs - lastCountdownUpdate >= 200) {
        lastCountdownUpdate = nowMs;
        String countStr = "Timeout in " + String(secLeft) + "s ";
        drawAutoScaledString(countStr, 160, 438, 2, 1.0f, 270, TFT_RED, TFT_BLACK, true);
      }
    }
  } else if (nowMs < tftMessageUntil) {
    // Keep showing high-priority punch success/denied screen
  } else {
    // If punch screen just expired, return to standard idle display
    if (currentTftState != TFT_IDLE && currentTftState != TFT_ENROLL_SCANNING) {
      currentTftState = TFT_IDLE;
      drawTftDefaultScreen();
    }

    // Periodically update the time/date/temp on the idle screen (every 1000ms)
    static unsigned long lastTftClockUpdate = 0;
    if (currentTftState == TFT_IDLE && nowMs - lastTftClockUpdate >= 1000) {
      lastTftClockUpdate = nowMs;
      updateTftClock();
    }
  }

  // Clean up disconnected WebSocket clients
  ws.cleanupClients();

  // Handle AP Mode Timeout (5 minutes)
  if (isAPMode && apModeStartTime > 0 && (millis() - apModeStartTime >= 300000)) {
    Serial.println("⏰ AP Mode 5-minute timer expired! Shutting down AP Mode...");
    WiFi.softAPdisconnect(true);
    dnsServer.stop();
    isAPMode = false;
    apModeStartTime = 0;
    
    // Switch back to STA mode
    WiFi.mode(WIFI_STA);
    if (WiFi.status() != WL_CONNECTED) {
      WiFi.begin();
    }
    
    drawTftDefaultScreen();
  }

  // Handle DNS requests in AP mode
  if (isAPMode) {
    dnsServer.processNextRequest();
  }

  // Auto-clean logs every 1 hour
  static unsigned long lastCleanLog = 0;
  if (millis() - lastCleanLog > 3600000) {
    autoCleanLogs();
    lastCleanLog = millis();
  }

  // Log heap memory every 10 seconds
  static unsigned long lastHeapLog = 0;
  if (millis() - lastHeapLog > 10000) {
    Serial.printf("📊 Heap: %d bytes | Uptime: %lu s\n", ESP.getFreeHeap(),
                  millis() / 1000);
    lastHeapLog = millis();
  }

  // Handle reboot request
  if (restartTime > 0 && millis() > restartTime) {
    Serial.println("\n🔄 Rebooting device...");
    ESP.restart();
  }

  // Turn off LEDs after activity
  if (millis() - lastScanTime > 1500) {
    if (!manualLedOn) {
      digitalWrite(LED_PIN, LOW);
      controlOnboardLed(false); // Turn off onboard LED
    }
  }

  // Enrollment or Scanning Mode
  if (!isEnrollMode) {
    if (wasEnrollActive) {
      wasEnrollActive = false;
      if (currentTftState == TFT_ENROLL_SCANNING) {
        currentTftState = TFT_IDLE;
        drawTftDefaultScreen();
      }
    }
    // RFID Scanning: Rate-limited to every 60ms to prevent SPI bus/CPU contention
    static unsigned long lastRfidCheck = 0;
    bool cardPresent = false;
    if (nowMs - lastRfidCheck >= 60) {
      lastRfidCheck = nowMs;
      if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
        cardPresent = true;
      }
    }

    if (cardPresent) {
      String uid = "";
      for (byte i = 0; i < rfid.uid.size; i++) {
        uid += String(rfid.uid.uidByte[i] < 0x10 ? "0" : "");
        uid += String(rfid.uid.uidByte[i], HEX);
      }
      uid.toUpperCase();
      rfid.PICC_HaltA();
      rfid.PCD_StopCrypto1();

      processAccess(uid, false);
    }

    // Fingerprint Scanning
    static unsigned long lastFpCheck = 0;
    if (millis() - lastScanTime > 750 && millis() - lastFpCheck >= 80) {
      lastFpCheck = millis();
      if (xSemaphoreTake(fpMutex, pdMS_TO_TICKS(200))) {
        // Clear serial buffer of any stray data
        int cleanCount = 0;
        while (mySerial.available() && cleanCount < 64) {
          mySerial.read();
          cleanCount++;
        }

        uint8_t p = finger.getImage();
        if (p == FINGERPRINT_OK) {
          Serial.println("👆 Finger detected, processing...");

          bool matched = false;
          uint8_t searchStatus = FINGERPRINT_NOTFOUND;

          for (int attempt = 0; attempt < 3; attempt++) {
            if (attempt > 0) {
              vTaskDelay(pdMS_TO_TICKS(150)); // Wait for finger to settle
              int cleanCount2 = 0;
              while (mySerial.available() && cleanCount2 < 64) {
                mySerial.read();
                cleanCount2++;
              }
              p = finger.getImage();
              if (p != FINGERPRINT_OK) {
                searchStatus = p;
                continue;
              }
            }

            int cleanCount3 = 0;
            while (mySerial.available() && cleanCount3 < 64) {
              mySerial.read();
              cleanCount3++;
            }
            p = finger.image2Tz(1);
            if (p != FINGERPRINT_OK) {
              searchStatus = p;
              continue;
            }

            int cleanCount4 = 0;
            while (mySerial.available() && cleanCount4 < 64) {
              mySerial.read();
              cleanCount4++;
            }
            p = finger.fingerSearch();
            searchStatus = p;

            if (p == FINGERPRINT_OK) {
              matched = true;
              break;
            } else if (p == FINGERPRINT_NOTFOUND) {
              // Unregistered finger: fail immediately to avoid false "Scan
              // Error 2" timeout logs
              matched = false;
              break;
            }
          }

          if (matched) {
            Serial.printf("✓ Match! ID: %d, Confidence: %d\n", finger.fingerID,
                          finger.confidence);
            processAccess(String(finger.fingerID), true);
          } else if (searchStatus == FINGERPRINT_NOTFOUND) {
            Serial.println("[Denied] Fingerprint not found in database.");
            Serial.println("[Denied] Triggering denied alert LEDs...");
            triggerDeniedLed();
            Serial.println("[Denied] Broadcasting Web Socket event...");
            ws.textAll("{\"type\":\"SCAN\",\"uid\":\"No "
                       "Match\",\"status\":\"Denied\"}");
            lastScanTime = millis();

            Serial.println("[Denied] Updating TFT display...");
            currentTftState = TFT_PUNCH_DENIED;
            drawTftDeniedScreen("Unknown Finger", "ACCESS DENIED");
            tftMessageUntil = millis() + 1500;

            Serial.println("[Denied] Buzzer alert triggered.");
            Serial.println("[Denied] Denied processing loop complete.");
          } else {
            Serial.printf("⚠ Search/Image error: %d\n", searchStatus);
            ws.textAll("{\"type\":\"SCAN\",\"uid\":\"Scan Error " +
                       String(searchStatus) + "\",\"status\":\"Error\"}");
            lastScanTime = millis();

            currentTftState = TFT_PUNCH_DENIED;
            drawTftDeniedScreen("Scan Error", "Code: " + String(searchStatus));
            tftMessageUntil = millis() + 1500;
          }
        }

        xSemaphoreGive(fpMutex);
      }
    }

    vTaskDelay(pdMS_TO_TICKS(2)); // Small scheduler yield to prevent CPU starvation while keeping loop extremely fast
  } else {
    // Handle fingerprint enrollment
    handleEnrollment();
    vTaskDelay(pdMS_TO_TICKS(10)); // Yield to prevent CPU starvation in enrollment mode
  }
}

void queueSqlSync(String id, String name, String role, String dir, String status, String type) {
  File fsync = LittleFS.open("/pending_sync.txt", FILE_APPEND);
  if (!fsync) fsync = LittleFS.open("/pending_sync.txt", FILE_WRITE);
  if (fsync) {
    time_t nowTime;
    time(&nowTime);
    fsync.println(String(nowTime) + "|" + id + "|" + name + "|" + role + "|" + dir + "|" + status + "|" + type);
    fsync.close();
    sqlPendingLogs++;
    sqlTotalLogs = sqlTransferredLogs + sqlPendingLogs;
    ws.textAll("{\"type\":\"SQL_SYNC\",\"transferred\":" + String(sqlTransferredLogs) + 
              ",\"pending\":" + String(sqlPendingLogs) + "}");
  }
}

void performSqlSync() {
  if (sqlApiUrl.length() < 10) return;
  if (WiFi.status() != WL_CONNECTED) return;
  if (!LittleFS.exists("/pending_sync.txt")) return;

  File f = LittleFS.open("/pending_sync.txt", FILE_READ);
  if (!f) return;

  // Read up to 20 lines to sync in this batch
  String lines[20];
  int count = 0;
  while (f.available() && count < 20) {
    lines[count] = f.readStringUntil('\n');
    lines[count].trim();
    if (lines[count].length() > 0) {
      count++;
    }
  }
  
  // Keep track of remaining lines in the file
  String remaining = "";
  while (f.available()) {
    remaining += f.readStringUntil('\n') + "\n";
  }
  f.close();

  if (count == 0) {
    LittleFS.remove("/pending_sync.txt");
    return;
  }

  int successCount = 0;
  for (int i = 0; i < count; i++) {
    int pipes[6];
    int pIdx = 0;
    int lastPos = -1;
    for (int j = 0; j < 6; j++) {
      pipes[j] = lines[i].indexOf('|', lastPos + 1);
      if (pipes[j] != -1) {
        lastPos = pipes[j];
        pIdx++;
      } else {
        pipes[j] = -1;
      }
    }

    if (pIdx >= 5) {
      String timestamp = lines[i].substring(0, pipes[0]);
      String id = lines[i].substring(pipes[0] + 1, pipes[1]);
      String name = lines[i].substring(pipes[1] + 1, pipes[2]);
      String role = lines[i].substring(pipes[2] + 1, pipes[3]);
      String direction = lines[i].substring(pipes[3] + 1, pipes[4]);
      String status = lines[i].substring(pipes[4] + 1, pipes[5]);
      String type = lines[i].substring(pipes[5] + 1);

      String json = "{";
      json += "\"timestamp\":" + timestamp + ",";
      json += "\"user_id\":\"" + id + "\",";
      json += "\"name\":\"" + name + "\",";
      json += "\"role\":\"" + role + "\",";
      json += "\"direction\":\"" + direction + "\",";
      json += "\"status\":\"" + status + "\",";
      json += "\"type\":\"" + type + "\"";
      json += "}";

      HTTPClient http;
      String targetEndpoint = sqlApiUrl;
      int protoIdx = targetEndpoint.indexOf("//");
      int slashIdx = targetEndpoint.indexOf('/', (protoIdx != -1) ? protoIdx + 2 : 0);
      if (slashIdx == -1) {
        targetEndpoint += "/add-log";
      }
      Serial.printf("🌐 [SQL SYNC] Sending log %d/%d to %s...\n", i+1, count, targetEndpoint.c_str());
      http.begin(targetEndpoint);
      http.addHeader("Content-Type", "application/json");
      http.setTimeout(5000);
      int code = http.POST(json);
      http.end();

      Serial.printf("📡 [SQL SYNC] HTTP Response Code: %d\n", code);

      if (code == 200) {
        successCount++;
      } else {
        Serial.printf("❌ [SQL SYNC] Sync failed for record %s (HTTP Code: %d)\n", id.c_str(), code);
        for (int k = i; k < count; k++) {
          remaining = lines[k] + "\n" + remaining;
        }
        break;
      }
    } else {
      successCount++;
    }
  }

  remaining.trim();
  if (remaining.length() > 0) {
    File fw = LittleFS.open("/pending_sync.txt", FILE_WRITE);
    if (fw) {
      fw.println(remaining);
      fw.close();
    }
  } else {
    LittleFS.remove("/pending_sync.txt");
  }

  if (successCount > 0) {
    sqlTransferredLogs += successCount;
    if (sqlPendingLogs >= successCount) {
      sqlPendingLogs -= successCount;
    } else {
      sqlPendingLogs = 0;
    }
    sqlTotalLogs = sqlTransferredLogs + sqlPendingLogs;
    
    File fs = LittleFS.open("/sql_transferred.txt", FILE_WRITE);
    if (fs) {
      fs.print(sqlTransferredLogs);
      fs.close();
    }
    
    ws.textAll("{\"type\":\"SQL_SYNC\",\"transferred\":" + String(sqlTransferredLogs) + 
              ",\"pending\":" + String(sqlPendingLogs) + "}");
  }
}
