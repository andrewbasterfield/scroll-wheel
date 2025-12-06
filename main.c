/**
 * \file
 *
 *  Main source file for the scroll wheel project. This file contains the main
 *  tasks of the project and is responsible for the initial application hardware
 *  configuration.
 */

#include "Descriptors.h"
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include <LUFA/Drivers/USB/USB.h>

// --- Global variables ---

/**
 * \brief Volatile variable to accumulate encoder steps.
 *
 * This variable is incremented or decremented by the ISR for each step of
 * the rotary encoder. It is then read and processed in the main context.
 */
static volatile int8_t encoder_delta = 0;

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

	// --- Encoder Initialization ---
	// The encoder pins are configured as inputs with internal pull-up resistors.
	// This is a common setup for mechanical rotary encoders.
	DDRD &= ~((1 << PD0) | (1 << PD1));
	PORTD |= (1 << PD0) | (1 << PD1);

	// We configure the external interrupts INT0 and INT1 to trigger on any
	// logical change. This allows us to catch all state changes of the encoder.
	EICRA |= (1 << ISC00); // Any logical change on INT0 generates an interrupt request.
	EICRA |= (1 << ISC10); // Any logical change on INT1 generates an interrupt request.

	// We enable the external interrupts for the encoder pins.
	EIMSK |= (1 << INT0) | (1 << INT1);

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
 * \brief Reads the rotary encoder and updates the global encoder_delta variable.
 *
 * This function implements a robust quadrature encoder state machine. It is
 * called from the ISRs of the two encoder pins. It compares the new state
 * of the encoder with the old state to determine the direction of rotation.
 */
void ReadEncoder(void) {
	static uint8_t old_state = 0;
	uint8_t new_state = 0;

	// The state is encoded as a 2-bit number, where bit 0 is the state of pin A
	// and bit 1 is the state of pin B.
	if (bit_is_set(PIND, PD0)) new_state |= (1 << 0); // Pin A
	if (bit_is_set(PIND, PD1)) new_state |= (1 << 1); // Pin B

	// If the state has not changed, there is nothing to do.
	if (new_state == old_state) return;

	// This state machine detects valid transitions for a quadrature encoder.
	// The sequence for clockwise rotation is 00 -> 01 -> 11 -> 10 -> 00.
	// The sequence for counter-clockwise is the reverse.
	// The state is encoded as (B << 1) | A.
	switch (old_state) {
		case 0b00:
			if (new_state == 0b01) encoder_delta++; // CW
			else if (new_state == 0b10) encoder_delta--; // CCW
			break;
		case 0b01:
			if (new_state == 0b11) encoder_delta++; // CW
			else if (new_state == 0b00) encoder_delta--; // CCW
			break;
		case 0b11:
			if (new_state == 0b10) encoder_delta++; // CW
			else if (new_state == 0b01) encoder_delta--; // CCW
			break;
		case 0b10:
			if (new_state == 0b00) encoder_delta++; // CW
			else if (new_state == 0b11) encoder_delta--; // CCW
			break;
	}

	old_state = new_state;
}

/**
 * \brief Interrupt Service Routine for encoder pin A (INT0).
 */
ISR(INT0_vect) {
	ReadEncoder();
}

/**
 * \brief Interrupt Service Routine for encoder pin B (INT1).
 */
ISR(INT1_vect) {
	ReadEncoder();
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
		// Report ID 2 is for the Resolution Multiplier
		if (*ReportID == 2)
		{
			uint8_t* FeatureReport = (uint8_t*)ReportData;
			*FeatureReport = ResolutionMultiplier;
			*ReportSize = sizeof(ResolutionMultiplier);
			return true;
		}
	}

	// Otherwise, create an INPUT report for the scroll wheel
	*ReportID = 1;
	USB_ScrollReport_Data_t* MouseReport = (USB_ScrollReport_Data_t*)ReportData;
	*MouseReport = (USB_ScrollReport_Data_t){
		.Button = 0,
		.X      = 0,
		.Y      = 0,
		.Wheel  = 0
	};

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		MouseReport->Wheel = encoder_delta;
		encoder_delta = 0;
	}

	*ReportSize = sizeof(USB_ScrollReport_Data_t);
	return (MouseReport->Wheel != 0);
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
		// Report ID 2 is for the Resolution Multiplier
		if (ReportID == 2)
		{
			ResolutionMultiplier = ((uint8_t*)ReportData)[0];
		}
	}
}
