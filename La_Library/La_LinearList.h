/*
***************************************************************************************
*  程    序: 
*
*  作    者: LaDzy
*
*  邮    箱: mathbewithcode@gmail.com
*
*  编译环境: Visual Studio 2019
*
*  创建时间: 2021/05/20 11:10:46
*  最后修改: 
*
*  简    介: 
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
	operator T* () { return list; }//重载类型转化
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
	NODE<T>* next;	//指向后继结点的指针
	T data;	//数据域
	NODE(const T& data, NODE<T>* next = nullptr) : data(data), next(next) {};    //构造函数
	NODE() :next(nullptr) {};
	void insertAfter(NODE<T>* p)	//在本结点之后插入一个同类结点p 
	{
		p->next = next;	//p 结点指针域指向当前结点的后继结点
		next = p;		//当前结点的指针域指向p 
	}
	NODE<T>* deleteAfter()	//删除本结点的后继结点，并返回其地址
	{
		NODE<T>* tempPtr = next;	//将欲删除的结点地址存储到tempPtr中
		if (next == 0)	//如果当前结点没有后继结点，则返回空指针
			return 0;
		next = tempPtr->next;	//使当前结点的指针域指向tempPtr的后继结点
		return tempPtr;			//返回被删除的结点的地址
	}
};
/*
* 采用了两个空节点front和rear，即链表的结构为front->a1->a2->...->rear
* 是为方便 current 和 preview 
*/
template <typename T>
class LINKEDLIST 
{
private:
	NODE<T>* front, * rear; //表头和表尾指针
	NODE<T>* prevPtr, * currPtr; //记录表当前遍历位置的指针，由插入和删除操作更新
	int size; //表中元素个数
	int position; //当前元素在表中的位置序号。由函数reset使用

	//生成新节点，数据与为item，指针域为prtNext
	NODE<T>* newNode(const T& item, NODE<T>* ptrNext = nullptr) 
	{
		NODE<T>* n = new NODE<T>(item, ptrNext);
		return n;
	}
	void freeNode(NODE<T>* p); //释放节点

	void copy(const LINKEDLIST<T>& L); //将链表L复制到当前表（假设当前表为空），被复制构造函数和operator=调用

public:
	LINKEDLIST()//构造函数
	{
		front = new NODE<T>();
		rear = new NODE<T>();
		front->next = rear;
		currPtr = rear;
		prevPtr = front;
		size = 0; //不计算front和rear
		position = 0; //在front下一个元素视为0
	}
	LINKEDLIST(const LINKEDLIST<T>& L) { copy(L); } //复制构造函数
	~LINKEDLIST() //析构函数
	{
		clear();
		delete front;
		delete rear;
	}
	LINKEDLIST<T>& operator = (const LINKEDLIST<T>& L) //重载赋值运算符
	{
		if (this != &L)
		{
			clear();
			copy(L);
		}
		return *this;
	}

	int getSize() const { return size; } //返回链表中元素个数
	bool isEmpty() const { return !size; } //链表是否为空

	void reset(int pos = 0); //初始化游标的位置
	void next() //使游标移动到下一个节点
	{
		assert(currPtr != rear);
		currPtr = currPtr->next;
		prevPtr = prevPtr->next;
		position++;
	}
	bool endOfList() const { return (currPtr == rear); } //游标是否移动到链尾
	int currentPosition() const { return position; } //返回游标当前的位置

	void insertFront(NODE<T>* n)
	{
		n->next = front;
		front = n;
		size++;
		position++;
	}
	void insertFront(const T& item) //在表头插入节点
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
	void insertRear(const T& item) //在表尾插入节点
	{
		NODE<T>* n = new NODE<T>();
		rear->next = n;
		rear->data = item;
		rear = n;
		size++;
	}

	//当前节点之前，不改变当前指向  ----> 正序
	void insertBefore(NODE<T>* temp)
	{
		temp->next = currPtr;
		prevPtr->next = temp;
		prevPtr = temp;
		size++;
		position++;
	}
	void insertBefore(const T& item) //在当前节点之前插入节点
	{
		NODE<T>* temp = new NODE<T>(item, currPtr);
		insertBefore(temp);
	}

	//当前节点之前，并指向新添的节点 ---> 倒序
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
	void insertAt()//这是为使用方便，先添加节点，然后立马写入数据
	{
		NODE<T>* n = new NODE<T>();
		insertAt(n);
	} 

	//当前节点之后，不改变当前的指向 ---->正序，但是注意结尾
	void insertAfter(NODE<T>* temp)
	{
		assert(!endOfList());
		temp->next = currPtr->next;
		currPtr->next = temp;
		size++;
	}
	void insertAfter(const T& item) //在当前节点之后插入节点，当前不能是末尾
	{
		NODE<T>* temp = new NODE<T>(item, currPtr->next);
		insertAfter(temp);
	}

	T deleteFront(); //删除头节点
	void deleteCurrent() //删除当前节点
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

	T& data() { return currPtr->data; } //返回对当前节点成员数据的引用
	const T& data() const { return currPtr->data; } //返回对当前节点成员数据的常引用

	void clear(); //清空链表：释放所有节点的内存空间，被析构函数和operator=使用

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

//释放节点
template <class T>
void LINKEDLIST<T>::freeNode(NODE<T>* p)
{
	NODE<T>* temp = front;
	while (temp->next != p) 
	{
		temp = temp->next;
		if (temp == currPtr)//看它是否经过 current
			position++;
	}
	//在 p 前一个停下来
	temp->next = p->next;
	//处理相应的指针
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

//将链表L复制到当前表（假设当前表为空），被复制构造函数和operator=调用
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


//初始化游标的位置
template <class T>
void LINKEDLIST<T>::reset(int pos) //初始化游标位置
{ 
	assert(pos >= 0 && pos <= size);//允许到 size 是因为 insertAt 和 before
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


//删除头节点，实质是删除front->next
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

//清空链表：释放所有节点的内存空间，被析构函数和operator=使用
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
