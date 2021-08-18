#include <math.h>
#include <string>
typedef float REAL;

inline REAL CalTangleArea(REAL a, REAL b, REAL c)
{
	REAL s = (a + b + c) / 2;
	return sqrt(s * (s - a) * (s - b) * (s - c));
}


//Greatest Common Divisor
int Gcd(int a, int b)
{
	if (!a) return b;
	if (!b) return a;
	int r;
	a = abs(a); b = abs(b);
	while (r = a % b)        	//用辗转求余法求a,b的余数，直到余数为0 
	{
		a = b;
		b = r;
	}
	return b;
}

inline int Gcm(int a, int b)
{
	if (!a && !b) return 0;
	return a * b / Gcd(a, b);
}

int CalNumOfDigital(int m)
{
	int n = 0;
	do
	{
		m /= 10; n++;
	} while (m != 0);
	return n;
}

template <typename T>
inline T sign(T n)
{
	return (n > 0 ? 1 : (n < 0 ? -1 : 0));
}


int MToDec(char* a, int base)  	//将base（8）进制数a转换成十进制数 
{
	int sum = 0;
	int len = strlen(a);

	for (int i = 0; i < len; i++)
	{
		sum = sum * base + a[i] - '0';
	}
	return sum;
}

void DToM(int d, int m, char* result)
{
	int len = 0, r;          	//存放result 的下标，及d%m的值     
	do
	{
		r = d % m;
		if (r >= 10)
			result[len++] = r - 10 + 'A';  	//将对应的整数转换成字母字符
		else
			result[len++] = r + '0';    	//将对应的整数转换成数字字符
		d /= m;
	} while (d != 0);
	result[len] = '\0';
	char t;
	for (int i = 0, j = len - 1; i < j; i++, j--)
	{//result 数组逆置 
		t = result[i];  result[i] = result[j];  result[j] = t;
	}
}

int NToD(char* num, int n)
{
	int weight = 1, d = 0;	//weight存放各位的权值及num对应的十进制数
	int len = strlen(num);            	//计算num的长度 

	for (int i = len - 1; i >= 0; i--)    	//n进制数num转换成十进制数
	{
		if (num[i] >= 'a' && num[i] <= 'z')	//将字母映射成对应的十进制数 
			d += (num[i] - 'a' + 10) * weight;
		else if (num[i] >= 'A' && num[i] <= 'Z')
			d += (num[i] - 'A' + 10) * weight;
		else if (num[i] >= '0' && num[i] <= '9')
			d += (num[i] - '0') * weight;
		weight *= n;             	//向左移一位，权值变为原来的n倍 
	}

	return d;
}


void BinToHex(char* bins, char* hexs)
{
	int len;              	//存放二进制数bins的长度	
	len = strlen(bins);     	//求bins的长度 
	int r = len % 4;	//存放对bins从右向左四位一段分组后最左段长度
	int   flag = 0;    	//flag标志r是否为0，1表示不为0，0表示余数为0
	if (r)  flag = 1;    	//flag为1表示最左段不足4位	
	int  i = 0, k = 0;
	while (bins[i])
	{
		int sum = 0;    	//存放二进制数4位一段对应的十进制数
		if (flag)
		{//求最左段长度不足4位的二进制数对应的十进制数 
			while (r--)
			{
				sum = sum * 2 + bins[i] - '0';
				i++;
			}
			flag = 0;
		}
		else
		{//求长度为4位的二进制数对应的十进制数
			r = 4;
			while (bins[i] && r--)
			{
				sum = sum * 2 + bins[i] - '0';
				i++;
			}
		}
		//将sum转换成十六进制对应的字符		
		if (sum >= 10) 	//值大于等于10时映射成字母A~F 
			hexs[k++] = sum - 10 + 'A';
		else       	//值小于10时映射成对应的数字字符 
			hexs[k++] = sum + '0';
	}
	hexs[k] = '\0';
}


#include <complex>
class QuadraticEquation
{
private:
	REAL a, b, c;
public:
	void set(REAL outA, REAL outB, REAL outC) { a = outA; b = outB; c = outC; }
	REAL delta() { return b * b - 4 * a * c; }
	//x1是较小的，x2是较大的
	bool getRealRoot(REAL& x1, REAL& x2)
	{
		REAL tempDelta = delta();
		if (tempDelta >= 0)
		{
			x1 = (-b - sqrt(tempDelta)) / (2 * a);
			x2 = (-b + sqrt(tempDelta)) / (2 * a);
			return true;
		}
		return false;
	}
	bool getRoot(std::complex<REAL>& x1, std::complex<REAL>& x2)
	{
		REAL tempDelta = delta();
		if (tempDelta >= 0)
		{
			x1 = (-b - sqrt(tempDelta)) / (2 * a);
			x2 = (-b + sqrt(tempDelta)) / (2 * a);
			return true;
		}
		else
		{
			x1.real(-b / (2 * a)); x1.imag(-sqrt(-tempDelta) / (2 * a));
			x2.real(-b / (2 * a)); x2.imag(sqrt(-tempDelta) / (2 * a));
		}
		return false;
	}
};



template<typename T>
inline void Swap(T* pa, T* pb)
{
	T temp;
	temp = *pa;
	*pa = *pb;
	*pb = temp;
}

//感觉更为深刻，下沉代表了为排好序，不下沉代表排好了序
void BubbleSort(int r[], int n)	//冒泡法改进：记录最后交换时相邻前面元素下标
{
	//为保证第1轮扫描到最后一对相邻元素，int lastSwapIndex初始设置成n-1 
	int lastSwapIndex = n - 1;  	//为了观察每轮结果，用i记录轮数
	for (int i = 1; lastSwapIndex > 0; i++)
	{//上轮最后交换不是第一个元素和第二个元素
	//临时存放每轮最后一轮交换时前面元素下标，每轮先假设没有交换		
		int temp = -1;
		for (int j = 0; j < lastSwapIndex; j++)	//每轮只需扫描到上轮最后交换位置 
		{
			if (r[j] > r[j + 1])    	//相邻元素如逆序则交换，temp = j
			{
				Swap(&r[j], &r[j + 1]);
				temp = j;
			}
		}
		lastSwapIndex = temp;  	//本轮最后交换前面元素下标赋给lastSwapIndex 	
	}
}



template <typename T>
void Insert(T* r, int n, T x)
{
	int j = n - 1;
	while (j >= 0 && x < r[j])
	{ 		//如果x<r[j],说明x一定在r[j]前面，需要继续向前扫描 
		r[j + 1] = r[j];             	//将下标为j的元素向后移动一位 
		j--;
	}
	r[j + 1] = x;      	//将x放置在它应该在的位置
}

void InsertSort(int* r, int n)
{
	int x;
	for (int i = 1; i < n; i++)
	{
		x = r[i]; 	//找r[i]位置时，前面元素可能向后移动，需用临时变量存r[i] 
		Insert(r, i, x);
	}
}


int Partition1(int a[], int left, int right)
{//左右交换法确定基准值位置
	int temp = a[left];   	//存放基准值 
	int i = left, j = right;
	int t;
	while (i < j)
	{	//i和j不同
		//必须先是j从右向左扫描找一个比基准值小的数
		while (i < j && a[j] >= temp) j--;
		//然后再是i从左向右扫描找一个比基准值大的数
		while (i < j && a[i] <= temp) i++;
		if (i < j)
		{ //如果i和j没有相遇，则交换 i 和 j 所指的数据 		
			t = a[i];   a[i] = a[j];   a[j] = t;
		}
	}
	//交换 left(基准值下标) 和 i 所指的数据 a[i] < 基准的
	t = a[left];   a[left] = a[i];   a[i] = t;
	////下面为了给出各趟模拟过程
	//printf("[ ");
	//for (int j = left; j < i; j++)
	//	printf("%d ", a[j]);
	//printf("] ");
	//printf("%d ", a[i]);
	//printf("[ ");
	//for (int j = i + 1; j <= right; j++)
	//	printf("%d ", a[j]);
	//printf("] \n");
	return i;
}
int Partition2(int a[], int left, int right)
{//填坑法确定基准值位置
	int temp = a[left];  			//存放基准值 
	int i = left, j = right;
	while (i < j)
	{
		while (i < j && a[j] >= temp)    j--;
		if (i < j)
			a[i] = a[j];   			//将a[j]放入坑中，新的坑是下标j处 		
		while (i < j && a[i] <= temp)   i++;
		if (i < j)
			a[j] = a[i];   			//将a[i]放入坑中，新的坑是下标i处 
	}
	a[i] = temp;
	////下面为了给出各趟模拟过程
	//printf("[ ");
	//for (int j = left; j < i; j++)
	//	printf("%d ", a[j]);
	//printf("] ");
	//printf("%d ", a[i]);
	//printf("[ ");
	//for (int j = i + 1; j <= right; j++)
	//	printf("%d ", a[j]);
	//printf("] \n");

	return i;
}

void QuickSort(int a[], int left, int right)
{
	if (left < right)
	{
		int pivot = Partition1(a, left, right); 	//计算基准值所在位置
		QuickSort(a, left, pivot - 1);  			//基准值左边区间快速排序
		QuickSort(a, pivot + 1, right); 			//基准值右边区间快速排序
	}
}

#include <string>
#include <cmath>
#include "La_LinearList.h"
class BIG_INTEGER
{
private:
	std::string result;
	void zero(ARRAY<int>& a)
	{
		for (int i = 0; i < a.getSize(); i++)
		{
			a[i] = 0;
		}
	}
public:
	void create(const char* num) { result = num; }
	BIG_INTEGER& operator=(const char* num)
	{
		create(num);
		return *this;
	}
	BIG_INTEGER() {};
	BIG_INTEGER(const BIG_INTEGER& bigInt)
	{
		result = bigInt.result;
	}
	operator const char* () { return result.c_str(); }
	BIG_INTEGER operator+(const BIG_INTEGER& bigInt)
	{
		int len1 = result.length(); int len2 = bigInt.result.length();
		int maxLen = std::max(len1, len2);
		ARRAY<int> a(len1), b(len2), c(maxLen + 1);//c 要担心进位
		zero(a); zero(b); zero(c);
		for (int i = len1 - 1, int j = 0; i >= 0; i--)
		{
			a[j++] = result[i] - '0';
		}

		for (int i = len2 - 1, int j = 0; i >= 0; i--)
		{
			b[j++] = bigInt.result[i] - '0';
		}

		for (int i = 0; i < maxLen; i++)          	//按加数位数多的位数进行计算
		{
			c[i] += a[i] + b[i];	//逐位相加
			c[i + 1] = c[i] / 10;                  	//i位的进位数存放到c[i+1]上 
			c[i] %= 10;                          	//i位进位后所得的数				
		}
		BIG_INTEGER temp;
		temp.result.resize(c[maxLen] ? maxLen : maxLen - 1);
		for (int i = temp.result.size(), int j = 0; i >= 0; i--)
		{
			temp.result[j++] = c[i] + '0';
		}

		return temp;
	}

};