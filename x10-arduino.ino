#include <Serial>
#include <x10.h>
#include <x10constants.h>

#define DATA_HEADER    0b1000
#define CONTROL_HEADER 0b1100

#define CTL_READY_STATUS 0b0000
#define CTL_TERMINATE 0b0001

#define READY_STATUS_NOT_READY 0
#define READY_STATUS_READY 1

#define X10_REPEAT 1

#define X10_ZC_PIN 2
#define X10_RX_PIN 4
#define X10_TX_PIN 5

byte signal[] = {0xff, 0x00, 0x00, 0xab};
byte buf = 0x00;
bool sAvail = false, xAvail = false;
x10 x10lib = x10(X10_ZC_PIN, X10_TX_PIN, X10_RX_PIN);

byte unitToNum(byte n) {
  switch (n) {
  case UNIT_1:
    return 0;
  case UNIT_2:
    return 1;
  case UNIT_3:
    return 2;
  case UNIT_4:
    return 3;
  case UNIT_5:
    return 4;
  case UNIT_6:
    return 5;
  case UNIT_7:
    return 6;
  case UNIT_8:
    return 7;
  case UNIT_9:
    return 8;
  case UNIT_10:
    return 9;
  case UNIT_11:
    return 10;
  case UNIT_12:
    return 11;
  case UNIT_13:
    return 12;
  case UNIT_14:
    return 13;
  case UNIT_15:
    return 14;
  case UNIT_16:
    return 15;
  default:
    return 0b11111;
  }
}

byte numToUnit(byte n) {
  switch(n) {
  case 0:
    return UNIT_1;
  case 1:
    return UNIT_2;
  case 2:
    return UNIT_3;
  case 3:
    return UNIT_4;
  case 4:
    return UNIT_5;
  case 5:
    return UNIT_6;
  case 6:
    return UNIT_7;
  case 7:
    return UNIT_8;
  case 8:
    return UNIT_9;
  case 9:
    return UNIT_10;
  case 10:
    return UNIT_11;
  case 11:
    return UNIT_12;
  case 12:
    return UNIT_13;
  case 13:
    return UNIT_14;
  case 14:
    return UNIT_15;
  case 15:
    return UNIT_16;
  default:
    return 0b11111;
  }
}

byte cmdToNum(byte n) {
  return n >> 1;
}

byte numToCmd(byte n) {
  return n << 1 | 1;
}

byte nthBit(byte signal[], byte n) {
  return signal[n/8] & (1 << (8 - n % 8)) >> (8 - n % 8);
}

byte nthNibble(byte signal[], byte n) {
  return signal[n/2] & (0xf << (2 - n % 2)) >> (2 - n % 2);
}

void handleData(byte signal[]) {
  byte house, unit, code;
  house = signal[0] & 0x0f;
  unit = (signal[1] & 0xf0) >> 4;
  code = (signal[1] & 0x0f);
  sendX10(house, unit, code);
}

void handleControl(byte signal[]) {
  // do nothing here too
}

void handleX10(byte house, byte unit, byte code) {
  sendData(house, unit, code);
}

void sendX10(byte house, byte unit, byte code) {
  x10lib.write(house, unit, X10_REPEAT);
  x10lib.write(house, code, X10_REPEAT);
}

void sendData(byte house, byte unit, byte code) {
  byte b1, b2, b3;
  b1 = ((DATA_HEADER << 4) & 0xf0) | (unitToNum(house << 1) & 0x0f);
  b2 = ((unitToNum(unit) << 4) & 0xf0) | ((cmdToNum(code) & 0x0f));
  b3 = X10_REPEAT;
  Serial.write(b1);
  Serial.write(b2);
  Serial.write(b3);
  Serial.write(b1 ^ b2 ^ b3);
}

void sendControl(byte opcode, byte val_hi, byte val_lo) {
  Serial.write((DATA_HEADER << 4) | (opcode & 0xf));
  Serial.write(val_hi);
  Serial.write(val_lo);
  Serial.write(((DATA_HEADER << 4) | (opcode & 0xf)) ^ val_lo ^ val_hi);
}

int verifySignal(byte signal[]) {
  byte checksum = signal[0] ^ signal[1] ^ signal[2];
  return checksum == signal[3] && nthBit(signal, 1) == 0x01;
}

void setup() {
  Serial.begin(9600);
  delay(500);
  
  sendControl(CTL_READY_STATUS, 0, READY_STATUS_READY);
}

void loop() {
  while (!sAvail && !xAvail && !(sAvail = Serial.available()) && !(xAvail = x10lib.received()));

  if (xAvail) {
    x10lib.reset();
    handleX10(x10lib.houseCode(), x10lib.unitCode(), x10lib.cmndCode());
    xAvail = false;
  }

  if (sAvail) {
    buf = Serial.read();

    signal[0] = signal[1];
    signal[1] = signal[2];
    signal[2] = signal[3];
    signal[3] = buf;

    if (verifySignal(signal)) {
      if (nthBit(signal, 1) == 0) {
	handleData(signal);
      } else {
	handleControl(signal);
      }
    }
    sAvail = false;
  }
}
