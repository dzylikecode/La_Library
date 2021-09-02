#pragma once
#include "La_BaseType.h"
// pcx file header
typedef struct PCX_HEADER
{
	UCHAR  manufacturer;    // always 0x0A
	UCHAR  version;         // version 0x05 for version 3.0 and later
	UCHAR  encoding;        // always 1
	UCHAR  bits_per_pixel;  // bits per pixel 1,2,4,8
	USHORT xmin, ymin;      // coordinates of upper left corner
	USHORT xmax, ymax;      // coordinates of lower right corner
	USHORT hres;            // horizontal resolution of image in dpi 75/100 typ
	USHORT yres;            // vertical resolution of image in dpi 75/100
	UCHAR  EGAcolors[48];   // ega palette, not used for 256 color images
	UCHAR  reserved;        // reserved for future use, video mode?
	UCHAR  color_planes;    // number of color planes 3 for 24-bit imagery
	USHORT bytes_per_line;  // bytes per line, always even!
	USHORT palette_type;    // 1 for gray scale, 2 for color palette
	USHORT scrnw;           // width of screen image taken from
	USHORT scrnh;           // height of screen image taken from     
	UCHAR  filler[54];      // filler bytes
};