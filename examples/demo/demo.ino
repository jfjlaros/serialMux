#include <serialMux.h>

SerialMux muxA(Serial);
SerialMux muxB(Serial);


void setup(void) {
  Serial.begin(9600);
}

void loop(void) {
  static uint16_t count = 0;

  if (muxA.available()) {
    muxA.print("received: ");
    muxA.println(muxA.read());
  }

  if (!count) {
    muxB.print("time: ");
    muxB.println(millis());
  }

  count++;
}
