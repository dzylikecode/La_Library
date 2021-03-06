#pragma once
#include <cmath>

typedef float  REAL;

//-------乘法的时候容易溢出------------
//FIX = real * scale
//so if fix1 * fix2 = real1 * real2 * scale^2 = fix3 * scale
//so if fix1 / fix2 = real1 / real2           = fix3 / scale
//-------注意要调整--------------------
//
//
//用于定点运算
//即同时将 float 和 int 同时放大2^16，存入整数当中
//用32位----高16存放 整数，低16存放 小数

typedef int FIXP16;

// fixed point mathematics constants
#define FIXP16_SHIFT     16				//位移，用来转化格式，整数放到高16位
#define FIXP16_MAG       65536			//乘法， 2^16
#define FIXP16_DP_MASK   0x0000ffff		//取出低16位
#define FIXP16_WP_MASK   0xffff0000		//取出高16位
#define FIXP16_ROUND_UP  0x00008000		//0.5用来四舍五入

//整数转化为定点数
#define INT_TO_FIXP(n)		(FIXP16((n) << FIXP16_SHIFT))
//浮点数转化为定点数
#define FLOAT_TO_FIXP(n)	(FIXP16( (float) (n) * (float) FIXP16_MAG + 0.5) )
#define FIXP_TO_FLOAT(n)	(FIXP16( (float) (n) / FIXP16_MAG ) )
//定点数转化为整数
#define FIXP_TO_INT(n)		(FIXP16((n) & FIXP16_WP_MASK))
//取出小数部分
#define FIXP_DEC_PART(n)	(FIXP16((n) & FIXP16_DP_MASK))
//四舍五入
#define  FIXP_TO_ROUND_UP_INT(n)   ((int)( (n) + FIXP16_ROUND_UP ) >> FIXP16_SHIFT )



#define SWAP(a,b,temp)	{temp = a; a = b; b = temp;}

#define PI         ((REAL)3.141592654)
#define PI2        ((REAL)6.283185307)
#define PI_DIV_2   ((REAL)1.570796327)
#define PI_DIV_4   ((REAL)0.785398163) 
#define PI_INV     ((REAL)0.318309886) 

// defines for small numbers
#define EPSILON_E3 (float)(1E-3) 
#define EPSILON_E4 (float)(1E-4) 
#define EPSILON_E5 (float)(1E-5)
#define EPSILON_E6 (float)(1E-6)

#define DEG_TO_RAD(ang) ((REAL)(ang)*PI/(REAL)180)
#define RAD_TO_DEG(rads) ((REAL)(rads)*(REAL)180/PI)
#define DEG_STARDAND(ang) ( ((ang%=360) < 0) ? (ang+360):ang)

//左闭右闭
#define RAND_RANGE(x,y) ( (x) + (rand()%((y)-(x) + 1)))


#ifndef min
#define min(a, b) ((a) < (b) ? (a) : (b))
#endif

#ifndef max
#define max(a, b) ((a) > (b) ? (a) : (b))
#endif

// storage for our lookup tables
// 360 是为了一些方便计算
extern REAL cos_look[361];
extern REAL sin_look[361];

inline REAL cos(int theta)
{
	return cos_look[theta];
}

inline REAL sin(int theta)
{
	return sin_look[theta];
}