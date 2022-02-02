#pragma once

#include <Arduino.h>

#define _PROTOCOL "serialMux\x01\x00\x00"

// Control commands.
#define CMD_PROTOCOL  '\x00'
#define CMD_GET_PORTS '\x01'
#define CMD_ENABLE    '\x02'
#define CMD_DISABLE   '\x03'
#define CMD_RESET     '\x04'

/*!
 */
class Control {
  public:
    Control(Stream&);
    uint8_t add(void);
    uint8_t available(uint8_t);
    int16_t peek(uint8_t);
    uint8_t read(uint8_t, uint8_t*, uint8_t);
    int16_t read(uint8_t);
    uint8_t write(uint8_t, uint8_t*, uint8_t);
    uint8_t write(uint8_t, uint8_t);

  private:
    uint8_t _available = 0;
    bool _enabled = false;
    uint8_t _ids = 0;
    uint8_t _lock;
    Stream* _serial = NULL;

    uint8_t _read(uint8_t*, uint8_t);
    uint8_t _write(uint8_t*, uint8_t);
    void _control(void);
};
