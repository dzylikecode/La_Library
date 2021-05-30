/*
***************************************************************************************
*  ��    ��: 
*
*  ��    ��: LaDzy
*
*  ��    ��: mathbewithcode@gmail.com
*
*  ���뻷��: Visual Studio 2019
*
*  ����ʱ��: 2021/05/20 11:10:46
*  ����޸�: 
*
*  ��    ��: 
*
***************************************************************************************
*/

#ifndef LA_LINEARLIST
#define LA_LINEARLIST

#include <cassert>

template <class T>
class ARRAY
{
private:
	T* list;
	int size;
public:
	ARRAY(int sz = 0);
	ARRAY(const ARRAY<T>& a);
	~ARRAY() { clear(); }
	ARRAY<T>& operator = (const ARRAY<T>& rhs);
	T& operator[] (int i) { assert(i >= 0 && i < size); return list[i]; }
	const T& operator[] (int i) const { assert(i >= 0 && i < size); return list[i]; }
	operator T* () { return list; }//��������ת��
	operator const T* () const { return list; }
	int getSize() const { return size; }
	void clear() 
	{
		if (list)
		{
			delete[] list;
			list = nullptr;
			size = 0;
		}
	}
	bool resize(int sz);
};

template <class T>
ARRAY<T>::ARRAY(int sz):list(nullptr)
{
	size = sz;
	if (sz == 0)
		return;
	assert(sz > 0);
	list = new T[size];
}

template <class T>
ARRAY<T>::ARRAY(const ARRAY<T>& a)
{
	size = a.size;
	list = new T[size];
	for (int i = 0; i < size; i++)
		list[i] = a.list[i];
}

template <class T>
ARRAY<T>& ARRAY<T>::operator =(const ARRAY<T>& rhs)
{
	if (&rhs != this)
	{
		if (size != rhs.size)
		{
			delete[]list;
			size = rhs.size;
			list = new T[size];
		}
		for (int i = 0; i < size; i++)
			list[i] = rhs.list[i];
	}
	return *this;
}

template <class T>
bool ARRAY<T>::resize(int sz)
{
	if (sz == size)
		return true;

	if (sz == 0)
	{
		size = 0;
		delete[] list;
		list = nullptr;
		return true;
	}
	assert(sz > 0);
	T* newList = new T[sz];

	if (!newList)
		return false;

	int n = (sz < size) ? sz : size;
	for (int i = 0; i < n; i++)
		newList[i] = list[i];
	delete[]list;
	list = newList;
	size = sz;
	return true;
}


template <typename T>
class NODE
{
public:
	NODE<T>* next;	//ָ���̽���ָ��
	T data;	//������
	NODE(const T& data, NODE<T>* next = nullptr) : data(data), next(next) {};    //���캯��
	NODE() :next(nullptr) {};
	void insertAfter(NODE<T>* p)	//�ڱ����֮�����һ��ͬ����p 
	{
		p->next = next;	//p ���ָ����ָ��ǰ���ĺ�̽��
		next = p;		//��ǰ����ָ����ָ��p 
	}
	NODE<T>* deleteAfter()	//ɾ�������ĺ�̽�㣬���������ַ
	{
		NODE<T>* tempPtr = next;	//����ɾ���Ľ���ַ�洢��tempPtr��
		if (next == 0)	//�����ǰ���û�к�̽�㣬�򷵻ؿ�ָ��
			return 0;
		next = tempPtr->next;	//ʹ��ǰ����ָ����ָ��tempPtr�ĺ�̽��
		return tempPtr;			//���ر�ɾ���Ľ��ĵ�ַ
	}
};
/*
* �����������սڵ�front��rear��������ĽṹΪfront->a1->a2->...->rear
* ��Ϊ���� current �� preview 
*/
template <typename T>
class LINKEDLIST 
{
private:
	NODE<T>* front, * rear; //��ͷ�ͱ�βָ��
	NODE<T>* prevPtr, * currPtr; //��¼��ǰ����λ�õ�ָ�룬�ɲ����ɾ����������
	int size; //����Ԫ�ظ���
	int position; //��ǰԪ���ڱ��е�λ����š��ɺ���resetʹ��

	//�����½ڵ㣬������Ϊitem��ָ����ΪprtNext
	NODE<T>* newNode(const T& item, NODE<T>* ptrNext = nullptr) 
	{
		NODE<T>* n = new NODE<T>(item, ptrNext);
		return n;
	}
	void freeNode(NODE<T>* p); //�ͷŽڵ�

	void copy(const LINKEDLIST<T>& L); //������L���Ƶ���ǰ�����赱ǰ��Ϊ�գ��������ƹ��캯����operator=����

public:
	LINKEDLIST()//���캯��
	{
		front = new NODE<T>();
		rear = new NODE<T>();
		front->next = rear;
		currPtr = rear;
		prevPtr = front;
		size = 0; //������front��rear
		position = 0; //��front��һ��Ԫ����Ϊ0
	}
	LINKEDLIST(const LINKEDLIST<T>& L) { copy(L); } //���ƹ��캯��
	~LINKEDLIST() //��������
	{
		clear();
		delete front;
		delete rear;
	}
	LINKEDLIST<T>& operator = (const LINKEDLIST<T>& L) //���ظ�ֵ�����
	{
		if (this != &L)
		{
			clear();
			copy(L);
		}
		return *this;
	}

	int getSize() const { return size; } //����������Ԫ�ظ���
	bool isEmpty() const { return !size; } //�����Ƿ�Ϊ��

	void reset(int pos = 0); //��ʼ���α��λ��
	void next() //ʹ�α��ƶ�����һ���ڵ�
	{
		assert(currPtr != rear);
		currPtr = currPtr->next;
		prevPtr = prevPtr->next;
		position++;
	}
	bool endOfList() const { return (currPtr == rear); } //�α��Ƿ��ƶ�����β
	int currentPosition() const { return position; } //�����α굱ǰ��λ��

	void insertFront(NODE<T>* n)
	{
		n->next = front;
		front = n;
		size++;
		position++;
	}
	void insertFront(const T& item) //�ڱ�ͷ����ڵ�
	{
		NODE<T>* n = new NODE<T>(item, front);
		insertFront(n);
	}
	
	void insertRear(NODE<T>* n)
	{
		NODE<T>* temp = front;
		while (temp->next != rear)
			temp = temp->next;
		n->next = rear;
		temp->next = n;
		size++;
	}
	void insertRear(const T& item) //�ڱ�β����ڵ�
	{
		NODE<T>* n = new NODE<T>();
		rear->next = n;
		rear->data = item;
		rear = n;
		size++;
	}

	//��ǰ�ڵ�֮ǰ�����ı䵱ǰָ��  ----> ����
	void insertBefore(NODE<T>* temp)
	{
		temp->next = currPtr;
		prevPtr->next = temp;
		prevPtr = temp;
		size++;
		position++;
	}
	void insertBefore(const T& item) //�ڵ�ǰ�ڵ�֮ǰ����ڵ�
	{
		NODE<T>* temp = new NODE<T>(item, currPtr);
		insertBefore(temp);
	}

	//��ǰ�ڵ�֮ǰ����ָ������Ľڵ� ---> ����
	void insertAt(NODE<T>* n)
	{
		n->next = currPtr;
		prevPtr->next = n;
		currPtr = n;
		size++;
	}
	void insertAt(const T& item)
	{
		NODE<T>* n = new NODE<T>(item, currPtr);
		insertAt(n);
	}
	void insertAt()//����Ϊʹ�÷��㣬����ӽڵ㣬Ȼ������д������
	{
		NODE<T>* n = new NODE<T>();
		insertAt(n);
	} 

	//��ǰ�ڵ�֮�󣬲��ı䵱ǰ��ָ�� ---->���򣬵���ע���β
	void insertAfter(NODE<T>* temp)
	{
		assert(!endOfList());
		temp->next = currPtr->next;
		currPtr->next = temp;
		size++;
	}
	void insertAfter(const T& item) //�ڵ�ǰ�ڵ�֮�����ڵ㣬��ǰ������ĩβ
	{
		NODE<T>* temp = new NODE<T>(item, currPtr->next);
		insertAfter(temp);
	}

	T deleteFront(); //ɾ��ͷ�ڵ�
	void deleteCurrent() //ɾ����ǰ�ڵ�
	{
		if (endOfList())
		{
			return;
		}
		NODE<T>* temp = currPtr;
		currPtr = currPtr->next;
		delete temp;
		size--;
	}

	T& data() { return currPtr->data; } //���ضԵ�ǰ�ڵ��Ա���ݵ�����
	const T& data() const { return currPtr->data; } //���ضԵ�ǰ�ڵ��Ա���ݵĳ�����

	void clear(); //��������ͷ����нڵ���ڴ�ռ䣬������������operator=ʹ��

	const T& operator[](int i)const;
	T& operator[](int i) { return const_cast<T&>((*static_cast<const LINKEDLIST*>(this))[i]); }
};

template <class T>
const T& LINKEDLIST<T>::operator[](int pos)const
{
	assert(pos >= 0 && pos < size);
	int i = 0;
	NODE<T>* temp = front->next;
	while (i < pos)
	{
		i++;
		temp = temp->next;
	}
	return temp->data;
}

//�ͷŽڵ�
template <class T>
void LINKEDLIST<T>::freeNode(NODE<T>* p)
{
	NODE<T>* temp = front;
	while (temp->next != p) 
	{
		temp = temp->next;
		if (temp == currPtr)//�����Ƿ񾭹� current
			position++;
	}
	//�� p ǰһ��ͣ����
	temp->next = p->next;
	//������Ӧ��ָ��
	if (currPtr == p)
		currPtr = currPtr->next;
	if (prevPtr == p) 
	{
		prevPtr = prevPtr->next;
		currPtr = currPtr->next;
	}
	delete p;
	size--;
	position--;
}

//������L���Ƶ���ǰ�����赱ǰ��Ϊ�գ��������ƹ��캯����operator=����
template <class T>
void LINKEDLIST<T>::copy(const LINKEDLIST<T>& L) 
{
	NODE<T>* temp = L.front, * ptr = front;
	while (temp != L.rear) 
	{
		ptr->next = new NODE<T>(temp->data, NULL);
		ptr = ptr->next;
		temp = temp->next;
	}
	ptr->next = rear;
	size = L.getSize();
	position = L.currentPosition();
}


//��ʼ���α��λ��
template <class T>
void LINKEDLIST<T>::reset(int pos) //��ʼ���α�λ��
{ 
	assert(pos >= 0 && pos <= size);//���� size ����Ϊ insertAt �� before
	int i = 0;
	prevPtr = front;
	currPtr = front->next;
	while (i < pos) 
	{
		i++;
		currPtr = currPtr->next;
		prevPtr = prevPtr->next;
	}
	position = pos;
}


//ɾ��ͷ�ڵ㣬ʵ����ɾ��front->next
template <class T>
T LINKEDLIST<T>::deleteFront()
{
	assert(front->next != rear);
	if (prevPtr == front->next)
	{
		prevPtr = prevPtr->next;
		currPtr = currPtr->next;
	}
	NODE<T>* temp = front->next;
	T d = temp->data;
	front->next = temp->next;
	delete temp;
	size--;
	if (front->next != rear)
		position--;
	return d;
}

//��������ͷ����нڵ���ڴ�ռ䣬������������operator=ʹ��
template <class T>
void LINKEDLIST<T>::clear() 
{
	NODE<T>* temp;
	while (front->next != rear) 
	{
		temp = front->next;
		front->next = temp->next;
		delete temp;
	}
	size = 0;
	position = 0;
}




#endif
