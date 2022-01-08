Usage
=====

Include the header file to use the serialMux library.

.. code-block:: cpp

    #include <serialMux.h>

The library provides the ``SerialMux`` class, which creates a virtual serial
device.

.. code-block:: cpp

    SerialMux muxA(Serial);
    SerialMux muxB(Serial);

The physical serial device must be configured as usual.

.. code-block:: cpp

    void setup(void) {
      Serial.begin(9600);
    }

Virtual serial devices can be used just like the familiar physical serial
device.

.. code-block:: cpp

    void loop(void) {
      muxA.println("Virtual serial port 1.");
      muxB.println("Virtual serial port 2.");
      delay(1000);
    }

Hard coded ``Serial`` workaround
--------------------------------

Some libraries will write debugging or other information directly to the
physical serial device. To force this library to use one of the virtual
serial devices, the following lines can be added before the library is
included.

.. code-block:: cpp

    HardwareSerial& masterSerial = Serial;
    SerialMux muxA(masterSerial);
    #define Serial muxA

    #include <hardcodedSerialUsingLib.h>

Note that any communication with the physical serial device must now be done
using ``masterSerial`` instead of ``Serial``.

.. code-block:: cpp

    void setup(void) {
      masterSerial.begin(9600);
    }
