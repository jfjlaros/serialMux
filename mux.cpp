#include "mux.h"


Mux::Mux(Stream& serial, uint8_t id) {
  _serial = &serial;
  _id = id;
}

int Mux::available(void) {
  return _serial->available() && _serial->peek() == _id;
}

int Mux::read(void) {
  while (!_serial->available());
  _serial->read();
  while (!_serial->available());
  return _serial->read();
}

size_t Mux::write(uint8_t data) {
  _serial->write(_id);

  return _serial->write(data);
}

int Mux::peek(void) {
  return _serial->peek();
}
