#ifndef SERIALMUX_H_
#define SERIALMUX_H_

#include <Arduino.h>

#define CMD_GET_PORTS '\0'  //!< Get ports control command.
#define CMD_ENABLE    '\1'  //!< Enable control command.
#define CMD_DISABLE   '\2'  //!< Disable control command.
#define CMD_RESET     '\3'  //!< Reset control command.

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
    uint8_t _available = 0;
    uint8_t _id = 0;
    Stream* _serial = NULL;
    int _read(void);
    size_t _write(uint8_t);
    void _control(void);
};

#endif