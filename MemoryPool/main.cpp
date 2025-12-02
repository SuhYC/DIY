#include "LockFreeStack.hpp"
#include "MemoryPool.hpp"
#include <chrono>
#include <vector>
#include <thread>

const int32_t DATA_COUNT = 100;
const int32_t TEST_COUNT = 1000000;
const int32_t THREAD_COUNT = 5;

LockFreeStack st;
LockFreeStackNoAlign nst;
MemoryPool pool;

void Prepare()
{
	for (int32_t i = 0; i < DATA_COUNT; i++)
	{
		void* block = ::operator new(8, std::nothrow);

		if (block != nullptr)
		{
			st.Push(block);
		}
	}
}

void Test()
{
	for (int32_t i = 0; i < TEST_COUNT; i++)
	{
		void* block = st.Pop();
		if (block == nullptr)
		{
			std::cout << "No Data.\n";
		}
		else
		{
			st.Push(block);
		}
	}
}

void Test2()
{
	for (int32_t i = 0; i < TEST_COUNT; i++)
	{
		void* block = ::operator new(8, std::nothrow);
		if (block == nullptr)
		{
			std::cerr << "Not Enough Memory.\n";
		}
		else
		{
			::operator delete(block);
		}
	}
}

void Test3()
{
	for (int32_t i = 0; i < TEST_COUNT; i++)
	{
		void* block = pool.Allocate();
		if (block == nullptr)
		{
			std::cout << "No Data.\n";
		}
		else
		{
			pool.Deallocate(block);
		}
	}
}

void PoolTest()
{
	std::chrono::high_resolution_clock::time_point st, end;

	std::vector<std::thread> ths;

	ths.reserve(THREAD_COUNT);

	st = std::chrono::high_resolution_clock::now();

	for (int32_t i = 0; i < THREAD_COUNT; i++)
	{
		ths.emplace_back([]() {Test3(); });
	}

	for (int32_t i = 0; i < THREAD_COUNT; i++)
	{
		if (ths[i].joinable())
		{
			ths[i].join();
		}
	}

	end = std::chrono::high_resolution_clock::now();
	auto diff = end - st;

	std::cout << diff.count() / 1000000 << '\n';
}

void SharedStackTest()
{
	std::chrono::high_resolution_clock::time_point st, end;

	std::vector<std::thread> ths;

	ths.reserve(THREAD_COUNT);

	Prepare();

	st = std::chrono::high_resolution_clock::now();

	for (int32_t i = 0; i < THREAD_COUNT; i++)
	{
		ths.emplace_back([]() {Test(); });
	}

	for (int32_t i = 0; i < THREAD_COUNT; i++)
	{
		if (ths[i].joinable())
		{
			ths[i].join();
		}
	}

	end = std::chrono::high_resolution_clock::now();
	auto diff = end - st;

	std::cout << diff.count() / 1000000 << '\n';
}

void NewDeleteTest()
{
	std::chrono::high_resolution_clock::time_point st, end;

	std::vector<std::thread> ths;

	ths.reserve(THREAD_COUNT);

	st = std::chrono::high_resolution_clock::now();

	for (int32_t i = 0; i < THREAD_COUNT; i++)
	{
		ths.emplace_back([]() {Test2(); });
	}

	for (int32_t i = 0; i < THREAD_COUNT; i++)
	{
		if (ths[i].joinable())
		{
			ths[i].join();
		}
	}

	end = std::chrono::high_resolution_clock::now();
	auto diff = end - st;

	std::cout << diff.count() / 1000000 << '\n';
}

void Prepare2()
{
	for (int32_t i = 0; i < DATA_COUNT; i++)
	{
		void* block = ::operator new(8, std::nothrow);

		if (block != nullptr)
		{
			nst.Push(block);
		}
	}
}

void Test4()
{
	for (int32_t i = 0; i < TEST_COUNT; i++)
	{
		void* block = nst.Pop();
		if (block == nullptr)
		{
			std::cout << "No Data.\n";
		}
		else
		{
			nst.Push(block);
		}
	}
}

void NoAlignTest()
{
	std::chrono::high_resolution_clock::time_point st, end;

	std::vector<std::thread> ths;

	ths.reserve(THREAD_COUNT);

	Prepare2();

	st = std::chrono::high_resolution_clock::now();

	for (int32_t i = 0; i < THREAD_COUNT; i++)
	{
		ths.emplace_back([]() {Test4(); });
	}

	for (int32_t i = 0; i < THREAD_COUNT; i++)
	{
		if (ths[i].joinable())
		{
			ths[i].join();
		}
	}

	end = std::chrono::high_resolution_clock::now();
	auto diff = end - st;

	std::cout << diff.count() / 1000000 << '\n';
}

int main()
{
	NewDeleteTest(); // new & delete
	SharedStackTest(); // shared stack + padding
	NoAlignTest(); // shared stack + no padding
	PoolTest(); // padding + tls stack + shared freelist->batch pop

	return 0;
}