#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include "textures.h"
#include "items.h"

#define PI 3.1415926536

#define MAP_WIDTH 24
#define MAP_HEIGHT 24

int SCREEN_WIDTH, SCREEN_HEIGHT;

// this section makes sure that the game width and height matches the terminal size.
#ifdef _WIN32
#include <windows.h>
void calculateScreenSize() {
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	SCREEN_WIDTH = csbi.srWindow.Right - csbi.srWindow.Left;
	SCREEN_HEIGHT = csbi.srWindow.Bottom - csbi.srWindow.Top;
}
#else
#include <sys/ioctl.h>
#include <unistd.h>
void calculateScreenSize() {
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	SCREEN_WIDTH = w.ws_col - 1;
	SCREEN_HEIGHT = w.ws_row - 1;
}
#endif

// sleep function
void sleep_ms(int milliseconds) {
#ifdef _WIN32
	Sleep(milliseconds);
#else
	struct timespec ts;
	ts.tv_sec = milliseconds / 1000;
	ts.tv_nsec = (milliseconds % 1000) * 1000000;
	nanosleep(&ts, NULL);
#endif
}

#define START_X 2
#define START_Y 2

#define ROTSPEED PI/2
#define MOVESPEED 1

#define MAXINVENTORY 50

#define VIEW_FIRST_PERSON 0
#define VIEW_TOP_DOWN 1


struct Sprite {
	int textureId;
	double x, y;
	int visible;

	// used for rendering, do not touch
	double dist;
};

void load_map(int map[][MAP_HEIGHT], char* mapfile);
void render_map(int map[][MAP_HEIGHT], double posX, double posY, int cardinalDir);
void render_screen(int map[][MAP_HEIGHT], double posX, double posY, double dirX, double dirY, double planeX, double planeY, int fisheyeEffect);
void init_sprite(struct Sprite* sprite, int textureId, double x, double y);
int canMoveTo(int map[][MAP_HEIGHT], double x, double y);
void grapple(int map[][MAP_HEIGHT], double* posX, double* posY, double dirX, double dirY, double planeX, double planeY);
void turnAround(double* dirX, double* dirY, double* planeX, double* planeY, int* cardinalDir);
int getSpriteFacing(double posX, double posY, double dirX, double dirY);

#define numSprites 10
struct Sprite SPRITES[numSprites];
int SPRITE_VISIBLE[numSprites];

int main() {
	calculateScreenSize();

	printf("Loading textures...\n");
	init_textures();
	printf("Loaded textures.\n");

	// initialize sprite memory
	{
		int i;
		for (i = 0; i < numSprites; ++i) {
			SPRITES[i].visible = 0;
		}
	}


	init_sprite(&SPRITES[SWORD_TEXID], SWORD_TEXID, 5.5, 22.5);
	init_sprite(&SPRITES[CHEST_TEXID], CHEST_TEXID, 10.5, 6.5);
	init_sprite(&SPRITES[DOOR_TEXID], DOOR_TEXID, 15.5, 17.5);
	init_sprite(&SPRITES[SHOP_TEXID], SHOP_TEXID, 10.5, 13.5);
	init_sprite(&SPRITES[MONSTER_TEXID], MONSTER_TEXID, 8.5, 1.5);
	init_sprite(&SPRITES[WEB_TEXID], WEB_TEXID, 4.5, 9.5);
	init_sprite(&SPRITES[TORCH_TEXID], TORCH_TEXID, 2.5, 10.5);
	init_sprite(&SPRITES[GRAPPLE_TEXID], GRAPPLE_TEXID, 16.5, 14.5);

	char input[255];
	double posX, posY, dirX, dirY, planeX, planeY, oldDirX, oldPlaneX;
	int i;
	int cardinalDir;

	struct inventory playerInventory;
	playerInventory.amt = 0;

	int map[MAP_WIDTH][MAP_HEIGHT];

	struct inventory shop1 = {{"key",1,5}, 1, 0};

	printf("Loading map...\n");

	load_map(map, "map/map1.bmf");

	printf("Map loaded.\n");

	posX = START_X + .5;
	posY = START_Y + .5;

	dirX = -1;
	dirY = 0;

	planeX = 0;
	planeY = 0.66;
	
	oldDirX = dirX;
	dirX = dirX * cos(-ROTSPEED) - dirY * sin(-ROTSPEED);
	dirY = oldDirX * sin(-ROTSPEED) + dirY * cos(-ROTSPEED);
	oldPlaneX = planeX;
	planeX = planeX * cos(-ROTSPEED) - planeY * sin(-ROTSPEED);
	planeY = oldPlaneX * sin(-ROTSPEED) + planeY * cos(-ROTSPEED);

	cardinalDir--;
	if(cardinalDir < 0) {
		cardinalDir = 3;
	}



	// use a fisheye effect to add a little disorientation
	render_screen(map, posX, posY, dirX, dirY, planeX, planeY, 1);

	printf("You wake up in a dimly lit room, entirely unaware of where you are.\n");
	printf("(type 'h' for a list of commands)\n");
	scanf("%s", input);

	int viewMode = 0;
	int shouldRenderNextFrame = 1;

	cardinalDir = 2;

	while (1) {
		if (strcmp(input, "quit") == 0) {
			break;
		} else if (strcmp(input, "f") == 0) {
			// forward
			if (canMoveTo(map, posX + dirX * MOVESPEED, posY + dirY * MOVESPEED)) {
				posX += dirX * MOVESPEED;
				posY += dirY * MOVESPEED;
			}
		} else if (strcmp(input, "r") == 0) {
			// rotate right
			oldDirX = dirX;
			dirX = dirX * cos(ROTSPEED) - dirY * sin(ROTSPEED);
			dirY = oldDirX * sin(ROTSPEED) + dirY * cos(ROTSPEED);
			oldPlaneX = planeX;
			planeX = planeX * cos(ROTSPEED) - planeY * sin(ROTSPEED);
			planeY = oldPlaneX * sin(ROTSPEED) + planeY * cos(ROTSPEED);

			cardinalDir++;
			cardinalDir %= 4;
		} else if (strcmp(input, "l") == 0) {
			// rotate left
			oldDirX = dirX;
			dirX = dirX * cos(-ROTSPEED) - dirY * sin(-ROTSPEED);
			dirY = oldDirX * sin(-ROTSPEED) + dirY * cos(-ROTSPEED);
			oldPlaneX = planeX;
			planeX = planeX * cos(-ROTSPEED) - planeY * sin(-ROTSPEED);
			planeY = oldPlaneX * sin(-ROTSPEED) + planeY * cos(-ROTSPEED);

			cardinalDir--;
			if(cardinalDir < 0) {
				cardinalDir = 3;
			}
			cardinalDir %= 4;
		} else if (strcmp(input, "m") == 0) {
			// render out a map
			render_map(map, posX, posY, cardinalDir);

			shouldRenderNextFrame = 0;
		} else if (strcmp(input, "g") == 0) {
			// pan around the room
			for (i = 0; i < 36; ++i) {
				oldDirX = dirX;
				dirX = dirX * cos(-PI / 18) - dirY * sin(-PI / 18);
				dirY = oldDirX * sin(-PI / 18) + dirY * cos(-PI / 18);
				oldPlaneX = planeX;
				planeX = planeX * cos(-PI / 18) - planeY * sin(-PI / 18);
				planeY = oldPlaneX * sin(-PI / 18) + planeY * cos(-PI / 18);

				render_screen(map, posX, posY, dirX, dirY, planeX, planeY, 0);

				sleep_ms(100);
			}

		} else if (strcmp(input, "v") == 0) {
			// toggle view mode
			viewMode = !viewMode;
		} else if (strcmp(input, "h") == 0) {
			printf("\n");
			printf("=-=-=-=-=-=-=-=\n");
			printf("   COMMANDS\n");
			printf("=-=-=-=-=-=-=-=\n\n");
			printf("f: move forward\n");
			printf("l/r: turn left/right\n");
			printf("m: check map\n");
			printf("v: toggle view mode\n");
			printf("quit: exit the game\n");
			printf("\n");

			// make sure the next frame doesn't get rendered so the user can
			// see the output.
			shouldRenderNextFrame = 0;
		} else if (strcmp(input, "grapple") == 0) {
			grapple(map, &posX, &posY, dirX, dirY, planeX, planeY);
		}

		if (shouldRenderNextFrame) {
			if(viewMode == VIEW_FIRST_PERSON) {
				render_screen(map, posX, posY, dirX, dirY, planeX, planeY, 0);
			} else if (viewMode == VIEW_TOP_DOWN) {
				render_map(map, posX, posY, cardinalDir);
			}
		} else {
			shouldRenderNextFrame = 1;
		}
		scanf("%s", input);

	}
	return 0;
}

void turnAround(double* dirX, double* dirY, double* planeX, double* planeY, int* cardinalDir) {
	double oldDirX, oldPlaneX;
	oldDirX = *dirX;
	*dirX = oldDirX * cos(PI) - *dirY * sin(PI);
	*dirY = oldDirX * sin(PI) + *dirY * cos(PI);
	oldPlaneX = *planeX;
	*planeX = oldPlaneX * cos(PI) - *planeY * sin(PI);
	*planeY = oldPlaneX * sin(PI) + *planeY * cos(PI);
	
	*cardinalDir += 2;
	*cardinalDir %= 4;
}

int getSpriteFacing(double posX, double posY, double dirX, double dirY) {
	double projectX, projectY;
	int i;
	projectX = posX + dirX * MOVESPEED;
	projectY = posY + dirY * MOVESPEED;

	for (i = 0; i < numSprites; ++i) {
		double dx, dy;
		if (!SPRITES[i].visible) continue;

		dx = projectX - SPRITES[i].x;
		dy = projectY - SPRITES[i].y;

		if (dx < 0) dx = -dx;
		if (dy < 0) dy = -dy;

		if (dx < 0.001 && dy < 0.001) {
			return i;
		}
	}
	return -1;
}


void init_sprite(struct Sprite* sprite, int textureId, double x, double y) {
	sprite->textureId = textureId;
	sprite->x = x;
	sprite->y = y;
	// sprites are invisible by default
	sprite->visible = 1;
}

void load_map(int map[][MAP_HEIGHT], char* mapfile) {
	FILE* fp;
	int x, y, num_sprites, i;
	fp = fopen(mapfile, "r");

	for (y = 0; y < MAP_HEIGHT; ++y) {
		char buffer[24];
		fscanf(fp, "%s", buffer);
		for (x = 0; x < MAP_WIDTH; ++x) {
			map[x][y] = buffer[x] - '0';
		}
	}

	fscanf(fp, "%d", &num_sprites);
	for (i = 0; i < num_sprites; ++i) {
		int spr;
		fscanf(fp, "%d", &spr);
		SPRITES[spr].visible = 1;
	}

	fclose(fp);
}

void render_map(int map[][MAP_HEIGHT], double posX, double posY, int cardinalDir) {
	int i, j;
	char player;
	char buffer[MAP_HEIGHT][MAP_WIDTH+1];
	for (j = 0; j < MAP_HEIGHT; ++j) {
		for (i = 0; i < MAP_WIDTH; ++i) {
			char out;
			switch (map[i][j]) {
				case 0: out = ' '; break;
				case 1: out = '#'; break;
			}
			buffer[j][i] = out;
		}
		buffer[j][MAP_WIDTH] = 0;
	}

	switch (cardinalDir) {
		case 0: player = '^'; break;
		case 1: player = '>'; break;
		case 2: player = 'v'; break;
		case 3: player = '<'; break;
	}

	buffer[(int) posY][(int) posX] = player;

	for (i = 0; i < numSprites; ++i) {
		if (SPRITES[i].visible) {
			buffer[(int) SPRITES[i].y][(int) SPRITES[i].x] = 'o';
		}
	}

	for (j = 0; j < MAP_HEIGHT; ++j) {
		printf("%s\n", buffer[j]);
	}
}

// A function to compare two sprites by render distance that will be necessary for sprite rendering in `render_screen`
int cmp_sprite_dist(const void* a, const void* b) {
	struct Sprite *aval, *bval;
	aval = (struct Sprite *) a;
	bval = (struct Sprite *) b;
	return (bval->dist) - (aval->dist);
}


// renders out some Wolfenstein-style raycasting
// MOST of this is taken from http://lodev.org/cgtutor/
void render_screen(int map[][MAP_HEIGHT], double posX, double posY, double dirX, double dirY, double planeX, double planeY, int fisheyeEffect) {
	int x, i, j;
	char* buffer = (char*) malloc(sizeof(int) * SCREEN_WIDTH * SCREEN_HEIGHT);
	double* zBuffer = (double*) malloc(sizeof(double) * SCREEN_WIDTH);

	for (i = 0; i < SCREEN_WIDTH; ++i) {
		for (j = 0; j < SCREEN_HEIGHT; ++j) {
			buffer[i*SCREEN_HEIGHT+j] = ' ';
		}
	}

	for (x = 0; x < SCREEN_WIDTH; ++x) {
		char outColor;
		int mapX, mapY, stepX, stepY, hit, side, lineHeight, drawStart, drawEnd;
		double cameraX, rayPosX, rayPosY, rayDirX, rayDirY, sideDistX, sideDistY, deltaDistX, deltaDistY, perpWallDist;
		cameraX = 2.0 * x / SCREEN_WIDTH - 1;
		rayPosX = posX;
		rayPosY = posY;
		rayDirX = dirX + planeX * cameraX;
		rayDirY = dirY + planeY * cameraX;

		mapX = (int) rayPosX;
		mapY = (int) rayPosY;

		deltaDistX = sqrt(1 + (rayDirY  * rayDirY) / (rayDirX * rayDirX));
		deltaDistY = sqrt(1 + (rayDirX  * rayDirX) / (rayDirY * rayDirY));

		hit = 0;

		if (rayDirX < 0) {
			stepX = -1;
			sideDistX = (rayPosX - mapX) * deltaDistX;
		} else {
			stepX = 1;
			sideDistX = (mapX + 1.0 - rayPosX) * deltaDistX;
		}

		if (rayDirY < 0) {
			stepY = -1;
			sideDistY = (rayPosY - mapY) * deltaDistY;
		} else {
			stepY = 1;
			sideDistY = (mapY + 1.0 - rayPosY) * deltaDistY;
		}

		while (hit == 0) {
			if (sideDistX < sideDistY) {
				sideDistX += deltaDistY;
				mapX += stepX;
				side = 0;
			} else {
				sideDistY += deltaDistY;
				mapY += stepY;
				side = 1;
			}
			if (map[mapX][mapY] > 0) hit = 1;
		}

		if (fisheyeEffect) {
			double dx, dy;
			dx = rayPosX - mapX;
			dy = rayPosY - mapY;
			perpWallDist = sqrt(dx*dx+dy*dy);
		} else {
			if (side == 0) 	perpWallDist = (mapX - rayPosX + (1 - stepX) / 2) / rayDirX;
			else 		perpWallDist = (mapY - rayPosY + (1 - stepY) / 2) / rayDirY;
		}

		lineHeight = (int) (SCREEN_HEIGHT / perpWallDist);

		drawStart = -lineHeight / 2 + (SCREEN_HEIGHT >> 1);
		if (drawStart < 0) drawStart = 0;

		drawEnd = (lineHeight + SCREEN_HEIGHT) >> 1;
		if (drawEnd >= SCREEN_HEIGHT) drawEnd = SCREEN_HEIGHT - 1;

		switch (map[mapX][mapY]) {
			case 0: outColor = ' '; break;
			case 1:
				if (side) outColor = '#';
				else outColor = '%';
				break;
		}

		// draw line
		for (i = drawStart; i <= drawEnd; ++i) {
			if (0 <= i && i < SCREEN_HEIGHT) {
				buffer[x*SCREEN_HEIGHT+i] = outColor;
			}
		}

		zBuffer[x] = perpWallDist;
	}

	for (i = 0; i < numSprites; ++i) {
		double dx, dy;
		dx = SPRITES[i].x - posX;
		dy = SPRITES[i].y - posY;
		SPRITES[i].dist = dx*dx + dy*dy;
	}

	qsort(SPRITES, numSprites, sizeof(struct Sprite), cmp_sprite_dist);

	for (i = 0; i < numSprites; ++i) {
		double spriteX, spriteY, invDet, transformX, transformY;
		int spriteScreenX, spriteSize, drawStartY, drawEndY, drawStartX, drawEndX, stripe;

		if (!SPRITES[i].visible) {
			continue;
		}

		spriteX = SPRITES[i].x - posX;
		spriteY = SPRITES[i].y - posY;

		invDet = 1.0 / (planeX * dirY - dirX * planeY);

		transformX = invDet * (dirY * spriteX - dirX * spriteY);
		transformY = invDet * (-planeY * spriteX + planeX * spriteY);

		spriteScreenX = (int) ((SCREEN_WIDTH >> 1) * (1 + transformX / transformY));

		if (fisheyeEffect) {
			spriteSize = abs((int) (SCREEN_HEIGHT / sqrt(spriteX*spriteX + spriteY*spriteY)));
		} else {
			spriteSize = abs((int) (SCREEN_HEIGHT / transformY));
		}

		drawStartY = (SCREEN_HEIGHT >> 1) - (spriteSize >> 1);
		if (drawStartY < 0) drawStartY = 0;
		drawEndY = (SCREEN_HEIGHT + spriteSize) >> 1;
		if (drawEndY >= SCREEN_HEIGHT) drawEndY = SCREEN_HEIGHT - 1;

		drawStartX = spriteScreenX - (spriteSize >> 1);
		if (drawStartX < 0) drawStartX = 0;
		drawEndX = spriteScreenX + (spriteSize >> 1);
		if (drawEndX >= SCREEN_WIDTH) drawEndX = SCREEN_WIDTH - 1;

		for (stripe = drawStartX; stripe < drawEndX; ++stripe) {
			int texX = ((int) (256 * (stripe - drawStartX) * 16 / spriteSize)) / 256;

			if (transformY > 0 && stripe > 0 && stripe < SCREEN_WIDTH && transformY < zBuffer[stripe]) {
				int y;
				for (y = drawStartY; y < drawEndY; ++y) {
					int d, texY;
					char color;
					d = y * 256 - SCREEN_HEIGHT * 128 + spriteSize * 128;
					texY = ((d * 16) / spriteSize) / 256;
					// the 15 - part is to fix an issue with mirroring
					color = get_texture_char_at(TEXTURES[SPRITES[i].textureId], 15 - texX, texY);
					if (color != 0) buffer[stripe*SCREEN_HEIGHT+y] = color;
				}
			}
		}
	}

	free(zBuffer);


	char* str = (char*) malloc(sizeof(char) * (SCREEN_WIDTH+1));
	for (i = 0; i < SCREEN_HEIGHT; ++i) {
		// iterating backwards to fix some really weird rendering bug
		// where the viewport is backwards
		for (j = SCREEN_WIDTH - 1; j >= 0; --j) {
			str[SCREEN_WIDTH - 1 - j] = buffer[j*SCREEN_HEIGHT+i];
		}
		str[SCREEN_WIDTH] = 0;
		printf("%s\n", str);
	}

	free(str);
	free(buffer);

}

int canMoveTo(int map[][MAP_HEIGHT], double x, double y) {
	if (map[(int) x][(int) y] == 0) {
		int i;
		for (i = 0; i < numSprites; ++i) {
			if (SPRITES[i].visible) {
				// check for x-y collisions
				double dx, dy;
				dx = SPRITES[i].x - x;
				dy = SPRITES[i].y - y;
				if (dx < 0) dx = -dx;
				if (dy < 0) dy = -dy;
				if (dx < 0.001 && dy < 0.001) {
					return 0;
				}
			}
		}
		return 1;
	}
	return 0;
}

void grapple(int map[][MAP_HEIGHT], double* posX, double* posY, double dirX, double dirY, double planeX, double planeY) {
	while (canMoveTo(map, *posX + dirX * MOVESPEED, *posY + dirY * MOVESPEED)) {
		*posX += dirX * MOVESPEED;
		*posY += dirY * MOVESPEED;
		
		render_screen(map, *posX, *posY, dirX, dirY, planeX, planeY, 0);

		sleep_ms(100);

	}
}
