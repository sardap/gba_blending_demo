#include <tonc.h>
#include <tonc_memdef.h>

#include "debug.h"

#include "assets/whale.h"
#include "assets/spriteShared.h"
#include "assets/bkg_shared.h"
#include "assets/background_light_32.h"
#include "assets/background_night_32.h"
#include "assets/car.h"

#define SB_SIZE sizeof(SCREENBLOCK)
#define BLD_MODE_ACTIVE 1

OBJ_ATTR _obj_buffer[128];

OBJ_ATTR *_player = &_obj_buffer[0];

FIXED _player_x;
FIXED _player_y;

FIXED _speed = (FIXED)(2.5f*FIX_SCALEF);

int facing = 0;

int night_sky_sb = 26;
int light_sb = 28;
int car_sb = 30;
int shared_cb = 0;

void game_loop() {
	FIXED bg_pos_x = 0;

	while(1)
	{
		vid_vsync();
		key_poll();

		REG_BG0HOFS = fx2int(bg_pos_x);
		REG_BG2HOFS = fx2int(bg_pos_x);

		if(facing) {
			if(key_hit(KEY_RIGHT)) {
				_player->attr1 ^= ATTR1_HFLIP;
				facing = 0;
			}
		} else {
			if(key_hit(KEY_LEFT)) {
				_player->attr1 ^= ATTR1_HFLIP;
				facing = 1;
			}
		}

		if(key_held(KEY_RIGHT)) {
			_player_x += _speed;
		}
		
		if(key_held(KEY_LEFT)) {
			_player_x -= _speed;
		}
		
		if(key_held(KEY_UP)) {
			_player_y -= _speed;
		}
		
		if(key_held(KEY_DOWN)) {
			_player_y += _speed;
		}

		obj_set_pos(_player, fx2int(_player_x), fx2int(_player_y));

		oam_copy(oam_mem, _obj_buffer, 1);

		bg_pos_x += float2fx(0.5f);
	}
}

void init() {
	dma3_cpy(pal_bg_mem, bkg_sharedPal, bkg_sharedPalLen);
	dma3_cpy(&tile_mem[shared_cb], bkg_sharedTiles, bkg_sharedTilesLen);

	dma3_cpy(&se_mem[night_sky_sb], background_night_32Map, background_night_32MapLen);
	dma3_cpy(&se_mem[light_sb], background_light_32Map, background_light_32MapLen);
	dma3_cpy(&se_mem[car_sb], carMap, carMapLen);

	dma3_cpy(&tile_mem[4][0], whaleTiles, whaleTilesLen);
	dma3_cpy(pal_obj_mem, spriteSharedPal, spriteSharedPalLen);

	obj_set_attr(_player,
		ATTR0_SQUARE | ATTR0_8BPP,
		ATTR1_SIZE_32,
		ATTR2_PALBANK(0) | ATTR2_PRIO(1) | 0
	);

	REG_BG0HOFS = 0;
	REG_BG0VOFS = 0;

	// Set bkg reg
	REG_BG0CNT = BG_PRIO(0) | BG_8BPP | BG_SBB(light_sb) 	 | BG_CBB(shared_cb) | BG_AFF_32x32;
	REG_BG1CNT = BG_PRIO(2) | BG_8BPP | BG_SBB(night_sky_sb) | BG_CBB(shared_cb);
	REG_BG2CNT = BG_PRIO(1) | BG_8BPP | BG_SBB(car_sb) 		 | BG_CBB(shared_cb) | BG_AFF_32x32;

	REG_DISPCNT =  DCNT_OBJ | DCNT_BG0 | DCNT_BG1 | DCNT_BG2 | DCNT_OBJ_1D;

	REG_BLDCNT = BLD_BUILD(
		BLD_BG0,  				// Top layers
        BLD_BG1 | BLD_BG2 | BLD_OBJ,      // Bottom layers
        BLD_MODE_ACTIVE			// Mode
	);

	// Update blend weights
	//Left EVA: Top weight max of 15 (4 bits)
	//Right EVB: Bottom wieght max of 15 (4 bits)
	REG_BLDALPHA = BLDA_BUILD(0, 5);
	REG_BLDY = BLDY_BUILD(0);

	_player_x = int2fx(SCREEN_WIDTH / 2 - 32 /2);
	_player_y = int2fx(SCREEN_HEIGHT / 2 - 32 / 2);
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