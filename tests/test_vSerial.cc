#include <catch.hpp>

#include "../src/vSerial.tcc"

void _checkTx(SerialMux_<6>&, char const*, uint8_t);

extern Stream Serial;
SerialMux_<6> _mux(Serial);
VSerial_<6> serialA(_mux);
VSerial_<6> serialB(_mux);


TEST_CASE("No data available for virtual devices", "[serial][empty]") {
  REQUIRE(serialA.available() == 0);
  REQUIRE(serialB.available() == 0);
}

TEST_CASE(
    "Peek byte from virtual devices, nothing available", "[serial][empty]") {
  REQUIRE(serialA.peek() == -1);
  REQUIRE(serialB.peek() == -1);
}

TEST_CASE(
    "Read byte from virtual devices, nothing available", "[serial][empty]") {
  REQUIRE(serialA.read() == -1);
  REQUIRE(serialB.read() == -1);
}

TEST_CASE(
    "Write byte to virtual devices, mux disabled", "[serial][empty]") {
  REQUIRE(serialA.write(0x00) == 1);
  REQUIRE(serialB.write(0x00) == 1);
}

TEST_CASE("Enable virtual devices", "[serial][protocol]") {
  Serial.prepare('\x03');
  _checkTx(_mux, "\x00", 1);
}

TEST_CASE("Write byte to virtual devices", "[serial][single]") {
  REQUIRE(serialA.write(0x01) == 1);
  _checkTx(_mux, "\xff\x00\x01", 3);
  REQUIRE(serialB.write(0x02) == 1);
  _checkTx(_mux, "\xff\x01\x02", 3);
}

TEST_CASE(
    "Send byte to virtual devices, one byte available", "[serial][single]") {
  Serial.prepare('\xff', '\x00', '\x01');
  REQUIRE(serialA.available() == 1);
  Serial.prepare('\xff', '\x01', '\x02');
  REQUIRE(serialB.available() == 1);
}

TEST_CASE(
    "Send byte to virtual devices, two bytes available",
    "[serial][single]") {
  Serial.prepare('\xff', '\x00', '\x03');
  REQUIRE(serialA.available() == 2);
  Serial.prepare('\xff', '\x01', '\x04');
  REQUIRE(serialB.available() == 2);
}

TEST_CASE(
    "Peek byte from virtual devices, two bytes remaining",
    "[serial][single]") {
  REQUIRE(serialA.peek() == '\x01');
  REQUIRE(serialA.available() == 2);
  REQUIRE(serialB.peek() == '\x02');
  REQUIRE(serialB.available() == 2);
}

TEST_CASE(
    "Read byte from virtual devices, one byte remaining",
    "[serial][single]") {
  REQUIRE(serialA.read() == '\x01');
  REQUIRE(serialA.available() == 1);
  REQUIRE(serialB.read() == '\x02');
  REQUIRE(serialB.available() == 1);
}

TEST_CASE(
    "Peek byte from virtual devices, one byte remaining",
    "[serial][single]") {
  REQUIRE(serialA.peek() == '\x03');
  REQUIRE(serialA.available() == 1);
  REQUIRE(serialB.peek() == '\x04');
  REQUIRE(serialB.available() == 1);
}

TEST_CASE(
    "Read byte from virtual devices, zero bytes remaining",
    "[serial][single]") {
  REQUIRE(serialA.read() == '\x03');
  REQUIRE(serialA.available() == 0);
  REQUIRE(serialB.read() == '\x04');
  REQUIRE(serialB.available() == 0);
}
