#include "serialMux.h"

/*!
 * Constructor.
 *
 * \param serial Serial device.
 */
SerialMux::SerialMux(Control& control) {
  _control = &control;
  _id = _control->add();
}

/*!
 * Get the number of bytes available for reading.
 *
 * \return Number of bytes.
 */
int SerialMux::available(void) {
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
size_t SerialMux::readBytes(uint8_t* data, size_t size) {
  return _control->read(_id, data, size);
}

/*!
 * Read incoming data.
 *
 * \return The first byte of incoming data or -1 if no data is available.
 */
int SerialMux::read(void) {
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
size_t SerialMux::write(uint8_t* data, size_t size) {
  return _control->write(_id, data, size);
}

/*!
 * Write outgoing data.
 *
 * \param data Data.
 *
 * \return Number of bytes written.
 */
size_t SerialMux::write(uint8_t data) {
  return _control->write(_id, data);
}

/*!
 * Return the next byte of incoming data without removing it from the
 * internal buffer.
 *
 * \return The first byte of incoming data or -1 if no data is available.
 */
int SerialMux::peek(void) {
  return _control->peek(_id);
}

/*!
 * Write a C string.
 *
 * \param data Data.
 */
size_t SerialMux::print(char const data[]) {
  return write((uint8_t*)data, strlen(data));
}

/*!
 * Write a string.
 *
 * \param data Data.
 */
size_t SerialMux::print(String& data) {
  return write((uint8_t*)data.c_str(), data.length());
}
