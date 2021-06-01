#pragma once
#include <cmath>

typedef float  REAL;

//-------�˷���ʱ���������------------
//FIX = real * scale
//so if fix1 * fix2 = real1 * real2 * scale^2 = fix3 * scale
//so if fix1 / fix2 = real1 / real2           = fix3 / scale
//-------ע��Ҫ����--------------------
//
//
//���ڶ�������
//��ͬʱ�� float �� int ͬʱ�Ŵ�2^16��������������
//��32λ----��16��� ��������16��� С��

typedef int FIXPOINT;

// fixed point mathematics constants
#define FIXP16_SHIFT     16				//λ�ƣ�����ת����ʽ�������ŵ���16λ
#define FIXP16_MAG       65536			//�˷��� 2^16
#define FIXP16_DP_MASK   0x0000ffff		//ȡ����16λ
#define FIXP16_WP_MASK   0xffff0000		//ȡ����16λ
#define FIXP16_ROUND_UP  0x00008000		//0.5������������

//����ת��Ϊ������
#define INT_TO_FIXP(n)		(FIXPOINT((n) << FIXP16_SHIFT))

//������ת��Ϊ������
#define FLOAT_TO_FIXP(n)	(FIXPOINT( (float) (n) * FIXP16_MAG ) )

//������ת��Ϊ����
#define FIXP_TO_INT(n)		(FIXPOINT((n) & FIXP16_WP_MASK))
//ȡ��С������
#define FIXP_DEC_PART(n)	(FIXPOINT((n) & FIXP16_DP_MASK))
//��������
#define  FIXP_TO_ROUND_UP_INT(n)   ((int)( (n) + FIXP16_ROUND_UP ) >> FIXP16_SHIFT )



#define SWAP(a,b,temp)	{temp = a; a = b; b = temp;}

#define PI         ((REAL)3.141592654)
#define PI2        ((REAL)6.283185307)
#define PI_DIV_2   ((REAL)1.570796327)
#define PI_DIV_4   ((REAL)0.785398163) 
#define PI_INV     ((REAL)0.318309886) 

#define DEG_TO_RAD(ang) ((REAL)(ang)*PI/(REAL)180)
#define RAD_TO_DEG(rads) ((REAL)(rads)*(REAL)180/PI)
#define DEG_STARDAND(ang) ( ((ang%=360) < 0) ? (ang+360):ang)

#define RAND_RANGE(x,y) ( (x) + (rand()%((y)-(x)+1)))


// storage for our lookup tables
extern REAL cos_look[360];
extern REAL sin_look[360];

inline REAL cos(int theta)
{
	return cos_look[theta];
}

inline REAL sin(int theta)
{
	return sin_look[theta];
}