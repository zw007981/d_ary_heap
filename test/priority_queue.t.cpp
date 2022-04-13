#include <boost/container_hash/hash.hpp>
#include <gtest/gtest.h>
#include <iostream>
#include <queue>
#include <random>
#include <string>

#include "../src/priority_queue.hpp"
#include "scoped_timer.h"

namespace usr_defined_cont {
namespace test_priority_queue {
    constexpr int RAND_SEED = 19950910;
    constexpr double DOUBLE_EPS = 0.01;
    struct MyNode {
        explicit MyNode(int node_id, double g, double h)
            : node_id_(node_id)
            , g_(g)
            , h_(h)
        {
            f_ = g_ + h_;
        }
        int node_id_;
        double f_ { 0.0 };
        double g_ { 0.0 };
        double h_ { 0.0 };
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
        std::string getInfoInStr() const
        {
            std::ostringstream info;
            info << "Node ID = " << node_id_ << ", f = " << f_
                 << ", g = " << g_ << ", h = " << h_;
            return info.str();
        }
    };
    struct MyNodeHasher {
        size_t operator()(const MyNode& my_node) const
        {
            std::size_t seed = 0;
            boost::hash_combine(seed, boost::hash_value(my_node.node_id_));
            return seed;
        }
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
    auto genNodeFunc = []() {
        int node_id = std::rand() % 1000;
        double g = 0.1 * (std::rand() % 10000), h = 0.1 * (std::rand() % 10000);
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

    class TestPriQueueFixture : public ::testing::Test {
        using MinPriQueue = PriQueue<MyNode, double, MyNodeHasher>;
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
            min_pri_queue_ = createEmptyMinPriQueue<MyNode, double, MyNodeHasher>(3);
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
            const auto& [node1, key1] = min_pri_queue_.topNodeAndPri();
            EXPECT_TRUE(node1 == expected_node);
            EXPECT_NEAR(key1, expected_node.f_, DOUBLE_EPS);
            auto [node2, key2] = min_pri_queue_.popAndReturn();
            EXPECT_TRUE(node2 == expected_node);
            EXPECT_NEAR(key2, expected_node.f_, DOUBLE_EPS);
            std_min_pri_queue.pop();
        }
        EXPECT_THROW(min_pri_queue_.popAndReturn(), std::out_of_range);
        for (size_t i = 0; i < num_strings_; i++) {
            const auto& expected_str = std_max_pri_queue.top();
            const auto& [str1, key1] = max_pri_queue_.topNodeAndPri();
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
        auto empty_min_pri_queue = createEmptyMinPriQueue<MyNode, double, MyNodeHasher>(2);
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
        auto empty_max_pri_queue = createEmptyMaxPriQueue<std::string, std::string>(10);
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
            EXPECT_NEAR(node.f_, min_pri_queue_.getPriOfNode(node), 0.01);
        }
        auto new_node = MyNode(88888888, 0.1, 0.2);
        EXPECT_THROW(min_pri_queue_.getPriOfNode(new_node), std::out_of_range);
        for (const auto& str : my_strings_) {
            EXPECT_TRUE(str == max_pri_queue_.getPriOfNode(str));
        }
        auto new_str = std::string("!233");
        EXPECT_THROW(max_pri_queue_.getPriOfNode(new_str), std::out_of_range);
    }

    TEST_F(TestPriQueueFixture, testUpdatePri)
    {
        for (size_t i = 0; i < num_nodes_; i++) {
            if (std::rand() % 10 <= 4) {
                auto prev_node = MyNode(my_nodes_[i]);
                my_nodes_[i].f_ -= 0.1 * (std::rand() % 10000);
                min_pri_queue_.updatePri(prev_node, my_nodes_[i].f_);
            }
        }
        for (size_t i = 0; i < num_strings_; i++) {
            if (std::rand() % 10 <= 4) {
                auto prev_str = std::string(my_strings_[i]);
                my_strings_[i] += "233";
                max_pri_queue_.updatePri(prev_str, my_strings_[i]);
            }
        }
        auto [std_min_pri_queue, std_max_pri_queue] = this->buildSTDPriQueue();
        for (size_t i = 0; i < num_nodes_; i++) {
            const auto& expected_node = std_min_pri_queue.top();
            const auto& [node, key] = min_pri_queue_.topNodeAndPri();
            EXPECT_TRUE(expected_node == node);
            EXPECT_NEAR(expected_node.f_, key, DOUBLE_EPS);
            EXPECT_GT(node.g_ + node.h_, node.f_ - DOUBLE_EPS);
            std_min_pri_queue.pop();
            min_pri_queue_.pop();
        }
        EXPECT_THROW(min_pri_queue_.popAndReturn(), std::out_of_range);
        for (size_t i = 0; i < num_strings_; i++) {
            const auto& expected_str = std_max_pri_queue.top();
            auto [str, key] = max_pri_queue_.popAndReturn();
            EXPECT_TRUE(expected_str == key);
            EXPECT_TRUE(str <= key);
            std_max_pri_queue.pop();
        }
        EXPECT_THROW(max_pri_queue_.popAndReturn(), std::out_of_range);
    }

    TEST_F(TestPriQueueFixture, testUpdatePriForNodeNotExist)
    {
        auto new_node = MyNode(88888888, 0.1, 0.2);
        EXPECT_THROW(min_pri_queue_.updatePri(new_node, new_node.f_), std::out_of_range);
        min_pri_queue_.push(new_node, new_node.f_);
        EXPECT_NO_THROW(min_pri_queue_.updatePri(new_node, new_node.f_ - 2.33));
        auto new_str = std::string("!233");
        EXPECT_THROW(max_pri_queue_.updatePri(new_str, new_str), std::out_of_range);
        max_pri_queue_.push(new_str, new_str);
        EXPECT_NO_THROW(max_pri_queue_.updatePri(new_str, new_str + "233"));
    }

    TEST_F(TestPriQueueFixture, testUpdatePriArbitrarily)
    {
        auto existing_node = *my_nodes_.begin();
        EXPECT_THROW(min_pri_queue_.updatePri(existing_node, existing_node.f_ + 10.0), std::logic_error);
        EXPECT_NO_THROW(min_pri_queue_.updatePri(existing_node, existing_node.f_ - 10.0));
        auto existing_str = *my_strings_.begin();
        EXPECT_THROW(max_pri_queue_.updatePri(existing_str, ""), std::logic_error);
        EXPECT_NO_THROW(max_pri_queue_.updatePri(existing_str, existing_str + "233"));
    }

    TEST_F(TestPriQueueFixture, testCompareWithSTDPriQueue)
    {
        int64_t runtime_min_pri_queue, runtime_std_min_pri_queue,
            runtime_max_pri_queue, runtime_std_max_pri_queue;
        auto min_pri_queue = createEmptyMinPriQueue<MyNode, double, MyNodeHasher>(3);
        {
            auto timer = usr_defined_timer::ScopedTimer("MinPriQueuePushAndPop");
            for (const auto& node : my_nodes_) {
                min_pri_queue.push(node, node.f_);
            }
            for (size_t i = 0; i < num_nodes_; i++) {
                if (std::rand() % 10 < 3) {
                    min_pri_queue.pop();
                }
            }
            runtime_min_pri_queue = timer.countInNS();
        }
        auto std_min_pri_queue = createSTDMinPriQueue<MyNode>();
        {
            auto timer = usr_defined_timer::ScopedTimer("STDMinPriQueuePushAndPop");
            for (const auto& node : my_nodes_) {
                std_min_pri_queue.push(node);
            }
            for (size_t i = 0; i < num_strings_; i++) {
                if (std::rand() % 10 < 3) {
                    std_min_pri_queue.pop();
                }
            }
            runtime_std_min_pri_queue = timer.countInNS();
        }
        auto max_pri_queue = createEmptyMaxPriQueue<std::string, std::string>(2);
        {
            auto timer = usr_defined_timer::ScopedTimer("MaxPriQueuePushAndPop");
            for (const auto& str : my_strings_) {
                max_pri_queue.push(str, str);
            }
            for (size_t i = 0; i < num_strings_; i++) {
                max_pri_queue.pop();
            }
            runtime_max_pri_queue = timer.countInNS();
        }
        auto std_max_pri_queue = createSTDMaxPriQueue<std::string>();
        {
            auto timer = usr_defined_timer::ScopedTimer("STDMaxPriQueuePushAndPop");
            for (const auto& str : my_strings_) {
                std_max_pri_queue.push(str);
            }
            for (size_t i = 0; i < num_strings_; i++) {
                std_max_pri_queue.pop();
            }
            runtime_std_max_pri_queue = timer.countInNS();
        }
        std::cout << "For min priority queue, usr_defined/std = " << runtime_min_pri_queue / static_cast<double>(runtime_std_min_pri_queue)
                  << ", for max priority queue, usr_defined/std = " << runtime_max_pri_queue / static_cast<double>(runtime_std_max_pri_queue)
                  << ".\n";
    }
}
}
