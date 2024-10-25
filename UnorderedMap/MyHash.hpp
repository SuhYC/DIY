#pragma once

#include <string>
#include <Windows.h>

/*
* 최대한 간단하게 구성한다.
* 원래라면 해시충돌이 일어날 확률이 적은지
* Avalanche Effect가 충분히 일어나는지
* 판단해야하지만,
* 간단한 해시테이블을 구성하기 위해 작성하는 것이니 최소한만 구성한다.
* 
* *31을 하는 이유 : 2^5 - 1 이기 때문에 inp << 5 - inp로 빠르게 계산이 가능하며 소수이기 때문에 해시충돌 가능성이 비교적 적다.
*/

// 어떤 데이터가 들어올 지 모르니 메모리를 복사하여 해시한다.
// 물론 동적메모리를 일일히 확인해 해시해주지는 않는다.
template <typename T>
size_t CustomHash(const T& data_)
{
	size_t datasize = sizeof(T);

	unsigned char* copy = new unsigned char[datasize];

	CopyMemory(copy, &data_, datasize);

	size_t hash = 0;

	for (size_t i = 0; i < datasize; i++)
	{
		hash = hash * 31 + static_cast<size_t>(copy[i]);
	}

	delete[] copy;

	return hash;
}

// std::string 특수화
template <>
size_t CustomHash(const std::string& str_)
{
	size_t hash = 0;
	for (char c : str_)
	{
		hash = hash * 31 + static_cast<size_t>(c);
	}

	return hash;
}