Protocol
========

In this section we describe the serialMux protocol.

Packets
-------

Communication over a shared serial connection is accomplished by using
packets. The packet structure is straightforward, every message is prefixed
by a virtual port number and the length of the message.

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

The first virtual device has port number ``1``, the second ``2``, etc.
Virtual port number ``0`` is reserved for control messages. The maximum
number of virtual devices is 254 and the length of the message is limited to
255 bytes.

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

After initialisation the first pseudo terminal can be used to communicate
with the virtual device on port ``1``, the second pseudo terminal can be used
to communicate with the virtual device on port ``2``, etc.
