#pragma once

#include "buffer.tcc"

#define _PROTOCOL "serialMux"
#define _VERSION "\x01\x00\x00"
#define _CONTROL_PORT 0xff

// Control commands.
#define CMD_PROTOCOL  '\x00'
#define CMD_VERSION   '\x01'
#define CMD_GET_PORTS '\x02'
#define CMD_ENABLE    '\x03'
#define CMD_DISABLE   '\x04'
#define CMD_RESET     '\x05'

/*!
 * Serial multiplexer.
 */
template <uint8_t bits=6>
class SerialMux {
  public:
    SerialMux(Stream&);
    ~SerialMux(void);

    uint8_t add(void);

    size_t available(uint8_t);
    size_t read(uint8_t, uint8_t*, uint8_t);
    int read(uint8_t);
    size_t write(uint8_t, uint8_t*, uint8_t);
    size_t write(uint8_t, uint8_t);
    int peek(uint8_t);

  private:
    Buffer<bits>* _buffer = NULL;
    bool _enabled = false;
    uint8_t _id = 0;
    Stream* _serial = NULL;

    void _control(void);
    void _update(void);

    uint8_t _read(void);
    size_t _write(uint8_t, uint8_t*, uint8_t);
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
SerialMux<bits>::~SerialMux(void) {
  free(_buffer);
}


/*!
 * Add a virtual serial device.
 *
 * \return New virtual serial device ID.
 */
template <uint8_t bits>
uint8_t SerialMux<bits>::add(void) {
  _buffer = (Buffer<bits>*)realloc(
    (void*)_buffer, ++_id * sizeof(Buffer<bits>));
  _buffer[_id - 1] = Buffer<bits>();
  return _id - 1;
}


/*!
 * Get the number of bytes available for reading.
 *
 * \param id Virtual device ID.
 *
 * \return Number of bytes.
 */
template <uint8_t bits>
size_t SerialMux<bits>::available(uint8_t id) {
  _update();
  return _buffer[id].available();
}

/*!
 * Read `size` bytes of data.
 *
 * \param id Virtual device ID.
 * \param data Buffer to receive `size` bytes of data.
 * \param size Number of bytes to read.
 *
 * \return Number of bytes read.
 */
template <uint8_t bits>
size_t SerialMux<bits>::read(uint8_t id, uint8_t* data, uint8_t size) {
  _update();
  return _buffer[id].read(data, size);
}

/*!
 * Read one byte of data.
 *
 * \param id Virtual device ID.
 *
 * \return The first byte of incoming data or `-1` if no data is available.
 */
template <uint8_t bits>
int SerialMux<bits>::read(uint8_t id) {
  _update();
  return _buffer[id].read();
}

/*!
 * Write `size` bytes of data.
 *
 * \param id Virtual device ID.
 * \param data Buffer containing `size` bytes of data.
 * \param size Number of bytes to write.
 *
 * \return Number of bytes written.
 */
template <uint8_t bits>
size_t SerialMux<bits>::write(uint8_t id, uint8_t* data, uint8_t size) {
  _update();
  return _write(id, data, size);
}

/*!
 * Write one byte of data.
 *
 * \param id Virtual device ID.
 * \param data Data.
 *
 * \return Number of bytes written.
 */
template <uint8_t bits>
size_t SerialMux<bits>::write(uint8_t id, uint8_t data) {
  _update();
  return _write(id, &data, 1);
}

/*!
 * Return the next byte of incoming data without removing it from the buffer.
 *
 * \param id Virtual device ID.
 *
 * \return The first byte of incoming data or `-1` if no data is available.
 */
template <uint8_t bits>
int SerialMux<bits>::peek(uint8_t id) {
  _update();
  return _buffer[id].peek();
}


/*
 * Control command handling.
 */
template <uint8_t bits>
void SerialMux<bits>::_control(void) {
  switch (_read()) {
    case CMD_PROTOCOL:
      write(_CONTROL_PORT, (uint8_t*)_PROTOCOL, sizeof(_PROTOCOL) - 1);
      return;
    case CMD_VERSION:
      write(_CONTROL_PORT, (uint8_t*)_VERSION, sizeof(_VERSION) - 1);
      return;
    case CMD_GET_PORTS:
      write(_CONTROL_PORT, _id);
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
  write(_CONTROL_PORT, 0);
}

/*
 * Distribute incoming data over virtual serial devices.
 */
template <uint8_t bits>
void SerialMux<bits>::_update(void) {
  if (_serial->available()) {
    uint8_t id = _read();
    uint8_t size = _read();
    if (id == _CONTROL_PORT) {
      _control();
      return;
    }
    for (uint8_t i = 0; i < size; i++) {
      _buffer[id].write(_read());
    }
  }
}


/*
 * Blocking read.
 *
 * \return The first byte of incoming data.
 */
template <uint8_t bits>
uint8_t SerialMux<bits>::_read(void) {
  while (!_serial->available());
  return _serial->read();
}

/*
 * Write `size` bytes of data.
 *
 * \param id Virtual device ID.
 * \param data Buffer containing `size` bytes of data.
 * \param size Number of bytes to write.
 *
 * \return Number of bytes written.
 */
template <uint8_t bits>
size_t SerialMux<bits>::_write(uint8_t id, uint8_t* data, uint8_t size) {
  if (_enabled || id == _CONTROL_PORT) {
    _serial->write(id);
    _serial->write(size);
    return _serial->write(data, size);
  }
  return 0;
}
