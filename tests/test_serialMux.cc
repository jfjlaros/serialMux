#include <catch.hpp>

#include "../src/serialMux.tcc"

extern Stream Serial;
SerialMux<6> mux(Serial);


#include <iostream>
void _checkTx(SerialMux<6>& mux, char const* data, uint8_t size) {
  mux.available(0);
  for (uint8_t i = 0; i < size; i++) {
    REQUIRE(Serial.inspect<uint8_t>() == (uint8_t)data[i]);
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
  REQUIRE(mux.write(0, '\x00') == 0);
  REQUIRE(mux.write(1, '\x00') == 0);
}

TEST_CASE("Query protocol", "[mux][protocol]") {
  Serial.prepare('\xff', '\x01', '\x00');
  _checkTx(mux, "\xff\x09serialMux", 11);
}

TEST_CASE("Get ports", "[mux][protocol]") {
  Serial.prepare('\xff', '\x01', '\x02');
  _checkTx(mux, "\xff\x01\x02", 3);
}

TEST_CASE("Enable mux", "[mux][protocol]") {
  Serial.prepare('\xff', '\x01', '\x03');
  _checkTx(mux, "\xff\x01\x00", 3);
}

TEST_CASE("Write byte to ports", "[mux][single]") {
  REQUIRE(mux.write(0, '\x01') == 1);
  _checkTx(mux, "\x00\x01\x01", 3);
  REQUIRE(mux.write(1, '\x02') == 1);
  _checkTx(mux, "\x01\x01\x02", 3);
}

TEST_CASE("Send byte to ports, one byte available", "[mux][single]") {
  Serial.prepare('\x00', '\x01', '\x01');
  REQUIRE(mux.available(0) == 1);
  Serial.prepare('\x01', '\x01', '\x02');
  REQUIRE(mux.available(1) == 1);
}

TEST_CASE("Send byte to ports, two bytes available", "[mux][single]") {
  Serial.prepare('\x00', '\x01', '\x03');
  REQUIRE(mux.available(0) == 2);
  Serial.prepare('\x01', '\x01', '\x04');
  REQUIRE(mux.available(1) == 2);
}

TEST_CASE("Peek byte from ports, two bytes remaining", "[mux][single]") {
  REQUIRE(mux.peek(0) == '\x01');
  REQUIRE(mux.available(0) == 2);
  REQUIRE(mux.peek(1) == '\x02');
  REQUIRE(mux.available(1) == 2);
}

TEST_CASE("Read byte from ports, one byte remaining", "[mux][single]") {
  REQUIRE(mux.read(0) == '\x01');
  REQUIRE(mux.available(0) == 1);
  REQUIRE(mux.read(1) == '\x02');
  REQUIRE(mux.available(1) == 1);
}

TEST_CASE("Peek byte from ports, one byte remaining", "[mux][single]") {
  REQUIRE(mux.peek(0) == '\x03');
  REQUIRE(mux.available(0) == 1);
  REQUIRE(mux.peek(1) == '\x04');
  REQUIRE(mux.available(1) == 1);
}

TEST_CASE("Read byte from ports, zero bytes remaining", "[mux][single]") {
  REQUIRE(mux.read(0) == '\x03');
  REQUIRE(mux.available(0) == 0);
  REQUIRE(mux.read(1) == '\x04');
  REQUIRE(mux.available(1) == 0);
}

TEST_CASE("Write multiple bytes to ports", "[mux][multiple]") {
  uint8_t* data = (uint8_t*)"\x01\x02\x03";
  REQUIRE(mux.write(0, data, 3) == 3);
  _checkTx(mux, "\x00\x03\x01\x02\x03", 5);
  data = (uint8_t*)"\x04\x05\x06";
  REQUIRE(mux.write(1, data, 3) == 3);
  _checkTx(mux, "\x01\x03\x04\x05\x06", 5);
}

TEST_CASE(
    "Read multiple bytes from ports, nothing available", "[mux][multiple]") {
  uint8_t* data;
  REQUIRE(mux.read(0, data, 3) == 0);
  REQUIRE(mux.read(1, data, 3) == 0);
}

TEST_CASE("Send multiple bytes to ports", "[mux][multiple]") {
  Serial.prepare('\x00', '\x03', 'a', 'b', 'c');
  REQUIRE(mux.available(0) == 3);
  Serial.prepare('\x01', '\x03', 'd', 'e', 'f');
  REQUIRE(mux.available(1) == 3);
}

TEST_CASE("Read multiple bytes from ports", "[mux][multiple]") {
  uint8_t data[4] = {};
  REQUIRE(mux.read(0, data, 3) == 3);
  REQUIRE(!strcmp((char const*)data, "abc"));
  REQUIRE(mux.available(0) == 0);
  REQUIRE(mux.read(1, data, 3) == 3);
  REQUIRE(!strcmp((char const*)data, "def"));
  REQUIRE(mux.available(1) == 0);
}

TEST_CASE("Read too many bytes from ports", "[mux][multiple]") {
  uint8_t data[4] = {};
  Serial.prepare('\x00', '\x03', 'a', 'b', 'c');
  REQUIRE(mux.read(0, data, 4) == 3);
  REQUIRE(mux.available(0) == 0);
  Serial.prepare('\x01', '\x03', 'd', 'e', 'f');
  REQUIRE(mux.read(1, data, 4) == 3);
  REQUIRE(mux.available(1) == 0);
}
