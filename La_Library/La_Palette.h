#pragma once
#include "La_GraphicBase.h"

namespace GRAPHIC
{
	void CopyPalette(laPALETTE& dest, laPALETTE& source);
	class laPALETTEMaster
	{
	private:
		laPALETTE palette;
	public:
		laPALETTEMaster() :palette{ 0 } {};
		bool load(LPCTCH fileName);
		void save(laPALETTE& outSave){ CopyPalette(outSave, palette); }
		bool save(LPCTCH fileName);
		void set(laPALETTE& outPal) { CopyPalette(palette, outPal); }
		void rotate(int beg, int end);
	public:
		const COLOR& operator[](int i)const { return palette[i]; }
		COLOR& operator[](int i) { return palette[i]; }
	};

	extern laPALETTEMaster curPalette;
}