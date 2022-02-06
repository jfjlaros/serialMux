Serial multiplexer for Arduino
==============================

.. image:: https://img.shields.io/github/last-commit/jfjlaros/serialMux.svg
   :target: https://github.com/jfjlaros/serialMux/graphs/commit-activity
.. image:: https://github.com/jfjlaros/serialMux/actions/workflows/arduino-package.yml/badge.svg
   :target: https://github.com/jfjlaros/serialMux/actions/workflows/arduino-package.yml
.. image:: https://readthedocs.org/projects/simplerpc/badge/?version=latest
   :target: https://serialMux.readthedocs.io/en/latest
.. image:: https://img.shields.io/github/release-date/jfjlaros/serialMux.svg
   :target: https://github.com/jfjlaros/serialMux/releases
.. image:: https://img.shields.io/github/release/jfjlaros/serialMux.svg
   :target: https://github.com/jfjlaros/serialMux/releases
..
    .. image:: https://www.ardu-badge.com/badge/serialMux.svg
       :target: https://www.ardu-badge.com/serialMux
.. image:: https://img.shields.io/github/languages/code-size/jfjlaros/serialMux.svg
   :target: https://github.com/jfjlaros/serialMux
.. image:: https://img.shields.io/github/languages/count/jfjlaros/serialMux.svg
   :target: https://github.com/jfjlaros/serialMux
.. image:: https://img.shields.io/github/languages/top/jfjlaros/serialMux.svg
   :target: https://github.com/jfjlaros/serialMux
.. image:: https://img.shields.io/github/license/jfjlaros/serialMux.svg
   :target: https://raw.githubusercontent.com/jfjlaros/serialMux/master/LICENSE.md

----

This library provides a simple way to create multiple virtual serial devices
that communicate over one physical serial connection. A virtual device can be
used as a drop-in replacement for ``Stream`` like objects such as ``Serial``.

A service is needed on the host for multiplexing and demultiplexing and to
create virtual ports. A Python client_ for Linux is provided as a reference
implementation.

Please see ReadTheDocs_ for the latest documentation.


Quick start
-----------

Create multiple virtual serial devices and use them like the standard
``Serial`` object.

.. code-block:: cpp

    #include <serialMux.h>

    SerialMux mux(Serial);
    VSerial serialA(mux);
    VSerial serialB(mux);

    void setup(void) {
      Serial.begin(9600);
    }

    void loop(void) {
      serialA.println("Virtual serial device A.");
      serialB.println("Virtual serial device B.");
      delay(1000);
    }

On the host, two virtual ports are created, e.g., ``/dev/pts/8`` and
``/dev/pts/9``. When we connect to one of these ports, we only see the
messages that are sent to that port.

::

    $ picocom -q /dev/pts/8
    Virtual device A.
    Virtual device A.


.. _ReadTheDocs: https://serialmux.readthedocs.io
.. _client: https://arduino-serial-mux.readthedocs.io
