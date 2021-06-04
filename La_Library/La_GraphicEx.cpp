#include "La_GraphicEx.h"
#include "La_Surface.h"

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