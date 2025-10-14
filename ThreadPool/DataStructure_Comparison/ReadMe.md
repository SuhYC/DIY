# 스레드풀 자료구조 비교
간단하게 스레드풀에 사용할 작업큐를 비교해봤다. <br/>
[concurrent_priority_queue 문서](https://learn.microsoft.com/en-us/cpp/parallel/concrt/reference/concurrent-priority-queue-class?view=msvc-170)에 lock-free 에 대한 언급이 없어서 직접 실험. <br/>
```std::mutex```와 ```std::priority_queue<T>```를 사용하는게 ```concurrent_priority_queue<T>```를 사용하는 것과 비교해 어떤 결과가 나오는 지 확인하기 위함. <br/>

# 비교군
```std::queue<T>```, ```std::priority_queue<T>```, ```Concurrency::concurrent_queue<T>```, ```Concurrency::concurrent_priority_queue<T>```

# 실험방법
2000000개의 ```int``` 데이터를 ```push```, ```try_pop```하는 과정을 4개의 스레드로 동시에 수행하는데 걸리는 시간을 측정한다. <br/>
```std::queue<T>```와 ```std::priority_queue<T>```는 ```front()```와 ```top()```을 확인하는 과정과 락을 사용하는 과정도 거친다. <br/>
당연히 데이터를 정확히 가져오는게 중요하기 때문. <br/>

이후 결과는 ms단위로 출력한다. <br/>

# 각 테스트코드
### ```std::queue<T>``` <br/>
```cpp
void TestQ(std::queue<int>& q)
{
    for (int i = 0; i < MAX_JOB; i++)
    {
        {
            std::lock_guard<std::mutex> guard(m);
            q.push(i);
        }

        {
            std::lock_guard<std::mutex> guard(m);
            q.push(MAX_JOB - i);
        }

        {
            std::lock_guard<std::mutex> guard(m);
            if (q.empty())
            {
                if (DEBUG)
                {
                    std::cout << "Empty\n";
                }
                
                continue;
            }

            int j = q.front();
            if (DEBUG)
            {
                std::cout << j << '\n';
            }
            q.pop();
        }

        {
            std::lock_guard<std::mutex> guard(m);
            if (q.empty())
            {
                if (DEBUG)
                {
                    std::cout << "Empty\n";
                }
                continue;
            }

            int j = q.front();
            if (DEBUG)
            {
                std::cout << j << '\n';
            }
            q.pop();
        }
    }

    return;
}
```

### ```std::priority_queue<T>```
```cpp
void TestPQ(std::priority_queue<int>& q)
{
    for (int i = 0; i < MAX_JOB; i++)
    {
        {
            std::lock_guard<std::mutex> guard(m);
            q.push(i);
        }

        {
            std::lock_guard<std::mutex> guard(m);
            q.push(MAX_JOB - i);
        }

        {
            std::lock_guard<std::mutex> guard(m);
            if (q.empty())
            {
                if (DEBUG)
                {
                    std::cout << "Empty\n";
                }
                continue;
            }

            int j = q.top();
            if (DEBUG)
            {
                std::cout << j << '\n';
            }
            q.pop();
        }

        {
            std::lock_guard<std::mutex> guard(m);
            if (q.empty())
            {
                if (DEBUG)
                {
                    std::cout << "Empty\n";
                }
                continue;
            }

            int j = q.top();
            if (DEBUG)
            {
                std::cout << j << '\n';
            }
            q.pop();
        }
    }

    return;
}
```

### ```Concurrency::concurrent_queue<T>```
```cpp
void TestCQ(Concurrency::concurrent_queue<int>& q)
{
    for (int i = 0; i < MAX_JOB; i++)
    {
        q.push(i);
        q.push(MAX_JOB - i);

        int j;
        if (q.try_pop(j))
        {
            if (DEBUG)
            {
                std::cout << j << '\n';
            }
        }
        else
        {
            if (DEBUG)
            {
                std::cout << "Empty\n";
            }
        }

        if (q.try_pop(j))
        {
            if (DEBUG)
            {
                std::cout << j << '\n';
            }
        }
        else
        {
            if (DEBUG)
            {
                std::cout << "Empty\n";
            }
        }
    }

    return;
}
```

### ```Concurrency::concurrent_priority_queue<T>```
```cpp
void TestCPQ(Concurrency::concurrent_priority_queue<int>& q)
{
    for (int i = 0; i < MAX_JOB; i++)
    {
        q.push(i);
        q.push(MAX_JOB - i);

        int j;
        if (q.try_pop(j))
        {
            if (DEBUG)
            {
                std::cout << j << '\n';
            }
        }
        else
        {
            if (DEBUG)
            {
                std::cout << "Empty\n";
            }
        }

        if (q.try_pop(j))
        {
            if (DEBUG)
            {
                std::cout << j << '\n';
            }
        }
        else
        {
            if (DEBUG)
            {
                std::cout << "Empty\n";
            }
        }
    }

    return;
}
```

# 실험결과
```std::queue<T>``` : 3505ms <br/>
```std::priority_queue<T>``` : 6990ms <br/>
```Concurrency::concurrent_queue<T>``` : 665ms <br/>
```Concurrency::concurrent_priority_queue<T>``` : 3197ms <br/>

# 분석
일단 ```std::mutex``` + ```std::priority_queue<T>```를 사용하는것 보다는 확실하게 ```Concurrency::concurrent_priority_queue<T>```를 사용하는 것이 유리하다. <br/>
그리고 당연하지만 ```Concurrency::concurrent_queue<T>```가 확실하게 다른 자료구조에 비해 빠르다. <br/>
일반적의 큐의 push/pop은 O(1)이지만, 우선순위큐는 이름만 큐이지 힙의 구조를 하기 때문에 push/pop이 O(log(N))이기 때문. <br/>
# 결론
특정 시점 이후에 동작해야할 함수가 아니라면, ```Concurrency::concurrent_queue<T>```에 넣는게 효율적이고, 오버헤드를 고려하고도 특정 시점에 수행되어야하는 동작에 한해서만 ```Concurrency::concurrent_priority_queue<T>```에 넣어서 처리하는 것이 좋겠다. <br/>

# 전체코드
```cpp
#include <iostream>
#include <queue>
#include <concurrent_queue.h>
#include <concurrent_priority_queue.h>
#include <chrono>
#include <mutex>

const int MAX_JOB = 10000000;
const int MAX_THREADS = 4;

const bool DEBUG = false;

std::mutex m;

void TestQ(std::queue<int>& q)
{
    for (int i = 0; i < MAX_JOB; i++)
    {
        {
            std::lock_guard<std::mutex> guard(m);

            q.push(i);
        }

        {
            std::lock_guard<std::mutex> guard(m);
            q.push(MAX_JOB - i);
        }

        {
            std::lock_guard<std::mutex> guard(m);
            if (q.empty())
            {
                if (DEBUG)
                {
                    std::cout << "Empty\n";
                }
                
                continue;
            }

            int j = q.front();
            if (DEBUG)
            {
                std::cout << j << '\n';
            }
            q.pop();
        }

        {
            std::lock_guard<std::mutex> guard(m);
            if (q.empty())
            {
                if (DEBUG)
                {
                    std::cout << "Empty\n";
                }
                continue;
            }

            int j = q.front();
            if (DEBUG)
            {
                std::cout << j << '\n';
            }
            q.pop();
        }
    }

    return;
}

void TestPQ(std::priority_queue<int>& q)
{
    for (int i = 0; i < MAX_JOB; i++)
    {
        {
            std::lock_guard<std::mutex> guard(m);
            q.push(i);
        }

        {
            std::lock_guard<std::mutex> guard(m);
            q.push(MAX_JOB - i);
        }

        {
            std::lock_guard<std::mutex> guard(m);
            if (q.empty())
            {
                if (DEBUG)
                {
                    std::cout << "Empty\n";
                }
                continue;
            }

            int j = q.top();
            if (DEBUG)
            {
                std::cout << j << '\n';
            }
            q.pop();
        }

        {
            std::lock_guard<std::mutex> guard(m);
            if (q.empty())
            {
                if (DEBUG)
                {
                    std::cout << "Empty\n";
                }
                continue;
            }

            int j = q.top();
            if (DEBUG)
            {
                std::cout << j << '\n';
            }
            q.pop();
        }
    }

    return;
}

void TestCQ(Concurrency::concurrent_queue<int>& q)
{
    for (int i = 0; i < MAX_JOB; i++)
    {
        q.push(i);
        q.push(MAX_JOB - i);

        int j;
        if (q.try_pop(j))
        {
            if (DEBUG)
            {
                std::cout << j << '\n';
            }
        }
        else
        {
            if (DEBUG)
            {
                std::cout << "Empty\n";
            }
        }

        if (q.try_pop(j))
        {
            if (DEBUG)
            {
                std::cout << j << '\n';
            }
        }
        else
        {
            if (DEBUG)
            {
                std::cout << "Empty\n";
            }
        }
    }

    return;
}

void TestCPQ(Concurrency::concurrent_priority_queue<int>& q)
{
    for (int i = 0; i < MAX_JOB; i++)
    {
        q.push(i);
        q.push(MAX_JOB - i);

        int j;
        if (q.try_pop(j))
        {
            if (DEBUG)
            {
                std::cout << j << '\n';
            }
        }
        else
        {
            if (DEBUG)
            {
                std::cout << "Empty\n";
            }
        }

        if (q.try_pop(j))
        {
            if (DEBUG)
            {
                std::cout << j << '\n';
            }
        }
        else
        {
            if (DEBUG)
            {
                std::cout << "Empty\n";
            }
        }
    }

    return;
}

int main() {
    std::queue<int> q;
    std::priority_queue<int> pq;
    Concurrency::concurrent_priority_queue<int> cpq;
    Concurrency::concurrent_queue<int> cq;

    std::vector<std::thread> ths;

    ths.reserve(MAX_THREADS);

    std::chrono::steady_clock::time_point st = std::chrono::steady_clock::now(), end;

    for (int i = 0; i < MAX_THREADS; i++)
    {
        //ths.emplace_back([&q]() {TestQ(q); });
        //ths.emplace_back([&pq]() {TestPQ(pq); });
        //ths.emplace_back([&cq]() {TestCQ(cq); });
        ths.emplace_back([&cpq]() {TestCPQ(cpq); });
    }

    for (auto& th : ths)
    {
        if (th.joinable())
        {
            th.join();
        }
    }

    end = std::chrono::steady_clock::now();

    auto dur = end - st;

    std::cout << dur.count() / 1000000;

    return 0;
}
```
