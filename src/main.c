#include "stm32f4xx.h"
#include "stm32f4_discovery.h"
#include "serial.h"
#include "vt100.h"
#include "start.h"
#include "game.h"


int main(void) {
	serial_init(115200);
	vt100_clear_screen();
	window('#');
	home_page();
	game_launch();
	init_player();
	init_enemy();
	vt100_move(2, 2);
	serial_puts("Life : 3");

	/* Initialize LEDs */
	STM_EVAL_LEDInit(LED3);
	STM_EVAL_LEDInit(LED4);
	STM_EVAL_LEDInit(LED5);
	STM_EVAL_LEDInit(LED6);

	/* Turn on LEDs */
	STM_EVAL_LEDOn(LED3);
	STM_EVAL_LEDOn(LED4);
	STM_EVAL_LEDOn(LED5);
	STM_EVAL_LEDOn(LED6);

	/* Infinite loop */
	while (1) {
		signed char touch = serial_get_last_char();
		if (touch == ' ') {
			shoot();
		}
		move_player(touch);
		shoot_enemy();
		timer();
	}
}

/*
 * Callback used by stm32f4_discovery_audio_codec.c.
 * Refer to stm32f4_discovery_audio_codec.h for more info.
 */
void EVAL_AUDIO_TransferComplete_CallBack(uint32_t pBuffer, uint32_t Size) {
	/* TODO, implement your code here */
	return;
}

/*
 * Callback used by stm324xg_eval_audio_codec.c.
 * Refer to stm324xg_eval_audio_codec.h for more info.
 */
uint16_t EVAL_AUDIO_GetSampleCallBack(void) {
	/* TODO, implement your code here */
	return -1;
}
