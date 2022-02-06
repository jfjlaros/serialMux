#include <catch.hpp>

#include "../src/vSerial.tcc"

void _checkTx(SerialMux<6>&, char const*, uint8_t);

extern Stream Serial;
SerialMux<6> _mux(Serial);
VSerial<6> serialA(_mux);
VSerial<6> serialB(_mux);


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
  REQUIRE(serialA.write('\x00') == 0);
  REQUIRE(serialB.write('\x00') == 0);
}

TEST_CASE("Enable virtual devices", "[serial][protocol]") {
  Serial.prepare('\xff', '\x01', '\x03');
  _checkTx(_mux, "\xff\x01\x00", 3);
}

TEST_CASE("Write byte to virtual devices", "[serial][single]") {
  REQUIRE(serialA.write('\x01') == 1);
  _checkTx(_mux, "\x00\x01\x01", 3);
  REQUIRE(serialB.write('\x02') == 1);
  _checkTx(_mux, "\x01\x01\x02", 3);
}

TEST_CASE(
    "Send byte to virtual devices, one byte available", "[serial][single]") {
  Serial.prepare('\x00', '\x01', '\x01');
  REQUIRE(serialA.available() == 1);
  Serial.prepare('\x01', '\x01', '\x02');
  REQUIRE(serialB.available() == 1);
}

TEST_CASE(
    "Send byte to virtual devices, two bytes available",
    "[serial][single]") {
  Serial.prepare('\x00', '\x01', '\x03');
  REQUIRE(serialA.available() == 2);
  Serial.prepare('\x01', '\x01', '\x04');
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

TEST_CASE("Write multiple bytes to virtual devices", "[serial][multiple]") {
  uint8_t* data = (uint8_t*)"\x01\x02\x03";
  REQUIRE(serialA.write(data, 3) == 3);
  _checkTx(_mux, "\x00\x03\x01\x02\x03", 5);
  data = (uint8_t*)"\x04\x05\x06";
  REQUIRE(serialB.write(data, 3) == 3);
  _checkTx(_mux, "\x01\x03\x04\x05\x06", 5);
}

TEST_CASE(
    "Read multiple bytes from virtual devices, nothing available",
    "[serial][multiple]") {
  uint8_t* data;
  REQUIRE(serialA.readBytes(data, 3) == 0);
  REQUIRE(serialB.readBytes(data, 3) == 0);
}

TEST_CASE("Send multiple bytes to virtual devices", "[serial][multiple]") {
  Serial.prepare('\x00', '\x03', 'a', 'b', 'c');
  REQUIRE(serialA.available() == 3);
  Serial.prepare('\x01', '\x03', 'd', 'e', 'f');
  REQUIRE(serialB.available() == 3);
}

TEST_CASE("Read multiple bytes from virtual devices", "[serial][multiple]") {
  uint8_t data[4] = {};
  REQUIRE(serialA.readBytes(data, 3) == 3);
  REQUIRE(!strcmp((char const*)data, "abc"));
  REQUIRE(serialA.available() == 0);
  REQUIRE(serialB.readBytes(data, 3) == 3);
  REQUIRE(!strcmp((char const*)data, "def"));
  REQUIRE(serialB.available() == 0);
}

TEST_CASE("Read too many bytes from virtual devices", "[serial][multiple]") {
  uint8_t data[4] = {};
  Serial.prepare('\x00', '\x03', 'a', 'b', 'c');
  REQUIRE(serialA.readBytes(data, 4) == 3);
  REQUIRE(serialA.available() == 0);
  Serial.prepare('\x01', '\x03', 'd', 'e', 'f');
  REQUIRE(serialB.readBytes(data, 4) == 3);
  REQUIRE(serialB.available() == 0);
}

TEST_CASE("Print C string", "[print]") {
  serialA.print("\x01\x02\x03");
  _checkTx(_mux, "\x00\x03\x01\x02\x03", 5);
  serialB.print("\x04\x05\x06");
  _checkTx(_mux, "\x01\x03\x04\x05\x06", 5);
}

TEST_CASE("Print string", "[print]") {
  String s("\x01\x02\x03");
  serialA.print(s);
  _checkTx(_mux, "\x00\x03\x01\x02\x03", 5);
  s = "\x04\x05\x06";
  serialB.print(s);
  _checkTx(_mux, "\x01\x03\x04\x05\x06", 5);
}
