Introduction
============

This library provides a simple way to create multiple virtual serial devices
that communicate over one physical serial connection. A virtual device can be
used as a drop-in replacement for ``Stream`` like objects such as ``Serial``.


Motivation
----------

Suppose we we have a function ``someFunction()`` that takes a command and
returns a status. Calling such a function from outside may look something
like this.

.. code-block:: cpp

    if (Serial.available()) {
      Serial.write(someFunction(Serial.read()));
    }

This approach works as long as there are no other processes using the serial
connection. This means that things like debugging information can no longer
be printed to ``Serial``.

This is where the serialMux library comes in. By creating virtual serial
devices, multiple connections can be made over the same physical serial line.
