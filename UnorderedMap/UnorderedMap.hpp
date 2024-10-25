#pragma once

#include "LinkedList.hpp"
#include "MyVector.hpp"
#include "MyHash.hpp"

template <typename TKey, typename TValue, size_t tableSize = 10>
class Unordered_Map
{
public:
	Unordered_Map() : m_BucketSize(tableSize)
	{
		m_vector.Resize(m_BucketSize);
	}

	~Unordered_Map()
	{

	}
	
	bool insert(const TKey& key_, const TValue& val_)
	{
		size_t hash = CustomHash(key_) % m_BucketSize;

		LinkedPairList<TKey, TValue>& list = m_vector[hash];

		int nRet = list.Find(key_);
		// 데이터가 있음
		if (nRet != -1)
		{
			return false;
		}

		list.PushFront(key_, val_);

		m_DataCount++;

		return true;
	}

	
	bool insert(const TKey&& key_, const TValue&& val_)
	{
		size_t hash = CustomHash(key_) % m_BucketSize;

		LinkedPairList<TKey, TValue>& list = m_vector[hash];

		int nRet = list.Find(key_);
		// 데이터가 있음
		if (nRet != -1)
		{
			return false;
		}

		list.PushFront(key_, val_);

		m_DataCount++;

		return true;
	}
	
	void erase(const TKey& key_)
	{
		size_t hash = CustomHash(key_) % m_BucketSize;

		LinkedPairList<TKey, TValue>& list = m_vector[hash];

		PairNode<TKey, TValue>* pNode = list.GetNode(key_);

		if (pNode != nullptr)
		{
			delete pNode;
			m_DataCount--;
		}

		return;
	}

	// can throw out_of_range
	// iterator를 안만들어서 이렇게 구현했다..
	TValue& operator[](const TKey& key_)
	{
		size_t hash = CustomHash(key_) % m_BucketSize;

		LinkedPairList<TKey, TValue>& list = m_vector[hash];
		int idx = list.Find(key_);

		try
		{
			return list[idx];
		}
		catch (std::out_of_range e)
		{
			throw std::out_of_range("");
		}
	}

	TValue& operator[](const TKey&& key_)
	{
		size_t hash = CustomHash(key_) % m_BucketSize;

		LinkedPairList<TKey, TValue>& list = m_vector[hash];
		int idx = list.Find(key_);

		try
		{
			return list[idx];
		}
		catch (std::out_of_range e)
		{
			throw std::out_of_range("");
		}
	}

	// 해당 키에 맞는 데이터가 있는지 반환
	bool find(const TKey& key_) const
	{
		size_t hash = CustomHash(key_) % m_BucketSize;

		auto& list = m_vector[hash];
		int idx = list.Find(key_);

		if (idx == -1)
		{
			return false;
		}
		return true;
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
	MyVector<LinkedPairList<TKey, TValue>> m_vector;
	size_t m_BucketSize;
	size_t m_DataCount;
};