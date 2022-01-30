#include <catch.hpp>

#include "../src/serialMux.h"

extern Stream Serial;
SerialMux muxA(Serial);
SerialMux muxB(Serial);


void _checkEmpty(SerialMux& mux) {
  REQUIRE(Serial.available() == 0);
  REQUIRE(mux.available() == 0);
}


void _checkTx(void) {}

template <class T, class... Args>
void _checkTx(T data, Args... args) {
  REQUIRE(Serial.inspect<T>() == data);
  _checkTx(args...);
}


TEST_CASE("Get ports", "[control]") {
  Serial.prepare('\x00', '\x01', CMD_GET_PORTS);
  REQUIRE(Serial.available() == 3);

  // Trigger control command.
  muxA.available();

  _checkEmpty(muxA);
  _checkEmpty(muxB);
  _checkTx('\x00', '\x01', '\x02');
}

TEST_CASE("Enable", "[control][read][write][peek][port1][port2]") {
  Serial.prepare('\x00', '\x01', CMD_ENABLE);
  REQUIRE(Serial.available() == 3);

  // Trigger control command.
  muxA.available();

  _checkEmpty(muxA);
  _checkEmpty(muxB);
  _checkTx('\x00', '\x01', '\x00');
}


TEST_CASE("One byte available, port 1", "[read][peek][port1]") {
  // Send one byte to port 1.
  Serial.prepare('\x01', '\x01', '\x02');
  
  REQUIRE(Serial.available() == 3);
  REQUIRE(muxA.available() == 1);
  REQUIRE(Serial.available() == 1);
  REQUIRE(muxB.available() == 0);
}

TEST_CASE("Peek byte, port 1", "[peek][port1]") {
  REQUIRE(muxA.peek() == '\x02');
  REQUIRE(muxB.peek() == -1);
}

TEST_CASE("Read byte, port 1", "[read][peek][port1]") {
  REQUIRE(muxA.read() == '\x02');
  REQUIRE(muxB.read() == -1);
  _checkEmpty(muxA);
  _checkEmpty(muxB);
}


TEST_CASE("One byte available, port 2", "[read][peek][port2]") {
  // Send one byte to port 2.
  Serial.prepare('\x02', '\x01', '\x02');
  REQUIRE(Serial.available() == 3);
  REQUIRE(muxB.available() == 1);
  REQUIRE(Serial.available() == 1);
  REQUIRE(muxA.available() == 0);
}

TEST_CASE("Peek byte, port 2", "[peek][port2]") {
  REQUIRE(muxB.peek() == '\x02');
  REQUIRE(muxA.peek() == -1);
}

TEST_CASE("Read byte, port 2", "[read][peek][port2]") {
  REQUIRE(muxB.read() == '\x02');
  REQUIRE(muxA.read() == -1);
  _checkEmpty(muxB);
  _checkEmpty(muxA);
}


TEST_CASE("One byte available, port 1, wrong order", "[read][peek][port1]") {
  // Send one byte to port 1.
  Serial.prepare('\x01', '\x01', '\x02');
  
  REQUIRE(Serial.available() == 3);
  REQUIRE(muxB.available() == 0);
  REQUIRE(Serial.available() == 1);
  REQUIRE(muxA.available() == 1);
  REQUIRE(Serial.available() == 1);
  REQUIRE(muxB.available() == 0);
}

TEST_CASE("Peek byte, port 1, wrong order", "[peek][port1]") {
  REQUIRE(muxB.peek() == -1);
  REQUIRE(muxA.peek() == '\x02');
}

TEST_CASE("Read byte, port 1, wrong order", "[read][peek][port1]") {
  REQUIRE(muxB.read() == -1);
  REQUIRE(muxA.read() == '\x02');
  _checkEmpty(muxB);
  _checkEmpty(muxA);
}


TEST_CASE("Multiple bytes available, port 1", "[read][port1]") {
  // Send three bytes to port 1.
  Serial.prepare('\x01', '\x03', '\x00', '\x01', '\x02');
  REQUIRE(Serial.available() == 5);
  REQUIRE(muxA.available() == 3);
  REQUIRE(Serial.available() == 3);
  REQUIRE(muxB.available() == 0);
}

TEST_CASE("Read multiple bytes, port 1", "[read][port1]") {
  uint8_t buffer[4] = {'\x00', '\x00', '\x00', '\x00'};
  muxA.readBytes(buffer, 3);
  REQUIRE(!strcmp((char const*)buffer, "\x00\x01\x02"));
  _checkEmpty(muxA);
  _checkEmpty(muxB);
}


TEST_CASE("Multiple bytes available, port 2", "[read][port2]") {
  // Send three bytes to port 2.
  Serial.prepare('\x02', '\x03', '\x00', '\x01', '\x02');
  REQUIRE(Serial.available() == 5);
  REQUIRE(muxB.available() == 3);
  REQUIRE(Serial.available() == 3);
  REQUIRE(muxA.available() == 0);
}

TEST_CASE("Read multiple bytes, port 2", "[read][port2]") {
  uint8_t buffer[4] = {'\x00', '\x00', '\x00', '\x00'};
  muxB.readBytes(buffer, 3);
  REQUIRE(!strcmp((char const*)buffer, "\x00\x01\x02"));
  _checkEmpty(muxB);
  _checkEmpty(muxA);
}


TEST_CASE("Peek byte immediately, port 1", "[peek][port1]") {
  Serial.prepare('\x01', '\x01', '\x02');
  REQUIRE(muxA.peek() == '\x02');
  REQUIRE(muxB.peek() == -1);
  REQUIRE(muxA.read() == '\x02');
  REQUIRE(muxB.read() == -1);
}

TEST_CASE("Read byte immediately, port 1", "[read][port1]") {
  Serial.prepare('\x01', '\x01', '\x02');
  REQUIRE(muxA.read() == '\x02');
  REQUIRE(muxB.read() == -1);
}

TEST_CASE("Peek byte immediately, port 2", "[peek][port2]") {
  Serial.prepare('\x02', '\x01', '\x02');
  REQUIRE(muxB.peek() == '\x02');
  REQUIRE(muxA.peek() == -1);
  REQUIRE(muxB.read() == '\x02');
  REQUIRE(muxA.read() == -1);
}

TEST_CASE("Read byte immediately, port 2", "[read][port2]") {
  Serial.prepare('\x02', '\x01', '\x02');
  REQUIRE(muxB.read() == '\x02');
  REQUIRE(muxA.read() == -1);
}

TEST_CASE("Write byte, port 1", "[write][port1]") {
  muxA.write('\x02');
  _checkTx('\x01', '\x01', '\x02');
}

TEST_CASE("Write multiple bytes, port 1", "[write][port1]") {
  muxA.write((uint8_t*)"\x00\x01\x02", 3);
  _checkTx('\x01', '\x03', '\x00', '\x01', '\x02');
}

TEST_CASE("Write byte, port 2", "[write][port2]") {
  muxB.write('\x02');
  _checkTx('\x02', '\x01', '\x02');
}

TEST_CASE("Write multiple bytes, port 2", "[write][port2]") {
  muxB.write((uint8_t*)"\x00\x01\x02", 3);
  _checkTx('\x02', '\x03', '\x00', '\x01', '\x02');
}


TEST_CASE("Print C string", "[print]") {
  muxA.print("abc");
  _checkTx('\x01', '\x03', 'a', 'b', 'c');
}

TEST_CASE("Print string", "[print]") {
  String s("abc");
  muxA.print(s);
  _checkTx('\x01', '\x03', 'a', 'b', 'c');
}
