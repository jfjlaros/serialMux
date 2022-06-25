#pragma once

#include "serialMux.tcc"

/*!
 * Virtual serial device.
 */
template <uint8_t bits=6>
class VSerial : public Stream {
  public:
    VSerial(SerialMux<bits>&);

    int available();
    int read();
    size_t write(uint8_t);
    int peek();

  private:
    SerialMux<bits>* _mux = NULL;
    uint8_t _port = 0;
};


/*!
 * Constructor.
 *
 * \param mux Serial multiplexer.
 */
template <uint8_t bits>
VSerial<bits>::VSerial(SerialMux<bits>& mux) {
  _mux = &mux;
  _port = _mux->add();
}


/*!
 * Get the number of bytes available for reading.
 *
 * \return Number of bytes.
 */
template <uint8_t bits>
int VSerial<bits>::available() {
  return _mux->available(_port);
}

/*!
 * Read one byte of data.
 *
 * \return The first byte of incoming data or `-1` if no data is available.
 */
template <uint8_t bits>
int VSerial<bits>::read() {
  return _mux->read(_port);
}

/*!
 * Write one byte of data.
 *
 * \param data Data.
 *
 * \return Number of bytes written.
 */
template <uint8_t bits>
size_t VSerial<bits>::write(uint8_t data) {
  _mux->write(_port, data);
  return 1;
}

/*!
 * Return the next byte of incoming data without removing it from the buffer.
 *
 * \return The first byte of incoming data or `-1` if no data is available.
 */
template <uint8_t bits>
int VSerial<bits>::peek() {
  return _mux->peek(_port);
}
