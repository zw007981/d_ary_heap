#include <functional>
#include <gtest/gtest.h>
#include <iostream>
#include <random>
#include <string>

#include "../src/d_ary_heap.hpp"
#include "test_data_generator.h"

namespace custom_cont::test_d_ary_heap {
class TestHeapFixture : public ::testing::Test {
    using MinHeap = DAryHeap<std::string>;
    using MaxHeap = DAryHeap<int>;

public:
    std::vector<std::string> values_in_str_;
    std::vector<int> values_in_int_;
    const int expected_values_in_str_size_ { 6 };
    const int expected_values_in_int_size_ { 44 };
    MinHeap min_d_heap_;
    MaxHeap max_d_heap_;

public:
    void SetUp() override
    {
        values_in_str_ = std::vector<std::string> { "Dijkstra", "Bellman-Ford",
            "A-star", "Hybrid A-star", "RRT", "RRT-star" };
        min_d_heap_ = buildMinDHeap<std::string>(10, values_in_str_);
        max_d_heap_ = createEmptyMaxDHeap<int>(2);
        srand((unsigned)time(NULL));
        for (int i = 0; i < expected_values_in_int_size_; i++) {
            int num_to_push = std::rand() % 1000;
            values_in_int_.push_back(num_to_push);
            max_d_heap_.push(num_to_push);
        }
        this->makeStdHeap();
    }
    void TearDown() override
    {
        values_in_str_.clear();
        values_in_int_.clear();
    }
    void makeStdHeap()
    {
        std::make_heap(values_in_str_.begin(), values_in_str_.end(), std::greater<std::string> {});
        std::make_heap(values_in_int_.begin(), values_in_int_.end(), std::less<int> {});
    }
    template <typename T>
    bool isTwoHeapsEqual(std::vector<T> std_heap, DAryHeap<T> k_heap, std::function<bool(T, T)> cmp_func) const
    {
        size_t length = std_heap.size();
        for (size_t i = 0; i < length; i++) {
            if (std_heap.size() != k_heap.size()) {
                return false;
            }
            auto top_node1 = std_heap.at(0);
            auto top_node2 = k_heap.top();
            if (top_node1 != top_node2) {
                return false;
            }
            std::pop_heap(std_heap.begin(), std_heap.end(), cmp_func);
            std_heap.pop_back();
            k_heap.pop();
        }
        return true;
    }
};

TEST_F(TestHeapFixture, testMbrFuncSizeAndEmpty)
{
    auto empty_d_heap = createEmptyMaxDHeap<int>(2);
    EXPECT_EQ(empty_d_heap.size(), 0);
    EXPECT_TRUE(empty_d_heap.empty());
    EXPECT_EQ(min_d_heap_.size(), expected_values_in_str_size_);
    EXPECT_FALSE(min_d_heap_.empty());
    EXPECT_EQ(max_d_heap_.size(), expected_values_in_int_size_);
    EXPECT_FALSE(max_d_heap_.empty());
    for (size_t i = 0; i < 1000; i++) {
        max_d_heap_.push(std::rand());
        EXPECT_EQ(max_d_heap_.size(), expected_values_in_int_size_ + i + 1);
    }
    for (size_t i = 0; i < 1000 + expected_values_in_int_size_; i++) {
        max_d_heap_.pop();
        EXPECT_EQ(max_d_heap_.size(), expected_values_in_int_size_ + 1000 - (i + 1));
    }
    EXPECT_TRUE(max_d_heap_.empty());
}

TEST_F(TestHeapFixture, testMbrFuncTopAndPop)
{
    for (size_t i = 0; i < expected_values_in_str_size_; i++) {
        auto top_node1 = values_in_str_.at(0);
        auto top_node2 = min_d_heap_.top();
        EXPECT_EQ(top_node1, top_node2);
        std::pop_heap(values_in_str_.begin(), values_in_str_.end(), std::greater<std::string> {});
        values_in_str_.pop_back();
        min_d_heap_.pop();
    }
    for (size_t i = 0; i < expected_values_in_int_size_; i++) {
        auto top_node1 = values_in_int_.at(0);
        auto top_node2 = max_d_heap_.popAndReturn();
        EXPECT_EQ(top_node1, top_node2);
        std::pop_heap(values_in_int_.begin(), values_in_int_.end(), std::less<int> {});
        values_in_int_.pop_back();
    }
}

TEST_F(TestHeapFixture, testMbrFuncPush)
{
    for (size_t i = 0; i < 100; i++) {
        auto str_to_push = genStrFunc();
        min_d_heap_.push(str_to_push);
        values_in_str_.push_back(str_to_push);
        std::make_heap(values_in_str_.begin(), values_in_str_.end(), std::greater<std::string> {});
        EXPECT_TRUE(this->isTwoHeapsEqual<std::string>(values_in_str_, min_d_heap_,
            std::greater<std::string> {}));
    }
    for (size_t i = 0; i < 100; i++) {
        int num_to_push = std::rand();
        max_d_heap_.push(num_to_push);
        values_in_int_.push_back(num_to_push);
        std::make_heap(values_in_int_.begin(), values_in_int_.end(), std::less<int> {});
        EXPECT_TRUE(this->isTwoHeapsEqual<int>(values_in_int_, max_d_heap_,
            std::less<int> {}));
    }
}
}
