#pragma once

#include "buffer.tcc"

#define _PROTOCOL "serialMux"
#define _VERSION "\x02\x00\x00"
#define _ESCAPE (uint8_t)0xff
#define _CONTROL_PORT (uint8_t)0xfe

// Control commands.
#define CMD_PROTOCOL '\x00'
#define CMD_VERSION '\x01'
#define CMD_GET_PORTS '\x02'
#define CMD_ENABLE '\x03'
#define CMD_DISABLE '\x04'
#define CMD_RESET '\x05'

/*!
 * Serial multiplexer.
 */
template <uint8_t bits=6>
class SerialMux {
  public:
    SerialMux(Stream&);
    ~SerialMux();

    uint8_t add();

    size_t available(uint8_t);
    int read(uint8_t);
    void write(uint8_t, uint8_t);
    int peek(uint8_t);

  private:
    Buffer<bits>* _buffer = NULL;
    bool _enabled = false;
    uint8_t _portRx = _CONTROL_PORT;
    uint8_t _portTx = _CONTROL_PORT;
    uint8_t _ports = 0;
    Stream* _serial = NULL;

    void _control(uint8_t);
    void _update();

    uint8_t _read();
    void _write(uint8_t, uint8_t*, uint8_t);
};


/*!
 * Constructor.
 *
 * \param serial Serial device.
 */
template <uint8_t bits>
SerialMux<bits>::SerialMux(Stream& serial) {
  _serial = &serial;
}

/*
 * Destructor.
 */
template <uint8_t bits>
SerialMux<bits>::~SerialMux() {
  free(_buffer);
}


/*!
 * Add a virtual serial device.
 *
 * \return New virtual serial port.
 */
template <uint8_t bits>
uint8_t SerialMux<bits>::add() {
  _buffer = (Buffer<bits>*)realloc(
    (void*)_buffer, ++_ports * sizeof(Buffer<bits>));
  _buffer[_ports - 1] = Buffer<bits>();
  return _ports - 1;
}


/*!
 * Get the number of bytes available for reading.
 *
 * \param port Virtual serial port.
 *
 * \return Number of bytes.
 */
template <uint8_t bits>
size_t SerialMux<bits>::available(uint8_t port) {
  _update();
  return _buffer[port].available();
}

/*!
 * Read one byte of data.
 *
 * \param port Virtual serial port.
 *
 * \return The first byte of incoming data or `-1` if no data is available.
 */
template <uint8_t bits>
int SerialMux<bits>::read(uint8_t port) {
  _update();
  return _buffer[port].read();
}

/*!
 * Write one byte of data.
 *
 * \param port Virtual serial port.
 * \param data Data.
 *
 * \return Number of bytes written.
 */
template <uint8_t bits>
void SerialMux<bits>::write(uint8_t port, uint8_t data) {
  _update();
  _write(port, &data, 1);
}

/*!
 * Return the next byte of incoming data without removing it from the buffer.
 *
 * \param port Virtual serial port.
 *
 * \return The first byte of incoming data or `-1` if no data is available.
 */
template <uint8_t bits>
int SerialMux<bits>::peek(uint8_t port) {
  _update();
  return _buffer[port].peek();
}


/*
 * Control command handling.
 */
template <uint8_t bits>
void SerialMux<bits>::_control(uint8_t data) {
  switch (data) {
    case CMD_PROTOCOL:
      _write(_CONTROL_PORT, (uint8_t*)_PROTOCOL, sizeof(_PROTOCOL) - 1);
      return;
    case CMD_VERSION:
      _write(_CONTROL_PORT, (uint8_t*)_VERSION, sizeof(_VERSION) - 1);
      return;
    case CMD_GET_PORTS:
      _write(_CONTROL_PORT, &_ports, 1);
      return;
    case CMD_ENABLE:
      _enabled = true;
      break;
    case CMD_DISABLE:
      _enabled = false;
      break;
    case CMD_RESET:
      break;
  }
  _write(_CONTROL_PORT, (uint8_t*)"\x00", 1);
}

/*
 * Send incoming data to virtual serial ports.
 */
template <uint8_t bits>
void SerialMux<bits>::_update() {
  while (_serial->available()) {
    uint8_t data = _read();
    if (data == _ESCAPE) {
      uint8_t port = _read();
      if (port != _ESCAPE) {
        _portRx = port;
        continue;
      }
    }
    if (_portRx == _CONTROL_PORT) {
      _control(data);
      return;
    }
    _buffer[_portRx].write(data);
  }
}

/*
 * Blocking read.
 *
 * \return The first byte of incoming data.
 */
template <uint8_t bits>
uint8_t SerialMux<bits>::_read() {
  while (!_serial->available());
  return _serial->read();
}

/*
 * Write `size` bytes of data.
 *
 * \param port Virtual serial port.
 * \param data Buffer containing `size` bytes of data.
 * \param size Number of bytes to write.
 *
 * \return Number of bytes written.
 */
template <uint8_t bits>
void SerialMux<bits>::_write(uint8_t port, uint8_t* data, uint8_t size) {
  if (_enabled || port == _CONTROL_PORT) {
    if (port != _portTx) {
      _portTx = port;
      _serial->write(_ESCAPE);
      _serial->write(_portTx);
    }
    for (uint8_t i = 0; i < size; i++) {
      if (data[i] == _ESCAPE) {
        _serial->write(_ESCAPE);
      }
      _serial->write(data[i]);
    }
  }
}
