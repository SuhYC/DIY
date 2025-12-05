#pragma once

#include <iostream>
#include <stdint.h>
#include <Windows.h>
#include <atomic>

/// <summary>
/// Michael Scott 방식의 lock-free queue를 구현한다.
/// Dummy Node를 배치하여 head - tail double cas 필요성을 없앤다.
/// </summary>
class alignas(64) LockFreeQueue
{
public:
	LockFreeQueue() :pad1{}, pad2{}, pad3{}
	{
		Node* dummy = new(std::nothrow) Node();

		if (dummy == nullptr)
		{
			std::cerr << "Not Enough Memory.\n";
		}

		m_Head = dummy;
		m_Tail = dummy;

		for (uint32_t i = 0; i < NODE_COUNT; i++)
		{
			dummy = new(std::nothrow) Node();
			if (dummy != nullptr)
			{
				PushFreeList(dummy);
			}
		}
	}

	~LockFreeQueue()
	{

	}

	void Push(void* data_)
	{
		Node* pNode = PopFreeList();
		if (pNode == nullptr)
		{
			std::cerr << "Not Enough Memory\n";
			::operator delete(data_);
			return;
		}

		pNode->data = data_;
		pNode->next = nullptr;

		Node* pTail, * expected = nullptr, * pNext;
		Node* removeTail;
		Node* stampNode;

		while (true)
		{
			stampNode = AddStamp(pNode);
			pTail = m_Tail.load();
			removeTail = RemoveStamp(pTail);
			pNext = removeTail->next.load();

			if (pTail == m_Tail.load())
			{
				if (pNext == nullptr)
				{
					if (removeTail->next.compare_exchange_weak(pNext, stampNode))
					{
						m_Tail.compare_exchange_weak(pTail, stampNode);
						return;
					}
				}
				else
				{
					m_Tail.compare_exchange_weak(pTail, pNext);
				}
			}
		}
	}

	void* Pop()
	{
		Node* oldHead, * NewHead, * removeHead, *removeNewHead;
		void* pData;

		do
		{
			oldHead = m_Head.load();
			removeHead = RemoveStamp(oldHead);

			NewHead = removeHead->next.load();
			removeNewHead = RemoveStamp(NewHead);

			if (removeNewHead == nullptr)
			{
				return nullptr;
			}

			pData = removeNewHead->data;
		} while (!m_Head.compare_exchange_weak(oldHead, NewHead));


		removeHead->data = nullptr;
		removeHead->next = nullptr;

		PushFreeList(removeHead);

		return pData;
	}

private:
	class Node
	{
	public:
		Node() : next(nullptr), data(nullptr)
		{

		}

		Node(void* data_) : data(data_), next(nullptr)
		{

		}

		void Init(void* data_)
		{
			data = data_;
		}

		std::atomic<Node*> next;
		void* data;
	};

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

	Node* AddStamp(Node* pNode_)
	{
		return reinterpret_cast<Node*>((m_Stamp++ << 52) | reinterpret_cast<uintptr_t>(pNode_));
	}

	static Node* RemoveStamp(Node* pNode_)
	{
		return reinterpret_cast<Node*>(reinterpret_cast<uintptr_t>(pNode_) & (0x000FFFFFFFFFFFFFU));
	}

	std::atomic_uint64_t m_Stamp;
	char pad1[56];
	std::atomic<Node*> m_Head;
	char pad2[56];
	std::atomic<Node*> m_Tail;
	char pad3[56];
	std::atomic<Node*> m_FreeList;
	const uint32_t NODE_COUNT = 150;
};