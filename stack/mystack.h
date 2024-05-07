#pragma once
#include <memory>

template<typename T>
class MyStack
{
public:
	MyStack() { mpHead = nullptr; mSize = 0; }
	MyStack& operator=(MyStack& other_)
	{
		mpHead = other_.mpHead;
		mSize = other_.mSize;

		return *this;
	}

	MyStack& operator=(MyStack&& other_)
	{
		mpHead = other_.mpHead;
		mSize = other_.mSize;

		return *this;
	}

	void push(T& data_)
	{
		std::shared_ptr<Node> tmp = std::make_shared<Node>(data_);
		tmp->mpNext = mpHead;
		mpHead = tmp;
		mSize++;
	}

	void push(T&& data_)
	{
		std::shared_ptr<Node> tmp = std::make_shared<Node>(data_);
		tmp->mpNext = mpHead;
		mpHead = tmp;
		mSize++;
	}

	void pop()
	{
		mpHead = mpHead->next;
		mSize--;
	}

	T& top() { return mpHead->mData; }
	size_t size() { return mSize; }
	bool empty() { return mSize == 0; }
	void swap(MyStack& other_)
	{
		std::swap(mSize, other_.mSize);
		std::swap(mpHead, other_.mpHead);
	}

private:
	class Node
	{
	public:
		Node(T& data_) : mData(data_) { mpNext = nullptr; }
		Node(T&& data_) : mData(data_) { mpNext = nullptr; }
	private:
		T mData;
		std::shared_ptr<Node> mpNext;
		friend class MyStack;
	};
	std::shared_ptr<Node> mpHead;
	size_t mSize;
};