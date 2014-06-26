#include <Serial.h>
#include <arduino-x10/x10.h>
#include <arduino-x10/x10constants.h>

#define CONTROL_BIT 1
#define DATA_BIT 0

#define CTL_READY_STATUS 0x00
#define CTL_TERMINATE 0x01

#define READY_STATUS_NOT_READY 0x000
#define READY_STATUS_READY 0x001

#define X10_REPEAT 1
#define X10_ZC_PIN 0
#define X10_TX_PIN 0
#define X10_RX_PIN 0

byte x10ToNum(byte n) {
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

byte numToX10(byte n) {
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

byte nthBit(byte signal[], byte n) {
  return signal[n/8] & (1 << (8 - n % 8)) >> (8 - n % 8);
}

byte nthNibble(byte signal[], byte n) {
  return signal[n/2] & (0xf << (2 - n % 2)) >> (2 - n % 2);
}

void handleData(byte signal[]) {
  // do nothing here
}

void handleControl(byte signal[]) {
  // do nothing here too
}

void handleX10(byte house, byte unit, byte code) {
  // do stuff
}

void sendX10(byte house, byte unit, byte code) {
  // send code
}

void sendData(byte opcode, byte data1, byte datar) {
  // send data to serial
}

void sendControl(byte opcode, byte datal, byte datar) {
  Serial.write(opcode);
  Serial.write(datal);
  Serial.write(datar);
  Serial.write(opcode ^ datal ^ datar);
}

int verifySignal(byte signal[]) {
  byte checksum = signal[0] ^ signal[1] ^ signal[2];
  return checksum == signal[3] && nthBit(signal, 0) == 0x01;
}

int main() {
  byte signal[4] = {0xff, 0x00, 0x00, 0xab};
  byte buf = 0;
  bool sAvail = false, xAvail = false;
  x10 x10lib;

  Serial.begin(9600);
  delay(500);

  x10lib =  = x10(X10_ZC_PIN, X10_TX_PIN, X10_RX_PIN);
  delay(500);

  sendControl(CTL_READY_STATUS, 0, READY_STATUS_READY);

  while (true) {
    while (!(sAvail = Serial.available()) && !(xAvail = x10lib.received()));

    if (xAvail) {
      x10lib.reset();
      handleX10(x10lib.houseCode(), x10lib.unitCode(), x10lib.cmndCode());
      xAvail = false;
    }

    if (sAvail) {
      buf = Serial.read();
      Serial.write(buf);

      if (verifySignal(signal)) {
	if (nthBit(signal, 1) == DATA_BIT) {
	  handleData(signal);
	} else {
	  handleControl(signal);
	}
      } else {
	signal[0] = signal[1];
	signal[1] = signal[2];
	signal[2] = signal[3];
	signal[3] = buf;
      }
      sAvail = false;
    }
  }
}
