// inclusion guards
#ifndef _TEXTURES_H_
#define _TEXTURES_H_

// Textures are 16x16 bit maps
struct Texture {
	int bits[16];
};

struct Texture TEXTURES[1];

void init_textures();

struct Texture load_texture_from_file(char* filename);
char get_texture_char_at(struct Texture texture, int x, int y);

#endif
