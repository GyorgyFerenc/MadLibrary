#include "Core.hpp"
#include <thread>


/*
 * This is an example of 
 * the 'pin' pattern
 *
 *  example usage:
 *  struct My_Thread_Data{
 *      inline static Thread_Data_Type type = thread_data_declare();
 *  
 *      int my_int_field;
 *  };
 *
 */

using Thread_Data_Type = usize;

Thread_Data_Type thread_data_declare(){
    static Thread_Data_Type idx = 0;
    return idx++;
}

struct Thread_Data{
    Thread_Data_Type type;
    void*            data;
};


template <class T>
Option<T*> thread_data_to(Thread_Data thread_data){
    if (T::type == thread_data.type) 
        return {true, (T*)thread_data.data};

    return {false};
}


template <class T>
Thread_Data thread_data_from(T* ptr){
    return {
        .type = T::type,
        .data = ptr,
    };
}


using Thread_Procedure = void (*)(Thread_Data);

struct Thread{
    std::thread thread;
};


Thread thread_start(Thread_Procedure proc, Thread_Data data = {}){
    return {
        .thread = std::thread{proc, data},
    };
}

void thread_join(Thread* t){
    t->thread.join();
}




