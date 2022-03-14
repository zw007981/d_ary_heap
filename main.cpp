#include <iostream>
#include <time.h>
#include <vector>

#include "src/heap.hpp"

struct MyInt {
    explicit MyInt(int _value)
        : value_(_value)
    {
    }
    bool operator<(const MyInt& another) const
    {
        return value_ < another.value_;
    }
    bool operator>(const MyInt& another) const
    {
        return value_ > another.value_;
    }
    int value_ { -1 };
};

int main(int argc, char const* argv[])
{
    auto nodes1 = std::vector<char> { 'D', 'i', 'j', 'k', 's', 't', 'r', 'a' };
    auto min_k_heap = pri_queue::makeMinKHeap(2, std::move(nodes1));
    std::cout << "Elements stored in min heap: ";
    while (!min_k_heap.empty()) {
        std::cout << min_k_heap.top() << ",";
        min_k_heap.pop();
    }
    std::cout << std::endl;
    auto max_k_heap = pri_queue::makeMaxKHeap<MyInt>(4);
    auto cur_time = time(NULL);
    auto cur_time_ptr = gmtime(&cur_time);
    max_k_heap.push(MyInt(1900 + cur_time_ptr->tm_year));
    max_k_heap.push(MyInt(1 + cur_time_ptr->tm_mon));
    max_k_heap.push(MyInt(cur_time_ptr->tm_mday));
    max_k_heap.push(MyInt(cur_time_ptr->tm_hour));
    max_k_heap.push(MyInt(cur_time_ptr->tm_min));
    max_k_heap.push(MyInt(cur_time_ptr->tm_sec));
    std::cout << "Current time: " << asctime(cur_time_ptr);
    std::cout << "Elements stored in max heap: ";
    while (!max_k_heap.empty()) {
        std::cout << max_k_heap.top().value_ << ",";
        max_k_heap.pop();
    }
    std::cout << std::endl;
    return 0;
}
