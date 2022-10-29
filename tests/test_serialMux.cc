#include <catch.hpp>

#include "../src/serialMux.tcc"

extern Stream Serial;
SerialMux_<6> mux(Serial);


void _checkTx(
    SerialMux_<6>& mux, char const* const data, uint8_t const size) {
  mux.available(0);
  for (uint8_t i = 0; i < size; i++) {
    REQUIRE(Serial.inspect<char>() == data[i]);
  }
  Serial.reset();
}


TEST_CASE("Add ports", "[mux][add]") {
  REQUIRE(mux.add() == 0);
  REQUIRE(mux.add() == 1);
}

TEST_CASE("No data available on ports", "[mux][empty]") {
  REQUIRE(mux.available(0) == 0);
  REQUIRE(mux.available(1) == 0);
}

TEST_CASE("Peek byte from ports, nothing available", "[mux][empty]") {
  REQUIRE(mux.peek(0) == -1);
  REQUIRE(mux.peek(1) == -1);
}

TEST_CASE("Read byte from ports, nothing available", "[mux][empty]") {
  REQUIRE(mux.read(0) == -1);
  REQUIRE(mux.read(1) == -1);
}

TEST_CASE("Write byte to ports, mux disabled", "[mux][empty]") {
  mux.write(0, 0x00);
  mux.write(1, 0x00);
}

TEST_CASE("Query protocol", "[mux][protocol]") {
  Serial.prepare('\x00');
  _checkTx(mux, "serialMux", 9);
}

TEST_CASE("Get ports", "[mux][protocol]") {
  Serial.prepare('\x02');
  _checkTx(mux, "\x02", 1);
}

TEST_CASE("Enable mux", "[mux][protocol]") {
  Serial.prepare('\x03');
  _checkTx(mux, "\x00", 1);
}

TEST_CASE("Write byte to ports", "[mux][single]") {
  mux.write(0, 0x01);
  _checkTx(mux, "\xff\x00\x01", 3);
  mux.write(1, 0x02);
  _checkTx(mux, "\xff\x01\x02", 3);
}

TEST_CASE("Send byte to ports, one byte available", "[mux][single]") {
  Serial.prepare('\xff', '\x00', '\x01');
  REQUIRE(mux.available(0) == 1);
  Serial.prepare('\xff', '\x01', '\x02');
  REQUIRE(mux.available(1) == 1);
}

TEST_CASE("Send byte to ports, two bytes available", "[mux][single]") {
  Serial.prepare('\xff', '\x00', '\x03');
  REQUIRE(mux.available(0) == 2);
  Serial.prepare('\xff', '\x01', '\x04');
  REQUIRE(mux.available(1) == 2);
}

TEST_CASE("Peek byte from ports, two bytes remaining", "[mux][single]") {
  REQUIRE(mux.peek(0) == 0x01);
  REQUIRE(mux.available(0) == 2);
  REQUIRE(mux.peek(1) == 0x02);
  REQUIRE(mux.available(1) == 2);
}

TEST_CASE("Read byte from ports, one byte remaining", "[mux][single]") {
  REQUIRE(mux.read(0) == 0x01);
  REQUIRE(mux.available(0) == 1);
  REQUIRE(mux.read(1) == 0x02);
  REQUIRE(mux.available(1) == 1);
}

TEST_CASE("Peek byte from ports, one byte remaining", "[mux][single]") {
  REQUIRE(mux.peek(0) == 0x03);
  REQUIRE(mux.available(0) == 1);
  REQUIRE(mux.peek(1) == 0x04);
  REQUIRE(mux.available(1) == 1);
}

TEST_CASE("Read byte from ports, zero bytes remaining", "[mux][single]") {
  REQUIRE(mux.read(0) == 0x03);
  REQUIRE(mux.available(0) == 0);
  REQUIRE(mux.read(1) == 0x04);
  REQUIRE(mux.available(1) == 0);
}

TEST_CASE("Write multiple bytes to ports", "[mux][multiple]") {
  {
    uint8_t const data[] = {0x01, 0x02, 0x03};
    mux.write_(0, data, 3);
    _checkTx(mux, "\xff\x00\x01\x02\x03", 5);
  }
  {
    uint8_t const data[] = {0x04, 0x05, 0x06};
    mux.write_(1, data, 3);
    _checkTx(mux, "\xff\x01\x04\x05\x06", 5);
  }
}

TEST_CASE("Send multiple bytes to ports", "[mux][multiple]") {
  Serial.prepare('\xff', '\x00', 'a', 'b', 'c');
  REQUIRE(mux.available(0) == 3);
  Serial.prepare('\xff', '\x01', 'd', 'e', 'f');
  REQUIRE(mux.available(1) == 3);
}

TEST_CASE("Read multiple bytes from ports", "[mux][multiple]") {
  REQUIRE(mux.read(0) == 'a');
  REQUIRE(mux.read(0) == 'b');
  REQUIRE(mux.read(0) == 'c');
  REQUIRE(mux.available(0) == 0);
  REQUIRE(mux.read(1) == 'd');
  REQUIRE(mux.read(1) == 'e');
  REQUIRE(mux.read(1) == 'f');
  REQUIRE(mux.available(1) == 0);
}
