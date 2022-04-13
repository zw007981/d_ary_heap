#include <algorithm>
#include <gtest/gtest.h>
#include <random>
#include <stdlib.h>
#include <string>
#include <time.h>
#include <vector>

#include "../src/heap.hpp"

namespace usr_defined_cont {
namespace test_heap {
    struct MyString {
        MyString(const std::string& _value)
            : value_(_value)
        {
        }
        const std::string& value()
        {
            return value_;
        }
        bool operator<(const MyString& another) const
        {
            return value_ < another.value_;
        }
        bool operator>(const MyString& another) const
        {
            return value_ > another.value_;
        }
        bool operator==(const MyString& another) const
        {
            return value_ == another.value_;
        }
        bool operator!=(const MyString& another) const
        {
            return value_ != another.value_;
        }
        std::string value_;
    };
    class TestHeapFixture : public ::testing::Test {
    public:
        std::vector<MyString> values_in_str_ { MyString("Dijkstra"), MyString("Bellman-Ford"),
            MyString("A-star"), MyString("Hybrid A-star"),
            MyString("RRT"), MyString("RRT-star") };
        std::vector<int> values_in_int_;
        const int expected_values_in_str_size_ { static_cast<int>(values_in_str_.size()) };
        const int expected_values_in_int_size_ { 44 };
        KHeap<MyString> min_k_heap_ = buildMinKHeap<MyString>(10, values_in_str_);
        KHeap<int> max_k_heap_ = createEmptyMaxKHeap<int>(2);

    public:
        void SetUp() override
        {
            srand((unsigned)time(NULL));
            for (int i = 0; i < expected_values_in_int_size_; i++) {
                int num_to_push = std::rand() % 100;
                values_in_int_.push_back(num_to_push);
                max_k_heap_.push(num_to_push);
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
            std::make_heap(values_in_str_.begin(), values_in_str_.end(), std::greater<MyString> {});
            std::make_heap(values_in_int_.begin(), values_in_int_.end(), std::less<int> {});
        }
        template <typename T>
        bool isTwoHeapsEqual(std::vector<T> std_heap, KHeap<T> k_heap, std::function<bool(T, T)> cmp_func) const
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

        static std::string strRand(int length)
        {
            std::string result = "";
            char tmp;
            if (length <= 0) {
                return result;
            }
            std::random_device rand_dev;
            std::default_random_engine engine(rand_dev());
            for (int i = 0; i < length; i++) {
                tmp = random() % 62;
                if (tmp < 10) {
                    tmp += '0';
                } else if (tmp < 36) {
                    tmp -= 10;
                    tmp += 'A';
                } else {
                    tmp -= 36;
                    tmp += 'a';
                }
                result += tmp;
            }
            return result;
        }
    };

    TEST_F(TestHeapFixture, testMbrFuncSizeAndEmpty)
    {
        auto empty_k_heap = createEmptyMaxKHeap<int>(2);
        EXPECT_EQ(empty_k_heap.size(), 0);
        EXPECT_TRUE(empty_k_heap.empty());
        EXPECT_EQ(min_k_heap_.size(), expected_values_in_str_size_);
        EXPECT_FALSE(min_k_heap_.empty());
        EXPECT_EQ(max_k_heap_.size(), expected_values_in_int_size_);
        EXPECT_FALSE(max_k_heap_.empty());
        for (size_t i = 0; i < 1000; i++) {
            max_k_heap_.push(std::rand());
            EXPECT_EQ(max_k_heap_.size(), expected_values_in_int_size_ + i + 1);
        }
        for (size_t i = 0; i < 1000 + expected_values_in_int_size_; i++) {
            max_k_heap_.pop();
            EXPECT_EQ(max_k_heap_.size(), expected_values_in_int_size_ + 1000 - (i + 1));
        }
        EXPECT_TRUE(max_k_heap_.empty());
    }

    TEST_F(TestHeapFixture, testMbrFuncTopAndPop)
    {
        for (size_t i = 0; i < expected_values_in_str_size_; i++) {
            auto top_node1 = values_in_str_.at(0);
            auto top_node2 = min_k_heap_.top();
            EXPECT_EQ(top_node1, top_node2);
            std::pop_heap(values_in_str_.begin(), values_in_str_.end(), std::greater<MyString> {});
            values_in_str_.pop_back();
            min_k_heap_.pop();
        }
        for (size_t i = 0; i < expected_values_in_int_size_; i++) {
            auto top_node1 = values_in_int_.at(0);
            auto top_node2 = max_k_heap_.top();
            EXPECT_EQ(top_node1, top_node2);
            std::pop_heap(values_in_int_.begin(), values_in_int_.end(), std::less<int> {});
            values_in_int_.pop_back();
            max_k_heap_.pop();
        }
    }

    TEST_F(TestHeapFixture, testMbrFuncPush)
    {
        auto cmp_func1 = std::greater<MyString> {};
        for (size_t i = 0; i < 100; i++) {
            auto str_to_push = this->strRand(std::rand() % 20);
            min_k_heap_.push(str_to_push);
            values_in_str_.push_back(str_to_push);
            std::make_heap(values_in_str_.begin(), values_in_str_.end(), cmp_func1);
            EXPECT_TRUE(this->isTwoHeapsEqual<MyString>(values_in_str_, min_k_heap_, cmp_func1));
        }
        auto cmp_func2 = std::less<int> {};
        for (size_t i = 0; i < 100; i++) {
            int num_to_push = std::rand();
            max_k_heap_.push(num_to_push);
            values_in_int_.push_back(num_to_push);
            std::make_heap(values_in_int_.begin(), values_in_int_.end(), cmp_func2);
            EXPECT_TRUE(this->isTwoHeapsEqual<int>(values_in_int_, max_k_heap_, cmp_func2));
        }
    }
}
}
