# Smart Pointer
## 스마트 포인터

### std::shared_ptr
가장 먼저 만들어본게 shared pointer. 처음에 weak count 생각 안하고 만들었다가 수정하느라 머리아팠다. <br/>
중점적으로 볼 건 소멸자나 대입연산자 등에서 UseCount(내 코드에선 mpRefCount)를 적절히 감소시키고<br/>
체크하여 소유한 포인터를 제때 delete시켜주는거.
### std::unique_ptr
다음으로 만들어본건 unique pointer. 구조가 복잡하지는 않다. <br/>
중점적으로 볼 건 복사생성자와 대입연산자를 제거하는것.
### std::weak_ptr
마지막으로 만들어본건 weak pointer. 만들다가 결국 shared pointer와 한몸처럼 작동하려면<br/>
UseCount와 WeakCount를 공유할 수 있어야 한다는 것을 늦게 깨달음.. <br/>
중점적으로 볼 건 Raw Pointer로 직접 생성자를 호출하는 건 안된다는 것과 <br/>
UseCount는 0이 되어 데이터는 delete되었지만 Weak Pointer는 아직 가리키고 있을 수 있으니<br/>
그에 대한 처리를 해주어야된다는 것

## 보충할 점
- operator[]에 대한 처리.
- thread-safe하게 만들어보기 (그냥 lock_guard 남발하면 성능에 좋지 않을 것 같으니 atomic을 사용할 수 있다면..)
