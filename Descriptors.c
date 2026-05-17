/**
 * \file
 *
 *  USB Device Descriptors, for library use when in USB device mode. Descriptors are special
 *  computer-readable structures which the host requests upon device enumeration, to determine
 *  the device's capabilities and functions.
 */
#include "Descriptors.h"

/**
 * \brief Defines the scaling factors for high-resolution scrolling.
 *
 * These values are used in the HID Report Descriptors to define the resolution
 * of the scroll wheel for each platform.
 *
 * Formula: (Scroll Value) = (Reported Value) * 120 / Multiplier
 *
 * Note on OS behavior:
 * - Windows/Linux: Higher values result in finer scrolling.
 * - macOS: Lower values result in finer scrolling.
 */
#define SCROLL_RESOLUTION_MULTIPLIER_MAC 8
#define SCROLL_RESOLUTION_MULTIPLIER_WIN 12

/**
 * \brief HID class report descriptor.
 *
 * This is a special descriptor constructed with values from the
 * USBIF HID class specification to describe the reports and capabilities of the HID device. This
 * descriptor is parsed by the host and its contents used to determine what data (and in what encoding)
 * the device will send, and what it may be sent back from the host.
 *
 * This descriptor defines a simple HID mouse with a single relative axis for the scroll wheel.
 * It does not include X/Y movement or buttons, to keep the device as simple as possible.
 */
/** \brief HID report descriptor for macOS mode (non-boot, wheel only). */
const USB_Descriptor_HIDReport_Datatype_t PROGMEM MouseReport_Mac[] =
{
    HID_RI_USAGE_PAGE(8, 0x01),        /* Generic Desktop */
    HID_RI_USAGE(8, 0x02),             /* Mouse */
    HID_RI_COLLECTION(8, 0x01),        /* Application */
        HID_RI_COLLECTION(8, 0x02), /* Logical Collection */
            /* Feature Report (ID 2) - Resolution Multiplier */
            HID_RI_REPORT_ID(8, 2),
            HID_RI_USAGE_PAGE(8, 0x01), /* Generic Desktop */
            HID_RI_USAGE(8, 0x48), /* Usage: Resolution Multiplier */
            HID_RI_LOGICAL_MINIMUM(8, 0),
            HID_RI_LOGICAL_MAXIMUM(8, 1),
            HID_RI_PHYSICAL_MINIMUM(8, 1),
            HID_RI_PHYSICAL_MAXIMUM(8, SCROLL_RESOLUTION_MULTIPLIER_MAC),
            HID_RI_REPORT_COUNT(8, 1),
            HID_RI_REPORT_SIZE(8, 8),
            HID_RI_FEATURE(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE | HID_IOF_NON_VOLATILE),

            /* Input Report (ID 1) - Vertical Wheel */
            HID_RI_REPORT_ID(8, 1),
            HID_RI_USAGE_PAGE(8, 0x01), /* Generic Desktop */
            HID_RI_USAGE(8, 0x38), /*   Wheel */
            HID_RI_LOGICAL_MINIMUM(8, -127),
            HID_RI_LOGICAL_MAXIMUM(8, 127),
            HID_RI_REPORT_SIZE(8, 8),
            HID_RI_REPORT_COUNT(8, 1),
            HID_RI_INPUT(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_RELATIVE),
        HID_RI_END_COLLECTION(0), /* Close Logical Collection */
    HID_RI_END_COLLECTION(0)
};

/** \brief HID report descriptor for Windows/Linux mode (Boot Protocol Mouse). */
const USB_Descriptor_HIDReport_Datatype_t PROGMEM MouseReport_Win[] =
{
    HID_RI_USAGE_PAGE(8, 0x01),        /* Generic Desktop */
    HID_RI_USAGE(8, 0x02),             /* Mouse */
    HID_RI_COLLECTION(8, 0x01),        /* Application */
        /* Pointer Collection */
        HID_RI_USAGE(8, 0x01),         /* Pointer */
        HID_RI_COLLECTION(8, 0x00),    /* Physical */

            /* Input Report (ID 1) - Buttons/X/Y part */
            HID_RI_REPORT_ID(8, 1),

            /* Buttons (3 buttons) */
            HID_RI_USAGE_PAGE(8, 0x09),      /* Button */
            HID_RI_USAGE_MINIMUM(8, 0x01),
            HID_RI_USAGE_MAXIMUM(8, 0x03),
            HID_RI_LOGICAL_MINIMUM(8, 0),
            HID_RI_LOGICAL_MAXIMUM(8, 1),
            HID_RI_REPORT_COUNT(8, 3),
            HID_RI_REPORT_SIZE(8, 1),
            HID_RI_INPUT(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE),

            /* Padding (5 bits) to round up to 1 byte */
            HID_RI_REPORT_COUNT(8, 1),
            HID_RI_REPORT_SIZE(8, 5),
            HID_RI_INPUT(8, HID_IOF_CONSTANT),

            /* X and Y Axis */
            HID_RI_USAGE_PAGE(8, 0x01),      /* Generic Desktop */
            HID_RI_USAGE(8, 0x30),           /* X */
            HID_RI_USAGE(8, 0x31),           /* Y */
            HID_RI_LOGICAL_MINIMUM(8, -127),
            HID_RI_LOGICAL_MAXIMUM(8, 127),
            HID_RI_REPORT_SIZE(8, 8),
            HID_RI_REPORT_COUNT(8, 2),
            HID_RI_INPUT(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_RELATIVE),

            /* Logical Collection to associate Wheel with Multiplier */
            HID_RI_COLLECTION(8, 0x02),    /* Logical */

                /* Feature Report (ID 2) - Resolution Multiplier */
                HID_RI_REPORT_ID(8, 2),
                HID_RI_USAGE_PAGE(8, 0x01),      /* Generic Desktop */
                HID_RI_USAGE(8, 0x48),           /* Usage: Resolution Multiplier */
                HID_RI_LOGICAL_MINIMUM(8, 0),
                HID_RI_LOGICAL_MAXIMUM(8, 1),
                HID_RI_PHYSICAL_MINIMUM(8, 1),
                HID_RI_PHYSICAL_MAXIMUM(8, SCROLL_RESOLUTION_MULTIPLIER_WIN),
                HID_RI_REPORT_COUNT(8, 1),
                HID_RI_REPORT_SIZE(8, 8),
                HID_RI_FEATURE(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE | HID_IOF_NON_VOLATILE),

                /* Vertical Wheel (Input Report ID 1 continuation) */
                HID_RI_REPORT_ID(8, 1),
                HID_RI_USAGE(8, 0x38),           /* Wheel */
                HID_RI_LOGICAL_MINIMUM(8, -127),
                HID_RI_LOGICAL_MAXIMUM(8, 127),
                HID_RI_PHYSICAL_MINIMUM(8, -127),
                HID_RI_PHYSICAL_MAXIMUM(8, 127),
                HID_RI_REPORT_SIZE(8, 8),
                HID_RI_REPORT_COUNT(8, 1),
                HID_RI_INPUT(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_RELATIVE),

            HID_RI_END_COLLECTION(0), /* End Logical */
        HID_RI_END_COLLECTION(0), /* End Physical */
    HID_RI_END_COLLECTION(0)
};

/**
 * \brief Device descriptor structure.
 *
 * This descriptor, located in FLASH memory, describes the overall
 * device characteristics, including the supported USB version, control endpoint size and the
 * number of device configurations. The descriptor is read out by the USB host when the enumeration
 * process begins.
 */
// Product IDs differ per mode so the host OS doesn't serve cached descriptors
// from the wrong mode.
#define PRODUCT_ID_WIN 0x2044
#define PRODUCT_ID_MAC 0x2045

const USB_Descriptor_Device_t PROGMEM DeviceDescriptor_Win =
{
    .Header = {.Size = sizeof(USB_Descriptor_Device_t), .Type = DTYPE_Device},

    .USBSpecification = VERSION_BCD(1, 1, 0),
    .Class = USB_CSCP_NoDeviceClass,
    .SubClass = USB_CSCP_NoDeviceSubclass,
    .Protocol = USB_CSCP_NoDeviceProtocol,

    .Endpoint0Size = FIXED_CONTROL_ENDPOINT_SIZE,

    .VendorID = 0x03EB,
    .ProductID = PRODUCT_ID_WIN,
    .ReleaseNumber = VERSION_BCD(0, 0, 1),

    .ManufacturerStrIndex = STRING_ID_Manufacturer,
    .ProductStrIndex = STRING_ID_Product,
    .SerialNumStrIndex = NO_DESCRIPTOR,

    .NumberOfConfigurations = FIXED_NUM_CONFIGURATIONS
};

const USB_Descriptor_Device_t PROGMEM DeviceDescriptor_Mac =
{
    .Header = {.Size = sizeof(USB_Descriptor_Device_t), .Type = DTYPE_Device},

    .USBSpecification = VERSION_BCD(1, 1, 0),
    .Class = USB_CSCP_NoDeviceClass,
    .SubClass = USB_CSCP_NoDeviceSubclass,
    .Protocol = USB_CSCP_NoDeviceProtocol,

    .Endpoint0Size = FIXED_CONTROL_ENDPOINT_SIZE,

    .VendorID = 0x03EB,
    .ProductID = PRODUCT_ID_MAC,
    .ReleaseNumber = VERSION_BCD(0, 0, 1),

    .ManufacturerStrIndex = STRING_ID_Manufacturer,
    .ProductStrIndex = STRING_ID_Product,
    .SerialNumStrIndex = NO_DESCRIPTOR,

    .NumberOfConfigurations = FIXED_NUM_CONFIGURATIONS
};

/**
 * \brief Configuration descriptor structure.
 *
 * This descriptor, located in FLASH memory, describes the usage
 * of the device in one of its supported configurations, including information about any device interfaces
 * and endpoints. The descriptor is read out by the USB host during the enumeration process when selecting
 * a configuration so that the host may correctly communicate with the USB device.
 */
/**
 * \brief Shared configuration descriptor fields (everything except
 * HID_Interface subclass/protocol and HIDReportLength).
 *
 * Two variants are needed because the HID interface subclass/protocol and the
 * report descriptor length differ between macOS and Windows/Linux modes.
 */

/** macOS configuration: non-boot HID, wheel-only report descriptor. */
const USB_Descriptor_Configuration_t PROGMEM ConfigurationDescriptor_Mac =
{
    .Config =
    {
        .Header = {
            .Size = sizeof(USB_Descriptor_Configuration_Header_t), .Type = DTYPE_Configuration
        },
        .TotalConfigurationSize = sizeof(USB_Descriptor_Configuration_t),
        .TotalInterfaces = 1,
        .ConfigurationNumber = 1,
        .ConfigurationStrIndex = NO_DESCRIPTOR,
        .ConfigAttributes = (USB_CONFIG_ATTR_RESERVED | USB_CONFIG_ATTR_SELFPOWERED),
        .MaxPowerConsumption = USB_CONFIG_POWER_MA(100)
    },
    .HID_Interface =
    {
        .Header = {.Size = sizeof(USB_Descriptor_Interface_t), .Type = DTYPE_Interface},
        .InterfaceNumber = INTERFACE_ID_Mouse,
        .AlternateSetting = 0x00,
        .TotalEndpoints = 1,
        .Class = HID_CSCP_HIDClass,
        .SubClass = HID_CSCP_NonBootSubclass,
        .Protocol = HID_CSCP_NonBootProtocol,
        .InterfaceStrIndex = NO_DESCRIPTOR
    },
    .HID_MouseHID =
    {
        .Header = {.Size = sizeof(USB_HID_Descriptor_HID_t), .Type = HID_DTYPE_HID},
        .HIDSpec = VERSION_BCD(1, 1, 1),
        .CountryCode = 0x00,
        .TotalReportDescriptors = 1,
        .HIDReportType = HID_DTYPE_Report,
        .HIDReportLength = sizeof(MouseReport_Mac)
    },
    .HID_ReportINEndpoint =
    {
        .Header = {.Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint},
        .EndpointAddress = MOUSE_EPADDR,
        .Attributes = (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
        .EndpointSize = MOUSE_EPSIZE,
        .PollingIntervalMS = 0x05
    }
};

/** Windows/Linux configuration: Boot Protocol Mouse, full report descriptor. */
const USB_Descriptor_Configuration_t PROGMEM ConfigurationDescriptor_Win =
{
    .Config =
    {
        .Header = {
            .Size = sizeof(USB_Descriptor_Configuration_Header_t), .Type = DTYPE_Configuration
        },
        .TotalConfigurationSize = sizeof(USB_Descriptor_Configuration_t),
        .TotalInterfaces = 1,
        .ConfigurationNumber = 1,
        .ConfigurationStrIndex = NO_DESCRIPTOR,
        .ConfigAttributes = (USB_CONFIG_ATTR_RESERVED | USB_CONFIG_ATTR_SELFPOWERED),
        .MaxPowerConsumption = USB_CONFIG_POWER_MA(100)
    },
    .HID_Interface =
    {
        .Header = {.Size = sizeof(USB_Descriptor_Interface_t), .Type = DTYPE_Interface},
        .InterfaceNumber = INTERFACE_ID_Mouse,
        .AlternateSetting = 0x00,
        .TotalEndpoints = 1,
        .Class = HID_CSCP_HIDClass,
        .SubClass = HID_CSCP_BootSubclass,
        .Protocol = HID_CSCP_MouseBootProtocol,
        .InterfaceStrIndex = NO_DESCRIPTOR
    },
    .HID_MouseHID =
    {
        .Header = {.Size = sizeof(USB_HID_Descriptor_HID_t), .Type = HID_DTYPE_HID},
        .HIDSpec = VERSION_BCD(1, 1, 1),
        .CountryCode = 0x00,
        .TotalReportDescriptors = 1,
        .HIDReportType = HID_DTYPE_Report,
        .HIDReportLength = sizeof(MouseReport_Win)
    },
    .HID_ReportINEndpoint =
    {
        .Header = {.Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint},
        .EndpointAddress = MOUSE_EPADDR,
        .Attributes = (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
        .EndpointSize = MOUSE_EPSIZE,
        .PollingIntervalMS = 0x05
    }
};

/**
 * \brief Language descriptor structure.
 *
 * This descriptor, located in FLASH memory, is returned when the host requests
 * the string descriptor with index 0 (the first index). It is actually an array of 16-bit integers, which indicate
 * via the language ID table available at USB.org what languages the device supports for its string descriptors.
 */
const USB_Descriptor_String_t PROGMEM LanguageString = USB_STRING_DESCRIPTOR_ARRAY(LANGUAGE_ID_ENG);

/**
 * \brief Manufacturer descriptor string.
 *
 * This is a Unicode string containing the manufacturer's details in human readable
 * form, and is read out upon request by the host when the appropriate string ID is requested, listed in the Device
 * Descriptor.
 */
const USB_Descriptor_String_t PROGMEM ManufacturerString = USB_STRING_DESCRIPTOR(L"My Company");

/**
 * \brief Product descriptor string.
 *
 * This is a Unicode string containing the product's details in human readable form,
 * and is read out upon request by a host when the appropriate string ID is requested, listed in the Device
 * Descriptor.
 */
const USB_Descriptor_String_t PROGMEM ProductString_Win = USB_STRING_DESCRIPTOR(L"MPG Scroll Wheel (Boot Protocol Mouse (Windows/Linux) mode)");
const USB_Descriptor_String_t PROGMEM ProductString_Mac = USB_STRING_DESCRIPTOR(L"MPG Scroll Wheel (Wheel-only (macOS) mode)");

/**
 * \brief This function is called by the library when in device mode, and must be overridden.
 *
 * This function is called by the library when in device mode, and must be overridden (see library "USB Descriptors"
 * documentation) by the application code so that the address and size of a requested descriptor can be given
 * to the USB library. When the device receives a Get Descriptor request on the control endpoint, this function
 * is called so that the descriptor details can be passed back and the appropriate descriptor sent back to the
 * USB host.
 */
uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue,
                                    const uint16_t wIndex,
                                    const void **const DescriptorAddress) {
    const uint8_t DescriptorType = (wValue >> 8);
    const uint8_t DescriptorNumber = (wValue & 0xFF);

    const void *Address = NULL;
    uint16_t Size = NO_DESCRIPTOR;

    const USB_Descriptor_Configuration_t *ConfigDesc =
        mac_mode ? &ConfigurationDescriptor_Mac : &ConfigurationDescriptor_Win;

    switch (DescriptorType) {
        case DTYPE_Device:
            Address = mac_mode ? &DeviceDescriptor_Mac : &DeviceDescriptor_Win;
            Size = sizeof(USB_Descriptor_Device_t);
            break;
        case DTYPE_Configuration:
            Address = ConfigDesc;
            Size = sizeof(USB_Descriptor_Configuration_t);
            break;
        case DTYPE_String:
            switch (DescriptorNumber) {
                case STRING_ID_Language:
                    Address = &LanguageString;
                    Size = pgm_read_byte(&LanguageString.Header.Size);
                    break;
                case STRING_ID_Manufacturer:
                    Address = &ManufacturerString;
                    Size = pgm_read_byte(&ManufacturerString.Header.Size);
                    break;
                case STRING_ID_Product:
                {
                    const USB_Descriptor_String_t *ps =
                        mac_mode ? &ProductString_Mac : &ProductString_Win;
                    Address = ps;
                    Size = pgm_read_byte(&ps->Header.Size);
                    break;
                }
            }

            break;
        case HID_DTYPE_HID:
            Address = &ConfigDesc->HID_MouseHID;
            Size = sizeof(USB_HID_Descriptor_HID_t);
            break;
        case HID_DTYPE_Report:
            if (wIndex == INTERFACE_ID_Mouse) {
                if (mac_mode) {
                    Address = &MouseReport_Mac;
                    Size = sizeof(MouseReport_Mac);
                } else {
                    Address = &MouseReport_Win;
                    Size = sizeof(MouseReport_Win);
                }
            }
            break;
    }

    *DescriptorAddress = Address;
    return Size;
}
