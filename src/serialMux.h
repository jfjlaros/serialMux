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
    size_t readBytes(uint8_t*, size_t);
    size_t write(uint8_t);
    size_t write(uint8_t*, size_t);
    int peek(void);
  private:
    static bool _enabled;
    static uint8_t _ids;
    static uint8_t _lock;
    uint8_t _available;
    uint8_t _id;
    Stream* _serial;
    int _read(void);
    size_t _write(uint8_t);
    void _control(void);
};

#endif
