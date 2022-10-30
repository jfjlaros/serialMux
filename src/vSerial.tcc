#pragma once

#include "serialMux.tcc"

/*! Virtual serial device.
 *
 * \tparam bits Buffer size control (2^`bits`).
 */
template <uint8_t bits=6>
class VSerial_ : public Stream {
public:
  /*! Create a virtual serial device.
   *
   * \param[in] mux Serial multiplexer.
   */
  VSerial_(SerialMux_<bits>&);


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
  size_t write(uint8_t);

  /*! Return the next byte of incoming data without removing it from the
   * buffer.
   *
   * \return The first byte of incoming data or `-1` if no data is available.
   */
  int peek();

private:
  SerialMux_<bits>* _mux {nullptr};
  uint8_t _port {0};
};

using VSerial = VSerial_<>;  //!< Virtual serial device.


template <uint8_t bits>
VSerial_<bits>::VSerial_(SerialMux_<bits>& mux) {
  _mux = &mux;
  _port = _mux->add();
}


template <uint8_t bits>
int VSerial_<bits>::available() {
  return _mux->available(_port);
}

template <uint8_t bits>
int VSerial_<bits>::read() {
  return _mux->read(_port);
}

template <uint8_t bits>
size_t VSerial_<bits>::write(uint8_t data) {
  _mux->write(_port, data);
  return 1;
}

template <uint8_t bits>
int VSerial_<bits>::peek() {
  return _mux->peek(_port);
}
