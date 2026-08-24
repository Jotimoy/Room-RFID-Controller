# Nano RFID two-relay controller

This Arduino Nano sketch reads an RC522 RFID/NFC card UID and pulses the relay assigned to that UID for two seconds.

## Wiring

| RC522 | Arduino Nano |
| --- | --- |
| 3.3V | 3.3V |
| GND | GND |
| RST | D9 |
| SDA / SS | D10 |
| MOSI | D11 |
| MISO | D12 |
| SCK | D13 |

| Relay module | Arduino Nano |
| --- | --- |
| Relay 1 IN | D6 |
| Relay 2 IN | D7 |
| GND | GND (shared with Nano) |
| VCC | 5V relay supply, per module rating |

**Important:** Power the RC522 from **3.3V only**. Do not connect its signal pins to 5V. Relay modules and the lock/load need an adequate separate power supply; share ground with the Nano where the relay module requires it.

## Install and upload

1. In Arduino IDE, install **MFRC522** from Library Manager (author: GithubCommunity).
2. Open `rfid_dual_relay.ino`, select **Arduino Nano** and its processor/port, then upload.
3. Open Serial Monitor at **115200 baud** and scan each card.
4. Copy the printed UID into `CARD_RELAY_1` or `CARD_RELAY_2`. Keep the `0x` prefix and use the exact UID byte count.
5. Upload again.

Example: output `Card UID: 04 A1 B2 C3 D4 E5 F6` becomes:

```cpp
const byte CARD_RELAY_1[] = {0x04, 0xA1, 0xB2, 0xC3, 0xD4, 0xE5, 0xF6};
```

The sketch assumes the common **active-LOW** relay modules. If yours turns on when it should be off, change `RELAY_ACTIVE_LOW` to `false`.

## Notes on Android NFC

An Android phone will work only when it presents a compatible card type and stable UID. Many modern Android devices randomize or block exposing their NFC UID, so a physical MIFARE-compatible card/fob is the reliable choice. This sketch uses UID matching—not encrypted card authentication—so it is suitable for basic hobby access control, not high-security doors.
