# Room RFID Controller

An Arduino Nano project that uses an RC522 RFID/NFC reader to control two room electricity circuits through a 2-channel relay module.

Scan an approved RFID card to toggle its assigned room **ON** or **OFF**.

## Features

- Controls two separate room circuits
- Uses Arduino Nano and RC522 RFID reader
- Each RFID card can be assigned to Relay 1 or Relay 2
- Unknown cards are denied and their UID is shown in Serial Monitor
- Relays start in the OFF state after reset or power loss
- Prevents repeated toggles when a card remains on the reader

## Hardware required

- Arduino Nano (Type-C Nano works)
- RC522 RFID/NFC card reader
- RFID cards or key fobs
- 2-channel relay module
- Jumper wires
- Suitable power supply and protected enclosure

## Wiring

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
| IN2 | D7 |
| GND | GND |
| VCC | 5V, according to your relay module |

## Installation

1. Install the Arduino IDE.
2. Open [`rfid_dual_relay/rfid_dual_relay.ino`](rfid_dual_relay/rfid_dual_relay.ino).
3. In Arduino IDE Library Manager, install **MFRC522** by GithubCommunity.
4. Select **Arduino Nano** and the correct processor and serial port.
5. Upload the sketch.
6. Open Serial Monitor at **115200 baud** and scan each RFID card.
7. Copy the UID shown in Serial Monitor into the card list in the sketch, then upload again.

Example UID output:

```text
Card UID: 04 A1 B2 C3 D4 E5 F6
```

Add it to the code like this:

```cpp
const byte CARD_RELAY_1[] = {0x04, 0xA1, 0xB2, 0xC3, 0xD4, 0xE5, 0xF6};
```

## How it works

| Card | Action |
| --- | --- |
| Card assigned to Relay 1 | Toggles Room 1 electricity ON/OFF |
| Card assigned to Relay 2 | Toggles Room 2 electricity ON/OFF |
| Unknown card | No action; access denied |

## Important safety notice

This project must **not** connect the Arduino or RC522 directly to AC mains electricity. Use a relay rated for the voltage, current, and type of electrical load. Put all mains wiring in a proper insulated enclosure and use a qualified electrician for fixed building wiring. For high-power room loads, use an appropriately rated contactor.

The RC522 must be powered from **3.3V only**.

## Project files

- [`rfid_dual_relay/rfid_dual_relay.ino`](rfid_dual_relay/rfid_dual_relay.ino) — Arduino source code
- [`rfid_dual_relay/README.md`](rfid_dual_relay/README.md) — detailed wiring and configuration guide

## License

This project is provided for educational and hobby use.
