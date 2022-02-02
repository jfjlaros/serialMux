#pragma once

#include "control.h"

/*!
 * Serial multiplexer.
 */
class SerialMux : public Stream, Control {
  public:
    SerialMux(void);
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
    uint8_t _id = 0;
};


/*!
 * Write outgoing data.
 *
 * \param data Data.
 *
 * \return Number of bytes written.
 */
template <class T>
size_t SerialMux::write(T data) {
  return write((uint8_t*)&data, sizeof(T));
}
