#pragma once
#include "La_GraphicBase.h"


namespace GRAPHIC
{
	class BLINKER;
	struct LIGHT
	{
		friend class BLINKER;
	private:
		enum STATE
		{
			On = 1,
			Off = -1,
			Available = 0,
		};
	public:
		int color_index;		//��ɫ����ڵ�ɫ���λ�ã����ص���ɫ����һ��
		COLOR onCol;			//������ɫ
		COLOR offCol;			//�ص���ɫ

		int on_time;
		int off_time;
		LIGHT() :color_index(0), onCol(0), offCol(0), on_time(0), off_time(0), counter(0), state(Available) {};
	private:
		int counter;    //ѭ�����������ڲ���ʱ��
		STATE state;
	};

	class BLINKER
	{
	public:
		enum OPERATOR
		{
			Add = 0,
			Delete = 1,
			Update = 2,
			Run = 3,
		};
	private:
		static const int blinkerNum = PALETTE_NUM;
		static LIGHT light[blinkerNum];
	public:
		int command(LIGHT& newLight, OPERATOR opera, int id);
	};


}