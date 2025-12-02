#pragma once

#include <stdint.h>
#include <new>
#include <iostream>


/// <summary>
/// Not Thread-safe
/// For TLS
/// </summary>
class alignas(64) Stack
{
public:
	Stack() : m_freeList(nullptr), m_Head(nullptr)
	{
		for (int32_t i = 0; i < NODE_COUNT; i++)
		{
			Node* pNode = new(std::nothrow) Node();

			if (pNode != nullptr)
			{
				pNode->next = m_freeList;
				pNode = m_freeList;
			}
		}
	}

	~Stack()
	{
		if (m_freeList != nullptr)
		{
			delete m_freeList;
		}

		if (m_Head != nullptr)
		{
			delete m_Head;
		}
	}

	void Push(void* data_)
	{
		if (data_ == nullptr)
		{
			return;
		}

		Node* pNode = nullptr;

		if (m_freeList != nullptr)
		{
			pNode = m_freeList;
			m_freeList = m_freeList->next;
			pNode->data = data_;
		}
		else
		{
			pNode = new(std::nothrow) Node(data_);
			if (pNode == nullptr)
			{
				std::cerr << "Not Enough Memory.\n";
				return;
			}
		}

		pNode->next = m_Head;
		m_Head = pNode;

		return;
	}

	void* Pop()
	{
		if (m_Head == nullptr)
		{
			return nullptr;
		}

		Node* pNode = m_Head;
		m_Head = m_Head->next;

		void* ret = pNode->data;

		pNode->data = nullptr;
		pNode->next = m_freeList;
		m_freeList = pNode;

		return ret;
	}

	bool IsEmpty()
	{
		return m_Head == nullptr;
	}

private:
	class Node
	{
	public:
		Node() : next(nullptr), data{}
		{

		}

		Node(void* data_) : data(data_), next(nullptr)
		{

		}

		~Node()
		{
			if (data != nullptr)
			{
				::operator delete(data);
			}
			
			if (next != nullptr)
			{
				delete next;
			}
		}

		void Init(void* data_)
		{
			data = data_;
			next = nullptr;
		}

		Node* next;
		void* data;
	};

	Node* m_freeList;
	Node* m_Head;
	const int32_t NODE_COUNT = 30;
};