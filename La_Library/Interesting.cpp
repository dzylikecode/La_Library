#include "La_Surface.h"
#include "La_GeometryBase.h"

namespace GRAPHIC
{
	//想着是可以放入色素矩阵
	void Blit_Clipped(int x, int y, int iwidth, int iheight, COLOR* bitmap)
	{
		// first do trivial rejections of bitmap, is it totally invisible?
		if ((x > max_clip_x) || (y > max_clip_y) ||
			((x + iwidth) < min_clip_x) || ((y + iheight) < min_clip_y))
			return;

		int x1 = x;
		int y1 = y;
		int x2 = x1 + iwidth - 1;
		int y2 = y1 + iheight - 1;


		x1 = max(min_clip_x, x1);
		y1 = max(min_clip_y, y1);
		x2 = min(max_clip_x, x2);
		y2 = min(max_clip_y, y2);

		// now we know to draw only the portions of the bitmap from (x1,y1) to (x2,y2)
		// compute offsets into bitmap on x,y axes, we need this to compute starting point
		// to rasterize from
		int x_off = x1 - x;
		int y_off = y1 - y;

		// compute number of columns and rows to blit
		int dx = x2 - x1 + 1;
		int dy = y2 - y1 + 1;

		COLOR* tempMemory = graphicOut.getMemory();
		// compute starting address in video_buffer 
		tempMemory += (x1 + y1 * graphicOut.getLpitch());

		// compute starting address in bitmap to scan data from
		bitmap += (x_off + y_off * iwidth);

		// at this point bitmap is pointing to the first pixel in the bitmap that needs to
		// be blitted, and video_buffer is pointing to the memory location on the destination
		// buffer to put it, so now enter rasterizer loop

		COLOR pixel; // used to read/write pixels

		for (int index_y = 0; index_y < dy; index_y++)
		{
			// inner loop, where the action takes place
			for (int index_x = 0; index_x < dx; index_x++)
			{
				// read pixel from source bitmap, test for transparency and plot
				if ((pixel = bitmap[index_x]))
					tempMemory[index_x] = pixel;

			}
			tempMemory += graphicOut.getLpitch();
			bitmap += iwidth;
		}
	}


	inline void DarkColor(int& color)
	{
		if ((color -= 4) < 0)
		{
			color = 0;
		}
	}

	inline void EnhanceColor(int& color)
	{
		if ((color += 4) > 255)
		{
			color = 255;
		}
	}


#define SCREEN_DARKNESS  0         // fade to black
#define SCREEN_WHITENESS 1         // fade to white
#define SCREEN_SWIPE_X   2         // do a horizontal swipe
#define SCREEN_SWIPE_Y   3         // do a vertical swipe
#define SCREEN_DISOLVE   4         // a pixel dissolve
#define SCREEN_SCRUNCH   5         // a square compression
#define SCREEN_BLUENESS  6         // fade to blue
#define SCREEN_REDNESS   7         // fade to red
#define SCREEN_GREENNESS 8         // fade to green

	/// <summary>
/// this function can be called to perform a my raid of screen transitions
/// to the destination buffer, make sure to save and restore the palette
/// when performing color transitions in 8-bit modes
/// </summary>
/// <param name="effect">
/// 多种模式，变暗，变亮，变红、绿、蓝，上下、左右收缩，溶解
/// </param>
	void ScreenTransitions(int effect)
	{
		SURFACE* surface = &graphicOut;
		int center_width = surface->getWidth() >> 1;
		int center_height = surface->getHeight() >> 1;
		int surface_square = surface->getWidth() * surface->getHeight();
		COLOR* color = surface->getMemory();
		int rand_number = 4 * surface_square;
		int screenLpitch = surface->getLpitch();
		int screenWidth = surface->getWidth();
		int screenHeight = surface->getHeight();

		switch (effect)
		{
		case SCREEN_DARKNESS:
			//一个比较好的办法是改变色彩表

			//改造每一个颜色，所有颜色全部更新
			//每个分量大于4，就-3 使其变得暗淡
			//小于等于4，直接变黑

			for (int i = 0; i < screenHeight; i++)
			{
				for (int j = 0; j < screenWidth; j++)
				{
					if (!*color)
					{
						continue;
					}

					int red = GetRValue_DX(*color);
					int yellow = GetGValue_DX(*color);
					int blue = GetBValue_DX(*color);

					DarkColor(red);
					DarkColor(yellow);
					DarkColor(blue);

					color[j] = RGB_DX(red, yellow, blue);
				}
				color += screenLpitch;
			}
			break;

		case SCREEN_WHITENESS:
			for (int i = 0; i < screenHeight; i++)
			{
				for (int j = 0; j < screenWidth; j++)
				{
					int red = GetRValue_DX(*color);
					int yellow = GetGValue_DX(*color);
					int blue = GetBValue_DX(*color);

					EnhanceColor(red);
					EnhanceColor(yellow);
					EnhanceColor(blue);

					color[j] = RGB_DX(red, yellow, blue);
				}
				color += screenLpitch;
			}
			break;

		case SCREEN_REDNESS:

			for (int i = 0; i < screenHeight; i++)
			{
				for (int j = 0; j < screenWidth; j++)
				{
					int red = GetRValue_DX(*color);
					int yellow = GetGValue_DX(*color);
					int blue = GetBValue_DX(*color);

					EnhanceColor(red);
					DarkColor(yellow);
					DarkColor(blue);

					color[j] = RGB_DX(red, yellow, blue);
				}
				color += screenLpitch;
			}
			break;

		case SCREEN_GREENNESS:

			for (int i = 0; i < screenHeight; i++)
			{
				for (int j = 0; j < screenWidth; j++)
				{
					int red = GetRValue_DX(*color);
					int yellow = GetGValue_DX(*color);
					int blue = GetBValue_DX(*color);

					DarkColor(red);
					EnhanceColor(yellow);
					DarkColor(blue);

					color[j] = RGB_DX(red, yellow, blue);
				}
				color += screenLpitch;
			}
			break;

		case SCREEN_BLUENESS:

			for (int i = 0; i < screenHeight; i++)
			{
				for (int j = 0; j < screenWidth; j++)
				{
					int red = GetRValue_DX(*color);
					int yellow = GetGValue_DX(*color);
					int blue = GetBValue_DX(*color);

					DarkColor(red);
					DarkColor(yellow);
					EnhanceColor(blue);

					color[j] = RGB_DX(red, yellow, blue);
				}
				color += screenLpitch;
			}
			break;

		case SCREEN_SWIPE_X:
			//从两边进行擦除，向中心靠拢
			for (int i = 0; i < center_width; i += 2)
			{
				//the right
				VLine(0, max_clip_y, max_clip_x - i, 0, surface);
				//the left
				VLine(0, max_clip_y, i, 0, surface);
				//the right
				VLine(0, max_clip_y, max_clip_x - i - 1, 0, surface);
				//the left
				VLine(0, max_clip_y, i + 1, 0, surface);
			}
			break;

		case SCREEN_SWIPE_Y:

			for (int i = 0; i < center_height; i += 2)
			{
				HLine(0, max_clip_x, max_clip_y - i, 0, surface);
				HLine(0, max_clip_x, i, 0, surface);
				HLine(0, max_clip_x, max_clip_y - i - 1, 0, surface);
				HLine(0, max_clip_x, i + 1, 0, surface);
			}
			break;

		case SCREEN_SCRUNCH:
			//正方形压缩
			//也就是上下左右同时进行
			//这是默认了 width 大于 center
			for (int i = 0; i < center_width; i += 2)
			{
				//先上下
				//同余来调整，担心过大
				HLine(0, max_clip_x, max_clip_y - i % center_height, 0, surface);
				HLine(0, max_clip_x, i % center_height, 0, surface);
				HLine(0, max_clip_x, max_clip_y - i % center_height - 1, 0, surface);
				HLine(0, max_clip_x, i % center_height + 1, 0, surface);

				//再左右
				//the right
				VLine(0, max_clip_y, max_clip_x - i, 0, surface);
				//the left
				VLine(0, max_clip_y, i, 0, surface);
				//the right
				VLine(0, max_clip_y, max_clip_x - i - 1, 0, surface);
				//the left
				VLine(0, max_clip_y, i + 1, 0, surface);
			}
			break;

		case SCREEN_DISOLVE:
			//dissolve the screen by plotting zillions of little black dots
			for (int i = 0; i <= rand_number; i++)
			{
				int x = rand() % screenWidth;
				int y = rand() % screenHeight;
				color[y * screenLpitch + x] = 0;
			}
			break;
		}
	}
}