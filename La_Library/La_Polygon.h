#pragma once
#include "La_GeometryBase.h"

namespace GRAPHIC
{
	class POLYGON2D
	{
	public:
		REAL xC, yC;  // 形心坐标
		COLOR color;
		REAL vx, vy;
	private:
		VERTEX2D* list;
		int numVertx;
	private:
		void copy(const POLYGON2D& poly)
		{
			clear();
			xC = poly.xC; yC = poly.yC; color = poly.color; vx = poly.vx; vy = poly.vy;
			numVertx = poly.numVertx;
			list = new VERTEX2D[numVertx];
			for (int i = 0; i < numVertx; i++)
			{
				list[i] = poly.list[i];
			}
		}
	public:
		POLYGON2D() :list(nullptr), numVertx(0), xC(0), yC(0), vx(0), vy(0), color(0) {};
		POLYGON2D(const POLYGON2D& poly) :list(nullptr), numVertx(0), xC(0), yC(0), vx(0), vy(0), color(0) { copy(poly); }
		POLYGON2D& operator=(const POLYGON2D& poly)
		{
			if (this != &poly) copy(poly);
			return *this;
		}
		void clear() { if (list) { delete[]list; numVertx = 0; } }
		void copyVertex(VERTEX2D* ver, int num)
		{
			clear();
			numVertx = num;
			list = new VERTEX2D[numVertx];
			for (int i = 0; i < numVertx; i++)
			{
				list[i] = ver[i];
			}
		}
		void addVertex(VERTEX2D* ver, int num) { clear(); list = ver; numVertx = num; }
		~POLYGON2D() { clear(); }
		VERTEX2D& operator[](int i) { return list[i]; }
	public:
		int getNumVertex()const { return numVertx; }
	public:
		void draw(SURFACE* surface = nullptr)
		{
			int i;
			for (i = 0; i < numVertx - 1; i++)
			{
				DrawLine(list[i].x + xC, list[i].y + yC,
					list[i + 1].x + xC, list[i + 1].y + yC,
					color,
					surface);
			}

			//首位相连
			DrawLine(list[i].x + xC, list[i].y + yC,
				list[0].x + xC, list[0].y + yC,
				color,
				surface);
		}
		//移动的是形心
		void translate(REAL dx, REAL dy) { xC += dx; yC += dy; }
		void translateEveryPoint(REAL dx, REAL dy)
		{
			for (int curr_vert = 0; curr_vert < numVertx; curr_vert++)
			{
				list[curr_vert].x += dx;
				list[curr_vert].y += dy;
			}
		}
		void translate(){ xC += vx; yC += vy; }
		void rotate(int theta)
		{
			theta = DEG_STARDAND(theta);

			REAL rx;
			REAL ry;
			for (int curr_vert = 0; curr_vert < numVertx; curr_vert++)
			{
				rx = list[curr_vert].x * cos(theta) - list[curr_vert].y * sin(theta);
				ry = list[curr_vert].x * sin(theta) + list[curr_vert].y * cos(theta);

				list[curr_vert].x = rx;
				list[curr_vert].y = ry;
			}
		}
		void scale(REAL sx, REAL sy)
		{
			for (int curr_vert = 0; curr_vert < numVertx; curr_vert++)
			{
				list[curr_vert].x *= sx;
				list[curr_vert].y *= sy;
			}
		}
		/// <summary>
/// 针对凸多边形
/// 函数没有裁剪，裁剪后仍是凸多边形
/// 所以可以凸多边形预处理
/// </summary>
		void drawFill(SURFACE* surface = nullptr)
		{
			surface = INCLUDE_NULL_SURFACE(surface);
			//从 y 轴方向来看，找到最低的点
				//假设第一个点最低
			int firstvert = 0;
			int min_y =list[0].y;

			//现在用一个已知的点来遍历比较，找到最低
			// i 从 1 开始
			for (int i = 1; i <numVertx; i++)
			{
				if (list[i].y < min_y)
				{
					firstvert = i;
					min_y = list[i].y;
				}
			}

			//两个定位点分头行动，点 1 向前， 点 2 向后运动
				//首先设置起点
			int startvert1 = firstvert;
			int xstart1 = list[startvert1].x + xC;
			int ystart1 = list[startvert1].y + yC;
			int startvert2 = firstvert;
			int xstart2 = list[startvert2].x + xC;
			int ystart2 = list[startvert2].y + yC;

			//设置第一段行程的终点
				//首先考虑 定位点 1 的终点
				//就是上一个端点
			int endvert1 = startvert1 - 1;
			//同余的想法，这是一个圈
			if (endvert1 < 0)
			{
				endvert1 = numVertx - 1;
			}
			int xend1 = list[endvert1].x + xC;
			int yend1 = list[endvert1].y + yC;

			//类似的 点 2 向后运动
			int endvert2 = startvert2 + 1;
			if (endvert2 == numVertx)
			{
				endvert2 = 0;
			}
			int xend2 = list[endvert2].x + xC;
			int yend2 = list[endvert2].y + yC;

			//需要移动的次数
			//除了起点，其他点都会被移动到，或者说被经过
			int edgecount = numVertx - 1;

			int offset1;//用来定位内存里面的位置
			int errorterm1;//用来估计误差
			int ydiff1;    //用来计算一个行程 y 移动的次数，同时也是 dy
			int xdiff1;		//用来计算一个行程 x 移动的步长， 也是 dx
			int xunit1;		//用来描述移动的方向
			int count1;		//用来计算移动的次数

			int offset2;
			int errorterm2;
			int ydiff2;
			int xdiff2;
			int xunit2;
			int count2;

			//用来填充使用
			int length; // length of horizontal line ，填充的长度
			int start;	//开始填充的位置

			int surLpitch = surface->getLpitch();
			COLOR* surMemory = surface->getMemory();

			while (edgecount > 0)
			{
				offset1 = ystart1 * surLpitch + xstart1;
				offset2 = ystart2 * surLpitch + xstart2;

				//initialize error terms
				errorterm1 = 0;
				errorterm2 = 0;

				//get absolute value of
				//觉得这很多余，因为这个算法要求是凸多边形才可以
				//而凸多边形从最低点往上，y是增加的
				if ((ydiff1 = yend1 - ystart1) < 0)
				{
					ydiff1 = -ydiff1;
				}

				if ((ydiff2 = yend2 - ystart2) < 0)
				{
					ydiff2 = -ydiff2;
				}

				if ((xdiff1 = xend1 - xstart1) < 0)
				{
					xunit1 = -1;
					xdiff1 = -xdiff1;
				}
				else
				{
					xunit1 = 1;
				}

				if ((xdiff2 = xend2 - xstart2) < 0)
				{
					xunit2 = -1;
					xdiff2 = -xdiff2;
				}
				else
				{
					xunit2 = 1;
				}

				//基于 Bresenham 的移动策略
				//考虑是 y 移动调整 x ， 还是 x 移动调整 y

				if (xdiff1 > ydiff1)//斜率小于1
				{
					if (xdiff2 > ydiff2)
					{
						count1 = xdiff1;
						count2 = xdiff2;

						//当递减到0的时候，说明其中一个已经到了终点
						//说明到达了下一个顶点，说明继续往下，斜率非常有可能改变
						while (count1 && count2)
						{
							//接下来的几个点 y 没有增量，直接在这之间描点
							//目的是找到准备改变 y 的临界点
							while ((errorterm1 < xdiff1) && (count1 > 0))
							{
								if (count1--)//一个循环点的次数少1
								{
									offset1 += xunit1;
									xstart1 += xunit1; //定位点移动，对应的内存位置跟着相应移动
								}

								errorterm1 += ydiff1; //这是放到倍数的增量

								if (errorterm1 < xdiff1)//y 不足以增加
								{
									//担心定位点向内靠近，外部的点没有描绘到
									//故先 plot      . . .  ->       <-    . . .
									//				plot	待会直接填充	plot	
									//向外远离则不会      
									surMemory[offset1] = color;
								}
							}
							//说明 y 要改变了，误差要更新
							errorterm1 -= xdiff1;

							//同理
							//接下来的几个点 y 没有增量，直接在这之间描点
							while ((errorterm2 < xdiff2) && (count2 > 0))
							{
								if (count2--)//一个循环点的次数少1
								{
									offset2 += xunit2;
									xstart2 += xunit2; //定位点移动，对应的内存位置跟着相应移动
								}

								errorterm2 += ydiff2; //这是放到倍数的增量

								if (errorterm2 < xdiff2)//y 不足以增加
								{
									//
									surMemory[offset2] = color;
								}
							}
							//说明 y 要改变了，误差要更新
							errorterm2 -= xdiff2;

							//此时两个定位点都在一个 水平线上

							//计算两个定位点的距离
							length = offset2 - offset1;

							if (length < 0)
							{
								length = -length;
								start = offset2;
							}
							else
							{
								start = offset1;
							}

							//填充水平点
							for (int i = start; i <= start + length; i++)
							{
								surMemory[i] = color;
							}

							//y 可以改变了
							offset1 += surLpitch;
							ystart1++;
							offset2 += surLpitch;
							ystart2++;
						}
					}
					else//现在考虑斜率大于1
					{
						count1 = xdiff1;
						count2 = ydiff2;//y 的步伐较大，y 来控制 x

						while (count1 && count2)
						{
							while ((errorterm1 < xdiff1) && (count1 > 0))
							{
								if (count1--)
								{
									offset1 += xunit1;
									xstart1 += xunit1;
								}

								errorterm1 += ydiff1;

								if (errorterm1 < xdiff1)
								{
									surMemory[offset1] = color;
								}
							}

							errorterm1 -= xdiff1;

							//y 的步伐大，已经到了y 要改变的临界点
							errorterm2 += xdiff2;

							//判断 x 是否准备改变
							if (errorterm2 >= ydiff2)
							{
								errorterm2 -= ydiff2;
								offset2 += xunit2;
								xstart2 += xunit2;
							}

							count2--;

							length = offset2 - offset1;

							if (length < 0)
							{
								length = -length;
								start = offset2;
							}
							else
							{
								start = offset1;
							}

							for (int i = start; i <= start + length; i++)
							{
								surMemory[i] = color;
							}

							//y 可以改变了
							offset1 += surLpitch;
							ystart1++;
							offset2 += surLpitch;
							ystart2++;
						}
					}
				}
				else
				{
					if (xdiff2 > ydiff2)
					{
						count1 = ydiff1;
						count2 = xdiff2;

						while (count1 && count2)
						{
							//也是 y 控制 x
							errorterm1 += xdiff1;

							if (errorterm1 >= ydiff1)
							{
								errorterm1 -= ydiff1;
								offset1 += xunit1;
								xstart1 += xunit1;
							}

							count1--;

							while ((errorterm2 < xdiff2) && (count2 > 0))
							{
								if (count2--)//一个循环点的次数少1
								{
									offset2 += xunit2;
									xstart2 += xunit2; //定位点移动，对应的内存位置跟着相应移动
								}

								errorterm2 += ydiff2; //这是放到倍数的增量

								if (errorterm2 < xdiff2)//y 不足以增加
								{
									//
									surMemory[offset2] = color;
								}
							}
							//说明 y 要改变了，误差要更新
							errorterm2 -= xdiff2;

							//此时两个定位点都在一个 水平线上

							//计算两个定位点的距离
							length = offset2 - offset1;

							if (length < 0)
							{
								length = -length;
								start = offset2;
							}
							else
							{
								start = offset1;
							}

							//填充水平点
							for (int i = start; i <= start + length; i++)
							{
								surMemory[i] = color;
							}

							//y 可以改变了
							offset1 += surLpitch;
							ystart1++;
							offset2 += surLpitch;
							ystart2++;
						}
					}
					else
					{
						count1 = ydiff1;
						count2 = ydiff2;

						while (count1 && count2)
						{
							//也是 y 控制 x
							errorterm1 += xdiff1;

							if (errorterm1 >= ydiff1)
							{
								errorterm1 -= ydiff1;
								offset1 += xunit1;
								xstart1 += xunit1;
							}

							count1--;

							//y 的步伐大，已经到了y 要改变的临界点
							errorterm2 += xdiff2;

							//判断 x 是否准备改变
							if (errorterm2 >= ydiff2)
							{
								errorterm2 -= ydiff2;
								offset2 += xunit2;
								xstart2 += xunit2;
							}

							count2--;

							length = offset2 - offset1;

							if (length < 0)
							{
								length = -length;
								start = offset2;
							}
							else
							{
								start = offset1;
							}

							for (int i = start; i <= start + length; i++)
							{
								surMemory[i] = color;
							}

							//y 可以改变了
							offset1 += surLpitch;
							ystart1++;
							offset2 += surLpitch;
							ystart2++;
						}
					}
				}

				//定位点到达了终点
				if (!count1)
				{
					edgecount--;

					startvert1 = endvert1;
					endvert1--;

					if (endvert1 < 0)
					{
						endvert1 = numVertx - 1;
					}

					xend1 =  list[endvert1].x + xC;
					yend1 =  list[endvert1].y + yC;
				}

				//有可能同时到达，所以不是 else
				if (!count2)
				{
					edgecount--;
					startvert2 = endvert2;
					endvert2++;

					if (endvert2 == numVertx)
					{
						endvert2 = 0;
					}

					xend2 = list[endvert2].x + xC;
					yend2 = list[endvert2].y + yC;
				}
			}
		}
		void findBound(REAL& min_x, REAL& max_x, REAL& min_y, REAL& max_y)
		{
			if (numVertx == 0)
			{
				min_x = max_x = min_y = max_y = 0;
				return;
			}

			for (int i = 0; i < numVertx; i++)
			{
				if (list[i].x > max_x)
				{
					max_x = list[i].x;
				}

				if (list[i].x < min_x)
				{
					min_x = list[i].x;
				}

				if (list[i].y > max_y)
				{
					max_y = list[i].y;
				}

				if (list[i].y < min_y)
				{
					min_y = list[i].y;
				}
			}

			min_x += xC; max_x += xC;
			min_y += yC; max_y += yC;
		}
	};
}