// inclusion guards
#ifndef _TEXTURES_H_
#define _TEXTURES_H_

#define SWORD_TEXID 0
#define EVIL_TEXID 1
#define DOOR_TEXID 2
#define SHOP_TEXID 3
#define MONSTER_TEXID 4
#define WEB_TEXID 5
#define TORCH_TEXID 6
#define GRAPPLE_TEXID 7

struct Texture {
	int vals[16][16];
};

struct Texture TEXTURES[8];

void init_textures();

struct Texture load_texture_from_file(char* filename);
char get_texture_char_at(struct Texture texture, int x, int y);

#endif
