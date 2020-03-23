#include "vt100.h"
#include "serial.h"
#include "start.h"
#include "game.h"

typedef struct {
	uint8_t l1[15][3];
	uint8_t l2[15][3];
} t_pos_enemy;

typedef struct {
	uint8_t x;
	uint8_t y;
	uint8_t life;
} t_pos_player;

t_pos_player player;
t_pos_enemy enemy;

static uint8_t compteur = 0;
static uint8_t a = 0x5B;
static uint8_t x_shoot_enemy;
static uint8_t y_shoot_enemy = 0;
static uint8_t win = 0;

t_pos_player *ptr_player = &player;
t_pos_enemy *ptr_enemy = &enemy;

void init_enemy(void) {
	uint8_t colone = 0;
	uint8_t x;
	win = 0;
	for (x = 5; x < 76; x = x + 5) {
		ptr_enemy->l1[colone][2] = 1;
		ptr_enemy->l2[colone][2] = 1;
		display_enemy(colone, 1, x, 3);
		display_enemy(colone, 2, x, 6);
		colone++;
	}
}

void init_player(void) {
	ptr_player->x = 39;
	ptr_player->y = 21;
	ptr_player->life = 3;
	vt100_move(ptr_player->x, ptr_player->y);
	serial_puts("alo");
}

void display_enemy(uint8_t colone, uint8_t ligne, uint8_t x, uint8_t y) {
	if (ask_enemy_dead(colone, ligne) == 1) {
		return;
	}
	if (ligne == 1) {
		ptr_enemy->l1[colone][0] = x;
		ptr_enemy->l1[colone][1] = y;
		vt100_move(ptr_enemy->l1[colone][0], ptr_enemy->l1[colone][1]);
		serial_puts("o-o");
	} else if (ligne == 2) {
		ptr_enemy->l2[colone][0] = x;
		ptr_enemy->l2[colone][1] = y;
		vt100_move(ptr_enemy->l2[colone][0], ptr_enemy->l2[colone][1]);
		serial_puts("o-o");
	}
}

void erase_enemy(uint8_t colone, uint8_t ligne) {
	if (ask_enemy_dead(colone, ligne) == 1) {
		return;
	}
	if (ligne == 1) {
		vt100_move(ptr_enemy->l1[colone][0], ptr_enemy->l1[colone][1]);
		serial_puts("   ");
	} else if (ligne == 2) {
		vt100_move(ptr_enemy->l2[colone][0], ptr_enemy->l2[colone][1]);
		serial_puts("   ");
	}
}

uint8_t PRNG_LSFR(void) {
	uint8_t bit0, bit3, bit5, xor;

	bit0 = (a >> 0) & 1;
	bit3 = (a >> 3) & 1;
	bit5 = (a >> 5) & 1;
	xor = bit0 ^ bit3 ^ bit5;
	a = a >> 1;
	a |= xor << 7;

	return a % 15;
}

void shoot_enemy(void) {
	if (y_shoot_enemy == 0) {
		uint8_t colone = PRNG_LSFR();
		if (ask_enemy_dead(colone, 2) == 0) {
			x_shoot_enemy = ptr_enemy->l2[colone][0] + 1;
			y_shoot_enemy = ptr_enemy->l2[colone][1] + 1;
		} else if (ask_enemy_dead(colone, 1) == 0) {
			x_shoot_enemy = ptr_enemy->l1[colone][0] + 1;
			y_shoot_enemy = ptr_enemy->l1[colone][1] + 1;
		} else {
			return;
		}
		vt100_move(x_shoot_enemy, y_shoot_enemy);
		serial_putchar('|');
	}
	vt100_move(x_shoot_enemy, y_shoot_enemy);
	serial_putchar(' ');
	y_shoot_enemy++;
	vt100_move(x_shoot_enemy, y_shoot_enemy);
	serial_putchar('|');
	if (y_shoot_enemy == ptr_player->y) {
		if (hitbox(x_shoot_enemy, y_shoot_enemy) == 1) {
			y_shoot_enemy = 0;
		} else {
			vt100_move(x_shoot_enemy, y_shoot_enemy);
			serial_putchar(' ');
			y_shoot_enemy = 0;
		}
	}
}

void shoot(void) {
	uint8_t x_shoot = ptr_player->x + 1;
	uint8_t y_shoot = ptr_player->y;
	uint32_t k = 0;
	uint32_t i = 0;
	uint8_t j = 0;
	uint32_t o = 0;
	signed char touch;
	vt100_move(x_shoot, y_shoot - 1);
	serial_putchar('(');
	vt100_move(x_shoot, y_shoot - 1);
	serial_putchar(')');
	for (y_shoot = 19; y_shoot > 1; y_shoot--) {
		for (i = 0; i < 0xF001; i++) {
			if (k == 0xFFF) {
				if (y_shoot == 1) {
					vt100_move(x_shoot, y_shoot + 1);
					serial_putchar(' ');
					return;
				}
				k = 0;
				vt100_move(x_shoot, y_shoot + 1);
				serial_putchar(' ');
				for (j = 0; j < 0x2F; j++) {
					vt100_move(x_shoot, y_shoot);
					serial_putchar('(');
					vt100_move(x_shoot, y_shoot);
					serial_putchar(')');

				}
				if (y_shoot_enemy == y_shoot) {
					if (x_shoot_enemy == x_shoot) {
						vt100_move(x_shoot, y_shoot);
						serial_putchar(' ');
						y_shoot_enemy = 0;
						return;
					}
				}
				y_shoot--;
			}
			if (o == 0x7800) {
				o = 0;
				shoot_enemy();
				move_enemy();
			}
			touch = serial_get_last_char();
			move_player(touch);
			k++;
			o++;
			if (hitbox(x_shoot, y_shoot) == 1) {
				vt100_move(x_shoot, y_shoot + 1);
				serial_putchar(' ');
				return;
			}
		}
		vt100_move(x_shoot, y_shoot + 1);
		serial_putchar(' ');
	}
}

uint8_t hitbox(uint8_t x, uint8_t y) {
	uint8_t i;
	uint8_t j;
	if (y == ptr_enemy->l1[0][1]) {
		for (i = 0; i < 15; i++) {
			if (x == ptr_enemy->l1[i][0] || x == ptr_enemy->l1[i][0] + 1
					|| x == ptr_enemy->l1[i][0] + 2) {
				if (ptr_enemy->l1[i][2] == 1) {
					for (j = 0; j < 0x1F; j++) {
						vt100_move(ptr_enemy->l1[i][0], ptr_enemy->l1[i][1]);
						serial_puts("x  ");
						vt100_move(ptr_enemy->l1[i][0], ptr_enemy->l1[i][1]);
						serial_puts("  x");
						vt100_move(ptr_enemy->l1[i][0], ptr_enemy->l1[i][1]);
						serial_puts(" x ");
					}
					erase_enemy(i, 1);
					ptr_enemy->l1[i][2] = 0;
					win++;
					return 1;
				}
			}
		}
	}
	else if (y == ptr_enemy->l2[0][1]) {
		for (i = 0; i < 15; i++) {
			if (x == ptr_enemy->l2[i][0] || x == ptr_enemy->l2[i][0] + 1
					|| x == ptr_enemy->l2[i][0] + 2) {
				if (ptr_enemy->l2[i][2] == 1) {
					for (j = 0; j < 0x1F; j++) {
						vt100_move(ptr_enemy->l2[i][0], ptr_enemy->l2[i][1]);
						serial_puts("x  ");
						vt100_move(ptr_enemy->l2[i][0], ptr_enemy->l2[i][1]);
						serial_puts("  x");
						vt100_move(ptr_enemy->l2[i][0], ptr_enemy->l2[i][1]);
						serial_puts(" x ");
					}
					erase_enemy(i, 2);
					ptr_enemy->l2[i][2] = 0;
					win++;
					return 1;
				}
			}
		}
	}
	else if (y == ptr_player->y) {
		if (x == ptr_player->x || x == ptr_player->x + 1
				|| x == ptr_player->x + 2) {
			for (i = 0; i < 0x2F; i++) {
				vt100_move(ptr_player->x, ptr_player->y);
				serial_puts("   ");
			}
			for (i = 0; i < 0x2F; i++) {
				vt100_move(ptr_player->x, ptr_player->y);
				serial_puts("alo");
			}
			ptr_player->life--;
			return 1;
		}
	}
	return 0;
}

uint8_t ask_enemy_dead(uint8_t colone, uint8_t ligne) {
	if (ligne == 1 && ptr_enemy->l1[colone][2] == 0) {
		return 1;
	} else if (ligne == 2 && ptr_enemy->l2[colone][2] == 0) {
		return 1;
	} else {
		return 0;
	}
}

uint8_t check_pos(uint8_t ligne, uint8_t direction) {
	uint8_t colone;
	if (direction == 0) {
		for (colone = 14; colone > 0; colone--) {
			if (ask_enemy_dead(colone, ligne) == 0) {
				return colone;
			}
		}
	}
	if (direction == 1) {
		for (colone = 0; colone != 15; colone++) {
			if (ask_enemy_dead(colone, ligne) == 0) {
				return colone;
			}
		}
	}
	return 0;
}

void move_enemy(void) {
	uint8_t colone;
	for (colone = 0; colone != 15; colone++) {
		erase_enemy(colone, 1);
		erase_enemy(colone, 2);
		if (compteur == 0) {
			ptr_enemy->l1[colone][0]++;
			ptr_enemy->l2[colone][0]++;
		} else if (compteur == 1) {
			ptr_enemy->l1[colone][0]--;
			ptr_enemy->l2[colone][0]--;
		}
		display_enemy(colone, 1, ptr_enemy->l1[colone][0],
				ptr_enemy->l1[colone][1]);
		display_enemy(colone, 2, ptr_enemy->l2[colone][0],
				ptr_enemy->l2[colone][1]);
	}
	if (ptr_enemy->l1[check_pos(1, 0)][0] == 77
			|| ptr_enemy->l2[check_pos(2, 0)][0] == 77) {
		compteur++;
	} else if (ptr_enemy->l1[check_pos(1, 1)][0] == 2
			|| ptr_enemy->l2[check_pos(2, 1)][0] == 2) {
		compteur--;
	}
}

void move_player(signed char sens) {
	if (sens == 'q' && ptr_player->x > 2) {
		vt100_move(ptr_player->x, ptr_player->y);
		serial_puts("   ");
		ptr_player->x--;
		vt100_move(ptr_player->x, ptr_player->y);
		serial_puts("alo");
	}
	else if (sens == 'd' && ptr_player->x < 77) {
		vt100_move(ptr_player->x, ptr_player->y);
		serial_puts("   ");
		ptr_player->x++;
		vt100_move(ptr_player->x, ptr_player->y);
		serial_puts("alo");
	}
}
void restart(void) {
	vt100_move(33, 15);
	serial_puts("Press space for restart");
	game_launch();
	init_player();
	init_enemy();

}
void timer(void) {
	signed char touch;
	uint32_t i;
	for (i = 0; i < 0xFFF30; i++) {
		touch = serial_get_last_char();
		move_player(touch);
		if (touch == ' ') {
			shoot();
			return;
		}
	}
	if (ptr_player->life == 3) {
		vt100_move(2, 2);
		serial_puts("Life : 3");
	} else if (ptr_player->life == 2) {
		vt100_move(2, 2);
		serial_puts("Life : 2");
	} else if (ptr_player->life == 1) {
		vt100_move(2, 2);
		serial_puts("Life : 1");
	} else if (ptr_player->life == 0) {
		vt100_clear_screen();
		vt100_move(33, 10);
		serial_puts("GAME OVER");
		restart();
	}
	if (win == 30) {
		vt100_clear_screen();
		vt100_move(33, 10);
		serial_puts("YOU WIN !");
		restart();
	}
	move_enemy();
}
