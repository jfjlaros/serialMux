#include <catch.hpp>

#include "../src/buffer.tcc"

Buffer<6> buffer;


TEST_CASE("Empty buffer", "[buffer][empty]") {
  REQUIRE(buffer.available() == 0);
}

TEST_CASE("Peek byte, nothing available", "[buffer][empty]") {
  REQUIRE(buffer.peek() == -1);
}

TEST_CASE("Read byte, nothing available", "[buffer][empty]") {
  REQUIRE(buffer.read() == -1);
}

TEST_CASE("Write byte, one byte available", "[buffer][simgle]") {
  buffer.write('\x00');
  REQUIRE(buffer.available() == 1);
}

TEST_CASE("Write byte, two bytes available", "[buffer][simgle]") {
  buffer.write('\x01');
  REQUIRE(buffer.available() == 2);
}

TEST_CASE("Peek byte, two bytes remaining", "[buffer][simgle]") {
  REQUIRE(buffer.peek() == '\x00');
  REQUIRE(buffer.available() == 2);
}

TEST_CASE("Read byte, one byte remaining", "[buffer][simgle]") {
  REQUIRE(buffer.read() == '\x00');
  REQUIRE(buffer.available() == 1);
}

TEST_CASE("Peek byte, one byte remaining", "[buffer][simgle]") {
  REQUIRE(buffer.peek() == '\x01');
  REQUIRE(buffer.available() == 1);
}

TEST_CASE("Read byte, zero bytes remaining", "[buffer][simgle]") {
  REQUIRE(buffer.read() == '\x01');
  REQUIRE(buffer.available() == 0);
}

TEST_CASE("Read multiple bytes, nothing available", "[buffer][multiple]") {
  uint8_t* data{};
  REQUIRE(buffer.read(data, 3) == 0);
}

TEST_CASE("Write multiple bytes", "[buffer][multiple]") {
  uint8_t* data = (uint8_t*)"abc";
  buffer.write(data, 3);
  REQUIRE(buffer.available() == 3);
}

TEST_CASE("Read multiple bytes", "[buffer][multiple]") {
  uint8_t data[4] = {};
  REQUIRE(buffer.read(data, 3) == 3);
  REQUIRE(!strcmp((char const*)data, "abc"));
  REQUIRE(buffer.available() == 0);
}

TEST_CASE("Read too many bytes", "[buffer][multiple]") {
  uint8_t data[4] = {};
  buffer.write(data, 3);
  REQUIRE(buffer.read(data, 4) == 3);
  REQUIRE(buffer.available() == 0);
}
