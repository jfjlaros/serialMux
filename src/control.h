#pragma once

#include <Arduino.h>

#define _PROTOCOL "serialMux\x01\x00\x00"

// Control commands.
#define CMD_PROTOCOL  '\x00'
#define CMD_GET_PORTS '\x01'
#define CMD_ENABLE    '\x02'
#define CMD_DISABLE   '\x03'
#define CMD_RESET     '\x04'

class Control {
  public:
    Control(void) {}
    Control(Stream&);

  protected:
    static uint8_t _available;
    static bool _enabled;
    static uint8_t _ids;
    static uint8_t _lock;
    static Stream* _serial;

    int _read(void);
    size_t _write(uint8_t);
    uint8_t _controlRead(void);
    void _controlWrite(uint8_t*, uint8_t);
    void _control(void);
};
