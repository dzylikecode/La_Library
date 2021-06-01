/*
***************************************************************************************
*  程    序:
*
*  作    者: LaDzy
*
*  邮    箱: mathbewithcode@gmail.com
*
*  编译环境: Visual Studio 2019
*
*  创建时间: 2021/04/23 22:42:26
*  最后修改:
*
*  简    介: 精灵，觉得这不仅是个集成的数据，
*			 图片和运动关联，构造生命的感觉是更深的含义
*			 借鉴文字的想法，文字同样可以表达出意境来，而它的构成是字的某种排列
*			 同样的思路，用一张张图来当作 word ，来构造图片的 string
*			 所谓 word 更多的像是一本 dictionary
*			 character —> string -> paragraph -> article -> novel
*			 真是美妙的想法，我又开心地笑了，哈哈
*			 再思考一下，paragraph 比 string 要多语境，语境是个好东西
*			 novel 明显是 little world 的感觉
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
	typedef STRING<int, ANI_NULL>		aniSTRING;//实际上存放的就是索引

	//语句在于增删，而且讲究先后，所以就链表啦
	class aniCONTENT:public LINKEDList<aniSTRING>//就是一堆的string
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
		int state;//总的状态
		int attr;
		int anim_state;//动画序列的状态
		int anim_counter; //循环多少次才显现一帧，不是每个循环都是一帧
		int anim_count_max;
	public:
		double x, y;
		double vx, vy;

	};
}