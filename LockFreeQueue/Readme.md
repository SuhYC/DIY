# Lock-Free Queue
## Michael-Scott방식의 lock-free queue

### 일반적으로 어려운 부분은?
lock-free stack에 비해 lock-free queue가 구현이 어려운 부분은 바로 head와 tail로 구분하여 두가지 포인터를 관리해야하는 부분이다. <br/>
특히 원소의 수가 0개 <-> 1개 의 두 상태간 전이는 head와 tail을 둘 다 변화시켜야하기 때문에, <br/>
기존 LinkedList기반의 구현을 생각하면 어렵게 느껴질 수 있다. <br/>

### 그러면 어떤 방식으로 우회할 수 있을까?
head와 tail을 모두 동시에 원자적으로 접근할 수 있는 double-cas를 머신이 지원해주는게 아니라면 다른 방식을 시도해야하는데, <br/>
그것이 바로 ```Dummy Node```이다. <br/>
```Michael-Scott```방식이라고 하는게 바로 이 Dummy Node를 이용해 double-cas가 필요한 지점을 없애는 방법이다. <br/>
항상 큐에는 1개 이상의 노드가 존재하며, 노드가 1개만 존재하는 경우가 바로 큐가 비어있는 상황을 표현하게 되는 방식이다. <br/>
이 방식으로 인해 앞서 말한 노드0개 <-> 노드1개의 상태전이를 배제할 수 있는 것. <br/>

### Push
먼저, 해당 방식의 lock-free queue는 tail을 엄밀하게 표현하지 않는다. <br/>
큐의 구조에 있어서 가장 중요한건 링크를 깨뜨리지 않는 것과, 깨뜨리지 않으면서 노드를 제거하는 것에 있다. <br/>
그렇기 때문에 Push연산에서는 tail이 진짜 tail이 맞는지 확인하고 연결하는 작업만 수행하면 된다. <br/>
만약 tail이 엄밀한 tail이 아니라면, tail을 갱신하고 다시 시도하는 식이다. <br/>

<details>
<summary>tail관리 코드</summary>

```cpp
if (pNext == nullptr) // 엄밀한 마지막 노드 맞아?
{
				if (removeTail->next.compare_exchange_weak(pNext, stampNode)) // nullptr에서 새로운 노드로 바꿔
				{
					m_Tail.compare_exchange_weak(pTail, stampNode); // 성공했으면 tail 갱신해
					return;
				}
}
else
{
				m_Tail.compare_exchange_weak(pTail, pNext); // 엄밀한 tail 아니면 tail 갱신하고 다시 시도해
}
```

</details>

### Pop
head는 계속해서 dummy노드로 생각하고, 그 다음 노드에서 데이터를 가져와 반환한다. <br/>
oldhead : 기존 dummy, newhead : 데이터를 빼낼 노드 & 새로운 dummy <br/>
단, head의 next(newhead)가 nullptr면 빼면 안된다! 노드가 1개뿐이다 -> 원소가 0개이다 이므로 빈 큐가 된다. <br/>
빈 큐에서 데이터를 뺀다는게 말이나 되는가? 그리고 dummy노드가 없어지는 순간 앞서 말한 모든 문제가 다시 생기게 된다. <br/>

### 한계점
일단 다른 사람의 구현은 어떤지 모르겠지만, <br/>
현재 내가 구현한 코드는 데이터의 유실은 없지만, 데이터가 있음에도 nullptr를 반환하는 문제가 있다. <br/>
해당 경우의 발생률은 높지는 않지만, 분명히 존재한다. (10만회 기준 10회 내외의 nullptr반환이 확인되었다.) <br/>
