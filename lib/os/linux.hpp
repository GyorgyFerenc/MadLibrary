#pragma  once

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cerrno>

#include "../core.hpp"
#include "shared.hpp" 

// ------
// Socket
// ------

struct Socket{
    int socket_descriptor;
    Address address;
};

Option<Socket> tcp_start_listener(Address address, usize listen_nr = 50, bool non_blocking = false){
    int type = SOCK_STREAM;
    if (non_blocking) type |= SOCK_NONBLOCK;
    Socket s;
    s.address = address;
    s.socket_descriptor = socket(AF_INET, type, 0);
    if (s.socket_descriptor < 0){
        return {{}, false};
    }

    int opt = 1;
    let optname = SO_REUSEADDR | SO_REUSEPORT;
    let ret = setsockopt(s.socket_descriptor, SOL_SOCKET, optname, &opt, sizeof(opt));
    if (ret != 0){
        return {{}, false};
    }

    struct sockaddr_in sockaddress;
    sockaddress.sin_family = AF_INET;
    // Todo(Ferenc): Check for endianness
    sockaddress.sin_addr.s_addr = htonl(*(u32*)address.ip);
    sockaddress.sin_port = htons(address.port);
    ret = bind(s.socket_descriptor, (struct sockaddr*)&sockaddress, sizeof(sockaddress));
    if (ret < 0){
        return {{}, false};
    }

    ret = listen(s.socket_descriptor, listen_nr);
    if (ret < 0){
        return {{}, false};
    }

    return {s, true};
}

void close(Socket socket){
    shutdown(socket.socket_descriptor, SHUT_RDWR);
    ::close(socket.socket_descriptor);
}


Address address_from(struct sockaddr_in sockaddress){
    Address adr;
    memcpy(adr.ip, &sockaddress.sin_addr.s_addr, sizeof(u32));
    adr.port = sockaddress.sin_port;
    return adr;
}


Option<Socket> tcp_accept(Socket listener){
    struct sockaddr_in sockaddress;
    socklen_t len = sizeof(sockaddress); // WHYYYYYYYYYYYYYYYYYyy
    
    let new_socket_descriptor = ::accept(listener.socket_descriptor, (struct sockaddr*)&sockaddress, &len);   
    if (new_socket_descriptor < 0) {
        return {{}, false};
    }

    return {{
        .socket_descriptor = new_socket_descriptor,
        .address = address_from(sockaddress),
    }, true};
}


Option<Socket> tcp_dial(Address address, bool non_blocking = false){
    int type = SOCK_STREAM;
    if (non_blocking) type |= SOCK_NONBLOCK;
    Socket s;
    s.address = address;
    s.socket_descriptor = socket(AF_INET, type, 0);
    if (s.socket_descriptor < 0){
        return {{}, false};
    }

    struct sockaddr_in sockaddress;
    sockaddress.sin_family = AF_INET;
    sockaddress.sin_addr.s_addr = *(u32*)address.ip;
    sockaddress.sin_port = htons(address.port);
    let ret = connect(s.socket_descriptor, (struct sockaddr*)&sockaddress, sizeof(sockaddress));
    if (ret < 0){
        return {{}, false};
    }

    return {s, true};
}

bool tcp_send(Socket socket, Slice<u8> data){
    // Todo(Ferenc): implement better errors
    let ret = ::send(socket.socket_descriptor, data.buffer, data.len, 0);
    return ret <= 0;
}

Pair<ssize, Errno> tcp_receive(Socket socket, Slice<u8> data){
    let nr = recv(socket.socket_descriptor, data.buffer, data.len, 0);
    let err = 0;
    if (nr < 0){
        err = errno;
    }
    return {nr, cast(Errno) err};
}


// ----
// File
// ----

//Todo(Ferenc): Implement files

//struct File{
//    enum Mode{
//        Read,
//        Write,  // Creates if doesn't exists
//        Append, // Creates if doesn't exists
//    };
//    int fd;
//};
//
//Option<File> file_open_c_str(const char* path, File::Mode mode = File::Mode::Read){
//    File file;
//
//    switch (mode) {
//    case File::Mode::Read:{
//        file.fd = open(path, O_RDONLY);
//    }break;
//    case File::Mode::Write:{
//        file.fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
//    }break;
//    case File::Mode::Append:{
//        file.fd = open(path, O_WRONLY | O_CREAT | O_APPEND, S_IRWXU);
//    }break;
//    default:
//        file.fd = -1;
//    }
//
//    if (file.fd < 0){
//        return {false, file};
//    }
//
//    return {true, file};
//}
//
//
//Option<File> file_open(String str, Allocator allocator, File::Mode mode = File::Mode::Read){
//    let [error, cstr] = string_c_str(str, allocator); 
//    if (error != Core_Error::Correct) return {false};
//
//    defer(allocator_free_array(allocator, cstr, str.size + 1));
//    return file_open_c_str(cstr, mode);
//}
//
//void file_close(File file){
//    close(file.fd);
//}
//
//usize file_read(File file, Array<u8>* array){
//    let nr = read(file.fd, array->ptr, array->size);
//    array->size = nr;
//    return nr;
//}
//
//
//usize file_write(File file, Array<u8> array){
//    return write(file.fd, array.ptr, array.size);
//}
//
//
//usize file_size(File file){
//    struct stat s;
//    fstat(file.fd, &s);
//    return s.st_size;
//}
//
//
//Array<u8> file_read_all(File file, Allocator allocator){
//    let size = file_size(file);
//    let array = array_create<u8>(allocator, size);
//    file_read(file, &array);
//    return array;
//}


// ------
// Thread
// ------

#include <pthread.h>

struct Thread_Data{
    Allocator allocator;
    void*     data;
};

using Thread_Proc = void(*)(Thread_Data);

struct Thread{
    pthread_t   p_thread;
    Thread_Proc procedure;
    Thread_Data data;
};

void start(Thread* thread, Thread_Proc procedure, Thread_Data data = {NULL}){
    thread->procedure = procedure;
    thread->data = data;
    pthread_create(&thread->p_thread, NULL, [](void* void_thread) -> void*{
        let thread = (Thread*)void_thread;
        thread->procedure(thread->data);
        return NULL;
    }, thread);
}

void join(Thread thread){
    pthread_join(thread.p_thread, NULL);
}

void cancel(Thread thread){
    pthread_cancel(thread.p_thread);
}

/*
 * Copying is not allowed
 */
struct Mutex{
    pthread_mutex_t p_mutex;  
};

inline
Mutex create_mutex(){
    let mutex = Mutex{ };
    pthread_mutex_init(&mutex.p_mutex, NULL);
    return mutex;
}

inline
void lock(Mutex* mutex){
    pthread_mutex_lock(&mutex->p_mutex);
}

/*
 * Returns true if it locked the mutex
 */
inline
bool try_lock(Mutex* mutex){
    return !pthread_mutex_trylock(&mutex->p_mutex);
}

inline
void unlock(Mutex* mutex){
    pthread_mutex_unlock(&mutex->p_mutex);
}

inline
void destroy(Mutex* mutex){
    pthread_mutex_destroy(&mutex->p_mutex);
}

struct Conditional_Variable{
    pthread_cond_t p_cond;
};

inline
Conditional_Variable create_conditional_variable(){
    Conditional_Variable cond;
    pthread_cond_init(&cond.p_cond, NULL);
    return cond;
}

inline
void destroy(Conditional_Variable* cond){
    pthread_cond_destroy(&cond->p_cond);
}

inline
void notify_one(Conditional_Variable* cond){
    pthread_cond_signal(&cond->p_cond);
}

inline
void notify_all(Conditional_Variable* cond){
    pthread_cond_broadcast(&cond->p_cond);
}

inline
void wait(Conditional_Variable* cond, Mutex* mutex){
    pthread_cond_wait(&cond->p_cond, &mutex->p_mutex);
}


/*
   ----
   Pipe
   ----
*/

struct Pipe{
    int read;
    int write;
};

void init(Pipe* p){
    int inner[2];
    pipe(inner);
    p->read  = inner[0];
    p->write = inner[1];
}

/*
   It closes both ends
*/
void close(Pipe p){
    close(p.read);
    close(p.write);
}
