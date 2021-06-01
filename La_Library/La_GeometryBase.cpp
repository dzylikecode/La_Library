#include "La_GeometryBase.h"
#include "La_Math.h"

namespace GRAPHIC
{
	void VLine(int y1, int y2, int x, COLORREF color, SURFACE* surface)
	{
		surface = INCLUDE_NULL_SURFACE(surface);
		if (x > max_clip_x || x < min_clip_x)
		{
			return;
		}

		if (y1 > y2)
		{
			int temp = y1;
			y1 = y2;
			y2 = y1;
		}

		if (y1 > max_clip_y || y2 < min_clip_y)
		{
			return;
		}

		y1 = max(y1, min_clip_y);
		y2 = min(y2, max_clip_y);

		COLOR* start_offset = surface->getMemory() + y1 * surface->getLpitch() + x;

		for (int i = 0; i <= y2 - y1; i++)
		{
			*start_offset = color;

			start_offset += surface->getLpitch();
		}
	}


	//based on Bresenahams Work
//设k < 1
//y = k*x + C
//k=dy/dx
//变形 dy*x-dx*y+C=0
//考虑光栅线和实际线段的误差
//可以写成err = dy*x-dx*y+C
//第一个点是在直线上的，err = 0
//考虑描绘下一个点 -> x+1时，y应该+（k=dy/dx），err += dy
//一方面 k<1,y+=0
//另一方面，y的增量会累积 x+n，y 应该+n*k，err+=n*dy
//n*k>1的时候 y应该+= 1，对应的恰好是 err>dx
//连续的考虑就是 n*k>m时， y+=m，对应的就是err>m*dx-------有点类似取整或者同余
//
//染色
//err += dy
//if (error > dx)
//		error -= dx
//		y++
//
//现在考虑 用0.5做分界，而不是1
//k = dy/dx
//原来的 err0 = k *x -    y + C -> err0 += k  来判断 err0 > 1  ,再 err0-1
//同时扩大dx
//对应   err1 = dy*x - dx*y + C -> err0 += dy 来判断 err1 > dx ,再 err0-dx
//
//现在考虑 用0.5做分界，而不是1
//err = k    * x -        y + C +0.5 -> err  += k    err > 1,    err - 1
//err0=0.5
//扩大2*dx
//err = 2*dy * x - 2*dx * y + CC     -> err  += 2*dy err > 2*dx  err  - 2*dx
//err0=dx

//通过平移可以改变判定标准
//err0=dx					err > 2*dx
//err0=dx - 2*dx = -dx		err > 0
//没必要从err0开始考虑，因为它一定符合
//考虑err1 = 2*dy - dx 作为初始值
	void DrawNotClipLine(int x0, int y0, int x1, int y1, COLORREF color, SURFACE* surface)
	{
		//计算表面对应的起点
		//相当于将（x0，y0）设置为原点
		int surLpitch = surface->getLpitch();
		COLOR* vb_start = surface->getMemory() + x0 + y0 * surLpitch;

		//计算新坐标系下的直线
		int dx = x1 - x0;
		int dy = y1 - y0;

		//采用向量来处理
		int x_inc;
		int y_inc;
		//判断象限，确定x，y的移动方向
		//并且全部转化到第一象限
		if (dx >= 0)
		{
			x_inc = 1;
		}
		else
		{
			x_inc = -1;
			dx = -dx;//转化到第一象限处理
			//dx 于是就描绘了移动的步数
		}

		if (dy >= 0)
		{
			y_inc = surLpitch;
		}
		else
		{
			y_inc = -surLpitch;
			dy = -dy;
		}

		int dx2 = dx << 1;
		int dy2 = dy << 1;

		//用来刻画光栅线和实际线段的误差
		int error;

		if (dx > dy)//斜率小于1
		{
			//第二个点的误差
			error = dy2 - dx;

			//一共要移动dx下，染dx个点
			for (int i = 0; i <= dx; i++)
			{
				//第一个点直接染色
				*vb_start = color;

				//考虑第二个点的位置
				//x 移动
				vb_start += x_inc;
				//y 是否需要移动
				if (error >= 0)
				{
					error -= dx2;
					vb_start += y_inc;
				}

				//第三个点的误差
				error += dy2;
			}
		}
		else
		{
			error = dx2 - dy;

			for (int i = 0; i <= dy; i++)
			{
				*vb_start = color;

				vb_start += y_inc;
				if (error >= 0)
				{
					error -= dy2;
					vb_start += x_inc;
				}

				error += dx2;
			}
		}
	}



	bool ClipLine(int& x1, int& y1, int& x2, int& y2)
	{
		//首先划分裁剪区域
		//			1<<3		1<<3 | 1<<1
		//
		//	1		0			1<<1
		//
		//			1<<2

		 //this function clips the sent line using the globally defined clipping
		 //region

		 //internal clipping codes
#define CLIP_CODE_C  0x0000
#define CLIP_CODE_N  0x0008
#define CLIP_CODE_S  0x0004
#define CLIP_CODE_E  0x0002
#define CLIP_CODE_W  0x0001

#define CLIP_CODE_NE 0x000a
#define CLIP_CODE_SE 0x0006
#define CLIP_CODE_NW 0x0009 
#define CLIP_CODE_SW 0x0005

		//int min_clip_x = ((640 / 2) - 100),      
		//max_clip_x = ((640 / 2) + 100),
		//min_clip_y = ((480 / 2) - 100),
		//max_clip_y = ((480 / 2) + 100);


		//求解裁剪后的数据
		int xc1 = x1;
		int yc1 = y1;
		int xc2 = x2;
		int yc2 = y2;

		//用于判断两点的区域
		int p1_code = 0;
		int p2_code = 0;

		//开始判断
		if (y1 < min_clip_y)
		{
			p1_code |= CLIP_CODE_N;
		}
		else if (y1 > max_clip_y)
		{
			p1_code |= CLIP_CODE_S;
		}

		if (x1 < min_clip_x)
		{
			p1_code |= CLIP_CODE_W;
		}
		else if (x1 > max_clip_x)
		{
			p1_code |= CLIP_CODE_E;
		}

		//对于第二个点
		if (y2 < min_clip_y)
		{
			p2_code |= CLIP_CODE_N;
		}
		else if (y2 > max_clip_y)
		{
			p2_code |= CLIP_CODE_S;
		}

		if (x2 < min_clip_x)
		{
			p2_code |= CLIP_CODE_W;
		}
		else if (x2 > max_clip_x)
		{
			p2_code |= CLIP_CODE_E;
		}
		//二者在相同的区域 中心区域不算，它为0
		if (p1_code & p2_code)//排除了部分 完全在外面的直线
		{
			//例如  \\ 这样的曲线无法排除,有的完全在外，有的部分在内
			return false;
		}

		if (p1_code == 0 && p2_code == 0)//都在中心，无需处理
		{
			return true;
		}

		//determine end clip point for p1
		switch (p1_code)
		{
		case CLIP_CODE_C: break;

			//和顶部直线相交
		case CLIP_CODE_N:
		{
			yc1 = min_clip_y;
			//不会发生y1 == y2 导致的无穷大，要不然都在区域外了
			//+0.5毫无意义，前面是整数除法，+0.5后又转化回整数被截断
			xc1 = x1 + (yc1 - y1) * (x2 - x1) / (y2 - y1);
		}
		break;

		case CLIP_CODE_S:
		{
			yc1 = max_clip_y;
			xc1 = x1 + (yc1 - y1) * (x2 - x1) / (y2 - y1);
		}
		break;

		case CLIP_CODE_W:
		{
			xc1 = min_clip_x;
			yc1 = y1 + (xc1 - x1) * (y2 - y1) / (x2 - x1);
		}
		break;

		case CLIP_CODE_E:
		{
			xc1 = max_clip_x;
			yc1 = y1 + (xc1 - x1) * (y2 - y1) / (x2 - x1);
		}
		break;


		case CLIP_CODE_NE:
		{
			//先假设交在北方
			yc1 = min_clip_y;
			xc1 = x1 + (yc1 - y1) * (x2 - x1) / (y2 - y1);

			//测试是否交点在区域外
			if (xc1 > max_clip_x)
			{
				//在区域外，说明真正的交点在东部
				xc1 = max_clip_x;
				yc1 = y1 + (xc1 - x1) * (y2 - y1) / (x2 - x1);
			}

		}
		break;

		case CLIP_CODE_SE:
		{
			yc1 = max_clip_y;
			xc1 = x1 + (yc1 - y1) * (x2 - x1) / (y2 - y1);

			if (xc1 > max_clip_x)
			{
				xc1 = max_clip_x;
				yc1 = y1 + (xc1 - x1) * (y2 - y1) / (x2 - x1);
			}
		}
		break;

		case CLIP_CODE_NW:
		{
			yc1 = min_clip_y;
			xc1 = x1 + (yc1 - y1) * (x2 - x1) / (y2 - y1);

			if (xc1 < min_clip_x)
			{
				xc1 = min_clip_x;
				yc1 = y1 + (xc1 - x1) * (y2 - y1) / (x2 - x1);
			}
		}
		break;

		case CLIP_CODE_SW:
		{
			yc1 = max_clip_y;
			xc1 = x1 + (yc1 - y1) * (x2 - x1) / (y2 - y1);

			if (xc1 < min_clip_x)
			{
				xc1 = min_clip_x;
				yc1 = y1 + (xc1 - x1) * (y2 - y1) / (x2 - x1);
			}
		}
		break;

		default:
			break;

		}

		switch (p2_code)
		{
		case CLIP_CODE_C: break;

		case CLIP_CODE_N:
		{
			yc2 = min_clip_y;
			xc2 = x2 + (yc2 - y2) * (x1 - x2) / (y1 - y2);
		}
		break;

		case CLIP_CODE_S:
		{
			yc2 = max_clip_y;
			xc2 = x2 + (yc2 - y2) * (x1 - x2) / (y1 - y2);
		}
		break;

		case CLIP_CODE_W:
		{
			xc2 = min_clip_x;
			yc2 = y2 + (xc2 - x2) * (y1 - y2) / (x1 - x2);
		}
		break;

		case CLIP_CODE_E:
		{
			xc2 = max_clip_x;
			yc2 = y2 + (xc2 - x2) * (y1 - y2) / (x1 - x2);
		}
		break;

		case CLIP_CODE_NE:
		{
			yc2 = min_clip_y;
			xc2 = x2 + (yc2 - y2) * (x1 - x2) / (y1 - y2);

			if (xc2 > max_clip_x)
			{
				xc2 = max_clip_x;
				yc2 = y2 + (xc2 - x2) * (y1 - y2) / (x1 - x2);
			}
		}
		break;

		case CLIP_CODE_SE:
		{
			yc2 = max_clip_y;
			xc2 = x2 + (yc2 - y2) * (x1 - x2) / (y1 - y2);

			if (xc2 > max_clip_x)
			{
				xc2 = max_clip_x;
				yc2 = y2 + (xc2 - x2) * (y1 - y2) / (x1 - x2);
			}
		}
		break;

		case CLIP_CODE_NW:
		{
			yc2 = min_clip_y;
			xc2 = x2 + (yc2 - y2) * (x1 - x2) / (y1 - y2);

			if (xc2 < min_clip_x)
			{
				xc2 = min_clip_x;
				yc2 = y2 + (xc2 - x2) * (y1 - y2) / (x1 - x2);
			}
		}
		break;

		case CLIP_CODE_SW:
		{
			yc2 = max_clip_y;
			xc2 = x2 + (yc2 - y2) * (x1 - x2) / (y1 - y2);

			if (xc2 < min_clip_x)
			{
				xc2 = min_clip_x;
				yc2 = y2 + (xc2 - x2) * (y1 - y2) / (x1 - x2);
			}
		}
		break;

		default:
			break;
		}
		//再次检验，因为还有少部分在外面的曲线
		if ((xc1 < min_clip_x) || (xc1 > max_clip_x) ||
			(yc1 < min_clip_y) || (yc1 > max_clip_y) ||
			(xc2 < min_clip_x) || (xc2 > max_clip_x) ||
			(yc2 < min_clip_y) || (yc2 > max_clip_y))
		{
			return false;
		}

		//修改引用值
		x1 = xc1;
		y1 = yc1;
		x2 = xc2;
		y2 = yc2;

		return true;

	}

	void DrawLine(int x0, int y0, int x1, int y1, COLORREF color, SURFACE* surface)
	{
		surface = INCLUDE_NULL_SURFACE(surface);
		int cxs = x0;
		int cys = y0;
		int cxe = x1;
		int cye = y1;

		if (ClipLine(cxs, cys, cxe, cye))
		{
			DrawNotClipLine(cxs, cys, cxe, cye, color, surface);
		}
	}


	//算法
//考虑平底三角形，▲
//一行一行水平地绘画，从上往下，需要画 y 次
//每次下移的时候，计算 x1 ，x2 变化了多少，进行调整
//然后再绘画
//而考虑 x1 ，x2 变化，可以基于 Bresenham 的类似想法
//从顶点开始（x0，y0），初始 y = y0， xLeft = xRight = x0
//下移的时候 xLeft += dx1， xRight += dx2
//
//为了方便，采用浮点计算，也就是不转化为整数，直接用实数来记录累计量
//
//
/// <summary>
/// 顺序符合右手螺旋  左下 1 右下 2 中上 3
/// 平底三角形，▲ ，笛卡尔坐标下
///			(x3,y3)
/// 
/// (x1,y1)			(x2,y2)
/// </summary>

/// <param name="color"></param>
	void FillBottomTri(int x1, int y1, int x2, int y2, int x3, int y3, COLOR color, SURFACE* surface)
	{
		//surface = CHECK_SURFACE(surface);  //只要最终那个保留就可
		//首先保证 x1 < x2
		if (x2 < x1)
		{
			int temp = x2;
			x2 = x1;
			x1 = temp;
		}

		//计算移动的斜率
		REAL height = y3 - y1;
		REAL dx_left = (x3 - x1) / height;
		REAL dx_right = (x3 - x2) / height;

		//起点
		REAL xs = (REAL)x1;
		REAL xe = (REAL)x2 + (REAL)+0.5;//包含一下端点

		//分解为一条一条的水平直线
		//裁剪
		//首先考虑 y 方向落在裁剪区的 H line
		//然后再考虑 H Line 是否在 x 方向内，进行裁剪

		if (y1 < min_clip_y)
		{
			//调整起点
			xs += dx_left * (REAL)(min_clip_y - y1);
			xe += dx_right * (REAL)(min_clip_y - y1);

			y1 = min_clip_y;
		}

		if (y3 > max_clip_y)
		{
			//调整终点
			y3 = max_clip_y;
		}

		//移动到内存起点
		int surLpitch = surface->getLpitch();
		COLOR* dest_addr = surface->getMemory() + y1 * surLpitch;

		//水平直线都在裁剪区域内
		if (x1 >= min_clip_x && x1 <= max_clip_x &&
			x2 >= min_clip_x && x2 <= max_clip_x &&
			x3 >= min_clip_x && x3 <= max_clip_x)
		{
			for (int i = y1; i <= y3; i++)
			{
				MemSetColor(dest_addr + (UINT)xs, color, (UINT)(xe - xs + 1));

				xs += dx_left;
				xe += dx_right;
				dest_addr += surLpitch;
			}
		}
		else//需要判断水平线是否在裁剪区域内
		{
			int left;
			int right;

			for (int i = y1; i <= y3; i++, dest_addr += surLpitch)
			{
				left = (int)xs;
				right = (int)xe;

				//由于continue的缘故，放在这，避免无限循环
				xs += dx_left;
				xe += dx_right;

				//clip line
				if (left < min_clip_x)
				{
					left = min_clip_x;

					if (right < min_clip_x)
					{
						continue;
					}
				}

				if (right > max_clip_x)
				{
					right = max_clip_x;

					if (left > max_clip_x)
					{
						continue;
					}
				}

				MemSetColor(dest_addr + (UINT)left, color, (UINT)(right - left + 1));
			}
		}
	}

	/// <summary>
/// 要想右手螺旋，将其定义为 负 即可，
/// 符合倒三角		中下 1 左上 2 右上 3
/// 三角形  ▼		从下往上
/// (x2,y2)			(x3,y3)
/// 
///			(x1,y1)
/// </summary>
	void FillTopTri(int x1, int y1, int x2, int y2, int x3, int y3, COLOR color, SURFACE* surface)
	{
		if (x3 < x2)
		{
			int temp = x2;
			x2 = x3;
			x3 = temp;
		}

		REAL height = y3 - y1;
		REAL dx_left = (x2 - x1) / height;
		REAL dx_right = (x3 - x1) / height;

		REAL xs = (REAL)x1;
		REAL xe = (REAL)x1;	//+ (REAL) 0.5;

		if (y1 < min_clip_y)
		{
			xs += dx_left * (REAL)(min_clip_y - y1);
			xe += dx_right * (REAL)(min_clip_y - y1);

			y1 = min_clip_y;
		}

		if (y3 > max_clip_y)
		{
			y3 = max_clip_y;
		}
		int surLpitch = surface->getLpitch();
		COLOR* dest_addr = surface->getMemory() + y1 * surLpitch;

		if (x1 >= min_clip_x && x1 <= max_clip_x &&
			x2 >= min_clip_x && x2 <= max_clip_x &&
			x3 >= min_clip_x && x3 <= max_clip_x)
		{
			for (int i = y1; i <= y3; i++)
			{
				MemSetColor(dest_addr + (UINT)xs, color, (UINT)(xe - xs + 1));

				xs += dx_left;
				xe += dx_right;
				dest_addr += surLpitch;
			}
		}
		else
		{
			int left;
			int right;

			//我也不想放在这，但是呢，因为continue的缘故，也这能放在这
			for (int i = y1; i <= y3; i++, dest_addr += surLpitch)
			{
				left = (int)xs;
				right = (int)xe;

				//由于 continue 的缘故，放在这
				xs += dx_left;
				xe += dx_right;

				//clip line
				if (left < min_clip_x)
				{
					left = min_clip_x;

					if (right < min_clip_x)
					{
						continue;
					}
				}

				if (right > max_clip_x)
				{
					right = max_clip_x;

					if (left > max_clip_x)
					{
						continue;
					}
				}

				MemSetColor(dest_addr + (UINT)left, color, (UINT)(right - left + 1));
			}
		}
	}


	/// <summary>
/// 采用定点运算
/// 这个就是Windows坐标系来看的啦
/// </summary>
	void FillTopTriFP(int x1, int y1, int x2, int y2, int x3, int y3, COLOR color, SURFACE* surface)
	{
		//排除三点共线
		if (y1 == y3 && y2 == y3)
		{
			return;
		}

		//首先保证 x2 < x3
		if (x3 < x2)
		{
			int temp = x2;
			x2 = x3;
			x3 = temp;
		}


		//计算移动的斜率
		int height = y3 - y1;
		FIXPOINT dx_left = (INT_TO_FIXP(x2 - x1)) / height;
		FIXPOINT dx_right = (INT_TO_FIXP(x3 - x1)) / height;

		//起点
		//都是从最低点开始
		FIXPOINT xs = INT_TO_FIXP(x1);
		FIXPOINT xe = INT_TO_FIXP(x1);// + (REAL)+0.5;//包含一下端点

		//分解为一条一条的水平直线
		//裁剪
		//首先考虑 y 方向落在裁剪区的 H line
		//然后再考虑 H Line 是否在 x 方向内，进行裁剪

		if (y1 < min_clip_y)
		{
			//调整起点
			//定点运算使然
			xs += dx_left * (min_clip_y - y1);
			xe += dx_right * (min_clip_y - y1);

			y1 = min_clip_y;
		}

		if (y3 > max_clip_y)
		{
			//调整终点
			y3 = max_clip_y;
		}

		//移动到内存起点
		int surLpitch = surface->getLpitch();
		COLOR*dest_addr = surface->getMemory() + y1 * surLpitch;

		//水平直线都在裁剪区域内
		if (x1 >= min_clip_x && x1 <= max_clip_x &&
			x2 >= min_clip_x && x2 <= max_clip_x &&
			x3 >= min_clip_x && x3 <= max_clip_x)
		{
			for (int i = y1; i <= y3; i++)
			{
				MemSetColor(dest_addr + FIXP_TO_ROUND_UP_INT(xs), color, (FIXP_TO_ROUND_UP_INT(xe - xs) + 1));

				xs += dx_left;
				xe += dx_right;
				dest_addr += surLpitch;
			}
		}
		else//需要判断水平线是否在裁剪区域内
		{
			int left;
			int right;

			for (int i = y1; i <= y3; i++, dest_addr += surLpitch)
			{
				left = FIXP_TO_ROUND_UP_INT(xs);
				right = FIXP_TO_ROUND_UP_INT(xe);

				//由于 continue 的缘故，放在这
				xs += dx_left;
				xe += dx_right;


				//clip line
				if (left < min_clip_x)
				{
					left = min_clip_x;

					if (right < min_clip_x)
					{
						continue;
					}
				}

				if (right > max_clip_x)
				{
					right = max_clip_x;

					if (left > max_clip_x)
					{
						continue;
					}
				}
				//这里已经转化为 int 了 不是定点数
				MemSetColor(dest_addr + (UINT)left, color, (UINT)(right - left + 1));
			}
		}
	}

	void FillBottomTriFP(int x1, int y1, int x2, int y2, int x3, int y3, COLOR color, SURFACE* surface)
	{
		if (y1 == y3 && y2 == y3)
		{
			return;
		}

		//首先保证 x1 < x2
		if (x2 < x1)
		{
			int temp = x2;
			x2 = x1;
			x1 = temp;
		}

		//计算移动的斜率
		int height = y3 - y1;
		FIXPOINT dx_left = (INT_TO_FIXP(x3 - x1)) / height;
		FIXPOINT dx_right = (INT_TO_FIXP(x3 - x2)) / height;

		//起点
		FIXPOINT xs = INT_TO_FIXP(x1);
		FIXPOINT xe = INT_TO_FIXP(x2);// + (REAL)+0.5;//包含一下端点

		//分解为一条一条的水平直线
		//裁剪
		//首先考虑 y 方向落在裁剪区的 H line
		//然后再考虑 H Line 是否在 x 方向内，进行裁剪

		if (y1 < min_clip_y)
		{
			//调整起点
			//定点运算使然
			xs += dx_left * (min_clip_y - y1);
			xe += dx_right * (min_clip_y - y1);

			y1 = min_clip_y;
		}

		if (y3 > max_clip_y)
		{
			//调整终点
			y3 = max_clip_y;
		}

		//移动到内存起点
		int surLpitch = surface->getLpitch();
		COLOR* dest_addr = surface->getMemory() + y1 * surLpitch;

		//水平直线都在裁剪区域内
		if (x1 >= min_clip_x && x1 <= max_clip_x &&
			x2 >= min_clip_x && x2 <= max_clip_x &&
			x3 >= min_clip_x && x3 <= max_clip_x)
		{
			for (int i = y1; i <= y3; i++)
			{
				MemSetColor(dest_addr + FIXP_TO_ROUND_UP_INT(xs), color, (FIXP_TO_ROUND_UP_INT(xe - xs) + 1));

				xs += dx_left;
				xe += dx_right;
				dest_addr += surLpitch;
			}
		}
		else//需要判断水平线是否在裁剪区域内
		{
			int left;
			int right;

			for (int i = y1; i <= y3; i++, dest_addr += surLpitch)
			{
				left = FIXP_TO_ROUND_UP_INT(xs);
				right = FIXP_TO_ROUND_UP_INT(xe);

				//由于 continue 的缘故，放在这
				xs += dx_left;
				xe += dx_right;

				//clip line
				if (left < min_clip_x)
				{
					left = min_clip_x;

					if (right < min_clip_x)
					{
						continue;
					}
				}

				if (right > max_clip_x)
				{
					right = max_clip_x;

					if (left > max_clip_x)
					{
						continue;
					}
				}

				MemSetColor(dest_addr + (UINT)left, color, (UINT)(right - left + 1));
			}
		}
	}

#define SWAP_POINT(x1, y1, x2, y2, tempX, tempY)	{tempX = x1; tempY = y1, x1 = x2; y1 = y2; x2 = tempX; y2 = tempY;}
	/// <summary>
	/// 可以贴任意的三角形
	/// 对于非平顶，先排序，
	/// 从下往上为 p1 p2 p3，
	/// 然后用 p2 水平分成两个三角形
	/// 归纳即可
	/// </summary>
	void FillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, COLOR color, SURFACE* surface)
	{
		surface = INCLUDE_NULL_SURFACE(surface);
		//退化形式
		//test for h lines and v lines
		if ((x1 == x2 && x2 == x3) || (y1 == y2 && y2 == y3))
		{
			return;
		}

		//sort p1, p2, p3 in ascending y order
		//	p3
		//	p2
		//	p1
		int tempX;
		int tempY;


		if (y2 < y1)
		{
			SWAP_POINT(x1, y1, x2, y2, tempX, tempY);
		}

		if (y3 < y1)
		{
			SWAP_POINT(x1, y1, x3, y3, tempX, tempY);
		}

		if (y3 < y2)
		{
			SWAP_POINT(x2, y2, x3, y3, tempX, tempY);
		}

		if (y3  < min_clip_y || y1 > max_clip_y ||
			(x1 < min_clip_x && x2 < min_clip_x && x3 < min_clip_x) ||
			(x1 > max_clip_x && x2 > max_clip_x && x3 > max_clip_x))
		{
			return;
		}

		if (y1 == y2) //处理平顶
		{
			FillBottomTri(x1, y1, x2, y2, x3, y3, color, surface);
		}
		else if (y2 == y3) //处理平底
		{
			FillTopTri(x1, y1, x2, y2, x3, y3, color, surface);
		}
		else//将其从拆分
		{
			int new_x = x1 + (int)(0.5 + (REAL)(y2 - y1) * (REAL)(x3 - x1) / (REAL)(y3 - y1));

			FillTopTri(x1, y1, new_x, y2, x2, y2, color, surface);
			FillBottomTri(x2, y2, new_x, y2, x3, y3, color, surface);
		}
	}


	void FillTriangleFP(int x1, int y1, int x2, int y2, int x3, int y3, COLOR color, SURFACE* surface)
	{
		surface = INCLUDE_NULL_SURFACE(surface);
		//退化形式
		//test for h lines and v lines
		if ((x1 == x2 && x2 == x3) || (y1 == y2 && y2 == y3))
		{
			return;
		}

		//sort p1, p2, p3 in ascending y order
		//	p3
		//	p2
		//	p1
		int tempX;
		int tempY;


		if (y2 < y1)
		{
			SWAP_POINT(x1, y1, x2, y2, tempX, tempY);
		}

		if (y3 < y1)
		{
			SWAP_POINT(x1, y1, x3, y3, tempX, tempY);
		}

		if (y3 < y2)
		{
			SWAP_POINT(x2, y2, x3, y3, tempX, tempY);
		}

		if (y3  < min_clip_y || y1 > max_clip_y ||
			(x1 < min_clip_x && x2 < min_clip_x && x3 < min_clip_x) ||
			(x1 > max_clip_x && x2 > max_clip_x && x3 > max_clip_x))
		{
			return;
		}

		if (y1 == y2)
		{
			FillBottomTriFP(x1, y1, x2, y2, x3, y3, color, surface);
		}
		else if (y2 == y3)
		{
			FillTopTriFP(x1, y1, x2, y2, x3, y3, color, surface);
		}
		else
		{
			int new_x = x1 + (int)(0.5 + (REAL)(y2 - y1) * (REAL)(x3 - x1) / (REAL)(y3 - y1));

			FillTopTriFP(x1, y1, new_x, y2, x2, y2, color, surface);
			FillBottomTriFP(x2, y2, new_x, y2, x3, y3, color, surface);
		}
	}



}


















