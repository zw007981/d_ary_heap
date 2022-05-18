#pragma once

#include <stdexcept>
#include <unordered_map>
#include <vector>

namespace custom_cont {
enum class PriQueueTyp {
    MIN_PRI_QUEUE,
    MAX_PRI_QUEUE
};

// 基于D叉堆的优先队列。T: 队列中的元素, TPri: 用于排序的元素优先级, THash: 用于求解元素哈希值的函数。
template <typename T, typename TPri, typename THash = std::hash<T>>
class PriQueue {
protected:
    // 节点，包含有元素的基本信息和优先级。
    using Node = std::pair<T, TPri>;
    // 节点在堆中的位置。
    using NodePos = size_t;
    // 用于比较两节点大小的函数。
    using CmpFunc = std::function<bool(TPri, TPri)>;

    // 每个父节点最多可以有多少个子节点（不得小于2）。
    int d_;
    // 优先队列的种类。
    PriQueueTyp typ_;
    // 用于比较两节点的函数。
    CmpFunc cmp_func_;
    // 优先队列中节点的个数。
    size_t size_;
    // 存储于堆中的节点。
    std::vector<Node> nodes_;
    // 从元素到它们在堆中位置的映射。
    std::unordered_map<T, NodePos, THash> element_to_pos_;

public:
    // 使用队列中的元素elements和它们的优先级priorities来构造优先队列。
    explicit PriQueue(int d, PriQueueTyp typ, CmpFunc&& cmp_func,
        const std::vector<T>& elements, const std::vector<TPri>& priorities)
        : d_(d)
        , typ_(typ)
        , cmp_func_(std::move(cmp_func))
        , size_(elements.size())
        , element_to_pos_(this->buildElementToPos(elements))
        , nodes_(this->assembleHeap(elements, priorities))
    {
        this->buildHeap();
    }
    PriQueue() = default;
    virtual ~PriQueue() = default;

    // 返回队列中存储的节点的数量。
    size_t size() const noexcept { return size_; }
    // 判断队列是否为空。
    bool empty() const noexcept { return size_ == 0; }
    // 判断一个元素element是否在队列中。
    bool contains(const T& element) const noexcept
    {
        return element_to_pos_.find(element) != element_to_pos_.end();
    }
    // 将一个元素element和它的优先级pri插入队列中，默认会执行重复性检测，时间复杂度：O(d*log_d(N))。
    template <bool perform_chk = true, typename TFwd, typename TPriFwd>
    void push(TFwd&& element, TPriFwd&& pri)
    {
        if (perform_chk && this->contains(element)) {
            throw std::logic_error("Element is in the queue!!!");
        }
        element_to_pos_[element] = size_;
        nodes_.emplace_back(std::forward<TFwd>(element), std::forward<TPriFwd>(pri));
        size_ += 1;
        this->heapifyUp(size_ - 1);
    }
    // 将元素element对应的优先级更新为pri，时间复杂度：O(d*log_d(N))。
    void updatePriority(const T& element, TPri pri)
    {
        auto pos_it = element_to_pos_.find(element);
        if (pos_it == element_to_pos_.end()) {
            throw std::out_of_range("No such element is present!!!");
        } else if (typ_ == PriQueueTyp::MIN_PRI_QUEUE) {
            if (nodes_.at(pos_it->second).second <= pri) {
                throw std::logic_error("Only decreas key operation can be performed in min priority queue!!!");
            } else {
                nodes_[pos_it->second].second = std::move(pri);
                this->heapifyUp(pos_it->second);
            }
        } else {
            if (nodes_.at(pos_it->second).second >= pri) {
                throw std::logic_error("Only increase key operation can be performed in max priority queue!!!");
            } else {
                nodes_[pos_it->second].second = std::move(pri);
                this->heapifyDown(pos_it->second);
            }
        }
    }
    // 返回元素element对应的优先级，默认会执行重复性检测。
    template <bool perform_chk = true>
    const TPri& getPriority(const T& element) const
    {
        auto pos_it = element_to_pos_.find(element);
        if (perform_chk && pos_it == element_to_pos_.end()) {
            throw std::out_of_range("Unable to find the given node!!!");
        }
        return nodes_.at(pos_it->second).second;
    }
    // 返回队列中的第一个元素。
    const T& top() const
    {
        if (size_ == 0) {
            throw std::out_of_range("The priority queue is empty!!!");
        }
        return nodes_.front().first;
    }
    // 返回队列中的第一个元素和它的优先级。
    const Node& topNode() const
    {
        if (size_ == 0) {
            throw std::out_of_range("The priority queue is empty!!!");
        }
        return nodes_.front();
    }
    // 移除队列中的第一个元素。
    void pop()
    {
        if (size_ == 0) {
            throw std::out_of_range("The priority queue is empty!!!");
        }
        const auto& node_to_pop = nodes_.at(0);
        element_to_pos_.erase(node_to_pop.first);
        nodes_.at(0) = nodes_.back();
        nodes_.pop_back();
        size_ -= 1;
        if (size_ > 0) {
            element_to_pos_[nodes_.front().first] = 0;
            this->heapifyDown(0);
        }
    }
    // 移除队列中的第一个元素并返回它和它的优先级。
    Node popAndReturn()
    {
        if (size_ == 0) {
            throw std::out_of_range("The priority queue is empty!!!");
        }
        Node node_to_return = std::move(nodes_.at(0));
        element_to_pos_.erase(node_to_return.first);
        nodes_.at(0) = nodes_.back();
        nodes_.pop_back();
        size_ -= 1;
        if (size_ > 0) {
            element_to_pos_[nodes_.front().first] = 0;
            this->heapifyDown(0);
        }
        return node_to_return;
    }

protected:
    // 构建从输入的元素到它在堆中位置的映射。
    static auto buildElementToPos(const std::vector<T>& elements)
    {
        decltype(element_to_pos_) element_to_pos;
        NodePos node_pos = 0;
        for (const auto& element : elements) {
            element_to_pos[element] = node_pos;
            node_pos++;
        }
        return element_to_pos;
    }
    // 根据输入的元素和优先级生成未经排序的堆。
    static auto assembleHeap(const std::vector<T>& elements, const std::vector<TPri>& priorities)
    {
        if (elements.size() != priorities.size()) {
            throw std::invalid_argument("Number of elements must be equal to number of priorities!!!");
        }
        decltype(nodes_) nodes;
        for (size_t i = 0; i < elements.size(); i++) {
            nodes.emplace_back(elements.at(i), priorities.at(i));
        }
        return nodes;
    }
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
    // 比较位置为i和j的两个节点的优先级的大小。
    bool cmpNodes(NodePos pos_i, NodePos pos_j) const noexcept
    {
        return cmp_func_(nodes_.at(pos_i).second, nodes_.at(pos_j).second);
    }
    // 交换第i个和第j个节点的位置。
    void swapNodes(NodePos pos_i, NodePos pos_j) noexcept
    {
        std::swap(element_to_pos_[nodes_.at(pos_i).first], element_to_pos_[nodes_.at(pos_j).first]);
        std::swap(nodes_[pos_i], nodes_[pos_j]);
    }
    // 在pos_to_fix位置添加一个节点后通过bubble down的方式修复堆，时间复杂度O(d)。
    void heapifyDown(NodePos pos_to_fix) noexcept
    {
        NodePos pos_to_cmp = pos_to_fix, cur_pos = pos_to_fix;
        while (!this->isLeafNode(cur_pos)) {
            for (size_t child_order = 0; child_order < d_; ++child_order) {
                NodePos child_node_pos = this->getChildNodePos(cur_pos, child_order);
                if (child_node_pos < size_ && this->cmpNodes(pos_to_cmp, child_node_pos)) {
                    pos_to_cmp = child_node_pos;
                }
            }
            if (cur_pos == pos_to_cmp) {
                return;
            }
            this->swapNodes(cur_pos, pos_to_cmp);
            cur_pos = pos_to_cmp;
        }
    }
    // 在pos_to_fix位置添加一个节点后通过bubble up的方式修复堆，时间复杂度O(d)。
    void heapifyUp(NodePos pos_to_fix) noexcept
    {
        while (pos_to_fix > 0) {
            NodePos parent_node_pos = this->getParentNodePos(pos_to_fix);
            if (!this->cmpNodes(parent_node_pos, pos_to_fix)) {
                return;
            }
            this->swapNodes(pos_to_fix, parent_node_pos);
            pos_to_fix = parent_node_pos;
        }
    }
};

// 构建空的最小优先队列。
template <typename T, typename TPri, typename THash = std::hash<T>>
auto createEmptyMinPriQueue(int d = 2)
{
    return PriQueue<T, TPri, THash>(d, PriQueueTyp::MIN_PRI_QUEUE, std::greater<> {},
        std::vector<T>(), std::vector<TPri>());
}

// 构建空的最大优先队列。
template <typename T, typename TPri, typename THash = std::hash<T>>
auto createEmptyMaxPriQueue(int d = 2)
{
    return PriQueue<T, TPri, THash>(d, PriQueueTyp::MAX_PRI_QUEUE, std::less<> {},
        std::vector<T>(), std::vector<TPri>());
}

// 使用队列中的元素elements和它们的优先级priorities来构造优先队列。
template <typename T, typename TPri, typename THash = std::hash<T>, typename Elements, typename Priorities>
auto buildMinPriQueue(int d, Elements&& elements, Priorities&& priorities)
{
    return PriQueue<T, TPri, THash>(d, PriQueueTyp::MIN_PRI_QUEUE, std::greater<> {},
        std::forward<Elements>(elements), std::forward<Priorities>(priorities));
}

// 使用队列中的元素elements和它们的优先级priorities来构造优先队列。
template <typename T, typename TPri, typename THash = std::hash<T>, typename Elements, typename Priorities>
auto buildMaxPriQueue(int d, Elements&& elements, Priorities&& priorities)
{
    return PriQueue<T, TPri, THash>(d, PriQueueTyp::MAX_PRI_QUEUE, std::less<> {},
        std::forward<Elements>(elements), std::forward<Priorities>(priorities));
}
}
