#include <algorithm>
#include <benchmark/benchmark.h>
#include <vector>

#include "../src/d_ary_heap.hpp"
#include "../src/priority_queue.hpp"
#include "../test/test_data_generator.h"

using namespace usr_defined_cont;

// 用于测试的容器的种类。
enum class ContType { UsrDefinedHeap,
    UsrDefinedPriQueue,
    STDPriQueue };

// 生成测试数据。
template <typename T, typename TGenFunc>
std::vector<T> genDataForTest(size_t num_data, TGenFunc genFunc, int seed = 19950910)
{
    srand(seed);
    std::vector<T> data_for_test;
    for (size_t i = 0; i < num_data; i++) {
        auto rand_data = genFunc();
        if (std::find(data_for_test.begin(), data_for_test.end(), rand_data)
            != data_for_test.end()) {
            continue;
        }
        data_for_test.push_back(std::move(rand_data));
    }
    num_data = data_for_test.size();
    return data_for_test;
}

// 在容器中进行push和pop操作。
template <typename TCont, typename TElements, typename TPriorities>
void containerPushAndPop(TCont& cont, const TElements& elements, const TPriorities& priorities)
{
    for (size_t i = 0; i < elements.size(); i++) {
        cont.push(elements.at(i), priorities.at(i));
    }
    for (size_t i = 0; i < elements.size() / 3; i++) {
        cont.pop();
    }
}
template <typename TCont, typename TElements>
void containerPushAndPop(TCont& cont, const TElements& elements)
{
    for (size_t i = 0; i < elements.size(); i++) {
        cont.push(elements.at(i));
    }
    for (size_t i = 0; i < elements.size() / 3; i++) {
        cont.pop();
    }
}

template <size_t size, ContType cont_type>
void benchContainerPushAndPop(benchmark::State& state)
{
    auto nodes = genDataForTest<MyNode, decltype(genNodeFunc)>(size, genNodeFunc);
    auto strings = genDataForTest<std::string, decltype(genStrFunc)>(size, genStrFunc);
    auto nodes_priorities = std::vector<decltype(MyNode::f_)>();
    std::for_each(nodes.begin(), nodes.end(), [&](const MyNode& node) { nodes_priorities.push_back(node.f_); });
    for (auto _ : state) {
        if (cont_type == ContType::UsrDefinedHeap) {
            auto min_heap_node = createEmptyMinDHeap<MyNode>(3);
            auto min_heap_str = createEmptyMinDHeap<std::string>(3);
            auto max_heap_node = createEmptyMaxDHeap<MyNode>(3);
            auto max_heap_str = createEmptyMaxDHeap<std::string>(3);
            containerPushAndPop(min_heap_node, nodes);
            containerPushAndPop(min_heap_str, strings);
            containerPushAndPop(max_heap_node, nodes);
            containerPushAndPop(max_heap_str, strings);
        } else if (cont_type == ContType::UsrDefinedPriQueue) {
            auto min_pri_queue_node = createEmptyMinPriQueue<MyNode, int, MyNodeHasher>(3);
            auto min_pri_queue_str = createEmptyMinPriQueue<std::string, std::string>(3);
            auto max_pri_queue_node = createEmptyMaxPriQueue<MyNode, int, MyNodeHasher>(3);
            auto max_pri_queue_str = createEmptyMaxPriQueue<std::string, std::string>(3);
            containerPushAndPop(min_pri_queue_node, nodes, nodes_priorities);
            containerPushAndPop(min_pri_queue_str, strings, strings);
            containerPushAndPop(max_pri_queue_node, nodes, nodes_priorities);
            containerPushAndPop(max_pri_queue_str, strings, strings);
        } else {
            auto std_min_pri_queue_node = createSTDMinPriQueue<MyNode>();
            auto std_min_pri_queue_str = createSTDMinPriQueue<std::string>();
            auto std_max_pri_queue_node = createSTDMaxPriQueue<MyNode>();
            auto std_max_pri_queue_str = createSTDMaxPriQueue<std::string>();
            containerPushAndPop(std_min_pri_queue_node, nodes);
            containerPushAndPop(std_min_pri_queue_str, strings);
            containerPushAndPop(std_max_pri_queue_node, nodes);
            containerPushAndPop(std_max_pri_queue_str, strings);
        }
    }
}

BENCHMARK_TEMPLATE(benchContainerPushAndPop, 66, ContType::UsrDefinedHeap);
BENCHMARK_TEMPLATE(benchContainerPushAndPop, 66, ContType::UsrDefinedPriQueue);
BENCHMARK_TEMPLATE(benchContainerPushAndPop, 66, ContType::STDPriQueue);

BENCHMARK_TEMPLATE(benchContainerPushAndPop, 666, ContType::UsrDefinedHeap);
BENCHMARK_TEMPLATE(benchContainerPushAndPop, 666, ContType::UsrDefinedPriQueue);
BENCHMARK_TEMPLATE(benchContainerPushAndPop, 666, ContType::STDPriQueue);

BENCHMARK_TEMPLATE(benchContainerPushAndPop, 6666, ContType::UsrDefinedHeap);
BENCHMARK_TEMPLATE(benchContainerPushAndPop, 6666, ContType::UsrDefinedPriQueue);
BENCHMARK_TEMPLATE(benchContainerPushAndPop, 6666, ContType::STDPriQueue);

BENCHMARK_MAIN();