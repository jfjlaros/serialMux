Usage
=====

Include the header file to use the serialMux library.

.. code-block:: cpp

    #include <serialMux.h>

The library provides the ``SerialMux`` and ``VSerial`` classes, which are
used to create virtual serial devices.

First create a serial multiplexer.

.. code-block:: cpp

    SerialMux mux(Serial);

The serial multiplexer can be used to create multiple virtual serial devices.

.. code-block:: cpp

    VSerial serialA(mux);
    VSerial serialB(mux);

The physical serial device must be configured as usual.

.. code-block:: cpp

    void setup() {
      Serial.begin(9600);
    }

Virtual serial devices can be used just like the familiar physical serial
device.

.. code-block:: cpp

    void loop() {
      serialA.println("Virtual serial device A.");
      serialB.println("Virtual serial device B.");
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
    SerialMux mux(masterSerial);
    VSerial serialA(mux)
    #define Serial serialA

    #include <hardcodedSerialUsingLib.h>

Note that any communication with the physical serial device must now be done
using ``masterSerial`` instead of ``Serial``.

.. code-block:: cpp

    void setup() {
      masterSerial.begin(9600);
    }
