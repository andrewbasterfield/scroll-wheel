/**
 * \file
 *
 *  USB Device Descriptors, for library use when in USB device mode. Descriptors are special
 *  computer-readable structures which the host requests upon device enumeration, to determine
 *  the device's capabilities and functions.
 */
#include "Descriptors.h"

/**
 * \brief Defines the scaling factor for high-resolution scrolling.
 *
 * This value is used in the HID Report Descriptor to define the resolution
 * of the scroll wheel. A higher value results in slower, finer scrolling.
 * The value is used in a formula by the host OS:
 * (Scroll Value) = (Reported Value) * 120 / SCROLL_RESOLUTION_MULTIPLIER
 *
 * So, a value of 4 here means each encoder tick is 1/4th of a standard scroll detent.
 */
#define SCROLL_RESOLUTION_MULTIPLIER 8

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
const USB_Descriptor_HIDReport_Datatype_t PROGMEM MouseReport[] =
{
    HID_RI_USAGE_PAGE(8, 0x01), /* Generic Desktop */
    HID_RI_USAGE(8, 0x02), /* Mouse */
    HID_RI_COLLECTION(8, 0x01), /* Application */
    HID_RI_COLLECTION(8, 0x02), /* Logical Collection */
    /* Feature Report (ID 2) - Resolution Multiplier */
    HID_RI_REPORT_ID(8, 2),
    HID_RI_USAGE_PAGE(8, 0x01), /* Generic Desktop */
    HID_RI_USAGE(8, 0x48), /* Usage: Resolution Multiplier */
    /* Logical/Physical range for Windows & firmware compatibility */
    HID_RI_LOGICAL_MINIMUM(8, 0),
    HID_RI_LOGICAL_MAXIMUM(8, 1),
    HID_RI_PHYSICAL_MINIMUM(8, 1),
    HID_RI_PHYSICAL_MAXIMUM(8, SCROLL_RESOLUTION_MULTIPLIER),
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
    HID_RI_END_COLLECTION(0),
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
const USB_Descriptor_Device_t PROGMEM DeviceDescriptor =
{
    .Header = {.Size = sizeof(USB_Descriptor_Device_t), .Type = DTYPE_Device},

    .USBSpecification = VERSION_BCD(1, 1, 0),
    .Class = USB_CSCP_NoDeviceClass,
    .SubClass = USB_CSCP_NoDeviceSubclass,
    .Protocol = USB_CSCP_NoDeviceProtocol,

    .Endpoint0Size = FIXED_CONTROL_ENDPOINT_SIZE,

    // Vendor and Product IDs.
    // The Atmel vendor ID is used here, with a custom product ID.
    // For a commercial product, you should obtain your own VID and PID.
    .VendorID = 0x03EB,
    .ProductID = 0x2042,
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
const USB_Descriptor_Configuration_t PROGMEM ConfigurationDescriptor =
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
        // This is a non-bootable HID device, as it does not conform to the
        // standard mouse or keyboard boot protocols.
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
        .HIDReportLength = sizeof(MouseReport)
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
const USB_Descriptor_String_t PROGMEM ProductString = USB_STRING_DESCRIPTOR(L"MPG Scroll Wheel");

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

    switch (DescriptorType) {
        case DTYPE_Device:
            Address = &DeviceDescriptor;
            Size = sizeof(USB_Descriptor_Device_t);
            break;
        case DTYPE_Configuration:
            Address = &ConfigurationDescriptor;
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
                    Address = &ProductString;
                    Size = pgm_read_byte(&ProductString.Header.Size);
                    break;
            }

            break;
        case HID_DTYPE_HID:
            Address = &ConfigurationDescriptor.HID_MouseHID;
            Size = sizeof(USB_HID_Descriptor_HID_t);
            break;
        case HID_DTYPE_Report:
            // Ensure the requested interface index is our HID interface
            if (wIndex == INTERFACE_ID_Mouse) {
                Address = &MouseReport;
                Size = sizeof(MouseReport);
            }
            break;
    }

    *DescriptorAddress = Address;
    return Size;
}
