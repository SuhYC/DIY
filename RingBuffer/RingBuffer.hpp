#pragma once

#include <string>
#include <mutex>
#include <utility>
#include <Windows.h>
#include <iostream>

/*
* 해야하는 동작
* 1. 데이터 추가하기
* 2. 데이터 읽기
* 3. 처리한 데이터 제거하기
*/

class RingBuffer
{
public:
	RingBuffer()
	{
		m_pData = nullptr;
	}

	~RingBuffer()
	{
		if (m_pData != nullptr)
		{
			delete[] m_pData;
		}
	}

	bool Init(const int capacity_)
	{
		if (capacity_ <= 0)
		{
			return false;
		}

		m_Capacity = capacity_;
		m_ReadPos = 0;
		m_WritePos = 0;

		m_pData = new char[capacity_];
		ZeroMemory(m_pData, capacity_);
	}


	bool enqueue(char* in_, int size_)
	{
		// 데이터 안담음
		if (size_ <= 0)
		{
			return false;
		}

		// 애초에 못담음
		if (size_ >= m_Capacity)
		{
			return false;
		}

		std::lock_guard<std::mutex> guard(m_mutex);

		// 남은 공간 초과
		if ((m_WritePos + size_ + 1) > m_ReadPos + m_Capacity)
		{
			return false;
		}

		char* source = in_;
		int leftsize = size_;

		// 저장할 데이터가 선형버퍼의 끝에 도달함 -> 분할 저장해야함 (끝부분, 처음부분으로 나눠서)
		if (m_WritePos + size_ > m_Capacity)
		{
			int availableSpace = m_Capacity - m_WritePos;
			CopyMemory(&m_pData[m_WritePos], source, availableSpace);
			m_WritePos = 0;
			source += availableSpace;
			leftsize -= availableSpace;
		}

		CopyMemory(&m_pData[m_WritePos], source, leftsize);
		m_WritePos += leftsize;

		m_WritePos %= m_Capacity;

		return true;
	}

	// ret : peeked Data size
	int dequeue(char* out_, int maxSize_)
	{
		std::lock_guard<std::mutex> guard(m_mutex);

		if (IsEmpty())
		{
			return 0;
		}

		int remain;

		// 일반적인 경우
		if (m_WritePos > m_ReadPos)
		{
			remain = m_WritePos - m_ReadPos;
			if (remain > maxSize_)
			{
				remain = maxSize_;
			}

			CopyMemory(out_, &m_pData[m_ReadPos], remain);
			m_ReadPos += remain;
			m_ReadPos %= m_Capacity;
			return remain;
		}
		// 선형버퍼의 끝까지 읽으면 됨
		else if (m_WritePos == 0)
		{
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
		// 선형버퍼의 끝까지 읽고 선형버퍼의 맨 앞부터 이어서 읽으면 됨
		else
		{
			remain = m_Capacity - m_ReadPos;
			
			if (remain > maxSize_)
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

	void printStatus()
	{
		std::cout << 1;
		for (int i = 0; i < m_Capacity; i++)
		{
			if (m_pData[i] == NULL)
			{
				std::cout << "-";
				continue;
			}

			std::cout << m_pData[i] << ", ";
		}

		std::cout << '\n';
	}

	bool IsEmpty() const { return m_ReadPos == m_WritePos; }
	bool IsFull() const { return (m_WritePos + 1) % m_Capacity == m_ReadPos; }
private:
	int m_ReadPos;
	int m_WritePos;

	char* m_pData;
	int m_Capacity;

	std::mutex m_mutex;
};