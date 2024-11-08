#include "Time_Based_PriorityQueue.hpp"
#include <iostream>
#include <conio.h>

Time_Based_PriorityQueue q;

void Job(time_t ReqTime, time_t expectedTime)
{
	time_t currentTime = time(NULL);

	std::cout << currentTime - ReqTime << ", " << expectedTime << '\n'; // 동일한 출력이 나오면 성공!

	return;
}

void Reserve(time_t elapsedTime)
{
	time_t ReqTime = time(NULL);

	q.push(ReqTime + elapsedTime, [=]() {Job(ReqTime, elapsedTime); });

	return;
}

int main()
{
	Reserve(5);
	Reserve(3);
	Reserve(1);

	_getch(); // 요청한 시간까지는 기다려주세요

	return 0;
}