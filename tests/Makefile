EXEC := run_tests
MAIN := test_lib
SRC :=
TESTS := test_buffer test_serialMux test_vSerial
FIXTURES := arduino-serial-fixture/src/Arduino


CC := g++
CC_ARGS := -std=c++11 -Wall -Wextra -pedantic -Wno-pmf-conversions \
  -I arduino-serial-fixture/src -I plugins


OBJS := $(addsuffix .o, $(SRC) $(TESTS) $(FIXTURES))

.PHONY: all check clean distclean


all: $(EXEC)

$(EXEC): $(MAIN).cc $(OBJS)
	$(CC) $(CC_ARGS) -o $@ $^

%.o: %.cpp
	$(CC) $(CC_ARGS) -o $@ -c $<

%.o: %.cc
	$(CC) $(CC_ARGS) -o $@ -c $<

check: all
	valgrind --error-exitcode=1 --track-origins=yes ./$(EXEC)

clean:
	rm -f $(OBJS)

distclean: clean
	rm -f $(EXEC)
