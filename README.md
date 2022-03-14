<center> <font size=8> Priority Queue </font></center>

# Heap

The `heap` namespace contains the abstract class heap and the concrete implementation `KHeap` (K-ary heaps), in which the nodes have at most `K` children.

`KHeap` are either Min Heaps or Max Heaps depending on comparator used to initialize the heap, much like what happens in the STL. To get a Min Heap the given comparator should be `std::less<>{}` , whereas to get a Max Heap, the given comparator should be `std::greater<>{}` .

A number of utility factory functions are provided to easily create the type of Heap you want, namely:

1. `make_min_k_heap<size_t K, typename T>`: create a Min K-ary Heap starting from a vector of keys and a vector of values.
2. `make_max_k_heap<size_t K, typename T>`: create a Max K-ary Heap starting from a vector of keys and a vector of values.

Each of these functions supports both copy and move semantics for the given input. The example usage of `KHeap` can be found below:

```c++
#include <iostream>
#include <vector>

#include "src/KHeap.h"

int main() {

    std::vector<char> vec1 {'b', 'c', 'f', 'a', 'e', 'd'};
    constexpr size_t K1 = 4;
    auto min_heap = heap::make_min_k_heap<K1>(std::move(vec1));
    // Output: a, b, c, d, e, f.
    while (!min_heap.empty()) {
        std::cout << min_heap.top() << std::endl;
        min_heap.pop();
    }

    auto max_heap = heap::make_max_k_heap<3, size_t>();
    max_heap.push(30);
    max_heap.push(15);
    max_heap.push(5000);
    max_heap.push(50);
    max_heap.push(599);
    // Output: 5000, 599, 50, 30, 15.
    while (!max_heap.empty()) {
        std::cout << max_heap.top() << std::endl;
        max_heap.pop();
    }
    return 0;

}

```

# Priority Queue

The `priority-queue` namespace contains the concrete class `PriorityQueue` . The main methods exposed by `PriorityQueue` are:

1. `size()`: return the number of elements in the heap with time complexity O(1).
2. `empty()`: return true iff the heap is empty with time complexity O(1).
3. `push(const Key& key, const T& element)`: add a new element to the heap and associates the given key to it, time complexity O(k*log_k(N)).
4. `update_key(const Key& key, const T& element)`: update the key of an existing element in the priority queue, time complexity O(k*log_k(N)).
5. `contains(const T& element)`: return true iff the given element is in the priority queue with time complexity O(1).
6. `top()`: return the top element with time complexity O(1).
7. `pop()`: remove the top element with time complexity O(k*log_k(N)).

**Node**: in order to keep `update_key` 's complexity logarithmic instead of linear, there's quite important caveat: arbitrary key updates are not allowed. More specifically if a Priority Queue based on a Max-Heap is used, only increase key operation can be performed; if a Priority Queue based on a Min-Heap is used, only decrease key operation can be performed. Otherwise arbitrary key updates may result in undefined behavior.

A number of utility factory functions are provided to easily create the type of Priority Queue you want, namely:

1. `make_min_priority_queue<size_t K, typename Key, typename Value, typename THash = std::hash<Value>`: create a Priority Queue based on a Min K-ary Heap starting from a vector of values.
2. `make_max_priority_queue<size_t K, typename Key, typename Value, typename THash = std::hash<Value>`: create a Priority Queue based on a Max K-ary Heap starting from a vector of values.

Each of these functions supports both copy and move semantics for the given inputs.

**Note**: `PriorityQueue` is implemented using `unordered_map` for fast retrieval of keys and element index given an element sorted in the underlying `Heap` . This means that your values's type must have an hash implementation. If you use trivial types (int, double, etc) you're already covered otherwise you will have to provide your own custom hash functor as the last template argument of the utility factory functions defined above.

```c++
#include <iostream>
#include <vector>
#include "src/PriorityQueue.h"

int main() {
    auto keys1 = std::vector<size_t> {5, 4, 1, 3, 6, 0, 2};
    auto values1 = std::vector<char> {'m', 'i', 'n', 'h', 'e', 'a', 'p'};
    constexpr size_t K1 = 4;
    auto min_pri_queue1 = priority_queue::make_min_k_priority_queue<K1>(keys1, values1);
    //Output: {0,a}, {1,n}, {2,p}, {3,h}, {4,i}, {5,m}, {6,e}.
    while (!min_pri_queue1.empty()) {
        const auto& [top_key, top_value] = min_pri_queue1.top();
        std::cout << "{" << top_key << "," << top_value << "}" << std::endl;
    }

    auto keys2 = std::vector<size_t> {0, 2, 4, 6, 8, 10, 12};
    auto values2 = std::vector<char> {'m', 'i', 'n', 'h', 'e', 'a', 'p'};
    constexpr size_t K2 = 3;
    auto min_pri_queue2 = priority_queue::make_min_k_priority_queue<K2>(keys1, values1);
    // Min heap: keys can only decrease.
    min_pri_queue2.update_key(5, 'e');
    min_pri_queue2.update_key(1, 'p');
    //Output: {0,m}, {1,p}, {2,i}, {4,n}, {5,e}, {6,h}, {10,a}.
    while (!min_pri_queue2.empty()) {
        const auto& [top_key, top_value] = min_pri_queue2.top();
        std::cout << "{" << top_key << "," << top_value << "}" << std::endl;
    }
    return 0;
}
```
