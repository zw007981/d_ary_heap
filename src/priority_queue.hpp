#pragma once

#include <unordered_map>

#include "heap.hpp"

namespace pri_queue {
enum class PriorityQueueType {
    MIN_HEAP,
    MAX_HEAP
};
// Generic priority queue based on a k-ary heap.
// T: type of nodes, TKey: type of keys used to order the heap, THash: functor used to hash values of type T.
template <typename T, typename TKey, typename THash = std::hash<T>>
class PriorityQueue : private KHeap {
protected:
    using CmpFunc = KHeap::CmpFunc;
    using NodeID = KHeap::NodeID;
    using super = KHeap;

    // Keep track of the value of the keys of each node.
    std::unordered_map<T, TKey, THash> node_to_key_;
    // Keep track of the index of the nodes stroed in the heap.
    std::unordered_map<T, NodeID, THash> node_to_id_;
    // Type of this priority queue.
    PriorityQueueType type_;

public:
    PriorityQueue() = delete;
    virtual ~PriorityQueue() = default;

    // Retrun the number of elements in the queue.
    int size() const noexcept
    {
        return super::size();
    }
    // Retrun true iff the heap is empty.
    bool empty() const noexcept
    {
        return super::empty();
    }
    // Add a new node and associates the given key to it, with time complexity O(k*log_k(N)).
    void push(const T& node, const TKey& key)
    {
        auto id_to_fix = this->size();
        node_to_key_[node] = key;
        node_to_id_[node] = id_to_fix;
        super::push(node);
    }
    // Update the key of an element in the queue, time complexity O(k*log_k(N)).
    void updateKey(const T& node, const TKey& key)
    {
        const auto id_to_fix = node_to_id_.at(element);
        node_to_key_.at(element) = key;
        // Recover the heap property.
        if (type_ == PriorityQueueType::MAX_HEAP) {
            super::heapifyDown(id_to_fix);
        } else {
            super::heapifyUp(id_to_fix);
        }
    }

    // Return the key of the given element.
    const TKey& keyOfNode(const T& node) const
    {
        return node_to_key_.at(node);
    }

    // Return true iff the given element is in the queue.
    inline bool contains(const T& node) const noexcept
    {
        return node_to_id_.find(node) != node_to_id_.end();
    }

    // Return the top element.
    const T& top() const
    {
        return super::top();
    }

    // Return top node-key pair.
    std::pair<const T&, const TKey&> topNodeAndKey() const noexcept
    {
        const auto& top_node = super::top();
        const auto& top_key = this->keyOfNode(top_node);
        return std::make_pair(top_node, top_key);
    }

    // Remove the top element, time complexity O(k*log_k(N)).
    void pop()
    {
        if (this->size() <= 0) {
            throw std::out_of_range("The priority queue is empty!");
        }
        NodeID id_to_remove = 0;
        const auto node_to_remove = nodes_.at(id_to_remove);
        node_to_id_.erase(node_to_remove);
        node_to_key_.erase(node_to_remove);
        // Replace root of the heap with the last element of the vector.
        nodes_.at(0) = nodes_.back();
        // Remove the last element of the vector.
        nodes_.pop_back();
        // Recover the heap property.
        if (this->size() > 0) {
            const auto front_node = nodes_.front();
            node_to_id_[front_node] = 0;
            super::heapifyDown(0);
        }
    }
};
}