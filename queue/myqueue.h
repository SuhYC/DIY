#pragma once

#include <memory>
#include <iostream>

// Singly Linked List Version
template <typename T>
class MyQueueS
{
public:
	MyQueueS() { mpHead = nullptr; mpTail = nullptr; mSize = 0; }
	~MyQueueS() {}
	
	MyQueueS& operator=(MyQueueS& other_)
	{
		mpHead = other_.mpHead;
		mpTail = other_.mpTail;
		mSize = other_.mSize;

		return *this;
	}
	
	inline bool empty() { return mSize == 0; }
	inline void push(T& data_)
	{
		if (mSize == 0)
		{
			mpHead = std::make_shared<Node>(data_);
			mpTail = mpHead;
		}
		else
		{
			mpTail->mpNext = std::make_shared<Node>(data_);
			mpTail = mpTail->mpNext;
		}
		mSize++;
	}

	inline void push(T&& data_)
	{
		if (mSize == 0)
		{
			mpHead = std::make_shared<Node>(data_);
			mpTail = mpHead;
		}
		else
		{
			mpTail->mpNext = std::make_shared<Node>(data_);
			mpTail = mpTail->mpNext;
		}
		mSize++;
	}

	inline void pop()
	{
		if (mSize > 1)
		{
			mpHead = mpHead->mpNext;
			mSize--;
		}
		else if (mSize == 1)
		{
			mpHead = nullptr;
			mpTail = nullptr;
			mSize--;
		}
	}
	inline size_t size() { return mSize; }
	inline T& front() { return mpHead->mData; }
	inline T& back() { return mpTail->mData; }

private:

	class Node
	{
	public:
		Node(T& data_) : mData(data_) { mpNext = nullptr; }
		Node(T&& data_) : mData(data_) { mpNext = nullptr; }
		~Node() { std::cout << "discarded.\n"; }
	private:
		T mData;
		std::shared_ptr<Node> mpNext;
		friend class MyQueueS;
	};

	std::shared_ptr<Node> mpHead;
	std::shared_ptr<Node> mpTail;
	size_t mSize;
};

// Circular Array Version.
template<typename T>
class MyQueueC
{
public:
	// HeadIdx와 TailIdx로 구현할 때
	// HeadIdx == TailIdx -> no Data
	// HeadIdx == TailIdx+1 mod size -> Full로 구현하니 1자리를 못쓴다..
	// 더 나은 방법 생각날 때까지는 일단 메모리 한칸을 더 먹기로 했다...
	MyQueueC(size_t size_ = 10) : mSize(size_ + 1)
	{
		p = new T[size_ + 1];
		mHead = 0;
		mTail = 0;
	}
	~MyQueueC() { delete[] p; }

	inline bool empty() { return mHead == mTail; }
	inline bool isfull() { return mHead == ((mTail + 1) % mSize); }
	inline T& front() { return p[mHead]; }
	inline T& back() { return p[mTail]; }
	
	inline void push(T& data_)
	{
		if (!isfull())
		{
			p[mTail] = data_;
			mTail = (mTail + 1) % mSize;
		}
		else
		{
			std::cout << "full\n";
		}
	}
	inline void push(T&& data_)
	{
		if (!isfull())
		{
			p[mTail] = data_;
			mTail = (mTail + 1) % mSize;
		}
		else
		{
			std::cout << "full\n";
		}
	}

	inline void pop()
	{
		if (mHead != mTail)
		{
			mHead = (mHead + 1) % mSize;
		}
		else
		{
			std::cout << "empty\n";
		}
	}

private:
	T* p;
	size_t mSize;
	int mHead, mTail;
};