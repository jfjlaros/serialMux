#include <catch.hpp>

#include "../src/serialMux.tcc"

extern Stream Serial;
SerialMux mux {Serial};


void checkTx(
    SerialMux& mux, char const* const data, uint8_t const size) {
  mux.update();
  for (uint8_t i {0}; i < size; ++i) {
    REQUIRE(Serial.inspect<char>() == data[i]);
  }
  Serial.reset();
}


Buffer<6> bufferA {};
Buffer<6> bufferB {};

TEST_CASE("Add ports", "[mux][add]") {
  REQUIRE(mux.add(&bufferA) == 0);
  REQUIRE(mux.add(&bufferB) == 1);
}

TEST_CASE("No data available on ports", "[mux][empty]") {
  REQUIRE(mux.buffer_[0]->available() == 0);
  REQUIRE(mux.buffer_[0]->available() == 0);
}

TEST_CASE("Peek byte from ports, nothing available", "[mux][empty]") {
  REQUIRE(mux.buffer_[0]->peek() == -1);
  REQUIRE(mux.buffer_[1]->peek() == -1);
}

TEST_CASE("Read byte from ports, nothing available", "[mux][empty]") {
  REQUIRE(mux.buffer_[0]->read() == -1);
  REQUIRE(mux.buffer_[1]->read() == -1);
}

TEST_CASE("Write byte to ports, mux disabled", "[mux][empty]") {
  mux.write(0, 0x00);
  mux.write(1, 0x00);
}

TEST_CASE("Query protocol", "[mux][protocol]") {
  Serial.prepare('\x00');
  mux.update();
  REQUIRE(Serial.tx == 9);
  checkTx(mux, "serialMux", 9);
}

TEST_CASE("Get ports", "[mux][protocol]") {
  Serial.prepare('\x02');
  checkTx(mux, "\x02", 1);
}

TEST_CASE("Enable mux", "[mux][protocol]") {
  Serial.prepare('\x03');
  checkTx(mux, "\x00", 1);
}

TEST_CASE("Write byte to ports", "[mux][single]") {
  mux.write(0, 0x01);
  checkTx(mux, "\xff\x00\x01", 3);
  mux.write(1, 0x02);
  checkTx(mux, "\xff\x01\x02", 3);
}

TEST_CASE("Send byte to ports, one byte available", "[mux][single]") {
  Serial.prepare('\xff', '\x00', '\x01');
  mux.update();
  REQUIRE(mux.buffer_[0]->available() == 1);
  Serial.prepare('\xff', '\x01', '\x02');
  mux.update();
  REQUIRE(mux.buffer_[1]->available() == 1);
}

TEST_CASE("Send byte to ports, two bytes available", "[mux][single]") {
  Serial.prepare('\xff', '\x00', '\x03');
  mux.update();
  REQUIRE(mux.buffer_[0]->available() == 2);
  Serial.prepare('\xff', '\x01', '\x04');
  mux.update();
  REQUIRE(mux.buffer_[1]->available() == 2);
}

TEST_CASE("Peek byte from ports, two bytes remaining", "[mux][single]") {
  REQUIRE(mux.buffer_[0]->peek() == 0x01);
  REQUIRE(mux.buffer_[0]->available() == 2);
  REQUIRE(mux.buffer_[1]->peek() == 0x02);
  REQUIRE(mux.buffer_[1]->available() == 2);
}

TEST_CASE("Read byte from ports, one byte remaining", "[mux][single]") {
  REQUIRE(mux.buffer_[0]->read() == 0x01);
  REQUIRE(mux.buffer_[0]->available() == 1);
  REQUIRE(mux.buffer_[1]->read() == 0x02);
  REQUIRE(mux.buffer_[1]->available() == 1);
}

TEST_CASE("Peek byte from ports, one byte remaining", "[mux][single]") {
  REQUIRE(mux.buffer_[0]->peek() == 0x03);
  REQUIRE(mux.buffer_[0]->available() == 1);
  REQUIRE(mux.buffer_[1]->peek() == 0x04);
  REQUIRE(mux.buffer_[1]->available() == 1);
}

TEST_CASE("Read byte from ports, zero bytes remaining", "[mux][single]") {
  REQUIRE(mux.buffer_[0]->read() == 0x03);
  REQUIRE(mux.buffer_[0]->available() == 0);
  REQUIRE(mux.buffer_[1]->read() == 0x04);
  REQUIRE(mux.buffer_[1]->available() == 0);
}

TEST_CASE("Write multiple bytes to ports", "[mux][multiple]") {
  {
    uint8_t const data[] {0x01, 0x02, 0x03};
    mux.write_(0, data, 3);
    checkTx(mux, "\xff\x00\x01\x02\x03", 5);
  }
  {
    uint8_t const data[] {0x04, 0x05, 0x06};
    mux.write_(1, data, 3);
    checkTx(mux, "\xff\x01\x04\x05\x06", 5);
  }
}

TEST_CASE("Send multiple bytes to ports", "[mux][multiple]") {
  Serial.prepare('\xff', '\x00', 'a', 'b', 'c');
  mux.update();
  REQUIRE(mux.buffer_[0]->available() == 3);
  Serial.prepare('\xff', '\x01', 'd', 'e', 'f');
  mux.update();
  REQUIRE(mux.buffer_[1]->available() == 3);
}

TEST_CASE("Read multiple bytes from ports", "[mux][multiple]") {
  REQUIRE(mux.buffer_[0]->read() == 'a');
  REQUIRE(mux.buffer_[0]->read() == 'b');
  REQUIRE(mux.buffer_[0]->read() == 'c');
  REQUIRE(mux.buffer_[0]->available() == 0);
  REQUIRE(mux.buffer_[1]->read() == 'd');
  REQUIRE(mux.buffer_[1]->read() == 'e');
  REQUIRE(mux.buffer_[1]->read() == 'f');
  REQUIRE(mux.buffer_[1]->available() == 0);
}
