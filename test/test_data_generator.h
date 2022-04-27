#include <functional>
#include <queue>
#include <random>
#include <string>

namespace usr_defined_cont {
struct MyNode {
    explicit MyNode(int node_id, int g, int h)
        : node_id_(node_id)
        , g_(g)
        , h_(h)
    {
        f_ = g_ + h_;
    }
    int node_id_ { -1 };
    int f_ { 0 };
    int g_ { 0 };
    int h_ { 0 };
    bool operator>(const MyNode& another) const
    {
        return f_ > another.f_;
    }
    bool operator<(const MyNode& another) const
    {
        return f_ < another.f_;
    }
    bool operator==(const MyNode& another) const
    {
        return node_id_ == another.node_id_;
    }
};
struct MyNodeHasher {
    size_t operator()(const MyNode& my_node) const
    {
        return std::hash<int>()(my_node.node_id_);
    }
};

auto genNodeFunc = []() {
    int node_id = std::rand() % 10000;
    int g = 0.1 * (std::rand() % 10000), h = 0.1 * (std::rand() % 10000);
    return MyNode(node_id, g, h);
};

auto genStrFunc = []() {
    int length = std::rand() % 30;
    std::string rand_str = "";
    if (length <= 0) {
        return rand_str;
    }
    char tmp;
    for (int i = 0; i < length; i++) {
        tmp = std::rand() % 62;
        if (tmp < 10) {
            tmp += '0';
        } else if (tmp < 36) {
            tmp -= 10;
            tmp += 'A';
        } else {
            tmp -= 36;
            tmp += 'a';
        }
        rand_str += tmp;
    }
    return rand_str;
};

template <typename T>
auto createSTDMinPriQueue = []() {
    auto cmpFunc = [](const T& element1, const T& element2) {
        return element1 > element2;
    };
    return std::priority_queue<T, std::vector<T>, decltype(cmpFunc)>(cmpFunc);
};
template <typename T>
auto createSTDMaxPriQueue = []() {
    auto cmpFunc = [](const T& element1, const T& element2) {
        return element1 < element2;
    };
    return std::priority_queue<T, std::vector<T>, decltype(cmpFunc)>(cmpFunc);
};
}