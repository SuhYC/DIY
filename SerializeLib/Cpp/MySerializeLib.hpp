#pragma once

#include <exception>
#include <iostream>
#include <Windows.h>

const unsigned long MAX_STRING_SIZE = 1000;

/// <summary>
/// char*�� ����.
/// push�� �����͸� ������� ������ �� char*�� �����ͷ� ������ �� �ִ�.
/// �ϴ� ��Ʋ��������� �����ϴ� ������ �����Ѵ�.
/// ���ڿ� �����ʹ� 4����Ʈ ���� + ���ڿ� �� �����Ѵ�.
/// </summary>
class SerializeLib
{
public:
	SerializeLib() : data(nullptr), size(0), capacity(0) {}

	~SerializeLib()
	{
		if (data != nullptr)
		{
			delete[] data;
		}
	}
	/// <summary>
	/// ������ �����͸� �����ϰ�
	/// ���ο� ���۸� ����� �Լ�
	/// </summary>
	/// <param name="cap_"></param>
	/// <returns></returns>
	bool Init(uint32_t cap_)
	{
		if (data != nullptr)
		{
			delete[] data;
		}

		data = new(std::nothrow) char[cap_];
		size = 0;

		if (data == nullptr)
		{
			std::cerr << "SerializeLib::Init : Memory Alloc Failed.\n";
			capacity = 0;
			return false;
		}

		capacity = cap_;
		return true;
	}


	/// <summary>
	/// ������ �����͸� ������ ä
	/// ������ ũ�⸦ �����ϴ� �Լ�.
	/// ���� �����Ͱ� �����Ϸ��� ũ�⺸�� ū ��� �����Ѵ�.
	/// </summary>
	/// <param name="cap_"></param>
	/// <returns></returns>
	bool Resize(uint32_t cap_)
	{
		if (size > cap_)
		{
			return false;
		}

		if (capacity == cap_)
		{
			return true;
		}

		char* newData = new(std::nothrow) char[cap_];

		if (newData == nullptr)
		{
			std::cerr << "SerializeLib::Resize : Memory Alloc Failed.\n";
			return false;
		}

		if (data != nullptr)
		{
			CopyMemory(newData, data, size);
			delete[] data;
		}

		data = newData;
		capacity = cap_;

		return true;
	}

	template <typename T>
	typename std::enable_if<
		std::is_same<T, uint16_t>::value ||
		std::is_same<T, uint32_t>::value ||
		std::is_same<T, uint64_t>::value ||
		std::is_same<T, int16_t>::value ||
		std::is_same<T, int32_t>::value ||
		std::is_same<T, int64_t>::value ||
		std::is_same<T, float>::value ||
		std::is_same<T, double>::value ||
		std::is_same<T, std::string>::value,
		bool>::type
		Push(const T& rhs_)
	{
		if (size + sizeof(rhs_) > capacity)
		{
			return false;
		}

		CopyMemory(data + size, &rhs_, sizeof(rhs_));
		size += sizeof(rhs_);
		return true;
	}

	/// <summary>
	/// ���ڿ��� ���ڿ��� ����Ʈũ�⸦ ���� ������ ��
	/// ���ڿ��� �����Ѵ�.
	/// </summary>
	/// <param name="rhs_"></param>
	/// <returns></returns>
	template <>
	bool Push<std::string>(const std::string& rhs_)
	{
		uint32_t len = rhs_.length();

		if (size + len + sizeof(uint32_t) > capacity)
		{
			return false;
		}

		CopyMemory(data + size, &len, sizeof(uint32_t));
		size += sizeof(uint32_t);

		CopyMemory(data + size, rhs_.c_str(), len);
		size += len;
		return true;
	}

	/*
	bool Push(uint16_t rhs_)
	{
		if (size + sizeof(rhs_) > capacity)
		{
			return false;
		}

		CopyMemory(data + size, &rhs_, sizeof(rhs_));
		size += sizeof(rhs_);
		return true;
	}

	bool Push(int16_t rhs_)
	{
		if (size + sizeof(rhs_) > capacity)
		{
			return false;
		}

		CopyMemory(data + size, &rhs_, sizeof(rhs_));
		size += sizeof(rhs_);
		return true;
	}

	bool Push(uint32_t rhs_)
	{
		if (size + sizeof(rhs_) > capacity)
		{
			return false;
		}

		CopyMemory(data + size, &rhs_, sizeof(rhs_));
		size += sizeof(rhs_);
		return true;
	}

	bool Push(int32_t rhs_)
	{
		if (size + sizeof(rhs_) > capacity)
		{
			return false;
		}

		CopyMemory(data + size, &rhs_, sizeof(rhs_));
		size += sizeof(rhs_);
		return true;
	}

	bool Push(uint64_t rhs_)
	{
		if (size + sizeof(rhs_) > capacity)
		{
			return false;
		}

		CopyMemory(data + size, &rhs_, sizeof(rhs_));
		size += sizeof(rhs_);
		return true;
	}

	bool Push(int64_t rhs_)
	{
		if (size + sizeof(rhs_) > capacity)
		{
			return false;
		}

		CopyMemory(data + size, &rhs_, sizeof(rhs_));
		size += sizeof(rhs_);
		return true;
	}

	bool Push(float rhs_)
	{
		if (size + sizeof(rhs_) > capacity)
		{
			return false;
		}

		CopyMemory(data + size, &rhs_, sizeof(rhs_));
		size += sizeof(rhs_);
		return true;
	}

	bool Push(double rhs_)
	{
		if (size + sizeof(rhs_) > capacity)
		{
			return false;
		}

		CopyMemory(data + size, &rhs_, sizeof(rhs_));
		size += sizeof(rhs_);
		return true;
	}

	/// <summary>
	/// ���ڿ��� ���ڿ��� ����Ʈũ�⸦ ���� ������ ��
	/// ���ڿ��� �����Ѵ�.
	/// </summary>
	/// <param name="rhs_"></param>
	/// <returns></returns>
	bool Push(const std::string& rhs_)
	{
		uint32_t len = rhs_.length();

		if (size + len + sizeof(uint32_t) > capacity)
		{
			return false;
		}

		CopyMemory(data + size, &len, sizeof(uint32_t));
		size += sizeof(uint32_t);

		CopyMemory(data + size, rhs_.c_str(), len);
		size += len;
		return true;
	}*/

	const char* GetData() const { return data; }
	uint32_t GetSize() const { return size; }
	uint32_t GetCap() const { return capacity; }

private:
	char* data;
	uint32_t size;
	uint32_t capacity;
};

class DeserializeLib
{
public:
	/// <summary>
	/// data_�� ũ�Ⱑ len_�� ��ġ�ϴ��� �ݵ�� Ȯ���� ��.
	/// ���ۿ����÷ο찡 �߻��� �� �ִ�.
	/// </summary>
	/// <param name="data_"></param>
	/// <param name="len_"></param>
	DeserializeLib(const char* data_, uint32_t len_) : data(nullptr), size(0), idx(0)
	{
		if (data_ != nullptr)
		{
			data = new(std::nothrow) char[len_];

			if (data != nullptr)
			{
				CopyMemory(data, data_, len_);
				size = len_;
			}
		}
	}

	~DeserializeLib()
	{
		if (data != nullptr)
		{
			delete[] data;
		}
	}

	template <typename T>
	typename std::enable_if<
		std::is_same<T, uint16_t>::value ||
		std::is_same<T, uint32_t>::value ||
		std::is_same<T, uint64_t>::value ||
		std::is_same<T, int16_t>::value ||
		std::is_same<T, int32_t>::value ||
		std::is_same<T, int64_t>::value ||
		std::is_same<T, float>::value ||
		std::is_same<T, double>::value ||
		std::is_same<T, std::string>::value,
		bool>::type
		Get(T& rhs_)
	{
		if (GetRemainSize() < sizeof(rhs_))
		{
			return false;
		}

		CopyMemory(&rhs_, data + idx, sizeof(rhs_));
		idx += sizeof(rhs_);
		return true;
	}

	/// <summary>
	/// ���� �κ��� �̸� üũ�ϰ� ������ȭ�� �õ��Ѵ�.
	/// </summary>
	/// <param name="rhs_"></param>
	/// <returns></returns>
	template<>
	bool Get<std::string>(std::string& rhs_)
	{
		if (GetRemainSize() < sizeof(uint32_t))
		{
			return false;
		}

		uint32_t len{ 0 };

		CopyMemory(&len, data + idx, sizeof(uint32_t));

		if (len == 0 || len > MAX_STRING_SIZE || len + sizeof(uint32_t) > GetRemainSize())
		{
			return false;
		}

		idx += sizeof(uint32_t);

		rhs_.resize(len);
		CopyMemory(&(rhs_[0]), data + idx, len);
		idx += len;

		return true;
	}

	/*
	bool Get(uint16_t& rhs_)
	{
		if (GetRemainSize() < sizeof(rhs_))
		{
			return false;
		}

		CopyMemory(&rhs_, data + idx, sizeof(rhs_));
		idx += sizeof(rhs_);
		return true;
	}

	bool Get(int16_t& rhs_)
	{
		if (GetRemainSize() < sizeof(rhs_))
		{
			return false;
		}

		CopyMemory(&rhs_, data + idx, sizeof(rhs_));
		idx += sizeof(rhs_);
		return true;
	}

	bool Get(uint32_t& rhs_)
	{
		if (GetRemainSize() < sizeof(rhs_))
		{
			return false;
		}

		CopyMemory(&rhs_, data + idx, sizeof(rhs_));
		idx += sizeof(rhs_);
		return true;
	}

	bool Get(int32_t& rhs_)
	{
		if (GetRemainSize() < sizeof(rhs_))
		{
			return false;
		}

		CopyMemory(&rhs_, data + idx, sizeof(rhs_));
		idx += sizeof(rhs_);
		return true;
	}

	bool Get(uint64_t& rhs_)
	{
		if (GetRemainSize() < sizeof(rhs_))
		{
			return false;
		}

		CopyMemory(&rhs_, data + idx, sizeof(rhs_));
		idx += sizeof(rhs_);
		return true;
	}

	bool Get(int64_t& rhs_)
	{
		if (GetRemainSize() < sizeof(rhs_))
		{
			return false;
		}

		CopyMemory(&rhs_, data + idx, sizeof(rhs_));
		idx += sizeof(rhs_);
		return true;
	}

	bool Get(float& rhs_)
	{
		if (GetRemainSize() < sizeof(rhs_))
		{
			return false;
		}

		CopyMemory(&rhs_, data + idx, sizeof(rhs_));
		idx += sizeof(rhs_);
		return true;
	}

	bool Get(double& rhs_)
	{
		if (GetRemainSize() < sizeof(rhs_))
		{
			return false;
		}

		CopyMemory(&rhs_, data + idx, sizeof(rhs_));
		idx += sizeof(rhs_);
		return true;
	}

	/// <summary>
	/// ���� �κ��� �̸� üũ�ϰ� ������ȭ�� �õ��Ѵ�.
	/// </summary>
	/// <param name="rhs_"></param>
	/// <returns></returns>
	bool Get(std::string& rhs_)
	{
		if (GetRemainSize() < sizeof(uint32_t))
		{
			return false;
		}

		uint32_t len{ 0 };

		CopyMemory(&len, data + idx, sizeof(uint32_t));

		if (len == 0 || len > MAX_STRING_SIZE || len + sizeof(uint32_t) > GetRemainSize())
		{
			return false;
		}

		idx += sizeof(uint32_t);

		rhs_.resize(len);
		CopyMemory(&(rhs_[0]), data + idx, len);
		idx += len;

		return true;
	}*/

	uint32_t GetRemainSize() const { return size - idx; }

private:
	char* data;
	uint32_t size;
	uint32_t idx;
};