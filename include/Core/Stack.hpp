#pragma once

#include "Context.hpp"
#include "Intrinsics.hpp"
#include "Memory.hpp"
/*
 * It uses a linked list to implement the stack
 * if you need a list based implementation just use List
 */
template <class T>
struct Stack {
    struct Node {
        T     elem;
        Node* next;
    };

    Node*      head = nullptr;
    usize      size = 0;
    IAllocator allocator;
};

template <class T>
static Stack<T> init(Stack<T>& stack, Context context) {
    stack.allocator = context.allocator;
    return stack;
}

template <class T>
void destroy(Stack<T>& stack) {
    let ptr = stack.head;
    while (ptr != nullptr) {
        let tmp = ptr->next;
        free(stack.allocator, ptr);
        ptr = tmp;
    }
}

template <class T>
void push(Stack<T>& stack, T element) {
    let try_node = allocate<typename Stack<T>::Node>(stack.allocator);
    let node = unwrap(try_node, "stack push");
    node->elem = element;
    node->next = stack.head;
    stack.head = node;

    stack.size++;
}

namespace Private {
template <class T>
void clone_aux(typename Stack<T>::Node* node, Stack<T>& new_stack) {
    if (node->next != nullptr) clone_aux(node->next, new_stack);
    push(new_stack, node->elem);
}
};  // namespace Private

template <class T>
Stack<T> clone(Stack<T>& stack, Context context) {
    Stack<T> new_stack;
    new_stack.allocator = context.allocator;

    Private::clone_aux(stack.head, new_stack);

    return new_stack;
}

/*
 * Does not handle empty case
 * Undifiend behaviour
 */
template <class T>
T& top(Stack<T>& stack) {
    return stack.head->elem;
}

/*
 * Errs if stack is empty
 */
template <class T>
Errorable<T*> top_safe(Stack<T>& stack) {
    if (stack.size == 0) return {CoreError::EmptyAcces};
    return {CoreError::Correct, &stack.head->elem};
}

/*
 * Poping on empty stack
 * results undefiend behaviour
 */
template <class T>
T pop(Stack<T>& stack) {
    let tmp = stack.head;
    stack.head = tmp->next;

    let elem = tmp->elem;
    stack.allocator->template free(tmp);

    stack.size--;

    return elem;
}

/*
 * Errs if stack is empty
 */
template <class T>
Errorable<T> pop_safe(Stack<T>& stack) {
    if (empty(stack)) return {CoreError::InvalidOperation};

    let elem = pop(stack);
    return {CoreError::Correct, elem};
}

template <class T>
inline bool empty(Stack<T>& stack) {
    return stack.size == 0;
}
