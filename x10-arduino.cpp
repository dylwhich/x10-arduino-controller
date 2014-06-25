#include <Serial.h>
#include <arduino-x10/x10.h>
#include <arduino-x10/x10constants.h>

#define CONTROL_BIT 1
#define DATA_BIT 0

byte convNum(byte n) {
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

  buf = Serial.read();
  
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
}
