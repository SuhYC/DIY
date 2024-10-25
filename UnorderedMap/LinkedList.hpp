#pragma once

#include "Node.hpp"

/*
* !���� thread-safe���� �ʴ�. �����Ұ�!
*/

template <typename T>
class LinkedList
{
public:
	LinkedList()
	{
		m_pHead = nullptr;
		m_size = 0;
	}

	~LinkedList()
	{
		if (m_pHead != nullptr)
		{
			delete m_pHead;
		}
	}

	void PushFront(const T& val_)
	{
		LinkedListNode<T>* pNode;
		try
		{
			pNode = new LinkedListNode<T>(val_);
		}
		catch (std::bad_alloc e)
		{
			std::cout << "�޸� ����\n";
			return;
		}

		pNode->PushBack(m_pHead);
		m_pHead = pNode;
		m_size++;

		return;
	}

	void PushFront(const T&& val_)
	{
		LinkedListNode<T>* pNode;
		try
		{
			pNode = new LinkedListNode<T>(val_);
		}
		catch (std::bad_alloc e)
		{
			std::cout << "�޸� ����\n";
			return;
		}

		pNode->PushBack(m_pHead);
		m_pHead = pNode;
		m_size++;

		return;
	}

	// pNode_�� ���ϳ�尡 �ƴϴ��� �������. ��, pNode_�� ����� ����� ���� ���� Head�� ���δ�.
	void PushFront(LinkedListNode<T>* const pNode_)
	{
		pNode_->PushBack(m_pHead);
		m_pHead = pNode_;
		m_size++;
	}

	void PushBack(const T& val_)
	{
		LinkedListNode<T>* pNode;
		try
		{
			pNode = new LinkedListNode<T>(val_);
		}
		catch (std::bad_alloc e)
		{
			std::cout << "�޸� ����\n";
			return;
		}

		m_pHead->PushBack(pNode);
		m_size++;
		return;
	}

	void PushBack(const T&& val_)
	{
		LinkedListNode<T>* pNode;
		try
		{
			pNode = new LinkedListNode<T>(val_);
		}
		catch (std::bad_alloc e)
		{
			std::cout << "�޸� ����\n";
			return;
		}

		m_pHead->PushBack(pNode);
		m_size++;
		return;
	}

	void PushBack(LinkedListNode<T>* const pNode_)
	{
		m_pHead->PushBack(pNode_);
		m_size++;
		return;
	}

	bool Pop(const int index_, T& out_)
	{
		// �ε��� �Է��� �߸���
		if (index_ > m_size || index_ < 0)
		{
			return false;
		}

		LinkedListNode<T>* pNode = m_pHead->Pop(index_);

		// ã�� ������ ����
		if (pNode == nullptr)
		{
			return false;
		}

		out_ = pNode->GetData();
		delete pNode;

		m_size--;

		return true;
	}

	int Find(const T& data_) const
	{
		if (m_pHead == nullptr)
		{
			return -1;
		}
		return m_pHead->Find(data_);
	}

	int Find(const T&& data_) const
	{
		if (m_pHead == nullptr)
		{
			return -1;
		}
		return m_pHead->Find(data_);
	}

	LinkedListNode<T>* GetFrontNode()
	{
		if (m_pHead == nullptr)
		{
			return nullptr;
		}

		LinkedListNode<T>* ret = m_pHead;
		ret->prev = nullptr;
		ret->next = nullptr;

		m_pHead = m_pHead->next;
		if (m_pHead != nullptr)
		{
			m_pHead->prev = nullptr;
		}
		
		return ret;
	}

	// iterator��ȯ�����̸� �� ������ �ϰڴ�.. iterator�� �ȸ���� �Ϸ��� ��ȸ�� 2���Ѵ�.
	LinkedListNode<T>* GetNode(const T& data_)
	{
		int index = Find(data_);

		// ã�� ������ ����
		if (index == -1)
		{
			return nullptr;
		}

		if (index == 0)
		{
			return GetFrontNode();
		}

		LinkedListNode<T>* ret = m_pHead->Pop(index);

		m_size--;

		return ret;
	}

	LinkedListNode<T>* GetNode(const T&& data_)
	{
		int index = Find(data_);

		// ã�� ������ ����
		if (index == -1)
		{
			return nullptr;
		}

		if (index == 0)
		{
			return GetFrontNode();
		}

		LinkedListNode<T>* ret = m_pHead->Pop(index);

		m_size--;

		return ret;
	}

	int size() const { return m_size; }

	void clear()
	{
		delete m_pHead;
		m_pHead = nullptr;

		m_size = 0;
	}

private:
	LinkedListNode<T>* m_pHead;
	int m_size;
};


template <typename TKey, typename TValue>
class LinkedPairList
{
public:
	LinkedPairList()
	{
		m_pHead = nullptr;
		m_size = 0;
	}

	~LinkedPairList()
	{
		if (m_pHead != nullptr)
		{
			delete m_pHead;
		}
	}

	void PushFront(const TKey& key_, const TValue& val_)
	{
		PairNode<TKey, TValue>* pNode;
		try
		{
			pNode = new PairNode<TKey, TValue>(key_, val_);
		}
		catch (std::bad_alloc e)
		{
			std::cout << "�޸� ����\n";
			return;
		}

		pNode->PushBack(m_pHead);
		m_pHead = pNode;
		m_size++;

		return;
	}

	void PushFront(const TKey&& key_, const TValue&& val_)
	{
		PairNode<TKey, TValue>* pNode;
		try
		{
			pNode = new PairNode<TKey, TValue>(key_, val_);
		}
		catch (std::bad_alloc e)
		{
			std::cout << "�޸� ����\n";
			return;
		}

		pNode->PushBack(m_pHead);
		m_pHead = pNode;
		m_size++;

		return;
	}

	// pNode_�� ���ϳ�尡 �ƴϴ��� �������. ��, pNode_�� ����� ����� ���� ���� Head�� ���δ�.
	void PushFront(PairNode<TKey, TValue>* const pNode_)
	{
		pNode_->PushBack(m_pHead);
		m_pHead = pNode_;
		m_size++;
	}

	void PushBack(const TKey& key_, const TValue& val_)
	{
		PairNode<TKey, TValue>* pNode;
		try
		{
			pNode = new PairNode<TKey, TValue>(key_, val_);
		}
		catch (std::bad_alloc e)
		{
			std::cout << "�޸� ����\n";
			return;
		}

		m_pHead->PushBack(pNode);
		m_size++;
		return;
	}

	void PushBack(const TKey&& key_, const TValue&& val_)
	{
		PairNode<TKey, TValue>* pNode;
		try
		{
			pNode = new PairNode<TKey, TValue>(key_, val_);
		}
		catch (std::bad_alloc e)
		{
			std::cout << "�޸� ����\n";
			return;
		}

		m_pHead->PushBack(pNode);
		m_size++;
		return;
	}

	void PushBack(PairNode<TKey, TValue>* const pNode_)
	{
		m_pHead->PushBack(pNode_);
		m_size++;
		return;
	}

	bool Pop(const int index_, TValue& out_)
	{
		// �ε��� �Է��� �߸���
		if (index_ > m_size || index_ < 0)
		{
			return false;
		}

		PairNode<TKey, TValue>* pNode = m_pHead->Pop(index_);

		// ã�� ������ ����
		if (pNode == nullptr)
		{
			return false;
		}

		out_ = pNode->GetData();
		delete pNode;

		m_size--;

		return true;
	}

	int Find(const TKey& key_) const
	{
		if (m_pHead == nullptr)
		{
			return -1;
		}
		return m_pHead->Find(key_);
	}

	int Find(const TKey&& key_) const
	{
		if (m_pHead == nullptr)
		{
			return -1;
		}
		return m_pHead->Find(key_);
	}

	PairNode<TKey, TValue>* GetFrontNode()
	{
		if (m_pHead == nullptr)
		{
			return nullptr;
		}

		PairNode<TKey, TValue>* ret = m_pHead;
		ret->prev = nullptr;
		ret->next = nullptr;

		m_pHead = m_pHead->next;
		if (m_pHead != nullptr)
		{
			m_pHead->prev = nullptr;
		}

		return ret;
	}

	// iterator��ȯ�����̸� �� ������ �ϰڴ�.. iterator�� �ȸ���� �Ϸ��� ��ȸ�� 2���Ѵ�.
	PairNode<TKey, TValue>* GetNode(const TKey& key_)
	{
		int index = Find(key_);

		// ã�� ������ ����
		if (index == -1)
		{
			return nullptr;
		}

		if (index == 0)
		{
			return GetFrontNode();
		}

		PairNode<TKey, TValue>* ret = m_pHead->Pop(index);

		m_size--;

		return ret;
	}

	PairNode<TKey, TValue>* GetNode(const TKey&& key_)
	{
		int index = Find(key_);

		// ã�� ������ ����
		if (index == -1)
		{
			return nullptr;
		}

		if (index == 0)
		{
			return GetFrontNode();
		}

		PairNode<TKey, TValue>* ret = m_pHead->Pop(index);

		m_size--;

		return ret;
	}

	int size() const { return m_size; }

	void clear()
	{
		delete m_pHead;
		m_pHead = nullptr;

		m_size = 0;
	}

	// can throw out_of_range
	TValue& operator[](int index_)
	{
		if (m_pHead == nullptr)
		{
			throw std::out_of_range("");
		}

		try
		{
			return m_pHead->GetRef(index_);
		}
		catch (std::out_of_range e)
		{
			throw std::out_of_range("");
		}
	}

private:
	PairNode<TKey, TValue>* m_pHead;
	int m_size;
};