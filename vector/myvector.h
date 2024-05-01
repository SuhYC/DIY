#pragma once
#include <memory>

template <typename T>
class MyVector
{
public:
	class iterator
	{
	public:
		// ������ �� �Ҹ���
		iterator(T* p_ = nullptr, size_t idx_ = 0) : pv(p_), pos(idx_) {};
		~iterator() {};

		// ���� ���� ������
		iterator& operator++()
		{
			++pos;
			return *this;
		}
		// ���� ���� ������
		iterator operator++(int)
		{
			auto tmp = *this;
			this->operator++();
			return tmp;
		}
		// ���� ���� ������
		iterator& operator--()
		{
			--pos;
			return *this;
		}
		// ���� ���� ������
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

	// �̵� ���� ������
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

	// assign : ������ �����͸� ������ count_���� val�� ���͸� �����Ѵ�.
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

	// assign : ���� �����͸� ������ first_���� last_�������� �����͸� �����Ͽ� ���� �����Ѵ�.
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

	// erase : it_�� ����Ű�� �����͸� ����.
	inline iterator erase(const iterator& it_)
	{
		memcpy(&p[it_.pos], &p[it_.pos + 1], sizeof(T) * (_size - it_.pos - 1));
		_size--;
		return iterator(p);
	}

	// erase : sit_���� eit_������ ����� �Լ���. ó���� eit_���� ������� �˾Ҵ�.
	inline iterator erase(const iterator& sit_, const iterator& eit_)
	{
		size_t size = eit_.pos - sit_.pos;
		memcpy(&p[sit_.pos], &p[eit_.pos], sizeof(T) * (_size - eit_.pos));
		_size -= size;
		return iterator(p);
	}

	// push_back : �����͸� ������ �ڿ� ����
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
	// push_back �����ε�. ��ü�� �������� ����.
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


	// discard object�� ������ϳ�..? �����͸� �����ϴ� ������..
	
	// pop_back : capacity�� ������ ä ���� ��ϵ� �����͸� ����.
	inline void pop_back() { _size -= _size > 0 ? 1 : 0; }
	// clear : capacity�� ������ ä ���� ��ϵ� �����͸� ����.
	inline void clear() { _size = 0; }
	// shrink_to_fit : ������ ������ ����. -> capacity�� size�� ����.
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

	// resize : val�� ���� �Է����� ������ ����Ʈ�� �⺻�����ڸ� �ִ´�.
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