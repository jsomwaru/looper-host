#include <iostream>

template<typename T, typename... Args>
void print(const T data, Args... args) {
    std::cout << data << ' ' << std::endl;
    print(args...)
}
