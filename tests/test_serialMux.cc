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
  Serial.prepare('\0', '\1', CMD_GET_PORTS);
  REQUIRE(Serial.available() == 3);

  // Trigger control command.
  muxA.available();

  _checkEmpty(muxA);
  _checkEmpty(muxB);
  _checkTx('\0', '\1', '\2');
}

TEST_CASE("Enable", "[control][read][write][peek][port1][port2]") {
  Serial.prepare('\0', '\1', CMD_ENABLE);
  REQUIRE(Serial.available() == 3);

  // Trigger control command.
  muxA.available();

  _checkEmpty(muxA);
  _checkEmpty(muxB);
  _checkTx('\0', '\1', '\0');
}


TEST_CASE("One byte available, port 1", "[read][peek][port1]") {
  // Send one byte to port 1.
  Serial.prepare('\1', '\1', '\2');
  
  REQUIRE(Serial.available() == 3);
  REQUIRE(muxA.available() == 1);
  REQUIRE(Serial.available() == 1);
  REQUIRE(muxB.available() == 0);
}

TEST_CASE("Peek byte, port 1", "[peek][port1]") {
  REQUIRE(muxA.peek() == '\2');
  REQUIRE(muxB.peek() == -1);
}

TEST_CASE("Read byte, port 1", "[read][peek][port1]") {
  REQUIRE(muxA.read() == '\2');
  REQUIRE(muxB.read() == -1);
  _checkEmpty(muxA);
  _checkEmpty(muxB);
}


TEST_CASE("One byte available, port 2", "[read][peek][port2]") {
  // Send one byte to port 2.
  Serial.prepare('\2', '\1', '\2');
  REQUIRE(Serial.available() == 3);
  REQUIRE(muxB.available() == 1);
  REQUIRE(Serial.available() == 1);
  REQUIRE(muxA.available() == 0);
}

TEST_CASE("Peek byte, port 2", "[peek][port2]") {
  REQUIRE(muxB.peek() == '\2');
  REQUIRE(muxA.peek() == -1);
}

TEST_CASE("Read byte, port 2", "[read][peek][port2]") {
  REQUIRE(muxB.read() == '\2');
  REQUIRE(muxA.read() == -1);
  _checkEmpty(muxB);
  _checkEmpty(muxA);
}


TEST_CASE("One byte available, port 1, wrong order", "[read][peek][port1]") {
  // Send one byte to port 1.
  Serial.prepare('\1', '\1', '\2');
  
  REQUIRE(Serial.available() == 3);
  REQUIRE(muxB.available() == 0);
  REQUIRE(Serial.available() == 2);
  REQUIRE(muxA.available() == 1);
  REQUIRE(Serial.available() == 1);
  REQUIRE(muxB.available() == 0);
}

TEST_CASE("Peek byte, port 1, wrong order", "[peek][port1]") {
  REQUIRE(muxB.peek() == -1);
  REQUIRE(muxA.peek() == '\2');
}

TEST_CASE("Read byte, port 1, wrong order", "[read][peek][port1]") {
  REQUIRE(muxB.read() == -1);
  REQUIRE(muxA.read() == '\2');
  _checkEmpty(muxB);
  _checkEmpty(muxA);
}


TEST_CASE("Multiple bytes available, port 1", "[read][port1]") {
  // Send three bytes to port 1.
  Serial.prepare('\1', '\3', '\0', '\1', '\2');
  REQUIRE(Serial.available() == 5);
  REQUIRE(muxA.available() == 3);
  REQUIRE(Serial.available() == 3);
  REQUIRE(muxB.available() == 0);
}

TEST_CASE("Read multiple bytes, port 1", "[read][port1]") {
  uint8_t buffer[4] = {'\0', '\0', '\0', '\0'};
  muxA.readBytes(buffer, 3);
  REQUIRE(!strcmp((char const*)buffer, "\0\1\2"));
  _checkEmpty(muxA);
  _checkEmpty(muxB);
}


TEST_CASE("Multiple bytes available, port 2", "[read][port2]") {
  // Send three bytes to port 2.
  Serial.prepare('\2', '\3', '\0', '\1', '\2');
  REQUIRE(Serial.available() == 5);
  REQUIRE(muxB.available() == 3);
  REQUIRE(Serial.available() == 3);
  REQUIRE(muxA.available() == 0);
}

TEST_CASE("Read multiple bytes, port 2", "[read][port2]") {
  uint8_t buffer[4] = {'\0', '\0', '\0', '\0'};
  muxB.readBytes(buffer, 3);
  REQUIRE(!strcmp((char const*)buffer, "\0\1\2"));
  _checkEmpty(muxB);
  _checkEmpty(muxA);
}


TEST_CASE("Peek byte immediately, port 1", "[peek][port1]") {
  Serial.prepare('\1', '\1', '\2');
  REQUIRE(muxA.peek() == '\2');
  REQUIRE(muxB.peek() == -1);
  REQUIRE(muxA.read() == '\2');
  REQUIRE(muxB.read() == -1);
}

TEST_CASE("Read byte immediately, port 1", "[read][port1]") {
  Serial.prepare('\1', '\1', '\2');
  REQUIRE(muxA.read() == '\2');
  REQUIRE(muxB.read() == -1);
}

TEST_CASE("Peek byte immediately, port 2", "[peek][port2]") {
  Serial.prepare('\2', '\1', '\2');
  REQUIRE(muxB.peek() == '\2');
  REQUIRE(muxA.peek() == -1);
  REQUIRE(muxB.read() == '\2');
  REQUIRE(muxA.read() == -1);
}

TEST_CASE("Read byte immediately, port 2", "[read][port2]") {
  Serial.prepare('\2', '\1', '\2');
  REQUIRE(muxB.read() == '\2');
  REQUIRE(muxA.read() == -1);
}

TEST_CASE("Write byte, port 1", "[write][port1]") {
  muxA.write('\2');
  _checkTx('\1', '\1', '\2');
}

TEST_CASE("Write multiple bytes, port 1", "[write][port1]") {
  muxA.write((uint8_t*)"\0\1\2", 3);
  _checkTx('\1', '\3', '\0', '\1', '\2');
}

TEST_CASE("Write byte, port 2", "[write][port2]") {
  muxB.write('\2');
  _checkTx('\2', '\1', '\2');
}

TEST_CASE("Write multiple bytes, port 2", "[write][port2]") {
  muxB.write((uint8_t*)"\0\1\2", 3);
  _checkTx('\2', '\3', '\0', '\1', '\2');
}
