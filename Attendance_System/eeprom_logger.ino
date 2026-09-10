void eeprom_write_buffer(uint16_t reg, const uint8_t* data, uint16_t len);
void eeprom_read_buffer(uint16_t reg, uint8_t* data, uint16_t len);
void loadEEPROMHeader();
void saveEEPROMHeader();
void clearAllEEPROMLogs();
void writeEEPROMLog(uint16_t logIdx, const EEPROMLogEntry& entry);
void readEEPROMLog(uint16_t logIdx, EEPROMLogEntry& entry);
void addEEPROMLog(const EEPROMLogEntry& entry);
void deleteEEPROMLogsForDate(String targetDate);
void lookupUserProfile(const String& uid, String& name, String& role, String& roll);
#include "eeprom_logger.h"
#include <LittleFS.h>
#include <esp_task_wdt.h>

// Global variable definitions
bool useEEPROM = false;
uint16_t eepromTotalLogs = 0;
uint16_t eepromNextIndex = 0;

void eeprom_write_buffer(uint16_t reg, const uint8_t* data, uint16_t len) {
  uint16_t bytesWritten = 0;
  while (bytesWritten < len) {
    uint16_t currentReg = reg + bytesWritten;
    uint8_t pageOffset = currentReg % EEPROM_PAGE_SIZE;
    uint8_t bytesToPageBoundary = EEPROM_PAGE_SIZE - pageOffset;
    uint8_t chunkLen = min((uint16_t)bytesToPageBoundary, (uint16_t)(len - bytesWritten));
    
    // Wire library limit is 128 bytes on ESP32, but standard safe limit is 32 bytes for I2C transmissions
    chunkLen = min(chunkLen, (uint8_t)32);
    
    Wire.beginTransmission(EEPROM_ADDR);
    Wire.write((uint8_t)(currentReg >> 8));   // MSB
    Wire.write((uint8_t)(currentReg & 0xFF)); // LSB
    for (uint8_t i = 0; i < chunkLen; i++) {
      Wire.write(data[bytesWritten + i]);
    }
    Wire.endTransmission();
    delay(5); // Physical EEPROM write cycle delay (5ms)
    
    bytesWritten += chunkLen;
  }
}

void eeprom_read_buffer(uint16_t reg, uint8_t* data, uint16_t len) {
  uint16_t bytesRead = 0;
  while (bytesRead < len) {
    uint16_t currentReg = reg + bytesRead;
    uint8_t chunkLen = min((uint16_t)32, (uint16_t)(len - bytesRead)); // Read in 32-byte chunks
    
    Wire.beginTransmission(EEPROM_ADDR);
    Wire.write((uint8_t)(currentReg >> 8));   // MSB
    Wire.write((uint8_t)(currentReg & 0xFF)); // LSB
    Wire.endTransmission();
    
    Wire.requestFrom(EEPROM_ADDR, (int)chunkLen);
    for (uint8_t i = 0; i < chunkLen; i++) {
      if (Wire.available()) {
        data[bytesRead + i] = Wire.read();
      } else {
        data[bytesRead + i] = 0xFF; // Fallback
      }
    }
    bytesRead += chunkLen;
  }
}

void loadEEPROMHeader() {
  uint8_t header[8];
  eeprom_read_buffer(0, header, 8);
  if (header[0] == 0xDE && header[1] == 0xCA && header[2] == 0x51 && header[3] == 0x12) {
    eepromTotalLogs = (header[4] << 8) | header[5];
    eepromNextIndex = (header[6] << 8) | header[7];
    
    // Safety boundaries validation
    if (eepromTotalLogs > MAX_EEPROM_LOGS) eepromTotalLogs = 0;
    if (eepromNextIndex > MAX_EEPROM_LOGS) eepromNextIndex = 0;
    
    Serial.printf("✓ EEPROM Header loaded. Logs stored: %d, Next index: %d\n", eepromTotalLogs, eepromNextIndex);
  } else {
    Serial.println("⚠ EEPROM not initialized. Formatting header...");
    clearAllEEPROMLogs();
  }
}

void saveEEPROMHeader() {
  uint8_t header[8];
  header[0] = 0xDE;
  header[1] = 0xCA;
  header[2] = 0x51;
  header[3] = 0x12;
  header[4] = (uint8_t)(eepromTotalLogs >> 8);
  header[5] = (uint8_t)(eepromTotalLogs & 0xFF);
  header[6] = (uint8_t)(eepromNextIndex >> 8);
  header[7] = (uint8_t)(eepromNextIndex & 0xFF);
  eeprom_write_buffer(0, header, 8);
}

void clearAllEEPROMLogs() {
  eepromTotalLogs = 0;
  eepromNextIndex = 0;
  saveEEPROMHeader();
  Serial.println("✓ EEPROM logs cleared.");
}

void writeEEPROMLog(uint16_t logIdx, const EEPROMLogEntry& entry) {
  if (logIdx >= MAX_EEPROM_LOGS) return;
  uint16_t regAddress = EEPROM_LOG_START + (logIdx * sizeof(EEPROMLogEntry));
  eeprom_write_buffer(regAddress, (const uint8_t*)&entry, sizeof(EEPROMLogEntry));
}

void readEEPROMLog(uint16_t logIdx, EEPROMLogEntry& entry) {
  if (logIdx >= MAX_EEPROM_LOGS) return;
  uint16_t regAddress = EEPROM_LOG_START + (logIdx * sizeof(EEPROMLogEntry));
  eeprom_read_buffer(regAddress, (uint8_t*)&entry, sizeof(EEPROMLogEntry));
}

void addEEPROMLog(const EEPROMLogEntry& entry) {
  if (!useEEPROM) return;
  
  // Write new log to nextIndex position
  writeEEPROMLog(eepromNextIndex, entry);
  
  // Update nextIndex and totalLogs count
  eepromNextIndex = (eepromNextIndex + 1) % MAX_EEPROM_LOGS;
  if (eepromTotalLogs < MAX_EEPROM_LOGS) {
    eepromTotalLogs++;
  }
  
  // Save updated header parameters
  saveEEPROMHeader();
  Serial.printf("✓ Log written to EEPROM at index %d (Total count: %d)\n", eepromNextIndex - 1, eepromTotalLogs);
}

void deleteEEPROMLogsForDate(String targetDate) {
  if (!useEEPROM) return;
  
  Serial.println("Starting in-place EEPROM log deletion via LittleFS temp file...");
  
  // 1. Open temp file on LittleFS
  File tempFile = LittleFS.open("/temp_eeprom_logs.bin", FILE_WRITE);
  if (!tempFile) {
    Serial.println("❌ Failed to open temp file on LittleFS for EEPROM compaction!");
    return;
  }
  
  uint16_t keptCount = 0;
  uint16_t logsToProcess = eepromTotalLogs;
  uint16_t readIdx = (eepromNextIndex - eepromTotalLogs + MAX_EEPROM_LOGS) % MAX_EEPROM_LOGS;
  
  for (uint16_t i = 0; i < logsToProcess; i++) {
    // Yield to let scheduler run idle task during heavy operation
    if (i % 50 == 0) delay(2);
    
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
    
    if (String(dateBuf) != targetDate) {
      tempFile.write((uint8_t*)&entry, sizeof(EEPROMLogEntry));
      keptCount++;
    }
    
    readIdx = (readIdx + 1) % MAX_EEPROM_LOGS;
  }
  tempFile.close();
  
  Serial.printf("Filtered logs. Kept count: %d. Writing back to EEPROM...\n", keptCount);
  
  // 2. Open temp file to read and write back to EEPROM
  tempFile = LittleFS.open("/temp_eeprom_logs.bin", FILE_READ);
  if (tempFile) {
    for (uint16_t i = 0; i < keptCount; i++) {
      if (i % 50 == 0) delay(2);
      
      EEPROMLogEntry entry;
      tempFile.read((uint8_t*)&entry, sizeof(EEPROMLogEntry));
      writeEEPROMLog(i, entry);
    }
    tempFile.close();
    LittleFS.remove("/temp_eeprom_logs.bin");
  } else {
    Serial.println("❌ Failed to read temp file for write-back!");
    return;
  }
  
  // 3. Update EEPROM Header
  eepromTotalLogs = keptCount;
  eepromNextIndex = keptCount % MAX_EEPROM_LOGS;
  saveEEPROMHeader();
  Serial.println("✓ EEPROM date logs deleted and compacted successfully!");
}

void lookupUserProfile(const String& uid, String& name, String& role, String& roll) {
  name = "Unknown";
  role = "-";
  roll = uid; // Fallback
  
  // Determine profile path
  String path = "/" + uid + ".txt";
  if (!LittleFS.exists(path)) {
    path = "/" + uid + "_f.txt";
  }
  
  if (LittleFS.exists(path)) {
    File f = LittleFS.open(path, FILE_READ);
    if (f) {
      String c = f.readString();
      f.close();
      
      // Parse pipes
      int pipe1 = c.indexOf('|');
      int pipe2 = c.indexOf('|', pipe1 + 1);
      int pipe3 = c.indexOf('|', pipe2 + 1);
      
      if (pipe1 != -1 && pipe2 != -1) {
        name = c.substring(0, pipe1);
        role = c.substring(pipe1 + 1, pipe2);
        if (pipe3 != -1) {
          roll = c.substring(pipe2 + 1, pipe3);
        } else {
          roll = c.substring(pipe2 + 1);
        }
        name.trim();
        role.trim();
        roll.trim();
      }
    }
  }
}

