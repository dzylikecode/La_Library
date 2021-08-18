
typedef float REAL;


#include <stdio.h>

const int N = 10;

int main(void)
{
	int a[N];              				//定义有N个整数元素的数组a
	int n;
	scanf("%d", &n);       				//输入整数个数
	for (int i = 0; i < n; i++)   		//输入数组a的n个元素 
		scanf("%d", &a[i]);  			//输入元素a[i], 注意它是a的第i+1个元素 
	for (int i = 0; i < n / 2; i++)  			//逆置，距离首（a[0]）和尾（a[n-1]）相等的元素交换其值
	{
		int t;
		t = a[i];  a[i] = a[n - 1 - i];  a[n - 1 - i] = t;	//交换a[i]与a[n-1-i]的值 
	}
	for (int i = 0; i < n; i++)    			//输出a的n个元素 
		printf("%d  ", a[i]);

	return 0;
}


#include <stdio.h>

void SelectSort(int r[], int n);    		//用选择排序思想对数组a中的n个元素排序  
const int N = 6;

int main(void)
{
	int a[N];                      		//①定义有N个元素的数组 a 		
	printf("输入%d个整数：\n", N);   		//提示输入N个元素 
	for (int i = 0; i < N; i++)          	//②输入数组的N个元素  		
		scanf("%d", &a[i]);
	SelectSort(a, N);             		//③调用函数对数组a进行排序

	printf("排序后结果为：");
	for (int i = 0; i < N; i++)        	//④输出排序后的数组元素
		printf("%d ", a[i]);
	printf("\n");

	return 0;
}
void SelectSort(int r[], int n)
{
	int i, j, mink, m;           			//mink存放最小元素的下标 
	for (i = 0; i < n - 1; i++)
	{
		mink = i;           				//在r[i]~r[n-1]中找最元素，用mink记录最小元素下标 
		for (j = i + 1; j < n; j++)
		{
			if (r[mink] > r[j])  mink = j;
		}
		if (mink != i)      		//如果a[i]元素不是r[i]~r[n-1]中的最小元素，则交换a[mink],a[i] 
		{
			int t;
			t = r[i];  r[i] = r[mink];  r[mink] = t;
		}
	}
}


#include <stdio.h>
#include <stdlib.h>

int zhenfen(int m, int n);//m和n能构成真分数返回1，否则返回0 
int main()
{
	int n, * a, i, j, k;

	scanf("%d", &n);
	a = (int*)malloc(sizeof(int) * n);
	for (i = 0; i < n; i++)
	{
		scanf("%d", &a[i]);
	}
	k = 0;
	for (i = 0; i < n - 1; i++)
	{
		for (j = i + 1; j < n; j++)
		{
			if (zhenfen(a[i], a[j])) k++;
		}
	}
	printf("%d\n", k);
	free(a);

	return 0;
}
int zhenfen(int m, int n)
{
	int r;
	while (m % n)
	{
		r = m % n;
		m = n;
		n = r;
	}
	if (n == 1) return 1;
	return 0;
}


#include <stdio.h>

int main(void)
{
	FILE* fp;
	char inputfile[20];
	printf("请输入要打开文件的名字：");
	scanf("%s", inputfile);      	//输入文件名 
	fp = fopen(inputfile, "r");   	//以只读方式打开文本文件  
	if (fp == NULL)         	//文件打开失败，直接退出程序 
	{
		printf("\n%s打开失败！\n", inputfile);
		return 0;
	}
	printf("%s 文件内容：\n", inputfile);
	while (!feof(fp))        	//当文件未结束时 
		putchar(fgetc(fp));  	//从文件中读取字符并显示 

	char ch;
	while ((ch = fgetc(fp)) != EOF)
		putchar(ch);
	printf("\n");
	fclose(fp);           	//关闭文件 

	return 0;
}


#include <stdio.h>
void WriteFile(FILE* fp);	//向文件里写内容
void ReadFile(FILE* fp);          	//从文件里读内容  

int main(void)
{
	FILE* fp;
	char inputfile[30];

	printf("请输入文件名: ");
	scanf("%s", inputfile);              	//输入要打开的文件名     
	fp = fopen(inputfile, "w+");
	if (fp == NULL)
	{
		printf("打开文件失败");
		return 0;
	}
	else
	{
		printf("%s 打开成功\n", inputfile);
		WriteFile(fp);  	//调用函数把字符写入文件    
		ReadFile(fp);  	//调用函数读文件内容并显示   
		fclose(fp);  	//关闭文件
	}
	return 0;
}
void WriteFile(FILE* fp)	//向文件里写内容 
{
	char ch;

	printf("请输入文件内容，以#结束！\n");
	while ((ch = getchar()) != '#')     	//从键盘读入字符，直到读入#循环结束 
	{
		fputc(ch, fp);  	//向fp所指文件里写字符ch		
	}
	rewind(fp);                	//将文件指针定位于文件开头 
}

void ReadFile(FILE* fp)        	//从文件里读内容 
{
	char ch;
	printf("读取文件内容：\n");
	ch = fgetc(fp);           	//从文件读取字符 
	while (ch != EOF)
	{
		putchar(ch);  	//显示字符 
		ch = fgetc(fp);  	//从文件读取下一个字符 
	}
	printf("\n文件读取结束");
}


#include <stdio.h>

int main(void)
{
	FILE* fp;
	char str[100];
	printf("请输入一个字符串：\n");
	gets(str);	//输入要写到文件中的字符串	
	char fileName[20]; 	//定义存放文件名的字符数组
	printf("请输入文件所在路径及名称：\n");
	scanf("%s", fileName);	//输入文件所在路径及名称
	if ((fp = fopen(fileName, "a")) == NULL)	//以追加方式打开指定文件     
	{
		printf("打开失败!");
		return  0;
	}
	fputs(str, fp); 	//把字符数组str中的字符串写到fp指向的文件
	fclose(fp);
	if ((fp = fopen(fileName, "r")) != NULL)
	{
		printf("%s 文件内容：\n", fileName);
		while (fgets(str, sizeof(str), fp)) 	//从fp所指的文件中读取字符串存入str中 
			printf("%s", str); 	//将字符串输出 
		fclose(fp);              	//关闭文件
	}

	return 0;
}



#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	FILE* in, * out;	//定义两个FILE类型的指针变量
	char infile[50], outfile[50];     	//分别存放源文件和目标文件名 
	char s[256];
	printf("请输入源文件名:");
	scanf("%s", infile);	//输入源文件所在路径及名称
	printf("请输入目标文件名:");
	scanf("%s", outfile);	//输入目标文件所在路径及名称
	if ((in = fopen(infile, "r")) == NULL)	//以只读方式打开指定文件
	{
		printf("打开文件%s失败\n", infile);
		exit(0);                               	//退出程序
	}
	if ((out = fopen(outfile, "w")) == NULL) 	//以只写方式打开指定文件
	{
		printf("不能建立%s文件\n", outfile);
		exit(0);                                	//退出程序
	}
	while (fgets(s, 256, in)) 	//将in指向的文件的内容复制到out所指向的文件中
		fputs(s, out);
	printf("文件复制完成\n");
	//关闭文件
	fclose(in);
	fclose(out);

	return 0;
}


#include<stdio.h>

typedef struct Rec	//定义结构体类型，使用typedef定义Rec为结构体类型名 
{
	char id[10];
	char name[20];
	float price;
	int  count;
}Rec;
void WriteFile(FILE* fp, int n);  	//将键盘输入的商品信息写到文件
void ReadFile(FILE* fp, int n);  	//从文件读出商品信息 

int main(void)
{
	char filename[20];
	int n;
	FILE* fp;
	printf("请输入目标文件:\n");
	scanf("%s", filename);
	fp = fopen(filename, "w+");  	//以文本读写方式打开文件	
	if (fp == NULL)
	{
		printf("打开文件失败");
		return 0;
	}
	printf("请输入商品数量:\n");
	scanf("%d", &n);  	//从键盘输入
	WriteFile(fp, n);
	ReadFile(fp, n);
	fclose(fp); 	//关闭文件 

	return 0;
}
void WriteFile(FILE* fp, int n)      	//将键盘输入的商品信息写到文件 
{
	int i;
	Rec record;
	printf("***********请输入商品数据***********\n");
	for (i = 1; i <= n; i++)            	//从键盘输入商品信息 
	{
		printf("请输入序号:");
		scanf("%s", record.id);
		printf("请输入名称:");
		scanf("%s", record.name);
		printf("请输入价格：");
		scanf("%f", &record.price);
		printf("请输入数量：");
		scanf("%d", &record.count);
		printf("\n");
		//写入文件
		fprintf(fp, "%s %s %5.2f %d\n", record.id, record.name, record.price, record.count);
	}
}

void ReadFile(FILE* fp, int n)        	//从文件读出商品信息 
{
	Rec record;
	double total = 0;
	rewind(fp);              	//把文件内部的位置指针移到文件首
	while (fscanf(fp, "%s %s %f %d\n", record.id, record.name, &record.price, &record.count) != EOF)
	{//输出数据
		printf("序号:%s 名称:%s 价格:%5.2f 数量:%d \n", record.id, record.name,
			record.price, record.count);
		total = total + record.price * record.count;
	}
	printf("合计:%5.2f\n", total);
}


#include<stdio.h>
#include <stdlib.h>

typedef struct Rec	//定义结构体类型，使用typedef定义Rec为结构体类型名 
{
	char id[10];
	char name[20];
	float price;
	int  count;
}Rec;
void WriteFile(FILE* fp, int n);  	//将键盘输入的商品信息写到文件
void ReadFile(FILE* fp, int n);  	//从文件读出商品信息 

int main(void)
{
	char filename[20];
	int n;
	FILE* fp;
	printf("请输入目标文件:\n");
	scanf("%s", filename);
	fp = fopen(filename, "wb+");  	//以二进制读写方式打开文件	
	if (fp == NULL)
	{
		printf("打开文件失败");
		exit(1);
	}
	printf("请输入商品数量:\n");
	scanf("%d", &n); 	//从键盘输入
	WriteFile(fp, n);
	ReadFile(fp, n);
	fclose(fp); 	//关闭文件 

	return 0;
}
void WriteFile(FILE* fp, int n)      	//将键盘输入的商品信息写到文件 
{
	int i;
	Rec record;

	printf("***********请输入商品数据***********\n");
	for (i = 1; i <= n; i++)                    	//从键盘输入商品信息 
	{
		printf("请输入序号:");
		scanf("%s", record.id);
		printf("请输入名称:");
		scanf("%s", record.name);
		printf("请输入价格：");
		scanf("%f", &record.price);
		printf("请输入数量：");
		scanf("%d", &record.count);
		printf("\n");
		fwrite(&record, sizeof(Rec), 1, fp); 	//以块的方式写入文件
	}

}
void ReadFile(FILE* fp, int n)        	//从文件读出商品信息 
{
	Rec record;
	double total = 0;

	rewind(fp);                	//把文件内部的位置指针移到文件首    
	while (fread(&record, sizeof(Rec), 1, fp)) 	//以块的方式读取文件中的数据
	{
		printf("序号:%s 名称:%s 价格:%5.2f 数量:%d \n", record.id, record.name,
			record.price, record.count);  			//输出数据 
		total = total + record.price * record.count;
	}
	printf("合计:%5.2f\n", total);
}


#include<stdio.h>
#include <stdlib.h>

typedef struct Rec	//定义结构体类型，使用typedef定义Rec为结构体类型名 
{
	char id[10];
	char name[20];
	float price;
	int  count;
}Rec;

int main(void)
{
	char filename[20]; 	//存放商品信息文件名    
	FILE* fp;
	int m;    	//存放要读取的商品记录号 
	Rec record;

	printf("请输入商品信息文件:\n");
	scanf("%s", filename);
	fp = fopen(filename, "rb"); 	//以文本读写方式打开文件	
	if (fp == NULL)
	{
		printf("打开文件失败");
		exit(1);
	}
	while (fread(&record, sizeof(Rec), 1, fp))
	{
		printf("%s 名称:%s 价格:%5.2f 数量:%d \n", record.id, record.name,
			record.price, record.count);
	}
	printf("请输入要读取的商品记录号:\n");
	scanf("%d", &m); 	//从键盘输入
	fseek(fp, (m - 1) * sizeof(Rec), 0); 	//将文件位置指针移到第m个商品信息位置
	fread(&record, sizeof(Rec), 1, fp);
	printf("第%d条记录\n序号:%s 名称:%s 价格:%5.2f 数量:%d \n", m, record.id,
		record.name, record.price, record.count);
	fclose(fp); 	//关闭文件 

	return 0;
}


#include <stdio.h>
typedef struct Student
{
	char sno[15];
	char name[20];
	int score;
}Student;

int main(void)
{
	int n;
	char filename[20];
	FILE* fp;

	printf("请输入要写入和读出数据的文件名：");
	scanf("%s", filename);
	fp = fopen(filename, "wb+");
	printf("请输入学生数：");
	scanf("%d", &n);
	Student s;
	printf("输入%d个学生的信息：学号 姓名 成绩：\n", n);
	for (int i = 1; i <= n; i++)
	{
		scanf("%s %s %d", s.sno, s.name, &s.score);
		fwrite(&s, sizeof(Student), 1, fp);
	}
	double sum = 0;
	int k = 0;
	rewind(fp);
	while (fread(&s, sizeof(Student), 1, fp))
	{
		sum += s.score;
		k++;
	}
	printf("平均成绩为：%.2f\n", sum / k);
	return 0;
}


#include <string.h>
char word[100], r[100];
int wordNum(const char* infilename)
{
	FILE* inp = fopen(infilename, "r+");
	fseek(inp, 0L, 0);  //将文件位置指针指向文件头
	int i = 0, num = 0, n = 30;
	while (!feof(inp))
	{
		char ch = getc(inp);
		if (ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <= 'Z')
		{
			r[i++] = ch;
		}
		else
		{
			r[i] = '\0';
			i = 0;
			if (!strcmp(word, r)) num++;
		}
	}
	fclose(inp);
	return num;
}

int main(void)
{
	char filename[30];

	printf("请输入文件名：");
	scanf("%s", filename);
	printf("请输入要查找的单词：");
	scanf("%s", word);
	printf("%d", wordNum(filename));

	return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct Singer
{
	char name[20];
	int score;
}singer;

int cmp(const void* a, const void* b)
{
	return ((singer*)b)->score - ((singer*)a)->score;
}

int main(void)
{
	struct Singer num[10];
	char filename1[30], filename2[30];
	int n, i;
	FILE* fp1;

	printf("输入歌手初赛文件：");
	scanf("%s", filename1);
	fp1 = fopen(filename1, "r+");
	if (fp1 == NULL)
	{
		printf("%s文件打开不成功", filename1);
		return 0;
	}
	printf("输入参赛人数\n");
	scanf("%d", &n);
	printf("输入初赛选手及成绩\n");

	for (i = 0; i < n; i++)
	{
		scanf("%s%d", num[i].name, &num[i].score);
	}
	qsort(num, n, sizeof(num[0]), cmp);
	for (i = 0; i < n; i++)
	{
		fprintf(fp1, "%s %d\n", num[i].name, num[i].score);
	}
	printf("输入复赛赛选手及成绩\n");
	for (i = 0; i < n; i++)
	{
		scanf("%s%d", num[i].name, &num[i].score);
	}
	fseek(fp1, 0L, 0);
	char last[20];
	while (!feof(fp1))
	{
		char oname[20];
		int oscore;
		fscanf(fp1, "%s%d", oname, &oscore);
		if (strcmp(last, oname) == 0) continue;
		for (i = 0; i < n; i++)
		{
			if (!strcmp(num[i].name, oname))
			{
				num[i].score += oscore;
				break;
			}
		}
		strcpy(last, oname);
	}
	fclose(fp1);
	printf("输入歌手复赛文件：");
	scanf("%s", filename2);
	FILE* fp2 = fopen(filename2, "r+");
	if (fp2 == NULL)
	{
		printf("%s文件打开不成功", filename2);
		return 0;
	}
	fseek(fp2, 0L, 0);
	qsort(num, n, sizeof(num[0]), cmp);
	for (i = 0; i < n; i++)
	{
		fprintf(fp2, "%s %d\n", num[i].name, num[i].score);
	}
	fclose(fp2);
	return 0;
}


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
bool search(char id[], char pass[])
{//验证账号密码
	FILE* fp;
	char tid[10], tpass[10];
	fp = fopen("text.txt", "r");
	while (!feof(fp)) {
		fscanf(fp, "%s%s", tid, tpass);
		if (strcmp(tid, id) == 0 && strcmp(tpass, pass) == 0)
		{
			fclose(fp);
			return true;
		}
	}
	fclose(fp);
	return false;
}
bool search2(char id[])
{//验证账号密码
	FILE* fp;
	char tid[10], pass[18];
	fp = fopen("text.txt", "r");
	while (!feof(fp))
	{
		fscanf(fp, "%s%s", tid, pass);
		//printf("%s %s %d\n",tid,id,strcmp(tid, id));
		if (strcmp(tid, id) == 0)
		{
			fclose(fp);
			return false;
		}
	}
	fclose(fp);
	return true;
}
bool login()
{//登录
	char id[10], pass[10];
	printf("Login\nPress the id: ");
	scanf("%s", id);
	printf("Press the password: ");
	scanf("%s", pass);
	printf("-----------------------\n");
	if (search(id, pass))
		return true;
	else
		return false;
}
void _add(char id[], char pass[])
{//写入id和密码
	FILE* fp;
	fp = fopen("text.txt", "a");
	fprintf(fp, "%s %s\n", id, pass);
	fclose(fp);
}
void regis()
{//注册
	char id[10], pass[10], tpass[10];
	printf("Register\nPress the id: ");
	scanf("%s", id);
	if (search2(id) == false)
	{
		printf("Id already exists!\n");
		return;
	}
	while (true)
	{
		printf("Press the password: ");
		scanf("%s", pass);
		printf("Press the password again: ");
		scanf("%s", tpass);
		if (strcmp(pass, tpass) != 0)
			printf("The passwords you pressed are not the same!\n");
		else
			break;
	}
	_add(id, pass);
	printf("Register successfully!\n");
}
int main(void)
{
	int command;
	while (true)
	{
		printf("(1.Login   2.Register  3.Exit)\n");
		scanf("%d", &command);
		printf("\n");
		if (command == 3)
			break;
		else if (command == 1)
		{
			int num = 3;
			while (num)
			{
				if (!login())
				{
					printf("ID is not exist or password is wrong!\n");
					printf("Again!\n");
				}
				else
				{
					printf("Login successfully!\n");
					break;
				}
				num--;
			}
			if (!num)
			{
				printf("Error 3 times! Exit!\n");
				break;
			}
		}
		else
			regis();
	}
	return 0;
}



#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int Compute(int x, char op, int y);    	//计算x op y的值并返回

int main(void)
{
	int n, k;          	//存放出题目数，每道题学生可输入答案次数 
	int a, b, ans, yans;  	//存放题中两个操作数、正确答案及学生给的答案 
	char op;                 	//存放运算符

	printf("输入题目数及运算符：\n");
	scanf("%d %c", &n, &op);    	//输入题目数及运算符 
	srand(time(NULL));        	//用机器时间设置随机种子 
	while (n--)
	{
		a = rand() % 10 + 1;
		b = rand() % 10 + 1;
		ans = Compute(a, op, b);   	//计算a op b的正确答案
		k = 3;
		while (k--)             	//每道题至多做3次 
		{
			printf("%d %c %d = ", a, op, b);
			scanf("%d", &yans);
			if (yans == ans) break;  	//答案正确，继续下一道题
			else   printf("no\n");   	//答案不正确，输出no，重做
		}
		if (yans != ans)          	//3次答案都不正确，输出计算式子与正确答案
			printf("%d %c %d = %d\n", a, op, b, ans);
		else                  	//答案正确，输出ok
			printf("ok!\n");
	}

	return 0;
}
int Compute(int x, char op, int y)
{
	int res;
	switch (op)
	{
	case '+':  res = x + y;  break;
	case '-':  res = x - y;   break;
	case '*':  res = x * y;  break;
	case '/':  res = x / y;   break;
	}

	return res;
}


#include <stdio.h>

int Isop(char ch);     	//判断ch是否为运算符，是返回1，否则返回0 
int Priority(char op);  	//返回运算符op的优先级，整数大小代表优先级大小 
int Compute(int x, char ch, int y);  	//返回x op y的计算结果 

int main(void)
{
	int a, b, c;          	//存放操作数 
	char op1, op2;     	//存放运算符和"=" 
	printf("输入表达式 = 结束\n");
	a = 0;
	op1 = '+';
	scanf("%d", &b);
	while (1)
	{
		op2 = getchar();
		if (op2 == '=')    break;       	//如果输入"="，表达式输入结束
		else
			scanf("%d", &c);
		if (!Isop(op2))             	//输入的运算不是"+，-，*，/"
		{
			printf("Error\n");
			return 0;
		}
		if (Priority(op1) >= Priority(op2))
		{                     	//先计算 a op1 b 
			if (op1 == '/' && b == 0)
			{                	//0为除数情况 
				printf("Divided by 0\n");
				return 0;
			}
			a = Compute(a, op1, b);
			op1 = op2;
			b = c;            	//使最后没有计算的式子是a op1 b
		}
		else
		{                   	//先计算 b op2 c 
			if (op2 == '/' && c == 0)
			{
				printf("Divided by 0\n");
				return 0;
			}
			b = Compute(b, op2, c);   	//使最后没有计算的式子是a op1 b
		}
	}
	//式子输入结束，计算最后剩余的式子a op1 b的值
	if (op1 == '/' && b == 0)
	{
		printf("Divided by 0\n");
		return 0;
	}
	a = Compute(a, op1, b);
	printf("%d\n", a);	//输出长表达式的值

	return 0;
}
int Isop(char ch)
{
	if (ch == '+' || ch == '-' || ch == '*' || ch == '/')
		return 1;
	return 0;
}
int Priority(char op)
{
	int res;            	//存放op的优先级整数	
	switch (op)
	{//'+'和'-'的优先级一样，'*'和'/'的优先级一样 
	case '+':
	case '-': res = 1; break;
	case '*':
	case '/': res = 2; break;
	}

	return res;
}
int Compute(int x, char ch, int y)
{
	int res;       	//存放x ch y的值	
	switch (ch)
	{
	case '+': res = x + y; break;
	case '-': res = x - y; break;
	case '*': res = x * y; break;
	case '/': res = x / y; break;
	}

	return res;
}


#include <stdio.h>

int Isop(char ch);     	//判断ch是否为运算符，是返回1，否则返回0 
int Priority(char op);  	//返回运算符op的优先级，整数大小代表优先级大小 
int Compute(int x, char ch, int y);  	//返回x op y的计算结果 

int main(void)
{
	int a, b, c;
	char op1, op2;
	printf("输入表达式: a op1 b op2 c= \n");
	scanf("%d %c %d %c %d", &a, &op1, &b, &op2, &c);
	getchar();                           	//接收'='
	if (!Isop(op1) || !Isop(op2))  	//op1和op2中有一个不是"+,-,*,/"，则输出Error
		printf("Error\n");
	else
	{
		if (Priority(op1) >= Priority(op2))
		{	//先计算 a op1 b 
			if (op1 == '/' && b == 0)
			{//被0除情况 
				printf("Divided by 0\n");
				return 0;
			}
			a = Compute(a, op1, b);
			op1 = op2;
			b = c;
		}
		else
		{//先计算 b op2 c 
			if (op2 == '/' && c == 0)
			{//被0除情况 
				printf("Divided by 0\n");
				return 0;
			}
			b = Compute(b, op2, c);
		}
		//计算上面两种情况后剩余式子
		if (op1 == '/' && b == 0)
		{//被0除情况 
			printf("Divided by 0\n");
			return 0;
		}
		a = Compute(a, op1, b);
		printf("%d\n", a);
	}
	return 0;
}
int Isop(char ch)
{
	if (ch == '+' || ch == '-' || ch == '*' || ch == '/')
		return 1;
	return 0;
}
int Priority(char op)
{
	int res;            	//存放op的优先级整数	
	switch (op)
	{//'+'和'-'的优先级一样，'*'和'/'的优先级一样 
	case '+':
	case '-': res = 1; break;
	case '*':
	case '/': res = 2; break;
	}

	return res;
}
int Compute(int x, char ch, int y)
{
	int res;       	//存放x ch y的值	
	switch (ch)
	{
	case '+': res = x + y; break;
	case '-': res = x - y; break;
	case '*': res = x * y; break;
	case '/': res = x / y; break;
	}

	return res;
}


#include <stdio.h>
#include <time.h>
#include <math.h>


double Polyf3(double a[], int n, double x)
{//用迭代法计算xi   
	double p = a[0];
	double term = 1;          	//term存放xi的值，初始term=x0=1

	for (int i = 1; i <= n; i++)
	{
		term *= x;           	//xi=xi-1*x			
		p += a[i] * term;
	}

	return p;
}

double Polyf4(double a[], int n, double x)
{//秦九韶的从最高阶一项开始迭代的方法 
	double p = a[n];

	for (int i = n - 1; i >= 0; i--)
	{
		p = p * x + a[i];
	}

	return p;
}

#include <stdio.h>

int IsLeap(int y);       	//判断y年是否为闰年，是返回1，否则返回0 
void WeekValue(int y);  	//求y年各个月1日的星期值，存到weekofmonth数组中 
void MonCalendar(int y, int m);  	//打印y年m月的月历表
int monthdays[13] = { 0,31,28,31,30,31,30,31,31,30,31,30,31 }; 	//非闰年每个月的天数 
int weekofmonth[13];        	//存放某年各个月1日是星期几

int main(void)
{
	int year, month;                       	//存放要输入的年月值	
	scanf("%d %d", &year, &month);    	//输入年月值
	WeekValue(year);     	//用打表法求year年每月1日是星期几 
	MonCalendar(year, month);  	//输出year年month月的月历表 

	return 0;
}
int  IsLeap(int y)
{
	return  y % 400 == 0 || y % 4 == 0 && y % 100 != 0;
}
void  WeekValue(int y)
{
	int days = 0;  	//存放y年各个月1日是1900年后的天数对 7 求余所得的数 

	for (int i = 1900; i < y; i++)
	{
		if (IsLeap(i)) days += 366 % 7;
		else           days += 365 % 7;
	}
	if (IsLeap(y))    monthdays[2] = 29;
	for (int i = 1; i <= 12; i++)  	//计算y年1至12月各个月1号的星期值 
	{
		weekofmonth[i] = (days + 1) % 7;
		days += monthdays[i];
	}
}
void MonCalendar(int y, int m)
{
	//months存放各个月英文单词串
	const char* months[13] = { "","January","February","March","April","May","June",
				   "July","August","September","October","November","December" };

	printf("\t%s(%d)\n", months[m], y);          	//输出 月（年） 
	printf("Sun Mon Tue Wed Thu Fri Sat\n");   	//输出星期简写 
	for (int i = 0; i < weekofmonth[m]; i++)   	//输出1日前的w个空白 
		printf("    ");
	if (IsLeap(y))    monthdays[2] = 29;
	else               monthdays[2] = 28;
	int  k = weekofmonth[m];
	for (int i = 1; i <= monthdays[m]; i++)     	//输出1至月末日期，每7个换行 
	{
		printf("%3d ", i);
		k++;
		if (k % 7 == 0)    printf("\n");
	}
	printf("\n");
}

#include <stdio.h>

int IsLeap(int y);  	//判断y是否是闰年
int monthdays[2][13] = { {0,31,28,31,30,31,30,31,31,30,31,30,31},
						{0,31,29,31,30,31,30,31,31,30,31,30,31} };

int main(void)
{
	int year, n, d;      	//存放year年第n周第d天

	printf("输入年份及第几周第几天: \n");
	scanf("%d %d %d", &year, &n, &d);
	int w = 0;
	for (int i = 1; i < year; i++)	//计算year-1 年12月31日是星期几
	{
		if (IsLeap(i))    w += 366 % 7;
		else              w += 365 % 7;
	}
	w %= 7;
	int days = (n - 1) * 7 + d - w;     	//求year年第days天的日期 
	int leap = 0;      	//表示是否是闰年，1表示是闰年，0表示不是闰年
	if (IsLeap(year))   leap = 1;
	int m = 1;
	while (days > monthdays[leap][m])
	{ //通过循环执行y年m月天数，计算所求日期的月份
		days -= monthdays[leap][m];
		m++;
	}
	printf("日期是：%d年%d月%d日\n", year, m, days);

	return 0;
}
int IsLeap(int y)
{
	return y % 400 == 0 || y % 4 == 0 && y % 100 != 0;
}


#include<stdio.h> 

int IsLeap(int y);   	//判断y年是否是闰年，是返回1，否则返回0
int monthdays[2][13] = { {0,31,28,31,30},{0,31,29,31,30} };
//每个月天数，行标为0表示非闰年的，行标为1表示闰年的，只关注1~4月份

int main(void)
{
	int year;
	printf("输入要求哪一年的母亲节日期:");
	scanf("%d", &year);
	int days = 0, i;      	//存year年4月30日是1900年后多少天	
	for (i = 1900; i < year; i++)
	{
		if (IsLeap(i))
			days = days + 366;
		else
			days = days + 365;
	}
	int leap = 0;
	if (IsLeap(year))  leap = 1;
	for (i = 1; i < 5; i++)
		days += monthdays[leap][i];
	int dayofweek, motherday;
	dayofweek = days % 7;          	//计算4月30日是周几
	motherday = (14 - dayofweek);    	//计算母亲节是5月多少日
	printf("%d年5月%d号是母亲节\n", year, motherday);
	return 0;
}
int IsLeap(int y)
{
	if (y % 400 == 0 || y % 4 == 0 && y % 100 != 0)
		return 1;
	else
		return 0;
}



#include <stdio.h>

int monthdays[2][13] = { {0,31,28,31,30,31,30,31,31,30,31,30,31},
		 {0,31,29,31,30,31,30,31,31,30,31,30,31} };
int days[2][13];  	//存放每个月13日是这一年的第几天 
int IsLeap(int year)
{
	return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}
void DaysF();   	//计算days数组元素的值

int main(void)
{
	int year, ans = 0;         	//存放年份，以及该年黑色星期五个数 
	int yeardays[2] = { 365, 366 }; 	//存放非闰年、闰年一年的天数 
	DaysF();                   	//计算days数组元素的值    
	printf("输入年份：");
	scanf("%d", &year);
	int leap = 0;
	if (IsLeap(year))  leap = 1;
	for (int i = 1; i <= 12; i++) 	//求1~year-1天数
	{
		int n = (year - 1) * 365 + (year - 1) / 4 - (year - 1) / 100 + (year - 1) / 400;
		if ((n + days[leap][i]) % 7 == 5)
		{
			printf("%d-%d-13\n", year, i);
			ans++;
		}
	}
	if (ans)
		printf("%d年有%d个黑色星期五\n", year, ans);
	else
		printf("%d年有没有黑色星期五\n", year);
	return 0;
}
void DaysF()
{
	for (int leap = 0; leap <= 1; leap++)
	{
		days[leap][0] = 13;
		for (int m = 1; m <= 12; m++)
			days[leap][m] = days[leap][m - 1] + monthdays[leap][m - 1];
	}
}


#include <stdio.h>
//获取公历年的天数
int yearDays(int year)
{
	if ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0) return 366;
	else return 365;
}
//获取公历1月1日至某整月的天数
int monthDays(int year, int month)
{
	int sum = 0;
	int rui[12] = { 31,29,31,30,31,30,31,31,30,31,30,31 };
	int ping[12] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
	int ruiflag = 0, i;
	if ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0) ruiflag = 1;
	for (i = 0; i < month - 1; i++)
	{
		if (ruiflag == 1) sum += rui[i];
		else sum += ping[i];
	}
	return sum;
}
//获取从公历1年1月1日至当前日期的总天数
int getDays(int year, int month, int day)
{
	int i = 1, days = 1;
	while (i < year)
	{
		days += yearDays(i);
		i++;
	}
	int days2 = monthDays(year, month);
	return days + days2 + day;
}


int main(void)
{
	int year1, year2, month1, month2, day1, day2;
	scanf("%d%d%d", &year1, &month1, &day1);
	scanf("%d%d%d", &year2, &month2, &day2);
	int days1 = getDays(year1, month1, day1);
	int days2 = getDays(year2, month2, day2);
	int ans = 0;
	if (days1 >= days2) ans = days1 - days2;
	else ans = days2 - days1;
	printf("%d\n", ans);
	return 0;
}



#include <stdio.h>
#include <string.h>
#define N 100

int main(void)
{
	int prime[N + 1];                    	//prim[i]值为0表示i在筛子上，值为1表示i不在筛子上

	int  n;
	scanf("%d", &n);            	//输入n，求1~n之间的质数
	memset(prime, 0, sizeof(prime));  	//将prime中所有元素值清0 
	prime[1] = 1;
	for (int i = 2; i * i <= n; i++)
	{ //筛去i的2倍，3倍，…，这些数肯定不是质数
		if (prime[i] == 0)
		{
			for (int j = 2 * i; j <= n; j += i)
				prime[j] = 1;
		}
	}
	int t = 0;  	//统计质数个数，以控制每行输出5个
	for (int i = 1; i <= n; i++)
	{
		if (prime[i] == 0)  	//i是质数，则输出，计数，一行输出5个
		{
			printf("%2d ", i);
			t++;
			if (t % 5 == 0) printf("\n");	//一行已输出5个
		}
	}

	return 0;
}



#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
	int m, n;

	printf("输入m,n:\n");
	scanf("%d %d", &m, &n);
	//动态申请n个int元素，或全局变量处定义int g[20000001];
	int* g = (int*)malloc(sizeof(int) * (n + 1)); 	//g[i]存放i的因子数	   
	memset(g, 0, sizeof(int) * (n + 1));           	//g[0]~g[n]清0 
	for (int i = 1; i <= n; i++)              	//求1~n所有数的因子数
	{
		for (int j = i; j <= n; j += i)      	//j为i的倍数，所以j的因子数加1 
			g[j]++;
	}
	int maxyn = 0;        	//存放最多的因子数 
	int flag = 0;           //标志某区间是否有反质数，0表示没有，1表示有	
	for (int i = 1; i < m; i++)
		if (g[i] > maxyn)  	//i的因子数大于比它小的数的因子数，则i是反质数
			maxyn = g[i];
	for (int i = m; i <= n; i++)
	{
		if (g[i] > maxyn)   	//i的因子数大于比它小的数的因子数，则i是反质数 
		{
			maxyn = g[i];        	//修改maxyn（目前为止数的最多因子数）
			if (flag)  printf(",");  	//判断是否输出过，flag为0表示没有  
			printf("%d", i);      	//输出反质数 
			flag = 1;    	//修改成[m,n] 中有反质数，同时表示也输出过反质数
		}
	}
	if (flag == 0)  printf("NO\n");
	free(g);        	//释放分配给g的堆内存空间，否则会造成内存泄漏

	return 0;
}


#include <stdio.h>
#include <string.h>

int main(void)
{
	char s1[100], s2[40];
	char temp[100] = { 0 };
	scanf("%s %s", s1, s2);
	if (strlen(s1) < strlen(s2))   	//如果串s1的长度小于串s2的长度，串s1和串s2交换
	{
		strcpy(temp, s1);
		strcpy(s1, s2);
		strcpy(s2, temp);
	}
	strcpy(temp, s1);
	strcat(temp, s1);             	//temp是串s1s1 //将其展开
	char* p = strstr(temp, s2);     	//子串查找 
	if (p)                    	//s2是temp的子串
		printf("true\n");
	else                       	//s2不是temp的子串
		printf("false\n");

	return 0;
}


#include <stdio.h>
#include <string.h>

int main(void)
{
	char str[202];
	gets(str);     	//读取带空格的串 
	int i = 0, j = 0;
	int flag = 1;
	while (str[j])    	//串扫描，如果没有到串尾 
	{
		if (str[j] != ' ')     	//非空格字符 
		{
			str[i++] = str[j];
			flag = 1;         	//表示下次遇到空格时，是第一个空格 
		}
		else if (str[j] == ' ' && flag) 	//单词后的第一个空格
		{
			str[i++] = str[j];
			flag = 0;  	//表示下次遇到空格时，不是单词后的第一个空格 
		}
		j++;
	}
	str[i] = '\0';      	//在串尾加字符串结束标记 
	printf("%s", str);
	return 0;
}

#include<string.h>
#include<stdio.h>

int main(void)
{
	int shu[1000];   	//存储从串中提取的整数 
	char str[500];
	scanf("%s", str);
	int k = 0;
	int sum = 0;
	int len = strlen(str);    	//串长度 
	for (int i = 0; i < len; i++)  	//串扫描 
	{
		if (str[i] >= '0' && str[i] <= '9') 	//一旦扫描到数字字符，提取整数开始 
		{
			sum = str[i] - '0';
			int j = i + 1;
			while (j < len && str[j] >= '0' && str[j] <= '9')	//继续直到串尾
													//或遇到非数字字符 
			{
				sum = sum * 10 + str[j] - '0';
				j++;
			}
			shu[k++] = sum;
			i = j;  	//修改i值，i下标位置的字符或者非数字，或到串尾
		}
	}
	for (int i = 0; i < k; i++)    	//输出从串里提取出来的所有整数
		printf("%d\n", shu[i]);

	return 0;
}

#include<string.h>
#include<stdio.h>
#include<stdlib.h>   	//atoi()

int main(void)
{
	int shu[1000];   	//存储从字符串中提取的整数 
	char str[500];
	scanf("%s", str);
	int k = 0;
	int len = strlen(str);
	for (int i = 0; i < len; i++)   	//所有非数字字符用空格替换
		if (str[i] < '0' || str[i] >'9')  str[i] = ' ';

	char* pch = strtok(str, " ");  	//用空格分隔字符串
	while (pch)
	{
		shu[k++] = atoi(pch);  	//atoi()函数可以把数字字符串转换成整数 
		pch = strtok(NULL, " ");  	//下一个子串  
	}
	for (int i = 0; i < k; i++)
		printf("%d\n", shu[i]);

	return 0;
}

#include <stdio.h>
#include <string.h>
#define MAXLEN 1001

void Invert(char* s, int* a); 	//将串s中每个数字字符转换成整数并逆置存放到a中
char str1[MAXLEN], str2[MAXLEN], str[2 * MAXLEN];	//存放乘数及乘积对应的串
int a[MAXLEN], b[MAXLEN];      	//存放乘数的各个位
int  c[2 * MAXLEN];              	//存放乘积的各个位  

int main(void)
{
	//运算前的准备		
	printf("输入两个乘数：\n");
	scanf("%s %s", str1, str2);  	//以串的形式输入了两个乘数	
	//将两个乘数串逆置并转换成整数存放到数组a,b中
	int len1 = strlen(str1), len2 = strlen(str2);

	//确定积的符号 
	int sign = 1, k;
	memset(a, 0, sizeof(a));
	memset(b, 0, sizeof(b));
	k = 0;
	if (str1[0] == '-')
	{
		len1--;
		sign *= -1;
		k++;
	}
	Invert(str1 + k, a);
	k = 0;
	if (str2[0] == '-')
	{
		len2--;
		sign *= -1;
		k++;
	}
	Invert(str2 + k, b);
	//逐位运算	

	memset(c, 0, sizeof(c));
	for (int i = 0; i < len2; i++)
	{
		for (int j = 0; j < len1; j++)
			c[i + j] += a[j] * b[i];
	}
	//处理进位
	for (int i = 0; i < len1 + len2; i++)
	{
		c[i + 1] += c[i] / 10;
		c[i] %= 10;
	}
	//结果处理
	//将最高位（len1+len2-1）前导0去掉后，转换成字符存放到积串str中
	//str[0]为最高位 
	int i = len1 + len2 - 1, j = 0;
	while (c[i] == 0) i--;
	memset(str, 0, sizeof(str));
	for (; i >= 0; i--)
		str[j++] = c[i] + '0';
	if (strlen(str) == 0) str[0] = '0';
	printf("%s * %s = ", str1, str2);
	if (sign == -1) printf("-");
	printf("%s \n", str);

	return 0;
}

void Invert(char* s, int* a)
{
	int len = strlen(s), i = 0;
	for (int j = len - 1; j >= 0; j--)
		a[i++] = s[j] - '0';
}


#include <stdio.h>
#include <string.h>
#define N  3000 
int result[N];      	//存放n!值的各个位上的数，result[0]存放个位

int main(void)
{
	int n;                         	//存放n 

	printf("计算n!，输入n的值：");  	//提示输入n 
	scanf("%d", &n);
	memset(result, 0, N);
	result[0] = 1;              	//累乘器初始赋值1
	int digits = 1;             	//存放结果的位数，初始时位数为1 
	for (int i = 2; i <= n; i++)
	{//1*2*...*n 
		int  jw = 0;	//jw存放低位向高位的进位数	
		for (int j = 0; j < digits; j++)   	//将result从个位起至第digits位依次乘以 i
		{//左起第j位数：result[j]，			
			int total = result[j] * i + jw;
			result[j] = total % 10;
			jw = total / 10;	//计算进位jw的值     
			if (j == digits - 1 && jw)  	//如果第digits位(最高位)有进位，则位数加1 
				digits++;
		}
	}
	char factarr[N];             	//存放result的串 
	memset(factarr, 0, N);
	for (int i = digits - 1, j = 0; i >= 0; i--)
	{
		factarr[j++] = result[i] + '0';
	}
	printf("%d! = %s\n", n, factarr);   	//输出n!=? 

	return 0;
}

#define SET_BIT(a, i)   (a |= 1 << (i))  
#define CLS_BIT(a, i)	(a &= ~(1 << (i)))


#include <stdio.h>

int main(void)
{
	int a = 0xFBA;  	//为便于分析程序，直接给 a 赋值	
	a &= 0x3F << 3;                       	//将0x3f左移3位构造的数与a位与运算
	a >>= 3;               	//即可得到 a的bit3~bit8对应的整数 
	printf("a = %#X\n", a);   	//将a按十六进制格式输出，字母为大写

	return 0;
}


#include <stdio.h>

int main(void)
{
	int a = 0xACF71;  	//为便于分析程序，直接给 a 赋值
	a &= ~(0x7FF << 7);/*0x7FF对应的数为111 1111 1111，左移7位再取反，即
			可构造一个bit7~bit17为0，其余位为1的整数，a与这
			个构造的数进行位与运算即可将a的bit7~bit17清0，其
			余位不变*/
	a |= 937 << 7;  	//实现将a的bit7~bit17赋值937整数，其余位不变
	printf("a = %#X\n", a);	//将a按十六进制格式输出，字母为大写

	return 0;
}


#include <stdio.h>

int main(void)
{
	int a = 0xACF71;      	//为便于分析程序，直接给 a 赋值
	unsigned tmp;    	//用于存放寄存器bit7~bit17原来的值及增加17后的值

	tmp = a & (0x7FF << 7); 	//将a与 bit7~bit17为1其余位为0的数进行位与操作
	tmp >>= 7;            	//获取a的bit7~bit17值		
	tmp += 17;           	//tmp 为寄存器bit7~bit17最终的值
	a &= ~(0x7FF << 7);  	//将 a 的 bit7~bit17清0，其余位不变 
	a |= tmp << 7;         	//将bit7~bit17赋值tmp整数，其余位不变 
	printf("a = %#X\n", a);  	//将a按十六进制格式输出，字母为大写

	return 0;
}


#include<stdio.h>
int CountOne(int a);

int main(void)
{
	int a;

	printf("输入一个整数：");
	scanf("%d", &a);
	printf("%d的二进制表示中1的个数是%d\n", a, CountOne(a));

	return 0;
}
int CountOne(int a)
{
	int c = 0;        	//记录a的二进制表示中1的个数 
	while (a)
	{
		c++;
		a &= a - 1;    	//可去掉a的二进制表示中最右边的1 
	}
	return c;
}


#include <stdio.h>

int main(void)
{
	int a, b;
	a = 10; b = 20;
	a = a ^ b;
	b = b ^ a;       	//b=b^a=b^a^b=a
	a = a ^ b;       	//a=a^b=a^b^a=b
	printf("a = %d  b=%d\n", a, b);

	return 0;
}


#include <stdio.h>
#include <stdlib.h>
int AppearOnce(int* a, int n);   	 //返回n个元素的数组a中只出现一次的元素

int main(void)
{
	int t, n;           	 //存放测试数据的组数，及每组测试数据的元素个数
	scanf("%d", &t);
	while (t--)
	{
		scanf("%d", &n);
		int* a = (int*)malloc(sizeof(int) * n);	//动态申请能存放n个整型数的数组 
		for (int i = 0; i < n; i++)
			scanf("%d", &a[i]);
		int OnceNum = AppearOnce(a, n);	 //求n个元素数组a中只出现一次的元素 
		printf("%d\n", OnceNum);
		free(a);                           	//释放空间 
	}

	return 0;
}

int AppearOnce(int* a, int n)
{
	int num = 0;
	//元素异或处理，因为相同元素异或结果为0 
	//所以num最后所得数为只出现一次的元素
	for (int i = 0; i < n; i++)
		num ^= a[i];
	return num;
}


#include <stdio.h>
void SubSet(int* a, int i);           //输出i对应二进制数中1所在位上对应元素构成的集合

int main(void)
{
	int a[] = { 1,2,3 }, N;

	N = sizeof(a) / sizeof(int);
	int t = 1 << N;
	for (int i = 1; i < t; i++)                //将1，2，...，2N-1的二进制数中1位上对应的元素输出 
		SubSet(a, i);
	return 0;
}
void SubSet(int* a, int i)
{
	int k, flag = 0;                      //k对应a的元素下标		
	printf("{");
	k = 0;
	while (i)
	{
		if (i & 1)
		{
			if (flag)  printf(",");
			printf("%d", a[k]);
			flag = 1;
		}
		i >>= 1;
		k++;
	}
	printf("}\n");
}






