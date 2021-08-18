#pragma once
#include <stdint.h> /** imports uint8_t, uint16_t and uint32_t **/
#ifndef GIF_MGET
#include <stdlib.h>
//s size  m  buffer   c = 0时 相当 free
#define GIF_MGET(m,s,a,c) m = (uint8_t*)realloc((c)? 0 : m, (c)? s : 0UL)
#endif

//#define GIF_BIGE (*(uint16_t*)"\x7F\x01" == 0x7F01) /** BigEndian machine **/
#ifndef GIF_BIGE
#define GIF_BIGE 0
#endif


#pragma pack(push, 1)
struct GIF_WHDR
{                                /** ======== frame writer info: ======== **/
	long xdim, ydim, clrs,       /** global dimensions, palette size      **/
		bkgd, tran,             /** background index, transparent index  **/
		intr, mode,             /** interlace flag, frame blending mode  **/
		frxd, fryd, frxo, fryo, /** current frame dimensions and offset  **/
		//时间  当前帧数  总的帧数
		time, ifrm, nfrm;       /** delay, frame number, frame count     **/
	uint8_t* bptr;               /** frame pixel indices or metadata      **/
	struct
	{                     /** [==== GIF RGB palette element: ====] **/
		uint8_t R, G, B;         /** [color values - red, green, blue   ] **/
	} *cpal;                     /** current palette                      **/
};
#pragma pack(pop)

enum { GIF_NONE = 0, GIF_CURR = 1, GIF_BKGD = 2, GIF_PREV = 3 };


long GIF_Load(void* data, long size,
	void (*gwfr)(void*, struct GIF_WHDR*),
	void (*eamf)(void*, struct GIF_WHDR*),
	void* anim, long skip);



