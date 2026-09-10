#ifndef EEPROM_LOGGER_H
#define EEPROM_LOGGER_H

#include <Arduino.h>
#include <Wire.h>

#define EEPROM_ADDR 0x50
#define EEPROM_PAGE_SIZE 128
#define EEPROM_TOTAL_BYTES 65536
#define EEPROM_LOG_START 8

struct EEPROMLogEntry {
  uint32_t timestamp;  // Unix Epoch time (4 bytes)
  char uid[12];        // RFID UID or Fingerprint ID (12 bytes, null terminated)
  uint8_t direction;   // 1 = In, 2 = Out (1 byte)
  uint8_t status;      // 1 = On-Time, 2 = Late, 3 = Early Exit, 4 = Accepted, 5 = Denied (1 byte)
  uint8_t method;      // 1 = Fingerprint, 2 = RFID Card, 3 = Manual (1 byte)
  uint8_t padding;     // Align struct to 20 bytes (1 byte)
};

#define MAX_EEPROM_LOGS ((EEPROM_TOTAL_BYTES - EEPROM_LOG_START) / sizeof(EEPROMLogEntry))

extern bool useEEPROM;
extern uint16_t eepromTotalLogs;
extern uint16_t eepromNextIndex;

// Function declarations
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

#endif
