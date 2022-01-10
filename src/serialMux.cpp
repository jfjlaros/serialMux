#include "serialMux.h"

bool SerialMux::_enabled = false;
uint8_t SerialMux::_ids = 0;
uint8_t SerialMux::_lock = 0;


/*!
 * Constructor.
 *
 * \param serial Serial device.
 */
SerialMux::SerialMux(Stream& serial) {
  _serial = &serial;
  _ids++;
  _id = _ids;
}

/*
 * Read incoming data.
 */
int SerialMux::_read(void) {
  while (!_serial->available());
  return _serial->read();
}

/*
 * Write outgoing data.
 */
size_t SerialMux::_write(uint8_t data) {
  return _serial->write(data);
}

/*
 * Control channel operations.
 */
void SerialMux::_control(void) {
  _read();
  _write('\0');
  _write('\1');
  switch (_read()) {
    case CMD_GET_PORTS:
      _write(_ids);
      return;
    case CMD_ENABLE:
      SerialMux::_enabled = true;
      break;
    case CMD_DISABLE:
      SerialMux::_enabled = false;
      break;
    case CMD_RESET:
      while (_serial->available()) {
        _read();
      }
      break;
  }
  _write('\0');
}

/*!
 * Get the number of bytes available for reading.
 *
 * \return Number of bytes.
 */
int SerialMux::available(void) {
  if (!_available && _serial->available()) {
    if (!_lock) {
      _lock = _read();
      if (!_lock) {
        _control();
        return 0;
      }
    }
    if (_enabled && _lock == _id) {
      _available = _read();
    }
  }
  return _available;
}

/*!
 * Read incoming data.
 *
 * \return The first byte of incoming data or -1 if no data is available.
 */
int SerialMux::read(void) {
  if (available() && _lock == _id) {
    _available--;
    if (!_available) {
      _lock = 0;
    }
    return _read();
  }
  return -1;
}

/*!
 * Read incoming data.
 *
 * \param data Data.
 * \param size Data size.
 *
 * \return Number of bytes written.
 */
size_t SerialMux::readBytes(uint8_t* data, size_t size) {
  if (_available && _lock == _id) {
    size_t i;
    for (i = 0; i < size; i++) {
      data[i] = _read();

      _available--;
      if (!_available) {
        _lock = 0;
        return i + 1;
      }
    }
    return size;
  }
  return 0;
}

/*!
 * Write outgoing data.
 *
 * \param data Data.
 *
 * \return Number of bytes written.
 */
size_t SerialMux::write(uint8_t data) {
  if (_enabled) {
    _write(_id);
    _write('\1');
    return _write(data);
  }
  return 0;
}

/*!
 * Write outgoing data.
 *
 * \param data Data.
 * \param size Data size.
 *
 * \return Number of bytes written.
 */
size_t SerialMux::write(uint8_t* data, size_t size) {
  if (_enabled) {
    _write(_id);
    _write((uint8_t)size);

    size_t i;
    for (i = 0; i < size && _write(data[i]); i++);
    return i;
  }
  return 0;
}

size_t SerialMux::print(char const data[]) {
  return write((uint8_t*)data, strlen(data));
}

size_t SerialMux::print(String& data) {
  return write((uint8_t*)data.c_str(), data.length());
}

/*!
 * Return the next byte of incoming data without removing it from the
 * internal buffer.
 *
 * \return The first byte of incoming data or -1 if no data is available.
 */
int SerialMux::peek(void) {
  if (available()) {
    return _serial->peek();
  }
  return -1;
}
