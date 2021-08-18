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
	while (r = a % b)        	//��շת���෨��a,b��������ֱ������Ϊ0 
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


int MToDec(char* a, int base)  	//��base��8��������aת����ʮ������ 
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
	int len = 0, r;          	//���result ���±꣬��d%m��ֵ     
	do
	{
		r = d % m;
		if (r >= 10)
			result[len++] = r - 10 + 'A';  	//����Ӧ������ת������ĸ�ַ�
		else
			result[len++] = r + '0';    	//����Ӧ������ת���������ַ�
		d /= m;
	} while (d != 0);
	result[len] = '\0';
	char t;
	for (int i = 0, j = len - 1; i < j; i++, j--)
	{//result �������� 
		t = result[i];  result[i] = result[j];  result[j] = t;
	}
}

int NToD(char* num, int n)
{
	int weight = 1, d = 0;	//weight��Ÿ�λ��Ȩֵ��num��Ӧ��ʮ������
	int len = strlen(num);            	//����num�ĳ��� 

	for (int i = len - 1; i >= 0; i--)    	//n������numת����ʮ������
	{
		if (num[i] >= 'a' && num[i] <= 'z')	//����ĸӳ��ɶ�Ӧ��ʮ������ 
			d += (num[i] - 'a' + 10) * weight;
		else if (num[i] >= 'A' && num[i] <= 'Z')
			d += (num[i] - 'A' + 10) * weight;
		else if (num[i] >= '0' && num[i] <= '9')
			d += (num[i] - '0') * weight;
		weight *= n;             	//������һλ��Ȩֵ��Ϊԭ����n�� 
	}

	return d;
}


void BinToHex(char* bins, char* hexs)
{
	int len;              	//��Ŷ�������bins�ĳ���	
	len = strlen(bins);     	//��bins�ĳ��� 
	int r = len % 4;	//��Ŷ�bins����������λһ�η��������γ���
	int   flag = 0;    	//flag��־r�Ƿ�Ϊ0��1��ʾ��Ϊ0��0��ʾ����Ϊ0
	if (r)  flag = 1;    	//flagΪ1��ʾ����β���4λ	
	int  i = 0, k = 0;
	while (bins[i])
	{
		int sum = 0;    	//��Ŷ�������4λһ�ζ�Ӧ��ʮ������
		if (flag)
		{//������γ��Ȳ���4λ�Ķ���������Ӧ��ʮ������ 
			while (r--)
			{
				sum = sum * 2 + bins[i] - '0';
				i++;
			}
			flag = 0;
		}
		else
		{//�󳤶�Ϊ4λ�Ķ���������Ӧ��ʮ������
			r = 4;
			while (bins[i] && r--)
			{
				sum = sum * 2 + bins[i] - '0';
				i++;
			}
		}
		//��sumת����ʮ�����ƶ�Ӧ���ַ�		
		if (sum >= 10) 	//ֵ���ڵ���10ʱӳ�����ĸA~F 
			hexs[k++] = sum - 10 + 'A';
		else       	//ֵС��10ʱӳ��ɶ�Ӧ�������ַ� 
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
	//x1�ǽ�С�ģ�x2�ǽϴ��
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

//�о���Ϊ��̣��³�������Ϊ�ź��򣬲��³������ź�����
void BubbleSort(int r[], int n)	//ð�ݷ��Ľ�����¼��󽻻�ʱ����ǰ��Ԫ���±�
{
	//Ϊ��֤��1��ɨ�赽���һ������Ԫ�أ�int lastSwapIndex��ʼ���ó�n-1 
	int lastSwapIndex = n - 1;  	//Ϊ�˹۲�ÿ�ֽ������i��¼����
	for (int i = 1; lastSwapIndex > 0; i++)
	{//������󽻻����ǵ�һ��Ԫ�غ͵ڶ���Ԫ��
	//��ʱ���ÿ�����һ�ֽ���ʱǰ��Ԫ���±꣬ÿ���ȼ���û�н���		
		int temp = -1;
		for (int j = 0; j < lastSwapIndex; j++)	//ÿ��ֻ��ɨ�赽������󽻻�λ�� 
		{
			if (r[j] > r[j + 1])    	//����Ԫ���������򽻻���temp = j
			{
				Swap(&r[j], &r[j + 1]);
				temp = j;
			}
		}
		lastSwapIndex = temp;  	//������󽻻�ǰ��Ԫ���±긳��lastSwapIndex 	
	}
}



template <typename T>
void Insert(T* r, int n, T x)
{
	int j = n - 1;
	while (j >= 0 && x < r[j])
	{ 		//���x<r[j],˵��xһ����r[j]ǰ�棬��Ҫ������ǰɨ�� 
		r[j + 1] = r[j];             	//���±�Ϊj��Ԫ������ƶ�һλ 
		j--;
	}
	r[j + 1] = x;      	//��x��������Ӧ���ڵ�λ��
}

void InsertSort(int* r, int n)
{
	int x;
	for (int i = 1; i < n; i++)
	{
		x = r[i]; 	//��r[i]λ��ʱ��ǰ��Ԫ�ؿ�������ƶ���������ʱ������r[i] 
		Insert(r, i, x);
	}
}


int Partition1(int a[], int left, int right)
{//���ҽ�����ȷ����׼ֵλ��
	int temp = a[left];   	//��Ż�׼ֵ 
	int i = left, j = right;
	int t;
	while (i < j)
	{	//i��j��ͬ
		//��������j��������ɨ����һ���Ȼ�׼ֵС����
		while (i < j && a[j] >= temp) j--;
		//Ȼ������i��������ɨ����һ���Ȼ�׼ֵ�����
		while (i < j && a[i] <= temp) i++;
		if (i < j)
		{ //���i��jû���������򽻻� i �� j ��ָ������ 		
			t = a[i];   a[i] = a[j];   a[j] = t;
		}
	}
	//���� left(��׼ֵ�±�) �� i ��ָ������ a[i] < ��׼��
	t = a[left];   a[left] = a[i];   a[i] = t;
	////����Ϊ�˸�������ģ�����
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
{//��ӷ�ȷ����׼ֵλ��
	int temp = a[left];  			//��Ż�׼ֵ 
	int i = left, j = right;
	while (i < j)
	{
		while (i < j && a[j] >= temp)    j--;
		if (i < j)
			a[i] = a[j];   			//��a[j]������У��µĿ����±�j�� 		
		while (i < j && a[i] <= temp)   i++;
		if (i < j)
			a[j] = a[i];   			//��a[i]������У��µĿ����±�i�� 
	}
	a[i] = temp;
	////����Ϊ�˸�������ģ�����
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
		int pivot = Partition1(a, left, right); 	//�����׼ֵ����λ��
		QuickSort(a, left, pivot - 1);  			//��׼ֵ��������������
		QuickSort(a, pivot + 1, right); 			//��׼ֵ�ұ������������
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
		ARRAY<int> a(len1), b(len2), c(maxLen + 1);//c Ҫ���Ľ�λ
		zero(a); zero(b); zero(c);
		for (int i = len1 - 1, int j = 0; i >= 0; i--)
		{
			a[j++] = result[i] - '0';
		}

		for (int i = len2 - 1, int j = 0; i >= 0; i--)
		{
			b[j++] = bigInt.result[i] - '0';
		}

		for (int i = 0; i < maxLen; i++)          	//������λ�����λ�����м���
		{
			c[i] += a[i] + b[i];	//��λ���
			c[i + 1] = c[i] / 10;                  	//iλ�Ľ�λ����ŵ�c[i+1]�� 
			c[i] %= 10;                          	//iλ��λ�����õ���				
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