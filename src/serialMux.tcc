#pragma once

#include "buffer.tcc"

uint8_t const protocol_[] {'s', 'e', 'r', 'i', 'a', 'l', 'M', 'u', 'x'};
uint8_t const version_[] {2, 0, 0};
uint8_t const escape_ {0xff};
uint8_t const controlPort_ {0xfe};

enum Command : uint8_t {protocol, version, getPorts, enable, disable, reset};

/*! Serial multiplexer.
 *
 * \tparam maxPorts Maximum number of virtual serial ports.
 * \tparam bits Buffer size control (2^`bits`).
 */
template <uint8_t maxPorts, uint8_t bits>
class SerialMux_ {
public:
  /*! Create a serial multiplexer.
   *
   * \param[in] serial Serial device.
   */
  SerialMux_(Stream& serial);


  /*! Add a virtual serial device.
   *
   * \param[in] buffer Pointer to virtual serial device buffer.
   *
   * \return Virtual serial port number.
   */
  uint8_t add(Buffer<bits>* buffer);

  /*! Write one byte of data.
   *
   * \param[in] port Virtual serial port number.
   * \param[in] data Data.
   */
  void write(uint8_t const port, uint8_t const data);

  /*! Send incoming data to virtual serial ports. */
  void update();

private:
  void control_(uint8_t const);
  uint8_t read_() const;
  void write_(uint8_t const, uint8_t const* const, uint8_t const);

  Buffer<bits>* buffer_[maxPorts] {};
  bool enabled_ {false};
  uint8_t portRx_ {controlPort_};
  uint8_t portTx_ {controlPort_};
  uint8_t ports_ {0};
  Stream* serial_ {nullptr};
};

using SerialMux = SerialMux_<4, 6>;  //!< Serial multiplexer.


template <uint8_t maxPorts, uint8_t bits>
SerialMux_<maxPorts, bits>::SerialMux_(Stream& serial) : serial_ {&serial} {}


template <uint8_t maxPorts, uint8_t bits>
uint8_t SerialMux_<maxPorts, bits>::add(Buffer<bits>* buffer) {
  buffer_[ports_++] = buffer;
  return ports_ - 1;
}

template <uint8_t maxPorts, uint8_t bits>
void SerialMux_<maxPorts, bits>::write(
    uint8_t const port, uint8_t const data) {
  write_(port, &data, 1);
}

template <uint8_t maxPorts, uint8_t bits>
void SerialMux_<maxPorts, bits>::update() {
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
    buffer_[portRx_]->write(data);
  }
}


/* Control command handling. */
template <uint8_t maxPorts, uint8_t bits>
void SerialMux_<maxPorts, bits>::control_(uint8_t const data) {
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
  write(controlPort_, 0);
}

/*
 * Blocking read.
 *
 * \return The first byte of incoming data.
 */
template <uint8_t maxPorts, uint8_t bits>
uint8_t SerialMux_<maxPorts, bits>::read_() const {
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
template <uint8_t maxPorts, uint8_t bits>
void SerialMux_<maxPorts, bits>::write_(
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
