# MCU and CPU frequency
MCU=atmega32u4
F_CPU=16000000UL
TARGET=main

# Build directories
BUILD_DIR=build
OBJ_DIR=$(BUILD_DIR)/obj

# Find all .c files in the current directory
SRCS=$(wildcard *.c)
# Create a list of object files in the OBJ_DIR
OBJS=$(patsubst %.c,$(OBJ_DIR)/%.o,$(SRCS))

# Compiler and linker settings
CC=avr-gcc
OBJCOPY=avr-objcopy
CFLAGS=-mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os -std=gnu11 -ffunction-sections -fdata-sections -Wall -Wextra
LDFLAGS=-mmcu=$(MCU) -Wl,--gc-sections

# Target files
ELF_FILE=$(BUILD_DIR)/$(TARGET).elf
HEX_FILE=$(BUILD_DIR)/$(TARGET).hex

.PHONY: all clean flash

all: $(HEX_FILE)

# Link object files to create ELF file
$(ELF_FILE): $(OBJS)
	@mkdir -p $(@D)
	$(CC) $(LDFLAGS) $(OBJS) -o $@

# Create HEX file from ELF file
$(HEX_FILE): $(ELF_FILE)
	$(OBJCOPY) -O ihex $< $@

# Compile .c files into .o object files
$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)

flash: all
	stty -F /dev/ttyACM0 1200
	sleep 2
	avrdude -p m32u4 -c avr109 -P /dev/ttyACM0 -b 57600 -U flash:w:$(HEX_FILE):i
