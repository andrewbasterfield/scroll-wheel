/**
 * \file
 *
 *  Quadrature rotary encoder driver. Reads a two-channel encoder on
 *  INT0/INT1 (PD0/PD1) via pin-change interrupts and accumulates a
 *  signed step count that the application retrieves with Encoder_GetDelta().
 */

#include "encoder.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

/**
 * \brief Volatile variable to accumulate encoder steps.
 *
 * This variable is incremented or decremented by the ISR for each step of
 * the rotary encoder. It is then read and processed in the main context.
 */
static volatile int8_t encoder_delta = 0;

/**
 * \brief Reads the rotary encoder and updates encoder_delta.
 *
 * This function implements a robust quadrature encoder state machine. It is
 * called from the ISRs of the two encoder pins. It compares the new state
 * of the encoder with the old state to determine the direction of rotation.
 */
static void ReadEncoder(void) {
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

void Encoder_Init(void) {
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
}

int8_t Encoder_GetDelta(void) {
	int8_t delta;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		delta = encoder_delta;
		encoder_delta = 0;
	}
	return delta;
}
