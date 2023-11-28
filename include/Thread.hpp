#include "Core.hpp"
#include <thread>


/*
 * This is an example of 
 * the 'pin' pattern
 *
 *  example usage:
 *  struct My_Thread_Data{
 *      inline static Thread_Data_Type type = declare_thread_data();
 *  
 *      int my_int_field;
 *  };
 *
 */

using Thread_Data_Type = usize;

Thread_Data_Type declare_thread_data(){
    static Thread_Data_Type idx = 0;
    return idx++;
}

struct Thread_Data{
    Thread_Data_Type type;
    void*            data;
};

namespace Thread_Data_{

template <class T>
Option<T*> to(Thread_Data thread_data){
    if (T::type == thread_data.type) 
        return {true, (T*)thread_data.data};

    return {false};
}

template <class T>
T* to_unwrap(Thread_Data thread_data){
    assert(thread_data.type == T::type);
    return (T*) thread_data.data;
}

template <class T>
Thread_Data from(T* ptr){
    return {
        .type = T::type,
        .data = ptr,
    };
}

};

using Thread_Procedure = void (*)(Thread_Data);

struct Thread{
    std::thread thread;
};

namespace Thread_{

Thread start(Thread_Procedure proc, Thread_Data data = {}){
    return {
        .thread = std::thread{proc, data},
    };
}

void join(Thread* t){
    t->thread.join();
}

};



