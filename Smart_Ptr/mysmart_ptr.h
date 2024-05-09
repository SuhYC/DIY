#pragma once

template <typename T>
class MyWeakPtr;

template <typename T>
class MySharedPtr
{
public:
	MySharedPtr()
	{
		mpRefCount = nullptr;
		mpWeakCount = nullptr;
		mp = nullptr;
	}
	MySharedPtr(std::nullptr_t)
	{
		mpRefCount = nullptr;
		mpWeakCount = nullptr;
		mp = nullptr;
	}
	MySharedPtr(T* ptr_)
	{
		mpRefCount = new size_t(1);
		mpWeakCount = new size_t(0);
		mp = ptr_;
	}

	MySharedPtr(const MyWeakPtr<T>& other_)
	{
		mpRefCount = other_.mpRefCount;
		mpWeakCount = other_.mpWeakCount;
		mp = other_.mp;
		(*mpRefCount)++;
	}

	// ���������
	MySharedPtr(const MySharedPtr<T>& other_)
	{
		mpRefCount = other_.mpRefCount;
		mpWeakCount = other_.mpWeakCount;
		mp = other_.mp;
		(*mpRefCount)++;
	}

	MySharedPtr(MyWeakPtr<T>&& other_)
	{
		mpRefCount = other_.mpRefCount;
		mpWeakCount = other_.mpWeakCount;
		mp = other_.mp;
		(*mpRefCount)++;
	}

	// �̵�������
	MySharedPtr(MySharedPtr<T>&& other_)
	{
		mpRefCount = other_.mpRefCount;
		mpWeakCount = other_.mpWeakCount;
		mp = other_.mp;
		(*mpRefCount)++;
	}

	~MySharedPtr()
	{
		if (mpRefCount != nullptr)
		{
			(*mpRefCount)--;
			// ����Ű�� �ִ� ���������Ͱ� �����Ƿ� �����͸� ����
			if (*mpRefCount == 0)
			{
				// ����Ű�� �ִ� ����Ʈ�����Ͱ� �ƿ� �����Ƿ� ī��Ʈ�� ����.
				if (*mpWeakCount == 0)
				{
					delete mpRefCount;
					delete mpWeakCount;
				}
				delete mp;
			}
		}
	}

	// shared_ptr = nullptr;
	MySharedPtr<T>& operator=(std::nullptr_t)
	{
		reset();
		return *this;
	}

	// ���Կ�����
	MySharedPtr<T>& operator=(MySharedPtr<T>& other_)
	{
		if (mpRefCount != nullptr)
		{
			(*mpRefCount)--;
			if (*mpRefCount == 0)
			{
				if ((*mpWeakCount) == 0)
				{
					delete mpRefCount;
					delete mpWeakCount;
				}
				delete mp;
			}
		}
		mpRefCount = other_.mpRefCount;
		mpWeakCount = other_.mpWeakCount;
		mp = other_.mp;
		if (mpRefCount != nullptr)
		{
			(*mpRefCount)++;
		}
		
		return *this;
	}
	// �̵����Կ�����
	MySharedPtr<T>& operator=(MySharedPtr<T>&& other_)
	{
		swap(other_);
		return *this;
	}

	void reset()
	{
		if (mpRefCount != nullptr)
		{
			(*mpRefCount)--;
			if (*mpRefCount == 0)
			{
				if (*mpWeakCount == 0)
				{
					delete mpRefCount;
					delete mpWeakCount;
				}
				delete mp;
			}
			mpWeakCount = nullptr;
			mpRefCount = nullptr;
			mp = nullptr;
		}
	}
	const T* get() { return mp; }

	// *shared_ptr
	T& operator*() { return *mp; }
	// shared_ptr->
	const T* operator->() { return mp; }
	
	// ����Ʈ ������ ���忡�� �迭���������� ������ ����� ������ ���� ����.
	// �迭�����Ϳ� ���������Ͱ� ȥ��Ǹ� delete���� ������ ���� �� ����.
	// ǥ�ض��̺귯�������� deleter�� Ŀ�����ϴ� ������� �ذ��ѰŰ���..
	//T& operator[](size_t npos_) { return mp[npos_]; }

	// if(shared_ptr){}
	operator bool() { return mp != nullptr; }

	size_t use_count()
	{
		if (mpRefCount == nullptr)
		{
			return 0;
		}

		return *mpRefCount;
	}
	bool unique()
	{
		if (mpRefCount == nullptr)
		{
			return false;
		}
		return *mpRefCount == 1;
	}
	void swap(MySharedPtr<T>& other_)
	{
		std::swap(mpWeakCount, other_.mpWeakCount);
		std::swap(mpRefCount, other_.mpRefCount);
		std::swap(mp, other_.mp);
	}

private:
	size_t* mpRefCount;
	size_t* mpWeakCount;
	T* mp;
	friend class MyWeakPtr<T>;
};

template <typename T>
class MyUniquePtr
{
public:
	MyUniquePtr() { mp = nullptr; }
	MyUniquePtr(std::nullptr_t) { mp = nullptr; }
	explicit MyUniquePtr(T* ptr_) : mp(ptr_) {}

	// ��������� ����
	MyUniquePtr(const MyUniquePtr&) = delete;

	// �̵�������
	MyUniquePtr(MyUniquePtr<T>&& other_)
	{
		mp = other_.mp;
	}

	~MyUniquePtr()
	{
		if (mp != nullptr)
		{
			delete mp;
		}
	}

	// �̵����Կ�����
	MyUniquePtr<T>& operator=(MyUniquePtr<T>&& other_)
	{
		other_.swap(*this);
		return *this;
	}

	// unique_ptr = nullptr;
	MyUniquePtr<T>& operator=(std::nullptr_t)
	{
		reset();
		return *this;
	}

	// ���Ի����� ����
	MyUniquePtr<T>& operator=(const MyUniquePtr<T>&) = delete;

	T* release()
	{
		T* tmp = mp;
		mp = nullptr;
		return tmp;
	}
	void reset()
	{
		if (mp != nullptr)
		{
			delete mp;
			mp = nullptr;
		}
	}

	void swap(MyUniquePtr<T>& other_) { std::swap(mp, other_.mp); }
	const T* get() { return mp; }

	// if(unique_ptr){}
	operator bool() { return mp != nullptr; }
	// *unique_ptr
	T& operator*() { return *mp; }
	// unique_ptr->
	const T* operator->() { return mp; }

	// ����Ʈ ������ ���忡�� �迭���������� ������ ����� ������ ���� ����.
	// �迭�����Ϳ� ���������Ͱ� ȥ��Ǹ� delete���� ������ ���� �� ����.
	// �ذ�å���δ� ���Կ����ڿ� raw pointer�� ���ԵǴ°� ���� �����ڿ��� �迭���������� ����ؾ��� �� ����.
	// ǥ�ض��̺귯�������� deleter�� Ŀ�����ϴ� ������� �ذ��ѰŰ���..
	//T& operator[](size_t npos_) { return mp[npos_]; }

private:
	T* mp;
};

template <typename T>
class MyWeakPtr
{
public:
	MyWeakPtr()
	{
		mp = nullptr;
		mpRefCount = nullptr;
		mpWeakCount = nullptr;
	}
	MyWeakPtr(std::nullptr_t)
	{
		mp = nullptr;
		mpRefCount = nullptr;
		mpWeakCount = nullptr;
	}

	MyWeakPtr(const MyWeakPtr<T>& other_)
	{
		mp = other_.mp;
		mpRefCount = other_.mpRefCount;
		mpWeakCount = other_.mpWeakCount;
		if (mpWeakCount != nullptr)
		{
			(*mpWeakCount)++;
		}
	}

	MyWeakPtr(const MySharedPtr<T>& other_)
	{
		mp = other_.mp;
		mpRefCount = other_.mpRefCount;
		mpWeakCount = other_.mpWeakCount;
		if (mpWeakCount != nullptr)
		{
			(*mpWeakCount)++;
		}
	}

	MyWeakPtr(MyWeakPtr<T>&& other_)
	{
		mp = other_.mp;
		mpRefCount = other_.mpRefCount;
		mpWeakCount = other_.mpWeakCount;
		if (mpWeakCount != nullptr)
		{
			(*mpWeakCount)++;
		}
	}

	MyWeakPtr(MySharedPtr<T>&& other_)
	{
		mp = other_.mp;
		mpRefCount = other_.mpRefCount;
		mpWeakCount = other_.mpWeakCount;
		if (mpWeakCount != nullptr)
		{
			(*mpWeakCount)++;
		}
	}

	~MyWeakPtr()
	{
		(*mpWeakCount)--;
		if (expired() && *mpWeakCount == 0)
		{
			delete mpRefCount;
			delete mpWeakCount;
		}
	}

	MyWeakPtr& operator=(std::nullptr_t)
	{
		reset();
		return *this;
	}

	MyWeakPtr& operator=(const MyWeakPtr<T>& other_)
	{
		if (mpWeakCount != nullptr)
		{
			(*mpWeakCount)--;
			if (expired() && *mpWeakCount == 0)
			{
				delete mpRefCount;
				delete mpWeakCount;
			}
		}

		mp = other_.mp;
		mpWeakCount = other_.mpWeakCount;
		mpRefCount = other_.mpRefCount;

		if (mpWeakCount != nullptr)
		{
			(*mpWeakCount)++;
		}

		return *this;
	}

	MyWeakPtr& operator=(const MySharedPtr<T>& other_)
	{
		if (mpWeakCount != nullptr)
		{
			(*mpWeakCount)--;
			if (expired() && *mpWeakCount == 0)
			{
				delete mpRefCount;
				delete mpWeakCount;
			}
		}

		mp = other_.mp;
		mpWeakCount = other_.mpWeakCount;
		mpRefCount = other_.mpRefCount;
		
		if(mpWeakCount != nullptr)
		{
			(*mpWeakCount)++;
		}

		return *this;
	}

	MyWeakPtr& operator=(MyWeakPtr<T>&& other_)
	{
		swap(other_);
		return *this;
	}

	void reset()
	{
		if (mpWeakCount != nullptr)
		{
			(*mpWeakCount)--;
			if (expired() && *mpWeakCount == 0)
			{
				delete mpRefCount;
				delete mpWeakCount;
			}
			mpRefCount = nullptr;
			mpWeakCount = nullptr;
		}
	}
	void swap(MyWeakPtr<T>& other_)
	{
		std::swap(mp, other_.mp);
		std::swap(mpWeakCount, other_.mpWeakCount);
		std::swap(mpRefCount, other_.mpRefCount);
	}
	size_t use_count()
	{
		if (mpRefCount == nullptr)
		{
			return 0;
		}
		return *mpRefCount;
	}
	bool expired() { return use_count() == 0; }
	MySharedPtr<T> lock()
	{
		if (expired())
		{
			return MySharedPtr<T>();
		}
		else
		{
			return MySharedPtr<T>(*this);
		}
	}

private:
	T* mp;
	size_t* mpRefCount;
	size_t* mpWeakCount;
	friend class MySharedPtr<T>;
};