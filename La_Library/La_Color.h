#pragma once


// these look up tables are used by the 8-bit lighting engine
// the first one holds a color translation table in the form of each
// row is a color 0..255, and each row consists of 256 shades of that color
// the data in each row is the color/intensity indices and the resulting value
// is an 8-bit index into the real color lookup that should be used as the color

// the second table works by each index being a compressed 16bit RGB value
// the data indexed by that RGB value IS the index 0..255 of the real
// color lookup that matches the desired color the closest

UCHAR rgbilookup[256][256];         // intensity RGB 8-bit lookup storage
UCHAR rgblookup[65536];             // RGB 8-bit color lookup
/// <summary>
/// 根据调色板，使用最小平方法找到 8 位 和 16位的关系，放入查找表里面
/// </summary>
/// <param name="rgb_format"></param>
/// <param name="src_palette"></param>
/// <param name="rgblookup">
/// 需要自己申请内存
/// </param>
/// <returns></returns>
int RGB_16_8_IndexedRGB_Table_Builder(int rgb_format,             // format we want to build table for
																  // 99/100 565
	LPPALETTEENTRY src_palette, // source palette
	UCHAR* rgblookup)           // lookup table
{
	// first check the pointers
	if (!src_palette || !rgblookup)
		return(-1);

	// what is the color depth we are building a table for?
	if (rgb_format == DD_PIXEL_FORMAT565)
	{
		// there are a total of 64k entries, perform a loop and look them up, do the least 
		// amount of work, even with a pentium, there are 65536*256 interations here!
		for (int rgbindex = 0; rgbindex < 65536; rgbindex++)
		{
			int  curr_index = -1;        // current color index of best match
			long curr_error = INT_MAX;    // distance in color space to nearest match or "error"

			for (int color_index = 0; color_index < 256; color_index++)
			{
				// extract r,g,b from rgbindex, assuming an encoding of 5.6.5, then scale to 8.8.8 since 
				// palette is in that format always
				int r = (rgbindex >> 11) << 3;;
				int g = ((rgbindex >> 5) & 0x3f) << 2;
				int b = (rgbindex & 0x1f) << 3;

				// compute distance to color from target
				long delta_red = abs(src_palette[color_index].peRed - r);
				long delta_green = abs(src_palette[color_index].peGreen - g);
				long delta_blue = abs(src_palette[color_index].peBlue - b);
				long error = (delta_red * delta_red) + (delta_green * delta_green) + (delta_blue * delta_blue);

				// is this color a better match?
				if (error < curr_error)
				{
					curr_index = color_index;
					curr_error = error;
				}
			}
			// best match has been found, enter it into table
			rgblookup[rgbindex] = curr_index;

		}
	}
	else if (rgb_format == DD_PIXEL_FORMAT555)
	{
		// there are a total of 32k entries, perform a loop and look them up, do the least 
		// amount of work, even with a pentium, there are 32768*256 interations here!
		for (int rgbindex = 0; rgbindex < 32768; rgbindex++)
		{
			int  curr_index = -1;        // current color index of best match
			long curr_error = INT_MAX;    // distance in color space to nearest match or "error"

			for (int color_index = 0; color_index < 256; color_index++)
			{
				// extract r,g,b from rgbindex, assuming an encoding of 5.6.5, then scale to 8.8.8 since 
				// palette is in that format always
				int r = (rgbindex >> 10) << 3;;
				int g = ((rgbindex >> 5) & 0x1f) << 3;
				int b = (rgbindex & 0x1f) << 3;

				// compute distance to color from target
				long delta_red = abs(src_palette[color_index].peRed - r);
				long delta_green = abs(src_palette[color_index].peGreen - g);
				long delta_blue = abs(src_palette[color_index].peBlue - b);
				long error = (delta_red * delta_red) + (delta_green * delta_green) + (delta_blue * delta_blue);

				// is this color a better match?
				if (error < curr_error)
				{
					curr_index = color_index;
					curr_error = error;
				}
			}
			// best match has been found, enter it into table
			rgblookup[rgbindex] = curr_index;
		}
	}
	else return(-1); // serious problem! unsupported format, what are you doing to me!!!!

// return success
	return(1);
}


/// <summary>
/// 每行都是调色板里面的颜色，每行第一个对应着调色板里相应的颜色，后面就是强度逐渐减弱的颜色
/// </summary>
/// <param name="src_palette"></param>
/// <param name="rgbilookup"></param>
/// <param name="intensity_normalization"></param>
/// <returns></returns>
int RGB_16_8_Indexed_Intensity_Table_Builder(LPPALETTEENTRY src_palette,  // source palette
	UCHAR rgbilookup[256][256],  // lookup table
	int intensity_normalization)
{
	// this function takes the source palette to compute the intensity shading table with
	// the table will be formatted such that each row is a color index, and each column
	// is the shade 0..255 desired, the output is a single byte index
	// in either case, it's up to the caller to send in the rgbilookup table pre-allocated
	// 64k buffer byte [256][256]the function doesn't allocate memory for the caller
	// the function builds the table by looping thru each color in the color palette and then
	// for each color, it scales the color to maximum intensity without overflow the RGB channels
	// and then uses this as the 100% intensity value of the color, then the algorithm computes
	// the 256 shades of the color, and then uses the standard least squares scan the find the 
	// colors in the palette and stores them in the row of the current color under intensity 
	// translation, sounds diabolical huh? Note: if you set intensity normalization to 0
	// the the maximization step isn't performed.

	int ri, gi, bi;        // initial color 
	int rw, gw, bw;        // current working color
	float ratio;         // scaling ratio
	float dl, dr, db, dg;   // intensity gradients for 256 shades

	// first check the pointers
	if (!src_palette || !rgbilookup)
		return(-1);

	// for each color in the palette, compute maximum intensity value then scan
	// for 256 shades of it
	for (int col_index = 0; col_index < 256; col_index++)
	{
		// extract color from palette
		ri = src_palette[col_index].peRed;
		gi = src_palette[col_index].peGreen;
		bi = src_palette[col_index].peBlue;

		// find largest channel then max it out and scale other
		// channels based on ratio
		if (intensity_normalization == 1)
		{
			// red largest?
			if (ri >= gi && ri >= bi)
			{
				// compute scaling ratio
				ratio = (float)255 / (float)ri;

				// max colors out
				ri = 255;
				gi = (int)((float)gi * ratio + 0.5);
				bi = (int)((float)bi * ratio + 0.5);

			} 
			else if (gi >= ri && gi >= bi)
				{
					// compute scaling ratio
					ratio = (float)255 / (float)gi;

					// max colors out
					gi = 255;
					ri = (int)((float)ri * ratio + 0.5);
					bi = (int)((float)bi * ratio + 0.5);

				} 
				else // blue is largest
				{
					// compute scaling ratio
					ratio = (float)255 / (float)bi;

					// max colors out
					bi = 255;
					ri = (int)((float)ri * ratio + 0.5);
					gi = (int)((float)gi * ratio + 0.5);
				} 
		} 

		// at this point, we need to compute the intensity gradients for this color,
		// so we can compute the RGB values for 256 shades of the current color
		dl = sqrt(ri * ri + gi * gi + bi * bi) / (float)256;
		dr = ri / dl,
			db = gi / dl,
			dg = bi / dl;

		// initialize working color
		rw = 0;
		gw = 0;
		bw = 0;

		// at this point rw,gw,bw, is the color that we need to compute the 256 intensities for to 
		// enter into the col_index (th) row of the table
		for (int intensity_index = 0; intensity_index < 256; intensity_index++)
		{
			int  curr_index = -1;        // current color index of best match
			long curr_error = INT_MAX;    // distance in color space to nearest match or "error"

			for (int color_index = 0; color_index < 256; color_index++)
			{
				// compute distance to color from target
				long delta_red = abs(src_palette[color_index].peRed - rw);
				long delta_green = abs(src_palette[color_index].peGreen - gw);
				long delta_blue = abs(src_palette[color_index].peBlue - bw);
				long error = (delta_red * delta_red) + (delta_green * delta_green) + (delta_blue * delta_blue);

				// is this color a better match?
				if (error < curr_error)
				{
					curr_index = color_index;
					curr_error = error;
				} 
			} 

		   // best match has been found, enter it into table
			rgbilookup[col_index][intensity_index] = curr_index;

			// compute next intensity level (test for overflow, shouldn't happen, but never know)
			if (rw += dr > 255) rw = 255;
			if (gw += dg > 255) gw = 255;
			if (bw += db > 255) bw = 255;
		} 
	} 

	// return success
	return(1);
} 



int RGBto8BitIndex(UCHAR r, UCHAR g, UCHAR b, LPPALETTEENTRY palette, int flush_cache = 0)
{
	// this function hunts thru the loaded 8-bit palette and tries to find the 
	// best match to the sent rgb color, the 8-bit index is returned. The algorithm
	// performings a least squares match on the values in the CLUT, also to speed up
	// the process, the last few translated colored are stored in a stack in the format
	// rgbi, so when a new rgb comes in, it is compared against the rgb entries in the
	// table, if found then that index is used, else, the rgb is translated and added to
	// the table, and the table is shifted one slot, so the last element is thrown away,
	// hence the table is FIFO in as much as the first discarded value will be the first
	// this way the system keeps previously translated colors cached, so the fairly long
	// least squared scan doesn't take forever!
	// also note the compression of the RGBI data, a compare is performed on the upper 24 bits only
	// also, if flush_cache = 1 then the local cache is flushed, for example a new palette is loaded

#define COLOR_CACHE_SIZE 16 // 16 entries should do for now

	typedef struct
	{
		UCHAR r, g, b;    // the rgb value of this translated color
		UCHAR index;    // the color index that matched is most closely
	} RGBINDEX, * RGBINDEX&;

	static RGBINDEX color_cache[COLOR_CACHE_SIZE];  // the color cache
	static int cache_entries = 0;                     // number of entries in the cache

	// test for flush cache command, new palette coming in...
	if (flush_cache == 1)
		cache_entries = 0;

	// test if the color is in the cache
	for (int cache_index = 0; cache_index < cache_entries; cache_index++)
	{
		// is this a match?
		if (r == color_cache[cache_index].r &&
			g == color_cache[cache_index].g &&
			b == color_cache[cache_index].b)
			return(color_cache[cache_index].index);

	} // end for

// if we get here then we had no luck, so least sqaures scan for best match
// and make sure to add results to cache

	int  curr_index = -1;        // current color index of best match
	long curr_error = INT_MAX;    // distance in color space to nearest match or "error"

	for (int color_index = 0; color_index < 256; color_index++)
	{
		// compute distance to color from target
		long delta_red = abs(palette[color_index].peRed - r);
		long delta_green = abs(palette[color_index].peGreen - g);
		long delta_blue = abs(palette[color_index].peBlue - b);

		long error = (delta_red * delta_red) + (delta_green * delta_green) + (delta_blue * delta_blue);

		// is this color a better match?
		if (error < curr_error)
		{
			curr_index = color_index;
			curr_error = error;
		} // end if

	} // end for color_index

// at this point we have the new color, insert it into cache
// shift cache over one entry, copy elements [0 - (n-1)] . [1 - n]
	memmove((void*)&color_cache[1], (void*)&color_cache[0], COLOR_CACHE_SIZE * sizeof(RGBINDEX) - sizeof(RGBINDEX));

	// now insert the new element
	color_cache[0].r = r;
	color_cache[0].b = b;
	color_cache[0].g = g;
	color_cache[0].index = curr_index;

	// increment number of elements in the cache until saturation
	if (++cache_entries > COLOR_CACHE_SIZE)
		cache_entries = COLOR_CACHE_SIZE;

	// return results
	return(curr_index);

}
