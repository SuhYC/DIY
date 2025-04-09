# C++ IOCP서버와 C# Client간의 통신에 직렬화 사용해보기
기존 [C++ 직렬화 라이브러리 만들기](https://github.com/SuhYC/DIY/tree/main/SerializeLib/Cpp)와 [C# 직렬화 라이브러리 만들기](https://github.com/SuhYC/DIY/tree/main/SerializeLib/CS)의 코드로 실제 통신에 사용해봤다. <br/>
에코서버와 크게 다른건 없다. 단, 수신 -> 역직렬화 -> 객체확인 -> 직렬화 -> 직렬화된 데이터로 전송 (당연히 절대 수신한 데이터를 바로 쏘지 않는다!) <br/>
쏠 데이터는 양쪽 모두 순서를 맞추어 <br/>
```int16_t (short)``` -> ```int32_t (int)``` -> ```int64_t (long)```<br/>
-> ```uint16_t (ushort)``` -> ```uint32_t (uint)``` -> ```uint64_t (ulong)```<br/>
-> ```float``` -> ```double``` -> ```std::string (string)``` 이다. <br/>
단, std::string (string)의 경우는 4바이트 헤더가 별도로 포함되며 euc-kr로 인코딩을 맞추어 C#의 string은 byte[]로 변환한 후 직렬화한다. <br/>
