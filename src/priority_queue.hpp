#pragma once

#include <stdexcept>
#include <unordered_map>
#include <vector>

namespace usr_defined_cont {
enum class PriQueueTyp {
    MIN_PRI_QUEUE,
    MAX_PRI_QUEUE
};

// 基于k叉堆的优先队列。T: 节点种类, TPri: 用于排序的节点优先级, THash: 用于求解节点哈希值的函数。
template <typename T, typename TPri, typename THash = std::hash<T>>
class PriQueue {
protected:
    using NodeID = int;
    using CmpFunc = std::function<bool(TPri, TPri)>;

    // 每个父节点最多可以有多少个子节点（不得小于2）。
    int k_;
    // 优先队列的种类。
    PriQueueTyp type_;
    // 用于比较两节点大小的函数。
    CmpFunc cmp_func_;
    // 堆中存储节点的个数。
    int size_;
    // 存储于堆中的节点。
    std::vector<T> nodes_;
    // 从节点到它优先级的映射。
    std::unordered_map<T, TPri, THash> node_to_pri_;
    // 从节点到它在堆中位置的映射。
    std::unordered_map<T, NodeID, THash> node_to_id_;

public:
    // 通过传引用的方式构造优先队列。
    explicit PriQueue(int k, PriQueueTyp type, CmpFunc&& cmp_func,
        const std::vector<T>& nodes, const std::vector<TPri>& priorities)
        : k_(k)
        , type_(type)
        , cmp_func_(std::move(cmp_func))
        , nodes_(nodes)
        , node_to_pri_(this->buildNodeToPriMap(this->nodes_, priorities))
        , node_to_id_(this->buildNodeToIDMap(this->nodes_))
    {
        this->buildHeap();
    }
    // 通过传右值的方式构造优先队列。
    explicit PriQueue(int k, PriQueueTyp type, CmpFunc&& cmp_func,
        std::vector<T>&& nodes, std::vector<TPri>&& priorities)
        : k_(k)
        , type_(type)
        , cmp_func_(std::move(cmp_func))
        , nodes_(std::move(nodes))
        , node_to_pri_(this->buildNodeToPriMap(this->nodes_, std::move(priorities)))
        , node_to_id_(this->buildNodeToIDMap(this->nodes_))
    {
        this->buildHeap();
    }
    PriQueue() = default;
    virtual ~PriQueue() = default;

    // 返回队列中存储节点的个数。
    int size() const noexcept
    {
        return size_;
    }
    // 判断队列是否为空。
    bool empty() const noexcept
    {
        return size_ == 0;
    }
    // 判断一个节点是否存在于队列中。
    bool contains(const T& node) const noexcept
    {
        return node_to_id_.find(node) != node_to_id_.end();
    }
    // 将一个节点和它的优先级插入队列中并返回成功与否，默认会在插入前执行重复性检测，时间复杂度：O(k*log_k(N))。
    void push(const T& node, const TPri& pri, bool perform_chk = true)
    {
        if (perform_chk && this->contains(node)) {
            throw std::logic_error("Node is already in the queue!!!");
        }
        auto id = this->size();
        node_to_id_[node] = id;
        node_to_pri_[node] = pri;
        nodes_.emplace_back(node);
        size_ += 1;
        this->heapifyUp(id);
    }
    // 更新节点对应的优先级，时间复杂度：O(k*log_k(N))。
    void updatePri(const T& node, const TPri& pri)
    {
        const auto it = node_to_id_.find(node);
        if (it == node_to_id_.end()) {
            throw std::out_of_range("No such node is present!!!");
        } else if (type_ == PriQueueTyp::MIN_PRI_QUEUE) {
            if (node_to_pri_.at(node) <= pri) {
                throw std::logic_error("Only decrease key operation can be performed!!!");
            } else {
                node_to_pri_.at(node) = pri;
                const NodeID id_to_fix = node_to_id_.at(node);
                this->heapifyUp(id_to_fix);
            }
        } else {
            if (node_to_pri_.at(node) >= pri) {
                throw std::logic_error("Only increase key operation can be performed!!!");
            } else {
                node_to_pri_.at(node) = pri;
                const NodeID id_to_fix = node_to_id_.at(node);
                this->heapifyDown(id_to_fix);
            }
        }
    }
    // 返回节点对应的优先级。
    const TPri& getPriOfNode(const T& node) const
    {
        const auto it = node_to_pri_.find(node);
        if (it == node_to_pri_.end()) {
            throw std::out_of_range("Unable to find the given node!!!");
        }
        return it->second;
    }
    // 返回队列中的第一个节点。
    const T& top() const
    {
        if (size_ == 0) {
            throw std::out_of_range("The priority queue is empty!!!");
        }
        return nodes_.at(0);
    }
    // 返回队列中的第一个节点和它的优先级。
    std::pair<const T&, const TPri&> topNodeAndPri() const
    {
        const auto& node = this->top();
        const auto& pri = this->getPriOfNode(node);
        return { node, pri };
    }
    // 移除队列中的第一个节点。
    void pop()
    {
        if (size_ == 0) {
            throw std::out_of_range("The priority queue is empty!!!");
        }
        const auto node_to_pop = nodes_.at(0);
        node_to_id_.erase(node_to_pop);
        node_to_pri_.erase(node_to_pop);
        nodes_.at(0) = nodes_.back();
        nodes_.pop_back();
        size_ -= 1;
        if (size_ > 0) {
            const auto node_front = nodes_.front();
            node_to_id_.at(node_front) = 0;
            this->heapifyDown(0);
        }
    }
    // 移除队列中的第一个节点并返回它和它的优先级。
    std::pair<T, TPri> popAndReturn()
    {
        if (size_ == 0) {
            throw std::out_of_range("The priority queue is empty!!!");
        }
        T node_to_return = std::move(nodes_.at(0));
        TPri pri_to_return = std::move(node_to_pri_.at(node_to_return));
        node_to_id_.erase(node_to_return);
        node_to_pri_.erase(node_to_return);
        nodes_.at(0) = nodes_.back();
        nodes_.pop_back();
        size_ -= 1;
        if (size_ > 0) {
            const auto node_front = nodes_.front();
            node_to_id_.at(node_front) = 0;
            this->heapifyDown(0);
        }
        return std::make_pair(node_to_return, pri_to_return);
    }

protected:
    // 构建从节点到它优先级的映射。
    static auto buildNodeToPriMap(const std::vector<T>& nodes, const std::vector<TPri>& priorities)
    {
        if (nodes.size() != priorities.size()) {
            throw std::invalid_argument("Number of nodes must be equal to the number of priorities!!!");
        }
        decltype(node_to_pri_) node_to_pri;
        for (size_t i = 0; i < nodes.size(); i++) {
            node_to_pri[nodes.at(i)] = priorities.at(i);
        }
        return node_to_pri;
    }
    // 构建从节点到它在堆中位置的映射。
    static auto buildNodeToIDMap(const std::vector<T>& nodes)
    {
        decltype(node_to_id_) node_to_id;
        NodeID index = 0;
        for (const auto& node : nodes) {
            node_to_id[node] = index;
            index++;
        }
        return node_to_id;
    }
    // 判断编号为node_id的节点是否为叶子节点。
    bool isLeaf(NodeID node_id) const noexcept
    {
        return node_id * k_ + 2 > this->size();
    }
    // 返回编号为parent_id的父节点的第child_order个子节点的编号。
    NodeID getChildID(NodeID parent_id, size_t child_order) const noexcept
    {
        return k_ * parent_id + child_order + 1;
    }
    // 返回编号为child_id的子节点的父节点的编号。
    NodeID getParentID(NodeID child_id) const noexcept
    {
        return (child_id - 1) / k_;
    }
    // 比较编号为i和j两个节点所对应优先级的大小。
    bool cmpTwoNodes(NodeID i, NodeID j) const
    {
        return cmp_func_(node_to_pri_.at(nodes_.at(i)),
            node_to_pri_.at(nodes_.at(j)));
    }
    // 构建堆，时间复杂度O(n).
    void buildHeap()
    {
        if (k_ < 2) {
            throw std::invalid_argument("K must be at least 2!!!");
        }
        size_ = nodes_.size();
        for (NodeID id_to_fix = this->size() / k_ + 1; id_to_fix > 0; --id_to_fix) {
            this->heapifyDown(id_to_fix - 1);
        }
    }
    // 交换第i和第j个节点的位置。
    void swapNodes(NodeID i, NodeID j) noexcept
    {
        auto node_i = this->nodes_.at(i), node_j = this->nodes_.at(j);
        std::swap(node_to_id_.at(node_i), node_to_id_.at(node_j));
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
                    && this->cmpTwoNodes(comp_est, child_id)) {
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
            if (!this->cmpTwoNodes(parent_id, id_to_fix)) {
                return;
            }
            this->swapNodes(id_to_fix, parent_id);
            id_to_fix = parent_id;
        }
    }
};

// 构建空的最小优先队列。
template <typename T, typename TPri, typename THash = std::hash<T>>
auto createEmptyMinPriQueue(int k)
{
    return PriQueue<T, TPri, THash>(k, PriQueueTyp::MIN_PRI_QUEUE, std::greater<> {},
        std::vector<T>(), std::vector<TPri>());
}

// 构建空的最大优先队列。
template <typename T, typename TPri, typename THash = std::hash<T>>
auto createEmptyMaxPriQueue(int k)
{
    return PriQueue<T, TPri, THash>(k, PriQueueTyp::MAX_PRI_QUEUE, std::less<> {},
        std::vector<T>(), std::vector<TPri>());
}

// 使用节点nodes和它们的优先级priorities构建最小优先队列。
template <typename T, typename TPri, typename THash = std::hash<T>, typename Nodes, typename Priorities>
auto buildMinPriQueue(int k, Nodes&& nodes, Priorities&& priorities)
{
    return PriQueue<T, TPri, THash>(k, PriQueueTyp::MIN_PRI_QUEUE, std::greater<> {},
        std::forward<Nodes>(nodes), std::forward<Priorities>(priorities));
}

// 使用节点nodes和它们的优先级priorities构建最大优先队列。
template <typename T, typename TPri, typename THash = std::hash<T>, typename Nodes, typename Priorities>
auto buildMaxPriQueue(int k, Nodes&& nodes, Priorities&& priorities)
{
    return PriQueue<T, TPri, THash>(k, PriQueueTyp::MAX_PRI_QUEUE, std::less<> {},
        std::forward<Nodes>(nodes), std::forward<Priorities>(priorities));
}
}