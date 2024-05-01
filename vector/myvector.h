#pragma once
#include <memory>

template <typename T>
class MyVector
{
public:
	class iterator
	{
	public:
		// 생성자 및 소멸자
		iterator(T* p_ = nullptr, size_t idx_ = 0) : pv(p_), pos(idx_) {};
		~iterator() {};

		// 전위 증가 연산자
		iterator& operator++()
		{
			++pos;
			return *this;
		}
		// 후위 증가 연산자
		iterator operator++(int)
		{
			auto tmp = *this;
			this->operator++();
			return tmp;
		}
		// 전위 감소 연산자
		iterator& operator--()
		{
			--pos;
			return *this;
		}
		// 후위 감소 연산자
		iterator operator--(int)
		{
			auto tmp = *this;
			this->operator--();
			return tmp;
		}
		
		iterator operator+(size_t s_)
		{
			return iterator(&pv[pos], pos + s_);
		}

		iterator operator-(size_t s_)
		{
			return iterator(&pv[pos], pos - s_);
		}

		bool operator==(const iterator& it_)
		{
			return &pv[pos] == &it_.pv[it_.pos];
		}

		bool operator!=(const iterator& it_)
		{
			return &pv[pos] != &it_.pv[it_.pos];
		}

		T& operator*()
		{
			return pv[pos];
		}

		const T* operator->()
		{
			return &pv[pos];
		}

	private:
		friend class MyVector;
		T* pv;
		size_t pos;
	};

	explicit MyVector(size_t cap_ = 1) : p(new T[cap_]{}), _capacity(cap_) { _size = 0; }
	~MyVector()
	{
		if (p)
		{
			delete[] p;
			p = nullptr;
		}
	}

	MyVector<T>& operator=(const MyVector& rhs_)
	{
		if (this != &rhs_)
		{
			MyVector tmp(rhs_);
			tmp.swap(*this);
		}

		return *this;
	}

	// 이동 대입 연산자
	MyVector<T>& operator=(MyVector&& rhs_)
	{
		rhs_.swap(*this);

		return *this;
	}

	T& operator[](size_t pos_) { return p[pos_]; }

	inline T& front() { return p[0]; }
	inline const T& front() const { return p[0]; }
	inline T& back() { return p[_size - 1]; }
	inline const T& back() const { return p[_size - 1]; }

	inline iterator begin() { return iterator(p); }
	inline iterator end() { return iterator(p, _size); }

	// assign : 기존의 데이터를 버리고 count_개의 val로 벡터를 구성한다.
	inline void assign(size_t count_, const T& val)
	{
		T* tmp = new T[count_];
		for (size_t i = 0; i < count_; i++)
		{
			memcpy(&tmp[i], &val, sizeof(T));
		}
		delete[] p;
		p = tmp;

		_size = _capacity = count_;
	}

	// assign : 기존 데이터를 버리고 first_부터 last_전까지의 데이터를 복사하여 새로 구성한다.
	inline void assign(iterator first_, iterator last_)
	{
		if (&first_ != &last_)
		{
			size_t size = last_.pos - first_.pos;
			T* tmp = new T[size];
			memcpy(tmp, &(first_.pv[first_.pos]), sizeof(T) * size);
			_size = _capacity = size;

			delete[] p;
			p = tmp;
		}
		else
		{
			clear();
		}
	}

	// erase : it_이 가리키는 데이터를 삭제.
	inline iterator erase(const iterator& it_)
	{
		memcpy(&p[it_.pos], &p[it_.pos + 1], sizeof(T) * (_size - it_.pos - 1));
		_size--;
		return iterator(p);
	}

	// erase : sit_부터 eit_전까지 지우는 함수다. 처음에 eit_까지 지우는줄 알았다.
	inline iterator erase(const iterator& sit_, const iterator& eit_)
	{
		size_t size = eit_.pos - sit_.pos;
		memcpy(&p[sit_.pos], &p[eit_.pos], sizeof(T) * (_size - eit_.pos));
		_size -= size;
		return iterator(p);
	}

	// push_back : 데이터를 벡터의 뒤에 삽입
	inline void push_back(const T& val)
	{
		if (_size >= _capacity)
		{
			_capacity *= 2;
			T* tmp = new T[_capacity];
			memcpy(tmp, p, sizeof(T) * _size);
			delete[] p;
			p = tmp;
		}
		p[_size++] = val;
	}
	// push_back 오버로딩. 객체의 소유권을 이전.
	inline void push_back(const T&& val)
	{
		if (_size >= _capacity)
		{
			_capacity *= 2;
			T* tmp = new T[_capacity];
			memcpy(tmp, p, sizeof(T) * _size);
			delete[] p;
			p = tmp;
		}
		p[_size++] = std::move(val);
	}


	// discard object는 해줘야하나..? 데이터를 무시하는 원리라..
	
	// pop_back : capacity는 유지한 채 현재 기록된 데이터를 무시.
	inline void pop_back() { _size -= _size > 0 ? 1 : 0; }
	// clear : capacity는 유지한 채 현재 기록된 데이터를 무시.
	inline void clear() { _size = 0; }
	// shrink_to_fit : 여분의 공간을 없앰. -> capacity를 size에 맞춤.
	inline void shrink_to_fit()
	{
		if (_size < _capacity)
		{
			T* tmp = new T[_size];
			memcpy(tmp, p, sizeof(T) * _size);
			delete[] p;
			p = tmp;
			_capacity = _size;
		}
	}

	inline void insert(const iterator& it, const T& val)
	{
		if (_capacity > _size)
		{
			memcpy(&p[it.pos + 1], &p[it.pos], sizeof(T) * (_size - it.pos));
			p[it.pos] = val;
			++_size;
		}
		else
		{
			T* tmp = new T[_size + 1];
			memcpy(tmp, p, sizeof(T) * it.pos);
			tmp[it.pos] = val;
			memcpy(&tmp[it.pos + 1], &p[it.pos], sizeof(T) * (_size - it.pos));
			delete[] p;
			p = tmp;
			_size++;
			_capacity = _size;
		}
	}

	// resize : val을 따로 입력하지 않으면 디폴트로 기본생성자를 넣는다.
	inline void resize(size_t n, const T& val = T())
	{
		if (n != _size)
		{
			T* tmp = new T[n];
			if (n > _size)
			{
				memcpy(tmp, p, sizeof(T) * _size);
				for (size_t i = _size; i < n; i++)
				{
					memcpy(&tmp[i], &val, sizeof(T));
				}
				_size = _capacity = n;
			}
			else
			{
				memcpy(tmp, p, sizeof(T) * n);
				_size = n;
			}
			delete[] p;
			p = tmp;
		}
	}

	inline void reserve(size_t n)
	{
		if (n <= _capacity)
		{
			return;
		}
		_capacity = n;
		T* tmp = new T[_capacity];
		
		memcpy(tmp, p, sizeof(T) * _size);

		delete[] p;
		p = tmp;
	}

	inline void swap(MyVector<T>& rhs)
	{
		std::swap(p, rhs.p);
		std::swap(_size, rhs._size);
		std::swap(_capacity, rhs._capacity);
	}

	inline bool empty() { return _size == 0; }
	
	inline size_t size() { return _size; }
	inline size_t capacity() { return _capacity; }

private:

	T* p;
	size_t _size, _capacity;
};