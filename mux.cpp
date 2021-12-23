#include "mux.h"

uint8_t Mux::_ids = 0;
bool Mux::_enabled = false;

Mux::Mux(Stream& serial) {
  _serial = &serial;
  _ids++;
  _id = _ids;
}

void Mux::_control(void) {
  _serial->write('\0');
  switch (read()) {
    case 0:
      _serial->write(_ids);
      return;
    case 1:
      Mux::_enabled = true;
      break;
    case 2:
      Mux::_enabled = false;
      break;
    case 3:
      while (_serial->available()) {
        _serial->read();
      }
      _serial->flush();
      break;
  }
  _serial->write('\0');
}

int Mux::available(void) {
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

int Mux::read(void) {
  while (!_serial->available());
  _serial->read();
  while (!_serial->available());
  return _serial->read();
}

size_t Mux::write(uint8_t data) {
  if (!_enabled) {
    return 0;
  }

  _serial->write(_id);

  return _serial->write(data);
}

int Mux::peek(void) {
  return _serial->peek();
}
