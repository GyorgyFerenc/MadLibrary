#pragma once
#include "Context.hpp"
#include "Intrinsics.hpp"
#include "Memory.hpp"

// TODO(Ferenc): Rewrite it using list maybe?

/*
 * Current implementation uses a linked list
 */
template <class T>
struct Queue {
    struct Node {
        T     elem;
        Node* next = nullptr;
    };

    Node* head = nullptr;
    Node* tail = nullptr;

    usize size = 0;

    IAllocator allocator;
};

namespace Queue_ {
template <class T>
void init(Queue<T>* queue, Context context) {
    queue->head = nullptr;
    queue->tail = nullptr;
    queue->size = 0;
    queue->allocator = context.allocator;
}

template <class T>
Queue<T> create(Context context) {
    Queue<T> q;
    init(&q, context);
    return q;
}

template <class T>
void destroy(Queue<T>* queue) {
    typename Queue<T>::Node* ptr = queue->head;
    while (ptr != nullptr) {
        let tmp = ptr->next;
        free(queue->allocator, ptr);
        ptr = tmp;
    }
}

template <class T>
void enque(Queue<T>* queue, T element) {
    let try_node = IAllocator_::allocate<typename Queue<T>::Node>(queue->allocator);
    let node = Errorable_::unwrap(try_node);
    node->next = nullptr;
    node->elem = element;

    if (queue->tail != nullptr) queue->tail->next = node;
    queue->tail = node;
    if (queue->head == nullptr) queue->head = node;

    queue->size++;
}

/*
 * this does not do boundary checking
 */
template <class T>
T deque(Queue<T>* queue) {
    queue->size--;

    let elem = queue->head->elem;
    let tmp = queue->head;

    queue->head = queue->head->next;
    if (tmp == queue->tail) queue->tail = nullptr;
    free(queue->allocator, tmp);

    return elem;
}

template <class T>
inline bool empty(Queue<T> queue) {
    return queue.size == 0;
}
}  // namespace Queue_
