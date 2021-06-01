/*
***************************************************************************************
*  ��    ��:
*
*  ��    ��: LaDzy
*
*  ��    ��: mathbewithcode@gmail.com
*
*  ���뻷��: Visual Studio 2019
*
*  ����ʱ��: 2021/04/23 22:42:26
*  ����޸�:
*
*  ��    ��: ���飬�����ⲻ���Ǹ����ɵ����ݣ�
*			 ͼƬ���˶����������������ĸо��Ǹ���ĺ���
*			 ������ֵ��뷨������ͬ�����Ա����⾳���������Ĺ������ֵ�ĳ������
*			 ͬ����˼·����һ����ͼ������ word ��������ͼƬ�� string
*			 ��ν word ���������һ�� dictionary
*			 character ��> string -> paragraph -> article -> novel
*			 ����������뷨�����ֿ��ĵ�Ц�ˣ�����
*			 ��˼��һ�£�paragraph �� string Ҫ���ﾳ���ﾳ�Ǹ��ö���
*			 novel ������ little world �ĸо�
*
***************************************************************************************
*/
#pragma once
#include "La_Surface.h"

namespace GRAPHIC
{
#define ANI_NULL		-1
	typedef SURFACE						aniWORD;
	typedef ARRAY<aniWORD>				aniDICT;
	typedef STRING<int, ANI_NULL>		aniSTRING;//ʵ���ϴ�ŵľ�������

	//���������ɾ�����ҽ����Ⱥ����Ծ�������
	class aniCONTENT:public LINKEDList<aniSTRING>//����һ�ѵ�string
	{
	private:
		aniDICT*  aniDict;
	public:
		aniWORD& operator[](int i) { return (*aniDict)[data()[i]]; }
		void setDict(aniDICT* dict) { aniDict = dict; }
	};



#define SPRITE_STATE_DEAD		0
#define SPRITE_STATE_DYING		2
#define SPRITE_STATE_ALIVE		1
	//animation state
#define SPRITE_STATE_ANIM_DONE	1

#define SPRITE_ATTR_SINGLE_FRAME	1
#define SPRITE_ATTR_MULTI_FRAME		2//1<<1
#define SPRITE_ATTR_MULTI_ANIM		4//1<<2
#define SPRITE_ATTR_ANIM_ONE_SHOT	8
#define SPRITE_ATTR_VISIBLE			16
#define SPRITE_ATTR_BOUNCE			32
#define SPRITE_ATTR_WRAPAROUND		64
#define SPRITE_ATTR_CLONE			256

	class SPRITE
	{
	private:
		aniCONTENT content;
		int state;//�ܵ�״̬
		int attr;
		int anim_state;//�������е�״̬
		int anim_counter; //ѭ�����ٴβ�����һ֡������ÿ��ѭ������һ֡
		int anim_count_max;
	public:
		double x, y;
		double vx, vy;

	};
}