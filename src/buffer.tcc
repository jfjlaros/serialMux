#pragma once

#include <Arduino.h>

/*! Ring buffer.
 *
 * \tparam bits Buffer size control (2^`bits`).
 */
template <uint8_t bits>
class Buffer {
public:
  /*! Create a ring buffer. */
  Buffer();


  /*! Get the number of bytes available for reading.
   *
   * \return Number of bytes.
   */
  uint8_t available() const;

  /*! Read `size` bytes of data.
   *
   * \param[out] data Buffer to receive `size` bytes of data.
   * \param[in] size Number of bytes to read.
   *
   * \return Number of bytes read.
   */
  uint8_t read(uint8_t* const, uint8_t const);

  /*! Read one byte of data.
   *
   * \return The first byte of incoming data or `-1` if no data is available.
   */
  int16_t read();

  /*! Write `size` bytes of data.
   *
   * \param[in] data Buffer containing `size` bytes of data.
   * \param[in] size Number of bytes to write.
   *
   * \return Number of bytes written.
   */
  void write(uint8_t* const, uint8_t const);

  /*! Write one byte of data.
   *
   * \param[in] data Data.
   */
  void write(uint8_t const);

  /*! Return the next byte of incoming data without removing it from the
   * buffer.
   *
   * \return The first byte of incoming data or `-1` if no data is available.
   */
  int16_t peek() const;

private:
  uint8_t buffer_[1ul << bits]{};
  uint8_t start_: bits;
  uint8_t end_: bits;
  uint8_t mask_ = (1ul << bits) - 1;
};


template <uint8_t bits>
Buffer<bits>::Buffer() {
  start_ = 0;
  end_ = 0;
}


template <uint8_t bits>
uint8_t Buffer<bits>::available() const {
  return (end_ - start_) & mask_;
}

template <uint8_t bits>
uint8_t Buffer<bits>::read(uint8_t* const data, uint8_t const size) {
  uint8_t size_ = min(size, available());
  for (uint8_t i = 0; i < size_; i++) {
    data[i] = buffer_[start_++];
  }
  return size_;
}

template <uint8_t bits>
int16_t Buffer<bits>::read() {
  if (start_ != end_) {
    return buffer_[start_++];
  }
  return -1;
}

template <uint8_t bits>
void Buffer<bits>::write(uint8_t* const data, uint8_t size) {
  for (uint8_t i = 0; i < size; i++) {
    buffer_[end_++] = data[i];
  }
}

template <uint8_t bits>
void Buffer<bits>::write(uint8_t data) {
  buffer_[end_++] = data;
}

template <uint8_t bits>
int16_t Buffer<bits>::peek() const {
  if (start_ != end_) {
    return buffer_[start_];
  }
  return -1;
}
