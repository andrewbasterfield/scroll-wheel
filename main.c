/**
 * \file
 *
 *  Main source file for the scroll wheel project. This file contains the main
 *  tasks of the project and is responsible for the initial application hardware
 *  configuration.
 */

#include "Descriptors.h"
#include "encoder.h"
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/power.h>
#include <util/delay.h>
#include <LUFA/Drivers/USB/USB.h>

// --- Global variables ---

/** Platform mode: true = macOS (wheel-only), false = Windows/Linux (Boot Mouse). */
bool mac_mode = false;

/**
 * \brief Stores the current resolution multiplier value set by the host.
 */
static uint8_t ResolutionMultiplier = 1;


/**
 * \brief Buffer to hold the previously generated HID report, for comparison purposes inside the HID class driver.
 *
 * This buffer is required by the LUFA HID class driver to determine if the
 * report has changed and needs to be sent to the host.
 */
static uint8_t PrevMouseHIDReportBuffer[sizeof(USB_ScrollReport_Data_t)];

/**
 * \brief LUFA HID Class driver interface configuration and state information.
 *
 * This structure is passed to all HID Class driver functions, so that multiple
 * instances of the same class within a device can be differentiated from one
 * another.
 */
USB_ClassInfo_HID_Device_t Mouse_HID_Interface =
{
	.Config =
	{
		.InterfaceNumber = INTERFACE_ID_Mouse,
		.ReportINEndpoint =
		{
			.Address = MOUSE_EPADDR,
			.Size = MOUSE_EPSIZE,
			.Banks = 1,
		},
		.PrevReportINBuffer = PrevMouseHIDReportBuffer,
		.PrevReportINBufferSize = sizeof(PrevMouseHIDReportBuffer),
	},
};

/**
 * \brief Main program entry point.
 *
 * This routine contains the overall program flow, including initial setup of
 * all components and the main program loop.
 */
int main(void) {
	// It's a good practice to disable the watchdog timer at the start,
	// as it may have been enabled by the bootloader.
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	// The ATmega32U4 runs at 16MHz, but the bootloader may have set a
	// prescaler. We disable it to run at full speed.
	clock_prescale_set(clock_div_1);

	// --- Platform Switch ---
	// Read the switch pin before USB_Init() so the correct descriptors are
	// served during enumeration. Low = macOS, high = Windows/Linux.
	PLATFORM_SWITCH_DDR  &= ~(1 << PLATFORM_SWITCH_BIT); // input
	PLATFORM_SWITCH_PORT |=  (1 << PLATFORM_SWITCH_BIT);  // pull-up
	_delay_ms(1); // let pull-up charge before reading (needed on cold boot)
	mac_mode = !(PLATFORM_SWITCH_PIN & (1 << PLATFORM_SWITCH_BIT));

	// --- Encoder Initialization ---
	Encoder_Init();

	// Initialize the USB stack. This function is configured by LUFAConfig.h.
	USB_Init();

	// Enable global interrupts so that the ISRs and the USB stack can function.
	GlobalInterruptEnable();

	// The main loop of the application. It continuously runs the USB tasks.
	for (;;) {
		HID_Device_USBTask(&Mouse_HID_Interface);
		USB_USBTask();
	}
}

/**
 * \brief Event handler for the library USB Connection event.
 */
void EVENT_USB_Device_Connect(void) {
	// Optional: You can add code here to run when the device is connected.
}

/**
 * \brief Event handler for the library USB Disconnection event.
 */
void EVENT_USB_Device_Disconnect(void) {
	// Optional: You can add code here to run when the device is disconnected.
}

/**
 * \brief Event handler for the library USB Configuration Changed event.
 */
void EVENT_USB_Device_ConfigurationChanged(void) {
	bool ConfigSuccess = true;
	ConfigSuccess &= HID_Device_ConfigureEndpoints(&Mouse_HID_Interface);
	USB_Device_EnableSOFEvents();
}

/**
 * \brief Event handler for the library USB Control Request reception event.
 */
void EVENT_USB_Device_ControlRequest(void) {
	HID_Device_ProcessControlRequest(&Mouse_HID_Interface);
}

/**
 * \brief Event handler for the USB device Start Of Frame event.
 */
void EVENT_USB_Device_StartOfFrame(void) {
	HID_Device_MillisecondElapsed(&Mouse_HID_Interface);
}

/**
 * \brief HID class driver callback function for the creation of HID reports to the host.
 */
bool CALLBACK_HID_Device_CreateHIDReport(USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
                                         uint8_t* const ReportID,
                                         const uint8_t ReportType,
                                         void* ReportData,
                                         uint16_t* const ReportSize)
{
	// Check if the host is requesting a FEATURE report
	if (ReportType == HID_REPORT_ITEM_Feature)
	{
		if (*ReportID == REPORT_ID_FEATURE)
		{
			uint8_t* FeatureReport = (uint8_t*)ReportData;
			*FeatureReport = ResolutionMultiplier;
			*ReportSize = sizeof(ResolutionMultiplier);
			return true;
		}
	}

	// Otherwise, create an INPUT report for the scroll wheel
	*ReportID = REPORT_ID_SCROLL;

	int8_t delta = Encoder_GetDelta();

	if (mac_mode) {
		USB_ScrollReport_Mac_t* Report = (USB_ScrollReport_Mac_t*)ReportData;
		Report->Wheel = delta;
		*ReportSize = sizeof(USB_ScrollReport_Mac_t);
	} else {
		USB_ScrollReport_Data_t* Report = (USB_ScrollReport_Data_t*)ReportData;
		Report->Button = 0;
		Report->X      = 0;
		Report->Y      = 0;
		Report->Wheel  = delta;
		*ReportSize = sizeof(USB_ScrollReport_Data_t);
	}

	return (delta != 0);
}

/**
 * \brief HID class driver callback function for the processing of HID reports from the host.
 */
void CALLBACK_HID_Device_ProcessHIDReport(USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
                                          const uint8_t ReportID,
                                          const uint8_t ReportType,
                                          const void* ReportData,
                                          const uint16_t ReportSize)
{
	// Check if the host is setting a FEATURE report
	if (ReportType == HID_REPORT_ITEM_Feature)
	{
		if (ReportID == REPORT_ID_FEATURE)
		{
			ResolutionMultiplier = ((uint8_t*)ReportData)[0];
		}
	}
}
