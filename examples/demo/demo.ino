#include <serialMux.h>

Control control(Serial);
SerialMux muxA;
SerialMux muxB;


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
