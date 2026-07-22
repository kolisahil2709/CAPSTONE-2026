#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <LittleFS.h>

extern void lookupUserProfile(const String& uid, String& name, String& role, String& roll);

extern size_t fsTotalBytes;
extern size_t fsUsedBytes;
extern void updateFsSizes();

#define MAX_FP 200
extern void syncFingerprintDatabase();
extern bool fpUsed[];
extern bool isEnrollMode;
extern bool wasEnrollActive;
extern TftState currentTftState;
extern unsigned long tftMessageUntil;
extern void drawTftEnrollScanningScreen(String type, int id, int step, String msg, bool forceRedraw);
extern void drawTftEnrollConfirmedScreen(String name, String roll, String role, String type);
extern void drawTftDefaultScreen();
extern String enrollType;
extern int enrollID;
extern int enrollStep;
extern unsigned long enrollTimer;
extern unsigned long lastScanTime;
extern String weatherCity;
extern float outsideTemp;
extern TaskHandle_t weatherTaskHandle;
extern String deviceName;
extern String googleApiKey;
extern String webhookUrl;
extern String sqlApiUrl;
extern uint32_t sqlTotalLogs;
extern uint32_t sqlTransferredLogs;
extern uint32_t sqlPendingLogs;
extern bool tftPromptEnabled;
extern bool twoFactorEnabled;
extern bool isAPMode;
extern unsigned long apModeStartTime;
#include <DNSServer.h>
extern DNSServer dnsServer;
extern void onBrandingUpdated();
extern unsigned long restartTime;
extern bool manualLedOn;
extern String getTodayDate();
extern String getNow();
extern String getTodayFileName();
extern void sendWebhook(String json);
extern void triggerBuzzer(int durationMs);
extern void controlOnboardLed(bool on, uint8_t r, uint8_t g, uint8_t b);
#include <ETH.h>
#include <esp_ota_ops.h>
#include "eeprom_logger.h"
extern String getActiveIPAddress();
extern volatile bool isOtaUpdating;
#ifndef I2S_BCLK
#define I2S_BCLK 6
#endif
#ifndef I2S_PORT
#define I2S_PORT I2S_NUM_0
#endif

extern int shiftStartHour;
extern int shiftStartMin;
extern int shiftEndHour;
extern int shiftEndMin;
extern int graceMins;
extern void saveShiftConfig();

bool isAuth(AsyncWebServerRequest *request) {
  if (request->hasHeader("Cookie")) {
    String cookie = request->header("Cookie");
    if (cookie.indexOf("session=admin_active") != -1) {
      return true;
    }
  }
  return false;
}

void setupDashboard() {
  // Authentication Routes
  server.on("/api/login", HTTP_POST, [](AsyncWebServerRequest *request) {
    String username = "";
    String password = "";
    
    if (request->hasParam("username", true)) {
      username = request->getParam("username", true)->value();
    }
    if (request->hasParam("password", true)) {
      password = request->getParam("password", true)->value();
    }
    
    if (username.length() == 0 && request->hasParam("username")) {
      username = request->getParam("username")->value();
    }
    if (password.length() == 0 && request->hasParam("password")) {
      password = request->getParam("password")->value();
    }

    extern char loginUser[32];
    extern char loginPass[32];
    
    if (username == String(loginUser) && password == String(loginPass)) {
      AsyncWebServerResponse *response = request->beginResponse(200, "application/json", "{\"status\":\"success\"}");
      response->addHeader("Set-Cookie", "session=admin_active; Path=/; HttpOnly");
      request->send(response);
    } else {
      request->send(401, "application/json", "{\"status\":\"error\",\"message\":\"Invalid username or password\"}");
    }
  });

  server.on("/api/logout", HTTP_GET, [](AsyncWebServerRequest *request) {
    AsyncWebServerResponse *response = request->beginResponse(200, "application/json", "{\"status\":\"success\"}");
    response->addHeader("Set-Cookie", "session=; Path=/; Expires=Thu, 01 Jan 1970 00:00:00 UTC; HttpOnly");
    request->send(response);
  });

  server.on("/api/set-credentials", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (!isAPMode) return request->send(403, "text/plain", "Forbidden: AP Mode Required");
    
    if (request->hasParam("username") && request->hasParam("password")) {
      String newU = request->getParam("username")->value();
      String newP = request->getParam("password")->value();
      newU.trim();
      newP.trim();
      
      if (newU.length() > 0 && newP.length() > 0) {
        extern char loginUser[32];
        extern char loginPass[32];
        
        strncpy(loginUser, newU.c_str(), sizeof(loginUser) - 1);
        loginUser[sizeof(loginUser) - 1] = '\0';
        
        strncpy(loginPass, newP.c_str(), sizeof(loginPass) - 1);
        loginPass[sizeof(loginPass) - 1] = '\0';
        
        extern void saveAuthConfig();
        saveAuthConfig();
        
        request->send(200, "text/plain", "OK");
        return;
      }
    }
    request->send(400, "text/plain", "Invalid Parameters");
  });

  // All your existing routes
  server.on("/list-users", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (!isAuth(request)) {
      return request->send(403, "text/plain", "Unauthorized");
    }
    AsyncResponseStream *response =
        request->beginResponseStream("application/json");
    response->print("[");
    File root = LittleFS.open("/");
    File file = root.openNextFile();
    bool first = true;
    String todayDateStr = getTodayDate();
    
    while (file) {
      String fn = file.name();
      if (fn.startsWith("/")) fn = fn.substring(1); // Remove leading slash
      
      // Only process .txt files that are not system files
      if (fn.endsWith(".txt") && fn != "wifi.txt" && fn != "wifi.conf" && fn != "config.sys" &&
          fn != "sql_config.txt" && fn != "sql_transferred.txt" && fn != "pending_sync.txt" &&
          fn != "weather_city.txt" && !fn.startsWith("logs_")) {
        String c = file.readString();
        if (c.startsWith("LINK|")) {
          file.close();
          file = root.openNextFile();
          continue;
        }
        
        // Parsing logic: find up to 8 pipe positions (9 fields)
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
          if (!first) response->print(",");
          first = false;
          
          String uid_clean = fn.substring(0, fn.length() - 4);
          String name = c.substring(0, pipes[0]);
          String role = (pCount >= 2) ? c.substring(pipes[0] + 1, pipes[1]) : "-";
          String roll = (pCount >= 3) ? c.substring(pipes[1] + 1, pipes[2]) : uid_clean;
          String state = (pCount >= 4) ? c.substring(pipes[2] + 1, pipes[3]) : "0";
          String lastDate = (pCount >= 5) ? c.substring(pipes[3] + 1, pipes[4]) : "";
          String firstTime = (pCount >= 6) ? c.substring(pipes[4] + 1, pipes[5]) : "-";
          
          String lastTime = "-";
          String linkedRfid = "";
          String dept = "-";
          
          if (fn.endsWith("_f.txt")) {
            // Fingerprint profile
            if (pCount >= 8) {
              lastTime = c.substring(pipes[5] + 1, pipes[6]);
              linkedRfid = c.substring(pipes[6] + 1, pipes[7]);
              dept = c.substring(pipes[7] + 1);
            } else if (pCount >= 7) {
              lastTime = c.substring(pipes[5] + 1, pipes[6]);
              linkedRfid = c.substring(pipes[6] + 1);
            } else if (pCount >= 6) {
              lastTime = c.substring(pipes[5] + 1);
            }
          } else {
            // Plain RFID profile
            if (pCount >= 7) {
              lastTime = c.substring(pipes[5] + 1, pipes[6]);
              dept = c.substring(pipes[6] + 1);
            } else if (pCount >= 6) {
              if (state == "0") {
                firstTime = c.substring(pipes[4] + 1, pipes[5]);
                dept = c.substring(pipes[5] + 1);
              } else {
                lastTime = c.substring(pipes[5] + 1);
              }
            } else if (pCount >= 5) {
              firstTime = c.substring(pipes[4] + 1);
            }
          }
          
          lastTime.trim();
          linkedRfid.trim();
          dept.trim();

          if (lastDate != todayDateStr) {
            state = "0";
            firstTime = "-";
            lastTime = "-";
          }

          response->printf(
              "{\"uid\":\"%s\",\"name\":\"%s\",\"role\":\"%s\","
              "\"roll\":\"%s\",\"state\":\"%s\",\"in\":\"%s\",\"out\":\"%s\",\"rfid\":\"%s\",\"dept\":\"%s\"}",
              uid_clean.c_str(), name.c_str(), role.c_str(), roll.c_str(),
              state.c_str(), firstTime.c_str(), lastTime.c_str(), linkedRfid.c_str(), dept.c_str());
        }
      }
      file.close(); // Close explicitly before opening next
      file = root.openNextFile();
      yield();
    }
    response->print("]");
    request->send(response);
  });

  server.on("/save", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (!isAuth(request)) {
      return request->send(403, "text/plain", "Unauthorized");
    }
    if (!request->hasParam("uid"))
      return request->send(400);

    String cleanUid = request->getParam("uid")->value();
    cleanUid.replace("UID:", "");
    cleanUid.replace(" ", "");
    cleanUid.trim();
    cleanUid.toUpperCase();

    String type = request->hasParam("type") ? request->getParam("type")->value() : "RFID";
    String path = "/" + cleanUid + (type == "FINGER" ? "_f.txt" : ".txt");
    
    String rfidVal = "";
    if (type == "FINGER" && request->hasParam("rfid")) {
      rfidVal = request->getParam("rfid")->value();
      rfidVal.replace("UID:", "");
      rfidVal.replace(" ", "");
      rfidVal.trim();
      rfidVal.toUpperCase();
    }

    String dept = request->hasParam("dept") ? request->getParam("dept")->value() : "-";
    dept.trim();
    if (dept.length() == 0) dept = "-";

    File f = LittleFS.open(path, FILE_WRITE);
    if (f) {
      if (type == "FINGER") {
        f.print(request->getParam("name")->value() + "|" +
                request->getParam("role")->value() + "|" +
                request->getParam("roll")->value() +
                "|0||-|-|" + rfidVal + "|" + dept); // State 0, no date, no times, linked RFID card, department
      } else {
        f.print(request->getParam("name")->value() + "|" +
                request->getParam("role")->value() + "|" +
                request->getParam("roll")->value() +
                "|0||-|" + dept); // Standard style with department
      }
      f.close();

      // Trigger 5-second buzzer for successful enrollment!
      triggerBuzzer(5000);
      
      // Update TFT screen to show confirmation
      String name = request->hasParam("name") ? request->getParam("name")->value() : "Unknown";
      String role = request->hasParam("role") ? request->getParam("role")->value() : "-";
      String roll = request->hasParam("roll") ? request->getParam("roll")->value() : "";
      drawTftEnrollConfirmedScreen(name, roll, role, type);
      tftMessageUntil = millis() + 5000; // Show confirmation screen for 5 seconds

      // Dual Persistence: Queue enrollment record for Webhook & SQL Database sync
      extern void queueSqlSync(String id, String name, String role, String dir, String status, String type);
      queueSqlSync(roll.length() > 0 ? roll : cleanUid, name, role, "ENROLL", "Registered", type);
    }

    // Write redirect link file if RFID card is specified
    if (rfidVal.length() > 0) {
      File lf = LittleFS.open("/" + rfidVal + ".txt", FILE_WRITE);
      if (lf) {
        lf.print("LINK|" + cleanUid);
        lf.close();
        Serial.printf("🔗 Linked RFID card %s to Finger ID %s\n", rfidVal.c_str(), cleanUid.c_str());
      }
    }

    isEnrollMode = false;
    if (xSemaphoreTake(fpMutex, pdMS_TO_TICKS(500))) {
      finger.LEDcontrol(4, 0, 1); // LED Off
      xSemaphoreGive(fpMutex);
    }
    updateFsSizes();
    request->send(200, "text/plain", "OK");
  });

  server.on("/skip-rfid", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (!isAuth(request)) return request->send(403, "text/plain", "Unauthorized");
    isEnrollMode = false;
    lastScanTime = millis();
    if (xSemaphoreTake(fpMutex, pdMS_TO_TICKS(500))) {
      finger.LEDcontrol(4, 0, 1); // Turn off R503 LED
      xSemaphoreGive(fpMutex);
    }
    // Broadcast success without RFID link
    ws.textAll("{\"type\":\"ENROLL_ST\",\"status\":\"SUCCESS\",\"uid\":\"" + String(enrollID) + "\",\"rfid\":\"\",\"msg\":\"Fingerprint captured! (Card skipped)\"}");
    triggerBuzzer(5000);
    
    // Update TFT to show captured, waiting for save
    wasEnrollActive = false; // Prevent automatic loop reset to idle
    drawTftEnrollScanningScreen("FINGER", enrollID, 3, "Captured! Save in browser.");
    
    request->send(200, "text/plain", "OK");
  });

  server.on("/delete-user", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (!isAuth(request)) return request->send(403, "text/plain", "Unauthorized");
    if (!request->hasParam("uid"))
      return request->send(400);
    String uid = request->getParam("uid")->value();
    uid.trim();
    bool removed = false;
    
    String path = "";
    bool isFingerprint = false;
    String cleanFpId = "";
    
    if (uid.endsWith("_f") || uid.endsWith("_F")) {
      isFingerprint = true;
      cleanFpId = uid.substring(0, uid.length() - 2);
      path = "/" + cleanFpId + "_f.txt";
    } else {
      uid.toUpperCase();
      path = "/" + uid + ".txt";
      if (!LittleFS.exists(path)) {
        path = "/" + uid + "_f.txt";
        if (LittleFS.exists(path)) {
          isFingerprint = true;
          cleanFpId = uid;
        } else {
          path = "/" + uid + ".txt";
        }
      }
    }
    
    if (isFingerprint && LittleFS.exists(path)) {
      File f = LittleFS.open(path, FILE_READ);
      if (f) {
        String c = f.readString();
        f.close();
        
        // Parse 8th field (index 7 after 7th pipe)
        int lastPos = -1;
        int pCount = 0;
        int pipes[8];
        for (int i = 0; i < 8; i++) {
          pipes[i] = c.indexOf('|', lastPos + 1);
          if (pipes[i] != -1) {
            lastPos = pipes[i];
            pCount++;
          } else {
            pipes[i] = -1;
          }
        }
        
        if (pCount >= 7 && pipes[6] != -1) {
          String linkedRfid = c.substring(pipes[6] + 1);
          linkedRfid.trim();
          if (linkedRfid.length() > 0) {
            LittleFS.remove("/" + linkedRfid + ".txt");
            Serial.printf("🗑 Deleted linked RFID card file: /%s.txt\n", linkedRfid.c_str());
          }
        }
      }
      LittleFS.remove(path);
      
      // Also delete from biometric sensor
      int fpId = cleanFpId.toInt();
      if (fpId > 0 && fpId <= MAX_FP) {
        if (xSemaphoreTake(fpMutex, pdMS_TO_TICKS(1000))) {
          if (finger.deleteModel(fpId) == FINGERPRINT_OK) {
            Serial.printf("🗑 Deleted finger template ID %d from sensor\n", fpId);
          }
          fpUsed[fpId] = false;
          xSemaphoreGive(fpMutex);
          extern void saveFpCache();
          saveFpCache();
        }
      }
      removed = true;
    } else if (LittleFS.exists(path)) {
      // Check if it's a LINK file
      File f = LittleFS.open(path, FILE_READ);
      if (f) {
        String c = f.readString();
        f.close();
        if (c.startsWith("LINK|")) {
          String linkedFpId = c.substring(5);
          linkedFpId.trim();
          // Delete both the link file and the fingerprint file!
          LittleFS.remove("/" + linkedFpId + "_f.txt");
          int fpId = linkedFpId.toInt();
          if (fpId > 0 && fpId <= MAX_FP) {
            if (xSemaphoreTake(fpMutex, pdMS_TO_TICKS(1000))) {
              finger.deleteModel(fpId);
              fpUsed[fpId] = false;
              xSemaphoreGive(fpMutex);
              extern void saveFpCache();
              saveFpCache();
            }
          }
        }
      }
      LittleFS.remove(path);
      removed = true;
    }
    
    if (removed) {
      updateFsSizes();
      request->send(200, "text/plain", "OK");
    } else
      request->send(404, "text/plain", "User not found");
  });

  server.on("/edit-user", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (!isAuth(request)) {
      return request->send(403, "text/plain", "Unauthorized");
    }
    if (!request->hasParam("uid") || !request->hasParam("name") ||
        !request->hasParam("role") || !request->hasParam("roll"))
      return request->send(400);

    String uid = request->getParam("uid")->value();
    uid.trim();
    String name = request->getParam("name")->value();
    String role = request->getParam("role")->value();
    String roll = request->getParam("roll")->value();
    
    String newRfid = request->hasParam("rfid") ? request->getParam("rfid")->value() : "";
    newRfid.replace("UID:", "");
    newRfid.replace(" ", "");
    newRfid.trim();
    newRfid.toUpperCase();

    String path = "";
    bool isFingerprint = false;
    String cleanFpId = "";

    if (uid.endsWith("_f") || uid.endsWith("_F")) {
      isFingerprint = true;
      cleanFpId = uid.substring(0, uid.length() - 2);
      path = "/" + cleanFpId + "_f.txt";
    } else {
      uid.toUpperCase();
      path = "/" + uid + ".txt";
      if (!LittleFS.exists(path)) {
        path = "/" + uid + "_f.txt";
        if (LittleFS.exists(path)) {
          isFingerprint = true;
          cleanFpId = uid;
        } else {
          path = "/" + uid + ".txt";
        }
      }
    }

    if (LittleFS.exists(path)) {
      String state = "0";
      String lastDate = "";
      String firstTime = "-";
      String lastTime = "-";
      String oldRfid = "";
      String oldDept = "-";
      
      File fr = LittleFS.open(path, FILE_READ);
      if (fr) {
        String c = fr.readString();
        fr.close();
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
          state = (pCount >= 4) ? c.substring(pipes[2] + 1, pipes[3]) : "0";
          lastDate = (pCount >= 5) ? c.substring(pipes[3] + 1, pipes[4]) : "";
          
          if (isFingerprint) {
            if (pCount >= 8) {
              firstTime = c.substring(pipes[4] + 1, pipes[5]);
              lastTime = c.substring(pipes[5] + 1, pipes[6]);
              oldRfid = c.substring(pipes[6] + 1, pipes[7]);
              oldDept = c.substring(pipes[7] + 1);
            } else if (pCount >= 7) {
              firstTime = c.substring(pipes[4] + 1, pipes[5]);
              lastTime = c.substring(pipes[5] + 1, pipes[6]);
              oldRfid = c.substring(pipes[6] + 1);
            } else if (pCount >= 6) {
              firstTime = c.substring(pipes[4] + 1, pipes[5]);
              lastTime = c.substring(pipes[5] + 1);
            } else if (pCount >= 5) {
              firstTime = c.substring(pipes[4] + 1);
            }
          } else {
            if (pCount >= 7) {
              firstTime = c.substring(pipes[4] + 1, pipes[5]);
              lastTime = c.substring(pipes[5] + 1, pipes[6]);
              oldDept = c.substring(pipes[6] + 1);
            } else if (pCount >= 6) {
              if (state == "0") {
                firstTime = c.substring(pipes[4] + 1, pipes[5]);
                oldDept = c.substring(pipes[5] + 1);
              } else {
                firstTime = c.substring(pipes[4] + 1, pipes[5]);
                lastTime = c.substring(pipes[5] + 1);
              }
            } else if (pCount >= 5) {
              firstTime = c.substring(pipes[4] + 1);
            }
          }
          
          state.trim();
          lastDate.trim();
          firstTime.trim();
          lastTime.trim();
          oldRfid.trim();
          oldDept.trim();
        }
      }
      
      String dept = request->hasParam("dept") ? request->getParam("dept")->value() : oldDept;
      dept.trim();
      if (dept.length() == 0) dept = "-";

      // If editing a fingerprint user and the RFID card link is being modified
      if (isFingerprint) {
        if (newRfid != oldRfid) {
          // Delete old RFID link file if it existed
          if (oldRfid.length() > 0) {
            LittleFS.remove("/" + oldRfid + ".txt");
            Serial.printf("🗑 Deleted old linked RFID file: /%s.txt\n", oldRfid.c_str());
          }
          
          // Create new RFID link file if new RFID is specified
          if (newRfid.length() > 0) {
            String checkPath = "/" + newRfid + ".txt";
            bool alreadyInUse = false;
            
            if (LittleFS.exists(checkPath)) {
              File cf = LittleFS.open(checkPath, FILE_READ);
              if (cf) {
                String contents = cf.readString();
                cf.close();
                String expectedLink1 = "LINK|" + cleanFpId;
                String expectedLink2 = "LINK|" + cleanFpId + "_f";
                contents.trim();
                if (contents.startsWith("LINK|") && contents != expectedLink1 && contents != expectedLink2) {
                  alreadyInUse = true;
                } else if (!contents.startsWith("LINK|")) {
                  alreadyInUse = true; // Legacy profile file with this card UID exists
                }
              }
            }
            
            if (alreadyInUse) {
              return request->send(409, "text/plain", "RFID Card already in use by another employee");
            }
            
            File lf = LittleFS.open(checkPath, FILE_WRITE);
            if (lf) {
              lf.print("LINK|" + cleanFpId);
              lf.close();
              Serial.printf("🔗 Linked new RFID card %s to Finger ID %s during edit\n", newRfid.c_str(), cleanFpId.c_str());
            }
          }
        }
      }

      File fw = LittleFS.open(path, FILE_WRITE);
      if (fw) {
        if (isFingerprint) {
          fw.print(name + "|" + role + "|" + roll + "|" + state + "|" + lastDate +
                   "|" + firstTime + "|" + lastTime + "|" + newRfid + "|" + dept);
        } else {
          fw.print(name + "|" + role + "|" + roll + "|" + state + "|" + lastDate +
                   "|" + firstTime + "|" + lastTime + "|" + dept);
        }
        fw.close();
        updateFsSizes();
        request->send(200, "text/plain", "OK");
      } else {
        request->send(500, "text/plain", "Write Error");
      }
    } else {
      request->send(404, "text/plain", "Not Found");
    }
  });

  server.on("/toggle-led", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (!isAuth(request)) return request->send(403, "text/plain", "Unauthorized");
    if (request->hasParam("state")) {
      String state = request->getParam("state")->value();
      manualLedOn = (state == "1");
      if (manualLedOn) {
        digitalWrite(LED_PIN, HIGH);
        controlOnboardLed(true, 0, 128, 0); // Green for active manual exit release
      } else {
        digitalWrite(LED_PIN, LOW);
        controlOnboardLed(false, 0, 0, 0); // Turn off
      }
      request->send(200, "text/plain", manualLedOn ? "ON" : "OFF");
    } else {
      request->send(400);
    }
  });

  server.on("/reboot", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (!isAuth(request)) return request->send(403, "text/plain", "Unauthorized");
    request->send(200, "text/plain", "OK");
    restartTime = millis() + 1000;
  });

  server.on("/delete-all-logs", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (!isAuth(request)) return request->send(403, "text/plain", "Unauthorized");
    if (useEEPROM) {
      clearAllEEPROMLogs();
    } else {
      File root = LittleFS.open("/");
      File file = root.openNextFile();
      while (file) {
        String fn = file.name();
        if (fn.startsWith("logs_") && fn.endsWith(".csv")) {
          LittleFS.remove("/" + fn);
        }
        file = root.openNextFile();
      }
    }
    updateFsSizes();
    request->send(200, "text/plain", "OK");
  });

  server.on("/get-history", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (!isAuth(request)) return request->send(403, "text/plain", "Unauthorized");
    if (!request->hasParam("date"))
      return request->send(400);
    String targetDate = request->getParam("date")->value();
    
    if (useEEPROM) {
      AsyncResponseStream *response = request->beginResponseStream("text/plain");
      response->println("Time,ID,Name,Role,Direction,Status,Type");
      
      uint16_t logsToProcess = eepromTotalLogs;
      uint16_t readIdx = (eepromNextIndex - eepromTotalLogs + MAX_EEPROM_LOGS) % MAX_EEPROM_LOGS;
      
      for (uint16_t i = 0; i < logsToProcess; i++) {
        EEPROMLogEntry entry;
        readEEPROMLog(readIdx, entry);
        
        time_t logTime = entry.timestamp;
        struct tm *ti = localtime(&logTime);
        char dateBuf[15];
        if (ti != nullptr) {
          strftime(dateBuf, sizeof(dateBuf), "%d-%m-%Y", ti);
        } else {
          strcpy(dateBuf, "00-00-0000");
        }
        
        if (String(dateBuf) == targetDate) {
          char timeBuf[10];
          if (ti != nullptr) {
            strftime(timeBuf, sizeof(timeBuf), "%H:%M:%S", ti);
          } else {
            strcpy(timeBuf, "00:00:00");
          }
          
          String uidStr = String(entry.uid);
          String name, role, roll;
          lookupUserProfile(uidStr, name, role, roll);
          
          String dirStr = (entry.direction == 1) ? "In" : "Out";
          
          String statusStr;
          if (entry.status == 1) statusStr = "On-Time";
          else if (entry.status == 2) statusStr = "Late";
          else if (entry.status == 3) statusStr = "Early Exit";
          else if (entry.status == 4) statusStr = "Accepted";
          else if (entry.status == 5) statusStr = "Denied";
          else statusStr = "Accepted";
          
          String methodStr;
          if (entry.method == 1) methodStr = "Fingerprint";
          else if (entry.method == 2) methodStr = "RFID Card";
          else if (entry.method == 3) methodStr = "Manual";
          else methodStr = "Unknown";
          
          response->println(String(timeBuf) + "," + roll + "," + name + "," + role + "," + dirStr + "," + statusStr + "," + methodStr);
        }
        
        readIdx = (readIdx + 1) % MAX_EEPROM_LOGS;
      }
      
      // Fallback: Also append logs from LittleFS if they exist
      String path = "/logs_" + targetDate + ".csv";
      if (LittleFS.exists(path)) {
        File f = LittleFS.open(path, FILE_READ);
        if (f) {
          if (f.available()) {
            f.readStringUntil('\n'); // Skip header
          }
          while (f.available()) {
            response->print(f.readStringUntil('\n') + "\n");
          }
          f.close();
        }
      }
      
      request->send(response);
    } else {
      String path = "/logs_" + targetDate + ".csv";
      if (LittleFS.exists(path))
        request->send(LittleFS, path, "text/plain");
      else
        request->send(200, "text/plain", "");
    }
  });

  // NEW: Delete logs route
  server.on("/delete-logs", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (!isAuth(request)) return request->send(403, "text/plain", "Unauthorized");
    if (!request->hasParam("date"))
      return request->send(400);
    String targetDate = request->getParam("date")->value();
    
    if (useEEPROM) {
      deleteEEPROMLogsForDate(targetDate);
      request->send(200, "text/plain", "OK");
    } else {
      String path = "/logs_" + targetDate + ".csv";
      if (LittleFS.exists(path)) {
        LittleFS.remove(path);
        updateFsSizes();
        request->send(200, "text/plain", "OK");
      } else {
        request->send(404, "text/plain", "Not Found");
      }
    }
  });

  // Added download logs route so the CSV export works (missing in original
  // code)
  server.on("/download-logs", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (!isAuth(request)) return request->send(403, "text/plain", "Unauthorized");
    if (!request->hasParam("date"))
      return request->send(400);
    String targetDate = request->getParam("date")->value();
    
    if (useEEPROM) {
      AsyncResponseStream *response = request->beginResponseStream("text/csv");
      response->addHeader("Content-Disposition", "attachment; filename=logs_" + targetDate + ".csv");
      response->println("Time,ID,Name,Role,Direction,Status,Type");
      
      uint16_t logsToProcess = eepromTotalLogs;
      uint16_t readIdx = (eepromNextIndex - eepromTotalLogs + MAX_EEPROM_LOGS) % MAX_EEPROM_LOGS;
      
      for (uint16_t i = 0; i < logsToProcess; i++) {
        EEPROMLogEntry entry;
        readEEPROMLog(readIdx, entry);
        
        time_t logTime = entry.timestamp;
        struct tm *ti = localtime(&logTime);
        char dateBuf[15];
        if (ti != nullptr) {
          strftime(dateBuf, sizeof(dateBuf), "%d-%m-%Y", ti);
        } else {
          strcpy(dateBuf, "00-00-0000");
        }
        
        if (String(dateBuf) == targetDate) {
          char timeBuf[10];
          if (ti != nullptr) {
            strftime(timeBuf, sizeof(timeBuf), "%H:%M:%S", ti);
          } else {
            strcpy(timeBuf, "00:00:00");
          }
          
          String uidStr = String(entry.uid);
          String name, role, roll;
          lookupUserProfile(uidStr, name, role, roll);
          
          String dirStr = (entry.direction == 1) ? "In" : "Out";
          
          String statusStr;
          if (entry.status == 1) statusStr = "On-Time";
          else if (entry.status == 2) statusStr = "Late";
          else if (entry.status == 3) statusStr = "Early Exit";
          else if (entry.status == 4) statusStr = "Accepted";
          else if (entry.status == 5) statusStr = "Denied";
          else statusStr = "Accepted";
          
          String methodStr;
          if (entry.method == 1) methodStr = "Fingerprint";
          else if (entry.method == 2) methodStr = "RFID Card";
          else if (entry.method == 3) methodStr = "Manual";
          else methodStr = "Unknown";
          
          response->println(String(timeBuf) + "," + roll + "," + name + "," + role + "," + dirStr + "," + statusStr + "," + methodStr);
        }
        
        readIdx = (readIdx + 1) % MAX_EEPROM_LOGS;
      }
      
      // Fallback: Also append logs from LittleFS if they exist
      String path = "/logs_" + targetDate + ".csv";
      if (LittleFS.exists(path)) {
        File f = LittleFS.open(path, FILE_READ);
        if (f) {
          if (f.available()) {
            f.readStringUntil('\n'); // Skip header
          }
          while (f.available()) {
            response->print(f.readStringUntil('\n') + "\n");
          }
          f.close();
        }
      }
      
      request->send(response);
    } else {
      String path = "/logs_" + targetDate + ".csv";
      if (LittleFS.exists(path)) {
        request->send(LittleFS, path, "text/csv", true);
      } else {
        request->send(404, "text/plain", "Logs not found");
      }
    }
  });

  server.on("/set-wifi", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (!isAuth(request)) return request->send(403, "text/plain", "Unauthorized");
    if (!request->hasParam("ssid"))
      return request->send(400);
    String s = request->getParam("ssid")->value();
    String p =
        request->hasParam("pass") ? request->getParam("pass")->value() : "";
    String ip = request->hasParam("ip") ? request->getParam("ip")->value() : "";
    String gw = request->hasParam("gw") ? request->getParam("gw")->value() : "";
    String sn = request->hasParam("sn") ? request->getParam("sn")->value() : "";

    File f = LittleFS.open("/wifi.conf", FILE_WRITE);
    if (f) {
      f.println(s);
      f.println(p);
      f.println(ip);
      f.println(gw);
      f.println(sn);
      f.close();
    }

    request->send(200, "text/plain", "OK");
    restartTime = millis() + 1000;
  });

  server.on("/scan-wifi", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (!isAuth(request)) return request->send(403, "text/plain", "Unauthorized");
    int n = WiFi.scanNetworks();
    String json = "[";
    for (int i = 0; i < n; ++i) {
      if (i)
        json += ",";
      json += "\"" + WiFi.SSID(i) + "\"";
    }
    json += "]";
    request->send(200, "application/json", json);
  });

  server.on("/factory-reset", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (!isAuth(request)) return request->send(403, "text/plain", "Unauthorized");
    Serial.println("Performing FULL Factory Reset...");
    
    // 1. Clear Fingerprint Sensor
    if (xSemaphoreTake(fpMutex, pdMS_TO_TICKS(2000))) {
      finger.emptyDatabase();
      for (int i = 1; i <= MAX_FP; i++) fpUsed[i] = false;
      xSemaphoreGive(fpMutex);
    }
    
    // 2. Delete All Files in LittleFS (Employees, Logs, Configs)
    File root = LittleFS.open("/");
    File file = root.openNextFile();
    while (file) {
      String fn = file.name();
      if (!fn.startsWith("/")) fn = "/" + fn;
      file.close(); // Close before removing
      LittleFS.remove(fn);
      file = root.openNextFile();
    }
    updateFsSizes();
    request->send(200, "text/plain", "OK");
    restartTime = millis() + 1500;
  });

  server.on("/reset-wifi", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (!isAuth(request)) return request->send(403, "text/plain", "Unauthorized");
    LittleFS.remove("/wifi.conf");
    request->send(200, "text/plain", "OK");
    restartTime = millis() + 1000;
  });

  server.on("/ip", HTTP_GET, [](AsyncWebServerRequest *request) {
    bool ethConnected = false;
    #if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0)
      ethConnected = ETH.linkUp();
    #else
      ethConnected = (ETH.localIP() != IPAddress(0,0,0,0));
    #endif

    if (ethConnected) {
      request->send(200, "text/plain", ETH.localIP().toString());
    } else if (WiFi.status() == WL_CONNECTED) {
      request->send(200, "text/plain", WiFi.localIP().toString());
    } else {
      request->send(200, "text/plain", "AP:" + WiFi.softAPIP().toString());
    }
  });

  // Test route to play buzzer: http://<IP-address>/play-test or http://<IP-address>/play-test?duration=1000
  server.on("/play-test", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (!isAuth(request)) return request->send(403, "text/plain", "Unauthorized");
    int dur = 1000;
    if (request->hasParam("duration")) {
      dur = request->getParam("duration")->value().toInt();
    }
    triggerBuzzer(dur);
    request->send(200, "text/plain", "🔊 Triggered buzzer for " + String(dur) + " ms");
  });

  server.on("/sys-info", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (!isAuth(request)) return request->send(403, "text/plain", "Unauthorized");
    size_t totalBytes = useEEPROM ? (MAX_EEPROM_LOGS * sizeof(EEPROMLogEntry)) : fsTotalBytes;
    size_t usedBytes = useEEPROM ? (eepromTotalLogs * sizeof(EEPROMLogEntry)) : fsUsedBytes;
    
    String json = "{";
    json += "\"totalBytes\":" + String(totalBytes) + ",";
    json += "\"usedBytes\":" + String(usedBytes) + ",";
    json += "\"freeHeap\":" + String(ESP.getFreeHeap()) + ",";
    json += "\"uptime\":" + String(millis() / 1000) + ",";
    json += "\"wifiRssi\":" + String(WiFi.RSSI()) + ",";
    json += "\"useEEPROM\":" + String(useEEPROM ? "true" : "false");
    json += "}";
    request->send(200, "application/json", json);
  });

  server.on("/get-ram", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (!isAuth(request)) return request->send(403, "text/plain", "Unauthorized");
    size_t totalBytes = useEEPROM ? (MAX_EEPROM_LOGS * sizeof(EEPROMLogEntry)) : fsTotalBytes;
    size_t usedBytes = useEEPROM ? (eepromTotalLogs * sizeof(EEPROMLogEntry)) : fsUsedBytes;
    float pct = (totalBytes > 0) ? ((float)usedBytes / totalBytes * 100.0) : 0.0;
    
    String json = "{";
    json += "\"free\":" + String(ESP.getFreeHeap()) + ",";
    json += "\"stor_pct\":" + String(pct) + ",";
    json += "\"ip\":\"" + getActiveIPAddress() + "\"";
    json += "}";
    request->send(200, "application/json", json);
  });


  server.on("/set-time", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (!isAuth(request)) return request->send(403, "text/plain", "Unauthorized");
    if (request->hasParam("t")) {
      struct timeval tv;
      tv.tv_sec = request->getParam("t")->value().toInt();
      tv.tv_usec = 0;
      settimeofday(&tv, NULL);
      Serial.printf("Time synced from browser: %d\n", tv.tv_sec);
    }
    request->send(200);
  });

  server.on("/init-data", HTTP_GET, [](AsyncWebServerRequest *request) {
    updateFsSizes();
    size_t otaSize = 0;
    const esp_partition_t* update_partition = esp_ota_get_next_update_partition(NULL);
    if (update_partition != NULL) {
      otaSize = update_partition->size;
    }

    size_t totalBytes = useEEPROM ? (MAX_EEPROM_LOGS * sizeof(EEPROMLogEntry)) : fsTotalBytes;
    size_t usedBytes = useEEPROM ? (eepromTotalLogs * sizeof(EEPROMLogEntry)) : fsUsedBytes;

    String json = "{";
    json += "\"ip\":\"" + getActiveIPAddress() + "\",";
    json += "\"mac\":\"" + WiFi.macAddress() + "\",";
    json += "\"heap\":" + String(ESP.getFreeHeap()) + ",";
    json += "\"uptime\":" + String(millis() / 1000) + ",";
    json += "\"totalBytes\":" + String(totalBytes) + ",";
    json += "\"usedBytes\":" + String(usedBytes) + ",";
    json += "\"rssi\":" + String(WiFi.RSSI()) + ",";
    json += "\"deviceName\":\"" + deviceName + "\",";
    json += "\"webhookUrl\":\"" + webhookUrl + "\",";
    json += "\"tftPromptEnabled\":" + String(tftPromptEnabled ? "true" : "false") + ",";
    json += "\"twoFactorEnabled\":" + String(twoFactorEnabled ? "true" : "false") + ",";
    json += "\"otaSize\":" + String(otaSize) + ",";
    unsigned long timeLeft = 0;
    if (isAPMode && apModeStartTime > 0) {
      unsigned long elapsed = millis() - apModeStartTime;
      if (elapsed < 300000) {
        timeLeft = (300000 - elapsed) / 1000;
      }
    }
    json += "\"flashSize\":" + String(ESP.getFlashChipSize()) + ",";
    json += "\"fsTotalBytes\":" + String(fsTotalBytes) + ",";
    json += "\"fsUsedBytes\":" + String(fsUsedBytes) + ",";
    json += "\"useEEPROM\":" + String(useEEPROM ? "true" : "false") + ",";
    json += "\"googleApiKey\":\"" + googleApiKey + "\",";
    json += "\"isAPMode\":" + String(isAPMode ? "true" : "false") + ",";
    json += "\"apModeTimeLeft\":" + String(timeLeft) + ",";
    json += "\"sqlApiUrl\":\"" + sqlApiUrl + "\",";
    json += "\"sqlTotalLogs\":" + String(sqlTotalLogs) + ",";
    json += "\"sqlTransferredLogs\":" + String(sqlTransferredLogs) + ",";
    json += "\"sqlPendingLogs\":" + String(sqlPendingLogs) + ",";
    json += "\"weatherCity\":\"" + weatherCity + "\"";
    json += "}";
    request->send(200, "application/json", json);
  });


  // --- Enrollment Routes ---
  server.on("/enroll-next-id", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (!isAuth(request)) return request->send(403, "text/plain", "Unauthorized");
    int nextID = -1;
    for (int i = 1; i <= MAX_FP; i++) {
      if (!fpUsed[i]) {
        nextID = i;
        break;
      }
    }
    if (nextID != -1) {
      request->send(200, "text/plain", String(nextID));
    } else {
      request->send(200, "text/plain", "FULL");
    }
  });

  server.on("/start-enroll", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (!isAuth(request)) return request->send(403, "text/plain", "Unauthorized");
    if (!request->hasParam("type"))
      return request->send(400);
    enrollType = request->getParam("type")->value();

    if (enrollType == "FINGER") {
      if (!request->hasParam("id"))
        return request->send(400);
      enrollID = request->getParam("id")->value().toInt();
      enrollStep = 1;
      if (xSemaphoreTake(fpMutex, pdMS_TO_TICKS(500))) {
        finger.LEDcontrol(1, 100, 3); // Breathing Purple
        xSemaphoreGive(fpMutex);
      }
      // Audio cue removed
    } else {
      enrollID = -1;
      enrollStep = 0;
      // Audio cue removed
    }

    isEnrollMode = true;
    enrollTimer = millis();
    request->send(200, "text/plain", "OK");
  });

  server.on("/cancel-enroll", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (!isAuth(request)) return request->send(403, "text/plain", "Unauthorized");
    isEnrollMode = false;
    wasEnrollActive = false;
    lastScanTime = millis();
    if (xSemaphoreTake(fpMutex, pdMS_TO_TICKS(500))) {
      finger.LEDcontrol(4, 0, 1); // LED Off
      xSemaphoreGive(fpMutex);
    }
    currentTftState = TFT_IDLE;
    drawTftDefaultScreen();
    request->send(200, "text/plain", "OK");
  });

  server.on("/get-fp-memory", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (!isAuth(request)) return request->send(403, "text/plain", "Unauthorized");
    if (request->hasParam("sync")) {
      syncFingerprintDatabase();
    }
    String json = "[";
    bool first = true;
    for (int i = 1; i <= MAX_FP; i++) {
      if (fpUsed[i]) {
        if (!first)
          json += ",";
        json += String(i);
        first = false;
      }
    }
    json += "]";
    request->send(200, "application/json", json);
  });

  server.on("/delete-fp", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (!isAuth(request)) return request->send(403, "text/plain", "Unauthorized");
    if (!request->hasParam("id"))
      return request->send(400);
    int id = request->getParam("id")->value().toInt();
    bool success = false;
    if (xSemaphoreTake(fpMutex, pdMS_TO_TICKS(1000))) {
      if (finger.deleteModel(id) == FINGERPRINT_OK)
        success = true;
      xSemaphoreGive(fpMutex);
    }
    if (success) {
      fpUsed[id] = false;
      extern void saveFpCache();
      saveFpCache();
      request->send(200, "text/plain", "OK");
    } else {
      request->send(500, "text/plain", "Delete Failed");
    }
  });

  server.on("/hard-wipe-sensor", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (!isAuth(request)) return request->send(403, "text/plain", "Unauthorized");
    Serial.println("Hard Wipe requested...");
    
    xTaskCreate([](void* p) {
      Serial.println(">>> Starting Background Hard Wipe...");
      if (xSemaphoreTake(fpMutex, pdMS_TO_TICKS(10000))) {
        for (int i = 1; i <= MAX_FP; i++) {
          finger.deleteModel(i);
          fpUsed[i] = false;
          vTaskDelay(pdMS_TO_TICKS(50)); 
        }
        extern void saveFpCache();
        saveFpCache();
        xSemaphoreGive(fpMutex);
        Serial.println(">>> Background Hard Wipe COMPLETE.");
      }
      vTaskDelete(NULL);
    }, "wipe_task", 4096, NULL, 1, NULL);
    
    request->send(200, "text/plain", "STARTED");
  });

  server.on("/clear-fp-all", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (!isAuth(request)) return request->send(403, "text/plain", "Unauthorized");
    bool success = false;
    if (xSemaphoreTake(fpMutex, pdMS_TO_TICKS(2000))) {
      if (finger.emptyDatabase() == FINGERPRINT_OK)
        success = true;
      xSemaphoreGive(fpMutex);
    }
    if (success) {
      for (int i = 1; i <= MAX_FP; i++)
        fpUsed[i] = false;
      extern void saveFpCache();
      saveFpCache();
      request->send(200, "text/plain", "OK");
    } else {
      request->send(500, "text/plain", "Clear Failed");
    }
  });

  server.on("/factory-reset", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (!isAuth(request)) return request->send(403, "text/plain", "Unauthorized");
    Serial.println("🔥 Factory Reset requested! Erasing all LittleFS files, logs, and sensor templates...");

    // 1. Collect all file paths in LittleFS root to avoid iterator mutation skips
    std::vector<String> filesToDelete;
    File root = LittleFS.open("/");
    if (root && root.isDirectory()) {
      File file = root.openNextFile();
      while (file) {
        String fn = String(file.name());
        if (!fn.startsWith("/")) fn = "/" + fn;
        filesToDelete.push_back(fn);
        file = root.openNextFile();
      }
      root.close();
    }

    // Also collect files inside /photos directory if present
    if (LittleFS.exists("/photos")) {
      File pDir = LittleFS.open("/photos");
      if (pDir && pDir.isDirectory()) {
        File file = pDir.openNextFile();
        while (file) {
          String fn = String(file.name());
          if (!fn.startsWith("/photos/")) fn = "/photos/" + fn;
          filesToDelete.push_back(fn);
          file = pDir.openNextFile();
        }
        pDir.close();
      }
      LittleFS.rmdir("/photos");
    }

    // Now delete all collected files cleanly
    for (size_t i = 0; i < filesToDelete.size(); i++) {
      LittleFS.remove(filesToDelete[i]);
    }

    // Explicitly remove system sync and state files
    LittleFS.remove("/pending_sync.txt");
    LittleFS.remove("/sql_config.txt");
    LittleFS.remove("/sql_transferred.txt");
    LittleFS.remove("/weather_city.txt");
    LittleFS.remove("/holidays.json");
    LittleFS.remove("/wifi.txt");
    LittleFS.remove("/wifi.conf");

    // 2. Reset in-memory SQL sync stats
    sqlPendingLogs = 0;
    sqlTransferredLogs = 0;
    sqlTotalLogs = 0;

    // 3. Clear EEPROM logger
    clearAllEEPROMLogs();

    // 4. Clear Fingerprint Sensor database
    if (xSemaphoreTake(fpMutex, pdMS_TO_TICKS(2000))) {
      finger.emptyDatabase();
      xSemaphoreGive(fpMutex);
    }
    for (int i = 1; i <= MAX_FP; i++) {
      fpUsed[i] = false;
    }
    extern void saveFpCache();
    saveFpCache();

    // Broadcast reset state to all open web dashboard clients
    ws.textAll("{\"type\":\"SQL_SYNC\",\"transferred\":0,\"pending\":0}");

    request->send(200, "text/plain", "OK");
    
    // Schedule reboot after 2 seconds
    restartTime = millis() + 2000;
  });

  server.on("/delete-logs", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (!isAuth(request)) return request->send(403, "text/plain", "Unauthorized");
    if (!request->hasParam("date")) return request->send(400, "text/plain", "Missing date");
    String dateStr = request->getParam("date")->value();
    
    // Delete LittleFS CSV file for target date
    String logPath = "/logs_" + dateStr + ".csv";
    if (LittleFS.exists(logPath)) {
      LittleFS.remove(logPath);
    }
    
    // Delete EEPROM logs for target date
    if (useEEPROM) {
      deleteEEPROMLogsForDate(dateStr);
    }
    
    updateFsSizes();
    request->send(200, "text/plain", "OK");
  });

  server.on("/delete-all-logs", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (!isAuth(request)) return request->send(403, "text/plain", "Unauthorized");
    Serial.println("🗑 Erasing ALL history logs...");
    
    // Collect all log files
    std::vector<String> logFiles;
    File root = LittleFS.open("/");
    if (root && root.isDirectory()) {
      File file = root.openNextFile();
      while (file) {
        String fn = String(file.name());
        if (fn.startsWith("logs_") || fn.startsWith("/logs_")) {
          if (!fn.startsWith("/")) fn = "/" + fn;
          logFiles.push_back(fn);
        }
        file = root.openNextFile();
      }
      root.close();
    }
    
    // Delete collected log files
    for (size_t i = 0; i < logFiles.size(); i++) {
      LittleFS.remove(logFiles[i]);
    }
    
    // Clear pending queue & transferred logs
    LittleFS.remove("/pending_sync.txt");
    LittleFS.remove("/sql_transferred.txt");
    sqlPendingLogs = 0;
    sqlTransferredLogs = 0;
    sqlTotalLogs = 0;
    
    // Clear EEPROM logger
    clearAllEEPROMLogs();
    
    updateFsSizes();
    ws.textAll("{\"type\":\"SQL_SYNC\",\"transferred\":0,\"pending\":0}");
    request->send(200, "text/plain", "OK");
  });

  server.on("/get-sql-history", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (!isAuth(request)) return request->send(403, "text/plain", "Unauthorized");
    if (!request->hasParam("date")) return request->send(400);
    String dateStr = request->getParam("date")->value();
    
    // If sqlApiUrl is configured, proxy request to sql_bridge Python server
    if (sqlApiUrl.length() > 0) {
      String bridgeUrl = sqlApiUrl;
      int protoIdx = bridgeUrl.indexOf("//");
      int slashIdx = bridgeUrl.indexOf('/', (protoIdx != -1) ? protoIdx + 2 : 0);
      if (slashIdx != -1) bridgeUrl = bridgeUrl.substring(0, slashIdx);
      bridgeUrl += "/get-logs?date=" + dateStr;
      
      HTTPClient http;
      http.begin(bridgeUrl);
      http.setTimeout(2000);
      int httpCode = http.GET();
      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        http.end();
        return request->send(200, "text/csv", payload);
      }
      http.end();
    }
    
    // Fallback to local LittleFS log history
    String logPath = "/logs_" + dateStr + ".csv";
    if (LittleFS.exists(logPath)) {
      request->send(LittleFS, logPath, "text/csv");
    } else {
      request->send(200, "text/plain", "");
    }
  });

  server.on("/manual-punch", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (!isAuth(request)) return request->send(403, "text/plain", "Unauthorized");
    if (!request->hasParam("uid") || !request->hasParam("dir")) {
      return request->send(400, "text/plain", "Missing parameters");
    }
    
    String uid = request->getParam("uid")->value();
    String dir = request->getParam("dir")->value();
    
    // Validate direction
    if (dir != "In" && dir != "Out") {
      return request->send(400, "text/plain", "Invalid direction");
    }
    
    // Check if user exists
    String path = "/" + uid + ".txt";
    if (!LittleFS.exists(path)) {
      path = "/" + uid + "_f.txt";
    }
    
    if (!LittleFS.exists(path)) {
      return request->send(404, "text/plain", "User not found");
    }
    
    String t = getNow();
    String method = "Manual";
    String name = "Unknown", role = "-", roll = uid;
    
    File f = LittleFS.open(path, FILE_READ);
    if (!f) {
      return request->send(500, "text/plain", "File error");
    }
    
    String c = f.readString();
    f.close();
    
    // Parse user data
    int pipes[6];
    int pCount = 0;
    int lastPos = -1;
    
    for (int i = 0; i < 6; i++) {
      pipes[i] = c.indexOf('|', lastPos + 1);
      if (pipes[i] != -1) {
        lastPos = pipes[i];
        pCount++;
      } else {
        pipes[i] = -1;
      }
    }
    
    if (pCount < 3) {
      return request->send(500, "text/plain", "Corrupted record");
    }
    
    name = c.substring(0, pipes[0]);
    role = c.substring(pipes[0] + 1, pipes[1]);
    roll = c.substring(pipes[1] + 1, pipes[2]);
    
    String stateStr = (pCount >= 4) ? c.substring(pipes[2] + 1, pipes[3]) : "0";
    String today = getTodayDate();
    String firstTime = (pCount >= 5) ? c.substring(pipes[3] + 1, pipes[4]) : "-";
    String lastTime = (pCount >= 6) ? c.substring(pipes[4] + 1) : "-";
    
    int punchesToday = stateStr.toInt();
    
    // Update punch count based on direction
    if (dir == "In") {
      punchesToday = 1;
      firstTime = t;
    } else {
      punchesToday = 2;
      lastTime = t;
    }

    // Write updated record
    File fw = LittleFS.open(path, FILE_WRITE);
    if (fw) {
      fw.print(name + "|" + role + "|" + roll + "|" + String(punchesToday) + 
               "|" + today + "|" + firstTime + "|" + lastTime);
      fw.close();
    }
    
    // Log the entry
    bool eepromFull = (eepromTotalLogs >= MAX_EEPROM_LOGS);
    if (useEEPROM && !eepromFull) {
      EEPROMLogEntry entry;
      time_t nowTime;
      time(&nowTime);
      entry.timestamp = nowTime;
      
      memset(entry.uid, 0, sizeof(entry.uid));
      strncpy(entry.uid, uid.c_str(), sizeof(entry.uid) - 1);
      
      entry.direction = (dir == "In") ? 1 : 2;
      entry.status = 4; // Accepted
      entry.method = 3; // Manual
      
      addEEPROMLog(entry);
    } else {
      File logFile = LittleFS.open(getTodayFileName(), FILE_APPEND);
      if (!logFile) {
        logFile = LittleFS.open(getTodayFileName(), FILE_WRITE);
      }
      
      if (logFile) {
        if (logFile.size() == 0) {
          logFile.println("Time,ID,Name,Role,Direction,Status,Type");
        }
        logFile.println(t + "," + roll + "," + name + "," + role + "," + 
                        dir + ",Accepted," + method);
        logFile.close();
      }
    }
    
    // Broadcast via WebSocket
    String wsData = "{\"type\":\"SCAN\",\"uid\":\"" + uid + "\",\"name\":\"" + 
                    name + "\",\"role\":\"" + role + "\",\"dir\":\"" + dir + 
                    "\",\"status\":\"Accepted\"}";
    ws.textAll(wsData);
    sendWebhook(wsData);

    extern void queueSqlSync(String id, String name, String role, String dir, String status, String type);
    queueSqlSync(uid, name, role, dir, "Accepted", method);
    
    request->send(200, "text/plain", "OK");
  });

  server.on("/set-branding", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (!isAPMode) return request->send(403, "text/plain", "Forbidden: AP Mode Required");
    if (request->hasParam("name")) {
      deviceName = request->getParam("name")->value();
    }
    if (request->hasParam("webhook")) {
      webhookUrl = request->getParam("webhook")->value();
    }
    if (request->hasParam("tftPromptEnabled")) {
      String val = request->getParam("tftPromptEnabled")->value();
      tftPromptEnabled = (val == "1" || val == "true");
    }
    if (request->hasParam("twoFactorEnabled")) {
      String val = request->getParam("twoFactorEnabled")->value();
      twoFactorEnabled = (val == "1" || val == "true");
    }
    if (request->hasParam("googleApiKey")) {
      googleApiKey = request->getParam("googleApiKey")->value();
      googleApiKey.trim();
      File fk = LittleFS.open("/google_key.txt", FILE_WRITE);
      if (fk) {
        fk.print(googleApiKey);
        fk.close();
      }
    }
    
    // Save to storage
    File f = LittleFS.open("/config.sys", FILE_WRITE);
    if (f) {
      f.println(deviceName);
      f.println(webhookUrl);
      f.println(tftPromptEnabled ? "1" : "0");
      f.println(twoFactorEnabled ? "1" : "0");
      f.close();
      onBrandingUpdated();
      request->send(200, "text/plain", "OK");
    } else {
      request->send(500, "text/plain", "Write error");
    }
  });

  server.on("/set-weather-city", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (!isAuth(request)) return request->send(403, "text/plain", "Unauthorized");
    if (request->hasParam("city")) {
      weatherCity = request->getParam("city")->value();
      weatherCity.replace(" ", "-");
      weatherCity.trim();
      
      File f = LittleFS.open("/weather_city.txt", FILE_WRITE);
      if (f) {
        f.print(weatherCity);
        f.close();
      }
      Serial.println("Weather city updated to: " + weatherCity);
      
      if (weatherTaskHandle != NULL) {
        xTaskNotifyGive(weatherTaskHandle); // Wake up task to fetch weather immediately
      }
    }
    request->send(200, "text/plain", "OK");
  });

  server.on("/export-users", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (!isAuth(request)) return request->send(403, "text/plain", "Unauthorized");
    AsyncResponseStream *response = request->beginResponseStream("application/json");
    response->print("[");
    
    File root = LittleFS.open("/");
    File file = root.openNextFile();
    bool first = true;
    
    while (file) {
      String fn = file.name();
      // Only export employee profile files, excluding system configs and weather files
      if (fn.endsWith(".txt") && fn != "/wifi.txt" && fn != "/wifi.conf" && 
          fn != "/config.sys" && fn != "wifi.txt" && fn != "wifi.conf" && 
          fn != "config.sys" && fn != "/weather_city.txt" && fn != "weather_city.txt" && 
          !fn.startsWith("/logs_") && !fn.startsWith("logs_")) {
        if (!first) {
          response->print(",");
        }
        first = false;
        
        String content = file.readString();
        // Fully escape backslashes, quotes, newlines, carriage returns, and tabs to guarantee valid JSON string
        content.replace("\\", "\\\\");
        content.replace("\"", "\\\"");
        content.replace("\n", "\\n");
        content.replace("\r", "\\r");
        content.replace("\t", "\\t");
        
        String cleanFn = fn;
        cleanFn.replace("\\", "\\\\");
        cleanFn.replace("\"", "\\\"");
        
        response->printf("{\"fn\":\"%s\",\"data\":\"%s\"}", cleanFn.c_str(), content.c_str());
      }
      file = root.openNextFile();
    }
    
    response->print("]");
    request->send(response);
  });

  server.on("/restore-user", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (!isAuth(request)) return request->send(403, "text/plain", "Unauthorized");
    if (!request->hasParam("fn") || !request->hasParam("data")) {
      return request->send(400, "text/plain", "Missing parameters");
    }
    
    String fn = request->getParam("fn")->value();
    String data = request->getParam("data")->value();
    
    if (!fn.startsWith("/")) {
      fn = "/" + fn;
    }
    
    // Security check - prevent path traversal
    if (fn.indexOf("..") != -1) {
      return request->send(400, "text/plain", "Invalid filename");
    }
    
    File f = LittleFS.open(fn, FILE_WRITE);
    if (f) {
      f.print(data);
      f.close();
      updateFsSizes();
      request->send(200, "text/plain", "OK");
    } else {
      request->send(500, "text/plain", "Write error");
    }
  });

  server.on("/sensor-status", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (!isAuth(request)) return request->send(403, "text/plain", "Unauthorized");
    String json = "{";
    
    bool fpOk = false;
    if (xSemaphoreTake(fpMutex, pdMS_TO_TICKS(500))) {
      fpOk = (finger.verifyPassword());
      xSemaphoreGive(fpMutex);
    }
    
    json += "\"fingerprint\":" + String(fpOk ? "true" : "false") + ",";
    
    // Check RFID sensor
    byte v = rfid.PCD_ReadRegister(rfid.VersionReg);
    bool rfidOk = (v != 0x00 && v != 0xFF);
    json += "\"rfid\":" + String(rfidOk ? "true" : "false") + ",";
    
    // Audio driver removed, status reports false
    json += "\"audio\":false";
    
    json += "}";
    request->send(200, "application/json", json);
  });

  server.on("/list-holidays", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (!isAuth(request)) return request->send(403, "text/plain", "Unauthorized");
    if (LittleFS.exists("/holidays.json")) {
      request->send(LittleFS, "/holidays.json", "application/json");
    } else {
      request->send(200, "application/json", "[]");
    }
  });

  server.on("/add-holiday", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (!isAuth(request)) return request->send(403, "text/plain", "Unauthorized");
    if (!request->hasParam("date") || !request->hasParam("name")) {
      return request->send(400, "text/plain", "Missing parameters");
    }
    
    String d = request->getParam("date")->value();
    String n = request->getParam("name")->value();
    String t = request->hasParam("type") ? request->getParam("type")->value() : "Public";
    String y = request->hasParam("yearly") ? request->getParam("yearly")->value() : "false";
    String r = request->hasParam("isRange") ? request->getParam("isRange")->value() : "false";
    String ed = request->hasParam("endDate") ? request->getParam("endDate")->value() : "";
    String desc = request->hasParam("desc") ? request->getParam("desc")->value() : "";
    
    String json = "[]";
    
    // Load existing holidays
    if (LittleFS.exists("/holidays.json")) {
      File f = LittleFS.open("/holidays.json", FILE_READ);
      if (f) {
        json = f.readString();
        f.close();
      }
    }
    
    // Construct new holiday item JSON
    String item = "{\"date\":\"" + d + 
                   "\",\"name\":\"" + n + 
                   "\",\"type\":\"" + t + 
                   "\",\"yearly\":" + y + 
                   ",\"isRange\":" + r + 
                   ",\"endDate\":\"" + ed + 
                   "\",\"desc\":\"" + desc + "\"}";
    
    // Add new holiday
    if (json == "[]" || json.length() == 0) {
      json = "[" + item + "]";
    } else {
      json.remove(json.length() - 1); // Remove closing bracket
      json += "," + item + "]";
    }
    
    // Write to file
    File fw = LittleFS.open("/holidays.json", FILE_WRITE);
    if (fw) {
      fw.print(json);
      fw.close();
      request->send(200, "text/plain", "OK");
    } else {
      request->send(500, "text/plain", "Write error");
    }
  });

  server.on("/clear-holidays", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (!isAuth(request)) return request->send(403, "text/plain", "Unauthorized");
    File fw = LittleFS.open("/holidays.json", FILE_WRITE);
    if (fw) {
      fw.print("[]");
      fw.close();
      request->send(200, "text/plain", "OK");
    } else {
      request->send(500, "text/plain", "Write error");
    }
  });


  server.on("/delete-holiday", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (!isAuth(request)) return request->send(403, "text/plain", "Unauthorized");
    if (!request->hasParam("date")) {
      return request->send(400, "text/plain", "Missing date parameter");
    }
    
    String d = request->getParam("date")->value();
    
    if (!LittleFS.exists("/holidays.json")) {
      return request->send(200, "text/plain", "OK");
    }
    
    File f = LittleFS.open("/holidays.json", FILE_READ);
    String content = f.readString();
    f.close();
    
    // Find and remove holiday entry
    String searchStr = "{\"date\":\"" + d;
    int idx = content.indexOf(searchStr);
    
    if (idx != -1) {
      int endIdx = content.indexOf("}", idx);
      if (endIdx != -1) {
        // Handle comma after entry
        if (content[endIdx + 1] == ',') {
          endIdx++;
        }
        // Handle comma before entry
        else if (idx > 0 && content[idx - 1] == ',') {
          idx--;
        }
        
        content.remove(idx, endIdx - idx + 1);
        
        // Clean up if array is now empty or malformed
        if (content == "[,]" || content == "[]") {
          content = "[]";
        }
        
        // Write updated content
        File fw = LittleFS.open("/holidays.json", FILE_WRITE);
        if (fw) {
          fw.print(content);
          fw.close();
        }
      }
    }
    
    request->send(200, "text/plain", "OK");
  });

  server.on("/get-shift", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (!isAuth(request)) return request->send(403, "text/plain", "Unauthorized");
    String json = "{";
    json += "\"startHour\":" + String(shiftStartHour) + ",";
    json += "\"startMin\":" + String(shiftStartMin) + ",";
    json += "\"endHour\":" + String(shiftEndHour) + ",";
    json += "\"endMin\":" + String(shiftEndMin) + ",";
    json += "\"graceMins\":" + String(graceMins);
    json += "}";
    request->send(200, "application/json", json);
  });

  server.on("/set-shift", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (!isAuth(request)) return request->send(403, "text/plain", "Unauthorized");
    if (request->hasParam("startHour")) shiftStartHour = request->getParam("startHour")->value().toInt();
    if (request->hasParam("startMin")) shiftStartMin = request->getParam("startMin")->value().toInt();
    if (request->hasParam("endHour")) shiftEndHour = request->getParam("endHour")->value().toInt();
    if (request->hasParam("endMin")) shiftEndMin = request->getParam("endMin")->value().toInt();
    if (request->hasParam("grace")) graceMins = request->getParam("grace")->value().toInt();
    saveShiftConfig();
    request->send(200, "text/plain", "OK");
  });

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", index_html_gz, index_html_gz_len);
    response->addHeader("Content-Encoding", "gzip");
    response->addHeader("Cache-Control", "no-cache, no-store, must-revalidate");
    response->addHeader("Pragma", "no-cache");
    response->addHeader("Expires", "-1");
    request->send(response);
  });

  // OTA (Over-The-Air) Update Pre-Erase Route
  server.on(
      "/update-start", HTTP_POST,
      [](AsyncWebServerRequest *request) {
        extern volatile bool isOtaUpdating;
        extern volatile unsigned long lastOtaChunkTime;
        extern void disableWatchdog();

        size_t updateSize = UPDATE_SIZE_UNKNOWN;
        if (request->hasHeader("X-File-Size")) {
          updateSize = request->header("X-File-Size").toInt();
        } else if (request->hasHeader("x-file-size")) {
          updateSize = request->header("x-file-size").toInt();
        } else if (request->hasParam("size")) {
          updateSize = request->getParam("size")->value().toInt();
        } else if (request->hasArg("size")) {
          updateSize = request->arg("size").toInt();
        }

        if (updateSize == 0 || updateSize == UPDATE_SIZE_UNKNOWN) {
          Serial.println("❌ OTA Pre-erase failed: Invalid/Missing size parameter!");
          request->send(400, "text/plain", "FAIL: Invalid Size");
          return;
        }

        Serial.printf("📦 OTA Pre-erase Start. Size: %u bytes\n", updateSize);
        isOtaUpdating = true;
        lastOtaChunkTime = millis();
        disableWatchdog();

        // Clear any previous stale/failed state before starting a new update
        Update.abort();

        Serial.println("⏳ Erasing flash partition... Please wait.");
        if (!Update.begin(updateSize, U_FLASH)) {
          Serial.println("❌ OTA begin failed during pre-erase!");
          Update.printError(Serial);
          String errMsg = "FAIL: Erase Error (Code: " + String(Update.getError()) + " - " + String(Update.errorString()) + ")";
          Update.abort();
          isOtaUpdating = false;
          request->send(500, "text/plain", errMsg);
        } else {
          Serial.println("🚀 OTA pre-erase successful. Flash is ready for upload!");
          request->send(200, "text/plain", "OK");
        }
      });

  // OTA (Over-The-Air) Update Route
  server.on(
      "/update", HTTP_POST,
      [](AsyncWebServerRequest *request) {
        bool shouldReboot = !Update.hasError();
        String resMsg;
        if (shouldReboot) {
          resMsg = "OK";
        } else {
          resMsg = "FAIL: Update Error (Code: " + String(Update.getError()) + " - " + String(Update.errorString()) + ")";
        }
        AsyncWebServerResponse *response = request->beginResponse(
            200, "text/plain", resMsg);
        response->addHeader("Connection", "close");
        request->send(response);
        
        if (shouldReboot) {
          Serial.println("✓ Update successful, rebooting...");
          restartTime = millis() + 1000;
        } else {
          Serial.println("✗ Update failed");
          Update.abort();
          isOtaUpdating = false;
        }
      },
      [](AsyncWebServerRequest *request, String filename, size_t index,
         uint8_t *data, size_t len, bool final) {
        extern volatile unsigned long lastOtaChunkTime;
        lastOtaChunkTime = millis(); // update chunk timestamp

        if (!index) {
          // If we didn't pre-initialize, initialize now
          if (!isOtaUpdating) {
            isOtaUpdating = true;
            Serial.printf("📦 OTA Update Start (No pre-erase): %s\n", filename.c_str());
            
            size_t updateSize = UPDATE_SIZE_UNKNOWN;
            if (request->hasHeader("X-File-Size")) {
              updateSize = request->header("X-File-Size").toInt();
              Serial.printf("   Exact size from HTTP Header (X-File-Size): %u bytes\n", updateSize);
            } else if (request->hasHeader("x-file-size")) {
              updateSize = request->header("x-file-size").toInt();
              Serial.printf("   Exact size from HTTP Header (x-file-size): %u bytes\n", updateSize);
            } else if (request->hasParam("size")) {
              updateSize = request->getParam("size")->value().toInt();
              Serial.printf("   Exact size from URL Param: %u bytes\n", updateSize);
            } else if (request->hasArg("size")) {
              updateSize = request->arg("size").toInt();
              Serial.printf("   Exact size from URL Arg: %u bytes\n", updateSize);
            }

            if (updateSize == 0) {
              Serial.println("   Parsed size is 0, falling back to UPDATE_SIZE_UNKNOWN to prevent failure.");
              updateSize = UPDATE_SIZE_UNKNOWN;
            }

            if (updateSize == UPDATE_SIZE_UNKNOWN) {
              Serial.println("   No size param or header found in request. Falling back to full erase...");
            }

            extern void disableWatchdog();
            disableWatchdog();

            Update.abort(); // Clear any previous failed state before beginning

            if (!Update.begin(updateSize, U_FLASH)) {
              Serial.println("❌ OTA begin failed!");
              Update.printError(Serial);
              Update.abort();
              isOtaUpdating = false;
              if (request->client()) request->client()->close(); // Abort TCP connection immediately
              return;
            } else {
              Serial.println("🚀 OTA begin successful. Erase complete!");
            }
          } else {
            Serial.printf("📦 OTA File Upload Start (Pre-erased): %s\n", filename.c_str());
          }
        }
        
        if (!Update.hasError() && len > 0) {
          if (Update.write(data, len) != len) {
            Serial.println("✗ OTA write error");
            Update.printError(Serial);
            Update.abort();
            isOtaUpdating = false;
            if (request->client()) request->client()->close(); // Abort TCP connection immediately
            return;
          }
        }
        
        if (final) {
          if (Update.end(true)) {
            Serial.printf("✓ Update complete: %uB written\n", index + len);
          } else {
            Serial.println("✗ OTA end error");
            Update.printError(Serial);
            Update.abort();
            isOtaUpdating = false;
            if (request->client()) request->client()->close(); // Abort TCP connection immediately
          }
        }
        
        yield();
      });
  // General File Upload Route (for WAV audio and custom config files)
  static File uploadFile;
  server.on("/upload", HTTP_POST, [](AsyncWebServerRequest *request) {
    if (!isAuth(request)) return request->send(403, "text/plain", "Unauthorized");
    request->send(200, "text/plain", "OK");
  }, [](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
    if (!isAuth(request)) return;
    if (!index) {
      if (!filename.startsWith("/")) filename = "/" + filename;
      uploadFile = LittleFS.open(filename, FILE_WRITE);
      Serial.printf("📥 Upload Started: %s\n", filename.c_str());
    }
    if (uploadFile) {
      uploadFile.write(data, len);
    }
    if (final) {
      if (uploadFile) {
        uploadFile.close();
        Serial.println("✓ Upload Complete!");
      }
    }
  });

  server.on("/apmode", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (isAPMode) {
      request->redirect("/AP");
    } else {
      isAPMode = true;
      apModeStartTime = millis();
      
      WiFi.mode(WIFI_AP_STA);
      IPAddress apIP(192, 168, 4, 1);
      WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
      WiFi.softAP("Anurag_0.1_AP", "admin123");
      dnsServer.start(53, "*", apIP);
      
      drawTftDefaultScreen();
      
      String html = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>AP Mode Activated</title>
<style>
  body {
    background: #090b10;
    color: #e6edf3;
    font-family: 'Segoe UI', Arial, sans-serif;
    display: flex;
    justify-content: center;
    align-items: center;
    min-height: 100vh;
    margin: 0;
    padding: 20px;
  }
  .card {
    background: #161b22;
    border: 1px solid #30363d;
    border-radius: 16px;
    padding: 30px;
    max-width: 500px;
    width: 100%;
    box-shadow: 0 8px 24px rgba(0,0,0,0.5);
    text-align: center;
  }
  h1 {
    color: #58a6ff;
    font-size: 24px;
    margin-top: 0;
  }
  p {
    color: #8b949e;
    font-size: 15px;
    line-height: 1.6;
  }
  .highlight {
    background: rgba(88, 166, 255, 0.1);
    border: 1px solid rgba(88, 166, 255, 0.2);
    border-radius: 8px;
    padding: 15px;
    margin: 20px 0;
    text-align: left;
  }
  .highlight div {
    margin-bottom: 8px;
    font-size: 14px;
  }
  .highlight div:last-child {
    margin-bottom: 0;
  }
  .highlight strong {
    color: #ffffff;
  }
  .timer {
    font-size: 32px;
    font-weight: 700;
    color: #ffd166;
    margin: 20px 0;
  }
  .btn {
    display: inline-block;
    background: #238636;
    color: #ffffff;
    padding: 12px 24px;
    border-radius: 8px;
    text-decoration: none;
    font-weight: 600;
    margin-top: 10px;
    transition: background 0.2s;
  }
  .btn:hover {
    background: #2ea043;
  }
</style>
</head>
<body>
<div class="card">
  <h1>✨ AP Mode Temporarily Active</h1>
  <p>To configure security-sensitive features, please connect to the device's local Access Point.</p>
  
  <div class="highlight">
    <div>📶 <strong>SSID:</strong> Anurag_0.1_AP</div>
    <div>🔑 <strong>Password:</strong> admin123</div>
    <div>🌐 <strong>Portal URL:</strong> <a href="http://192.168.4.1/AP" style="color: #58a6ff; text-decoration: none;">http://192.168.4.1/AP</a></div>
  </div>
  
  <p>AP mode will automatically disable, returning to Station mode in:</p>
  <div class="timer" id="countdown">5:00</div>
  
  <p style="font-size: 12px; color: #8b949e;">Once connected to <strong>Anurag_0.1_AP</strong>, open <strong>http://192.168.4.1/AP</strong> to configure Change Admin Credentials and Premium Features.</p>
</div>

<script>
  var timeLeft = 300; // 5 minutes in seconds
  var timerEl = document.getElementById('countdown');
  var interval = setInterval(function() {
    timeLeft--;
    if (timeLeft <= 0) {
      clearInterval(interval);
      timerEl.innerText = "0:00";
      timerEl.style.color = "#ef476f";
    } else {
      var m = Math.floor(timeLeft / 60);
      var s = timeLeft % 60;
      timerEl.innerText = m + ":" + (s < 10 ? "0" : "") + s;
    }
  }, 1000);
</script>
</body>
</html>
)rawliteral";
      request->send(200, "text/html", html);
    }
  });

  server.on("/AP", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (!isAPMode) return request->send(403, "text/plain", "Forbidden: AP Mode Required");
    extern const uint8_t ap_html_gz[];
    extern const uint32_t ap_html_gz_len;
    AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", ap_html_gz, ap_html_gz_len);
    response->addHeader("Content-Encoding", "gzip");
    response->addHeader("Cache-Control", "no-cache, no-store, must-revalidate");
    response->addHeader("Pragma", "no-cache");
    response->addHeader("Expires", "-1");
    request->send(response);
  });

  server.on("/ap", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->redirect("/AP");
  });

  server.on("/deactivate-apmode", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "OK");
    
    isAPMode = false;
    apModeStartTime = 0;
    
    WiFi.softAPdisconnect(true);
    dnsServer.stop();
    WiFi.mode(WIFI_STA);
    if (WiFi.status() != WL_CONNECTED) {
      WiFi.begin();
    }
    
    drawTftDefaultScreen();
  });

  server.on("/set-sql-config", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (!isAPMode && !isAuth(request)) return request->send(403, "text/plain", "Unauthorized");
    if (request->hasParam("url")) {
      sqlApiUrl = request->getParam("url")->value();
      sqlApiUrl.trim();
      File f = LittleFS.open("/sql_config.txt", FILE_WRITE);
      if (f) {
        f.print(sqlApiUrl);
        f.close();
      }
      Serial.println("SQL API URL updated to: " + sqlApiUrl);
      extern void performSqlSync();
      performSqlSync();
      request->send(200, "text/plain", "OK");
    } else {
      request->send(400, "text/plain", "Missing URL");
    }
  });

  server.on("/get-sql-history", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (!isAuth(request)) return request->send(403, "text/plain", "Unauthorized");
    if (sqlApiUrl.length() < 10) {
      return request->send(400, "text/plain", "SQL URL not configured");
    }
    
    String baseUrl = sqlApiUrl;
    int protoIdx = baseUrl.indexOf("//");
    int slashIdx = baseUrl.indexOf('/', (protoIdx != -1) ? protoIdx + 2 : 0);
    if (slashIdx != -1) {
      baseUrl = baseUrl.substring(0, slashIdx);
    }
    String url = baseUrl + "/get-logs";
    bool first = true;
    if (request->hasParam("date")) {
      url += (first ? "?" : "&"); first = false;
      url += "date=" + request->getParam("date")->value();
    }
    if (request->hasParam("filter")) {
      url += (first ? "?" : "&"); first = false;
      url += "filter=" + request->getParam("filter")->value();
    }
    if (request->hasParam("role")) {
      url += (first ? "?" : "&"); first = false;
      url += "role=" + request->getParam("role")->value();
    }
    
    HTTPClient http;
    http.begin(url);
    http.setTimeout(10000);
    int code = http.GET();
    if (code == 200) {
      String payload = http.getString();
      http.end();
      request->send(200, "text/plain", payload);
    } else {
      http.end();
      request->send(502, "text/plain", "SQL Bridge unreachable or returned error " + String(code));
    }
  });

  server.on("/sync-history-all", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (!isAuth(request)) return request->send(403, "text/plain", "Unauthorized");
    
    xTaskCreate([](void *p) {
      if (useEEPROM) {
        uint16_t logsToProcess = eepromTotalLogs;
        uint16_t readIdx = (eepromNextIndex - eepromTotalLogs + MAX_EEPROM_LOGS) % MAX_EEPROM_LOGS;
        for (uint16_t i = 0; i < logsToProcess; i++) {
          EEPROMLogEntry entry;
          readEEPROMLog(readIdx, entry);
          readIdx = (readIdx + 1) % MAX_EEPROM_LOGS;
          
          String uidStr = String(entry.uid);
          String name, role, roll;
          lookupUserProfile(uidStr, name, role, roll);
          String dirStr = (entry.direction == 1) ? "In" : "Out";
          String statusStr = "Accepted";
          if (entry.status == 1) statusStr = "On-Time";
          else if (entry.status == 2) statusStr = "Late";
          else if (entry.status == 3) statusStr = "Early Exit";
          else if (entry.status == 4) statusStr = "Accepted";
          else if (entry.status == 5) statusStr = "Denied";
          
          String methodStr = "RFID Card";
          if (entry.method == 1) methodStr = "Fingerprint";
          else if (entry.method == 2) methodStr = "RFID Card";
          else if (entry.method == 3) methodStr = "Manual";
          
          File fsync = LittleFS.open("/pending_sync.txt", FILE_APPEND);
          if (fsync) {
            fsync.println(String(entry.timestamp) + "|" + uidStr + "|" + name + "|" + role + "|" + dirStr + "|" + statusStr + "|" + methodStr);
            fsync.close();
            sqlPendingLogs++;
          }
          vTaskDelay(pdMS_TO_TICKS(1));
        }
      } else {
        File root = LittleFS.open("/");
        File file = root.openNextFile();
        while (file) {
          String fn = file.name();
          if (fn.startsWith("/")) fn = fn.substring(1);
          if (fn.startsWith("logs_") && fn.endsWith(".csv")) {
            String c = file.readString();
            int start = c.indexOf('\n');
            while (start != -1 && start < c.length()) {
              int next = c.indexOf('\n', start + 1);
              String line = (next == -1) ? c.substring(start + 1) : c.substring(start + 1, next);
              line.trim();
              if (line.length() > 0) {
                int commas[6];
                int cIdx = 0;
                int lastPos = -1;
                for (int j = 0; j < 6; j++) {
                  commas[j] = line.indexOf(',', lastPos + 1);
                  if (commas[j] != -1) {
                    lastPos = commas[j];
                    cIdx++;
                  } else {
                    commas[j] = -1;
                  }
                }
                
                if (cIdx >= 5) {
                  String timeStr = line.substring(0, commas[0]);
                  String id = line.substring(commas[0] + 1, commas[1]);
                  String name = line.substring(commas[1] + 1, commas[2]);
                  String role = line.substring(commas[2] + 1, commas[3]);
                  String direction = line.substring(commas[3] + 1, commas[4]);
                  String status = line.substring(commas[4] + 1, commas[5]);
                  String type = line.substring(commas[5]);
                  
                  time_t dummyTime = 0;
                  
                  File fsync = LittleFS.open("/pending_sync.txt", FILE_APPEND);
                  if (fsync) {
                    fsync.println(String(dummyTime) + "|" + id + "|" + name + "|" + role + "|" + direction + "|" + status + "|" + type);
                    fsync.close();
                    sqlPendingLogs++;
                  }
                }
              }
              start = next;
            }
          }
          file.close();
          file = root.openNextFile();
          vTaskDelay(pdMS_TO_TICKS(1));
        }
      }
      sqlTotalLogs = sqlTransferredLogs + sqlPendingLogs;
      ws.textAll("{\"type\":\"SQL_SYNC\",\"transferred\":" + String(sqlTransferredLogs) + 
                ",\"pending\":" + String(sqlPendingLogs) + "}");
      extern void performSqlSync();
      performSqlSync();
      vTaskDelete(NULL);
    }, "sync_all_task", 4096, NULL, 1, NULL);
    
    request->send(200, "text/plain", "Sync started");
  });

  // Captive Portal Redirect
  server.onNotFound([](AsyncWebServerRequest *request) {
    if (isAPMode) {
      request->redirect("http://192.168.4.1/");
    } else {
      request->send(404, "text/plain", "Not found");
    }
  });

  server.addHandler(&ws);
  server.begin();
  Serial.println("Server Started with Login System.");
}

