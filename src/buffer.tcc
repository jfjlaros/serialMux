#pragma once

#include <Arduino.h>

/*!
 */
template <uint8_t bits>
class Buffer {
  public:
    Buffer(void);
    uint8_t available(void);
    uint8_t read(uint8_t*, uint8_t);
    int16_t read(void);
    void write(uint8_t*, uint8_t);
    void write(uint8_t);
    int16_t peek(void);

  private:
    uint8_t _buffer[1ul << bits] = {};
    uint8_t _start: bits;
    uint8_t _end: bits;
    uint8_t _mask = (1ul << bits) - 1;
};


/*!
 */
template <uint8_t bits>
Buffer<bits>::Buffer(void) {
  _start = 0;
  _end = 0;
}

/*!
 */
template <uint8_t bits>
uint8_t Buffer<bits>::available(void) {
  return (_end - _start) & _mask;
}

/*!
 */
template <uint8_t bits>
uint8_t Buffer<bits>::read(uint8_t* data, uint8_t size) {
  uint8_t size_ = min(size, available());
  for (uint8_t i = 0; i < size_; i++) {
    data[i] = _buffer[_start++];
  }
  return size_;
}

/*!
 */
template <uint8_t bits>
int16_t Buffer<bits>::read(void) {
  if (_start != _end) {
    return _buffer[_start++];
  }
  return -1;
}

/*!
 */
template <uint8_t bits>
void Buffer<bits>::write(uint8_t* data, uint8_t size) {
  for (uint8_t i = 0; i < size; i++) {
    _buffer[_end++] = data[i];
  }
}

/*!
 */
template <uint8_t bits>
void Buffer<bits>::write(uint8_t data) {
  _buffer[_end++] = data;
}

/*!
 */
template <uint8_t bits>
int16_t Buffer<bits>::peek(void) {
  if (_start != _end) {
    return _buffer[_start];
  }
  return -1;
}
