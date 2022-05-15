#pragma once

#include <deque>
#include <functional>
#include <stdexcept>

namespace custom_cont {
// D叉堆(D-ary heap)数据结构。
template <typename T>
class DAryHeap {
protected:
    // 节点在堆中的位置。
    using NodePos = size_t;
    // 用于比较两节点大小的函数。
    using CmpFunc = std::function<bool(T, T)>;

    // 每个父节点最多可以有多少个子节点（不得小于2）。
    int d_;
    // 用于比较两节点的函数。
    CmpFunc cmp_func_;
    // 堆中节点的个数。
    size_t size_;
    // 存储于堆中的节点。
    std::deque<T> nodes_;

public:
    // 使用堆中的节点nodes来构造堆。
    template <typename Nodes>
    explicit DAryHeap(int d, CmpFunc&& cmp_func, Nodes&& nodes)
        : d_(d)
        , cmp_func_(std::move(cmp_func))
        , size_(nodes.size())
        , nodes_(std::forward<Nodes>(nodes))
    {
        this->buildHeap();
    }
    DAryHeap() = default;
    virtual ~DAryHeap() = default;

    // 返回堆中存储的节点的数量。
    size_t size() const noexcept { return size_; }
    // 判断堆是否为空。
    bool empty() const noexcept { return size_ == 0; }
    // 将一个节点node插入堆中，时间复杂度：O(d*log_d(N))。
    template <typename TNode>
    void push(TNode&& node)
    {
        size_ += 1;
        nodes_.push_back(std::forward<TNode>(node));
        this->heapifyUp(size_ - 1);
    }
    // 返回堆顶的节点。
    const T& top() const
    {
        if (size_ == 0) {
            throw std::out_of_range("The D-ary heap is empty!!!");
        }
        return nodes_.front();
    }
    // 移除堆顶的节点。
    void pop()
    {
        if (size_ == 0) {
            throw std::out_of_range("The D-ary heap is empty!!!");
        }
        nodes_.at(0) = nodes_.back();
        nodes_.pop_back();
        size_ -= 1;
        if (size_ > 0) {
            this->heapifyDown(0);
        }
    }
    // 移除堆顶的节点并返回。
    T popAndReturn()
    {
        if (size_ == 0) {
            throw std::out_of_range("The D-ary heap is empty!!!");
        }
        T node_to_return = std::move(nodes_.at(0));
        nodes_.at(0) = nodes_.back();
        nodes_.pop_back();
        size_ -= 1;
        if (size_ > 0) {
            this->heapifyDown(0);
        }
        return node_to_return;
    }

protected:
    // 构建堆，时间复杂度O(n)。
    void buildHeap()
    {
        if (d_ < 2) {
            throw std::invalid_argument("D must be lareger or equal to 2!!!");
        }
        for (NodePos pos_to_fix = size_ / d_ + 1; pos_to_fix > 0; --pos_to_fix) {
            this->heapifyDown(pos_to_fix - 1);
        }
    }
    // 判断堆中第node_pos个节点是否为叶节点。
    bool isLeafNode(NodePos node_pos) const noexcept
    {
        return d_ * node_pos + 2 > size_;
    }
    // 返回堆中第parent_node_pos个节点的第child_ord个子节点的位置。
    NodePos getChildNodePos(NodePos parent_node_pos, size_t child_ord) const noexcept
    {
        return d_ * parent_node_pos + child_ord + 1;
    }
    // 返回堆中第child_pos个节点所属的父节点的位置。
    NodePos getParentNodePos(NodePos child_pos) const noexcept
    {
        return (child_pos - 1) / d_;
    }
    // 交换第i个和第j个节点的位置。
    void swapNodes(NodePos pos_i, NodePos pos_j) noexcept
    {
        std::swap(nodes_[pos_i], nodes_[pos_j]);
    }
    // 在pos_to_fix位置添加一个节点后通过bubble down的方式修复堆，时间复杂度O(d)。
    void heapifyDown(NodePos pos_to_fix) noexcept
    {
        NodePos comp_est = pos_to_fix, cur_pos = pos_to_fix;
        while (!this->isLeafNode(cur_pos)) {
            for (size_t child_order = 0; child_order < d_; ++child_order) {
                auto child_node_pos = this->getChildNodePos(cur_pos, child_order);
                if (child_node_pos < size_
                    && cmp_func_(nodes_.at(comp_est), nodes_.at(child_node_pos))) {
                    comp_est = child_node_pos;
                }
            }
            if (cur_pos == comp_est) {
                return;
            }
            this->swapNodes(cur_pos, comp_est);
            cur_pos = comp_est;
        }
    }
    // 在pos_to_fix位置添加一个节点后通过bubble up的方式修复堆，时间复杂度O(k)。
    void heapifyUp(NodePos pos_to_fix) noexcept
    {
        while (pos_to_fix > 0) {
            NodePos parent_node_pos = this->getParentNodePos(pos_to_fix);
            if (!cmp_func_(nodes_.at(parent_node_pos), nodes_.at(pos_to_fix))) {
                return;
            }
            this->swapNodes(pos_to_fix, parent_node_pos);
            pos_to_fix = parent_node_pos;
        }
    }
};

// 构建空的最小堆。
template <typename T>
auto createEmptyMinDHeap(int d = 2)
{
    return DAryHeap<T>(d, std::greater<T>(), std::deque<T>());
}

// 构建空的最大堆。
template <typename T>
auto createEmptyMaxDHeap(int d = 2)
{
    return DAryHeap<T>(d, std::less<T>(), std::deque<T>());
}

// 使用堆中的节点nodes来构造最小堆。
template <typename T, typename Nodes>
auto buildMinDHeap(int d, Nodes&& nodes)
{
    return DAryHeap<T>(d, std::greater<T>(), std::forward<Nodes>(nodes));
}

// 使用堆中的节点nodes来构造最大堆。
template <typename T, typename Nodes>
auto buildMaxDHeap(int d, Nodes&& nodes)
{
    return DAryHeap<T>(d, std::less<T>(), std::forward<Nodes>(nodes));
}
}