#ifndef GAME_H_
#define GAME_H_

void init_enemy(void);
void init_player(void);
void display_enemy(uint8_t, uint8_t, uint8_t, uint8_t);
void erase_enemy(uint8_t, uint8_t);
void shoot_enemy(void);
void shoot(void);
uint8_t ask_enemy_dead(uint8_t, uint8_t);
uint8_t check_pos(uint8_t, uint8_t);
uint8_t hitbox(uint8_t, uint8_t);
void move_enemy(void);
void move_player(signed char);
void timer(void);

#endif /* GAME_H_ */
