# 스레드풀
작업을 큐에 넣는 구조로, 큐에 작업이 들어오면 작업스레드가 해당 작업을 처리하는 방식으로 동작하는 모델. <br/>
일단 3가지의 방식을 비교해보기로 한다. <br/>
큐 자체는 lock-free queue를 사용하기로 한다. <br/>

## std::this_thread::sleep_for 기반 (C++11)
큐를 확인하고 큐가 비어있다면 스레드를 잠시 sleep상태로 변경하고 이후에 재확인한다. <br/>
가장 간단한 구현 형식이며, 폴링 형태라고 봐도 무방. <br/>
다른 실행흐름이 알려주지 않아도 일정시간마다 큐를 확인하는 구조이다. <br/>

<details>
<summary>코드 접기/펼치기</summary>

```cpp
void Enqueue(Job* pJob) override
{
    jobq.push(pJob);

    return;
}

void Thread()
{
    Job* pJob = nullptr;

    while (m_IsRun)
    {
        if (!jobq.try_pop(pJob))
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1)); // 잠시 후에 다시 확인!
            continue;
        }

        if (pJob != nullptr)
        {
            pJob->Execute();

            delete pJob;
            pJob = nullptr;
        }
    }
}
```
</details>

#### 장점
1. 구현이 간단하다 <br/>
2. 별도의 동기화가 필요없다. (큐는 ```concurrent_queue```를 쓰니까)

#### 단점
1. 반응이 늦다.
2. sleep_for의 매개변수를 작게 만들수록 성능이 좋아지다가, 0에 가까워지면 활성화되어있는 스레드가 너무 많아 불필요한 CPU점유가 늘어난다.

## std::condition_variable 기반 (C++11)
내부적으로 ```std::unique_lock```을 사용하며 락을 통한 조건대기하는 방식. <br/>
함수 반환 시 락을 습득한 상태이기 때문에 락이 필요한 로직에서는 상당히 편리하게 동기화할 수 있다. <br/>
다만 해당 실험에서는 lock-free queue를 사용하기 때문에 락을 굳이 걸 필요는 당연히 없다. <br/>
```sleep_for```기반과 비교하면 그래도 다른 생산자역할을 하는 실행흐름에서 소비자실행흐름을 깨워줄 수 있다는 점. <br/>

<details>
<summary>코드 접기/펼치기</summary>
    
```cpp
void Enqueue(Job* pJob) override
{
    jobq.push(pJob);

    cv_jobq.notify_one(); // 스레드 하나 깨워주세요!

    return;
}

void Thread()
{
    Job* pJob = nullptr;

    while (m_IsRun)
    {
        std::unique_lock<std::mutex> lock(mu);
        cv_jobq.wait(lock, [this]() {return !this->jobq.empty() || !m_IsRun; });

        lock.unlock(); // 여기 코드에서는 락이 필요 없어서 그냥 바로 해제 ㅠ

        jobq.try_pop(pJob);

        if (pJob != nullptr)
        {
            pJob->Execute();

            delete pJob;
            pJob = nullptr;
        }
    }
}
```
</details>

#### 장점
1. 반환과 동시에 락을 획득한 상태이기 때문에 별도의 동기화 로직이 필요한 경우 유용하다.
2. 생산자 실행흐름이 소비자 실행흐름을 바로 깨워줄 수 있다.

#### 단점
1. 락이 불필요한 경우 락 습득과 해제에 불필요한 오버헤드가 발생한다.

## std::atomic_wait 기반 (C++20)
락 습득 없이 특정 원자 변수 값이 바뀌기를 기다릴 수 있는 방식. <br/>
cv모델과 비슷하게 생산자/소비자 구조로 소비자 실행흐름을 깨워줄 수 있다. <br/>

<details>
<summary>코드 접기/펼치기</summary>
   
```cpp
void Enqueue(Job* pJob) override
{
    jobq.push(pJob);

    atm++; // cv랑은 다르게 값을 변화시켜주어야한다.

    std::atomic_notify_one(&atm); // 스레드 하나 깨워주세요!

    return;
}

void Thread()
{
    Job* pJob = nullptr;

    while (m_IsRun)
    {
        if (!jobq.try_pop(pJob))
        {
            std::atomic_wait(&atm, 0);
            continue;
        }
        
        atm--; // 증가시켰던거 다시 감소!

        if (pJob != nullptr)
        {
            pJob->Execute();

            delete pJob;
            pJob = nullptr;
        }
    }
}
```
</details>

#### 장점
1. 생산자 실행흐름이 소비자 실행흐름을 깨워줄 수 있다.
2. std::condition_variable에 비해 락 습득을 하지 않기 때문에 오버헤드가 적다.

#### 단점
1. 혼자만 버전이 다르다. (C++20이기 때문에 C++버전이 낮은 프로젝트에서 사용할 수 없다.)
2. 단일 원자 변수에 대해서만 wait가 가능하여 복잡한 조건식에는 불편할 수 있다.

## 전체 코드

<details>
<summary>코드 접기/펼치기</summary>
   
```cpp
#include <iostream>
#include <chrono>
#include <concurrent_queue.h>
#include <thread>

#include <vector>

#include <mutex>
#include <condition_variable>

#include <conio.h>

std::chrono::steady_clock::time_point start;
std::chrono::steady_clock::time_point end;

constexpr uint32_t MAX_THREADS = 12;
constexpr uint32_t MAX_JOB_COUNT = 1000;

class Job
{
public:
    void Execute()
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        end = std::chrono::high_resolution_clock::now();

        std::chrono::duration<double, std::milli> duration = end - start;

        std::cout << duration.count() << '\n';

        return;
    }
};


class Pool
{
public:
    virtual void Enqueue(Job* pJob) = 0;
};

class SleepWaitThreadPool : public Pool
{
public:
    SleepWaitThreadPool() : m_IsRun(false)
    {

    }

    ~SleepWaitThreadPool()
    {
        while (!jobq.empty())
        {
            Job* pJob = nullptr;

            jobq.try_pop(pJob);

            if (pJob != nullptr)
            {
                delete pJob;
            }
        }
    }

    void Init()
    {
        m_IsRun = true;

        ths.reserve(MAX_THREADS);

        for (int i = 0; i < MAX_THREADS; i++)
        {
            ths.emplace_back([this]() {Thread(); });
        }

        return;
    }

    void Enqueue(Job* pJob) override
    {
        jobq.push(pJob);

        return;
    }

    void End()
    {
        m_IsRun = false;

        for (auto& t : ths)
        {
            if (t.joinable())
            {
                t.join();
            }
        }

        return;
    }

private:
    void Thread()
    {
        Job* pJob = nullptr;

        while (m_IsRun)
        {
            if (!jobq.try_pop(pJob))
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                continue;
            }

            if (pJob != nullptr)
            {
                pJob->Execute();

                delete pJob;
                pJob = nullptr;
            }
        }
    }

    bool m_IsRun;

    Concurrency::concurrent_queue<Job*> jobq;

    std::vector<std::thread> ths;
};

class ConditionVariableThreadPool : public Pool
{
public:
    ConditionVariableThreadPool() : m_IsRun(false)
    {
        
    }

    ~ConditionVariableThreadPool()
    {
        if (m_IsRun)
        {
            End();
        }

        while (!jobq.empty())
        {
            Job* pJob = nullptr;

            jobq.try_pop(pJob);

            if (pJob != nullptr)
            {
                delete pJob;
            }
        }
    }

    void Init()
    {
        m_IsRun = true;

        ths.reserve(MAX_THREADS);

        for (int i = 0; i < MAX_THREADS; i++)
        {
            ths.emplace_back([this]() {Thread(); });
        }

        return;
    }

    void Enqueue(Job* pJob) override
    {
        jobq.push(pJob);

        cv_jobq.notify_one();

        return;
    }

    void End()
    {
        m_IsRun = false;

        cv_jobq.notify_all();

        for (auto& t : ths)
        {
            if (t.joinable())
            {
                t.join();
            }
        }

        return;
    }

private:
    void Thread()
    {
        Job* pJob = nullptr;

        while (m_IsRun)
        {
            std::unique_lock<std::mutex> lock(mu);
            cv_jobq.wait(lock, [this]() {return !this->jobq.empty() || !m_IsRun; });

            lock.unlock();

            jobq.try_pop(pJob);

            if (pJob != nullptr)
            {
                pJob->Execute();

                delete pJob;
                pJob = nullptr;
            }
        }
    }

    bool m_IsRun;

    Concurrency::concurrent_queue<Job*> jobq;
    std::condition_variable cv_jobq;
    std::mutex mu;

    std::vector<std::thread> ths;
};

class AtomicWaitThreadPool : public Pool
{
public:
    AtomicWaitThreadPool() : m_IsRun(false), atm(0)
    {

    }

    ~AtomicWaitThreadPool()
    {
        if (m_IsRun)
        {
            End();
        }

        while (!jobq.empty())
        {
            Job* pJob = nullptr;

            jobq.try_pop(pJob);

            if (pJob != nullptr)
            {
                delete pJob;
            }
        }
    }

    void Init()
    {
        m_IsRun = true;

        ths.reserve(MAX_THREADS);

        for (int i = 0; i < MAX_THREADS; i++)
        {
            ths.emplace_back([this]() {Thread(); });
        }

        return;
    }

    void Enqueue(Job* pJob) override
    {
        jobq.push(pJob);

        atm++;

        std::atomic_notify_one(&atm);

        return;
    }

    void End()
    {
        m_IsRun = false;

        atm.store(-1);

        std::atomic_notify_all(&atm);

        for (auto& t : ths)
        {
            if (t.joinable())
            {
                t.join();
            }
        }

        return;
    }

private:
    void Thread()
    {
        Job* pJob = nullptr;

        while (m_IsRun)
        {
            if (!jobq.try_pop(pJob))
            {
                std::atomic_wait(&atm, 0);
                continue;
            }
            
            atm--;

            if (pJob != nullptr)
            {
                pJob->Execute();

                delete pJob;
                pJob = nullptr;
            }
        }
    }

    bool m_IsRun;

    Concurrency::concurrent_queue<Job*> jobq;
    std::atomic_int atm;

    std::vector<std::thread> ths;
};

void Test(Pool& pool)
{
    std::vector<Job*> jobs(MAX_JOB_COUNT);

    for (int i = 0; i < MAX_JOB_COUNT; i++)
    {
        jobs[i] = new Job();
    }

    start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < MAX_JOB_COUNT; i++)
    {
        pool.Enqueue(jobs[i]);
        std::this_thread::sleep_for(std::chrono::nanoseconds(250));
    }

    return;
}

int main() {
    SleepWaitThreadPool pool;
    //ConditionVariableThreadPool pool;
    //AtomicWaitThreadPool pool;

    pool.Init();

    Test(pool);

    int a = _getch();

    pool.End();

    return 0;
}

```
</details>

## 수행결과
```sleep_for``` : 3584.91ms 소요 <br/>
```condition_variable``` : 2445.94ms 소요 <br/>
```atomic_wait``` : 1897.18ms 소요

## 분석
사실 큐에 작업 1000개를 빠르게 push하고 실험해보면 큰 차이를 보이지 않는다. <br/>
해당 실험에서 가장 중요한 변인은 작업큐가 도중에 비어있는 시간이 있어야한다는 것. <br/>

아래 코드는 전체코드 중 ```Test(Pool&)```에서 미리 작성한 작업객체를 큐에 삽입하는 과정이다. <br/>
```cpp
for (int i = 0; i < MAX_JOB_COUNT; i++)
{
    pool.Enqueue(jobs[i]);
    std::this_thread::sleep_for(std::chrono::nanoseconds(250)); // 대충 밀리초당 4천개의 작업만 요청되는 경우
}
```
실제 서버를 가동해보면 클라이언트의 요청이 언제나 큐를 가득 메울정도로 있지 않을 것이다. <br/>
언젠가는 요청 빈도가 줄어 스레드가 idle상태로 넘어갈 수도 있는 거고, <br/>
이런 경우에 반응성을 확보하기 위해 notify기반 생산자 소비자 모델을 사용하는거니까.
