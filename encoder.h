/**
 * \file
 *
 *  Quadrature rotary encoder driver for INT0/INT1 (PD0/PD1).
 */

#ifndef _ENCODER_H_
#define _ENCODER_H_

#include <stdint.h>

/** Initialise encoder hardware (pin directions, pull-ups, external interrupts). */
void Encoder_Init(void);

/**
 * \brief Read and reset the accumulated encoder delta atomically.
 *
 * Safe to call from main context while ISRs are running.
 * Returns the number of quadrature steps since the last call.
 */
int8_t Encoder_GetDelta(void);

#endif
