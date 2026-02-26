#include <stdbool.h>
//#include <stdint.h>

//#include <avr/io.h>

#include "twi.h"
#include "oled.h"

int main(void) {
	twi_init();
	oled_init();
	oled_clean();

	//while (true) {
	//	
	//}

	return 0;
}
