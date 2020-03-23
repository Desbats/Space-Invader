#include "vt100.h"
#include "serial.h"
#include "start.h"

void window(uint8_t character) {
	uint8_t i;
	for (i = 1; i < 80; i++) {
		vt100_move(i, 1);
		serial_putchar(character);
		vt100_move(i, 24);
		serial_putchar(character);
	}
	for (i = 1; i < 24; i++) {
		vt100_move(1, i);
		serial_putchar(character);
		vt100_move(80, i);
		serial_putchar(character);
	}
}

void home_page(void) {
	vt100_move(33, 6);
	serial_puts("SPACE INVADERS");
	vt100_move(33, 15);
	serial_puts("Insert a coin");
}


void game_launch(void) {
	while (serial_get_last_char() != ' ') {
	}
	vt100_clear_screen();
	window('#');

}
