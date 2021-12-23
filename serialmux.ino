#include <simpleRPC.h>

#include "mux.h"

Mux muxA(Serial, 1);
Mux muxB(Serial, 2);

uint8_t inc(uint8_t a) {
  return a + 1;
}


void setup(void) {
  Serial.begin(9600);
}

void loop(void) {
  static uint16_t count = 0;

  interface(
    muxA,
    inc, F("inc: Increment a value. @a: Value. @return: a + 1."));

  if (!count) {
    muxB.print("System time: ");
    muxB.println(millis());
  }

  count++;
}
