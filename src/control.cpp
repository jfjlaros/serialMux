#include "control.h"


/*!
 * Constructor.
 *
 * \param serial Serial device.
 */
Control::Control(Stream& serial) {
  _serial = &serial;
}

/*!
 */
uint8_t Control::_read(uint8_t* data, uint8_t size) {
  while (!_serial->available());
  return _serial->readBytes(data, size);
}

/*!
 */
uint8_t Control::_write(uint8_t* data, uint8_t size) {
  return _serial->write(data, size);
}

/*!
 */
void Control::_control(void) {
  switch (read(0)) {
    case CMD_PROTOCOL:
      write(0, (uint8_t*)_PROTOCOL, sizeof(_PROTOCOL) - 1);
      return;
    case CMD_GET_PORTS:
      write(0, &_ids, 1);
      return;
    case CMD_ENABLE:
      Control::_enabled = true;
      break;
    case CMD_DISABLE:
      Control::_enabled = false;
      break;
    case CMD_RESET:
      break;
  }
  write(0, (uint8_t*)"\x00", 1);
}

/*!
 */
uint8_t Control::add(void) {
  return ++_ids;
}

/*!
 */
uint8_t Control::available(uint8_t id) {
  if (!_available && _serial->available()) {
    _read(&_lock, 1);
    _read(&_available, 1);
    if (!_lock) {
      _control();
      return 0;
    }
  }
  if (id == _lock) {
    return _available;
  }
  return 0;
}

/*!
 */
int16_t Control::peek(uint8_t id) {
  if (available(id)) {
    return _serial->peek();
  }
  return -1;
}

/*!
 */
uint8_t Control::read(uint8_t id, uint8_t* data, uint8_t size) {
  if (available(id)) {
    uint8_t size_ = _read(data, size);
    _available -= size_;
    if (!_available) {
      _lock = 0;
    }
    return size_;
  }
  return 0;
}

/*
 */
int16_t Control::read(uint8_t id) {
  if (available(id)) {
    uint8_t data;
    read(id, &data, 1);
    return data;
  }
  return -1;
}

/*!
 */
uint8_t Control::write(uint8_t id, uint8_t* data, uint8_t size) {
  if (_enabled || !id) {
    _write(&id, 1);
    _write(&size, 1);
    return _write(data, size);
  }
  return 0;
}

/*!
 */
uint8_t Control::write(uint8_t id, uint8_t data) {
  return write(id, &data, 1);
}
