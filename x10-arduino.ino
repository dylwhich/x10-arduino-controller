#include <Serial>
#include <x10.h>
#include <x10constants.h>

#define DATA_HEADER    0b1000
#define CONTROL_HEADER 0b1100

#define CTL_READY_STATUS 0b0000
#define CTL_TERMINATE 0b0001
#define CTL_ACKNOWLEDGE 0b0010

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

byte houseToNum(byte n) {
  return n - 'A';
}

byte numToHouse(byte n) {
  switch(n) {
  case 0:
    return HOUSE_A;
  case 1:
    return HOUSE_B;
  case 2:
    return HOUSE_C;
  case 3:
    return HOUSE_D;
  case 4:
    return HOUSE_E;
  case 5:
    return HOUSE_F;
  case 6:
    return HOUSE_G;
  case 7:
    return HOUSE_H;
  case 8:
    return HOUSE_I;
  case 9:
    return HOUSE_J;
  case 10:
    return HOUSE_K;
  case 11:
    return HOUSE_L;
  case 12:
    return HOUSE_M;
  case 13:
    return HOUSE_N;
  case 14:
    return HOUSE_O;
  case 15:
    return HOUSE_P;
  default:
    return HOUSE_A;
  }
}

byte unitToNum(byte n) {
  return (n - 1) & 0x0f;
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
    return UNIT_1;
  }
}

byte cmdToNum(byte n) {
  return (n >> 1) & 0x0f;
}

byte numToCmd(byte n) {
  return ((n & 0x0f) << 1) | 0x01;
}

void handleData(byte signal[]) {
  byte house, unit, code;
  house = numToHouse(signal[0] & 0x0f);
  unit = numToUnit((signal[1] >> 4) & 0x0f);
  code = numToCmd((signal[1] & 0x0f));
  sendX10(house, unit, code, signal[2]);
  sendControl(CTL_ACKNOWLEDGE, 0, 0);
}

void handleControl(byte signal[]) {
  // TODO
}

void handleX10(byte house, byte unit, byte code) {
  sendData(house, unit, code);
}

void sendX10(byte house, byte unit, byte code, byte repeat) {
  if (repeat < 1)
    repeat = 1;
  x10lib.write(house, unit, repeat * X10_REPEAT);
  x10lib.write(house, code, repeat * X10_REPEAT);
}

void sendData(byte house, byte unit, byte code) {
  byte b1, b2, b3;
  b1 = ((DATA_HEADER << 4) & 0xf0) | (houseToNum(house) & 0x0f);
  b2 = ((unitToNum(unit) << 4) & 0xf0) | (cmdToNum(code) & 0x0f);
  b3 = X10_REPEAT;
  Serial.write(b1);
  Serial.write(b2);
  Serial.write(b3);
  Serial.write(b1 ^ b2 ^ b3);
}

void sendControl(byte opcode, byte val_hi, byte val_lo) {
  Serial.write((CONTROL_HEADER << 4) | (opcode & 0xf));
  Serial.write(val_hi);
  Serial.write(val_lo);
  Serial.write(((CONTROL_HEADER << 4) | (opcode & 0xf)) ^ val_lo ^ val_hi);
}

int verifySignal(byte signal[]) {
  byte checksum = signal[0] ^ signal[1] ^ signal[2];
  return (checksum == signal[3]) && ((signal[0] & 0x80) == 0x80);
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
      if ((signal[0] >> 4) == DATA_HEADER) {
	handleData(signal);
      } else if ((signal[0] >> 4) == CONTROL_HEADER) {
	handleControl(signal);
      }
    }
    sAvail = 0;
  }
}
