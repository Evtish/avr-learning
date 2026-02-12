#include <avr/io.h>
#include <avr/interrupt.h>

#include "spm.h"

volatile bool spm_ready = false;

ISR(SPM_READY_vect) {
	spm_ready = true;
}

void spm_init(void) {
	SPMCSR |= (1 << SPMIE); // SPM ready interrupt
}
