#pragma once

#include "buffer.tcc"

uint8_t const protocol_[] =  {'s', 'e', 'r', 'i', 'a', 'l', 'M', 'u', 'x'};
uint8_t const version_[] =  {2, 0, 0};
uint8_t const escape_ = 0xff;
uint8_t const controlPort_ = 0xfe;

// Control commands.
uint8_t const cmdProtocol_ = 0x00;
uint8_t const cmdVersion_ = 0x01;
uint8_t const cmdGet_ports_ = 0x02;
uint8_t const cmdEnable_ = 0x03;
uint8_t const cmdDisable_ = 0x04;
uint8_t const cmdReset_ = 0x05;

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
  SerialMux_(Stream&);

  ~SerialMux_();


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
  size_t available(uint8_t const);

  /*! Read one byte of data.
   *
   * \param[in] port Virtual serial port.
   *
   * \return The first byte of incoming data or `-1` if no data is available.
   */
  int read(uint8_t const);

  /*! Write one byte of data.
   *
   * \param[in] port Virtual serial port.
   * \param[in] data Data.
   *
   * \return Number of bytes written.
   */
  void write(uint8_t const, uint8_t const);

  /*! Return the next byte of incoming data without removing it from the
   * buffer.
   *
   * \param[in] port Virtual serial port.
   *
   * \return The first byte of incoming data or `-1` if no data is available.
   */
  int peek(uint8_t const);

private:
  void control_(uint8_t const);
  void update_();

  uint8_t read_() const;
  void write_(uint8_t const, uint8_t const* const, uint8_t const);

  Buffer<bits>* buffer_ = nullptr;
  bool enabled_ = false;
  uint8_t portRx_ = controlPort_;
  uint8_t portTx_ = controlPort_;
  uint8_t ports_ = 0;
  Stream* serial_ = nullptr;
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
  Buffer<bits>* newBuffer = new Buffer<bits>[++ports_];
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


/*
 * Control command handling.
 */
template <uint8_t bits>
void SerialMux_<bits>::control_(uint8_t const data) {
  switch (data) {
    case cmdProtocol_:
      write_(controlPort_, protocol_, sizeof(protocol_));
      return;
    case cmdVersion_:
      write_(controlPort_, version_, sizeof(version_));
      return;
    case cmdGet_ports_:
      write_(controlPort_, &ports_, 1);
      return;
    case cmdEnable_:
      enabled_ = true;
      break;
    case cmdDisable_:
      enabled_ = false;
      break;
    case cmdReset_:
      break;
  }
  uint8_t const data_ = 0;
  write_(controlPort_, &data_, 1);
}

/*
 * Send incoming data to virtual serial ports.
 */
template <uint8_t bits>
void SerialMux_<bits>::update_() {
  while (serial_->available()) {
    uint8_t data = read_();
    if (data == escape_) {
      uint8_t port = read_();
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

/*
 * Write `size` bytes of data.
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
    for (uint8_t i = 0; i < size; i++) {
      if (data[i] == escape_) {
        serial_->write(escape_);
      }
      serial_->write(data[i]);
    }
  }
}
