# C++ IOCP서버와 C# Client간의 통신에 직렬화 사용해보기
기존 [C++ 직렬화 라이브러리 만들기](https://github.com/SuhYC/DIY/tree/main/SerializeLib/Cpp)와 [C# 직렬화 라이브러리 만들기](https://github.com/SuhYC/DIY/tree/main/SerializeLib/CS)의 코드로 실제 통신에 사용해봤다. <br/>
에코서버와 크게 다른건 없다. 단, 수신 -> 역직렬화 -> 객체확인 -> 직렬화 -> 직렬화된 데이터로 전송 (당연히 절대 수신한 데이터를 바로 쏘지 않는다!) <br/>
쏠 데이터는 양쪽 모두 순서를 맞추어 <br/>
```int16_t (short)``` -> ```int32_t (int)``` -> ```int64_t (long)```<br/>
-> ```uint16_t (ushort)``` -> ```uint32_t (uint)``` -> ```uint64_t (ulong)```<br/>
-> ```float``` -> ```double``` -> ```std::string (string)``` 이다. <br/>
단, std::string (string)의 경우는 4바이트 헤더가 별도로 포함되며 euc-kr로 인코딩을 맞추어 C#의 string은 byte[]로 변환한 후 직렬화한다. <br/>


## 결과
클라이언트 : <br/>
![클라 이미지](https://github.com/SuhYC/DIY/blob/main/SerializeLib/CppToCS/client.png) <br/>
서버 : <br/>
![서버 이미지](https://github.com/SuhYC/DIY/blob/main/SerializeLib/CppToCS/server.png) <br/>

아주 잘 된다. <br/>

전송할 데이터는 상기한 데이터를 한번에 보낼 시 총 40바이트 + 문자열이 차지하는 부분이 된다. <br/>
문자열은 기본 4바이트 + 보낼문자열의 길이가 되므로 <br/>
```hello0``` ~ ```hello9```는 문자열이 차지하는 크기가 총 10바이트, ```hello10```~```hello99```의 경우는 문자열이 차지하는 크기가 총 11바이트로 <br/>
이번 코드에서 사용한 한 페이로드의 크기는 50 ~ 51바이트가 된다. <br/>

여기에 패킷마다의 고정헤더 4바이트를 적용하면 54 ~ 55바이트가 되는 셈.

## 주의사항
```SerializeLib``` 클래스에서 Push 연산이 들어왔을 때 버퍼 크기가 모자란 경우에 재할당하는 부분을 구현하지 않았다. <br/>
적절한 크기를 설정하고 사용할 것. <br/>
디버깅 첫 오류는 ```SerializeLib``` 크기를 너무 작게 설정해서 발생했고, <br/>
두번째 오류는 GC를 고려하다보니 ```DeserializeLib```을 한번만 할당하고 재사용하려다 멤버변수```idx```를 초기화하는 걸 까먹어서 발생했다.
