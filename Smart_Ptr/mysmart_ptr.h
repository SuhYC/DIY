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

	// 복사생성자
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

	// 이동생성자
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
			// 가리키고 있는 공유포인터가 없으므로 포인터를 해제
			if (*mpRefCount == 0)
			{
				// 가리키고 있는 스마트포인터가 아예 없으므로 카운트도 해제.
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

	// 대입연산자
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
	// 이동대입연산자
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
	
	// 스마트 포인터 입장에서 배열포인터인지 구분할 방법이 생각이 나지 않음.
	// 배열포인터와 단일포인터가 혼용되면 delete에서 문제가 생길 수 있음.
	// 표준라이브러리에서는 deleter를 커스텀하는 방법으로 해결한거같음..
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

	// 복사생성자 삭제
	MyUniquePtr(const MyUniquePtr&) = delete;

	// 이동생성자
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

	// 이동대입연산자
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

	// 대입생성자 삭제
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

	// 스마트 포인터 입장에서 배열포인터인지 구분할 방법이 생각이 나지 않음.
	// 배열포인터와 단일포인터가 혼용되면 delete에서 문제가 생길 수 있음.
	// 해결책으로는 대입연산자에 raw pointer가 대입되는걸 막고 생성자에도 배열포인터임을 명시해야할 것 같음.
	// 표준라이브러리에서는 deleter를 커스텀하는 방법으로 해결한거같음..
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