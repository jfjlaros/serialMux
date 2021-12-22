#include <Arduino.h>

class Mux : public Stream {
  public:
    Mux(Stream&, uint8_t);
    int available(void);
    int read(void);
    size_t write(uint8_t);
    int peek(void);
  private:
    Stream* _serial;
    uint8_t _id;
};
