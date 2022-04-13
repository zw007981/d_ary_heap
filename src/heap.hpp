#pragma once

#include <algorithm>
#include <functional>
#include <stdexcept>
#include <vector>

namespace usr_defined_cont {
// K叉堆（K-ary Heap）数据结构。
template <typename T>
class KHeap {
public:
    using CmpFunc = std::function<bool(T, T)>;
    using NodeID = int;

protected:
    // 每个父节点最多可以有多少个子节点（不得小于2）。
    int k_;
    // 堆中存储节点的个数。
    int size_;
    // 存储于堆中的节点。
    std::vector<T> nodes_;
    // 用于比较两节点大小的函数。
    CmpFunc cmp_func_;

public:
    explicit KHeap(int k, const std::vector<T>& nodes, CmpFunc&& cmp_func)
        : k_(k)
        , nodes_(nodes)
        , cmp_func_(std::move(cmp_func))
    {
        this->initHeap();
    }
    explicit KHeap(int k, std::vector<T>&& nodes, CmpFunc&& cmp_func)
        : k_(k)
        , nodes_(std::move(nodes))
        , cmp_func_(std::move(cmp_func))
    {
        this->initHeap();
    }
    KHeap() = delete;
    virtual ~KHeap() = default;

    // 返回堆中存储的元素的个数。
    int size() const noexcept
    {
        return size_;
    }
    // 判断堆是否为空。
    bool empty() const noexcept
    {
        return size_ == 0;
    }
    // 返回堆顶的元素。
    const T& top() const
    {
        if (size_ == 0) {
            throw std::out_of_range("The K-ary heap is empty!!!");
        }
        return nodes_.at(0);
    }
    // 移除堆顶的元素。
    void pop()
    {
        if (size_ == 0) {
            throw std::out_of_range("The K-ary heap is empty!!!");
        }
        nodes_.at(0) = nodes_.back();
        nodes_.pop_back();
        size_ -= 1;
        this->heapifyDown(0);
    }
    // 将一个元素插入堆中。
    template <class... Args>
    void push(Args&&... args)
    {
        auto id_to_fix = this->size();
        nodes_.emplace_back(std::forward<Args>(args)...);
        size_ += 1;
        this->heapifyUp(id_to_fix);
    }

protected:
    // 判断编号为node_id的节点是否为叶子节点。
    bool isLeaf(NodeID node_id) const noexcept
    {
        return node_id * k_ + 2 > this->size();
    }
    // 返回编号为parent_id的父节点的第child_order个子节点的序号。
    NodeID getChildID(NodeID parent_id, size_t child_order) const noexcept
    {
        return k_ * parent_id + child_order + 1;
    }
    // 返回编号为child_id的子节点的父节点的序号。
    NodeID getParentID(NodeID child_id) const noexcept
    {
        return (child_id - 1) / k_;
    }
    // 构建堆，时间复杂度O(n)。
    void buildHeap() noexcept
    {
        for (NodeID id_to_fix = this->size() / k_ + 1; id_to_fix > 0; --id_to_fix) {
            this->heapifyDown(id_to_fix - 1);
        }
    }
    // 交换堆中编号i和编号j的两个节点。
    void swapNodes(NodeID i, NodeID j) noexcept
    {
        std::swap(nodes_.at(i), nodes_.at(j));
    }
    // 在id_to_fix位置添加一个节点后通过bubble down的方式修复堆，时间复杂度O(k)。
    void heapifyDown(NodeID id_to_fix) noexcept
    {
        const int length = this->size();
        NodeID comp_est = id_to_fix, cur_id = id_to_fix;
        while (!this->isLeaf(cur_id)) {
            for (size_t child_order = 0; child_order < k_; ++child_order) {
                auto child_id = this->getChildID(cur_id, child_order);
                if (child_id < length
                    && cmp_func_(nodes_.at(comp_est), nodes_.at(child_id))) {
                    comp_est = child_id;
                }
            }
            if (cur_id == comp_est) {
                return;
            }
            this->swapNodes(cur_id, comp_est);
            cur_id = comp_est;
        }
    }
    // 在id_to_fix位置添加一个节点后通过bubble up的方式修复堆，时间复杂度O(k)。
    void heapifyUp(NodeID id_to_fix) noexcept
    {
        while (id_to_fix > 0) {
            auto parent_id = this->getParentID(id_to_fix);
            if (!cmp_func_(nodes_.at(parent_id), nodes_.at(id_to_fix))) {
                return;
            }
            this->swapNodes(id_to_fix, parent_id);
            id_to_fix = parent_id;
        }
    }
    // 堆的初始化，时间复杂度O(n).
    void initHeap()
    {
        if (k_ < 2) {
            throw std::invalid_argument("K must be at least 2!!!");
        }
        size_ = nodes_.size();
        this->buildHeap();
    }
};

// 构建空的最小堆。
template <typename T>
auto createEmptyMinKHeap(int k)
{
    return KHeap<T>(k, std::vector<T>(), std::greater<T>());
}

// 构建空的最大堆。
template <typename T>
auto createEmptyMaxKHeap(int k)
{
    return KHeap<T>(k, std::vector<T>(), std::less<T>());
}

// 使用提供的节点nodes来构建最小堆。
template <typename T, typename Nodes>
auto buildMinKHeap(int k, Nodes&& nodes)
{
    return KHeap<T>(k, std::forward<Nodes>(nodes), std::greater<T>());
}

// 使用提供的节点nodes来构建最大堆。
template <typename T, typename Nodes>
auto buildMaxKHeap(int k, Nodes&& nodes)
{
    return KHeap<T>(k, std::forward<Nodes>(nodes), std::less<T>());
}
}
