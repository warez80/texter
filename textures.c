#include <stdio.h>
#include "textures.h"

void init_textures() {
	TEXTURES[0] = load_texture_from_file("tex/sword.bmf");
}

struct Texture load_texture_from_file(char* filename) {
	FILE* fp;
	int x, y;
	struct Texture tex;

	fp = fopen(filename, "r");

	for (y = 0; y < 16; ++y) {
		tex.bits[y] = 0;
		char buffer[16];
		fscanf(fp, "%s", buffer);
		for (x = 0; x < 16; ++x) {
			if (buffer[x] == '1') {
				tex.bits[y] |= (1 << x);
			}
		}
	}

	return tex;
}

char get_texture_char_at(struct Texture texture, int x, int y) {
	if (texture.bits[y] & (1 << x)) {
		return '@';
	} else {
		return ' ';
	}
}
