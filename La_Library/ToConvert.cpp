
typedef float REAL;


#include <stdio.h>

const int N = 10;

int main(void)
{
	int a[N];              				//������N������Ԫ�ص�����a
	int n;
	scanf("%d", &n);       				//������������
	for (int i = 0; i < n; i++)   		//��������a��n��Ԫ�� 
		scanf("%d", &a[i]);  			//����Ԫ��a[i], ע������a�ĵ�i+1��Ԫ�� 
	for (int i = 0; i < n / 2; i++)  			//���ã������ף�a[0]����β��a[n-1]����ȵ�Ԫ�ؽ�����ֵ
	{
		int t;
		t = a[i];  a[i] = a[n - 1 - i];  a[n - 1 - i] = t;	//����a[i]��a[n-1-i]��ֵ 
	}
	for (int i = 0; i < n; i++)    			//���a��n��Ԫ�� 
		printf("%d  ", a[i]);

	return 0;
}


#include <stdio.h>

void SelectSort(int r[], int n);    		//��ѡ������˼�������a�е�n��Ԫ������  
const int N = 6;

int main(void)
{
	int a[N];                      		//�ٶ�����N��Ԫ�ص����� a 		
	printf("����%d��������\n", N);   		//��ʾ����N��Ԫ�� 
	for (int i = 0; i < N; i++)          	//�����������N��Ԫ��  		
		scanf("%d", &a[i]);
	SelectSort(a, N);             		//�۵��ú���������a��������

	printf("�������Ϊ��");
	for (int i = 0; i < N; i++)        	//���������������Ԫ��
		printf("%d ", a[i]);
	printf("\n");

	return 0;
}
void SelectSort(int r[], int n)
{
	int i, j, mink, m;           			//mink�����СԪ�ص��±� 
	for (i = 0; i < n - 1; i++)
	{
		mink = i;           				//��r[i]~r[n-1]������Ԫ�أ���mink��¼��СԪ���±� 
		for (j = i + 1; j < n; j++)
		{
			if (r[mink] > r[j])  mink = j;
		}
		if (mink != i)      		//���a[i]Ԫ�ز���r[i]~r[n-1]�е���СԪ�أ��򽻻�a[mink],a[i] 
		{
			int t;
			t = r[i];  r[i] = r[mink];  r[mink] = t;
		}
	}
}


#include <stdio.h>
#include <stdlib.h>

int zhenfen(int m, int n);//m��n�ܹ������������1�����򷵻�0 
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
	printf("������Ҫ���ļ������֣�");
	scanf("%s", inputfile);      	//�����ļ��� 
	fp = fopen(inputfile, "r");   	//��ֻ����ʽ���ı��ļ�  
	if (fp == NULL)         	//�ļ���ʧ�ܣ�ֱ���˳����� 
	{
		printf("\n%s��ʧ�ܣ�\n", inputfile);
		return 0;
	}
	printf("%s �ļ����ݣ�\n", inputfile);
	while (!feof(fp))        	//���ļ�δ����ʱ 
		putchar(fgetc(fp));  	//���ļ��ж�ȡ�ַ�����ʾ 

	char ch;
	while ((ch = fgetc(fp)) != EOF)
		putchar(ch);
	printf("\n");
	fclose(fp);           	//�ر��ļ� 

	return 0;
}


#include <stdio.h>
void WriteFile(FILE* fp);	//���ļ���д����
void ReadFile(FILE* fp);          	//���ļ��������  

int main(void)
{
	FILE* fp;
	char inputfile[30];

	printf("�������ļ���: ");
	scanf("%s", inputfile);              	//����Ҫ�򿪵��ļ���     
	fp = fopen(inputfile, "w+");
	if (fp == NULL)
	{
		printf("���ļ�ʧ��");
		return 0;
	}
	else
	{
		printf("%s �򿪳ɹ�\n", inputfile);
		WriteFile(fp);  	//���ú������ַ�д���ļ�    
		ReadFile(fp);  	//���ú������ļ����ݲ���ʾ   
		fclose(fp);  	//�ر��ļ�
	}
	return 0;
}
void WriteFile(FILE* fp)	//���ļ���д���� 
{
	char ch;

	printf("�������ļ����ݣ���#������\n");
	while ((ch = getchar()) != '#')     	//�Ӽ��̶����ַ���ֱ������#ѭ������ 
	{
		fputc(ch, fp);  	//��fp��ָ�ļ���д�ַ�ch		
	}
	rewind(fp);                	//���ļ�ָ�붨λ���ļ���ͷ 
}

void ReadFile(FILE* fp)        	//���ļ�������� 
{
	char ch;
	printf("��ȡ�ļ����ݣ�\n");
	ch = fgetc(fp);           	//���ļ���ȡ�ַ� 
	while (ch != EOF)
	{
		putchar(ch);  	//��ʾ�ַ� 
		ch = fgetc(fp);  	//���ļ���ȡ��һ���ַ� 
	}
	printf("\n�ļ���ȡ����");
}


#include <stdio.h>

int main(void)
{
	FILE* fp;
	char str[100];
	printf("������һ���ַ�����\n");
	gets(str);	//����Ҫд���ļ��е��ַ���	
	char fileName[20]; 	//�������ļ������ַ�����
	printf("�������ļ�����·�������ƣ�\n");
	scanf("%s", fileName);	//�����ļ�����·��������
	if ((fp = fopen(fileName, "a")) == NULL)	//��׷�ӷ�ʽ��ָ���ļ�     
	{
		printf("��ʧ��!");
		return  0;
	}
	fputs(str, fp); 	//���ַ�����str�е��ַ���д��fpָ����ļ�
	fclose(fp);
	if ((fp = fopen(fileName, "r")) != NULL)
	{
		printf("%s �ļ����ݣ�\n", fileName);
		while (fgets(str, sizeof(str), fp)) 	//��fp��ָ���ļ��ж�ȡ�ַ�������str�� 
			printf("%s", str); 	//���ַ������ 
		fclose(fp);              	//�ر��ļ�
	}

	return 0;
}



#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	FILE* in, * out;	//��������FILE���͵�ָ�����
	char infile[50], outfile[50];     	//�ֱ���Դ�ļ���Ŀ���ļ��� 
	char s[256];
	printf("������Դ�ļ���:");
	scanf("%s", infile);	//����Դ�ļ�����·��������
	printf("������Ŀ���ļ���:");
	scanf("%s", outfile);	//����Ŀ���ļ�����·��������
	if ((in = fopen(infile, "r")) == NULL)	//��ֻ����ʽ��ָ���ļ�
	{
		printf("���ļ�%sʧ��\n", infile);
		exit(0);                               	//�˳�����
	}
	if ((out = fopen(outfile, "w")) == NULL) 	//��ֻд��ʽ��ָ���ļ�
	{
		printf("���ܽ���%s�ļ�\n", outfile);
		exit(0);                                	//�˳�����
	}
	while (fgets(s, 256, in)) 	//��inָ����ļ������ݸ��Ƶ�out��ָ����ļ���
		fputs(s, out);
	printf("�ļ��������\n");
	//�ر��ļ�
	fclose(in);
	fclose(out);

	return 0;
}


#include<stdio.h>

typedef struct Rec	//����ṹ�����ͣ�ʹ��typedef����RecΪ�ṹ�������� 
{
	char id[10];
	char name[20];
	float price;
	int  count;
}Rec;
void WriteFile(FILE* fp, int n);  	//�������������Ʒ��Ϣд���ļ�
void ReadFile(FILE* fp, int n);  	//���ļ�������Ʒ��Ϣ 

int main(void)
{
	char filename[20];
	int n;
	FILE* fp;
	printf("������Ŀ���ļ�:\n");
	scanf("%s", filename);
	fp = fopen(filename, "w+");  	//���ı���д��ʽ���ļ�	
	if (fp == NULL)
	{
		printf("���ļ�ʧ��");
		return 0;
	}
	printf("��������Ʒ����:\n");
	scanf("%d", &n);  	//�Ӽ�������
	WriteFile(fp, n);
	ReadFile(fp, n);
	fclose(fp); 	//�ر��ļ� 

	return 0;
}
void WriteFile(FILE* fp, int n)      	//�������������Ʒ��Ϣд���ļ� 
{
	int i;
	Rec record;
	printf("***********��������Ʒ����***********\n");
	for (i = 1; i <= n; i++)            	//�Ӽ���������Ʒ��Ϣ 
	{
		printf("���������:");
		scanf("%s", record.id);
		printf("����������:");
		scanf("%s", record.name);
		printf("������۸�");
		scanf("%f", &record.price);
		printf("������������");
		scanf("%d", &record.count);
		printf("\n");
		//д���ļ�
		fprintf(fp, "%s %s %5.2f %d\n", record.id, record.name, record.price, record.count);
	}
}

void ReadFile(FILE* fp, int n)        	//���ļ�������Ʒ��Ϣ 
{
	Rec record;
	double total = 0;
	rewind(fp);              	//���ļ��ڲ���λ��ָ���Ƶ��ļ���
	while (fscanf(fp, "%s %s %f %d\n", record.id, record.name, &record.price, &record.count) != EOF)
	{//�������
		printf("���:%s ����:%s �۸�:%5.2f ����:%d \n", record.id, record.name,
			record.price, record.count);
		total = total + record.price * record.count;
	}
	printf("�ϼ�:%5.2f\n", total);
}


#include<stdio.h>
#include <stdlib.h>

typedef struct Rec	//����ṹ�����ͣ�ʹ��typedef����RecΪ�ṹ�������� 
{
	char id[10];
	char name[20];
	float price;
	int  count;
}Rec;
void WriteFile(FILE* fp, int n);  	//�������������Ʒ��Ϣд���ļ�
void ReadFile(FILE* fp, int n);  	//���ļ�������Ʒ��Ϣ 

int main(void)
{
	char filename[20];
	int n;
	FILE* fp;
	printf("������Ŀ���ļ�:\n");
	scanf("%s", filename);
	fp = fopen(filename, "wb+");  	//�Զ����ƶ�д��ʽ���ļ�	
	if (fp == NULL)
	{
		printf("���ļ�ʧ��");
		exit(1);
	}
	printf("��������Ʒ����:\n");
	scanf("%d", &n); 	//�Ӽ�������
	WriteFile(fp, n);
	ReadFile(fp, n);
	fclose(fp); 	//�ر��ļ� 

	return 0;
}
void WriteFile(FILE* fp, int n)      	//�������������Ʒ��Ϣд���ļ� 
{
	int i;
	Rec record;

	printf("***********��������Ʒ����***********\n");
	for (i = 1; i <= n; i++)                    	//�Ӽ���������Ʒ��Ϣ 
	{
		printf("���������:");
		scanf("%s", record.id);
		printf("����������:");
		scanf("%s", record.name);
		printf("������۸�");
		scanf("%f", &record.price);
		printf("������������");
		scanf("%d", &record.count);
		printf("\n");
		fwrite(&record, sizeof(Rec), 1, fp); 	//�Կ�ķ�ʽд���ļ�
	}

}
void ReadFile(FILE* fp, int n)        	//���ļ�������Ʒ��Ϣ 
{
	Rec record;
	double total = 0;

	rewind(fp);                	//���ļ��ڲ���λ��ָ���Ƶ��ļ���    
	while (fread(&record, sizeof(Rec), 1, fp)) 	//�Կ�ķ�ʽ��ȡ�ļ��е�����
	{
		printf("���:%s ����:%s �۸�:%5.2f ����:%d \n", record.id, record.name,
			record.price, record.count);  			//������� 
		total = total + record.price * record.count;
	}
	printf("�ϼ�:%5.2f\n", total);
}


#include<stdio.h>
#include <stdlib.h>

typedef struct Rec	//����ṹ�����ͣ�ʹ��typedef����RecΪ�ṹ�������� 
{
	char id[10];
	char name[20];
	float price;
	int  count;
}Rec;

int main(void)
{
	char filename[20]; 	//�����Ʒ��Ϣ�ļ���    
	FILE* fp;
	int m;    	//���Ҫ��ȡ����Ʒ��¼�� 
	Rec record;

	printf("��������Ʒ��Ϣ�ļ�:\n");
	scanf("%s", filename);
	fp = fopen(filename, "rb"); 	//���ı���д��ʽ���ļ�	
	if (fp == NULL)
	{
		printf("���ļ�ʧ��");
		exit(1);
	}
	while (fread(&record, sizeof(Rec), 1, fp))
	{
		printf("%s ����:%s �۸�:%5.2f ����:%d \n", record.id, record.name,
			record.price, record.count);
	}
	printf("������Ҫ��ȡ����Ʒ��¼��:\n");
	scanf("%d", &m); 	//�Ӽ�������
	fseek(fp, (m - 1) * sizeof(Rec), 0); 	//���ļ�λ��ָ���Ƶ���m����Ʒ��Ϣλ��
	fread(&record, sizeof(Rec), 1, fp);
	printf("��%d����¼\n���:%s ����:%s �۸�:%5.2f ����:%d \n", m, record.id,
		record.name, record.price, record.count);
	fclose(fp); 	//�ر��ļ� 

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

	printf("������Ҫд��Ͷ������ݵ��ļ�����");
	scanf("%s", filename);
	fp = fopen(filename, "wb+");
	printf("������ѧ������");
	scanf("%d", &n);
	Student s;
	printf("����%d��ѧ������Ϣ��ѧ�� ���� �ɼ���\n", n);
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
	printf("ƽ���ɼ�Ϊ��%.2f\n", sum / k);
	return 0;
}


#include <string.h>
char word[100], r[100];
int wordNum(const char* infilename)
{
	FILE* inp = fopen(infilename, "r+");
	fseek(inp, 0L, 0);  //���ļ�λ��ָ��ָ���ļ�ͷ
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

	printf("�������ļ�����");
	scanf("%s", filename);
	printf("������Ҫ���ҵĵ��ʣ�");
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

	printf("������ֳ����ļ���");
	scanf("%s", filename1);
	fp1 = fopen(filename1, "r+");
	if (fp1 == NULL)
	{
		printf("%s�ļ��򿪲��ɹ�", filename1);
		return 0;
	}
	printf("�����������\n");
	scanf("%d", &n);
	printf("�������ѡ�ּ��ɼ�\n");

	for (i = 0; i < n; i++)
	{
		scanf("%s%d", num[i].name, &num[i].score);
	}
	qsort(num, n, sizeof(num[0]), cmp);
	for (i = 0; i < n; i++)
	{
		fprintf(fp1, "%s %d\n", num[i].name, num[i].score);
	}
	printf("���븴����ѡ�ּ��ɼ�\n");
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
	printf("������ָ����ļ���");
	scanf("%s", filename2);
	FILE* fp2 = fopen(filename2, "r+");
	if (fp2 == NULL)
	{
		printf("%s�ļ��򿪲��ɹ�", filename2);
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
{//��֤�˺�����
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
{//��֤�˺�����
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
{//��¼
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
{//д��id������
	FILE* fp;
	fp = fopen("text.txt", "a");
	fprintf(fp, "%s %s\n", id, pass);
	fclose(fp);
}
void regis()
{//ע��
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

int Compute(int x, char op, int y);    	//����x op y��ֵ������

int main(void)
{
	int n, k;          	//��ų���Ŀ����ÿ����ѧ��������𰸴��� 
	int a, b, ans, yans;  	//���������������������ȷ�𰸼�ѧ�����Ĵ� 
	char op;                 	//��������

	printf("������Ŀ�����������\n");
	scanf("%d %c", &n, &op);    	//������Ŀ��������� 
	srand(time(NULL));        	//�û���ʱ������������� 
	while (n--)
	{
		a = rand() % 10 + 1;
		b = rand() % 10 + 1;
		ans = Compute(a, op, b);   	//����a op b����ȷ��
		k = 3;
		while (k--)             	//ÿ����������3�� 
		{
			printf("%d %c %d = ", a, op, b);
			scanf("%d", &yans);
			if (yans == ans) break;  	//����ȷ��������һ����
			else   printf("no\n");   	//�𰸲���ȷ�����no������
		}
		if (yans != ans)          	//3�δ𰸶�����ȷ���������ʽ������ȷ��
			printf("%d %c %d = %d\n", a, op, b, ans);
		else                  	//����ȷ�����ok
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

int Isop(char ch);     	//�ж�ch�Ƿ�Ϊ��������Ƿ���1�����򷵻�0 
int Priority(char op);  	//���������op�����ȼ���������С�������ȼ���С 
int Compute(int x, char ch, int y);  	//����x op y�ļ����� 

int main(void)
{
	int a, b, c;          	//��Ų����� 
	char op1, op2;     	//����������"=" 
	printf("������ʽ = ����\n");
	a = 0;
	op1 = '+';
	scanf("%d", &b);
	while (1)
	{
		op2 = getchar();
		if (op2 == '=')    break;       	//�������"="�����ʽ�������
		else
			scanf("%d", &c);
		if (!Isop(op2))             	//��������㲻��"+��-��*��/"
		{
			printf("Error\n");
			return 0;
		}
		if (Priority(op1) >= Priority(op2))
		{                     	//�ȼ��� a op1 b 
			if (op1 == '/' && b == 0)
			{                	//0Ϊ������� 
				printf("Divided by 0\n");
				return 0;
			}
			a = Compute(a, op1, b);
			op1 = op2;
			b = c;            	//ʹ���û�м����ʽ����a op1 b
		}
		else
		{                   	//�ȼ��� b op2 c 
			if (op2 == '/' && c == 0)
			{
				printf("Divided by 0\n");
				return 0;
			}
			b = Compute(b, op2, c);   	//ʹ���û�м����ʽ����a op1 b
		}
	}
	//ʽ������������������ʣ���ʽ��a op1 b��ֵ
	if (op1 == '/' && b == 0)
	{
		printf("Divided by 0\n");
		return 0;
	}
	a = Compute(a, op1, b);
	printf("%d\n", a);	//��������ʽ��ֵ

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
	int res;            	//���op�����ȼ�����	
	switch (op)
	{//'+'��'-'�����ȼ�һ����'*'��'/'�����ȼ�һ�� 
	case '+':
	case '-': res = 1; break;
	case '*':
	case '/': res = 2; break;
	}

	return res;
}
int Compute(int x, char ch, int y)
{
	int res;       	//���x ch y��ֵ	
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

int Isop(char ch);     	//�ж�ch�Ƿ�Ϊ��������Ƿ���1�����򷵻�0 
int Priority(char op);  	//���������op�����ȼ���������С�������ȼ���С 
int Compute(int x, char ch, int y);  	//����x op y�ļ����� 

int main(void)
{
	int a, b, c;
	char op1, op2;
	printf("������ʽ: a op1 b op2 c= \n");
	scanf("%d %c %d %c %d", &a, &op1, &b, &op2, &c);
	getchar();                           	//����'='
	if (!Isop(op1) || !Isop(op2))  	//op1��op2����һ������"+,-,*,/"�������Error
		printf("Error\n");
	else
	{
		if (Priority(op1) >= Priority(op2))
		{	//�ȼ��� a op1 b 
			if (op1 == '/' && b == 0)
			{//��0����� 
				printf("Divided by 0\n");
				return 0;
			}
			a = Compute(a, op1, b);
			op1 = op2;
			b = c;
		}
		else
		{//�ȼ��� b op2 c 
			if (op2 == '/' && c == 0)
			{//��0����� 
				printf("Divided by 0\n");
				return 0;
			}
			b = Compute(b, op2, c);
		}
		//�����������������ʣ��ʽ��
		if (op1 == '/' && b == 0)
		{//��0����� 
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
	int res;            	//���op�����ȼ�����	
	switch (op)
	{//'+'��'-'�����ȼ�һ����'*'��'/'�����ȼ�һ�� 
	case '+':
	case '-': res = 1; break;
	case '*':
	case '/': res = 2; break;
	}

	return res;
}
int Compute(int x, char ch, int y)
{
	int res;       	//���x ch y��ֵ	
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
{//�õ���������xi   
	double p = a[0];
	double term = 1;          	//term���xi��ֵ����ʼterm=x0=1

	for (int i = 1; i <= n; i++)
	{
		term *= x;           	//xi=xi-1*x			
		p += a[i] * term;
	}

	return p;
}

double Polyf4(double a[], int n, double x)
{//�ؾ��صĴ���߽�һ�ʼ�����ķ��� 
	double p = a[n];

	for (int i = n - 1; i >= 0; i--)
	{
		p = p * x + a[i];
	}

	return p;
}

#include <stdio.h>

int IsLeap(int y);       	//�ж�y���Ƿ�Ϊ���꣬�Ƿ���1�����򷵻�0 
void WeekValue(int y);  	//��y�������1�յ�����ֵ���浽weekofmonth������ 
void MonCalendar(int y, int m);  	//��ӡy��m�µ�������
int monthdays[13] = { 0,31,28,31,30,31,30,31,31,30,31,30,31 }; 	//������ÿ���µ����� 
int weekofmonth[13];        	//���ĳ�������1�������ڼ�

int main(void)
{
	int year, month;                       	//���Ҫ���������ֵ	
	scanf("%d %d", &year, &month);    	//��������ֵ
	WeekValue(year);     	//�ô����year��ÿ��1�������ڼ� 
	MonCalendar(year, month);  	//���year��month�µ������� 

	return 0;
}
int  IsLeap(int y)
{
	return  y % 400 == 0 || y % 4 == 0 && y % 100 != 0;
}
void  WeekValue(int y)
{
	int days = 0;  	//���y�������1����1900���������� 7 �������õ��� 

	for (int i = 1900; i < y; i++)
	{
		if (IsLeap(i)) days += 366 % 7;
		else           days += 365 % 7;
	}
	if (IsLeap(y))    monthdays[2] = 29;
	for (int i = 1; i <= 12; i++)  	//����y��1��12�¸�����1�ŵ�����ֵ 
	{
		weekofmonth[i] = (days + 1) % 7;
		days += monthdays[i];
	}
}
void MonCalendar(int y, int m)
{
	//months��Ÿ�����Ӣ�ĵ��ʴ�
	const char* months[13] = { "","January","February","March","April","May","June",
				   "July","August","September","October","November","December" };

	printf("\t%s(%d)\n", months[m], y);          	//��� �£��꣩ 
	printf("Sun Mon Tue Wed Thu Fri Sat\n");   	//������ڼ�д 
	for (int i = 0; i < weekofmonth[m]; i++)   	//���1��ǰ��w���հ� 
		printf("    ");
	if (IsLeap(y))    monthdays[2] = 29;
	else               monthdays[2] = 28;
	int  k = weekofmonth[m];
	for (int i = 1; i <= monthdays[m]; i++)     	//���1����ĩ���ڣ�ÿ7������ 
	{
		printf("%3d ", i);
		k++;
		if (k % 7 == 0)    printf("\n");
	}
	printf("\n");
}

#include <stdio.h>

int IsLeap(int y);  	//�ж�y�Ƿ�������
int monthdays[2][13] = { {0,31,28,31,30,31,30,31,31,30,31,30,31},
						{0,31,29,31,30,31,30,31,31,30,31,30,31} };

int main(void)
{
	int year, n, d;      	//���year���n�ܵ�d��

	printf("������ݼ��ڼ��ܵڼ���: \n");
	scanf("%d %d %d", &year, &n, &d);
	int w = 0;
	for (int i = 1; i < year; i++)	//����year-1 ��12��31�������ڼ�
	{
		if (IsLeap(i))    w += 366 % 7;
		else              w += 365 % 7;
	}
	w %= 7;
	int days = (n - 1) * 7 + d - w;     	//��year���days������� 
	int leap = 0;      	//��ʾ�Ƿ������꣬1��ʾ�����꣬0��ʾ��������
	if (IsLeap(year))   leap = 1;
	int m = 1;
	while (days > monthdays[leap][m])
	{ //ͨ��ѭ��ִ��y��m�������������������ڵ��·�
		days -= monthdays[leap][m];
		m++;
	}
	printf("�����ǣ�%d��%d��%d��\n", year, m, days);

	return 0;
}
int IsLeap(int y)
{
	return y % 400 == 0 || y % 4 == 0 && y % 100 != 0;
}


#include<stdio.h> 

int IsLeap(int y);   	//�ж�y���Ƿ������꣬�Ƿ���1�����򷵻�0
int monthdays[2][13] = { {0,31,28,31,30},{0,31,29,31,30} };
//ÿ�����������б�Ϊ0��ʾ������ģ��б�Ϊ1��ʾ����ģ�ֻ��ע1~4�·�

int main(void)
{
	int year;
	printf("����Ҫ����һ���ĸ�׽�����:");
	scanf("%d", &year);
	int days = 0, i;      	//��year��4��30����1900��������	
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
	dayofweek = days % 7;          	//����4��30�����ܼ�
	motherday = (14 - dayofweek);    	//����ĸ�׽���5�¶�����
	printf("%d��5��%d����ĸ�׽�\n", year, motherday);
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
int days[2][13];  	//���ÿ����13������һ��ĵڼ��� 
int IsLeap(int year)
{
	return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}
void DaysF();   	//����days����Ԫ�ص�ֵ

int main(void)
{
	int year, ans = 0;         	//�����ݣ��Լ������ɫ��������� 
	int yeardays[2] = { 365, 366 }; 	//��ŷ����ꡢ����һ������� 
	DaysF();                   	//����days����Ԫ�ص�ֵ    
	printf("������ݣ�");
	scanf("%d", &year);
	int leap = 0;
	if (IsLeap(year))  leap = 1;
	for (int i = 1; i <= 12; i++) 	//��1~year-1����
	{
		int n = (year - 1) * 365 + (year - 1) / 4 - (year - 1) / 100 + (year - 1) / 400;
		if ((n + days[leap][i]) % 7 == 5)
		{
			printf("%d-%d-13\n", year, i);
			ans++;
		}
	}
	if (ans)
		printf("%d����%d����ɫ������\n", year, ans);
	else
		printf("%d����û�к�ɫ������\n", year);
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
//��ȡ�����������
int yearDays(int year)
{
	if ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0) return 366;
	else return 365;
}
//��ȡ����1��1����ĳ���µ�����
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
//��ȡ�ӹ���1��1��1������ǰ���ڵ�������
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
	int prime[N + 1];                    	//prim[i]ֵΪ0��ʾi��ɸ���ϣ�ֵΪ1��ʾi����ɸ����

	int  n;
	scanf("%d", &n);            	//����n����1~n֮�������
	memset(prime, 0, sizeof(prime));  	//��prime������Ԫ��ֵ��0 
	prime[1] = 1;
	for (int i = 2; i * i <= n; i++)
	{ //ɸȥi��2����3����������Щ���϶���������
		if (prime[i] == 0)
		{
			for (int j = 2 * i; j <= n; j += i)
				prime[j] = 1;
		}
	}
	int t = 0;  	//ͳ�������������Կ���ÿ�����5��
	for (int i = 1; i <= n; i++)
	{
		if (prime[i] == 0)  	//i���������������������һ�����5��
		{
			printf("%2d ", i);
			t++;
			if (t % 5 == 0) printf("\n");	//һ�������5��
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

	printf("����m,n:\n");
	scanf("%d %d", &m, &n);
	//��̬����n��intԪ�أ���ȫ�ֱ���������int g[20000001];
	int* g = (int*)malloc(sizeof(int) * (n + 1)); 	//g[i]���i��������	   
	memset(g, 0, sizeof(int) * (n + 1));           	//g[0]~g[n]��0 
	for (int i = 1; i <= n; i++)              	//��1~n��������������
	{
		for (int j = i; j <= n; j += i)      	//jΪi�ı���������j����������1 
			g[j]++;
	}
	int maxyn = 0;        	//������������� 
	int flag = 0;           //��־ĳ�����Ƿ��з�������0��ʾû�У�1��ʾ��	
	for (int i = 1; i < m; i++)
		if (g[i] > maxyn)  	//i�����������ڱ���С����������������i�Ƿ�����
			maxyn = g[i];
	for (int i = m; i <= n; i++)
	{
		if (g[i] > maxyn)   	//i�����������ڱ���С����������������i�Ƿ����� 
		{
			maxyn = g[i];        	//�޸�maxyn��ĿǰΪֹ���������������
			if (flag)  printf(",");  	//�ж��Ƿ��������flagΪ0��ʾû��  
			printf("%d", i);      	//��������� 
			flag = 1;    	//�޸ĳ�[m,n] ���з�������ͬʱ��ʾҲ�����������
		}
	}
	if (flag == 0)  printf("NO\n");
	free(g);        	//�ͷŷ����g�Ķ��ڴ�ռ䣬���������ڴ�й©

	return 0;
}


#include <stdio.h>
#include <string.h>

int main(void)
{
	char s1[100], s2[40];
	char temp[100] = { 0 };
	scanf("%s %s", s1, s2);
	if (strlen(s1) < strlen(s2))   	//�����s1�ĳ���С�ڴ�s2�ĳ��ȣ���s1�ʹ�s2����
	{
		strcpy(temp, s1);
		strcpy(s1, s2);
		strcpy(s2, temp);
	}
	strcpy(temp, s1);
	strcat(temp, s1);             	//temp�Ǵ�s1s1 //����չ��
	char* p = strstr(temp, s2);     	//�Ӵ����� 
	if (p)                    	//s2��temp���Ӵ�
		printf("true\n");
	else                       	//s2����temp���Ӵ�
		printf("false\n");

	return 0;
}


#include <stdio.h>
#include <string.h>

int main(void)
{
	char str[202];
	gets(str);     	//��ȡ���ո�Ĵ� 
	int i = 0, j = 0;
	int flag = 1;
	while (str[j])    	//��ɨ�裬���û�е���β 
	{
		if (str[j] != ' ')     	//�ǿո��ַ� 
		{
			str[i++] = str[j];
			flag = 1;         	//��ʾ�´������ո�ʱ���ǵ�һ���ո� 
		}
		else if (str[j] == ' ' && flag) 	//���ʺ�ĵ�һ���ո�
		{
			str[i++] = str[j];
			flag = 0;  	//��ʾ�´������ո�ʱ�����ǵ��ʺ�ĵ�һ���ո� 
		}
		j++;
	}
	str[i] = '\0';      	//�ڴ�β���ַ���������� 
	printf("%s", str);
	return 0;
}

#include<string.h>
#include<stdio.h>

int main(void)
{
	int shu[1000];   	//�洢�Ӵ�����ȡ������ 
	char str[500];
	scanf("%s", str);
	int k = 0;
	int sum = 0;
	int len = strlen(str);    	//������ 
	for (int i = 0; i < len; i++)  	//��ɨ�� 
	{
		if (str[i] >= '0' && str[i] <= '9') 	//һ��ɨ�赽�����ַ�����ȡ������ʼ 
		{
			sum = str[i] - '0';
			int j = i + 1;
			while (j < len && str[j] >= '0' && str[j] <= '9')	//����ֱ����β
													//�������������ַ� 
			{
				sum = sum * 10 + str[j] - '0';
				j++;
			}
			shu[k++] = sum;
			i = j;  	//�޸�iֵ��i�±�λ�õ��ַ����߷����֣��򵽴�β
		}
	}
	for (int i = 0; i < k; i++)    	//����Ӵ�����ȡ��������������
		printf("%d\n", shu[i]);

	return 0;
}

#include<string.h>
#include<stdio.h>
#include<stdlib.h>   	//atoi()

int main(void)
{
	int shu[1000];   	//�洢���ַ�������ȡ������ 
	char str[500];
	scanf("%s", str);
	int k = 0;
	int len = strlen(str);
	for (int i = 0; i < len; i++)   	//���з������ַ��ÿո��滻
		if (str[i] < '0' || str[i] >'9')  str[i] = ' ';

	char* pch = strtok(str, " ");  	//�ÿո�ָ��ַ���
	while (pch)
	{
		shu[k++] = atoi(pch);  	//atoi()�������԰������ַ���ת�������� 
		pch = strtok(NULL, " ");  	//��һ���Ӵ�  
	}
	for (int i = 0; i < k; i++)
		printf("%d\n", shu[i]);

	return 0;
}

#include <stdio.h>
#include <string.h>
#define MAXLEN 1001

void Invert(char* s, int* a); 	//����s��ÿ�������ַ�ת�������������ô�ŵ�a��
char str1[MAXLEN], str2[MAXLEN], str[2 * MAXLEN];	//��ų������˻���Ӧ�Ĵ�
int a[MAXLEN], b[MAXLEN];      	//��ų����ĸ���λ
int  c[2 * MAXLEN];              	//��ų˻��ĸ���λ  

int main(void)
{
	//����ǰ��׼��		
	printf("��������������\n");
	scanf("%s %s", str1, str2);  	//�Դ�����ʽ��������������	
	//���������������ò�ת����������ŵ�����a,b��
	int len1 = strlen(str1), len2 = strlen(str2);

	//ȷ�����ķ��� 
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
	//��λ����	

	memset(c, 0, sizeof(c));
	for (int i = 0; i < len2; i++)
	{
		for (int j = 0; j < len1; j++)
			c[i + j] += a[j] * b[i];
	}
	//�����λ
	for (int i = 0; i < len1 + len2; i++)
	{
		c[i + 1] += c[i] / 10;
		c[i] %= 10;
	}
	//�������
	//�����λ��len1+len2-1��ǰ��0ȥ����ת�����ַ���ŵ�����str��
	//str[0]Ϊ���λ 
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
int result[N];      	//���n!ֵ�ĸ���λ�ϵ�����result[0]��Ÿ�λ

int main(void)
{
	int n;                         	//���n 

	printf("����n!������n��ֵ��");  	//��ʾ����n 
	scanf("%d", &n);
	memset(result, 0, N);
	result[0] = 1;              	//�۳�����ʼ��ֵ1
	int digits = 1;             	//��Ž����λ������ʼʱλ��Ϊ1 
	for (int i = 2; i <= n; i++)
	{//1*2*...*n 
		int  jw = 0;	//jw��ŵ�λ���λ�Ľ�λ��	
		for (int j = 0; j < digits; j++)   	//��result�Ӹ�λ������digitsλ���γ��� i
		{//�����jλ����result[j]��			
			int total = result[j] * i + jw;
			result[j] = total % 10;
			jw = total / 10;	//�����λjw��ֵ     
			if (j == digits - 1 && jw)  	//�����digitsλ(���λ)�н�λ����λ����1 
				digits++;
		}
	}
	char factarr[N];             	//���result�Ĵ� 
	memset(factarr, 0, N);
	for (int i = digits - 1, j = 0; i >= 0; i--)
	{
		factarr[j++] = result[i] + '0';
	}
	printf("%d! = %s\n", n, factarr);   	//���n!=? 

	return 0;
}

#define SET_BIT(a, i)   (a |= 1 << (i))  
#define CLS_BIT(a, i)	(a &= ~(1 << (i)))


#include <stdio.h>

int main(void)
{
	int a = 0xFBA;  	//Ϊ���ڷ�������ֱ�Ӹ� a ��ֵ	
	a &= 0x3F << 3;                       	//��0x3f����3λ���������aλ������
	a >>= 3;               	//���ɵõ� a��bit3~bit8��Ӧ������ 
	printf("a = %#X\n", a);   	//��a��ʮ�����Ƹ�ʽ�������ĸΪ��д

	return 0;
}


#include <stdio.h>

int main(void)
{
	int a = 0xACF71;  	//Ϊ���ڷ�������ֱ�Ӹ� a ��ֵ
	a &= ~(0x7FF << 7);/*0x7FF��Ӧ����Ϊ111 1111 1111������7λ��ȡ������
			�ɹ���һ��bit7~bit17Ϊ0������λΪ1��������a����
			�������������λ�����㼴�ɽ�a��bit7~bit17��0����
			��λ����*/
	a |= 937 << 7;  	//ʵ�ֽ�a��bit7~bit17��ֵ937����������λ����
	printf("a = %#X\n", a);	//��a��ʮ�����Ƹ�ʽ�������ĸΪ��д

	return 0;
}


#include <stdio.h>

int main(void)
{
	int a = 0xACF71;      	//Ϊ���ڷ�������ֱ�Ӹ� a ��ֵ
	unsigned tmp;    	//���ڴ�żĴ���bit7~bit17ԭ����ֵ������17���ֵ

	tmp = a & (0x7FF << 7); 	//��a�� bit7~bit17Ϊ1����λΪ0��������λ�����
	tmp >>= 7;            	//��ȡa��bit7~bit17ֵ		
	tmp += 17;           	//tmp Ϊ�Ĵ���bit7~bit17���յ�ֵ
	a &= ~(0x7FF << 7);  	//�� a �� bit7~bit17��0������λ���� 
	a |= tmp << 7;         	//��bit7~bit17��ֵtmp����������λ���� 
	printf("a = %#X\n", a);  	//��a��ʮ�����Ƹ�ʽ�������ĸΪ��д

	return 0;
}


#include<stdio.h>
int CountOne(int a);

int main(void)
{
	int a;

	printf("����һ��������");
	scanf("%d", &a);
	printf("%d�Ķ����Ʊ�ʾ��1�ĸ�����%d\n", a, CountOne(a));

	return 0;
}
int CountOne(int a)
{
	int c = 0;        	//��¼a�Ķ����Ʊ�ʾ��1�ĸ��� 
	while (a)
	{
		c++;
		a &= a - 1;    	//��ȥ��a�Ķ����Ʊ�ʾ�����ұߵ�1 
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
int AppearOnce(int* a, int n);   	 //����n��Ԫ�ص�����a��ֻ����һ�ε�Ԫ��

int main(void)
{
	int t, n;           	 //��Ų������ݵ���������ÿ��������ݵ�Ԫ�ظ���
	scanf("%d", &t);
	while (t--)
	{
		scanf("%d", &n);
		int* a = (int*)malloc(sizeof(int) * n);	//��̬�����ܴ��n�������������� 
		for (int i = 0; i < n; i++)
			scanf("%d", &a[i]);
		int OnceNum = AppearOnce(a, n);	 //��n��Ԫ������a��ֻ����һ�ε�Ԫ�� 
		printf("%d\n", OnceNum);
		free(a);                           	//�ͷſռ� 
	}

	return 0;
}

int AppearOnce(int* a, int n)
{
	int num = 0;
	//Ԫ���������Ϊ��ͬԪ�������Ϊ0 
	//����num���������Ϊֻ����һ�ε�Ԫ��
	for (int i = 0; i < n; i++)
		num ^= a[i];
	return num;
}


#include <stdio.h>
void SubSet(int* a, int i);           //���i��Ӧ����������1����λ�϶�ӦԪ�ع��ɵļ���

int main(void)
{
	int a[] = { 1,2,3 }, N;

	N = sizeof(a) / sizeof(int);
	int t = 1 << N;
	for (int i = 1; i < t; i++)                //��1��2��...��2N-1�Ķ���������1λ�϶�Ӧ��Ԫ����� 
		SubSet(a, i);
	return 0;
}
void SubSet(int* a, int i)
{
	int k, flag = 0;                      //k��Ӧa��Ԫ���±�		
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






