# Room RFID Controller

An Arduino Nano project that uses an RC522 RFID/NFC reader to control two room electricity circuits through a 2-channel relay module.

Scan any RFID card once to save it automatically. After that, scanning a saved card toggles the room electricity **ON** or **OFF**.

## Features

- Controls one room circuit through Relay 1
- Uses Arduino Nano and RC522 RFID reader
- New RFID cards save automatically in the Nano EEPROM (up to 80 cards)
- Saved card IDs remain saved after reset or power loss
- Relay starts in the OFF state after reset or power loss
- Prevents repeated toggles when a card remains on the reader

## Hardware required

- Arduino Nano (Type-C Nano works)
- RC522 RFID/NFC card reader
- RFID cards or key fobs
- 2-channel relay module
- Jumper wires
- Suitable power supply and protected enclosure

## Wiring

![Low-voltage hardware wiring diagram](hardware-wiring-diagram.svg)

This diagram shows only the low-voltage control wiring. Keep AC mains/load wiring inside a suitable enclosure and have it completed by a qualified electrician.

### RC522 to Arduino Nano

| RC522 pin | Arduino Nano pin |
| --- | --- |
| 3.3V | 3.3V |
| GND | GND |
| RST | D9 |
| SDA / SS | D10 |
| MOSI | D11 |
| MISO | D12 |
| SCK | D13 |

### Relay module to Arduino Nano

| Relay module pin | Arduino Nano pin |
| --- | --- |
| IN1 | D6 |
| IN2 | D7 (reserved; not used in current automatic-save mode) |
| GND | GND |
| VCC | 5V, according to your relay module |

## Installation

1. Install the Arduino IDE.
2. Open [`rfid_dual_relay/rfid_dual_relay.ino`](rfid_dual_relay/rfid_dual_relay.ino).
3. In Arduino IDE Library Manager, install **MFRC522** by GithubCommunity.
4. Select **Arduino Nano** and the correct processor and serial port.
5. Upload the sketch.
6. Open Serial Monitor at **115200 baud** and scan each RFID card.
7. Scan a card. It is automatically saved in the Nano EEPROM and can immediately control Relay 1.

## How it works

| Card | Action |
| --- | --- |
| New card | Saves its UID automatically, then toggles Relay 1 |
| Saved card | Toggles Room electricity ON/OFF |

## Important safety notice

This project must **not** connect the Arduino or RC522 directly to AC mains electricity. Use a relay rated for the voltage, current, and type of electrical load. Put all mains wiring in a proper insulated enclosure and use a qualified electrician for fixed building wiring. For high-power room loads, use an appropriately rated contactor. Because every new card is accepted automatically, this mode is not suitable for a public or high-security location.

The RC522 must be powered from **3.3V only**.

## Project files

- [`rfid_dual_relay/rfid_dual_relay.ino`](rfid_dual_relay/rfid_dual_relay.ino) — Arduino source code
- [`rfid_dual_relay/README.md`](rfid_dual_relay/README.md) — detailed wiring and configuration guide

## License

This project is provided for educational and hobby use.
