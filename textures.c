#include <stdio.h>
#include "textures.h"

void init_textures() {
	TEXTURES[0] = load_texture_from_file("tex/sword.bmf");
	TEXTURES[1] = load_texture_from_file("tex/evil.bmf");
	TEXTURES[2] = load_texture_from_file("tex/door.bmf");
	TEXTURES[3] = load_texture_from_file("tex/shop.bmf");
	TEXTURES[4] = load_texture_from_file("tex/monster.bmf");
}

struct Texture load_texture_from_file(char* filename) {
	FILE* fp;
	int x, y;
	struct Texture tex;

	fp = fopen(filename, "r");

	for (y = 0; y < 16; ++y) {
		char buffer[16];
		fscanf(fp, "%s", buffer);
		for (x = 0; x < 16; ++x) {
			if (48 <= buffer[x] && buffer[x] <= 58) {
				tex.vals[x][y] = buffer[x] - '0';
			} else {
				tex.vals[x][y] = 0;
			}
		}
	}

	return tex;
}

char get_texture_char_at(struct Texture texture, int x, int y) {
	switch (texture.vals[x][y]) {
		case 1: return '@';
		case 2: return ' ';
	}
	return 0;
}
