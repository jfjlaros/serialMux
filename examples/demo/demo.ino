#include <serialMux.h>

SerialMux mux(Serial);
VSerial serialA(mux);
VSerial serialB(mux);


void setup(void) {
  Serial.begin(9600);
}

void loop(void) {
  static uint16_t count = 0;

  if (serialA.available()) {
    serialA.print("received: ");
    serialA.println(serialA.read());
  }

  if (!count) {
    serialB.print("time: ");
    serialB.println(millis());
  }

  count++;
}
