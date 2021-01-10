#include <tonc.h>
#include <tonc_memdef.h>

#include "debug.h"

#include "assets/bkg_shared.h"
#include "assets/spriteShared.h"
#include "assets/background_2048x32.h"
#include "assets/paul.h"

#define SHARED_CB 0

#define BACKGROUND_SSB 26

#define CHUNK_WIDTH_PIXELS (8 * 32)
#define CHUNK_HEIGHT_PIXELS (8 * 32)

#define LEVEL_WIDTH_CHUNKS 8
#define LEVEL_HEIGHT_CHUNKS 1

static const int sb_size = sizeof(SCREENBLOCK);
static const int sb_size_short = sizeof(SCREENBLOCK) / 2;

OBJ_ATTR _obj_buffer[128];

OBJ_ATTR *_player = &_obj_buffer[0];

FIXED _player_x;
FIXED _player_y;

FIXED _speed = (FIXED)(2.0f*FIX_SCALEF);

bool facing_left = false;

void game_loop() {
	FIXED bg_pos_x = int2fx(0);
	FIXED bg_pos_y = int2fx(0);

	while(1)
	{
		vid_vsync();
		key_poll();

		//Movement
		if(key_held(KEY_RIGHT)) {
			bg_pos_x += _speed;
		}
		
		if(key_held(KEY_LEFT)) {
			bg_pos_x -= _speed;
		}
		
		if(key_held(KEY_UP)) {
			bg_pos_y -= _speed;
		}
		
		if(key_held(KEY_DOWN)) {
			bg_pos_y += _speed;
		}

		//Stolen from Goodboy Advancde
		//Transformed from fixed to int
		int ofs_x = fx2int(bg_pos_x);
		int ofs_y = fx2int(bg_pos_y);
	
		//Checks if it's at the end 
		if(ofs_x > LEVEL_WIDTH_CHUNKS * CHUNK_WIDTH_PIXELS - CHUNK_WIDTH_PIXELS + 16) {
			ofs_x = LEVEL_WIDTH_CHUNKS * CHUNK_WIDTH_PIXELS - CHUNK_WIDTH_PIXELS + 16;
		} else if(ofs_x < 0) {
			ofs_x = 0;
		}

		//Get the remainder of the offset from the chunk
		//This works well since 256 % 256 is 0 so is 512 % 256 and so on
		//For somthing like 258 we are 2 pixels into the next chunk which is 
		//good since this will resolve to 2
		REG_BG0HOFS = ofs_x % CHUNK_WIDTH_PIXELS;
		REG_BG0VOFS = ofs_y % CHUNK_HEIGHT_PIXELS;
		//Logging
		char str[200];
		sprintf(str, "x:%d hofs:%d", ofs_x, ofs_x % CHUNK_WIDTH_PIXELS);
		write_to_log(LOG_LEVEL_WARN, str);

		//Getting chunk number
		//First we / the offset x with the width of a chunk (256) This offsets it to the target chunk
		int chunk_x0 = (ofs_x/CHUNK_WIDTH_PIXELS);
		int chunk_x1 = (ofs_x/CHUNK_WIDTH_PIXELS + 1);

		//Getting map data for target chunk
		//background_2048x32Map is the map
		//sb_size_short is 256 since a chunk is 32 x 32
		//We then access the chunk by the offset using *
		unsigned short *src_sb_0 = (unsigned short*)background_2048x32Map + sb_size_short * chunk_x0;
		unsigned short *src_sb_1 = (unsigned short*)background_2048x32Map + sb_size_short * chunk_x1;

		//Copying into the vram
		dma3_cpy(&se_mem[BACKGROUND_SSB], src_sb_0, sb_size);
		dma3_cpy(&se_mem[BACKGROUND_SSB+1], src_sb_1, sb_size);
	}
}

void init() {
	dma3_cpy(pal_bg_mem, bkg_sharedPal, bkg_sharedPalLen);
	dma3_cpy(&tile_mem[SHARED_CB], bkg_sharedTiles, bkg_sharedTilesLen);

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

	REG_DISPCNT =  DCNT_BG0 ;

	_player_x = int2fx(SCREEN_WIDTH / 2 - 32);
	_player_y = int2fx(SCREEN_HEIGHT/ 2 - 32);
}

void start_game() {
	init_debug();
	init();
	game_loop();
}

int main()
{
	start_game();

    while(1);

    return 0;
}
