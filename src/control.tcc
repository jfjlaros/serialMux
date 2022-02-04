#pragma once

#include "buffer.tcc"

#define _PROTOCOL "serialMux\x01\x00\x00"

// Control commands.
#define CMD_PROTOCOL  '\x00'
#define CMD_GET_PORTS '\x01'
#define CMD_ENABLE    '\x02'
#define CMD_DISABLE   '\x03'
#define CMD_RESET     '\x04'

/*!
 */
template <uint8_t bits=6>
class Control {
  public:
    Control(Stream&);
    ~Control(void);
    uint8_t add(void);

    size_t available(uint8_t);
    int peek(uint8_t);
    size_t read(uint8_t, uint8_t*, uint8_t);
    size_t read(uint8_t);
    size_t write(uint8_t, uint8_t*, uint8_t);
    size_t write(uint8_t, uint8_t);

  private:
    Buffer<bits>* _buffer = NULL;
    bool _enabled = false;
    uint8_t _id = 0;
    Stream* _serial = NULL;

    void _control(void);
    void _update(void);
    uint8_t _read(void);
};


/*!
 * Constructor.
 *
 * \param serial Serial device.
 */
template <uint8_t bits>
Control<bits>::Control(Stream& serial) {
  _serial = &serial;
}

/*!
 * Destructor.
 */
template <uint8_t bits>
Control<bits>::~Control(void) {
  free(_buffer);
}

/*
 */
template <uint8_t bits>
uint8_t Control<bits>::_read(void) {
  while (!_serial->available());
  return _serial->read();
}

/*
 */
template <uint8_t bits>
void Control<bits>::_control(void) {
  switch (_read()) {
    case CMD_PROTOCOL:
      write(0, (uint8_t*)_PROTOCOL, sizeof(_PROTOCOL) - 1);
      return;
    case CMD_GET_PORTS:
      write(0, _id);
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
  write(0, 0);
}

/*
 */
template <uint8_t bits>
void Control<bits>::_update(void) {
  if (_serial->available()) {
    uint8_t id = _read();
    uint8_t size = _read();
    if (!id) {
      _control();
      return;
    }
    for (uint8_t i = 0; i < size; i++) {
      _buffer[id].write(_read());
    }
  }
}

/*!
 */
template <uint8_t bits>
uint8_t Control<bits>::add(void) {
  _buffer = (Buffer<bits>*)realloc(
    (void*)_buffer, ++_id * sizeof(Buffer<bits>));
  _buffer[_id] = Buffer<bits>();
  return _id;
}


/*!
 */
template <uint8_t bits>
size_t Control<bits>::available(uint8_t id) {
  _update();
  return _buffer[id].available();
}

/*!
 */
template <uint8_t bits>
int Control<bits>::peek(uint8_t id) {
  _update();
  return _buffer[id].peek();
}

/*
 */
template <uint8_t bits>
size_t Control<bits>::read(uint8_t id, uint8_t* data, uint8_t size) {
  _update();
  return _buffer[id].read(data, size);
}

/*
 */
template <uint8_t bits>
size_t Control<bits>::read(uint8_t id) {
  _update();
  return _buffer[id].read();
}

/*!
 */
template <uint8_t bits>
size_t Control<bits>::write(uint8_t id, uint8_t* data, uint8_t size) {
  _update();
  if (_enabled || !id) {
    _serial->write(id);
    _serial->write(size);
    return _serial->write(data, size);
  }
  return 0;
}

/*!
 */
template <uint8_t bits>
size_t Control<bits>::write(uint8_t id, uint8_t data) {
  _update();
  if (_enabled || !id) {
    _serial->write(id);
    _serial->write('\x01');
    return _serial->write(data);
  }
  return 0;
}
