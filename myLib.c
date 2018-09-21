#include "myLib.h"
#include "my_images.h"
#include "text.h"

#include <stdio.h>
#include <stdlib.h>

u16* videoBuffer = (u16*)0x6000000;

void waitForVBlank() {
	while (SCANLINECOUNTER > 160);
	while (SCANLINECOUNTER < 160);
}

void setPixel(int row, int col, unsigned short color) {
	videoBuffer[row * 240 + col] = color;	// For drawing text
}

void drawFullscreenImage(int width, int height, const u16* image) {
	for (int i = 0; i < height; i++) {
		DMA[3].src = &image[i * width + 0];
		DMA[3].dst = &videoBuffer[OFFSET(i, 0, 240)];
		DMA[3].cnt = width | DMA_ON;
	}
}

void drawPiece(Vector pos, int width, int height, const u16* piece) {
	for (int i = 0; i < height; i++) {
		DMA[3].src = &piece[i * width];
		DMA[3].dst = &videoBuffer[OFFSET(i + pos.y, pos.x, 240)];
		DMA[3].cnt = width | DMA_ON;
	}
}

void drawColoredRectangle(Vector pos, int width, int height, volatile unsigned short* color) {
	for (int i = 0; i < height; i++) {
		DMA[3].src = color;
		DMA[3].dst = &videoBuffer[OFFSET(i + pos.y, pos.x, 240)];
		DMA[3].cnt = width | DMA_ON | DMA_SOURCE_FIXED;
	}
}

void movePlayer(Player* hero, int up, int down, int right, int left) {
	up = up != 0 ? 1 : 0;
	down = down != 0 ? 1 : 0;
	right = right != 0 ? 1 : 0;
	left = left != 0 ? 1 : 0;
	hero->pos.x += PLAYER_SPEED * (right - left);
	if (hero->pos.x + hero->size.x > 240) {
		hero->pos.x = 240 - hero->size.x;
	}
	if (hero->pos.x < 0) {
		hero->pos.x = 0;
	}
	hero->pos.y += PLAYER_SPEED * (down - up);
	if (hero->pos.y + hero->size.y > 160) {
		hero->pos.y = 160 - hero->size.y;
	}
	if (hero->pos.y < 0) {
		hero->pos.y = 0;
	}
}

int moveBulldog(Bulldog* curr, Player* hero, int speed) {
	curr->pos.x += curr->direction.x * speed;
	curr->pos.y += curr->direction.y * speed;
	if (curr->pos.x + curr->size.x > 240) {
		curr->pos.x = 240 - curr->size.x;
		curr->direction.x *= -1;
	} else if (curr->pos.x < 0) {
		curr->pos.x = 0;
		curr->direction.x *= -1;
	}
	if (curr->pos.y + curr->size.y > 160) {
		curr->pos.y = 160 - curr->size.y;
		curr->direction.y *= -1;
	} else if (curr->pos.y < 0) {
		curr->pos.y = 0;
		curr->direction.y *= -1;
	}
	return detectCollisions(hero->pos, hero->size, curr->pos, curr->size);
}

int moveJacket(Jacket* curr, Player* hero, int speed) {
	curr->pos.x += curr->direction.x * speed;
	curr->pos.y += curr->direction.y * speed;
	if (curr->pos.x + curr->size.x > 240) {
		curr->pos.x = 240 - curr->size.x;
		curr->direction.x *= -1;
	} else if (curr->pos.x < 0) {
		curr->pos.x = 0;
		curr->direction.x *= -1;
	}
	if (curr->pos.y + curr->size.y > 160) {
		curr->pos.y = 160 - curr->size.y;
		curr->direction.y *= -1;
	} else if (curr->pos.y < 0) {
		curr->pos.y = 0;
		curr->direction.y *= -1;
	}
	return detectCollisions(hero->pos, hero->size, curr->pos, curr->size);
}

int detectCollisions(Vector hero_pos, Vector hero_size, Vector piece_pos, Vector piece_size) {
	int did_collide = 0;
	// Since our player and piece size are both 25x25, we can just detect collisions on corners
	// Top left corner
	if (hero_pos.x >= piece_pos.x) {
		if (hero_pos.x <= piece_pos.x + piece_size.x) {
			if (hero_pos.y >= piece_pos.y) {
				if (hero_pos.y <= piece_pos.y + piece_size.y) {
					did_collide = 1;
				}
			}
		}
	}
	// Top right corner
	hero_pos.x += hero_size.x;
	if (hero_pos.x >= piece_pos.x) {
		if (hero_pos.x <= piece_pos.x + piece_size.x) {
			if (hero_pos.y >= piece_pos.y) {
				if (hero_pos.y <= piece_pos.y + piece_size.y) {
					did_collide = 1;
				}
			}
		}
	}
	// Bottom right corner
	hero_pos.y += hero_size.y;
	if (hero_pos.x >= piece_pos.x) {
		if (hero_pos.x <= piece_pos.x + piece_size.x) {
			if (hero_pos.y >= piece_pos.y) {
				if (hero_pos.y <= piece_pos.y + piece_size.y) {
					did_collide = 1;
				}
			}
		}
	}
	// Bottom left corner
	hero_pos.x -= hero_size.x;
	if (hero_pos.x >= piece_pos.x) {
		if (hero_pos.x <= piece_pos.x + piece_size.x) {
			if (hero_pos.y >= piece_pos.y) {
				if (hero_pos.y <= piece_pos.y + piece_size.y) {
					did_collide = 1;
				}
			}
		}
	}

	return did_collide;
}

void drawPlayerStats(Player* hero) {
	// Draw HP
	for (int i = 0; i < PLAYER_HP; i++) {
		if (i < hero->hp) {
			volatile unsigned short red = RED;
			drawColoredRectangle((Vector) { 20 + (i * 12), 140 }, 10, 7, &red);
		} else {
			volatile unsigned short gray = GRAY;
			drawColoredRectangle((Vector) { 20 + (i * 12), 140 }, 10, 7, &gray);
		}
	}
}

void drawScore(int score) {
	char buffer[51];
	sprintf(buffer, "Final score: %d", score);
	drawString(5, 5, buffer, GREEN);
}

void initializePieces(Player* hero, int new_hp, int new_score, int new_level, int* current_level,
    int new_jacket_count, int new_bulldog_count, int* jacket_count, int* bulldog_count,
    Jacket jackets[], Bulldog bulldogs[]) {
	int possible_directions[] = { -1, 1 };
	*current_level = new_level;
	*jacket_count = new_jacket_count;
	*bulldog_count = new_bulldog_count;
	hero->hp = new_hp;
	hero->score = new_score;
	hero->pos = (Vector) { 0, 0 };
	for (int i = 0; i < *jacket_count; i++) {
		// Initialize jackets for next level
		jackets[i] = (Jacket) {
			(Vector) { rand() % (240 - YELLOW_JACKET_WIDTH), rand() % (160 - YELLOW_JACKET_WIDTH) },
			(Vector) { YELLOW_JACKET_WIDTH, YELLOW_JACKET_HEIGHT },
			(Vector) { possible_directions[rand() % 2],
						possible_directions[rand() % 2] },
			rand() % 10
		};
	}
	for (int i = 0; i < *bulldog_count; i++) {
		// Initialize bulldogs for next level
		bulldogs[i] = (Bulldog) {
			(Vector) { rand() % (240 - BULLDOG_WIDTH) + i, rand() % (160 - BULLDOG_WIDTH) + i },
			(Vector) { BULLDOG_WIDTH, BULLDOG_HEIGHT },
			(Vector) { possible_directions[rand() % 2],
						possible_directions[rand() % 2] },
			rand() % 5
		};
	}

	UNUSED(jacket_count);
	UNUSED(bulldog_count);
	UNUSED(jackets);
	UNUSED(bulldogs);
	UNUSED(possible_directions);
}
