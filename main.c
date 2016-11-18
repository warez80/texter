#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>

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

#define START_X 18
#define START_Y 5

#define ROTSPEED 1.57079632679
#define MOVESPEED 1

#define MAXINVENTORY 50

struct item {
    char name[25];
    int quantity;
};

struct inventory {
    struct item items[MAXINVENTORY];
    int size;
};

struct room {
    char desc[200];
    int map[MAP_WIDTH][MAP_HEIGHT];
};

void fill_map(int map[][MAP_HEIGHT]);
void render_screen(int map[][MAP_HEIGHT], double posX, double posY, double dirX, double dirY, double planeX, double planeY);
int hasItem(struct inventory, char itemName[]);
void addItem(struct inventory, char itemName[], int quantity);

int main() {
	calculateScreenSize();
	
	render_screen(map, posX, posY, dirX, dirY, planeX, planeY);
	
	printf("You wake up in a dimly lit room, entirely unaware of where you are.\n);
	printf("(type h to for a list of commands)\n");
	scanf("%s", input);

	char input[255];
	double posX, posY, dirX, dirY, planeX, planeY, oldDirX, oldPlaneX;
	int waste;
	int i, j;
	char out;

	struct inventory playerInventory;
	playerInventory.size = 0;

	int map[MAP_WIDTH][MAP_HEIGHT];

	fill_map(map);

	posX = START_X;
	posY = START_Y;

	dirX = -1;
	dirY = 0;

	planeX = 0;
	planeY = 0.66;

	while (1) {
		
		render_screen(map, posX, posY, dirX, dirY, planeX, planeY);

		printf("w-what now senpai?\n");
		scanf("%s", input);

		if (strcmp(input, "quit") == 0) {
			break;
		} else if (strcmp(input, "f") == 0) {
			if (map[(int) (posX + dirX * MOVESPEED)][(int) posY] == 0) posX += dirX * MOVESPEED;
			if (map[(int) posX][(int) (posY + dirY * MOVESPEED)] == 0) posY += dirY * MOVESPEED;
		} else if (strcmp(input, "r") == 0) {
			// rotate right
			oldDirX = dirX;
			dirX = dirX * cos(-ROTSPEED) - dirY * sin(-ROTSPEED);
			dirY = oldDirX * sin(-ROTSPEED) + dirY * cos(-ROTSPEED);
			oldPlaneX = planeX;
			planeX = planeX * cos(-ROTSPEED) - planeY * sin(-ROTSPEED);
			planeY = oldPlaneX * sin(-ROTSPEED) + planeY * cos(-ROTSPEED);
		} else if (strcmp(input, "l") == 0) {
			oldDirX = dirX;
			dirX = dirX * cos(ROTSPEED) - dirY * sin(ROTSPEED);
			dirY = oldDirX * sin(ROTSPEED) + dirY * cos(ROTSPEED);
			oldPlaneX = planeX;
			planeX = planeX * cos(ROTSPEED) - planeY * sin(ROTSPEED);
			planeY = oldPlaneX * sin(ROTSPEED) + planeY * cos(ROTSPEED);
		} else if (strcmp(input, "m") == 0) {
			char dummy[255];
			// render out a map
			for (j = 0; j < MAP_HEIGHT; ++j) {
				for (i = 0; i < MAP_WIDTH; ++i) {
					switch (map[i][j]) {
						case 0: out = ' '; break;
						case 1: out = '#'; break;
					}
					if (((int) posX) == i && ((int) posY) == j) {
						out = 'P';
					}
					printf("%c", out);
				}
				printf("\n");
			}
	            	printf("return to first person? ");
			scanf("%s", dummy);
		} else if (strcmp(input, "g") == 0) {
			for (i = 0; i < 36; ++i) {
				oldDirX = dirX;
				dirX = dirX * cos(-PI / 18) - dirY * sin(-PI / 18);
				dirY = oldDirX * sin(-PI / 18) + dirY * cos(-PI / 18);
				oldPlaneX = planeX;
				planeX = planeX * cos(-PI / 18) - planeY * sin(-PI / 18);
				planeY = oldPlaneX * sin(-PI / 18) + planeY * cos(-PI / 18);

				render_screen(map, posX, posY, dirX, dirY, planeX, planeY);

				sleep_ms(100);
			}

		}

	}
	return 0;
}

void fill_map(int map[][MAP_HEIGHT]) {
	int x, y;

	// zero out the map
	for (x = 0; x < MAP_WIDTH; ++x) {
		for (y = 0; y < MAP_HEIGHT; ++y) {
			map[x][y] = 0;
		}
	}

	// create map boundaries
	for (x = 0; x < MAP_WIDTH; ++x) {
		map[x][0] = 1;
		map[x][MAP_HEIGHT - 1] = 1;
	}
	for (y = 0; y < MAP_HEIGHT; ++y) {
		map[0][y] = 1;
		map[MAP_WIDTH - 1][y] = 1;
	}

	// TODO: generate the actual map here

	for (x = 0; x < MAP_HEIGHT; ++x) {
		if (x < MAP_WIDTH && x % 3 == 0) {
			map[x][x] = 1;
		}
	}
}


void render_screen(int map[][MAP_HEIGHT], double posX, double posY, double dirX, double dirY, double planeX, double planeY) {
	int x, i, j;
	char buffer[SCREEN_WIDTH][SCREEN_HEIGHT];

	for (i = 0; i < SCREEN_WIDTH; ++i) {
		for (j = 0; j < SCREEN_HEIGHT; ++j) {
			buffer[i][j] = ' ';
		}
	}

	for (x = 0; x < SCREEN_WIDTH; ++x) {
		char outColor;
		int mapX, mapY, stepX, stepY, hit, side, lineHeight, drawStart, drawEnd;
		double cameraX, cameraY, rayPosX, rayPosY, rayDirX, rayDirY, sideDistX, sideDistY, deltaDistX, deltaDistY, perpWallDist;
		cameraX = 2.0 * x / SCREEN_WIDTH - 1;
		rayPosX = posX;
		rayPosY = posY;
		rayDirX = dirX + planeX * cameraX;
		rayDirY = dirY + planeY * cameraY;

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

		if (side == 0) 	perpWallDist = (mapX - rayPosX + (1 - stepX) / 2) / rayDirX;
		else 		perpWallDist = (mapY - rayPosY + (1 - stepY) / 2) / rayDirY;

		lineHeight = (int) (SCREEN_HEIGHT / perpWallDist);

		drawStart = -lineHeight / 2 + SCREEN_HEIGHT / 2;
		if (drawStart < 0) drawStart = 0;

		drawEnd = lineHeight / 2 + SCREEN_HEIGHT / 2;
		if (drawEnd >= SCREEN_HEIGHT) drawEnd = SCREEN_HEIGHT - 1;

		switch (map[mapX][mapY]) {
			case 0: outColor = ' '; break;
			case 1:
				if (side) outColor = '%';
				else outColor = '#';
				break;
		}

		// draw line
		for (i = drawStart; i <= drawEnd; ++i) {
			if (0 <= i && i < SCREEN_HEIGHT) {
				buffer[x][i] = outColor;
			}
		}
	}

	for (i = 0; i < SCREEN_HEIGHT; ++i) {
		for (j = 0; j < SCREEN_WIDTH; ++j) {
			printf("%c", buffer[j][i]);
		}
		printf("\n");
	}

}

int hasItem(struct inventory playerInventory, char itemName[])
{
    int i, size = playerInventory.size;
    for(i = 0; i < size; i++)
    {
        if(strcmp(playerInventory.items[i].name, itemName) == 0)
        {
            return playerInventory.items[i].quantity;
        }
    }

    return playerInventory.items[i].quantity;
}

void addItem(struct inventory playerInventory, char itemName[], int quantity)
{
    int i, size = playerInventory.size;
    for(i = 0; i < size; i++)
    {
        if(strcmp(playerInventory.items[i].name, itemName) == 0)
        {
            playerInventory.items[i].quantity += quantity;
            return;
        }
    }
    strcpy(playerInventory.items[size].name, itemName);
    playerInventory.items[size].quantity = quantity;
    playerInventory.size += 1;
}
