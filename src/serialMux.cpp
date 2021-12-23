#include "serialMux.h"

#define CMD_GET_PORTS 0
#define CMD_ENABLE 1
#define CMD_DISABLE 2
#define CMD_RESET 3


bool SerialMux::_enabled = false;
uint8_t SerialMux::_ids = 0;


SerialMux::SerialMux(Stream& serial) {
  _serial = &serial;
  _ids++;
  _id = _ids;
}

void SerialMux::_control(void) {
  _serial->write('\0');
  switch (read()) {
    case CMD_GET_PORTS:
      _serial->write(_ids);
      return;
    case CMD_ENABLE:
      SerialMux::_enabled = true;
      break;
    case CMD_DISABLE:
      SerialMux::_enabled = false;
      break;
    case CMD_RESET:
      while (_serial->available()) {
        _serial->read();
      }
      _serial->flush();
      break;
  }
  _serial->write('\0');
}

int SerialMux::available(void) {
  if (_serial->available()) {
    uint8_t data = _serial->peek();

    if (!data) {
      _control();
    }
    if (_enabled && data == _id) {
      return 1;
    }
  }
  return 0;
}

int SerialMux::read(void) {
  while (!_serial->available());
  _serial->read();
  while (!_serial->available());
  return _serial->read();
}

size_t SerialMux::write(uint8_t data) {
  if (!_enabled) {
    return 0;
  }

  _serial->write(_id);

  return _serial->write(data);
}

int SerialMux::peek(void) {
  return _serial->peek();
}
