#pragma once

#include <string>
#include <mutex>
#include <utility>
#include <Windows.h>
#include <iostream>

// ĳ������ ��ġ������ �����ϱ� ���ؼ� ������ �ʿ��� �Ͱ���.
// 1. ������ġ 2. ����ӵ�
// �� �� ���� Ŭ���̾�Ʈ���� Ư���ð����� 1ȸ �õ��Ѵ�. (30ms�������� �ص� �Ƿ���?)
// ��ġ�������� �������� ��Ŷ ���� ������ �����Ͽ� ǥ���Ѵ�. (���巹Ŀ��)
// 
// �׷��� ��ġ���� �ӵ����� ���Žð��� ����Ͽ� ��ġ�� �׸��� �ǳ�?
// �Ϲ����� �̵��� �ƴ� ��쿡�� �������� �ʴ´� (ĳ������ �̵��ӵ��� �����״�.. �װɷ� ����)
// ex. 500ms �̳����� ���� ������ ��� ��ų ������ ���� ��ų ��뿡 ���� ��ġ�ݿ��� �ϴ°� �´�.
// ���������� ����� n�� �ӷ��� ���µ� ��Ŷ������ �ȴٸ� �̻��� ������ �� ���� ������.
// 
// �ش� ������ ����Ϸ���
// �� ��Ŷ�� ũ�⸦ ������ ���������. (���̸��� �߶� ���)
// �� ��Ŷ�� ũ�Ⱑ ����ġ�� ū ���� Ȯ���۾��� �ʿ��� (Ŭ���̾�Ʈ�� ���������� ������ �����?)
// 
// ���� Connection�� �߰��ϰ� ���� �۽Ź��۴� ����� �� ��
// ���̴� ��Ŷ�� ���� �����صΰ�, 1ȸ �۽��� ������ �ִ��� dequeue�ؼ� �۽Ź��ۿ� ��� �۽ſ����� �Ǵ�.
//
// ������?
// �ϴ� ������ �����ʹ� ó���� �� �ִ� ��ŭ ReqHandler�� ó���� �ñ��,
// �����Ͱ� �߷� ������ �� ���� ���� Connection�� ��Ƶξ��ٰ� ���� ���ſ��� �̾ ó������.

class RingBuffer
{
public:
	RingBuffer()
	{
		m_pData = nullptr;
		debug = false;

		m_Capacity = 0;
		m_size = 0;
	}

	~RingBuffer()
	{
		if (m_pData != nullptr)
		{
			//delete[] m_pData;
		}
	}

	bool Init(const int capacity_, bool deb = false)
	{
		debug = deb;

		if (capacity_ <= 0)
		{
			return false;
		}

		m_Capacity = capacity_;
		m_size = 0;

		if (m_pData != nullptr)
		{
			delete[] m_pData;
		}
		m_pData = new char[capacity_];
		ZeroMemory(m_pData, capacity_);

		return true;
	}


	bool enqueue(char* in_, int size_)
	{
		// ������ �ȴ���
		if (size_ <= 0)
		{
			return false;
		}

		if (size_ + m_size > m_Capacity)
		{
			return false;
		}

		CopyMemory(&m_pData[m_size], in_, size_);
		m_size += size_;

		//std::cout << "enqueue : size : " << m_size << '\n';

		return true;
	}

	// ret : peeked Data size
	int dequeue(char* out_, int maxSize_)
	{
		if (m_size == 0)
		{
			return 0;
		}

		if (maxSize_ >= m_size)
		{
			int ret = m_size;
			CopyMemory(out_, m_pData, m_size);
			m_size = 0;

			//std::cout << "dequeue : size : " << ret << '\n';
			return ret;
		}
		else
		{
			int ret = maxSize_;
			CopyMemory(out_, m_pData, maxSize_);
			CopyMemory(m_pData, &m_pData[maxSize_], m_size - maxSize_);

			m_size -= maxSize_;

			//std::cout << "dequeue : size : " << ret << '\n';

			return ret;
		}
	}

	bool IsEmpty() const
	{
		return m_size == 0;
	}
private:
	char* m_pData;
	int m_Capacity;
	int m_size;

	bool debug;
};