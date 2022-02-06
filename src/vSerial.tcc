#pragma once

#include "serialMux.tcc"

/*!
 * Virtual serial device.
 */
template <uint8_t bits=6>
class VSerial : public Stream {
  public:
    VSerial(SerialMux<bits>&);

    int available(void);
    int read(void);
    size_t readBytes(uint8_t*, size_t);
    size_t write(uint8_t*, size_t);
    size_t write(uint8_t);
    template <class T>
      size_t write(T);
    int peek(void);
    size_t print(char const[]);
    size_t print(String&);

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
int VSerial<bits>::available(void) {
  return _mux->available(_port);
}

/*!
 * Read one byte of data.
 *
 * \return The first byte of incoming data or `-1` if no data is available.
 */
template <uint8_t bits>
int VSerial<bits>::read(void) {
  return _mux->read(_port);
}

/*!
 * Read `size` bytes of data.
 *
 * \param data Buffer to receive `size` bytes of data.
 * \param size Number of bytes to read.
 *
 * \return Number of bytes read.
 */
template <uint8_t bits>
size_t VSerial<bits>::readBytes(uint8_t* data, size_t size) {
  return _mux->read(_port, data, size);
}

/*!
 * Write `size` bytes of data.
 *
 * \param data Buffer containing `size` bytes of data.
 * \param size Number of bytes to write.
 *
 * \return Number of bytes written.
 */
template <uint8_t bits>
size_t VSerial<bits>::write(uint8_t* data, size_t size) {
  return _mux->write(_port, data, size);
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
  return _mux->write(_port, data);
}

/*!
 * Write outgoing data.
 *
 * \param data Data.
 *
 * \return Number of bytes written.
 */
template <uint8_t bits>
template <class T>
size_t VSerial<bits>::write(T data) {
  return _mux->write(_port, (uint8_t*)&data, sizeof(T));
}

/*!
 * Return the next byte of incoming data without removing it from the buffer.
 *
 * \return The first byte of incoming data or `-1` if no data is available.
 */
template <uint8_t bits>
int VSerial<bits>::peek(void) {
  return _mux->peek(_port);
}

/*!
 * Write a C string.
 *
 * \param data Data.
 *
 * \return Number of bytes written.
 */
template <uint8_t bits>
size_t VSerial<bits>::print(char const data[]) {
  return _mux->write(_port, (uint8_t*)data, strlen(data));
}

/*!
 * Write a string.
 *
 * \param data Data.
 *
 * \return Number of bytes written.
 */
template <uint8_t bits>
size_t VSerial<bits>::print(String& data) {
  return _mux->write(_port, (uint8_t*)data.c_str(), data.length());
}
