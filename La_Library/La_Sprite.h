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
#define ANIM_NULL		-1
	typedef SURFACE						aniPARAPHRASE;
	typedef ARRAY<aniPARAPHRASE>		aniDICT;
	typedef int							aniWORD;
	typedef STRING<aniWORD, ANIM_NULL>	aniSTRING;//ʵ���ϴ�ŵľ�������

#define IS_ANIM_NULL(word)			(word == ANIM_NULL)

	//����һ�ѵĹ��� dict �� string
	//�ü��εĹ۵������������� �� i ��j �� �㴦�ĺ���
	class aniCONTENT:public ARRAY<aniSTRING>
	{
	private:
		aniDICT*  aniDict;
	public:
		void	setDict(aniDICT* dict) { aniDict = dict; }
		aniPARAPHRASE& operator()(int i, int j) { return (*aniDict)[(*this)[i][j]]; }
		const aniPARAPHRASE& operator()(int i, int j)const { return (*aniDict)[(*this)[i][j]]; }
	};

	//����û�в���ʱ��
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
		aniSTRING aniString(num);//���ǳ��ȣ��Զ��� 1 ���Լ���ƵĻ�����������
		
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
		int anim_counter; //ѭ�����ٴβ�����һ֡������ÿ��ѭ������һ֡
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
			//anim_counter���������ڻ���Ŀ�����
			//������Ϸѭ���Ŀ�����bob�ڲ�Ҳ�ɵ��ڣ����Ժ��˹����ܣ��߼����뿪��
			//��Ϸѭ���˶��ٴΣ��Ų���һ֡
			//anim_counter++;  �ϲ�������

			//�ﵽ�����ӵĴ���
			if (++anim_counter >= anim_count_max)
			{
				//reset
				anim_counter = 0;
				//move to next frame
				//ѭ��
				if (IS_ANIM_NULL(content[cur_string][++cur_word]))
				{
					if (attr & LOOP)
						cur_word = 0;
					else 
						cur_word--; //ֹͣ�����һ��
				}
			}
		}
		int  getWidth()const { return content(cur_string, cur_word).getWidth(); }
		int  getHeight()const { return content(cur_string, cur_word).getHeight(); }
		void move()
		{
			x += vx;
			y += vy;

			//����Ļ������������
			//���ֵ�����Բ�ĸо�
			int width = getWidth();
			int height = getHeight();
			if (attr & ROUND)
			{
				if (x > max_clip_x)
				{
					x = min_clip_x - width;//Ԥ����������������������
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
			else if (attr & BOUNCE)//������ʽ��������������ײ
			{
				//�߽練��,�ı���Ƕ���
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
			//��������Ϊ3/8��3/8��
			int width1 = (getWidth() >> 1) - (getWidth() >> 3);
			int height1 = (getHeight() >> 1) - (getHeight() >> 3);

			int width2 = (sp.getWidth() >> 1) - (sp.getWidth() >> 3);
			int height2 = (sp.getHeight() >> 1) - (sp.getHeight() >> 3);

			//��λ�����Ե�����
			int cx1 = x + width1;
			int cy1 = y + height1;

			int cx2 = sp.x + width2;
			int cy2 = sp.y + height2;

			//��������֮��ľ���
			int dx = abs(cx1 - cx2);
			int dy = abs(cy1 - cy2);

			//��ײ�ж�
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

