#pragma once
#include <mutex>

#include "Context.hpp"
#include "Intrinsics.hpp"
#include "Memory.hpp"

// TODO(Ferenc): Implement it
// rewrite it using list maybe?

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

    Allocator*      allocator;
    static Queue<T> create(Context context) {
        Queue<T> q;
        q.head = nullptr;
        q.tail = nullptr;
        q.size = 0;
        q.allocator = context.allocator;
        return q;
    }
    static Queue<T> create() {
        return create(default_context());
    }
};

template <class T>
void destroy(Queue<T>& queue) {
    typename Queue<T>::Node* ptr = queue.head;
    while (ptr != nullptr) {
        let tmp = ptr->next;
        queue.allocator->template free(ptr);
        ptr = tmp;
    }
}

template <class T>
void enque(Queue<T>& queue, T element) {
    let try_node = queue.allocator->template allocate<typename Queue<T>::Node>();
    let node = unwrap(try_node);
    node->next = nullptr;
    node->elem = element;

    if (queue.tail != nullptr) queue.tail->next = node;
    queue.tail = node;
    if (queue.head == nullptr) queue.head = node;

    queue.size++;
}

/*
 * this does not do boundary checking
 */
template <class T>
T deque(Queue<T>& queue) {
    queue.size--;

    let elem = queue.head->elem;
    let tmp = queue.head;

    queue.head = queue.head->next;
    if (tmp == queue.tail) queue.tail = nullptr;
    queue.allocator->template free(tmp);

    return elem;
}

template <class T>
inline bool empty(Queue<T>& queue) {
    return queue.size == 0;
}

/*
 * Thread safe wrapper
 * around Queue<T>
 * it uses std::mutex
 */
template <class T>
struct SharedQueue {
    Queue<T>   queue;
    std::mutex mutex;
};

template <class T>
inline void init(SharedQueue<T>& shared_queue, Context context) {
    shared_queue.queue = Queue<T>::create(context);
}

template <class T>
inline void init(SharedQueue<T>& shared_queue) {
    init(shared_queue, default_context());
}

template <class T>
void destroy(SharedQueue<T>& queue) {
    destroy(queue.queue);
}

template <class T>
void enque(SharedQueue<T>& queue, T element) {
    queue.mutex.lock();
    enque(queue.queue, element);
    queue.mutex.unlock();
}

/*
 * this does not do boundary checking
 */
template <class T>
T deque(SharedQueue<T>& queue) {
    queue.mutex.lock();
    let el = deque(queue.queue);
    queue.mutex.unlock();
    return el;
}

template <class T>
inline bool empty(SharedQueue<T>& queue) {
    queue.mutex.lock();
    let e = empty(queue.queue);
    queue.mutex.unlock();
    return e;
}

template <class T>
inline usize size(SharedQueue<T>& shared_queue) {
    shared_queue.mutex.lock();
    let size = shared_queue.queue.size;
    shared_queue.mutex.unlock();
    return size;
}