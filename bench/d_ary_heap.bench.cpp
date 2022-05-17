#include <algorithm>
#include <benchmark/benchmark.h>
#include <tuple>
#include <vector>

#include "../src/d_ary_heap.hpp"
#include "../src/priority_queue.hpp"
#include "../test/test_data_generator.h"

using namespace custom_cont;

// 用于测试的容器的种类。
enum class Container { CustomHeap,
    CustomPriQueue,
    STDPriQueue };

// 适用于bench d ary heap的fixture，包含测试数据集和一些工具函数。
class BenchDAryHeapFixture {
public:
    // 输入需要的数据集的大小dataset size来初始化fixture。
    explicit BenchDAryHeapFixture(size_t dataset_size)
    {
        strings_ = BenchDAryHeapFixture::genDatasetForTest<std::string, decltype(genStrFunc)>(dataset_size, genStrFunc);
        nodes_ = BenchDAryHeapFixture::genDatasetForTest<MyNode, decltype(genNodeFunc)>(dataset_size, genNodeFunc);
        std::for_each(nodes_.begin(), nodes_.end(), [&](const MyNode& node) { nodes_priorities_.push_back(node.f_); });
    }
    // 返回包含std::string的测试数据集。
    const auto& getStringsForTest() const { return strings_; }
    // 返回包含MyNode的测试数据集。
    const auto& getNodesForTest() const { return nodes_; }
    // 返回包含MyNode优先级的测试数据集。
    const auto& getPrioritiesForTest() const { return nodes_priorities_; }
    // 构造空的std::priority_queue用于测试。
    static auto createSTDPriQueue()
    {
        return std::make_tuple(createSTDMinPriQueue<std::string>(), createSTDMaxPriQueue<std::string>(),
            createSTDMinPriQueue<MyNode>(), createSTDMaxPriQueue<MyNode>());
    }
    // 构造空的D ary heap用于测试。
    static auto createHeap()
    {
        return std::make_tuple(createEmptyMinDHeap<std::string>(), createEmptyMaxDHeap<std::string>(),
            createEmptyMinDHeap<MyNode>(), createEmptyMaxDHeap<MyNode>());
    }
    // 构造空的updatable priority queue用于测试。
    static auto createPriorityQueue()
    {
        return std::make_tuple(createEmptyMinPriQueue<std::string, std::string>(),
            createEmptyMaxPriQueue<std::string, std::string>(),
            createEmptyMinPriQueue<MyNode, int, MyNodeHasher>(),
            createEmptyMaxPriQueue<MyNode, int, MyNodeHasher>());
    }
    // 对容器进行count次pop操作。
    template <typename TCont>
    static void containerPop(TCont& container, size_t count)
    {
        for (size_t i = 0; i < count; i++) {
            container.pop();
        }
    }
    // 对容器进行count次push操作。
    template <typename TCont, typename TElements>
    static void containerPush(TCont& container, const TElements& elements, size_t count)
    {
        for (size_t i = 0; i < count; i++) {
            container.push(elements.at(i));
        }
    }
    // 对updatable priority queue进行count次push操作。
    template <typename TCont, typename TElements, typename TPriorities>
    static void containerPush(TCont& container, const TElements& elements, const TPriorities& priorities, size_t count)
    {
        for (size_t i = 0; i < count; i++) {
            container.push(elements.at(i), priorities.at(i));
        }
    }

private:
    // 使用生成函数genFunc生成包含num data个类型为T的数据的数据集。
    template <typename T, typename TGenFunc>
    static std::vector<T> genDatasetForTest(size_t num_data, TGenFunc genFunc, int seed = 1995)
    {
        srand(seed);
        std::vector<T> dataset;
        for (size_t i = 0; i < num_data; i++) {
            auto rand_data = genFunc();
            while (std::find(dataset.begin(), dataset.end(), rand_data) != dataset.end()) {
                rand_data = genFunc();
            }
            dataset.push_back(std::move(rand_data));
        }
        return dataset;
    }

private:
    std::vector<std::string> strings_;
    std::vector<MyNode> nodes_;
    std::vector<decltype(MyNode::f_)> nodes_priorities_;
};

auto fixture = BenchDAryHeapFixture(10000);

// 对种类为cont type的容器执行count次push操作。
template <Container cont_type, size_t count>
void benchPush(benchmark::State& state)
{
    const auto& strings = fixture.getStringsForTest();
    const auto& nodes = fixture.getNodesForTest();
    const auto& priorities = fixture.getPrioritiesForTest();
    for (auto _ : state) {
        if (cont_type == Container::CustomHeap) {
            auto [min_heap_str, max_heap_str, min_heap_node, max_heap_node] = fixture.createHeap();
            BenchDAryHeapFixture::containerPush(min_heap_str, strings, count);
            BenchDAryHeapFixture::containerPush(max_heap_str, strings, count);
            BenchDAryHeapFixture::containerPush(min_heap_node, nodes, count);
            BenchDAryHeapFixture::containerPush(max_heap_node, nodes, count);
        } else if (cont_type == Container::CustomPriQueue) {
            auto [min_pri_queue_str, max_pri_queue_str, min_pri_queue_node, max_pri_queue_node] = fixture.createPriorityQueue();
            BenchDAryHeapFixture::containerPush(min_pri_queue_str, strings, strings, count);
            BenchDAryHeapFixture::containerPush(max_pri_queue_str, strings, strings, count);
            BenchDAryHeapFixture::containerPush(min_pri_queue_node, nodes, priorities, count);
            BenchDAryHeapFixture::containerPush(max_pri_queue_node, nodes, priorities, count);
        } else {
            auto [min_std_pri_queue_str, max_std_pri_queue_str, min_std_pri_queue_node, max_std_pri_queue_node] = fixture.createSTDPriQueue();
            BenchDAryHeapFixture::containerPush(min_std_pri_queue_str, strings, count);
            BenchDAryHeapFixture::containerPush(max_std_pri_queue_str, strings, count);
            BenchDAryHeapFixture::containerPush(min_std_pri_queue_node, nodes, count);
            BenchDAryHeapFixture::containerPush(max_std_pri_queue_node, nodes, count);
        }
    }
}

// 对种类为cont type的容器执行count次push和pop操作。
template <Container cont_type, size_t count>
void benchPushThenPop(benchmark::State& state)
{
    const auto& strings = fixture.getStringsForTest();
    const auto& nodes = fixture.getNodesForTest();
    const auto& priorities = fixture.getPrioritiesForTest();
    for (auto _ : state) {
        if (cont_type == Container::CustomHeap) {
            auto [min_heap_str, max_heap_str, min_heap_node, max_heap_node] = fixture.createHeap();
            BenchDAryHeapFixture::containerPush(min_heap_str, strings, count);
            BenchDAryHeapFixture::containerPush(max_heap_str, strings, count);
            BenchDAryHeapFixture::containerPush(min_heap_node, nodes, count);
            BenchDAryHeapFixture::containerPush(max_heap_node, nodes, count);
            BenchDAryHeapFixture::containerPop(min_heap_str, count);
            BenchDAryHeapFixture::containerPop(max_heap_str, count);
            BenchDAryHeapFixture::containerPop(min_heap_node, count);
            BenchDAryHeapFixture::containerPop(max_heap_node, count);

        } else if (cont_type == Container::CustomPriQueue) {
            auto [min_pri_queue_str, max_pri_queue_str, min_pri_queue_node, max_pri_queue_node] = fixture.createPriorityQueue();
            BenchDAryHeapFixture::containerPush(min_pri_queue_str, strings, strings, count);
            BenchDAryHeapFixture::containerPush(max_pri_queue_str, strings, strings, count);
            BenchDAryHeapFixture::containerPush(min_pri_queue_node, nodes, priorities, count);
            BenchDAryHeapFixture::containerPush(max_pri_queue_node, nodes, priorities, count);
            BenchDAryHeapFixture::containerPop(min_pri_queue_str, count);
            BenchDAryHeapFixture::containerPop(max_pri_queue_str, count);
            BenchDAryHeapFixture::containerPop(min_pri_queue_node, count);
            BenchDAryHeapFixture::containerPop(max_pri_queue_node, count);
        } else {
            auto [min_std_pri_queue_str, max_std_pri_queue_str, min_std_pri_queue_node, max_std_pri_queue_node] = fixture.createSTDPriQueue();
            BenchDAryHeapFixture::containerPush(min_std_pri_queue_str, strings, count);
            BenchDAryHeapFixture::containerPush(max_std_pri_queue_str, strings, count);
            BenchDAryHeapFixture::containerPush(min_std_pri_queue_node, nodes, count);
            BenchDAryHeapFixture::containerPush(max_std_pri_queue_node, nodes, count);
            BenchDAryHeapFixture::containerPop(min_std_pri_queue_str, count);
            BenchDAryHeapFixture::containerPop(max_std_pri_queue_str, count);
            BenchDAryHeapFixture::containerPop(min_std_pri_queue_node, count);
            BenchDAryHeapFixture::containerPop(max_std_pri_queue_node, count);
        }
    }
}

int main(int argc, char** argv)
{
    benchmark::SetDefaultTimeUnit(benchmark::TimeUnit::kMillisecond);
    // ----------------------------------------------------------------------------
    // std::priority_queue
    BENCHMARK_TEMPLATE(benchPush, Container::STDPriQueue, 1000);
    BENCHMARK_TEMPLATE(benchPushThenPop, Container::STDPriQueue, 1000);
    BENCHMARK_TEMPLATE(benchPush, Container::STDPriQueue, 3000);
    BENCHMARK_TEMPLATE(benchPushThenPop, Container::STDPriQueue, 3000);
    BENCHMARK_TEMPLATE(benchPush, Container::STDPriQueue, 5000);
    BENCHMARK_TEMPLATE(benchPushThenPop, Container::STDPriQueue, 5000);
    BENCHMARK_TEMPLATE(benchPush, Container::STDPriQueue, 7000);
    BENCHMARK_TEMPLATE(benchPushThenPop, Container::STDPriQueue, 7000);
    // ----------------------------------------------------------------------------
    // d_ary_heap
    BENCHMARK_TEMPLATE(benchPush, Container::CustomHeap, 1000);
    BENCHMARK_TEMPLATE(benchPushThenPop, Container::CustomHeap, 1000);
    BENCHMARK_TEMPLATE(benchPush, Container::CustomHeap, 3000);
    BENCHMARK_TEMPLATE(benchPushThenPop, Container::CustomHeap, 3000);
    BENCHMARK_TEMPLATE(benchPush, Container::CustomHeap, 5000);
    BENCHMARK_TEMPLATE(benchPushThenPop, Container::CustomHeap, 5000);
    BENCHMARK_TEMPLATE(benchPush, Container::CustomHeap, 7000);
    BENCHMARK_TEMPLATE(benchPushThenPop, Container::CustomHeap, 7000);
    // ----------------------------------------------------------------------------
    // priority_queue
    BENCHMARK_TEMPLATE(benchPush, Container::CustomPriQueue, 1000);
    BENCHMARK_TEMPLATE(benchPushThenPop, Container::CustomPriQueue, 1000);
    BENCHMARK_TEMPLATE(benchPush, Container::CustomPriQueue, 3000);
    BENCHMARK_TEMPLATE(benchPushThenPop, Container::CustomPriQueue, 3000);
    BENCHMARK_TEMPLATE(benchPush, Container::CustomPriQueue, 5000);
    BENCHMARK_TEMPLATE(benchPushThenPop, Container::CustomPriQueue, 5000);
    BENCHMARK_TEMPLATE(benchPush, Container::CustomPriQueue, 7000);
    BENCHMARK_TEMPLATE(benchPushThenPop, Container::CustomPriQueue, 7000);
    // ----------------------------------------------------------------------------
    benchmark::Initialize(&argc, argv);
    if (benchmark::ReportUnrecognizedArguments(argc, argv)) {
        return 1;
    }
    benchmark::RunSpecifiedBenchmarks();
    benchmark::Shutdown();
    return 0;
}