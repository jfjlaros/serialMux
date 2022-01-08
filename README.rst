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

.. class:: center

    **UNDER CONSTRUCTION**

Please see the usage_ examples and demo_.


.. _usage: https://github.com/jfjlaros/serialMux/blob/master/docs/usage.rst
.. _demo: https://github.com/jfjlaros/serialMux/blob/master/examples/demo/demo.ino

----

Todo:

- Discard input after timeout to prevent locking.

.. list-table:: Packet structure.
   :header-rows: 1

   * - offset
     - length
     - description
   * - 0
     - 1
     - virtual port
   * - 1
     - 1
     - length
   * - 2
     - length
     - data

Control messages are used to set up the multiplexed connections, they are
sent to virtual port ``0``.

.. list-table:: Control messages.
   :header-rows: 1

   * - message
     - response
     - description
   * - 0
     - Number of virtual ports.
     - Get the number of virtual ports.
   * - 1
     - ``0x00``
     - Enable multiplexer.
   * - 2
     - ``0x00``
     - Disable multiplexer.
   * - 3
     - ``0x00``
     - Reset.

A typical initialisation procedure looks as follows.

1. The host requests the number of virtual ports (``0x00, 0x01, 0x00``).
2. The device sends the number of virtual ports (e.g., ``0x00, 0x01, 0x02``).
3. The host sets up pseudo terminals that connect to the virtual ports.
4. The host send the enable command (``0x00, 0x01, 0x01``).
