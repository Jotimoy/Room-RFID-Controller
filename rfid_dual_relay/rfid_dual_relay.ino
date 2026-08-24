/*
  Arduino Nano + RC522 RFID reader + room electricity relay controller

  Every new card scanned is automatically saved in the Arduino Nano EEPROM.
  Every saved card toggles Relay 1 ON/OFF. Saved cards remain after power loss.

  Required Arduino library: MFRC522 by GithubCommunity
*/

#include <SPI.h>
#include <EEPROM.h>
#include <MFRC522.h>

// RC522 SPI wiring for Arduino Nano (ATmega328P).
constexpr byte SS_PIN = 10;
constexpr byte RST_PIN = 9;
constexpr byte RELAY_1_PIN = 6;
constexpr byte RELAY_2_PIN = 7;  // Not used in automatic Relay 1 mode.

// Most relay modules are active LOW. Set to false for an active-HIGH module.
constexpr bool RELAY_ACTIVE_LOW = true;
constexpr unsigned long SAME_CARD_COOLDOWN_MS = 3000;

// Change to true, upload once, and reset the Nano to remove every saved card.
// Immediately change it back to false and upload again afterwards.
constexpr bool CLEAR_SAVED_CARDS_ON_BOOT = false;

MFRC522 rfid(SS_PIN, RST_PIN);

constexpr byte EEPROM_MAGIC_1 = 0x52;
constexpr byte EEPROM_MAGIC_2 = 0x46;
constexpr int EEPROM_HEADER_SIZE = 2;
constexpr byte UID_MAX_BYTES = 10;
constexpr byte MAX_SAVED_CARDS = 80;  // Uses 882 of the Nano's 1024 EEPROM bytes.

struct StoredCard {
  byte uidSize;
  byte uid[UID_MAX_BYTES];
};

constexpr int CARD_SLOT_SIZE = sizeof(StoredCard);

bool relay1On = false;
byte lastUid[UID_MAX_BYTES];
byte lastUidSize = 0;
unsigned long lastScanAt = 0;

int cardAddress(byte index) {
  return EEPROM_HEADER_SIZE + (index * CARD_SLOT_SIZE);
}

void setRelay(byte pin, bool on) {
  const byte level = (on == RELAY_ACTIVE_LOW) ? LOW : HIGH;
  digitalWrite(pin, level);
}

void printUid(const MFRC522::Uid &uid) {
  for (byte i = 0; i < uid.size; i++) {
    if (uid.uidByte[i] < 0x10) Serial.print('0');
    Serial.print(uid.uidByte[i], HEX);
    if (i + 1 < uid.size) Serial.print(' ');
  }
}

bool validUidSize(byte size) {
  return size == 4 || size == 7 || size == 10;
}

bool uidMatches(const MFRC522::Uid &scanned, const StoredCard &saved) {
  if (!validUidSize(saved.uidSize) || scanned.size != saved.uidSize) return false;
  for (byte i = 0; i < scanned.size; i++) {
    if (scanned.uidByte[i] != saved.uid[i]) return false;
  }
  return true;
}

bool cardIsSaved(const MFRC522::Uid &scanned) {
  StoredCard saved;
  for (byte i = 0; i < MAX_SAVED_CARDS; i++) {
    EEPROM.get(cardAddress(i), saved);
    if (uidMatches(scanned, saved)) return true;
  }
  return false;
}

bool saveCard(const MFRC522::Uid &scanned) {
  StoredCard saved;
  for (byte i = 0; i < MAX_SAVED_CARDS; i++) {
    const int address = cardAddress(i);
    EEPROM.get(address, saved);
    if (!validUidSize(saved.uidSize)) {
      saved.uidSize = scanned.size;
      for (byte j = 0; j < UID_MAX_BYTES; j++) {
        saved.uid[j] = (j < scanned.size) ? scanned.uidByte[j] : 0;
      }
      EEPROM.put(address, saved);
      return true;
    }
  }
  return false;
}

void clearSavedCards() {
  for (byte i = 0; i < MAX_SAVED_CARDS; i++) EEPROM.update(cardAddress(i), 0);
  Serial.println(F("All saved RFID cards were erased."));
}

void initialiseCardStorage() {
  const bool validStorage = EEPROM.read(0) == EEPROM_MAGIC_1 &&
                            EEPROM.read(1) == EEPROM_MAGIC_2;
  if (!validStorage || CLEAR_SAVED_CARDS_ON_BOOT) {
    EEPROM.update(0, EEPROM_MAGIC_1);
    EEPROM.update(1, EEPROM_MAGIC_2);
    clearSavedCards();
  }
}

bool sameAsLastCard(const MFRC522::Uid &scanned) {
  if (scanned.size != lastUidSize) return false;
  for (byte i = 0; i < scanned.size; i++) {
    if (scanned.uidByte[i] != lastUid[i]) return false;
  }
  return true;
}

void rememberCard(const MFRC522::Uid &scanned) {
  lastUidSize = scanned.size;
  for (byte i = 0; i < scanned.size; i++) lastUid[i] = scanned.uidByte[i];
  lastScanAt = millis();
}

void toggleRelay1() {
  relay1On = !relay1On;
  setRelay(RELAY_1_PIN, relay1On);
  Serial.println(relay1On ? F("Room electricity ON") : F("Room electricity OFF"));
}

void setup() {
  Serial.begin(115200);
  pinMode(RELAY_1_PIN, OUTPUT);
  pinMode(RELAY_2_PIN, OUTPUT);
  setRelay(RELAY_1_PIN, false);
  setRelay(RELAY_2_PIN, false);

  initialiseCardStorage();
  SPI.begin();
  rfid.PCD_Init();
  Serial.println(F("RFID room controller ready."));
  Serial.println(F("New cards are automatically saved for Relay 1."));
}

void loop() {
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) return;

  const MFRC522::Uid &scanned = rfid.uid;
  Serial.print(F("Card UID: "));
  printUid(scanned);
  Serial.println();

  const bool duplicate = sameAsLastCard(scanned) &&
                         (millis() - lastScanAt < SAME_CARD_COOLDOWN_MS);
  rememberCard(scanned);

  if (!duplicate) {
    if (cardIsSaved(scanned)) {
      Serial.println(F("Saved card: access granted."));
      toggleRelay1();
    } else if (saveCard(scanned)) {
      Serial.println(F("New card saved: access granted."));
      toggleRelay1();
    } else {
      Serial.println(F("Card memory is full; no new card was saved."));
    }
  }

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}
