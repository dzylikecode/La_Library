#include "La_GraphicEx.h"
#include "La_Surface.h"
#include "La_Geometry.h"

namespace GRAPHIC
{
	//想着是可以放入色素矩阵
	void BlitMatrix(int x, int y, COLOR* bitmapMatrix, int iwidth, int iheight, COLOR colorKey)
	{
		// first do trivial rejections of bitmap, is it totally invisible?
		if ((x > max_clip_x) || (y > max_clip_y) ||
			((x + iwidth) < min_clip_x) || ((y + iheight) < min_clip_y))
			return;

		int clipLeft = max(min_clip_x, x);
		int clipTop = max(min_clip_y, y);
		int clipRight = min(max_clip_x, x + iwidth - 1);
		int clipBottom = min(max_clip_y, y + iheight - 1);

		int surLpitch = graphicOut.getLpitch();
		COLOR* tempMemory = graphicOut.getMemory() + clipLeft + clipTop * surLpitch;

		int x_off = clipLeft - x;
		int y_off = clipTop - y;

		bitmapMatrix += (x_off + y_off * iwidth);

		int dx = clipRight - clipLeft + 1;
		int dy = clipBottom - clipTop + 1;


		COLOR pixel;
		for (int index_y = 0; index_y < dy; index_y++)
		{
			for (int index_x = 0; index_x < dx; index_x++)
			{
				if ((pixel = bitmapMatrix[index_x]) != colorKey)
					tempMemory[index_x] = pixel;
			}
			tempMemory += surLpitch;
			bitmapMatrix += iwidth;
		}
	}

}

namespace GRAPHIC
{
#define DARK_NUM	60			// 近似	(256/4)
	inline void DarkColor(int& color)
	{
		if ((color -= 4) < 0)
		{
			color = 0;
		}
	}
#define ENHANCE_NUM		64		//	(256/4)
	inline void EnhanceColor(int& color)
	{
		if ((color += 4) > 255)
		{
			color = 255;
		}
	}



	/// <summary>
	/// this function can be called to perform a my raid of screen transitions
	/// to the destination buffer, make sure to save and restore the palette
	/// when performing color transitions in 8-bit modes
	/// </summary>
	/// <param name="effect">
	/// 多种模式，变暗，变亮，变红、绿、蓝，上下、左右收缩，溶解
	/// </param>
	void ScreenTransitions(int effect, int gap)
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

			for (int times = 0; times < DARK_NUM; times++)
			{
				COLOR* color = surface->getMemory();
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
				Sleep(gap);
			}
			break;

		case SCREEN_WHITENESS:
			for (int times = 0; times < ENHANCE_NUM; times++)
			{
				COLOR* color = surface->getMemory();
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
				Sleep(gap);
			}

			break;

		case SCREEN_REDNESS:

			for (int times = 0; times < ENHANCE_NUM; times++)
			{
				COLOR* color = surface->getMemory();
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
				Sleep(gap);
			}

			break;

		case SCREEN_GREENNESS:

			for (int times = 0; times < ENHANCE_NUM; times++)
			{
				COLOR* color = surface->getMemory();
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
				Sleep(gap);
			}


			break;

		case SCREEN_BLUENESS:

			for (int times = 0; times < ENHANCE_NUM; times++)
			{
				COLOR* color = surface->getMemory();
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
				Sleep(gap);
			}


			break;

		case SCREEN_SWIPE_X:

			for (int times = 0; times < ENHANCE_NUM; times++)
			{
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
				Sleep(gap);
			}


			break;

		case SCREEN_SWIPE_Y:

			for (int times = 0; times < ENHANCE_NUM; times++)
			{
				for (int i = 0; i < center_height; i += 2)
				{
					HLine(0, max_clip_x, max_clip_y - i, 0, surface);
					HLine(0, max_clip_x, i, 0, surface);
					HLine(0, max_clip_x, max_clip_y - i - 1, 0, surface);
					HLine(0, max_clip_x, i + 1, 0, surface);
				}
				Sleep(gap);
			}


			break;

		case SCREEN_SCRUNCH:

			for (int times = 0; times < ENHANCE_NUM; times++)
			{
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
				Sleep(gap);
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