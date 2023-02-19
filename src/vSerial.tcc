#pragma once

#include "buffer.tcc"
#include "serialMux.tcc"

/*! Virtual serial device.
 *
 * \tparam maxPorts Maximum number of virtual serial ports.
 * \tparam bits Buffer size control (2^`bits`).
 */
template <uint8_t maxPorts, uint8_t bits>
class VSerial_ : public Stream {
public:
  /*! Create a virtual serial device.
   *
   * \param[in] mux Serial multiplexer.
   */
  VSerial_(SerialMux_<maxPorts, bits>& mux);


  /*! Get the number of bytes available for reading.
   *
   * \return Number of bytes.
   */
  int available();

  /*! Read one byte of data.
   *
   * \return The first byte of incoming data or `-1` if no data is available.
   */
  int read();

  /*! Write one byte of data.
   *
   * \param[in] data Data.
   *
   * \return Number of bytes written.
   */
  size_t write(uint8_t data);

  /*! Return the next byte of incoming data without removing it from the
   * buffer.
   *
   * \return The first byte of incoming data or `-1` if no data is available.
   */
  int peek();

private:
  Buffer<bits> buffer_ {};
  SerialMux_<maxPorts, bits>* mux_ {nullptr};
  uint8_t port_ {0};
};

using VSerial = VSerial_<4, 6>;  //!< Virtual serial device.


template <uint8_t maxPorts, uint8_t bits>
VSerial_<maxPorts, bits>::VSerial_(SerialMux_<maxPorts, bits>& mux)
    : mux_ {&mux}, port_ {mux.add(&buffer_)} {}


template <uint8_t maxPorts, uint8_t bits>
int VSerial_<maxPorts, bits>::available() {
  mux_->update();
  return buffer_.available();
}

template <uint8_t maxPorts, uint8_t bits>
int VSerial_<maxPorts, bits>::read() {
  mux_->update();
  return buffer_.read();
}

template <uint8_t maxPorts, uint8_t bits>
size_t VSerial_<maxPorts, bits>::write(uint8_t data) {
  mux_->update();
  mux_->write(port_, data);
  return 1;
}

template <uint8_t maxPorts, uint8_t bits>
int VSerial_<maxPorts, bits>::peek() {
  mux_->update();
  return buffer_.peek();
}
