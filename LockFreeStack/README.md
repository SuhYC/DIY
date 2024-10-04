# Lock-Free Stack
```atomic연산```을 이용해 구현한 ```lock-free stack```이다.

테스트를 위해 ```lock-free stack```을 사용한 것과 동적할당을 통해 그때 그때 노드를 할당하고 해제하는 것을 비교한다. <br/>
비교방법은 다음과 같다. <br/>
1. ```lock-free stack```을 통해 메모리풀에서 노드를 가져온다 (동적할당의 경우는 그냥 노드를 할당한다.)
2. 노드를 가져오는데 성공했다면 ```increment```연산을 100회 수행한다. (보통 객체풀에서 객체를 가져오고 하는 동작이라고 생각하자)
3. ```lock-free stack```에 노드를 반납한다. (동적할당의 경우는 그냥 노드를 해제한다.)

위 과정을 100000회 씩 10스레드에서 실행한다.

![이미지](https://github.com/SuhYC/DIY/blob/main/LockFreeStack/a.png)
