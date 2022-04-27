#include <functional>
#include <gtest/gtest.h>
#include <iostream>
#include <queue>
#include <random>
#include <string>

#include "../src/priority_queue.hpp"
#include "test_data_generator.h"

namespace usr_defined_cont {
namespace test_priority_queue {
    constexpr int RAND_SEED = 19950910;

    class TestPriQueueFixture : public ::testing::Test {
        using MinPriQueue = PriQueue<MyNode, int, MyNodeHasher>;
        using MaxPriQueue = PriQueue<std::string, std::string>;

    public:
        void SetUp() override
        {
            // 生成测试数据。
            my_nodes_ = this->genDataForTest<MyNode, decltype(genNodeFunc)>(num_nodes_, genNodeFunc);
            my_strings_ = this->genDataForTest<std::string, decltype(genStrFunc)>(num_strings_, genStrFunc);
            this->buildPriQueue();
        }

        // 使用genFunc生成num_data个类型为T的数据并返回。
        template <typename T, typename TGenFunc>
        std::vector<T> genDataForTest(int& num_data, TGenFunc genFunc, int seed = RAND_SEED)
        {
            srand(seed);
            std::vector<T> data_for_test;
            for (int i = 0; i < num_data; i++) {
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

        // 使用随机生成的数据构建自定义的优先队列。
        void buildPriQueue()
        {
            min_pri_queue_ = createEmptyMinPriQueue<MyNode, int, MyNodeHasher>(3);
            max_pri_queue_ = createEmptyMaxPriQueue<std::string, std::string>(2);
            for (const auto& node : my_nodes_) {
                min_pri_queue_.push(node, node.f_);
            }
            for (const auto& str : my_strings_) {
                max_pri_queue_.push(str, str);
            }
        }

        // 使用随机生成的数据构建std中的优先队列。
        auto buildSTDPriQueue()
        {
            auto std_min_pri_queue = createSTDMinPriQueue<MyNode>();
            auto std_max_pri_queue = createSTDMaxPriQueue<std::string>();
            for (const auto& node : my_nodes_) {
                std_min_pri_queue.push(node);
            }
            for (const auto& str : my_strings_) {
                std_max_pri_queue.push(str);
            }
            return std::make_pair(std_min_pri_queue, std_max_pri_queue);
        }

        MinPriQueue min_pri_queue_;
        MaxPriQueue max_pri_queue_;
        std::vector<MyNode> my_nodes_;
        std::vector<std::string> my_strings_;
        int num_nodes_ { 66 }, num_strings_ { 88 };
    };

    TEST_F(TestPriQueueFixture, testContentsEqual)
    {
        auto [std_min_pri_queue, std_max_pri_queue] = this->buildSTDPriQueue();
        for (size_t i = 0; i < num_nodes_; i++) {
            const auto& node = min_pri_queue_.top();
            const auto& expected_node = std_min_pri_queue.top();
            EXPECT_TRUE(node == expected_node);
            min_pri_queue_.pop();
            std_min_pri_queue.pop();
        }
        EXPECT_EQ(min_pri_queue_.empty(), std_min_pri_queue.empty());
        EXPECT_THROW(min_pri_queue_.pop(), std::out_of_range);
        for (size_t i = 0; i < num_strings_; i++) {
            const auto& str = max_pri_queue_.top();
            const auto& expected_str = std_max_pri_queue.top();
            EXPECT_TRUE(str == expected_str);
            max_pri_queue_.pop();
            std_max_pri_queue.pop();
        }
        EXPECT_EQ(max_pri_queue_.empty(), std_max_pri_queue.empty());
        EXPECT_THROW(max_pri_queue_.pop(), std::out_of_range);
    }

    TEST_F(TestPriQueueFixture, testGetNodeAndPriSimultaneously)
    {
        auto [std_min_pri_queue, std_max_pri_queue] = this->buildSTDPriQueue();
        for (size_t i = 0; i < num_nodes_; i++) {
            const auto& expected_node = std_min_pri_queue.top();
            const auto& [node1, key1] = min_pri_queue_.topNode();
            EXPECT_TRUE(node1 == expected_node);
            EXPECT_EQ(key1, expected_node.f_);
            auto [node2, key2] = min_pri_queue_.popAndReturn();
            EXPECT_TRUE(node2 == expected_node);
            EXPECT_EQ(key2, expected_node.f_);
            std_min_pri_queue.pop();
        }
        EXPECT_THROW(min_pri_queue_.popAndReturn(), std::out_of_range);
        for (size_t i = 0; i < num_strings_; i++) {
            const auto& expected_str = std_max_pri_queue.top();
            const auto& [str1, key1] = max_pri_queue_.topNode();
            EXPECT_TRUE(str1 == expected_str);
            EXPECT_TRUE(key1 == expected_str);
            auto [str2, key2] = max_pri_queue_.popAndReturn();
            EXPECT_TRUE(str2 == expected_str);
            EXPECT_TRUE(key2 == expected_str);
            std_max_pri_queue.pop();
        }
        EXPECT_THROW(max_pri_queue_.popAndReturn(), std::out_of_range);
    }

    TEST_F(TestPriQueueFixture, testContains)
    {
        auto empty_min_pri_queue = createEmptyMinPriQueue<MyNode, int, MyNodeHasher>(2);
        auto empty_max_pri_queue = createEmptyMaxPriQueue<std::string, std::string>(3);
        for (const auto& node : my_nodes_) {
            EXPECT_TRUE(min_pri_queue_.contains(node));
            EXPECT_FALSE(empty_min_pri_queue.contains(node));
        }
        for (const auto& str : my_strings_) {
            EXPECT_TRUE(max_pri_queue_.contains(str));
            EXPECT_FALSE(empty_max_pri_queue.contains(str));
        }
    }

    TEST_F(TestPriQueueFixture, testChangeSizeOfQueue)
    {
        int num_new_nodes { 123 }, num_new_str { 233 };
        auto datasets_of_node = this->genDataForTest<MyNode, decltype(genNodeFunc)>(num_new_nodes,
            genNodeFunc, 1);
        auto datasets_of_str = this->genDataForTest<std::string, decltype(genStrFunc)>(num_new_str,
            genStrFunc, 2);
        EXPECT_EQ(min_pri_queue_.size(), num_nodes_);
        EXPECT_FALSE(min_pri_queue_.empty());
        auto expected_size_of_min_pri_queue = min_pri_queue_.size();
        for (const auto& new_node : datasets_of_node) {
            if (std::find(my_nodes_.begin(), my_nodes_.end(), new_node) == my_nodes_.end()) {
                EXPECT_FALSE(min_pri_queue_.contains(new_node));
                expected_size_of_min_pri_queue += 1;
            }
            try {
                min_pri_queue_.push(new_node, new_node.f_);
            } catch (const std::exception& e) {
            }
            EXPECT_EQ(min_pri_queue_.size(), expected_size_of_min_pri_queue);
        }
        while (!min_pri_queue_.empty()) {
            min_pri_queue_.pop();
            expected_size_of_min_pri_queue -= 1;
            EXPECT_EQ(min_pri_queue_.size(), expected_size_of_min_pri_queue);
        }
        EXPECT_TRUE(min_pri_queue_.empty());
        auto empty_max_pri_queue = createEmptyMaxPriQueue<std::string, std::string>(10);
        auto expected_size_of_max_pri_queue = 0;
        EXPECT_TRUE(empty_max_pri_queue.empty());
        for (const auto& new_str : datasets_of_str) {
            EXPECT_FALSE(empty_max_pri_queue.contains(new_str));
            empty_max_pri_queue.push(new_str, new_str);
            expected_size_of_max_pri_queue += 1;
            EXPECT_TRUE(empty_max_pri_queue.contains(new_str));
            EXPECT_EQ(empty_max_pri_queue.size(), expected_size_of_max_pri_queue);
        }
        while (!empty_max_pri_queue.empty()) {
            empty_max_pri_queue.pop();
            expected_size_of_max_pri_queue -= 1;
            EXPECT_EQ(empty_max_pri_queue.size(), expected_size_of_max_pri_queue);
        }
        EXPECT_TRUE(empty_max_pri_queue.empty());
    }

    TEST_F(TestPriQueueFixture, testGetPri)
    {
        for (const auto& node : my_nodes_) {
            EXPECT_EQ(node.f_, min_pri_queue_.getPriority(node));
        }
        auto new_node = MyNode(88888888, 0.1, 0.2);
        EXPECT_THROW(min_pri_queue_.getPriority(new_node), std::out_of_range);
        for (const auto& str : my_strings_) {
            EXPECT_TRUE(str == max_pri_queue_.getPriority(str));
        }
        auto new_str = std::string("!233");
        EXPECT_THROW(max_pri_queue_.getPriority(new_str), std::out_of_range);
    }

    TEST_F(TestPriQueueFixture, testUpdatePri)
    {
        for (size_t i = 0; i < num_nodes_; i++) {
            if (std::rand() % 10 <= 4) {
                auto prev_node = MyNode(my_nodes_[i]);
                my_nodes_[i].f_ -= std::rand() % 100 + 1;
                min_pri_queue_.updatePriority(prev_node, my_nodes_[i].f_);
            }
        }
        for (size_t i = 0; i < num_strings_; i++) {
            if (std::rand() % 10 <= 4) {
                auto prev_str = std::string(my_strings_[i]);
                my_strings_[i] += "233";
                max_pri_queue_.updatePriority(prev_str, my_strings_[i]);
            }
        }
        auto [std_min_pri_queue, std_max_pri_queue] = this->buildSTDPriQueue();
        for (size_t i = 0; i < num_nodes_; i++) {
            const auto& expected_node = std_min_pri_queue.top();
            const auto& [node, pri] = min_pri_queue_.topNode();
            EXPECT_TRUE(expected_node == node);
            EXPECT_EQ(expected_node.f_, pri);
            EXPECT_GE(node.g_ + node.h_, node.f_);
            std_min_pri_queue.pop();
            min_pri_queue_.pop();
        }
        EXPECT_THROW(min_pri_queue_.top(), std::out_of_range);
        for (size_t i = 0; i < num_strings_; i++) {
            const auto& expected_str = std_max_pri_queue.top();
            auto [str, pri] = max_pri_queue_.topNode();
            EXPECT_TRUE(expected_str == pri);
            EXPECT_TRUE(str <= pri);
            std_max_pri_queue.pop();
            max_pri_queue_.pop();
        }
        EXPECT_THROW(max_pri_queue_.topNode(), std::out_of_range);
    }

    TEST_F(TestPriQueueFixture, testUpdatePriForNodeNotExist)
    {
        auto new_node = MyNode(88888888, 1, 2);
        EXPECT_THROW(min_pri_queue_.updatePriority(new_node, new_node.f_), std::out_of_range);
        min_pri_queue_.push(new_node, new_node.f_);
        EXPECT_NO_THROW(min_pri_queue_.updatePriority(new_node, new_node.f_ - 1));
        auto new_str = std::string("!233");
        EXPECT_THROW(max_pri_queue_.updatePriority(new_str, new_str), std::out_of_range);
        max_pri_queue_.push(new_str, new_str);
        EXPECT_NO_THROW(max_pri_queue_.updatePriority(new_str, new_str + "233"));
    }

    TEST_F(TestPriQueueFixture, testUpdatePriArbitrarily)
    {
        auto existing_node = *my_nodes_.begin();
        EXPECT_THROW(min_pri_queue_.updatePriority(existing_node, existing_node.f_ + 10), std::logic_error);
        EXPECT_NO_THROW(min_pri_queue_.updatePriority(existing_node, existing_node.f_ - 10));
        auto existing_str = *my_strings_.begin();
        EXPECT_THROW(max_pri_queue_.updatePriority(existing_str, ""), std::logic_error);
        EXPECT_NO_THROW(max_pri_queue_.updatePriority(existing_str, existing_str + "233"));
    }
}
}
