#ifndef SERIALMUX_H_
#define SERIALMUX_H_

#include <Arduino.h>

/*!
 * Serial multiplexer.
 */
class SerialMux : public Stream {
  public:
    SerialMux(Stream&);
    int available(void);
    int read(void);
    size_t write(uint8_t);
    int peek(void);
  private:
    static bool _enabled;
    static uint8_t _ids;
    uint8_t _id;
    Stream* _serial;
    void _control(void);
};

#endif
