# 메모리풀
미리 할당한 메모리블록을 생성자와 소멸자만 명시호출하여 사용하고, <br/>
사용이 끝난 메모리블록을 이후 다시 재사용할 수 있게 하는 도구 <br/>

## ::operator new, ::operator delete
요청한 크기의 메모리블록을 할당/해제하는 함수이다. <br/>
다른 new/delete쌍 처럼 교차사용은 권장되지 않고 ```::operator new```로 할당한 데이터는 ```::operator delete```로 해제하는게 좋다. <br/>
이렇게 할당한 메모리 블록은 ```void*```타입의 포인터로 주어지며, <br/>
해당 포인터는 또한 Placement New방식을 사용해 생성자를 호출하여 해당 클래스의 포인터로 반환받을 수 있다. <br/>
사용이 끝난 인스턴스를 소멸자만 명시호출하여 메모리블럭상태로 다시 바꾼 후 메모리풀에 반환하면 된다. <br/>

## false sharing
[거짓 공유](https://github.com/SuhYC/Lesson/blob/main/Computer_Structure/False_Sharing.md)는 인접한 메모리에 있는 데이터가 수정되어 캐시미스가 발생하는 성능저하현상으로, 패딩을 통해 방지할 수 있다. <br/>
아쉽게도 해당 구현에서는 false sharing이 주로 발생하는 큐보다는 스택으로 비교한다. (lock-free 큐의 구현이 난이도가 높기 때문.) <br/>

## thread local storage
[TLS](https://github.com/SuhYC/Lesson/blob/main/C%2B%2B/Thread_Local_Storage.md)는 스레드마다의 별도공간에 객체를 배치하는 방법으로 경쟁상태를 방지할 수 있다. <br/>
해당 구현에서는 TLS에 스택을 배치하고, 사용한 데이터는 공유FreeList에 반환한 후, <br/>
TLS 스택에 데이터가 모자라면 공유FreeList로부터 데이터를 Batch-Pop하는 형태로 구현되었다. <br/>

## 실험 준비
다음 4가지 방식의 성능을 비교한다. <br/>
1. new/delete 직접 호출
2. 공유 lock-free Stack 1개 사용 (padding 추가)
3. 공유 lock-free Stack 1개 사용 (내부패딩x 외부패딩x)
4. tls스택 사용 + 공유 lock-free Stack(freelist용) 1개 사용 및 batch-pop 가능 (padding 추가)

공통된 변인은 다음과 같다. <br/>
1. Pop하여 나온 데이터를 확인 후 다시 Push한다.
2. 1번의 과정을 100만회 반복한다.
3. 2번의 과정을 수행하는 스레드가 5개 존재한다.

즉, 500만회의 동작을 5개의 스레드가 나누어 실행한다. <br/>

## 코드

<details>
<summary>Lock-Free Stack with Padding</summary>

```cpp
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
```

</details>
<details>
<summary>tls + batchpop MemoryPool</summary>

```cpp
#include "LockFreeStack.hpp"
#include "Stack.hpp"

const int32_t BLOCK_COUNT = 1000;

thread_local Stack g_tls_Stack;

class alignas(64) MemoryPool
{
public:
	MemoryPool()
	{
		for (int32_t i = 0; i < BLOCK_COUNT; i++)
		{
			void* block = ::operator new(8, std::nothrow);

			if (block != nullptr)
			{
				m_Stack.Push(block);
			}
		}
	}

	void* Allocate()
	{
		// 1차로 tls 접근
		void* block = g_tls_Stack.Pop();

		if (block != nullptr)
		{
			return block;
		}

		// 2차로 공유 lock-free Stack접근하여 Batch Pop으로 tls stack을 채움
		m_Stack.BatchPop(g_tls_Stack);

		block = g_tls_Stack.Pop();

		// 3차로 직접 할당
		if (block == nullptr)
		{
			block = ::operator new(8, std::nothrow);
		}

		// 이래도 nullptr면 메모리부족.. ㅠㅠ
		return block;
	}

	void Deallocate(void* data_)
	{
		if (data_ == nullptr)
		{
			return;
		}

		m_Stack.Push(data_);
	}

private:
	LockFreeStack m_Stack;
};
```

</details>

## 결과

<img src="https://github.com/SuhYC/DIY/blob/main/MemoryPool/result.png"><br/>
1. 1307ms
2. 1150ms
3. 1983ms
4. 637ms

패딩이 없는 lock-free Stack으로 경합이 발생한 경우는 결과가 대박살.. new/delete보다도 성능이 안좋다. <br/>
패딩, TLS와 Batch-Pop방식으로 최적화한 lock-free Stack방식은 2.05배의 속도를 보였다. <br/>

## 아쉬운점
lock-free queue로도 실험해보고 싶다. <br/>
Michael & Scott Queue (MS Queue) 방식을 이해하고 이후에 재시도해야겠다.
