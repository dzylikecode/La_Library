#include "La_GifClass.h"
#include "La_Gif.h"

namespace
{
	using GRAPHIC::aniDICT;
	// data 是自己定义调用的参数
	void GifToTga(void* data, struct GIF_WHDR* whdr)
	{
		uint32_t* pict, * prev, x, y, yoff, iter, ifin, dsrc, ddst;
		aniDICT* stat = (aniDICT*)data;
		static void* statPict, * statPrev;
		static long  statLast = 0;
		static bool  bLast = false;
#define TRANSCOLOR(i)   ((whdr->bptr[i] == whdr->tran) ?  0 : \
						RGB_DX(whdr->cpal[whdr->bptr[i]].R,   \
						whdr->cpal[whdr->bptr[i]].G,		\
						whdr->cpal[whdr->bptr[i]].B			) )


		if (whdr->nfrm - 1 == whdr->ifrm)
		{
			bLast = true;
		}

		if (!whdr->ifrm) //第一帧图
		{
			stat->resize(whdr->nfrm);
			/** TGA doesn`t support heights over 0xFFFF, so we have to trim: **/
			whdr->nfrm = ((whdr->nfrm < 0) ? -whdr->nfrm : whdr->nfrm) * whdr->ydim;
			whdr->nfrm = (whdr->nfrm < 0xFFFF) ? whdr->nfrm : 0xFFFF;

			ddst = (uint32_t)(whdr->xdim * whdr->ydim);
			statPict = calloc(sizeof(uint32_t), ddst);
			statPrev = calloc(sizeof(uint32_t), ddst);
		}
		/** [TODO:] the frame is assumed to be inside global bounds,
					however it might exceed them in some GIFs; fix me. **/
		pict = (uint32_t*)statPict;       //目标文件
		//色素起点
		ddst = (uint32_t)(whdr->xdim * whdr->fryo + whdr->frxo);
		ifin = (!(iter = (whdr->intr) ? 0 : 4)) ? 4 : 5; /** interlacing support **/
		for (dsrc = (uint32_t)-1; iter < ifin; iter++)
			for (yoff = 16U >> ((iter > 1) ? iter : 1), y = (8 >> iter) & 7; y < (uint32_t)whdr->fryd; y += yoff)
				for (x = 0; x < (uint32_t)whdr->frxd; x++)
					if (whdr->tran != (long)whdr->bptr[++dsrc])  //不复制透明色
						pict[(uint32_t)whdr->xdim * y + x + ddst] = TRANSCOLOR(dsrc);

		(*stat)[whdr->ifrm].createFromBuffer(pict, whdr->xdim, whdr->ydim);
		if ((whdr->mode == GIF_PREV) && !statLast)
		{
			whdr->frxd = whdr->xdim;
			whdr->fryd = whdr->ydim;
			whdr->mode = GIF_BKGD;
			ddst = 0;
		}
		else
		{
			statLast = (whdr->mode == GIF_PREV) ? statLast : (unsigned long)(whdr->ifrm + 1);
			pict = (uint32_t*)((whdr->mode == GIF_PREV) ? statPict : statPrev);
			prev = (uint32_t*)((whdr->mode == GIF_PREV) ? statPrev : statPict);
			for (x = (uint32_t)(whdr->xdim * whdr->ydim); --x; pict[x - 1] = prev[x - 1]);
		}
		if (whdr->mode == GIF_BKGD) /** cutting a hole for the next frame **/
			for (whdr->bptr[0] = (uint8_t)((whdr->tran >= 0) ?
				whdr->tran : whdr->bkgd), y = 0,
				pict = (uint32_t*)statPict; y < (uint32_t)whdr->fryd; y++)
				for (x = 0; x < (uint32_t)whdr->frxd; x++)
					pict[(uint32_t)whdr->xdim * y + x + ddst] = TRANSCOLOR(0);

		if (bLast)
		{
			statPict = realloc(statPict, 0L);//释放内存
			statPrev = realloc(statPrev, 0L);//释放内存
		}
#undef TRANSCOLOR
	}
}


namespace GRAPHIC
{
	bool IMAGE_GIF::load(LPCTSTR fileName, ...)
	{
		TCHAR pathFileName[MAX_PATH];
		GetVariableArgument(pathFileName, MAX_PATH, fileName);
		int handle = _open(TToA(pathFileName), O_RDONLY | O_BINARY);
		if (handle <= 0)
			return false;
		int size = (unsigned long)_lseek(handle, 0UL, SEEK_END);
		_lseek(handle, 0UL, SEEK_SET);
		void* data;
		_read(handle, data = realloc(0, size), size);
		_close(handle);
		GIF_Load(data, (long)size, GifToTga, 0, (void*)&frame, 0L);
		data = realloc(data, 0L);//释放内存
	}
}