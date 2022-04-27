/*
 * init_periph.h
 *
 *  Created on: Apr 17, 2022
 *      Author: lam120
 */

#ifndef INIT_PERIPH_H_
#define INIT_PERIPH_H_

#define SOUND_MULTIPLIER 5
void init_audio();
void init_buttons();
void setup_score(int);
void loop();
void update_score(int);
void end_game();
void small_delay(int);
int check_endgame();
void setup();
void reset();
void endscreen();
void timer16_init();
void reset_screen();

#endif /* INIT_PERIPH_H_ */
