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
		/**
		 * \brief Endpoint address of the Mouse HID reporting IN endpoint.
		 *
		 * This is the endpoint where the device will send HID reports to the host.
		 */
		#define MOUSE_EPADDR              (ENDPOINT_DIR_IN | 1)

	/* Type Defines: */
        /**
         * \brief Custom HID report structure for the scroll wheel.
         *
         * This structure defines the layout of the HID report that will be sent to the host.
         * It contains a single signed 8-bit integer for the scroll wheel movement.
         */
        typedef struct
        {
            int8_t  Wheel; /**< Current delta wheel movement. */
        } ATTR_PACKED USB_ScrollReport_Data_t;

		/**
		 * \brief Size in bytes of the Mouse HID reporting IN endpoint.
		 *
		 * This is set to the size of our custom scroll report.
		 */
		#define MOUSE_EPSIZE              sizeof(USB_ScrollReport_Data_t)

		/**
		 * \brief Type define for the device configuration descriptor structure.
		 *
		 * This must be defined in the application code, as the configuration descriptor contains several
		 * sub-descriptors which vary between devices, and which describe the device's usage to the host.
		 */
		typedef struct
		{
			USB_Descriptor_Configuration_Header_t Config;
			USB_Descriptor_Interface_t            HID_Interface;
			USB_HID_Descriptor_HID_t              HID_MouseHID;
			USB_Descriptor_Endpoint_t             HID_ReportINEndpoint;
		} USB_Descriptor_Configuration_t;

		/**
		 * \brief Enum for the device interface descriptor IDs within the device.
		 *
		 * Each interface descriptor should have a unique ID index associated with it, which can be used to refer to the
		 * interface from other descriptors.
		 */
		enum InterfaceDescriptors_t
		{
			INTERFACE_ID_Mouse = 0, /**< Mouse interface descriptor ID */
		};

		/**
		 * \brief Enum for the device string descriptor IDs within the device.
		 *
		 * Each string descriptor should have a unique ID index associated with it, which can be used to refer to the string from
		 * other descriptors.
		 */
		enum StringDescriptors_t
		{
			STRING_ID_Language     = 0, /**< Supported Languages string descriptor ID (must be zero) */
			STRING_ID_Manufacturer = 1, /**< Manufacturer string ID */
			STRING_ID_Product      = 2, /**< Product string ID */
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
		                                    const void** const DescriptorAddress)
		                                    ATTR_WARN_UNUSED_RESULT ATTR_NON_NULL_PTR_ARG(3);

#endif
