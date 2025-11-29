#include <avr/io.h>
#include <util/delay.h>

// On Pro Micro/32U4:
// PD5 is the TX LED (Active LOW usually, or High depending on circuit)
// PB0 is the RX LED

int main(void)
{
    // 1. Set PD5 as Output
    // DDRD is the Data Direction Register for Port D
    DDRD |= (1 << PD5);

    while (1) {
        // 2. Toggle PD5
        // PIND allows reading the current state to toggle it
        // Note: Writing logic 1 to a PINx register toggles the corresponding bit in PORTx
        PIND = (1 << PD5);

        // 4 Hz blink
        _delay_ms(125);
    }
}