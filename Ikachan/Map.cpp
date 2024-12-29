#include "Map.h"
#include "Draw.h"
#include "Player.h"
#include "Boss.h"
#include <stdio.h>

BOOL LoadMapData(LPCTSTR path, MAP *map)
{
	//Open file
	FILE *fp = fopen(path, "rb");
	if (fp == NULL)
		return FALSE;
	
	//Read file
	BITMAPFILEHEADER bitmap_file_header;
	BITMAPINFOHEADER bitmap_info_header;
	DWORD bitmap_pal_data[0x100];

	fread(&bitmap_file_header, sizeof(BITMAPFILEHEADER), 1, fp);
	fread(&bitmap_info_header, sizeof(BITMAPINFOHEADER), 1, fp);
	fread(bitmap_pal_data, 4, 0x100, fp);

	//Verify file
	if (bitmap_file_header.bfType != 0x4D42) //'BM' little endian
		return FALSE;
	if (bitmap_info_header.biBitCount != 8) //8bpp
		return FALSE;

	//Get width and height and allocate buffer
	map->width = bitmap_info_header.biWidth;
	map->length = bitmap_info_header.biHeight;
	BYTE *final_buf = (BYTE*)malloc(bitmap_info_header.biWidth * bitmap_info_header.biHeight);
	
	//Read data from file
	BYTE *pre_buf = (BYTE*)malloc(bitmap_info_header.biWidth * bitmap_info_header.biHeight);
	fread(pre_buf, 1, bitmap_info_header.biWidth * bitmap_info_header.biHeight, fp);
	fclose(fp);
	
	//Copy data flipped vertically
	int v13 = 0;
	int v3 = map->width * (map->length - 1);
	while (v13 < (map->width * map->length))
	{
		memcpy(&final_buf[v3], &pre_buf[v13], map->width);
		v13 += map->width;
		v3 -= map->width;
	}
	free(pre_buf);

	//Use data
	map->data = final_buf;
	return TRUE;
}

//Background drawing
#define BACK_WIDTH (((SURFACE_WIDTH + 63) / 64) * 2 + 3)
#define BACK_HEIGHT (((SURFACE_HEIGHT + 63) / 64) * 2 + 3)

void PutBack(FRAME *frame)
{
	RECT rcBack[2] = {
		{ 0, 0, 32, 32 },
		{ 32, 0, 64, 32 },
	};
	for (int i = 0; i < (BACK_WIDTH * BACK_HEIGHT); i++)
	{
		PutBitmap3(&grcFull, 
			((i % BACK_WIDTH) * 32) - (2 * (frame->x / 0x400) / 3 % 64),
			((i / BACK_WIDTH) * 32) - (2 * (frame->y / 0x400) / 3 % 64),
			&rcBack[i % 2],
			SURFACE_ID_BACK);
	}
}

//Map drawing
#define MAP_WIDTH ((SURFACE_WIDTH + 15) / 16 + 1)
#define MAP_HEIGHT ((SURFACE_HEIGHT + 15) / 16 + 1)

RECT rcParts[80] = {
	{   0,  0,  16, 16},
	{  16,  0,  32, 16},
	{  32,  0,  48, 16},
	{  48,  0,  64, 16},
	{  64,  0,  80, 16},
	{  80,  0,  96, 16},
	{  96,  0, 112, 16},
	{ 112,  0, 128, 16},
	{   0,  16,  16, 32},
	{  16,  16,  32, 32},
	{  32,  16,  48, 32},
	{  48,  16,  64, 32},
	{  64,  16,  80, 32},
	{  80,  16,  96, 32},
	{  96,  16, 112, 32},
	{ 112,  16, 128, 32},
	{   0,  32,  16, 48},
	{  16,  32,  32, 48},
	{  32,  32,  48, 48},
	{  48,  32,  64, 48},
	{  64,  32,  80, 48},
	{  80,  32,  96, 48},
	{  96,  32, 112, 48},
	{ 112,  32, 128, 48},
	{   0,  48,  16, 64},
	{  16,  48,  32, 64},
	{  32,  48,  48, 64},
	{  48,  48,  64, 64},
	{  64,  48,  80, 64},
	{  80,  48,  96, 64},
	{  96,  48, 112, 64},
	{ 112,  48, 128, 64},
	{   0,  64,  16, 80},
	{  16,  64,  32, 80},
	{  32,  64,  48, 80},
	{  48,  64,  64, 80},
	{  64,  64,  80, 80},
	{  80,  64,  96, 80},
	{  96,  64, 112, 80},
	{ 112,  64, 128, 80},
	{   0,  80,  16, 96},
	{  16,  80,  32, 96},
	{  32,  80,  48, 96},
	{  48,  80,  64, 96},
	{  64,  80,  80, 96},
	{  80,  80,  96, 96},
	{  96,  80, 112, 96},
	{ 112,  80, 128, 96},
	{   0,  96,  16, 112},
	{  16,  96,  32, 112},
	{  32,  96,  48, 112},
	{  48,  96,  64, 112},
	{  64,  96,  80, 112},
	{  80,  96,  96, 112},
	{  96,  96, 112, 112},
	{ 112,  96, 128, 112},
	{   0, 112,  16, 128},
	{  16, 112,  32, 128},
	{  32, 112,  48, 128},
	{  48, 112,  64, 128},
	{  64, 112,  80, 128},
	{  80, 112,  96, 128},
	{  96, 112, 112, 128},
	{ 112, 112, 128, 128},
	{   0, 128,  16, 144},
	{  16, 128,  32, 144},
	{  32, 128,  48, 144},
	{  48, 128,  64, 144},
	{  64, 128,  80, 144},
	{  80, 128,  96, 144},
	{  96, 128, 112, 144},
	{ 112, 128, 128, 144},
	{   0, 144,  16, 160},
	{  16, 144,  32, 160},
	{  32, 144,  48, 160},
	{  48, 144,  64, 160},
	{  64, 144,  80, 160},
	{  80, 144,  96, 160},
	{  96, 144, 112, 160},
	{ 112, 144, 128, 160},
};

int[2] ColourIndexToTileset(BYTE tile)
{
	switch((tile >> 4)) {
		case 15: // PRTDMG
		case 14: // PRTDMG
			return {5,tile&32};
		case 13: // PRTFILT
		case 12: // PRTFILT
		case 11: // PRTFILT
		case 10: // PRTFILT
		case 9: // PRTFILT
			return {1,tile&128};
		case 8: // PRTBLOCK
		case 7: // PRTBLOCK
		case 6: // PRTBLOCK
		case 5: // PRTBLOCK
			return {4,tile&64};
		case 3: // PRTSNACK
			return {7,tile&16};
		case 2: // PRTDIR
			return {3,tile&16};
		case 1: // PRTITEM
			return {2,tile&16};
		default: // PRTBACK
			return {0,0};
	}
	return {0,0};
}

void PutMapBack(MAP *map, int fx, int fy)
{
	//Draw tiles
	for (int y = (fy / 0x400) / 16; y < ((fy / 0x400) / 16 + MAP_HEIGHT); y++)
	{
		for (int x = (fx / 0x400) / 16; x < ((fx / 0x400) / 16 + MAP_WIDTH); x++)
		{
			//Check if this is a back tile
			BYTE tile = map->data[x + map->width * y];
			int[2] tileindices=ColourIndexToTileset(tile);
			if(tileindices[0] > 1)
			{
				//Draw tile
				PutBitmap3(&grcFull,
					(x * 16) - (fx / 0x400),
					(y * 16) - (fy / 0x400),
					&rcParts[tileindices[1]],
					SURFACE_ID_PRTBACK + tileindices[0]);
			}
		}
	}
}

void PutMapFront(MAP *map, int fx, int fy)
{
	//Draw tiles
	for (int y = (fy / 0x400) / 16; y < ((fy / 0x400) / 16 + MAP_HEIGHT); y++)
	{
		for (int x = (fx / 0x400) / 16; x < ((fx / 0x400) / 16 + MAP_WIDTH); x++)
		{
			//Check if this is a front tile
			BYTE tile = map->data[x + map->width * y];
			int[2] tileindices=ColourIndexToTileset(tile);
			if (tileindices[0] == 1)
			{
				//Draw tile
				PutBitmap3(&grcFull,
					(x * 16) - (fx / 0x400),
					(y * 16) - (fy / 0x400),
					&rcParts[tileindices[1]],
					SURFACE_ID_PRTBACK + tileindices[0]);
			}
		}
	}
}

void PutMapVector(MAP *map, int fx, int fy)
{
	//Scroll
	map->fx += 2;
	if (map->fx >= 16)
		map->fx = 0;

	//Get scrolling rects
	RECT rect[4];

	//Right
	rect[0].left = map->fx;
	rect[0].right = rect[0].left + 16;
	rect[0].top = 0;
	rect[0].bottom = 16;

	//Left
	rect[1].left = 16 - map->fx;
	rect[1].right = rect[1].left + 16;
	rect[1].top = 0;
	rect[1].bottom = 16;

	//Down
	rect[2].left = 0;
	rect[2].right = 16;
	rect[2].top = map->fx;
	rect[2].bottom = rect[2].top + 16;

	//Up
	rect[3].left = 0;
	rect[3].right = 16;
	rect[3].top = 16 - map->fx;
	rect[3].bottom = rect[3].top + 16;

	//Draw tiles
	for (int y = (fy / 0x400) / 16; y < ((fy / 0x400) / 16 + MAP_HEIGHT); y++)
	{
		for (int x = (fx / 0x400) / 16; x < ((fx / 0x400) / 16 + MAP_WIDTH); x++)
		{
			BYTE tile = map->data[x + map->width * y];
			int[2] tileindices=ColourIndexToTileset(tile);
			if (tileindices[0] == 3)
			{
				PutBitmap3(&grcFull,
					(x * 16) - (fx / 0x400),
					(y * 16) - (fy / 0x400),
					&rect[tileindices[1]],
					SURFACE_ID_PRTBACK + tileindices[0]);
			}
		}
	}
}

//Frame
void MoveFrame(FRAME *frame, NPCHAR *npc, MAP *map)
{
	//Get target position
	int tx, ty;

	switch (frame->mode)
	{
		case FRAME_MODE_MYCHAR:
			tx = gMC.x;
			ty = gMC.y;
			break;
		case FRAME_MODE_NPCHAR:
			tx = npc[frame->npc].x;
			ty = npc[frame->npc].y;
			break;
		case FRAME_MODE_BOSS:
			tx = gBoss.x;
			ty = gBoss.y;
			break;
	}
	
	//Move frame towards target
	if ((frame->x + (SURFACE_WIDTH << 9) - 0x2000) < tx)
		frame->x += (tx - (frame->x + (SURFACE_WIDTH << 9) - 0x2000)) / 16;
	if ((frame->x + (SURFACE_WIDTH << 9) - 0x2000) > tx)
		frame->x += (tx - (frame->x + (SURFACE_WIDTH << 9) - 0x2000)) / 16;
	if ((frame->y + (SURFACE_HEIGHT << 9) - 0x2000) < ty)
		frame->y += (ty - (frame->y + (SURFACE_HEIGHT << 9) - 0x2000)) / 16;
	if ((frame->y + (SURFACE_HEIGHT << 9) - 0x2000) > ty)
		frame->y += (ty - (frame->y + (SURFACE_HEIGHT << 9) - 0x2000)) / 16;

	//Keep frame in map bounds
	if (frame->x < 0)
		frame->x = 0;
	if (frame->x > ((map->width - ((SURFACE_WIDTH + 15) / 16)) << 14))
		frame->x = ((map->width - ((SURFACE_WIDTH + 15) / 16)) << 14);
	if (frame->y < 0)
		frame->y = 0;
	if (frame->y > ((map->length - ((SURFACE_HEIGHT + 15) / 16)) << 14))
		frame->y = ((map->length - ((SURFACE_HEIGHT + 15) / 16)) << 14);
}

void MoveFrameEditor(FRAME *frame, MAP *map)
{
	static int holdt;
	
	//Shift frame when direction pressed
	if (gKeyTrg & KEY_LEFT)
		frame->x -= 0x1000;
	if (gKeyTrg & KEY_RIGHT)
		frame->x += 0x1000;
	if (gKeyTrg & KEY_UP)
		frame->y -= 0x1000;
	if (gKeyTrg & KEY_DOWN)
		frame->y += 0x1000;

	//Move frame while direction held for 20 frames
	if (gKey & (KEY_LEFT | KEY_UP | KEY_RIGHT | KEY_DOWN))
	{
		//Wait 20 frames
		if (++holdt >= 20)
		{
			//Cap timer and move
			holdt = 20;
			if (gKey & KEY_LEFT)
				frame->x -= 0x1000;
			if (gKey & KEY_RIGHT)
				frame->x += 0x1000;
			if (gKey & KEY_UP)
				frame->y -= 0x1000;
			if (gKey & KEY_DOWN)
				frame->y += 0x1000;
		}
	}
	else
	{
		//Reset hold timer
		holdt = 0;
	}
	
	//Keep frame in map bounds
	if (frame->x < 0)
		frame->x = 0;
	if (frame->x > ((map->width - ((SURFACE_WIDTH + 15) / 16)) << 14))
		frame->x = ((map->width - ((SURFACE_WIDTH + 15) / 16)) << 14);
	if (frame->y < 0)
		frame->y = 0;
	if (frame->y > ((map->length - ((SURFACE_HEIGHT + 15) / 16)) << 14))
		frame->y = ((map->length - ((SURFACE_HEIGHT + 15) / 16)) << 14);
}