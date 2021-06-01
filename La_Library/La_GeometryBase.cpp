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
//��k < 1
//y = k*x + C
//k=dy/dx
//���� dy*x-dx*y+C=0
//���ǹ�դ�ߺ�ʵ���߶ε����
//����д��err = dy*x-dx*y+C
//��һ��������ֱ���ϵģ�err = 0
//���������һ���� -> x+1ʱ��yӦ��+��k=dy/dx����err += dy
//һ���� k<1,y+=0
//��һ���棬y���������ۻ� x+n��y Ӧ��+n*k��err+=n*dy
//n*k>1��ʱ�� yӦ��+= 1����Ӧ��ǡ���� err>dx
//�����Ŀ��Ǿ��� n*k>mʱ�� y+=m����Ӧ�ľ���err>m*dx-------�е�����ȡ������ͬ��
//
//Ⱦɫ
//err += dy
//if (error > dx)
//		error -= dx
//		y++
//
//���ڿ��� ��0.5���ֽ磬������1
//k = dy/dx
//ԭ���� err0 = k *x -    y + C -> err0 += k  ���ж� err0 > 1  ,�� err0-1
//ͬʱ����dx
//��Ӧ   err1 = dy*x - dx*y + C -> err0 += dy ���ж� err1 > dx ,�� err0-dx
//
//���ڿ��� ��0.5���ֽ磬������1
//err = k    * x -        y + C +0.5 -> err  += k    err > 1,    err - 1
//err0=0.5
//����2*dx
//err = 2*dy * x - 2*dx * y + CC     -> err  += 2*dy err > 2*dx  err  - 2*dx
//err0=dx

//ͨ��ƽ�ƿ��Ըı��ж���׼
//err0=dx					err > 2*dx
//err0=dx - 2*dx = -dx		err > 0
//û��Ҫ��err0��ʼ���ǣ���Ϊ��һ������
//����err1 = 2*dy - dx ��Ϊ��ʼֵ
	void DrawNotClipLine(int x0, int y0, int x1, int y1, COLORREF color, SURFACE* surface)
	{
		//��������Ӧ�����
		//�൱�ڽ���x0��y0������Ϊԭ��
		int surLpitch = surface->getLpitch();
		COLOR* vb_start = surface->getMemory() + x0 + y0 * surLpitch;

		//����������ϵ�µ�ֱ��
		int dx = x1 - x0;
		int dy = y1 - y0;

		//��������������
		int x_inc;
		int y_inc;
		//�ж����ޣ�ȷ��x��y���ƶ�����
		//����ȫ��ת������һ����
		if (dx >= 0)
		{
			x_inc = 1;
		}
		else
		{
			x_inc = -1;
			dx = -dx;//ת������һ���޴���
			//dx ���Ǿ�������ƶ��Ĳ���
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

		//�����̻���դ�ߺ�ʵ���߶ε����
		int error;

		if (dx > dy)//б��С��1
		{
			//�ڶ���������
			error = dy2 - dx;

			//һ��Ҫ�ƶ�dx�£�Ⱦdx����
			for (int i = 0; i <= dx; i++)
			{
				//��һ����ֱ��Ⱦɫ
				*vb_start = color;

				//���ǵڶ������λ��
				//x �ƶ�
				vb_start += x_inc;
				//y �Ƿ���Ҫ�ƶ�
				if (error >= 0)
				{
					error -= dx2;
					vb_start += y_inc;
				}

				//������������
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
		//���Ȼ��ֲü�����
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


		//���ü��������
		int xc1 = x1;
		int yc1 = y1;
		int xc2 = x2;
		int yc2 = y2;

		//�����ж����������
		int p1_code = 0;
		int p2_code = 0;

		//��ʼ�ж�
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

		//���ڵڶ�����
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
		//��������ͬ������ ���������㣬��Ϊ0
		if (p1_code & p2_code)//�ų��˲��� ��ȫ�������ֱ��
		{
			//����  \\ �����������޷��ų�,�е���ȫ���⣬�еĲ�������
			return false;
		}

		if (p1_code == 0 && p2_code == 0)//�������ģ����账��
		{
			return true;
		}

		//determine end clip point for p1
		switch (p1_code)
		{
		case CLIP_CODE_C: break;

			//�Ͷ���ֱ���ཻ
		case CLIP_CODE_N:
		{
			yc1 = min_clip_y;
			//���ᷢ��y1 == y2 ���µ������Ҫ��Ȼ������������
			//+0.5�������壬ǰ��������������+0.5����ת�����������ض�
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
			//�ȼ��轻�ڱ���
			yc1 = min_clip_y;
			xc1 = x1 + (yc1 - y1) * (x2 - x1) / (y2 - y1);

			//�����Ƿ񽻵���������
			if (xc1 > max_clip_x)
			{
				//�������⣬˵�������Ľ����ڶ���
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
		//�ٴμ��飬��Ϊ�����ٲ��������������
		if ((xc1 < min_clip_x) || (xc1 > max_clip_x) ||
			(yc1 < min_clip_y) || (yc1 > max_clip_y) ||
			(xc2 < min_clip_x) || (xc2 > max_clip_x) ||
			(yc2 < min_clip_y) || (yc2 > max_clip_y))
		{
			return false;
		}

		//�޸�����ֵ
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


	//�㷨
//����ƽ�������Σ���
//һ��һ��ˮƽ�ػ滭���������£���Ҫ�� y ��
//ÿ�����Ƶ�ʱ�򣬼��� x1 ��x2 �仯�˶��٣����е���
//Ȼ���ٻ滭
//������ x1 ��x2 �仯�����Ի��� Bresenham �������뷨
//�Ӷ��㿪ʼ��x0��y0������ʼ y = y0�� xLeft = xRight = x0
//���Ƶ�ʱ�� xLeft += dx1�� xRight += dx2
//
//Ϊ�˷��㣬���ø�����㣬Ҳ���ǲ�ת��Ϊ������ֱ����ʵ������¼�ۼ���
//
//
/// <summary>
/// ˳�������������  ���� 1 ���� 2 ���� 3
/// ƽ�������Σ��� ���ѿ���������
///			(x3,y3)
/// 
/// (x1,y1)			(x2,y2)
/// </summary>

/// <param name="color"></param>
	void FillBottomTri(int x1, int y1, int x2, int y2, int x3, int y3, COLOR color, SURFACE* surface)
	{
		//surface = CHECK_SURFACE(surface);  //ֻҪ�����Ǹ������Ϳ�
		//���ȱ�֤ x1 < x2
		if (x2 < x1)
		{
			int temp = x2;
			x2 = x1;
			x1 = temp;
		}

		//�����ƶ���б��
		REAL height = y3 - y1;
		REAL dx_left = (x3 - x1) / height;
		REAL dx_right = (x3 - x2) / height;

		//���
		REAL xs = (REAL)x1;
		REAL xe = (REAL)x2 + (REAL)+0.5;//����һ�¶˵�

		//�ֽ�Ϊһ��һ����ˮƽֱ��
		//�ü�
		//���ȿ��� y �������ڲü����� H line
		//Ȼ���ٿ��� H Line �Ƿ��� x �����ڣ����вü�

		if (y1 < min_clip_y)
		{
			//�������
			xs += dx_left * (REAL)(min_clip_y - y1);
			xe += dx_right * (REAL)(min_clip_y - y1);

			y1 = min_clip_y;
		}

		if (y3 > max_clip_y)
		{
			//�����յ�
			y3 = max_clip_y;
		}

		//�ƶ����ڴ����
		int surLpitch = surface->getLpitch();
		COLOR* dest_addr = surface->getMemory() + y1 * surLpitch;

		//ˮƽֱ�߶��ڲü�������
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
		else//��Ҫ�ж�ˮƽ���Ƿ��ڲü�������
		{
			int left;
			int right;

			for (int i = y1; i <= y3; i++, dest_addr += surLpitch)
			{
				left = (int)xs;
				right = (int)xe;

				//����continue��Ե�ʣ������⣬��������ѭ��
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
/// Ҫ���������������䶨��Ϊ �� ���ɣ�
/// ���ϵ�����		���� 1 ���� 2 ���� 3
/// ������  ��		��������
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

			//��Ҳ��������⣬�����أ���Ϊcontinue��Ե�ʣ�Ҳ���ܷ�����
			for (int i = y1; i <= y3; i++, dest_addr += surLpitch)
			{
				left = (int)xs;
				right = (int)xe;

				//���� continue ��Ե�ʣ�������
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
/// ���ö�������
/// �������Windows����ϵ��������
/// </summary>
	void FillTopTriFP(int x1, int y1, int x2, int y2, int x3, int y3, COLOR color, SURFACE* surface)
	{
		//�ų����㹲��
		if (y1 == y3 && y2 == y3)
		{
			return;
		}

		//���ȱ�֤ x2 < x3
		if (x3 < x2)
		{
			int temp = x2;
			x2 = x3;
			x3 = temp;
		}


		//�����ƶ���б��
		int height = y3 - y1;
		FIXPOINT dx_left = (INT_TO_FIXP(x2 - x1)) / height;
		FIXPOINT dx_right = (INT_TO_FIXP(x3 - x1)) / height;

		//���
		//���Ǵ���͵㿪ʼ
		FIXPOINT xs = INT_TO_FIXP(x1);
		FIXPOINT xe = INT_TO_FIXP(x1);// + (REAL)+0.5;//����һ�¶˵�

		//�ֽ�Ϊһ��һ����ˮƽֱ��
		//�ü�
		//���ȿ��� y �������ڲü����� H line
		//Ȼ���ٿ��� H Line �Ƿ��� x �����ڣ����вü�

		if (y1 < min_clip_y)
		{
			//�������
			//��������ʹȻ
			xs += dx_left * (min_clip_y - y1);
			xe += dx_right * (min_clip_y - y1);

			y1 = min_clip_y;
		}

		if (y3 > max_clip_y)
		{
			//�����յ�
			y3 = max_clip_y;
		}

		//�ƶ����ڴ����
		int surLpitch = surface->getLpitch();
		COLOR*dest_addr = surface->getMemory() + y1 * surLpitch;

		//ˮƽֱ�߶��ڲü�������
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
		else//��Ҫ�ж�ˮƽ���Ƿ��ڲü�������
		{
			int left;
			int right;

			for (int i = y1; i <= y3; i++, dest_addr += surLpitch)
			{
				left = FIXP_TO_ROUND_UP_INT(xs);
				right = FIXP_TO_ROUND_UP_INT(xe);

				//���� continue ��Ե�ʣ�������
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
				//�����Ѿ�ת��Ϊ int �� ���Ƕ�����
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

		//���ȱ�֤ x1 < x2
		if (x2 < x1)
		{
			int temp = x2;
			x2 = x1;
			x1 = temp;
		}

		//�����ƶ���б��
		int height = y3 - y1;
		FIXPOINT dx_left = (INT_TO_FIXP(x3 - x1)) / height;
		FIXPOINT dx_right = (INT_TO_FIXP(x3 - x2)) / height;

		//���
		FIXPOINT xs = INT_TO_FIXP(x1);
		FIXPOINT xe = INT_TO_FIXP(x2);// + (REAL)+0.5;//����һ�¶˵�

		//�ֽ�Ϊһ��һ����ˮƽֱ��
		//�ü�
		//���ȿ��� y �������ڲü����� H line
		//Ȼ���ٿ��� H Line �Ƿ��� x �����ڣ����вü�

		if (y1 < min_clip_y)
		{
			//�������
			//��������ʹȻ
			xs += dx_left * (min_clip_y - y1);
			xe += dx_right * (min_clip_y - y1);

			y1 = min_clip_y;
		}

		if (y3 > max_clip_y)
		{
			//�����յ�
			y3 = max_clip_y;
		}

		//�ƶ����ڴ����
		int surLpitch = surface->getLpitch();
		COLOR* dest_addr = surface->getMemory() + y1 * surLpitch;

		//ˮƽֱ�߶��ڲü�������
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
		else//��Ҫ�ж�ˮƽ���Ƿ��ڲü�������
		{
			int left;
			int right;

			for (int i = y1; i <= y3; i++, dest_addr += surLpitch)
			{
				left = FIXP_TO_ROUND_UP_INT(xs);
				right = FIXP_TO_ROUND_UP_INT(xe);

				//���� continue ��Ե�ʣ�������
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
	/// �����������������
	/// ���ڷ�ƽ����������
	/// ��������Ϊ p1 p2 p3��
	/// Ȼ���� p2 ˮƽ�ֳ�����������
	/// ���ɼ���
	/// </summary>
	void FillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, COLOR color, SURFACE* surface)
	{
		surface = INCLUDE_NULL_SURFACE(surface);
		//�˻���ʽ
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

		if (y1 == y2) //����ƽ��
		{
			FillBottomTri(x1, y1, x2, y2, x3, y3, color, surface);
		}
		else if (y2 == y3) //����ƽ��
		{
			FillTopTri(x1, y1, x2, y2, x3, y3, color, surface);
		}
		else//����Ӳ��
		{
			int new_x = x1 + (int)(0.5 + (REAL)(y2 - y1) * (REAL)(x3 - x1) / (REAL)(y3 - y1));

			FillTopTri(x1, y1, new_x, y2, x2, y2, color, surface);
			FillBottomTri(x2, y2, new_x, y2, x3, y3, color, surface);
		}
	}


	void FillTriangleFP(int x1, int y1, int x2, int y2, int x3, int y3, COLOR color, SURFACE* surface)
	{
		surface = INCLUDE_NULL_SURFACE(surface);
		//�˻���ʽ
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


















