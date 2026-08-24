/*
  Arduino Nano + RC522 RFID reader + two relay modules

  Scan an approved RFID/NFC card to toggle its assigned room relay ON/OFF.
  Unknown cards are not allowed; their UID is printed in the Serial Monitor.

  Required Arduino library: MFRC522 by GithubCommunity
*/

#include <SPI.h>
#include <MFRC522.h>

// RC522 SPI wiring for Arduino Nano (ATmega328P).
constexpr byte SS_PIN = 10;
constexpr byte RST_PIN = 9;

// Relay inputs. Change these if your wiring is different.
constexpr byte RELAY_1_PIN = 6;
constexpr byte RELAY_2_PIN = 7;

// Most relay modules are active LOW. Set to false for an active-HIGH module.
constexpr bool RELAY_ACTIVE_LOW = true;

constexpr unsigned long SAME_CARD_COOLDOWN_MS = 3000;

MFRC522 rfid(SS_PIN, RST_PIN);

struct CardAccess {
  const byte *uid;
  byte uidSize;
  byte relayPin;
  const char *name;
};

// Replace the example values with the UIDs printed for your cards.
// UID length may be 4, 7, or 10 bytes; use exactly the length printed.
const byte CARD_RELAY_1[] = {0xDE, 0xAD, 0xBE, 0xEF};
const byte CARD_RELAY_2[] = {0x12, 0x34, 0x56, 0x78};

const CardAccess allowedCards[] = {
  {CARD_RELAY_1, sizeof(CARD_RELAY_1), RELAY_1_PIN, "Relay 1 card"},
  {CARD_RELAY_2, sizeof(CARD_RELAY_2), RELAY_2_PIN, "Relay 2 card"},
};

constexpr byte CARD_COUNT = sizeof(allowedCards) / sizeof(allowedCards[0]);

bool relay1On = false;
bool relay2On = false;
// RC522 can report UIDs up to 10 bytes long.
byte lastUid[10];
byte lastUidSize = 0;
unsigned long lastScanAt = 0;

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

bool uidMatches(const MFRC522::Uid &scanned, const CardAccess &card) {
  if (scanned.size != card.uidSize) return false;
  for (byte i = 0; i < scanned.size; i++) {
    if (scanned.uidByte[i] != card.uid[i]) return false;
  }
  return true;
}

const CardAccess *findCard(const MFRC522::Uid &scanned) {
  for (byte i = 0; i < CARD_COUNT; i++) {
    if (uidMatches(scanned, allowedCards[i])) return &allowedCards[i];
  }
  return nullptr;
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

void toggleRelay(byte pin) {
  bool *state = (pin == RELAY_1_PIN) ? &relay1On : &relay2On;
  *state = !*state;
  setRelay(pin, *state);

  Serial.print(F("Relay "));
  Serial.print((pin == RELAY_1_PIN) ? 1 : 2);
  Serial.println(*state ? F(" ON") : F(" OFF"));
}

void setup() {
  Serial.begin(115200);
  pinMode(RELAY_1_PIN, OUTPUT);
  pinMode(RELAY_2_PIN, OUTPUT);
  setRelay(RELAY_1_PIN, false);
  setRelay(RELAY_2_PIN, false);

  SPI.begin();
  rfid.PCD_Init();
  Serial.println(F("RFID dual-relay controller ready."));
  Serial.println(F("Scan a card; copy the printed UID into allowedCards."));
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
    const CardAccess *card = findCard(scanned);
    if (card != nullptr) {
      Serial.print(F("Access granted: "));
      Serial.println(card->name);
      toggleRelay(card->relayPin);
    } else {
      Serial.println(F("Access denied. Add this UID to allowedCards if approved."));
    }
  }

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}
