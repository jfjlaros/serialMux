Protocol
========

In this section we describe the serialMux protocol.


Packets
-------

Communication over a shared serial connection is accomplished by using
packets. The packet structure is straightforward, every message is prefixed
by a virtual serial port number and the length of the message.

.. list-table:: Packet structure.
   :header-rows: 1

   * - offset
     - length
     - description
   * - 0
     - 1
     - virtual serial port
   * - 1
     - 1
     - size
   * - 2
     - size
     - data

The first virtual device has port number ``0``, the second ``1``, etc. Port
number ``255`` is reserved for control messages. The maximum number of virtual
devices is 254 and the length of the message is limited to 255 bytes.


Control messages
----------------

Before any of the virtual devices can be used, the host must be informed
about the number of virtual devices. To ensure that this initial
communication is interference free, all virtual devices are disabled on start
up.

.. list-table:: Control messages.
   :header-rows: 1

   * - message
     - response
     - description
   * - 0
     - ``serialMux``
     - Protocol identifier.
   * - 1
     - ``\x01\x00\x00`` (example)
     - Version (major, minor, patch).
   * - 2
     - Number of virtual serial ports.
     - Get the number of virtual serial ports.
   * - 3
     - ``0x00``
     - Enable multiplexer.
   * - 4
     - ``0x00``
     - Disable multiplexer.
   * - 5
     - ``0x00``
     - Reset.

A typical initialisation procedure looks as follows.

1. The host asks for the protocol identifier (``0xff, 0x01, 0x00``).
2. The device responds with the protocol identifier.
3. The host asks for the protocol version (``0xff, 0x01, 0x01``).
4. The device responds with the protocol version.
5. The host requests the number of virtual serial ports (``0xff, 0x01, 0x02``).
6. The device sends the number of virtual serial ports (e.g., ``0xff, 0x01,
   0x02``).
7. The host sets up pseudo terminals that connect to the virtual serial ports.
8. The host send the enable command (``0xff, 0x01, 0x03``).
9. The device responds with an acknowledgement (``0xff, 0x01, 0x00``).

After initialisation the first pseudo terminal can be used to communicate
with the virtual device on port ``0``, the second pseudo terminal can be used
to communicate with the virtual device on port ``1``, etc.
