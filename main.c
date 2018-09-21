#include "myLib.h"

#include "my_images.h"

int main() {
	REG_DISPCNT = MODE_3 | BG2_ENABLE;

	GameState my_state = START; // Set first state to START and initialize

	int a_down = TRUE;
	int current_level = 0;
	int frame_counter = 0;
	int last_hit = 0;
	int available_jacket_count[NUM_LEVELS] = { NUM_JACKETS_1, NUM_JACKETS_2, NUM_JACKETS_3 };
	int available_bulldog_count[NUM_LEVELS] = { NUM_BULLDOGS_1, NUM_BULLDOGS_2, NUM_BULLDOGS_3 };
	int bulldog_count = available_bulldog_count[current_level];
	int jacket_count = available_jacket_count[current_level];
	volatile unsigned short gray = GRAY;
	const u16* levels[] = { level1, level2, level3 };


	Player hero;
	hero.size = (Vector) { PLAYER_WIDTH, PLAYER_HEIGHT };
	Bulldog bulldogs[MAX_BULLDOGS]; // Make it max so we our pointers remain same :)
	Jacket jackets[MAX_JACKETS];


	while(1) {
		waitForVBlank();

		if (KEY_DOWN_NOW(BUTTON_SELECT)) {
			my_state = START; // If at any time they press Backspace, go back to beginning
		}

		switch(my_state) {
			case START:
				// Reset all stats and then draw intro stuff
				initializePieces(&hero, PLAYER_HP, 0, 0, &current_level, available_jacket_count[0],
				available_bulldog_count[0], &jacket_count, &bulldog_count, 
					jackets, bulldogs);
				drawFullscreenImage(INTRO_WIDTH, INTRO_HEIGHT, intro);
				my_state = INTRO;
			break;

			case INTRO:
				if (KEY_DOWN_NOW(BUTTON_A) && !a_down) {
					// Starts game
					drawFullscreenImage(LEVEL1_WIDTH, LEVEL1_WIDTH, level1);
					my_state = LEVEL_CHANGE;
				}
				if (KEY_DOWN_NOW(BUTTON_B)) {
					// Goes to instructions
					drawFullscreenImage(INSTRUCTIONS_WIDTH, INSTRUCTIONS_HEIGHT, instructions);
					my_state = INSTRUCTIONS;
				}
				
				if (!KEY_DOWN_NOW(BUTTON_A) && a_down) {
					a_down = FALSE;
				}
			break;

			case INSTRUCTIONS:
				if (KEY_DOWN_NOW(BUTTON_A)) {
					a_down = TRUE;
					drawFullscreenImage(INTRO_WIDTH, INTRO_HEIGHT, intro);
					my_state = INTRO;
				}
			break;

			case LEVEL_CHANGE:
				if (KEY_DOWN_NOW(BUTTON_B)) {
					jacket_count = available_jacket_count[current_level];
					bulldog_count = available_bulldog_count[current_level];
					initializePieces(&hero, hero.hp, hero.score, current_level, &current_level,
						available_jacket_count[current_level], available_bulldog_count[current_level],
						&jacket_count, &bulldog_count, jackets, bulldogs);
					drawColoredRectangle((Vector) { 0, 0 }, 240, 160, &gray);
					my_state = PLAY;
				}
			break;

			case PLAY: // This is where the fun begins
				if (hero.hp <= 0) { 
					// We lost because go dwags :[
					drawFullscreenImage(DEFEAT_WIDTH, DEFEAT_HEIGHT, defeat);
					my_state = DEFEAT;
				} else if (jacket_count <= 0) {	
					// We won because GO JACKETS :]
					current_level++;
					if (current_level >= NUM_LEVELS) {
						drawFullscreenImage(VICTORY_WIDTH, VICTORY_HEIGHT, victory);
						my_state = VICTORY;
					} else {
						drawFullscreenImage(LEVEL1_WIDTH, LEVEL1_HEIGHT, levels[current_level]);
						my_state = LEVEL_CHANGE;
					}
				} else {
					// Draw over old pieces
					drawColoredRectangle(hero.pos, hero.size.x, hero.size.y, &gray);
					for (int i = 0; i < bulldog_count; i++) {
						drawColoredRectangle(bulldogs[i].pos, bulldogs[i].size.x,
						 bulldogs[i].size.y, &gray);
					}
					for (int i = 0; i < jacket_count; i++) {
						drawColoredRectangle(jackets[i].pos, jackets[i].size.x,
						 jackets[i].size.y, &gray);
					}

					// Move our stuff
					movePlayer(&hero, KEY_DOWN_NOW(BUTTON_UP), KEY_DOWN_NOW(BUTTON_DOWN),
						KEY_DOWN_NOW(BUTTON_RIGHT), KEY_DOWN_NOW(BUTTON_LEFT));
					if (frame_counter % 3 == 0) { // Move every 3 frames so it's not too fast
						for (int i = 0; i < bulldog_count; i++) {
							if (moveBulldog(&bulldogs[i], &hero, BULLDOG_SPEED)) {
								// We collided, remove some player hp
								if (last_hit >= 60) {
									hero.hp -= bulldogs[i].strength;
									last_hit = 0;
								}
							}
						}
						for (int i = 0; i < jacket_count; i++) {
							if (moveJacket(&jackets[i], &hero, JACKET_SPEED)) {
								// We collided, give player some points and take away jacket
								hero.score += jackets[i].reward;
								for (int j = i; j < jacket_count; j++) {
									jackets[j] = jackets[j + 1];
								}
								jacket_count--;
							}
						}
					}


					// Draw our new pieces and stats
					drawPlayerStats(&hero);
					for (int i = 0; i < bulldog_count; i++) {
						drawPiece(bulldogs[i].pos, bulldogs[i].size.x,
						bulldogs[i].size.y, bulldog);
					}
					for (int i = 0; i < jacket_count; i++) {
						drawPiece(jackets[i].pos, jackets[i].size.x,
						jackets[i].size.y, yellow_jacket);
					}
					drawPiece(hero.pos, hero.size.x, hero.size.y, player);
				}
			break;

			case VICTORY:
				// Probably do nothing
				drawScore(hero.score);
			break;

			case DEFEAT:
				// Probably do nothing
				drawScore(hero.score);
			break;

			default:
				my_state = START;
			break;
		}
		last_hit++;
		frame_counter++;
	}

	return 0;
}
