Serial Multiplexer for Arduino
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

**UNDER CONSTRUCTION**

This library provides a simple way to create multiple virtual serial devices
that communicate over one physical serial connection. A virtual device can be
used as a drop-in replacement for ``Stream`` like objects such as ``Serial``.

Please see ReadTheDocs_ for the latest documentation.


Quick start
-----------

.. code-block:: cpp

    #include <serialMux.h>

    SerialMux muxA(Serial);
    SerialMux muxB(Serial);

    void setup(void) {
      Serial.begin(9600);
    }

    void loop(void) {
      muxA.println("This is a message from virtual device 1.");
      muxB.println("This is a message from virtual device 2.");
      delay(1000);
    }


.. _ReadTheDocs: https://serialmux.readthedocs.io
