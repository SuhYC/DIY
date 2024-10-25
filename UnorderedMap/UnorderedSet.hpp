#pragma once

#include "LinkedList.hpp"
#include "MyVector.hpp"
#include "MyHash.hpp"

template <typename T, size_t tableSize = 10>
class Unordered_Set
{
public:
	Unordered_Set() : m_BucketSize(tableSize)
	{
		m_vector.Resize(m_BucketSize);
		m_DataCount = 0;
	}

	~Unordered_Set()
	{

	}

	bool insert(const T& data_)
	{
		size_t hash = CustomHash(data_) % m_BucketSize;

		LinkedList<T>& list = m_vector[hash];

		int nRet = list.Find(data_);
		// 데이터가 있음
		if (nRet != -1)
		{
			return false;
		}

		list.PushFront(data_);

		m_DataCount++;

		return true;
	}

	bool insert(const T&& data_)
	{
		size_t hash = CustomHash(data_) % m_BucketSize;

		LinkedList<T>& list = m_vector[hash];

		int nRet = list.Find(data_);
		// 데이터가 있음
		if (nRet != -1)
		{
			return false;
		}

		list.PushFront(data_);

		m_DataCount++;

		return true;
	}

	void erase(const T& data_)
	{
		size_t hash = CustomHash(data_) % m_BucketSize;

		auto& list = m_vector[hash];

		LinkedListNode<T>* pNode = list.GetNode(data_);

		if (pNode != nullptr)
		{
			delete pNode;
			m_DataCount--;
		}

		return;
	}

	// 해당 키에 맞는 데이터가 있는지 반환
	bool find(const T& data_) const
	{
		size_t hash = CustomHash(data_) % m_BucketSize;

		auto& list = m_vector[hash];

		int nRet = list.Find(data_);
		if (nRet != -1)
		{
			return true;
		}
		return false;
	}

	void clear()
	{
		m_vector.clear();
		m_vector.Resize(m_BucketSize);
		m_DataCount = 0;
	}

	size_t size() const { return m_DataCount; }
	bool empty() const { return m_DataCount == 0; }

private:
	MyVector<LinkedList<T>> m_vector;
	size_t m_BucketSize;
	size_t m_DataCount;
};