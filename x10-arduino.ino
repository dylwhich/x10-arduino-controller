#include <serial>
#include <x10.h>
#include <x10constants.h>

#define CONTROL_BIT 1
#define DATA_BIT 0

char[4] signal;
char buf, checksum;

char convNum(char n) {
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

void handleData() {
  // do nothing here
}

void handleControl() {
  // do nothing here too
}

int verifySignal() {
  checksum = signal[0] ^ signal[1] ^ signal[2];
  return checksum == signal[3];
}

char nthBit(char n) {
  return signal[n/8] & (1 << (8 - n % 8)) >> (8 - n % 8);
}

char nthNibble(char n) {
  return signal[n/2] & (0xf << (2 - n % 2)) >> (2 - n % 2);
}

// Arduino boilerplate stuff
void setup() {
  signal = {0, 0, 0, 0};
  buf = 0;
  checksum = 0xff;
}

void loop() {
  buf = Serial.read();
  
  if (verifySignal()) {
    if (nthBit(1) == DATA_BIT) {
      handleData();
    } else {
      handleControl();
    }
  } else {
    signal[0] = signal[1];
    signal[1] = signal[2];
    signal[2] = signal[3];
    signal[3] = buf;
  }
}
