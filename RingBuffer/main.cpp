#include "RingBuffer.hpp"
#include <vector>

const int cnt = 1000000;

void func(RingBuffer& rb_, char ch_)
{
	char buf[4] = { ch_, NULL, NULL, NULL };
	int datasize = 1;

	for (int i = 0; i < cnt; i++)
	{
		if (datasize != 0)
		{
			while (!rb_.enqueue(buf, datasize)) {}
		}

		datasize = 0;
		int try_read = 100;

		while (try_read > 0 && datasize == 0)
		{
			datasize = rb_.dequeue(buf, 3);
			try_read--;
		}
	}

	for (int i = 0; i < datasize; i++)
	{
		std::cout << buf[i];
	}
}

void func(OtherRingBuffer& rb_, char ch_)
{
	char buf[4] = { ch_, NULL, NULL, NULL };
	int datasize = 1;

	for (int i = 0; i < cnt; i++)
	{
		if (datasize != 0)
		{
			while (!rb_.enqueue(buf, datasize)) {}
		}

		datasize = 0;
		int try_read = 100;

		while (try_read > 0 && datasize == 0)
		{
			datasize = rb_.dequeue(buf, 3);
			try_read--;
		}
	}

	for (int i = 0; i < datasize; i++)
	{
		std::cout << buf[i];
	}
}

void test()
{
	RingBuffer rb;
	//OtherRingBuffer rb;

	rb.Init(6);

	std::vector<std::thread> v;

	v.reserve(26);

	for (int i = 0; i < 26; i++)
	{
		v.emplace_back([&rb, i]() { func(rb, 'a' + i); });
	}

	for (int i = 0; i < 26; i++)
	{
		v[i].join();
	}

	return;
}

void dbg()
{
	RingBuffer rb;
	rb.Init(6);

	char* buf = new char[5];
	std::string str = "";

	while (str.length() <= 5)
	{
		std::cout << "입력 :";
		std::cin >> str;

		if (str == "1")
		{
			int cap = rb.dequeue(buf, 5);

			std::cout << "출력 : ";
			for (int i = 0; i < cap; i++)
			{
				std::cout << buf[i];
			}
			std::cout << "\n";
			continue;
		}

		CopyMemory(buf, str.c_str(), str.length());
		rb.enqueue(buf, str.length());
	}

	delete[] buf;
}

int main()
{
	test();

	return 0;
}