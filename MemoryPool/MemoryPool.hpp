#pragma once

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