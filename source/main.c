#include <tonc.h>
#include <tonc_memdef.h>

#include "debug.h"

#include "assets/whale.h"
#include "assets/spriteShared.h"
#include "assets/bkg_shared.h"
#include "assets/background_light_32.h"
#include "assets/background_night_32.h"
#include "assets/car.h"

#define SHARED_CB 0

#define NIGHT_SKY_SBB 26
#define LIGHT_SBB 28
#define CAR_SBB 30

#define BLD_MODE_ACTIVE 1

OBJ_ATTR _obj_buffer[128];

OBJ_ATTR *_player = &_obj_buffer[0];

FIXED _player_x;
FIXED _player_y;

FIXED _speed = (FIXED)(2.5f*FIX_SCALEF);

bool facing_left = false;

void game_loop() {
	FIXED bg_pos_x = int2fx(400);
	FIXED eva_weight = int2fx(3);
	int count = 0;

	while(1)
	{
		vid_vsync();
		key_poll();
		count++;

		REG_BG0HOFS = fx2int(bg_pos_x);
		REG_BG2HOFS = fx2int(bg_pos_x);

		if(facing_left) {
			if(key_hit(KEY_RIGHT)) {
				_player->attr1 ^= ATTR1_HFLIP;
				facing_left = false;
			}
		} else {
			if(key_hit(KEY_LEFT)) {
				_player->attr1 ^= ATTR1_HFLIP;
				facing_left = true;
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
		// if(count % 60 == 0) {
		// 	eva_weight += int2fx(1);
		// 	if(fx2int(eva_weight) > 15) {
		// 		eva_weight = 0;
		// 	}
		// }

		// REG_BLDALPHA = BLDA_BUILD(fx2int(eva_weight), 5);
	}
}

void init() {
	dma3_cpy(pal_bg_mem, bkg_sharedPal, bkg_sharedPalLen);
	dma3_cpy(&tile_mem[SHARED_CB], bkg_sharedTiles, bkg_sharedTilesLen);

	dma3_cpy(&se_mem[NIGHT_SKY_SBB], background_night_32Map, background_night_32MapLen);
	dma3_cpy(&se_mem[LIGHT_SBB], background_light_32Map, background_light_32MapLen);
	dma3_cpy(&se_mem[CAR_SBB], carMap, carMapLen);

	dma3_cpy(&tile_mem[4][0], whaleTiles, whaleTilesLen);
	dma3_cpy(pal_obj_mem, spriteSharedPal, spriteSharedPalLen);

	obj_set_attr(_player,
		ATTR0_SQUARE | ATTR0_8BPP,
		ATTR1_SIZE_32,
		ATTR2_PALBANK(0) | ATTR2_PRIO(2) | 0
	);

	REG_BG0HOFS = 0;
	REG_BG0VOFS = 0;

	// Set bkg reg
	REG_BG0CNT = BG_PRIO(0) | BG_8BPP | BG_SBB(LIGHT_SBB) 	 	| BG_CBB(SHARED_CB) | BG_AFF_32x32;
	REG_BG1CNT = BG_PRIO(3) | BG_8BPP | BG_SBB(NIGHT_SKY_SBB) 	| BG_CBB(SHARED_CB);
	REG_BG2CNT = BG_PRIO(1) | BG_8BPP | BG_SBB(CAR_SBB) 	 	| BG_CBB(SHARED_CB) | BG_AFF_32x32;

	REG_DISPCNT =  DCNT_BG0 | DCNT_BG1 | DCNT_BG2 | DCNT_OBJ | DCNT_OBJ_1D;

	//Set Blend reg
	REG_BLDCNT = BLD_BUILD(
		BLD_BG0,  						// Top layers
        BLD_BG1 | BLD_BG2 | BLD_OBJ, 	// Bottom layers
        BLD_MODE_ACTIVE					// Mode
	);

	// Update blend weights
	//Left EVA: Top weight max of 15 (4 bits)
	//Right EVB: Bottom wieght max of 15 (4 bits)
	// REG_BLDALPHA = BLDA_BUILD(3, 5);
	REG_BLDALPHA = BLDA_BUILD(3, 5);
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
