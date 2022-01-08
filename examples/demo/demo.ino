#include <serialMux.h>
#include <simpleRPC.h>

SerialMux muxA(Serial);
SerialMux muxB(Serial);


uint8_t inc(uint8_t a) {
  return a + 1;
}


void setup(void) {
  Serial.begin(9600);
}

void loop(void) {
  static uint32_t count = 0;

  interface(
    muxA,
    inc, F("inc: Increment a value. @a: Value. @return: a + 1."));

  if (!count) {
    muxB.print("System time: ");
    muxB.println(millis());
  }

  count += 8192;
}