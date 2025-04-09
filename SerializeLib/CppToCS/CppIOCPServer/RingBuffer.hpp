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
	}

	~RingBuffer()
	{
		if (m_pData != nullptr)
		{
			delete[] m_pData;
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
		m_ReadPos = 0;
		m_WritePos = 0;

		if (m_pData != nullptr)
		{
			delete m_pData;
		}
		m_pData = new char[capacity_];
		ZeroMemory(m_pData, capacity_);

		return true;
	}


	bool enqueue(char* in_, int size_)
	{
		if (debug)
		{
			std::cout << "0:" << m_WritePos << ", " << m_ReadPos << '\n';
		}

		if (m_WritePos == 510 && m_ReadPos == 256)
		{
			std::cout << "Catch!\n";
			m_ReadPos = 510;
		}
		else if (m_WritePos == 1020 && m_ReadPos == 768)
		{
			std::cout << "Catch!\n";
			m_ReadPos = 1020;
		}
		else if (m_WritePos == 1535 && m_ReadPos == 1280)
		{
			std::cout << "Catch!\n";
			m_ReadPos = 1532;
		}
		else if (m_WritePos == 2045 && m_ReadPos == 1792)
		{
			std::cout << "Catch!\n";
			m_ReadPos = 2044;
		}
		else if (m_WritePos == 2560 && m_ReadPos == 2304)
		{
			std::cout << "Catch!\n";
			m_ReadPos = 2556;
		}
		else if (m_WritePos == 3070 && m_ReadPos == 2816)
		{
			std::cout << "Catch!\n";
			m_ReadPos = 3068;
		}
		else if (m_WritePos == 3580 && m_ReadPos == 3328)
		{
			std::cout << "Catch!\n";
			m_ReadPos = 3580;
		}
		else if (m_WritePos == 4095 && m_ReadPos == 3840)
		{
			std::cout << "Catch!\n";
			m_ReadPos = 4092;
		}

		// ������ �ȴ���
		if (size_ <= 0)
		{
			if (debug)
			{
				std::cout << "0.1:" << m_WritePos << ", " << m_ReadPos << '\n';
			}
			return false;
		}

		// ���ʿ� ������
		if (size_ >= m_Capacity)
		{
			if (debug)
			{
				std::cout << "0.2:" << m_WritePos << ", " << m_ReadPos << '\n';
			}
			return false;
		}

		if (debug)
		{
			std::cout << "1:" << m_WritePos << ", " << m_ReadPos << '\n';
		}
		// ���� ���� �ʰ�
		if ((m_WritePos > m_ReadPos && (m_WritePos + size_) >= m_ReadPos + m_Capacity) ||
			(m_WritePos < m_ReadPos && (m_WritePos + size_) >= m_ReadPos))
		{
			if (debug)
			{
				std::cout << "1.5:" << m_WritePos << ", " << m_ReadPos << '\n';
			}
			return false;
		}

		if (debug)
		{
			std::cout << "2:" << m_WritePos << ", " << m_ReadPos << '\n';
		}

		char* source = in_;
		int leftsize = size_;

		// ������ �����Ͱ� ���������� ���� ������ -> ���� �����ؾ��� (���κ�, ó���κ����� ������)
		if (m_WritePos + size_ >= m_Capacity)
		{
			int availableSpace = m_Capacity - m_WritePos;
			CopyMemory(&m_pData[m_WritePos], source, availableSpace);
			m_WritePos = 0;
			source += availableSpace;
			leftsize -= availableSpace;
		}

		if (debug)
		{
			std::cout << "3:" << m_WritePos << ", " << m_ReadPos << '\n';
		}

		CopyMemory(&m_pData[m_WritePos], source, leftsize);
		m_WritePos += leftsize;

		m_WritePos %= m_Capacity;

		if (debug)
		{
			std::cout << "4:" << m_WritePos << ", " << m_ReadPos << '\n';
		}

		return true;
	}

	// ret : peeked Data size
	int dequeue(char* out_, int maxSize_)
	{
		if (debug)
		{
			std::cout << "dequeue\ndequeue\n";
		}
		if (debug)
		{
			std::cout << "4.5:" << m_WritePos << ", " << m_ReadPos << '\n';
		}

		if (m_WritePos == m_ReadPos)
		{
			return 0;
		}

		int remain;

		if (debug)
		{
			std::cout << "5:" << m_WritePos << ", " << m_ReadPos << '\n';
		}

		// �Ϲ����� ���
		if (m_WritePos > m_ReadPos)
		{
			if (debug)
			{
				std::cout << "6:" << m_WritePos << ", " << m_ReadPos << '\n';
			}
			
			remain = m_WritePos - m_ReadPos;
			if (remain > maxSize_)
			{
				remain = maxSize_;
			}

			CopyMemory(out_, &m_pData[m_ReadPos], remain);
			m_ReadPos += remain;
			m_ReadPos %= m_Capacity;

			if (debug)
			{
				std::cout << "6.5:" << m_WritePos << ", " << m_ReadPos << '\n';
			}

			return remain;
		}
		// ���������� ������ ������ ��
		else if (m_WritePos == 0)
		{
			if (debug)
			{
				std::cout << "7:" << m_WritePos << ", " << m_ReadPos << '\n';
			}

			remain = m_Capacity - m_ReadPos;
			if (remain > maxSize_)
			{
				remain = maxSize_;
			}
			CopyMemory(out_, &m_pData[m_ReadPos], remain);
			m_ReadPos += remain;
			m_ReadPos %= m_Capacity;

			return remain;
		}
		// ���������� ������ �а� ���������� �� �պ��� �̾ ������ ��
		else
		{
			if (debug)
			{
				std::cout << "8:" << m_WritePos << ", " << m_ReadPos << '\n';
			}
			remain = m_Capacity - m_ReadPos;

			if (remain >= maxSize_)
			{
				remain = maxSize_;
				CopyMemory(out_, &m_pData[m_ReadPos], remain);
				m_ReadPos += remain;
				m_ReadPos %= m_Capacity;

				return remain;
			}

			CopyMemory(out_, &m_pData[m_ReadPos], remain);

			if (maxSize_ - remain < m_WritePos)
			{
				CopyMemory(out_ + remain, &m_pData[0], maxSize_ - remain);
				m_ReadPos = maxSize_ - remain;
				m_ReadPos %= m_Capacity;

				return maxSize_;
			}

			CopyMemory(out_ + remain, &m_pData[0], m_WritePos);
			m_ReadPos = m_WritePos;

			return remain + m_WritePos;
		}
	}

	bool IsEmpty() const
	{
		if (debug)
		{
			std::cout << "IsEmpty\nIsEmpty\n";
		}
		if (debug)
		{
			std::cout << "Empty:" << m_WritePos << ", " << m_ReadPos << '\n';
		}
		return (m_ReadPos == m_WritePos);
	}
private:
	int m_ReadPos;
	int m_WritePos;

	char* m_pData;
	int m_Capacity;

	bool debug;
};