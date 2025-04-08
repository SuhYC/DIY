#include "MySerializeLib.hpp"

int main()
{
	// ----- ����ȭ ���� -----
	SerializeLib slib;
	bool ret = slib.Init(21);
	if (!ret)
	{
		std::cerr << "main : slib �޸� �Ҵ� ����.\n";
		return 0;
	}

	// 4����Ʈ ��ȣ�ִ� ����
	ret = slib.Push(4);
	if (!ret)
	{
		std::cerr << "main : 4����Ʈ ���� ������ ���� ����.\n";
		return 0;
	}

	std::cout << slib.GetSize() << '/' << slib.GetCap() << '\n'; // �뷮 4/21

	// 8����Ʈ �Ǽ�
	ret = slib.Push(1.5);
	if (!ret)
	{
		std::cerr << "main : 8����Ʈ �ε��Ҽ��� ������ ���� ����.\n";
		return 0;
	}

	std::cout << slib.GetSize() << '/' << slib.GetCap() << '\n'; // �뷮 12/21

	// ����9����Ʈ ���ڿ�
	std::string str("Hello"); // 4����Ʈ(size���) + 5����Ʈ(���ڿ�) = 9����Ʈ

	ret = slib.Push(str);
	if (!ret)
	{
		std::cerr << "main : 5����Ʈ ���ڿ� (+4����Ʈ��� = 9����Ʈ) ������ ���� ����.\n";
		return 0;
	}

	std::cout << slib.GetSize() << '/' << slib.GetCap() << '\n'; // �뷮 21/21

	const char* serializedStr = slib.GetData(); // [4] [1.0] ["Hello"]
	if (serializedStr == nullptr)
	{
		std::cerr << "main : slib ����ȭ ������ ���� ����\n";
		return 0;
	}

	// ----- ������ȭ ���� -----
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