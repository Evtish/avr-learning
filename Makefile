PROGRAM_NAME := main

SRC_DIR := src
INC_DIR := inc
BUILD_DIR := build
USB_PORT := /dev/ttyUSB0

SOURCE_FILES := $(wildcard $(SRC_DIR)/*.c)
OBJECT_FILES := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SOURCE_FILES))
DEPENDENCY_FILES := $(OBJECT_FILES:.o=.d)
EXEC_FILE := $(BUILD_DIR)/$(PROGRAM_NAME)
HEX_FILE := $(BUILD_DIR)/$(PROGRAM_NAME).hex

CC := avr-gcc
CFLAGS := -DF_CPU=16000000UL -mmcu=atmega328p -ggdb -MMD -MP -Wall -Wextra -std=c99 -pedantic -Os -I$(INC_DIR) -save-temps=obj
LDFLAGS := -ggdb -mmcu=atmega328p
OBJCOPY_FLAGS := -R .eeprom -O ihex
AVRDUDE_FLAGS := -c arduino -p m328p -P $(USB_PORT)

all: $(HEX_FILE)

# create the build directory
$(BUILD_DIR):
	mkdir -pv $(BUILD_DIR)

# check if the build directory exists
$(OBJECT_FILES): | $(BUILD_DIR)

# compile
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) -c $(CFLAGS) $< -o $@

-include $(DEPENDENCY_FILES)

# link
$(EXEC_FILE): $(OBJECT_FILES)
	$(CC) $^ $(LDFLAGS) -o $@

# create .hex file from the executable
$(HEX_FILE): $(EXEC_FILE)
	avr-objcopy $(OBJCOPY_FLAGS) $^ $@

# flash an MCU
flash: $(HEX_FILE)
	avrdude $(AVRDUDE_FLAGS) -U flash:w:$<

# remove the build files
clean:
	rm -rf $(BUILD_DIR)

.PHONY: all flash clean
