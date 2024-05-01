# std::vector
## C++ STL

일단 함수목록을 보면서 최대한 함수를 구현해보려고 노력. <br/>
emplace_back같은 경우는 좀 더 생각을 해봐야할 것 같다. <br/>

stl에 있는 vector의 경우는 pop_back이나 clear같은 경우 해당 부분의 객체에 대해 destructor가 호출이 되는데 <br/>
일단 데이터를 무시하는 원리로 만들었다. <br/>
속도면에서 이득일 수도 있겠는데 RAII 관련 데이터를 넣어뒀을 때 문제가 생길 수도 있을 것 같기도..?
