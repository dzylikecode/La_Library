#pragma once
#include "La_GeometryBase.h"

namespace GRAPHIC
{
	class POLYGON2D
	{
	public:
		REAL xC, yC;  // ��������
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

			//��λ����
			DrawLine(list[i].x + xC, list[i].y + yC,
				list[0].x + xC, list[0].y + yC,
				color,
				surface);
		}
		//�ƶ���������
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
/// ���͹�����
/// ����û�вü����ü�������͹�����
/// ���Կ���͹�����Ԥ����
/// </summary>
		void drawFill(SURFACE* surface = nullptr)
		{
			surface = INCLUDE_NULL_SURFACE(surface);
			//�� y �᷽���������ҵ���͵ĵ�
				//�����һ�������
			int firstvert = 0;
			int min_y =list[0].y;

			//������һ����֪�ĵ��������Ƚϣ��ҵ����
			// i �� 1 ��ʼ
			for (int i = 1; i <numVertx; i++)
			{
				if (list[i].y < min_y)
				{
					firstvert = i;
					min_y = list[i].y;
				}
			}

			//������λ���ͷ�ж����� 1 ��ǰ�� �� 2 ����˶�
				//�����������
			int startvert1 = firstvert;
			int xstart1 = list[startvert1].x + xC;
			int ystart1 = list[startvert1].y + yC;
			int startvert2 = firstvert;
			int xstart2 = list[startvert2].x + xC;
			int ystart2 = list[startvert2].y + yC;

			//���õ�һ���г̵��յ�
				//���ȿ��� ��λ�� 1 ���յ�
				//������һ���˵�
			int endvert1 = startvert1 - 1;
			//ͬ����뷨������һ��Ȧ
			if (endvert1 < 0)
			{
				endvert1 = numVertx - 1;
			}
			int xend1 = list[endvert1].x + xC;
			int yend1 = list[endvert1].y + yC;

			//���Ƶ� �� 2 ����˶�
			int endvert2 = startvert2 + 1;
			if (endvert2 == numVertx)
			{
				endvert2 = 0;
			}
			int xend2 = list[endvert2].x + xC;
			int yend2 = list[endvert2].y + yC;

			//��Ҫ�ƶ��Ĵ���
			//������㣬�����㶼�ᱻ�ƶ���������˵������
			int edgecount = numVertx - 1;

			int offset1;//������λ�ڴ������λ��
			int errorterm1;//�����������
			int ydiff1;    //��������һ���г� y �ƶ��Ĵ�����ͬʱҲ�� dy
			int xdiff1;		//��������һ���г� x �ƶ��Ĳ����� Ҳ�� dx
			int xunit1;		//���������ƶ��ķ���
			int count1;		//���������ƶ��Ĵ���

			int offset2;
			int errorterm2;
			int ydiff2;
			int xdiff2;
			int xunit2;
			int count2;

			//�������ʹ��
			int length; // length of horizontal line �����ĳ���
			int start;	//��ʼ����λ��

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
				//������ܶ��࣬��Ϊ����㷨Ҫ����͹����βſ���
				//��͹����δ���͵����ϣ�y�����ӵ�
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

				//���� Bresenham ���ƶ�����
				//������ y �ƶ����� x �� ���� x �ƶ����� y

				if (xdiff1 > ydiff1)//б��С��1
				{
					if (xdiff2 > ydiff2)
					{
						count1 = xdiff1;
						count2 = xdiff2;

						//���ݼ���0��ʱ��˵������һ���Ѿ������յ�
						//˵����������һ�����㣬˵���������£�б�ʷǳ��п��ܸı�
						while (count1 && count2)
						{
							//�������ļ����� y û��������ֱ������֮�����
							//Ŀ�����ҵ�׼���ı� y ���ٽ��
							while ((errorterm1 < xdiff1) && (count1 > 0))
							{
								if (count1--)//һ��ѭ����Ĵ�����1
								{
									offset1 += xunit1;
									xstart1 += xunit1; //��λ���ƶ�����Ӧ���ڴ�λ�ø�����Ӧ�ƶ�
								}

								errorterm1 += ydiff1; //���Ƿŵ�����������

								if (errorterm1 < xdiff1)//y ����������
								{
									//���Ķ�λ�����ڿ������ⲿ�ĵ�û����浽
									//���� plot      . . .  ->       <-    . . .
									//				plot	����ֱ�����	plot	
									//����Զ���򲻻�      
									surMemory[offset1] = color;
								}
							}
							//˵�� y Ҫ�ı��ˣ����Ҫ����
							errorterm1 -= xdiff1;

							//ͬ��
							//�������ļ����� y û��������ֱ������֮�����
							while ((errorterm2 < xdiff2) && (count2 > 0))
							{
								if (count2--)//һ��ѭ����Ĵ�����1
								{
									offset2 += xunit2;
									xstart2 += xunit2; //��λ���ƶ�����Ӧ���ڴ�λ�ø�����Ӧ�ƶ�
								}

								errorterm2 += ydiff2; //���Ƿŵ�����������

								if (errorterm2 < xdiff2)//y ����������
								{
									//
									surMemory[offset2] = color;
								}
							}
							//˵�� y Ҫ�ı��ˣ����Ҫ����
							errorterm2 -= xdiff2;

							//��ʱ������λ�㶼��һ�� ˮƽ����

							//����������λ��ľ���
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

							//���ˮƽ��
							for (int i = start; i <= start + length; i++)
							{
								surMemory[i] = color;
							}

							//y ���Ըı���
							offset1 += surLpitch;
							ystart1++;
							offset2 += surLpitch;
							ystart2++;
						}
					}
					else//���ڿ���б�ʴ���1
					{
						count1 = xdiff1;
						count2 = ydiff2;//y �Ĳ����ϴ�y ������ x

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

							//y �Ĳ������Ѿ�����y Ҫ�ı���ٽ��
							errorterm2 += xdiff2;

							//�ж� x �Ƿ�׼���ı�
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

							//y ���Ըı���
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
							//Ҳ�� y ���� x
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
								if (count2--)//һ��ѭ����Ĵ�����1
								{
									offset2 += xunit2;
									xstart2 += xunit2; //��λ���ƶ�����Ӧ���ڴ�λ�ø�����Ӧ�ƶ�
								}

								errorterm2 += ydiff2; //���Ƿŵ�����������

								if (errorterm2 < xdiff2)//y ����������
								{
									//
									surMemory[offset2] = color;
								}
							}
							//˵�� y Ҫ�ı��ˣ����Ҫ����
							errorterm2 -= xdiff2;

							//��ʱ������λ�㶼��һ�� ˮƽ����

							//����������λ��ľ���
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

							//���ˮƽ��
							for (int i = start; i <= start + length; i++)
							{
								surMemory[i] = color;
							}

							//y ���Ըı���
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
							//Ҳ�� y ���� x
							errorterm1 += xdiff1;

							if (errorterm1 >= ydiff1)
							{
								errorterm1 -= ydiff1;
								offset1 += xunit1;
								xstart1 += xunit1;
							}

							count1--;

							//y �Ĳ������Ѿ�����y Ҫ�ı���ٽ��
							errorterm2 += xdiff2;

							//�ж� x �Ƿ�׼���ı�
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

							//y ���Ըı���
							offset1 += surLpitch;
							ystart1++;
							offset2 += surLpitch;
							ystart2++;
						}
					}
				}

				//��λ�㵽�����յ�
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

				//�п���ͬʱ������Բ��� else
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