# Project Information
TARGET       = build/scroll-wheel
MCU          = atmega32u4
F_CPU        = 16000000
F_USB        = $(F_CPU)
ARCH         = AVR8
BOARD        = NONE
OPTIMIZATION = s
OBJDIR       = build

# Compiler flags
CC_FLAGS     = -DUSE_LUFA_CONFIG_HEADER -IConfig/

# Create the build directory if it doesn't exist
$(shell mkdir -p $(dir $(TARGET)))

# LUFA Path
LUFA_PATH    = ./lufa/LUFA

# Include LUFA sources makefile
include $(LUFA_PATH)/Build/LUFA/lufa-sources.mk

# Source files
SRC          = main.c Descriptors.c $(LUFA_SRC_USB) $(filter %HIDClassDevice.c, $(LUFA_SRC_USBCLASS_DEVICE))

# Default target
all:

# Include common DMBS build system modules
DMBS_PATH      ?= $(LUFA_PATH)/Build/DMBS/DMBS
include $(DMBS_PATH)/core.mk
include $(DMBS_PATH)/gcc.mk
include $(DMBS_PATH)/avrdude.mk

# Include LUFA GCC modules
include $(LUFA_PATH)/Build/LUFA/lufa-gcc.mk

# Our custom flash target.
# Allows overriding the serial port via a PORT variable, e.g.:
#   make flash PORT=/dev/cu.usbmodem1234
#
PORT ?= $$(ls -1t /dev/ttyACM* | head -n 1)
.PHONY: flash
flash: all
	avrdude -p m32u4 -c avr109 -P $(PORT) -b 57600 -U flash:w:$(TARGET).hex:i
