FQBN := arduino:avr:pro
PORT := /dev/ttyUSB0
BUILD := build
FLAGS := -pedantic

ARDUINO := arduino-cli
PICOCOM := picocom

SKETCH := $(wildcard *.ino)
LIBS := $(wildcard *.cpp *.h *.tcc)
HEX := $(BUILD)/$(addsuffix .hex, $(SKETCH))


all: $(HEX)

$(HEX): $(SKETCH) $(LIBS)
	$(ARDUINO) compile --fqbn $(FQBN) --warnings all --output-dir $(BUILD) \
    --build-property compiler.cpp.extra_flags="$(FLAGS)"

upload: $(HEX)
	$(ARDUINO) upload --fqbn $(FQBN) --port $(PORT) --input-file $(HEX)

comm:
	$(PICOCOM) -q $(PORT)
