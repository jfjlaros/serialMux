#ifndef SERIALMUX_H_
#define SERIALMUX_H_

#include <Arduino.h>

// Control commands.
#define CMD_GET_PORTS '\0'
#define CMD_ENABLE    '\1'
#define CMD_DISABLE   '\2'
#define CMD_RESET     '\3'

/*!
 * Serial multiplexer.
 */
class SerialMux : public Stream {
  public:
    SerialMux(Stream&);
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
    static bool _enabled;
    static uint8_t _ids;
    static uint8_t _lock;
    uint8_t _available = 0;
    uint8_t _id = 0;
    Stream* _serial = NULL;
    int _read(void);
    size_t _write(uint8_t);
    uint8_t _controlRead(void);
    void _controlWrite(uint8_t);
    void _control(void);
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

#endif
