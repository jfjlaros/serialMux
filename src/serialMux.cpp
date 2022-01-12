#include "serialMux.h"

bool SerialMux::_enabled = false;
uint8_t SerialMux::_ids = 0;
uint8_t SerialMux::_lock = 0;
uint8_t SerialMux::_available = 0;


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

int SerialMux::_read(void) {
  while (!_serial->available());
  return _serial->read();
}

size_t SerialMux::_write(uint8_t data) {
  return _serial->write(data);
}

uint8_t SerialMux::_controlRead(void) {
  _available--;
  return _read();
}

void SerialMux::_controlWrite(uint8_t data) {
  _write('\0');
  _write('\1');
  _write(data);
}

void SerialMux::_control(void) {
  switch (_controlRead()) {
    case CMD_GET_PORTS:
      _controlWrite(_ids);
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
  _controlWrite('\0');
}

/*!
 * Get the number of bytes available for reading.
 *
 * \return Number of bytes.
 */
int SerialMux::available(void) {
  if (!_available && _serial->available()) {
    _lock = _read();
    _available = _read();
    if (!_lock) {
      _control();
      return 0;
    }
  }
  if (_lock == _id) {
    return _available;
  }
  return 0;
}

/*!
 * Read incoming data.
 *
 * \param data Data.
 * \param size Data size.
 *
 * \return Number of bytes read.
 */
size_t SerialMux::readBytes(uint8_t* data, size_t size) {
  if (available()) {
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
 * Read incoming data.
 *
 * \return The first byte of incoming data or -1 if no data is available.
 */
int SerialMux::read(void) {
  uint8_t data;
  if (readBytes(&data, 1)) {
    return data;
  }
  return -1;
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

/*!
 * Write outgoing data.
 *
 * \param data Data.
 *
 * \return Number of bytes written.
 */
size_t SerialMux::write(uint8_t data) {
  return write(&data, 1);
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

/*!
 * Write a C string.
 *
 * \param data Data.
 */
size_t SerialMux::print(char const data[]) {
  return write((uint8_t*)data, strlen(data));
}

/*!
 * Write a string.
 *
 * \param data Data.
 */
size_t SerialMux::print(String& data) {
  return write((uint8_t*)data.c_str(), data.length());
}
