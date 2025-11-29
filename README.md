# MPG Scroll Wheel

This project provides the firmware for a custom USB scroll wheel based on an Arduino Pro Micro compatible board with an ATmega32U4 microcontroller. It uses the LUFA (Lightweight USB Framework for AVRs) library to implement a USB HID device that provides high-resolution, cross-platform smooth scrolling.

## Features

*   **High-Resolution Scrolling**: Reports fine-grained scroll events, allowing for smooth, pixel-level scrolling in supported applications.
*   **Cross-Platform**: Designed to work on Windows, macOS, and Linux.
*   **Simple Hardware**: Works with a standard CNC MGP (Manual Pulse Generator, or jog wheel).
*   **LUFA-based**: Built upon the LUFA framework.

## Hardware Requirements

*   An ATmega32U4-based microcontroller board (e.g., Arduino Pro Micro).
*   A standard 5V 2-bit quadrature rotary encoder. If the encode supports differential outputs, the inverted outputs can be ignored.

### Pinout

The encoder should be connected to the microcontroller as follows:

| Encoder Pin | MCU Pin |
| ----------- | ------- |
| A           | `PD0`   |
| B           | `PD1`   |
| Common      | `GND`   |

Internal pull-up resistors are enabled on the data pins, so external resistors are not required.

## Building the Firmware

### Dependencies

To build this firmware, you will need a complete AVR development toolchain, including:
*   `avr-gcc`
*   `avr-objcopy`
*   `avr-size`
*   `make`

### Build Steps

1.  **Clone the repository:**
    ```bash
    git clone <repository-url>
    cd scroll-wheel
    ```

2.  **Initialize the LUFA Submodule:**
    This project includes LUFA as a git submodule. To pull it into the project, run:
    ```bash
    git submodule update --init --recursive
    ```

3.  **Compile the code:**
    Run the `make` command from the root of the project directory.
    ```bash
    make
    ```
    This will compile the source files and generate the final firmware. The output file will be located at `build/scroll-wheel.hex`.

## Flashing the Firmware

The method for flashing uses `avrdude`.

**1. Entering Bootloader Mode**

Before flashing, the microcontroller must be in bootloader mode. For many Arduino Pro Micro compatible boards, you can enter bootloader mode by **quickly shorting the `RST` pin to `GND` twice within 1 second**. This will make a new serial port device appear for approximately 8 seconds, during which you can start the upload of the new firmware.

**2. Flashing the Device**

**Option A: Using the Makefile Target (Recommended)**

The `Makefile` includes a convenient `flash` target that can be used on all platforms. Put your device in bootloader mode and run one of the following commands:

*   **On Linux:** The command will attempt to auto-detect the serial port.
    ```bash
    make flash
    ```
*   **On macOS or Windows:** You must specify the port using the `PORT` variable.
    ```bash
    # Example for macOS
    make flash PORT=/dev/cu.usbmodem12345

    # Example for Windows (in a compatible shell like Git Bash or WSL)
    make flash PORT=COM3
    ```

**Option B: Manual Flashing**

If you prefer, or if the Makefile target fails, you can always call `avrdude` directly.
```bash
# General command
avrdude -p atmega32u4 -c avr109 -P <YOUR_SERIAL_PORT> -U flash:w:build/scroll-wheel.hex
```
Replace `<YOUR_SERIAL_PORT>` with your device's actual serial port (e.g. `/dev/ttyACM0`, `COM3`).

---

## Known Issues

- **Windows Compatibility:** This firmware is **not currently working on Windows**. While the device is recognized, the standard `mouhid` driver fails to start it.
- **Problem Code:** `0xA` (This device cannot start.)
- **Status Code:** `0xC0110004` (This often indicates that the HID Report Descriptor is considered invalid by the Windows driver.)
    
This is a known issue being investigated. The device currently works as expected on Linux and macOS.
