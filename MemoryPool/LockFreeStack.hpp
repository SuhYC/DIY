#pragma once

#include <iostream>
#include <stdint.h>
#include <Windows.h>
#include <atomic>

#include "Stack.hpp"

/// <summary>
/// ABA방지로 상위 12비트 stamp 사용.
/// 25년 현재는 일부 최신 프로세서가 LA57옵션을 제공하기도 함. 해당 경우 7비트 stamp 또는 다른 방식을 사용해야함.
/// </summary>
class alignas(64) LockFreeStack
{
public:
	LockFreeStack() : m_Head(nullptr), m_FreeList(nullptr), pad1{}, pad2{}
	{
		for (int32_t i = 0; i < NODE_COUNT; i++)
		{
			Node* pNode = new(std::nothrow) Node();
			if (pNode != nullptr)
			{
				PushFreeList(pNode);
			}
		}
	}

	~LockFreeStack()
	{
		if (m_Head != nullptr)
		{
			Node* pNode = m_Head.load();
			Node* removeNode = RemoveStamp(pNode);
			delete removeNode;
		}

		if (m_FreeList != nullptr)
		{
			Node* pNode = m_FreeList.load();
			Node* removeNode = RemoveStamp(pNode);
			delete removeNode;
		}
	}

	void Push(void* data_)
	{
		if (data_ == nullptr)
		{
			return;
		}

		Node* oldtop;
		Node* newtop = PopFreeList();
		Node* stampNewTop;

		newtop->data = data_;

		do
		{
			oldtop = m_Head.load();
			newtop->next = oldtop;

			stampNewTop = AddStamp(newtop);
		} while (!m_Head.compare_exchange_weak(oldtop, stampNewTop));

	}

	void* Pop()
	{
		Node* oldtop;
		Node* newtop;
		Node* removeOldTop;

		do
		{
			oldtop = m_Head.load();

			if (oldtop == nullptr)
			{
				return nullptr;
			}

			removeOldTop = RemoveStamp(oldtop);

			newtop = removeOldTop->next;
		} while (!m_Head.compare_exchange_weak(oldtop, newtop));

		void* pRet = removeOldTop->data;

		PushFreeList(removeOldTop);

		return pRet;
	}

	void BatchPop(Stack& target_, int16_t maxNode_ = 20)
	{
		Node* oldtop;
		Node* newtop;
		Node* removeOldTop;
		Node* removeNewTop;

		int32_t batch_cnt = 1;

		do
		{
			oldtop = m_Head.load();

			if (oldtop == nullptr)
			{
				return;
			}

			removeOldTop = RemoveStamp(oldtop);
			newtop = removeOldTop->next;

			batch_cnt = 1;

			for (int32_t i = 1; i < maxNode_; i++)
			{
				if (newtop == nullptr)
				{
					break;
				}
				batch_cnt++;
				removeNewTop = RemoveStamp(newtop);
				newtop = removeNewTop->next;

			}
		} while (!m_Head.compare_exchange_weak(oldtop, newtop));

		Node* now = oldtop;
		Node* removeNow;
		Node* tail = RemoveStamp(now);

		while (batch_cnt--)
		{
			removeNow = RemoveStamp(now);

			if (batch_cnt == 0)
			{
				tail = removeNow;
			}

			target_.Push(removeNow->data);
			now = removeNow->next;
		}

		Node* oldHead; // freelist
		Node* stampHead;

		do
		{
			oldHead = m_FreeList.load();
			tail->next = oldHead;
			stampHead = AddStamp(removeOldTop);
		} while (!m_FreeList.compare_exchange_weak(oldHead, stampHead));

		return;
	}

private:
	class Node
	{
	public:
		Node() : data(nullptr), next(nullptr)
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
				Node* removeNext = RemoveStamp(next);

				delete removeNext;
			}
		}
		
		void Init(void* data_)
		{
			data = data_;
		}

		Node* next;
		void* data;
	};

	void PushFreeList(Node* pNode_)
	{
		if (pNode_ == nullptr)
		{
			return;
		}

		Node* oldHead;
		Node* stampHead;
		do
		{
			oldHead = m_FreeList.load();
			pNode_->next = oldHead;

			stampHead = AddStamp(pNode_);
		} while (!m_FreeList.compare_exchange_weak(oldHead, stampHead));
	}

	Node* PopFreeList()
	{
		Node* pRet{};
		Node* newHead;
		Node* removeHead;

		do
		{
			pRet = m_FreeList.load();
			if (pRet == nullptr)
			{
				pRet = new(std::nothrow) Node();
				if (pRet == nullptr)
				{
					std::cerr << "Not Enough Memory.\n";
				}
				return pRet;
			}

			removeHead = RemoveStamp(pRet);
			newHead = removeHead->next;
		} while (!m_FreeList.compare_exchange_weak(pRet, newHead));

		return removeHead;
	}
	
	static Node* AddStamp(Node* pNode_)
	{
		return reinterpret_cast<Node*>((m_Stamp++ << 52) | reinterpret_cast<uintptr_t>(pNode_));
	}

	static Node* RemoveStamp(Node* pNode_)
	{
		return reinterpret_cast<Node*>(reinterpret_cast<uintptr_t>(pNode_) & (0x000FFFFFFFFFFFFFU));
	}

	static std::atomic_uint64_t m_Stamp;
	char pad1[56];
	std::atomic<Node*> m_Head;
	char pad2[56];
	std::atomic<Node*> m_FreeList;
	const int32_t NODE_COUNT = 100;
};

std::atomic_uint64_t LockFreeStack::m_Stamp = 0;

class LockFreeStackNoAlign
{

public:
	LockFreeStackNoAlign() : m_Head(nullptr), m_FreeList(nullptr)
	{
		for (int32_t i = 0; i < NODE_COUNT; i++)
		{
			Node* pNode = new(std::nothrow) Node();
			if (pNode != nullptr)
			{
				PushFreeList(pNode);
			}
		}
	}

	~LockFreeStackNoAlign()
	{
		if (m_Head != nullptr)
		{
			Node* pNode = m_Head.load();
			Node* removeNode = RemoveStamp(pNode);
			delete removeNode;
		}

		if (m_FreeList != nullptr)
		{
			Node* pNode = m_FreeList.load();
			Node* removeNode = RemoveStamp(pNode);
			delete removeNode;
		}
	}

	void Push(void* data_)
	{
		if (data_ == nullptr)
		{
			return;
		}

		Node* oldtop;
		Node* newtop = PopFreeList();
		Node* stampNewTop;

		newtop->data = data_;

		do
		{
			oldtop = m_Head.load();
			newtop->next = oldtop;

			stampNewTop = AddStamp(newtop);
		} while (!m_Head.compare_exchange_weak(oldtop, stampNewTop));

	}

	void* Pop()
	{
		Node* oldtop;
		Node* newtop;
		Node* removeOldTop;

		do
		{
			oldtop = m_Head.load();

			if (oldtop == nullptr)
			{
				return nullptr;
			}

			removeOldTop = RemoveStamp(oldtop);

			newtop = removeOldTop->next;
		} while (!m_Head.compare_exchange_weak(oldtop, newtop));

		void* pRet = removeOldTop->data;

		PushFreeList(removeOldTop);

		return pRet;
	}

	void BatchPop(Stack& target_, int16_t maxNode_ = 20)
	{
		Node* oldtop;
		Node* newtop;
		Node* removeOldTop;
		Node* removeNewTop;

		int32_t batch_cnt = 1;

		do
		{
			oldtop = m_Head.load();

			if (oldtop == nullptr)
			{
				return;
			}

			removeOldTop = RemoveStamp(oldtop);
			newtop = removeOldTop->next;

			batch_cnt = 1;

			for (int32_t i = 1; i < maxNode_; i++)
			{
				if (newtop == nullptr)
				{
					break;
				}
				batch_cnt++;
				removeNewTop = RemoveStamp(newtop);
				newtop = removeNewTop->next;

			}
		} while (!m_Head.compare_exchange_weak(oldtop, newtop));

		Node* now = oldtop;
		Node* removeNow;
		Node* tail = RemoveStamp(now);

		while (batch_cnt--)
		{
			removeNow = RemoveStamp(now);

			if (batch_cnt == 0)
			{
				tail = removeNow;
			}

			target_.Push(removeNow->data);
			now = removeNow->next;
		}

		Node* oldHead; // freelist
		Node* stampHead;

		do
		{
			oldHead = m_FreeList.load();
			tail->next = oldHead;
			stampHead = AddStamp(removeOldTop);
		} while (!m_FreeList.compare_exchange_weak(oldHead, stampHead));

		return;
	}

private:
	class Node
	{
	public:
		Node() : data(nullptr), next(nullptr)
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
				Node* removeNext = RemoveStamp(next);

				delete removeNext;
			}
		}

		void Init(void* data_)
		{
			data = data_;
		}

		Node* next;
		void* data;
	};

	void PushFreeList(Node* pNode_)
	{
		if (pNode_ == nullptr)
		{
			return;
		}

		Node* oldHead;
		Node* stampHead;
		do
		{
			oldHead = m_FreeList.load();
			pNode_->next = oldHead;

			stampHead = AddStamp(pNode_);
		} while (!m_FreeList.compare_exchange_weak(oldHead, stampHead));
	}

	Node* PopFreeList()
	{
		Node* pRet{};
		Node* newHead;
		Node* removeHead;

		do
		{
			pRet = m_FreeList.load();
			if (pRet == nullptr)
			{
				pRet = new(std::nothrow) Node();
				if (pRet == nullptr)
				{
					std::cerr << "Not Enough Memory.\n";
				}
				return pRet;
			}

			removeHead = RemoveStamp(pRet);
			newHead = removeHead->next;
		} while (!m_FreeList.compare_exchange_weak(pRet, newHead));

		return removeHead;
	}

	static Node* AddStamp(Node* pNode_)
	{
		return reinterpret_cast<Node*>((m_Stamp++ << 52) | reinterpret_cast<uintptr_t>(pNode_));
	}

	static Node* RemoveStamp(Node* pNode_)
	{
		return reinterpret_cast<Node*>(reinterpret_cast<uintptr_t>(pNode_) & (0x000FFFFFFFFFFFFFU));
	}

	static std::atomic_uint64_t m_Stamp;
	std::atomic<Node*> m_Head;
	std::atomic<Node*> m_FreeList;
	const int32_t NODE_COUNT = 100;
};

std::atomic_uint64_t LockFreeStackNoAlign::m_Stamp = 0;