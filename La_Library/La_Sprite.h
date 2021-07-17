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
#define ANIM_NULL		-1
	typedef SURFACE						aniPARAPHRASE;
	typedef ARRAY<aniPARAPHRASE>		aniDICT;
	typedef int							aniWORD;
	typedef STRING<aniWORD, ANIM_NULL>	aniSTRING;//实际上存放的就是索引

#define IS_ANIM_NULL(word)			(word == ANIM_NULL)

	//就是一堆的共用 dict 的 string
	//用几何的观点来看，就是在 （ i ，j ） 点处的含义
	class aniCONTENT:public ARRAY<aniSTRING>
	{
	private:
		aniDICT*  aniDict;
	public:
		void	setDict(aniDICT* dict) { aniDict = dict; }
		aniPARAPHRASE& operator()(int i, int j) { return (*aniDict)[(*this)[i][j]]; }
		const aniPARAPHRASE& operator()(int i, int j)const { return (*aniDict)[(*this)[i][j]]; }
	};

	//这是没有参数时候
	inline void InitializeAniString(int pos, aniSTRING& aniString)
	{
		aniString[pos] = ANIM_NULL;
	}

	template <typename T, typename...Args>
	void InitializeAniString(int pos, aniSTRING& aniString, const T& t, const Args&...rest)
	{
		aniString[pos++] = t;
		InitializeAniString(pos, aniString, rest...);
	}

	template <typename...Args>
	aniSTRING InitAniString(const Args&...rest)
	{
		int num = sizeof...(Args);
		aniSTRING aniString(num);//这是长度，自动加 1 ，自己设计的还会搞错。。。。
		
		InitializeAniString(0, aniString, rest...);
		return aniString;
	}
#define ANIM InitAniString
	class SPRITE
	{
	public:
		static const int LOOP = 2;
		static const int VISIBLE = 16;
		static const int BOUNCE = 32;
		static const int ROUND = 64;
	public:
		aniCONTENT content;
	private:
		int attr;
		int anim_counter; //循环多少次才显现一帧，不是每个循环都是一帧
		int anim_count_max;
		int cur_word;
		int cur_string;
	public:
		double x, y;
		double vx, vy;
	public:
		SPRITE()
		{
			attr = ROUND | VISIBLE | LOOP;

			anim_counter = anim_count_max = 0;

			x = x;
			y = y;
			vx = 0;
			vy = 0;

			cur_string = cur_word = 0;
		}
		void drawOn(SURFACE* surface = nullptr) { if (attr & VISIBLE) content(cur_string, cur_word).drawOn(x, y, surface); }
		void drawOn(int sx, int sy, SURFACE* surface = nullptr) { if (attr & VISIBLE) content(cur_string, cur_word).drawOn(x, y, sx, sy, surface); }
		void animate()
		{
			//anim_counter是用来调节画面的快慢的
			//除了游戏循环的快慢，bob内部也可调节，可以和人工智能，逻辑分离开来
			//游戏循环了多少次，才播放一帧
			//anim_counter++;  合并到下面

			//达到了拖延的次数
			if (++anim_counter >= anim_count_max)
			{
				//reset
				anim_counter = 0;
				//move to next frame
				//循环
				if (IS_ANIM_NULL(content[cur_string][++cur_word]))
				{
					if (attr & LOOP)
						cur_word = 0;
					else 
						cur_word--; //停止在最后一画
				}
			}
		}
		int  getWidth()const { return content(cur_string, cur_word).getWidth(); }
		int  getHeight()const { return content(cur_string, cur_word).getHeight(); }
		void move()
		{
			x += vx;
			y += vy;

			//把屏幕左右两端相连
			//有种地球是圆的感觉
			int width = getWidth();
			int height = getHeight();
			if (attr & ROUND)
			{
				if (x > max_clip_x)
				{
					x = min_clip_x - width;//预备出来，而不是立即出来
				}
				else if (x + width < min_clip_x)
				{
					x = max_clip_x;
				}

				if (y > max_clip_y)
				{
					y = min_clip_y - height;
				}
				else if (y + height < min_clip_y)
				{
					y = max_clip_y;
				}
			}
			else if (attr & BOUNCE)//反弹形式，就像是物理碰撞
			{
				//边界反弹,改变的是动量
				if ((x + width > max_clip_x) || (x < min_clip_x))
				{
					vx = -vx;
				}

				if ((y + height > max_clip_y) || (y < min_clip_y))
				{
					vy = -vy;
				}
			}
		}
		void setAniSpeed(int speed){ anim_count_max = speed; }
		void setAniString(int index) { cur_string = index; cur_word = 0; }
		bool isCollsion(const SPRITE& sp)
		{
			//调整形心为3/8，3/8处
			int width1 = (getWidth() >> 1) - (getWidth() >> 3);
			int height1 = (getHeight() >> 1) - (getHeight() >> 3);

			int width2 = (sp.getWidth() >> 1) - (sp.getWidth() >> 3);
			int height2 = (sp.getHeight() >> 1) - (sp.getHeight() >> 3);

			//定位到绝对的形心
			int cx1 = x + width1;
			int cy1 = y + height1;

			int cx2 = sp.x + width2;
			int cy2 = sp.y + height2;

			//计算形心之间的距离
			int dx = abs(cx1 - cx2);
			int dy = abs(cy1 - cy2);

			//碰撞判定
			if (dx < (width1 + width2) && dy < (height1 + height2))
			{
				return true;
			}
			//else no collision
			return false;
		}
		void set(int attrSprite) { SET_BIT(attr, attrSprite); }
		void reset(int attrSprite) { RESET_BIT(attr, attrSprite); }
		int  getCurString()const { return cur_string; }
		int  getCurWord()const { return cur_word; }
	};
}

