#include "MySerializeLib.hpp"

int main()
{
	// ----- 직렬화 시작 -----
	SerializeLib slib;
	bool ret = slib.Init(21);
	if (!ret)
	{
		std::cerr << "main : slib 메모리 할당 실패.\n";
		return 0;
	}

	// 4바이트 부호있는 정수
	ret = slib.Push(4);
	if (!ret)
	{
		std::cerr << "main : 4바이트 정수 데이터 삽입 실패.\n";
		return 0;
	}

	std::cout << slib.GetSize() << '/' << slib.GetCap() << '\n'; // 용량 4/21

	// 8바이트 실수
	ret = slib.Push(1.5);
	if (!ret)
	{
		std::cerr << "main : 8바이트 부동소수점 데이터 삽입 실패.\n";
		return 0;
	}

	std::cout << slib.GetSize() << '/' << slib.GetCap() << '\n'; // 용량 12/21

	// 총합9바이트 문자열
	std::string str("Hello"); // 4바이트(size헤더) + 5바이트(문자열) = 9바이트

	ret = slib.Push(str);
	if (!ret)
	{
		std::cerr << "main : 5바이트 문자열 (+4바이트헤더 = 9바이트) 데이터 삽입 실패.\n";
		return 0;
	}

	std::cout << slib.GetSize() << '/' << slib.GetCap() << '\n'; // 용량 21/21

	const char* serializedStr = slib.GetData(); // [4] [1.0] ["Hello"]
	if (serializedStr == nullptr)
	{
		std::cerr << "main : slib 직렬화 데이터 추출 실패\n";
		return 0;
	}

	// ----- 역직렬화 시작 -----
	DeserializeLib dlib(serializedStr, slib.GetSize());

	std::cout << "DLib Start, " << dlib.GetRemainSize() << "bytes remained.\n"; // 21/21

	int iData = 0;
	double dData = 0.0;
	std::string strData;

	ret = dlib.Get(iData); // 17/21
	if (ret)
	{
		std::cout << iData << '\n';
	}
	else
	{
		std::cout << "Failed\n";
	}

	ret = dlib.Get(dData); // 9/21
	if (ret)
	{
		std::cout << dData << '\n';
	}
	else
	{
		std::cout << "Failed\n";
	}

	ret = dlib.Get(strData); // 0/21
	if (ret)
	{
		std::cout << strData << '\n';
	}
	else
	{
		std::cout << "Failed\n";
	}

	std::cout << dlib.GetRemainSize() << "bytes remained.\n";

	return 0;
}