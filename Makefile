PROGRAM_NAME := main

SRC_DIR := src
INC_DIR := inc
BUILD_DIR := build
USB_PORT := /dev/ttyUSB1

SOURCE_FILES := $(wildcard $(SRC_DIR)/*.c)
OBJECT_FILES := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SOURCE_FILES))
DEPENDENCY_FILES := $(OBJECT_FILES:.o=.d)
ELF_FILE := $(BUILD_DIR)/$(PROGRAM_NAME).elf
HEX_FILE := $(BUILD_DIR)/$(PROGRAM_NAME).hex

CC := avr-gcc
CFLAGS := -DF_CPU=16000000UL -mmcu=atmega328p -MMD -MP -Wall -Wextra -pedantic -Os -I ./$(INC_DIR)
OBJCOPY_FLAGS := -R .eeprom -O ihex
AVRDUDE_FLAGS := -c arduino -p m328p -P $(USB_PORT)

all: $(HEX_FILE)

# create the build directory
$(BUILD_DIR):
	mkdir -pv $(BUILD_DIR)

# check if build directory exists
$(OBJECT_FILES): | $(BUILD_DIR)

# compile
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) -c $(CFLAGS) $< -o $@

-include $(DEPENDENCY_FILES)

# link
$(ELF_FILE): $(OBJECT_FILES)
	$(CC) $^ $(CFLAGS) -o $@

# create .hex file from .elf
$(HEX_FILE): $(ELF_FILE)
	avr-objcopy $(OBJCOPY_FLAGS) $^ $@

# flash MCU
flash: $(HEX_FILE)
	avrdude $(AVRDUDE_FLAGS) -U flash:w:$<

# remove build files
clean:
	rm -rf $(BUILD_DIR)

.PHONY: all flash clean
