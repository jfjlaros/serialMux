#include "control.h"

uint8_t Control::_available = 0;
bool Control::_enabled = false;
uint8_t Control::_ids = 0;
uint8_t Control::_lock = 0;
Stream* Control::_serial = NULL;


/*!
 * Constructor.
 *
 * \param serial Serial device.
 */
Control::Control(Stream& serial) {
  _serial = &serial;
}

int Control::_read(void) {
  while (!_serial->available());
  return _serial->read();
}

size_t Control::_write(uint8_t data) {
  return _serial->write(data);
}

uint8_t Control::_controlRead(void) {
  _available--;
  return _read();
}

void Control::_controlWrite(uint8_t* data, uint8_t size) {
  _write('\x00');
  _write(size);
  for (uint8_t i = 0; i < size; i++) {
    _write(data[i]);
  }
}

void Control::_control(void) {
  switch (_controlRead()) {
    case CMD_PROTOCOL:
      _controlWrite((uint8_t*)_PROTOCOL, sizeof(_PROTOCOL) - 1);
      return;
    case CMD_GET_PORTS:
      _controlWrite(&_ids, 1);
      return;
    case CMD_ENABLE:
      Control::_enabled = true;
      break;
    case CMD_DISABLE:
      Control::_enabled = false;
      break;
    case CMD_RESET:
      while (_serial->available()) {
        _read();
      }
      break;
  }
  _controlWrite((uint8_t*)"\x00", 1);
}
