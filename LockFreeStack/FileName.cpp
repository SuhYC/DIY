#include <iostream>
#include <vector>
#include <thread>
#include "MyStack.hpp"
#include <ctime>

const int THWORK = 1000000;
const int THNUM = 10;

void threadWork(MyStack<int>& st)
{
    for (int i = 0; i < THWORK;)
    {
        MyStack<int>::Node* tmp = st.pop();
        if (tmp != nullptr)
        {
            i++;

            int job = 0;

            while (job++ < 100)
            {

            }

            st.push(tmp);
        }
    }

    return;
}

void threadWorkMalloc()
{
    for (int i = 0; i < THWORK; i++)
    {
        MyStack<int>::Node* tmp = new MyStack<int>::Node(i);

        int job = 0;

        while (job++ < 100)
        {

        }

        delete tmp;
    }
}

int main() {

    MyStack<int> st;

    std::vector<std::thread> v;

    st.Init(1);

    v.reserve(THNUM);

    //----------memory pool (atomic) check-----

    clock_t start = clock();

    for (int i = 0; i < THNUM; i++)
    {
        v.emplace_back(threadWork, std::ref(st));
    }

    for (int i = 0; i < THNUM; i++)
    {
        v[i].join();
    }

    clock_t end = clock();

    std::cout << end - start << "msec.\n";

    //-----------aba check-----

    MyStack<int>::Node* tmp = st.pop();

    int cnt = 0;

    while (tmp != nullptr)
    {
        cnt++;
        delete tmp;
        tmp = st.pop();
    }

    std::cout << cnt << "nodes left.\n";

    v.clear();
    v.reserve(THNUM);

    //------------malloc-----

    start = clock();

    for (int i = 0; i < THNUM; i++)
    {
        v.emplace_back(threadWorkMalloc);
    }

    for (int i = 0; i < THNUM; i++)
    {
        v[i].join();
    }

    end = clock();

    std::cout << end - start << "msec.\n";

    return 0;
}