#include "mux.h"

Mux muxA(Serial, 1);
Mux muxB(Serial, 2);


void setup(void) {
  Serial.begin(9600);
}

void loop(void) {
  static uint16_t count = 0;

  if (muxA.available()) {
    muxA.write(muxA.read() + 1);
  }

  if (!count) {
    muxB.print("System time: ");
    muxB.println(millis());
  }

  count++;
}
