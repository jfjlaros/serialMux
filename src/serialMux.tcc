#pragma once

#include "buffer.tcc"

uint8_t const protocol_[] {'s', 'e', 'r', 'i', 'a', 'l', 'M', 'u', 'x'};
uint8_t const version_[] {2, 0, 0};
uint8_t const escape_ {0xff};
uint8_t const controlPort_ {0xfe};

enum Command : uint8_t {protocol, version, getPorts, enable, disable, reset};

/*! Serial multiplexer.
 *
 * \tparam bits Buffer size control (2^`bits`).
 */
template <uint8_t bits=6>
class SerialMux_ {
public:
  /*! Create a serial multiplexer.
   *
   * \param[in] serial Serial device.
   */
  SerialMux_(Stream& serial);

  SerialMux_(SerialMux_ const&) = delete;

  ~SerialMux_();


  SerialMux_& operator =(SerialMux_ const&) = delete;


  /*! Add a virtual serial device.
   *
   * \return New virtual serial port.
   */
  uint8_t add();


  /*! Get the number of bytes available for reading.
   *
   * \param[in] port Virtual serial port.
   *
   * \return Number of bytes.
   */
  size_t available(uint8_t const port);

  /*! Read one byte of data.
   *
   * \param[in] port Virtual serial port.
   *
   * \return The first byte of incoming data or `-1` if no data is available.
   */
  int read(uint8_t const port);

  /*! Write one byte of data.
   *
   * \param[in] port Virtual serial port.
   * \param[in] data Data.
   *
   * \return Number of bytes written.
   */
  void write(uint8_t const port, uint8_t const data);

  /*! Return the next byte of incoming data without removing it from the
   * buffer.
   *
   * \param[in] port Virtual serial port.
   *
   * \return The first byte of incoming data or `-1` if no data is available.
   */
  int peek(uint8_t const port);

private:
  void control_(uint8_t const);
  void update_();

  uint8_t read_() const;
  void write_(uint8_t const, uint8_t const* const, uint8_t const);

  Buffer<bits>* buffer_ {nullptr};
  bool enabled_ {false};
  uint8_t portRx_ {controlPort_};
  uint8_t portTx_ {controlPort_};
  uint8_t ports_ {0};
  Stream* serial_ {nullptr};
};

using SerialMux = SerialMux_<>;  //!< Serial multiplexer.


template <uint8_t bits>
SerialMux_<bits>::SerialMux_(Stream& serial) {
  serial_ = &serial;
}

template <uint8_t bits>
SerialMux_<bits>::~SerialMux_() {
  delete[] buffer_;
}


template <uint8_t bits>
uint8_t SerialMux_<bits>::add() {
  Buffer<bits>* newBuffer {new Buffer<bits>[++ports_]};
  memcpy(newBuffer, buffer_, (ports_ - 1) * sizeof(Buffer<bits>));
  delete[] buffer_;
  buffer_ = newBuffer;
  return ports_ - 1;
}


template <uint8_t bits>
size_t SerialMux_<bits>::available(uint8_t const port) {
  update_();
  return buffer_[port].available();
}

template <uint8_t bits>
int SerialMux_<bits>::read(uint8_t const port) {
  update_();
  return buffer_[port].read();
}

template <uint8_t bits>
void SerialMux_<bits>::write(uint8_t const port, uint8_t const data) {
  update_();
  write_(port, &data, 1);
}

template <uint8_t const bits>
int SerialMux_<bits>::peek(uint8_t const port) {
  update_();
  return buffer_[port].peek();
}


/* Control command handling. */
template <uint8_t bits>
void SerialMux_<bits>::control_(uint8_t const data) {
  switch (static_cast<Command>(data)) {
    case protocol:
      write_(controlPort_, protocol_, sizeof(protocol_));
      return;
    case version:
      write_(controlPort_, version_, sizeof(version_));
      return;
    case getPorts:
      write_(controlPort_, &ports_, 1);
      return;
    case enable:
      enabled_ = true;
      break;
    case disable:
      enabled_ = false;
      break;
    case reset:
      break;
  }
  uint8_t const data_ {0};
  write_(controlPort_, &data_, 1);
}

/*
 * Send incoming data to virtual serial ports.
 */
template <uint8_t bits>
void SerialMux_<bits>::update_() {
  while (serial_->available()) {
    uint8_t data {read_()};
    if (data == escape_) {
      uint8_t port {read_()};
      if (port != escape_) {
        portRx_ = port;
        continue;
      }
    }
    if (portRx_ == controlPort_) {
      control_(data);
      return;
    }
    buffer_[portRx_].write(data);
  }
}

/*
 * Blocking read.
 *
 * \return The first byte of incoming data.
 */
template <uint8_t bits>
uint8_t SerialMux_<bits>::read_() const {
  while (!serial_->available());
  return serial_->read();
}

/* Write `size` bytes of data.
 *
 * \param[in] port Virtual serial port.
 * \param[in] data Buffer containing `size` bytes of data.
 * \param[in] size Number of bytes to write.
 *
 * \return Number of bytes written.
 */
template <uint8_t bits>
void SerialMux_<bits>::write_(
    uint8_t const port, uint8_t const* const data, uint8_t const size) {
  if (enabled_ || port == controlPort_) {
    if (port != portTx_) {
      portTx_ = port;
      serial_->write(escape_);
      serial_->write(portTx_);
    }
    for (uint8_t i {0}; i < size; i++) {
      if (data[i] == escape_) {
        serial_->write(escape_);
      }
      serial_->write(data[i]);
    }
  }
}
