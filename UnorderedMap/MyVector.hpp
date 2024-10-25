#pragma once

#include <exception>

template <typename T>
class MyVector
{
public:
	MyVector()
	{
		m_pData = nullptr;
		m_Size = 0;
		m_Capacity = 0;
	}

	MyVector(const size_t size_) : m_Size(size_), m_Capacity(size_)
	{
		m_pData = new T[size_];
		for (size_t i = 0; i < size_; i++)
		{
			m_pData[i]();
		}
	}

	~MyVector()
	{
		if (m_pData != nullptr)
		{
			delete[] m_pData;
		}
	}

	T& operator[](size_t index_)
	{
		return m_pData[index_];
	}
	
	const T& operator[](size_t index_) const
	{
		return m_pData[index_];
	}

	void Push_Back(const T data_)
	{
		if (m_Capacity == 0)
		{
			Resize(1);
			m_Capacity = 1;
		}

		if (m_Size == m_Capacity)
		{
			m_Capacity *= 2; // 2배의 크기를 할당한다.
			Resize(m_Capacity); 
		}

		m_pData[m_Size] = data_;
		m_Size++;
		return;
	}

	void Resize(const size_t size_)
	{
		T* pNewData;
		try
		{
			pNewData = new T[size_];
		}
		catch (std::bad_alloc e)
		{
			std::cout << "용량부족\n";
			return;
		}

		if (size_ < m_Size)
		{
			for (size_t i = 0; i < size_; i++)
			{
				pNewData[i] = m_pData[i];
			}
		}
		else
		{
			for (size_t i = 0; i < m_Size; i++)
			{
				pNewData[i] = m_pData[i];
			}
		}

		if (m_pData != nullptr)
		{
			delete[] m_pData;
		}
		m_pData = pNewData;
		m_Size = size_;
		m_Capacity = size_;
	}

	void Reserve(const size_t cap_)
	{
		// 아무 동작도 수행하지 않음.
		if (cap_ <= m_Capacity)
		{
			return;
		}

		T* pNewData;

		try
		{
			pNewData = new T[cap_];
		}
		catch (std::bad_alloc e)
		{
			std::cout << "용량부족\n";
			return;
		}

		for (size_t i = 0; i < m_Size; i++)
		{
			pNewData[i] = std::move(m_pData[i]);
		}

		if (m_pData != nullptr)
		{
			delete[] m_pData;
		}

		m_pData = pNewData;
		m_Capacity = cap_;

		return;
	}

	void shrink_to_fit()
	{
		if (m_Size == m_Capacity)
		{
			return;
		}

		T* pNewData;
		try
		{
			pNewData = new T[m_Size];
		}
		catch (std::bad_alloc e)
		{
			std::cout << "용량부족\n";
			return;
		}

		for (size_t i = 0; i < m_Size; i++)
		{
			pNewData[i] = std::move(m_pData[i]);
		}

		if (m_pData != nullptr)
		{
			delete[] m_pData;
		}

		m_pData = pNewData;
		m_Capacity = m_Size;

		return;
	}

	void clear()
	{
		if (m_pData != nullptr)
		{
			delete[] m_pData;
			m_pData = nullptr;
			m_Size = 0;
			m_Capacity = 0;
		}
	}

	size_t GetSize() const { return m_Size; }
	size_t GetCapacity() const { return m_Capacity; }

private:
	T* m_pData;
	size_t m_Size;
	size_t m_Capacity;
};