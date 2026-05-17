# MPG Scroll Wheel

This project provides the firmware for a custom USB scroll wheel based on an Arduino Pro Micro compatible board with an ATmega32U4 microcontroller. It uses the LUFA (Lightweight USB Framework for AVRs) library to implement a USB HID device that provides high-resolution, cross-platform smooth scrolling.

## Features

*   **High-Resolution Scrolling**: Reports fine-grained scroll events, allowing for smooth, pixel-level scrolling in supported applications.
*   **Cross-Platform**: Designed to work on Windows, macOS, and Linux.
*   **Simple Hardware**: Works with a standard CNC MPG (Manual Pulse Generator, or jog wheel).
*   **LUFA-based**: Built upon the LUFA framework.

## Hardware Requirements

*   An ATmega32U4-based microcontroller board (e.g., Arduino Pro Micro).
*   A standard 5V 2-bit quadrature rotary encoder. If the encoder supports differential outputs, the inverted outputs can be ignored.

### Pinout

The encoder should be connected to the microcontroller as follows:

| Encoder Pin | MCU Pin |
| ----------- | ------- |
| A           | `PD0`   |
| B           | `PD1`   |
| Common      | `GND`   |

Internal pull-up resistors are enabled on the data pins, so external resistors are not required.

### Platform Mode Switch

A switch or jumper selects between macOS and Windows/Linux mode at boot:

| Switch (PD7) | Mode |
| ------------ | ---- |
| Open (default) | Windows / Linux (Boot Protocol Mouse) |
| Closed to GND  | macOS (non-boot HID, wheel-only report) |

The switch is read once at power-on before USB enumeration. Changing it while running has no effect until the next reset or power cycle.

Linux supports the wheel-only (macOS) mode, but follows Windows precedent for scroll resolution behaviour, so the Windows/Linux mode is recommended for Linux users.

## Configuring Scroll Resolution

The scroll granularity is defined by platform-specific macros in `Descriptors.c`. These values are declared in the HID report descriptors as the Physical Maximum of the Resolution Multiplier feature report.

To change the scroll resolution:
1. Open `Descriptors.c`.
2. Locate the following lines:
   ```c
   #define SCROLL_RESOLUTION_MULTIPLIER_MAC 8
   #define SCROLL_RESOLUTION_MULTIPLIER_WIN 8
   ```
3. Change the value for your target platform.
4. Rebuild and flash the firmware.

### OS-Specific Behavior

The relationship between the multiplier and scroll "feel" varies by operating system.

| Platform | Macro | Default | Effect of Higher Value |
|---|---|---------|---|
| **macOS** | `SCROLL_RESOLUTION_MULTIPLIER_MAC` | `8`     | Coarser scrolling |
| **Windows/Linux** | `SCROLL_RESOLUTION_MULTIPLIER_WIN` | `12`    | Finer scrolling |

**macOS Details:**
Counter-intuitively, **lower** values result in **finer** scrolling. macOS reports its effective scroll resolution via `HIDScrollResolution` in the IO registry as a 16.16 fixed-point number. You can verify this with:
```bash
ioreg -r -l -w 0 -c IOHIDDevice | grep -A 30 "MPG Scroll Wheel" | grep Resolution
```

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
    The output file will be located at `build/scroll-wheel.hex`. A single firmware image supports both platforms; the mode is selected by the platform switch at boot.

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
