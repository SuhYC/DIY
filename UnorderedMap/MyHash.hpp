#pragma once

#include <string>
#include <Windows.h>

/*
* �ִ��� �����ϰ� �����Ѵ�.
* ������� �ؽ��浹�� �Ͼ Ȯ���� ������
* Avalanche Effect�� ����� �Ͼ����
* �Ǵ��ؾ�������,
* ������ �ؽ����̺��� �����ϱ� ���� �ۼ��ϴ� ���̴� �ּ��Ѹ� �����Ѵ�.
* 
* *31�� �ϴ� ���� : 2^5 - 1 �̱� ������ inp << 5 - inp�� ������ ����� �����ϸ� �Ҽ��̱� ������ �ؽ��浹 ���ɼ��� ���� ����.
*/

// � �����Ͱ� ���� �� �𸣴� �޸𸮸� �����Ͽ� �ؽ��Ѵ�.
// ���� �����޸𸮸� ������ Ȯ���� �ؽ��������� �ʴ´�.
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

// std::string Ư��ȭ
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