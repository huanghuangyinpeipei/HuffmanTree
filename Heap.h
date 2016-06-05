#pragma once

#include <vector>
#include<iostream>
#include<assert.h>

using namespace std;

// С��
template<class T>
struct Less
{
	// operator<
	bool operator() (const T& left, const T& right)
	{
		return left < right;
	}
};
//���
template<class T>
struct Greater
{
	// operator>
	bool operator() (const T& left, const T& right)
	{
		return left> right;
	}
};


//template<class T, class Compare = Greater <T> >//���
template<class T, class Compare = Less<T> >//С��
class Heap
{
private:
	vector<T> _vec;
public:
	Heap()
	{}

	Heap(const T* array, size_t size)
	{
		assert(array);
		//��������
		for (int i = 0; i < size; ++i)
		{
			_vec.push_back(array[i]);
		}

		for (int i = _vec.size() / 2 - 1; i >= 0; --i)
		{
			_AdjustDown(_vec, i, _vec.size());
		}
	}

	Heap(const vector<T>& vec)
	{
		_vec.swap(vec);

		for (int i = _vec.size() / 2 - 1; i >= 0; --i)
		{
			_AdjustDown(_vec, i, _vec.size());
		}
	}

	// ����һ������x����С/�����
	void Push(const T& x)
	{
		_vec.push_back(x);

		if (_vec.size() > 1)
		{
			_AdjustUp(_vec, _vec.size() - 1);
		}
	}

	// ɾ���Ѷ�Ԫ��
	void Pop()
	{
		int size = _vec.size();
		assert(size > 0);

		swap(_vec[0], _vec[size - 1]);
		_vec.pop_back();

		if (_vec.size() > 1)
		{
			_AdjustDown(_vec, 0, _vec.size());
		}
	}

	const T& GetTop()
	{
		return _vec[0];
	}

	// �����ڵ����µ�����С�Ѿ�����
	void _AdjustDown(vector<T>& vec, int root, int size)
	{
		// 1.childָ�����ӽڵ�
		int child = root * 2 + 1;
		while (child < size)
		{
			// 2.childָ�����Һ�����С�Ľڵ�
			if (child + 1 < size && Compare()(vec[child + 1], vec[child]))
			{
				++child;
			}

			// 3.��childС�ڸ��ڵ㣬�򽻻�child��root�ڵ㣬���������µ���
			if (Compare()(vec[child], vec[root]))
				//if (vec[child] < vec[root])
			{
				swap(vec[child], vec[root]);
				root = child;
				child = root * 2 + 1;
			}
			else
			{
				break;
			}
		}
	}

	// һ���ڵ����ϵ�����С�Ѿ���˵����
	void _AdjustUp(vector<T>& vec, int pos)
	{
		// 1.parentָ�򸸽ڵ�
		int child = pos;
		int parent = (child - 1) / 2;

		// ��child==0ʱ�����ϵ���ɡ�����ʹ��parent���жϣ�parent����С��0
		//while(parent >= 0)
		while (child > 0)
		{
			// 2.��childС�ڸ��ڵ㣬�򽻻����ӽڵ㣬���������ϵ�����ֱ�����ڵ�
			if (Compare()(vec[child], vec[parent]))
				//if (vec[child] < vec[parent])
			{
				swap(vec[child], vec[parent]);
				child = parent;
				parent = (child - 1) / 2;
			}
			else
			{
				break;
			}
		}
	}

	// �ж��Ƿ�Ϊ��
	bool Empty()
	{
		return _vec.empty();
	}

	size_t Size()
	{
		return _vec.size();
	}

};

//���Զ�
void TestHeap()
{
	//��Ѳ���
	Heap<int, Greater<int>> heap;
	heap.Push(3);
	heap.Push(5);
	heap.Push(1);
	heap.Push(4);
	heap.Push(5);
	heap.Push(1);
	heap.Push(8);

	while (!heap.Empty())
	{
		cout << heap.GetTop() << " ";
		heap.Pop();
	}
	cout << endl;

	//С�Ѳ���
	//int array[10] = {9,1,3,5,6,7,8,0,2,4};
	int array[10] = { 10, 16, 18, 12, 11, 13, 15, 17, 14, 19 };
	Heap<int> heap1(array, 10);

	while (!heap1.Empty())
	{
		cout << heap1.GetTop() << " ";
		heap1.Pop();
	}
	cout << endl;
}