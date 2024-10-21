#include "RingBuffer.hpp"
#include <vector>

const int cnt = 100;

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

int main()
{
	RingBuffer rb;

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

	return 0;
}