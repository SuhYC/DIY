#include <iostream>
#include "UnorderedSet.hpp"
#include "UnorderedMap.hpp"


void TestSet()
{
	std::cout << "Set Test Start.\n";
	Unordered_Set<int> uset;

	if (uset.empty())
	{
		std::cout << "empty correct\n";
	}

	if (uset.find(1))
	{
		std::cout << "find incorrect\n";
	}

	if (uset.insert(5))
	{
		std::cout << "insert correct\n";
	}

	if (uset.insert(5))
	{
		std::cout << "insert incorrect\n";
	}

	if (uset.find(5))
	{
		std::cout << "find correct\n";
	}

	uset.erase(5);
	if (uset.find(5))
	{
		std::cout << "erase incorrect\n";
	}

	uset.insert(5);
	uset.insert(1);

	uset.clear();

	if (uset.empty())
	{
		std::cout << "clear correct\n";
	}

	return;
}

void TestMap()
{
	std::cout << "Map Test Start.\n";
	Unordered_Map<int, int> uset;

	if (uset.empty())
	{
		std::cout << "empty correct\n";
	}

	if (uset.find(1))
	{
		std::cout << "find incorrect\n";
	}

	if (uset.insert(5, 7))
	{
		std::cout << "insert correct\n";
	}

	if (uset.insert(5, 5))
	{
		std::cout << "insert incorrect\n";
	}

	if (uset.find(5))
	{
		std::cout << "find correct\n";
	}

	uset.erase(5);
	if (uset.find(5))
	{
		std::cout << "erase incorrect\n";
	}

	uset.insert(5, 7);
	uset.insert(1, 2);

	try
	{
		std::cout << uset[5];
		std::cout << " : operator[] correct\n";
	}
	catch (std::out_of_range e)
	{
		std::cout << "out of range\n";
	}

	uset.clear();

	if (uset.empty())
	{
		std::cout << "clear correct\n";
	}

	return;
}

int main()
{
	TestSet();
	TestMap();

	return 0;
}