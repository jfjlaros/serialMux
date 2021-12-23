#include <Arduino.h>

class Mux : public Stream {
  public:
    Mux(Stream&);
    int available(void);
    int read(void);
    size_t write(uint8_t);
    int peek(void);
  private:
    void _control(void);
    static uint8_t _ids;
    static bool _enabled;
    Stream* _serial;
    uint8_t _id;
};
