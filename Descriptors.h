/**
 * \file
 *
 *  Header file for Descriptors.c.
 */

#ifndef _DESCRIPTORS_H_
#define _DESCRIPTORS_H_

/* Includes: */
#include <LUFA/Drivers/USB/USB.h>
#include <avr/pgmspace.h>

/* Macros: */
/** HID Report ID for the scroll/mouse input report. */
#define REPORT_ID_SCROLL          1

/** HID Report ID for the resolution multiplier feature report. */
#define REPORT_ID_FEATURE         2

/** USB Vendor ID (Atmel/Microchip demo VID). */
#define USB_VID                   0x03EB

/**
 * \brief Endpoint address of the Mouse HID reporting IN endpoint.
 *
 * This is the endpoint where the device will send HID reports to the host.
 */
#define MOUSE_EPADDR              (ENDPOINT_DIR_IN | 1)

/**
 * \brief Platform mode switch pin.
 *
 * Connect a switch between this pin and GND.
 * Open (pull-up high) = Windows/Linux mode.
 * Closed (pulled low) = macOS mode.
 * Read once at boot before USB enumeration.
 */
#define PLATFORM_SWITCH_DDR   DDRD
#define PLATFORM_SWITCH_PORT  PORTD
#define PLATFORM_SWITCH_PIN   PIND
#define PLATFORM_SWITCH_BIT   PD7

/** Runtime platform mode flag, set before USB_Init(). */
extern bool mac_mode;

/* Type Defines: */
/** \brief HID report for Windows/Linux mode (Boot Protocol Mouse, 4 bytes). */
typedef struct
{
    uint8_t Button; /**< Button mask (Bits 0-2). */
    int8_t  X;      /**< X axis movement. */
    int8_t  Y;      /**< Y axis movement. */
    int8_t  Wheel;  /**< Current delta wheel movement. */
} ATTR_PACKED USB_ScrollReport_Data_t;

/** \brief HID report for macOS mode (wheel only, 1 byte). */
typedef struct
{
    int8_t  Wheel;  /**< Current delta wheel movement. */
} ATTR_PACKED USB_ScrollReport_Mac_t;

/**
 * \brief Size in bytes of the Mouse HID reporting IN endpoint.
 */
#define MOUSE_EPSIZE              8

/**
 * \brief Type define for the device configuration descriptor structure.
 *
 * This must be defined in the application code, as the configuration descriptor contains several
 * sub-descriptors which vary between devices, and which describe the device's usage to the host.
 */
typedef struct {
	USB_Descriptor_Configuration_Header_t Config;
	USB_Descriptor_Interface_t HID_Interface;
	USB_HID_Descriptor_HID_t HID_MouseHID;
	USB_Descriptor_Endpoint_t HID_ReportINEndpoint;
} USB_Descriptor_Configuration_t;

/**
 * \brief Enum for the device interface descriptor IDs within the device.
 *
 * Each interface descriptor should have a unique ID index associated with it, which can be used to refer to the
 * interface from other descriptors.
 */
enum InterfaceDescriptors_t {
	INTERFACE_ID_Mouse = 0, /**< Mouse interface descriptor ID */
};

/**
 * \brief Enum for the device string descriptor IDs within the device.
 *
 * Each string descriptor should have a unique ID index associated with it, which can be used to refer to the string from
 * other descriptors.
 */
enum StringDescriptors_t {
	STRING_ID_Language = 0, /**< Supported Languages string descriptor ID (must be zero) */
	STRING_ID_Manufacturer = 1, /**< Manufacturer string ID */
	STRING_ID_Product = 2, /**< Product string ID */
};

/* Function Prototypes: */
/**
 * \brief Callback function for the retrieval of device descriptors.
 *
 * This function is called by the LUFA library when the host requests a descriptor.
 * It should return the address and size of the requested descriptor.
 */
uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue,
                                    const uint16_t wIndex,
                                    const void **const DescriptorAddress)
ATTR_WARN_UNUSED_RESULT ATTR_NON_NULL_PTR_ARG(3);

#endif
