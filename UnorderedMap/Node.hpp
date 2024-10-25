#pragma once

#include <stdexcept>

/*
* �߰� ������ �����ϱ� ���� ���߿��Ḯ��Ʈ�� �����Ѵ�.
*/

template <typename T>
class LinkedListNode
{
public:
	LinkedListNode() : data(T())
	{
		prev = nullptr;
		next = nullptr;
	}

	LinkedListNode(T& a) : data(a)
	{
		prev = nullptr;
		next = nullptr;
	}

	LinkedListNode(T&& a) : data(a)
	{
		prev = nullptr;
		next = nullptr;
	}

	LinkedListNode(const T& a) : data(a)
	{
		prev = nullptr;
		next = nullptr;
	}

	LinkedListNode(const T&& a) : data(a)
	{
		prev = nullptr;
		next = nullptr;
	}

	~LinkedListNode()
	{
		if (next != nullptr)
		{
			delete next;
		}
	}

	void PushBack(LinkedListNode<T>* const pNode_)
	{
		if (pNode_ == nullptr)
		{
			return;
		}

		if (next == nullptr)
		{
			pNode_->prev = this;
			next = pNode_;
		}
		else
		{
			next->PushBack(pNode_);
		}
	}

	void PushBack(const T& val_)
	{
		if (next == nullptr)
		{
			next = new LinkedListNode<T>(val_);
			next->prev = this;
		}
		else
		{
			next->PushBack(val_);
		}
	}

	void PushBack(const T&& val_)
	{
		if (next == nullptr)
		{
			next = new LinkedListNode<T>(val_);
			next->prev = this;
		}
		else
		{
			next->PushBack(val_);
		}
	}

	void Insert(LinkedListNode<T>* const pNode_) noexcept
	{
		if (pNode_ == nullptr)
		{
			return;
		}

		pNode_->next = this->next;
		this->next->prev = pNode_;

		next = pNode_;
		pNode_->prev = this;

		return;
	}

	bool Insert(LinkedListNode<T>* const pNode_, const int index_)
	{
		if (index_ < 0)
		{
			return false;
		}

		if (index_ == 0)
		{
			Insert(pNode_);
			return true;
		}

		return Insert(pNode_, index_ - 1);
	}

	bool Insert(const T& val_, const int index_)
	{
		LinkedListNode<T>* pNode;
		try
		{
			pNode = new LinkedListNode<T>(val_);
		}
		catch (std::bad_alloc e)
		{
			return false;
		}

		return Insert(pNode, index_);
	}

	bool Insert(const T&& val_, const int index_)
	{
		LinkedListNode<T>* pNode;
		try
		{
			pNode = new LinkedListNode<T>(val_);
		}
		catch (std::bad_alloc e)
		{
			return false;
		}

		return Insert(pNode, index_);
	}

	LinkedListNode<T>* Pop(const int index_)
	{
		// �ε��� �Է��� �߸���
		if (index_ < 0)
		{
			return nullptr;
		}

		if (index_ == 0)
		{
			return Pop();
		}

		// �ε����� �ڷᱸ���� ũ�⸦ ���
		if (next == nullptr)
		{
			return nullptr;
		}

		return next->Pop(index_ - 1);
	}

	int Find(const T& data_) const
	{
		// ���� �߰��� �ش� �����Ͱ� ���� �����.
		if (data == data_)
		{
			return 0;
		}

		// �ڷᱸ�� �� �ش� ������ ����.
		if (next == nullptr)
		{
			return -1;
		}

		int ret = next->Find(data_);

		// �ش� ������ ������ -1�� ��ȯ
		if (ret == -1)
		{
			return ret;
		}
		else
		{
			return ret + 1;
		}
	}

	T GetData() const { return data; }

	LinkedListNode<T>* Pop()
	{
		if (prev != nullptr)
		{
			prev->next = next;
		}
		
		if (next != nullptr)
		{
			next->prev = prev;
		}

		this->prev = nullptr;
		this->next = nullptr;

		return this;
	}

	T data;
	LinkedListNode<T>* next;
	LinkedListNode<T>* prev;
};

// for map.
template <typename TKey, typename TValue>
class PairNode
{
public:
	PairNode() : key(TKey()), value(TValue())
	{
		prev = nullptr;
		next = nullptr;
	}

	PairNode(TKey& key_, TValue& value_) : key(key_), value(value_)
	{
		prev = nullptr;
		next = nullptr;
	}

	PairNode(TKey&& key_, TValue&& value_) : key(key_), value(value_)
	{
		prev = nullptr;
		next = nullptr;
	}

	PairNode(const TKey& key_, const TValue& value_) : key(key_), value(value_)
	{
		prev = nullptr;
		next = nullptr;
	}

	PairNode(const TKey&& key_, const TValue&& value_) : key(key_), value(value_)
	{
		prev = nullptr;
		next = nullptr;
	}

	~PairNode()
	{
		if (next != nullptr)
		{
			delete next;
		}
	}

	void PushBack(PairNode<TKey, TValue>* const pNode_)
	{
		if (pNode_ == nullptr)
		{
			return;
		}

		if (next == nullptr)
		{
			pNode_->prev = this;
			next = pNode_;
		}
		else
		{
			next->PushBack(pNode_);
		}
	}

	void PushBack(const TKey& key_, const TValue& val_)
	{
		if (next == nullptr)
		{
			next = new PairNode<TKey, TValue>(key_, val_);
			next->prev = this;
		}
		else
		{
			next->PushBack(key_, val_);
		}
	}

	void PushBack(const TKey&& key_, const TValue&& val_)
	{
		if (next == nullptr)
		{
			next = new PairNode<TKey, TValue>(key_, val_);
			next->prev = this;
		}
		else
		{
			next->PushBack(key_, val_);
		}
	}

	void Insert(PairNode<TKey, TValue>* const pNode_) noexcept
	{
		if (pNode_ == nullptr)
		{
			return;
		}

		pNode_->next = this->next;
		this->next->prev = pNode_;

		next = pNode_;
		pNode_->prev = this;

		return;
	}

	bool Insert(PairNode<TKey, TValue>* const pNode_, const int index_)
	{
		if (index_ < 0)
		{
			return false;
		}

		if (index_ == 0)
		{
			Insert(pNode_);
			return true;
		}

		return Insert(pNode_, index_ - 1);
	}

	bool Insert(const TKey& key_, const TValue& val_, const int index_)
	{
		PairNode<TKey, TValue>* pNode;
		try
		{
			pNode = new PairNode<TKey, TValue>(key_, val_);
		}
		catch (std::bad_alloc e)
		{
			return false;
		}

		return Insert(pNode, index_);
	}

	bool Insert(const TKey&& key_, const TValue&& val_, const int index_)
	{
		PairNode<TKey, TValue>* pNode;
		try
		{
			pNode = new PairNode<TKey, TValue>(key_, val_);
		}
		catch (std::bad_alloc e)
		{
			return false;
		}

		return Insert(pNode, index_);
	}

	PairNode<TKey, TValue>* Pop(const int index_)
	{
		// �ε��� �Է��� �߸���
		if (index_ < 0)
		{
			return nullptr;
		}

		if (index_ == 0)
		{
			return Pop();
		}

		// �ε����� �ڷᱸ���� ũ�⸦ ���
		if (next == nullptr)
		{
			return nullptr;
		}

		return next->Pop(index_ - 1);
	}

	int Find(const TKey& key_) const
	{
		// ���� �߰��� �ش� �����Ͱ� ���� �����.
		if (key == key_)
		{
			return 0;
		}

		// �ڷᱸ�� �� �ش� ������ ����.
		if (next == nullptr)
		{
			return -1;
		}

		int ret = next->Find(key_);

		// �ش� ������ ������ -1�� ��ȯ
		if (ret == -1)
		{
			return ret;
		}
		else
		{
			return ret + 1;
		}
	}

	TValue GetData() const { return value; }

	// can throw out_of_range
	TValue& GetRef(int index_)
	{
		if (index_ == 0)
		{
			return GetRef();
		}

		if (index_ < 0 || next == nullptr)
		{
			throw std::out_of_range("");
		}

		return next->GetRef(index_ - 1);
	}
	TValue& GetRef() { return value; }

	PairNode<TKey, TValue>* Pop()
	{
		if (prev != nullptr)
		{
			prev->next = next;
		}

		if (next != nullptr)
		{
			next->prev = prev;
		}

		this->prev = nullptr;
		this->next = nullptr;

		return this;
	}

	TKey key;
	TValue value;
	PairNode<TKey, TValue>* next;
	PairNode<TKey, TValue>* prev;
};