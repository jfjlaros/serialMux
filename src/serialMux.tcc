#pragma once

#include "control.tcc"

/*!
 * Serial multiplexer.
 */
template <uint8_t bits=6>
class SerialMux : public Stream {
  public:
    SerialMux(Control<bits>&);
    int available(void);
    size_t readBytes(uint8_t*, size_t);
    int read(void);
    size_t write(uint8_t*, size_t);
    size_t write(uint8_t);
    template <class T>
      size_t write(T);
    int peek(void);
    size_t print(char const[]);
    size_t print(String&);

  private:
    Control<bits>* _control = NULL;
    uint8_t _id = 0;
};


/*!
 * Write outgoing data.
 *
 * \param data Data.
 *
 * \return Number of bytes written.
 */
template <uint8_t bits>
template <class T>
size_t SerialMux<bits>::write(T data) {
  return write((uint8_t*)&data, sizeof(T));
}
/*!
 * Constructor.
 *
 * \param serial Serial device.
 */
template <uint8_t bits>
SerialMux<bits>::SerialMux(Control<bits>& control) {
  _control = &control;
  _id = _control->add();
}

/*!
 * Get the number of bytes available for reading.
 *
 * \return Number of bytes.
 */
template <uint8_t bits>
int SerialMux<bits>::available(void) {
  return _control->available(_id);
}

/*!
 * Read incoming data.
 *
 * \param data Data.
 * \param size Data size.
 *
 * \return Number of bytes read.
 */
template <uint8_t bits>
size_t SerialMux<bits>::readBytes(uint8_t* data, size_t size) {
  return _control->read(_id, data, size);
}

/*!
 * Read incoming data.
 *
 * \return The first byte of incoming data or -1 if no data is available.
 */
template <uint8_t bits>
int SerialMux<bits>::read(void) {
  return _control->read(_id);
}

/*!
 * Write outgoing data.
 *
 * \param data Data.
 * \param size Data size.
 *
 * \return Number of bytes written.
 */
template <uint8_t bits>
size_t SerialMux<bits>::write(uint8_t* data, size_t size) {
  return _control->write(_id, data, size);
}

/*!
 * Write outgoing data.
 *
 * \param data Data.
 *
 * \return Number of bytes written.
 */
template <uint8_t bits>
size_t SerialMux<bits>::write(uint8_t data) {
  return _control->write(_id, data);
}

/*!
 * Return the next byte of incoming data without removing it from the
 * internal buffer.
 *
 * \return The first byte of incoming data or -1 if no data is available.
 */
template <uint8_t bits>
int SerialMux<bits>::peek(void) {
  return _control->peek(_id);
}

/*!
 * Write a C string.
 *
 * \param data Data.
 */
template <uint8_t bits>
size_t SerialMux<bits>::print(char const data[]) {
  return write((uint8_t*)data, strlen(data));
}

/*!
 * Write a string.
 *
 * \param data Data.
 */
template <uint8_t bits>
size_t SerialMux<bits>::print(String& data) {
  return write((uint8_t*)data.c_str(), data.length());
}
