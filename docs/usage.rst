Usage
=====

.. code-block:: cpp

    #include <serialMux.h>

    Mux muxA(Serial);
    Mux muxB(Serial);


    void setup(void) {
      Serial.begin(9600);
    }

    void loop(void) {
      muxA.println("Virtual serial port one.");
      muxB.println("Virtual serial port two.");
      delay(1000);
    }


.. code-block:: cpp

    HardwareSerial& masterSerial = Serial;

    Mux muxA(masterSerial);
    Mux muxB(masterSerial);

    #define Serial muxB


    void setup(void) {
      masterSerial.begin(9600);
    }

    void loop(void) {
      muxA.println("Virtual serial port one.");
      Serial.println("Virtual serial port two.");
      delay(1000);
    }
