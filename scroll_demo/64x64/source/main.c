#include <tonc.h>
#include <tonc_memdef.h>

#include "debug.h"

#include "assets/bkg_shared.h"
#include "assets/spriteShared.h"
#include "assets/background_64x32.h"
#include "assets/paul.h"

#define SHARED_CB 0

#define BACKGROUND_SSB 26

#define BLD_MODE_ACTIVE 1

OBJ_ATTR _obj_buffer[128];

OBJ_ATTR *_player = &_obj_buffer[0];

FIXED _player_x;
FIXED _player_y;

FIXED _speed = (FIXED)(2.0f*FIX_SCALEF);

bool facing_left = false;

void game_loop() {
	FIXED bg_pos_x = int2fx(0);
	FIXED bg_pos_Y = int2fx(0);
	int count = 0;

	while(1)
	{
		vid_vsync();
		key_poll();
		count++;

		if(key_held(KEY_RIGHT)) {
			bg_pos_x += _speed;
		}
		
		if(key_held(KEY_LEFT)) {
			bg_pos_x -= _speed;
		}
		
		if(key_held(KEY_UP)) {
			bg_pos_Y -= _speed;
		}
		
		if(key_held(KEY_DOWN)) {
			bg_pos_Y += _speed;
		}

		REG_BG0HOFS = fx2int(bg_pos_x);
		REG_BG0VOFS = fx2int(bg_pos_Y);

		oam_copy(oam_mem, _obj_buffer, 1);

		obj_set_pos(_player, fx2int(_player_x), fx2int(_player_y));
	}
}

void init() {
	dma3_cpy(pal_bg_mem, bkg_sharedPal, bkg_sharedPalLen);
	dma3_cpy(&tile_mem[SHARED_CB], bkg_sharedTiles, bkg_sharedTilesLen);

	dma3_cpy(&se_mem[BACKGROUND_SSB], background_64x32Map, background_64x32MapLen);

	dma3_cpy(&tile_mem[4][0], paulTiles, paulTilesLen);
	dma3_cpy(pal_obj_mem, spriteSharedPal, spriteSharedPalLen);

	obj_set_attr(_player,
		ATTR0_SQUARE | ATTR0_8BPP,
		ATTR1_SIZE_64,
		ATTR2_PALBANK(0) | ATTR2_PRIO(0) | 0
	);

	REG_BG0HOFS = 0;
	REG_BG0VOFS = 0;

	// Set bkg reg
	REG_BG0CNT = BG_PRIO(2) | BG_8BPP | BG_SBB(BACKGROUND_SSB) | BG_CBB(SHARED_CB) | BG_REG_64x32;

	REG_DISPCNT =  DCNT_BG0 | DCNT_OBJ | DCNT_OBJ_1D;

	_player_x = int2fx(SCREEN_WIDTH / 2 - 32);
	_player_y = int2fx(SCREEN_HEIGHT/ 2 - 32);
}

void start_game() {
	init();
	game_loop();
}

int main()
{
	start_game();

    while(1);

    return 0;
}
