#pragma once

#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cstdint>


/*
 * ========
 * Internal
 * ========
 */

//#define internal static

#define ML_LINUX   0
#define ML_WINDOWS 1

#ifndef ML_OS
    #define ML_OS  ML_LINUX
#endif

#define ML_LITTLE_ENDIAN 0
#define ML_BIG_ENDIAN    1

#ifndef ML_BYTE_ORDER
    #define ML_BYTE_ORDER  ML_LITTLE_ENDIAN
#endif

//#define ML_DEBUG
#ifdef ML_DEBUG
    #define debug_assert(a) assert(a)
#else
    #define debug_assert(a)
#endif

#define KB 1024
#define MB 1024 * KB
#define GB 1024 * MB

using uint = unsigned int;
using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;
using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;
using usize = std::size_t;
using ssize = ssize_t;
using f64 = double;
using f32 = float;
using uptr = uintptr_t;
using iptr  = intptr_t;

#define UNREACHABLE assert(false && "uncreachable");

#define let auto // variable declaration
#define cast(type) (type)
#define interface class

#include <functional>
struct defer_implementation {
    std::function<void()> func;

    inline ~defer_implementation() {
        func();
    }
};

struct defer_dummy {
    inline defer_implementation operator+(std::function<void()> func) {
        return defer_implementation{func};
    }
};

#define CONCAT_IMPL(str1, str2) str1##str2
#define CONCAT(str1, str2) CONCAT_IMPL(str1, str2)

#ifdef __COUNTER__
    #define ANONYMOUS_VARIABLE(str) CONCAT(str, __COUNTER__)
#else
    #define ANONYMOUS_VARIABLE(str) CONCAT(str, __LINE__)
#endif


#define defer_block auto ANONYMOUS_VARIABLE(__defer_instance) = defer_dummy{} + [&]()
#define defer(body) \
    auto ANONYMOUS_VARIABLE(__defer_instance) = defer_implementation{[&]() { body; }};

#define UNREACHABLE assert(false && "uncreachable");

void panic(const char* text) {
    printf("%s\n",text);
    assert(false && "panic");
}

void panic() {
    assert(false && "panic");
}

/*
   Errno
*/

enum struct Errno: int{
    Ok = 0,
    Eperm = 1,       // Operation not permitted
    Enoent,          // No such file or directory
    Esrch,           // No such process
    Eintr,           // Interrupted system call
    Eio,             // I/O error
    Enxio,           // No such device or address
    E2big,           // Argument list too long
    Enoexec,         // Exec format error
    Ebadf,           // Bad file number
    Echild,          // No child processes
    Eagain,          // Try again
    Enomem,          // Out of memory
    Eacces,          // Permission denied
    Efault,          // Bad address
    Enotblk,         // Block device required
    Ebusy,           // Device or resource busy
    Eexist,          // File exists
    Exdev,           // Cross-device link
    Enodev,          // No such device
    Enotdir,         // Not a directory
    Eisdir,          // Is a directory
    Einval,          // Invalid argument
    Enfile,          // File table overflow
    Emfile,          // Too many open files
    Enotty,          // Not a typewriter
    Etxtbsy,         // Text file busy
    Efbig,           // File too large
    Enospc,          // No space left on device
    Espipe,          // Illegal seek
    Erofs,           // Read-only file system
    Emlink,          // Too many links
    Epipe,           // Broken pipe
    Edom,            // Math argument out of domain of func
    Erange,          // Math result not representable
    Edeadlk,         // Resource deadlock would occur
    Enametoolong,    // File name too long
    Enolck,          // No record locks available
    Enosys,          // Function not implemented
    Enotempty,       // Directory not empty
    Eloop,           // Too many symbolic links encountered
    Enomsg,          // No message of desired type
    Eidrm,           // Identifier removed
    Echrng,          // Channel number out of range
    El2nsync,        // Level 2 not synchronized
    El3hlt,          // Level 3 halted
    El3rst,          // Level 3 reset
    Elnrng,          // Link number out of range
    Eunatch,         // Protocol driver not attached
    Enocsi,          // No CSI structure available
    El2hlt,          // Level 2 halted
    Ebade,           // Invalid exchange
    Ebadr,           // Invalid request descriptor
    Exfull,          // Exchange full
    Enoano,          // No anode
    Ebadrqc,         // Invalid request code
    Ebadslt,         // Invalid slot
    Ebfont,          // Bad font file format
    Enostr,          // Device not a stream
    Enodata,         // No data available
    Etime,           // Timer expired
    Enosr,           // Out of streams resources
    Enonet,          // Machine is not on the network
    Enopkg,          // Package not installed
    Eremote,         // Object is remote
    Enolink,         // Link has been severed
    Eadv,            // Advertise error
    Esrmnt,          // Srmount error
    Ecomm,           // Communication error on send
    Eproto,          // Protocol error
    Emultihop,       // Multihop attempted
    Edotdot,         // RFS specific error
    Ebadmsg,         // Not a data message
    Eoverflow,       // Value too large for defined data type
    Enotuniq,        // Name not unique on network
    Ebadfd,          // File descriptor in bad state
    Eremchg,         // Remote address changed
    Elibacc,         // Can not access a needed shared library
    Elibbad,         // Accessing a corrupted shared library
    Elibscn,         // .lib section in a.out corrupted
    Elibmax,         // Attempting to link in too many shared libraries
    Elibexec,        // Cannot exec a shared library directly
    Eilseq,          // Illegal byte sequence
    Erestart,        // Interrupted system call should be restarted
    Estrpipe,        // Streams pipe error
    Eusers,          // Too many users
    Enotsock,        // Socket operation on non-socket
    Edestaddrreq,    // Destination address required
    Emsgsize,        // Message too long
    Eprototype,      // Protocol wrong type for socket
    Enoprotoopt,     // Protocol not available
    Eprotonosupport, // Protocol not supported
    Esocktnosupport, // Socket type not supported
    Eopnotsupp,      // Operation not supported on transport endpoint
    Epfnosupport,    // Protocol family not supported
    Eafnosupport,    // Address family not supported by protocol
    Eaddrinuse,      // Address already in use
    Eaddrnotavail,   // Cannot assign requested address
    Enetdown,        // Network is down
    Enetunreach,     // Network is unreachable
    Enetreset,       // Network dropped connection because of reset
    Econnaborted,    // Software caused connection abort
    Econnreset,      // Connection reset by peer
    Enobufs,         // No buffer space available
    Eisconn,         // Transport endpoint is already connected
    Enotconn,        // Transport endpoint is not connected
    Eshutdown,       // Cannot send after transport endpoint shutdown
    Etoomanyrefs,    // Too many references: cannot splice
    Etimedout,       // Connection timed out
    Econnrefused,    // Connection refused
    Ehostdown,       // Host is down
    Ehostunreach,    // No route to host
    Ealready,        // Operation already in progress
    Einprogress,     // Operation now in progress
    Estale,          // Stale NFS file handle
    Euclean,         // Structure needs cleaning
    Enotnam,         // Not a XENIX named type file
    Enavail,         // No XENIX semaphores available
    Eisnam,          // Is a named type file
    Eremoteio,       // Remote I/O error
    Edquot,          // Quota exceeded
    Enomedium,       // No medium found
    Emediumtype,     // Wrong medium type
    Ecanceled,       // Operation Canceled
    Enokey,          // Required key not available
    Ekeyexpired,     // Key has expired
    Ekeyrevoked,     // Key has been revoked
    Ekeyrejected,    // Key was rejected by service
    Eownerdead,      // Owner died
    Enotrecoverable, // State not recoverable
};

const char* errno_to_c_str(Errno e){
    switch (e){
    case Errno::Ok:              return "Ok";
    case Errno::Eperm:           return "Operation not permitted";
    case Errno::Enoent:          return "No such file or directory";
    case Errno::Esrch:           return "No such process";
    case Errno::Eintr:           return "Interrupted system call";
    case Errno::Eio:             return "I/O error";
    case Errno::Enxio:           return "No such device or address";
    case Errno::E2big:           return "Argument list too long";
    case Errno::Enoexec:         return "Exec format error";
    case Errno::Ebadf:           return "Bad file number";
    case Errno::Echild:          return "No child processes";
    case Errno::Eagain:          return "Try again";
    case Errno::Enomem:          return "Out of memory";
    case Errno::Eacces:          return "Permission denied";
    case Errno::Efault:          return "Bad address";
    case Errno::Enotblk:         return "Block device required";
    case Errno::Ebusy:           return "Device or resource busy";
    case Errno::Eexist:          return "File exists";
    case Errno::Exdev:           return "Cross-device link";
    case Errno::Enodev:          return "No such device";
    case Errno::Enotdir:         return "Not a directory";
    case Errno::Eisdir:          return "Is a directory";
    case Errno::Einval:          return "Invalid argument";
    case Errno::Enfile:          return "File table overflow";
    case Errno::Emfile:          return "Too many open files";
    case Errno::Enotty:          return "Not a typewriter";
    case Errno::Etxtbsy:         return "Text file busy";
    case Errno::Efbig:           return "File too large";
    case Errno::Enospc:          return "No space left on device";
    case Errno::Espipe:          return "Illegal seek";
    case Errno::Erofs:           return "Read-only file system";
    case Errno::Emlink:          return "Too many links";
    case Errno::Epipe:           return "Broken pipe";
    case Errno::Edom:            return "Math argument out of domain of func";
    case Errno::Erange:          return "Math result not representable";
    case Errno::Edeadlk:         return "Resource deadlock would occur";
    case Errno::Enametoolong:    return "File name too long";
    case Errno::Enolck:          return "No record locks available";
    case Errno::Enosys:          return "Function not implemented";
    case Errno::Enotempty:       return "Directory not empty";
    case Errno::Eloop:           return "Too many symbolic links encountered";
    case Errno::Enomsg:          return "No message of desired type";
    case Errno::Eidrm:           return "Identifier removed";
    case Errno::Echrng:          return "Channel number out of range";
    case Errno::El2nsync:        return "Level 2 not synchronized";
    case Errno::El3hlt:          return "Level 3 halted";
    case Errno::El3rst:          return "Level 3 reset";
    case Errno::Elnrng:          return "Link number out of range";
    case Errno::Eunatch:         return "Protocol driver not attached";
    case Errno::Enocsi:          return "No CSI structure available";
    case Errno::El2hlt:          return "Level 2 halted";
    case Errno::Ebade:           return "Invalid exchange";
    case Errno::Ebadr:           return "Invalid request descriptor";
    case Errno::Exfull:          return "Exchange full";
    case Errno::Enoano:          return "No anode";
    case Errno::Ebadrqc:         return "Invalid request code";
    case Errno::Ebadslt:         return "Invalid slot";
    case Errno::Ebfont:          return "Bad font file format";
    case Errno::Enostr:          return "Device not a stream";
    case Errno::Enodata:         return "No data available";
    case Errno::Etime:           return "Timer expired";
    case Errno::Enosr:           return "Out of streams resources";
    case Errno::Enonet:          return "Machine is not on the network";
    case Errno::Enopkg:          return "Package not installed";
    case Errno::Eremote:         return "Object is remote";
    case Errno::Enolink:         return "Link has been severed";
    case Errno::Eadv:            return "Advertise error";
    case Errno::Esrmnt:          return "Srmount error";
    case Errno::Ecomm:           return "Communication error on send";
    case Errno::Eproto:          return "Protocol error";
    case Errno::Emultihop:       return "Multihop attempted";
    case Errno::Edotdot:         return "RFS specific error";
    case Errno::Ebadmsg:         return "Not a data message";
    case Errno::Eoverflow:       return "Value too large for defined data type";
    case Errno::Enotuniq:        return "Name not unique on network";
    case Errno::Ebadfd:          return "File descriptor in bad state";
    case Errno::Eremchg:         return "Remote address changed";
    case Errno::Elibacc:         return "Can not access a needed shared library";
    case Errno::Elibbad:         return "Accessing a corrupted shared library";
    case Errno::Elibscn:         return ".lib section in a.out corrupted";
    case Errno::Elibmax:         return "Attempting to link in too many shared libraries";
    case Errno::Elibexec:        return "Cannot exec a shared library directly";
    case Errno::Eilseq:          return "Illegal byte sequence";
    case Errno::Erestart:        return "Interrupted system call should be restarted";
    case Errno::Estrpipe:        return "Streams pipe error";
    case Errno::Eusers:          return "Too many users";
    case Errno::Enotsock:        return "Socket operation on non-socket";
    case Errno::Edestaddrreq:    return "Destination address required";
    case Errno::Emsgsize:        return "Message too long";
    case Errno::Eprototype:      return "Protocol wrong type for socket";
    case Errno::Enoprotoopt:     return "Protocol not available";
    case Errno::Eprotonosupport: return "Protocol not supported";
    case Errno::Esocktnosupport: return "Socket type not supported";
    case Errno::Eopnotsupp:      return "Operation not supported on transport endpoint";
    case Errno::Epfnosupport:    return "Protocol family not supported";
    case Errno::Eafnosupport:    return "Address family not supported by protocol";
    case Errno::Eaddrinuse:      return "Address already in use";
    case Errno::Eaddrnotavail:   return "Cannot assign requested address";
    case Errno::Enetdown:        return "Network is down";
    case Errno::Enetunreach:     return "Network is unreachable";
    case Errno::Enetreset:       return "Network dropped connection because of reset";
    case Errno::Econnaborted:    return "Software caused connection abort";
    case Errno::Econnreset:      return "Connection reset by peer";
    case Errno::Enobufs:         return "No buffer space available";
    case Errno::Eisconn:         return "Transport endpoint is already connected";
    case Errno::Enotconn:        return "Transport endpoint is not connected";
    case Errno::Eshutdown:       return "Cannot send after transport endpoint shutdown";
    case Errno::Etoomanyrefs:    return "Too many references: cannot splice";
    case Errno::Etimedout:       return "Connection timed out";
    case Errno::Econnrefused:    return "Connection refused";
    case Errno::Ehostdown:       return "Host is down";
    case Errno::Ehostunreach:    return "No route to host";
    case Errno::Ealready:        return "Operation already in progress";
    case Errno::Einprogress:     return "Operation now in progress";
    case Errno::Estale:          return "Stale NFS file handle";
    case Errno::Euclean:         return "Structure needs cleaning";
    case Errno::Enotnam:         return "Not a XENIX named type file";
    case Errno::Enavail:         return "No XENIX semaphores available";
    case Errno::Eisnam:          return "Is a named type file";
    case Errno::Eremoteio:       return "Remote I/O error";
    case Errno::Edquot:          return "Quota exceeded";
    case Errno::Enomedium:       return "No medium found";
    case Errno::Emediumtype:     return "Wrong medium type";
    case Errno::Ecanceled:       return "Operation Canceled";
    case Errno::Enokey:          return "Required key not available";
    case Errno::Ekeyexpired:     return "Key has expired";
    case Errno::Ekeyrevoked:     return "Key has been revoked";
    case Errno::Ekeyrejected:    return "Key was rejected by service";
    case Errno::Eownerdead:      return "Owner died";
    case Errno::Enotrecoverable: return "State not recoverable";
    }

    return "";
}

/*
   ------
   Option
   ------
*/

template<class T>
struct Option{
    T    value;
    bool ok = false;
};

/*
 * ======
 * Memory
 * ======
 */

template <class T>
inline
void typed_memset(T* ptr, T value, usize count){
    for (usize i = 0; i < count; i++){
        ptr[i] = value;
    }
}

bool is_power_of_two(uptr x) {
	return (x & (x-1)) == 0;
}

usize align_size(usize size, usize align){
	assert(is_power_of_two(align));

    let modulo = size % align; 
    if (modulo == 0) return size;
    return size + align - modulo;
}

constexpr usize Default_Align = sizeof(void*);

struct Allocator_Message{
    enum {
        Alloc_Zero,
        Alloc_Non_Zero,
        Free,
        Free_All,
        Realloc, // Copys and frees if needed
    } kind;

    union {
        struct {
            usize size;
            usize align;
        } alloc;

        struct {
            void* ptr;
        } free;

        struct {
            void* ptr;
            usize old_size;
            usize size;
            usize align;
        } realloc;
    };
};

using Allocator_Proc = void*(*)(void*, Allocator_Message);

/* 
interface Allocator{
    void* allocate_raw(Allocator allocator, usize size, usize align);
    void* allocate_raw_non_zero(Allocator allocator, usize size, usize align);
    void  free_all(Allocator allocator);
    void  free(Allocator allocator, void* ptr);
    void* reallocate_raw(Allocator allocator, void* ptr, usize old_size, usize size, usize align);
}
*/
struct Allocator{
    void* data = NULL;
    Allocator_Proc procedure = NULL;
};

void* allocate_raw(Allocator allocator, usize size, usize align = Default_Align) {
    return allocator.procedure(allocator.data, {
        .kind = Allocator_Message::Alloc_Zero,
        .alloc = {
            .size = size,
            .align = align,
        }
    });
}

void* allocate_raw_non_zero(Allocator allocator, usize size, usize align = Default_Align) {
    return allocator.procedure(allocator.data, {
        .kind = Allocator_Message::Alloc_Non_Zero,
        .alloc = {
            .size = size,
            .align = align,
        }
    });
}

void free_all(Allocator allocator){
    allocator.procedure(allocator.data, { .kind = Allocator_Message::Free_All, {} });
}

void free(Allocator allocator, void* ptr){
    allocator.procedure(allocator.data, { 
        .kind = Allocator_Message::Free, 
        .free = {
            .ptr = ptr,
        },
    });
}


void* reallocate_raw(Allocator allocator, void* ptr, usize old_size, usize size, usize align = Default_Align) {
    return allocator.procedure(allocator.data, {
        .kind = Allocator_Message::Realloc,
        .realloc = {
            .ptr = ptr,
            .old_size = old_size,
            .size = size,
            .align = align,
        }
    });
}

template <class T, interface Allocator>
T* allocate(Allocator allocator, usize align = Default_Align){ 
    return cast(T*) allocate_raw(allocator, sizeof(T), align); 
}

template <class T, interface Allocator>
T* allocate_non_zero(Allocator allocator, usize align = Default_Align) { 
    return cast(T*) allocate_raw_non_zero(allocator, sizeof(T), align); 
}

template <class T, interface Allocator>
T* allocate_array(Allocator allocator, usize size, usize align = Default_Align) { 
    return cast(T*) allocate_raw(allocator, sizeof(T) * size, align); 
}

template <class T, interface Allocator>
T* allocate_array_non_zero(Allocator allocator, usize size, usize align = Default_Align) { 
    return cast(T*) allocate_raw_non_zero(allocator, sizeof(T) * size, align); 
}

template <class T, interface Allocator>
T* reallocate(Allocator allocator, T* ptr, usize old_size, usize size, usize align = Default_Align) { 
    return cast(T*) reallocate_raw(allocator, ptr, sizeof(T) * old_size, sizeof(T) * size, align); 
}


/* 
 * ----------
 * Mallocator
 * ----------
 */

void* malloc_allocate(void*, Allocator_Message msg) {
    // Todo(Ferenc): Maybe add align
    switch (msg.kind){
    case Allocator_Message::Alloc_Zero: {
        let ptr = malloc(msg.alloc.size);
        memset(ptr, 0, msg.alloc.size);
        return ptr;
    } break;
    case Allocator_Message::Alloc_Non_Zero: {
        return malloc(msg.alloc.size);
    } break;
    case Allocator_Message::Free:{
        free(msg.free.ptr);
        return NULL;
    }break;
    case Allocator_Message::Free_All: return NULL;
    case Allocator_Message::Realloc: {
        return realloc(msg.realloc.ptr, msg.realloc.size);
    } break;
    }

    return NULL;
}

let mallocator = Allocator{
    .data = NULL,
    .procedure = malloc_allocate,
};

/*
 * -----
 * Arena
 * -----
 */

struct Arena{
    struct Dynamic{
        u8* next = NULL;
    };

    u8*   buffer = NULL;
    usize pos = 0;
    usize len = 0;
    void* last_alloc = NULL;

    // Dynamic if allocator is not NULL
    Allocator allocator;
    usize static_len = 0;
};

Arena create_static_arena(void* buffer, usize len){
    return {
        .buffer = cast(u8*) buffer,
        .len = len,
        .allocator = {},
    };
}

Arena create_dynamic_arena(Allocator allocator, usize len){
    return {
        .allocator = allocator,
        .static_len = len,
    };
}

inline
bool is_dynamic(Arena arena){
    return arena.allocator.procedure != NULL;
}

void dynamic_arena_grow(Arena* arena, usize size){
    let needed_size = sizeof(Arena::Dynamic) + size;
    let len = arena->static_len < needed_size ? needed_size : arena->static_len;
    let buffer = allocate_raw(arena->allocator, len);
    (cast(Arena::Dynamic*) buffer)->next = arena->buffer;

    arena->pos = sizeof(Arena::Dynamic);
    arena->len = len;
    arena->buffer = cast(u8*) buffer;
}

void* allocate_raw_non_zero(Arena* arena, usize size, usize align = Default_Align) {
    size = align_size(size, align);
    if (arena->buffer == NULL || arena->pos + size > arena->len) {
        if (!is_dynamic(*arena)) return NULL;
        dynamic_arena_grow(arena, size);
    }

    let ptr = arena->buffer + arena->pos;
    arena->pos += size;
    arena->last_alloc = ptr;
    return ptr;
}

void* allocate_raw(Arena* arena, usize size, usize align = Default_Align) {
    let ptr = allocate_raw_non_zero(arena, size, align);
    if (ptr != NULL) memset(ptr, 0, size); 
    return ptr;
}

void free_all(Arena* arena){
    if (is_dynamic(*arena)){
        let current = arena->buffer;
        while (current != NULL){
            let next = (cast(Arena::Dynamic*) current)->next;
            free(arena->allocator, current);
            current = next;
        }
    } else {
        arena->pos = 0;
        arena->last_alloc = NULL;
    }
}

void free(Arena* arena, void* ptr){ }

void* reallocate_raw(Arena* arena, void* ptr, usize old_size, usize size, usize align = Default_Align){
    let pos_before_alloc = arena->pos - old_size;

    if (arena->last_alloc == ptr){
        arena->pos = pos_before_alloc;
        return allocate_raw_non_zero(arena, size, align);
    }

    let new_ptr = cast(u8*)allocate_raw_non_zero(arena, size, align);
    if (new_ptr != NULL) { memcpy(new_ptr, ptr, old_size); }

    return new_ptr;
}

Allocator arena_allocator(Arena* arena){
    return {
        .data = arena,
        .procedure = [](void* data, Allocator_Message msg) -> void*{
            let arena = cast(Arena*) data;
            switch(msg.kind){
            case Allocator_Message::Alloc_Zero: 
                return allocate_raw(arena, msg.alloc.size, msg.alloc.align);
            case Allocator_Message::Alloc_Non_Zero: 
                return allocate_raw(arena, msg.alloc.size, msg.alloc.align);
            case Allocator_Message::Free_All: 
                free_all(arena);
                return NULL;
            case Allocator_Message::Free: 
                free(arena, msg.free.ptr);
                return NULL;
            case Allocator_Message::Realloc: 
                reallocate_raw(arena, msg.realloc.ptr, msg.realloc.old_size, msg.realloc.size, msg.realloc.align);
                return NULL;
            }

            return NULL;
        },
    };
}

/*
 * ============
 * Basic stuff
 * ===========
 */

/*
 * ------
 * Tuple
 * ------
 */

template <typename T, typename... Ts>
struct Tuple {
    Tuple(const T& t, const Ts&... ts)
        : value(t)
        , rest(ts...)
    {
    }

    constexpr int size() const { return 1 + rest.size(); }

    T value;
    Tuple<Ts...> rest;
};
template <typename T>
struct Tuple<T> {
    Tuple(const T& t)
        : value(t)
    {
    }

    constexpr int size() const { return 1; }

    T value;
};

template <size_t i, typename T, typename... Ts>
struct nthType : nthType<i-1, Ts...> {
    static_assert(i < sizeof...(Ts) + 1, "index out of bounds");
};

template <typename T, typename... Ts>
struct nthType<0, T, Ts...> { T value; };

template <size_t i>
struct getter {
    template <typename... Ts>
    static decltype(nthType<i, Ts...>::value)& get(Tuple<Ts...>& t) {
        return getter<i-1>::get(t.rest);
    }
};
template <>
struct getter<0> {
    template <typename T, typename... Ts>
    static T& get(Tuple<T, Ts...>& t) {
        return t.value;
    }
};

template <size_t i, typename... Ts>
decltype(nthType<i, Ts...>::value)& get(Tuple<Ts...>& t) {
    return getter<i>::get(t);
}

/*
 * ----
 * Pair
 * ----
 */

template <class T1, class T2>
struct Pair{
    T1 x;
    T2 y;
};

/* --------
 * Iterator
 * --------
 */

/*
interface Iter{
    bool next(Iter* iter);
};
 */
#define For_Each(iter)     for (let it = iter; next(&it);)
#define For_Each_Ref(iter) for (let it = &iter; next(it);)

template <bool inclusive>
struct Range_Iter{
    int low = 0;
    int high = 0;

    int value = 0;
};

template <bool inclusive>
bool next(Range_Iter<inclusive>* range){
    range->value = range->low;
    // Is this done optimizied at compile time?
    if (inclusive){
        if (range->low <= range->high){
            range->low += 1;
            return true;
        }

    } else {
        if (range->low < range->high){
            range->low += 1;
            return true;
        }
    }
    return false;
}

inline
Range_Iter<false> range(int el, int high){
    return {el, high};
}

inline
Range_Iter<true> range_inclusive(int el, int high){
    return {el, high};
}

/*
 * -----
 * Slice
 * -----
 */

template <class T>
struct Slice{
    T*      buffer = NULL;
    usize   len = 0;
    Allocator allocator;

    T& operator[](usize idx){ return this->buffer[idx]; }
};

template <class T>
Slice<T> create_slice(Allocator allocator, usize len){
    return {
        .buffer = allocate_array<T>(allocator, len),
        .len = len,
        .allocator = allocator,
    };
}

template <class T>
Slice<T> create_slice_non_zero(Allocator allocator, usize len){
    return {
        .buffer = allocate_array_non_zero<T>(allocator, len),
        .len = len,
        .allocator = allocator,
    };
}

template <class T>
void destroy(Slice<T> slice){
    free(slice.allocator, slice.buffer);
}

template <class T>
Slice<T> empty_slice(){
    return {NULL, 0, {}};
}

/*
 * This used so any data can be easily 
 * aliasad as an slice of bytes
 * e. g.
 *
 * ```cpp
 *  u32 a = 12;
 *  let bytes = bytes_of(&a); // it is the bytes of a;
 * ```
 */
template<class T>
Slice<u8> bytes_of(T* data){
    return {
        .buffer  = (u8*) data,
        .len = sizeof(T),
        .allocator = {},
    };
}

/*

interface Sliceable{
    template<class T>
    Slice<T> slice_from(Sliceable, usize pos, usize len){

    template<class T>
    Slice<T> slice_from(Sliceable, usize pos){
    
    template<class T>
    Option<Slice<T>> slice_from_safe(Sliceable, usize pos, usize len){
    
    template<class T>
    Option<Slice<T>> slice_from_safe(Sliceable, usize pos){
}
 
*/

template<class T>
Slice<T> slice_from(Slice<T> slice, usize pos, usize len){
    debug_assert(pos + len <= slice.len && "slice_from");
    return {
        .buffer = slice.buffer + pos,
        .len = len,
        .allocator = slice.allocator,
    };
}

template<class T>
Slice<T> slice_from(Slice<T> slice, usize pos = 0){
    debug_assert(pos <= slice.len && "slice_from");
    return {
        .buffer = slice.buffer + pos,
        .len = slice.len - pos,
        .allocator = slice.allocator,
    };
}

template<class T>
Option<Slice<T>> slice_from_safe(Slice<T> slice, usize pos, usize len){
    if (pos + len > slice.len) return {{}, false};
    return {{
        .buffer = slice.buffer + pos,
        .len = len,
        .allocator = slice.allocator,
    }, true};
}

template<class T>
Option<Slice<T>> slice_from_safe(Slice<T> slice, usize pos = 0){
    if (pos > slice.len) return {{}, false};
    return {{
        .buffer = slice.buffer + pos,
        .len = slice.len - pos,
        .allocator = slice.allocator,
    }, true};
}

template<class T>
struct Slice_Iter{
    T  value; 
    T* value_ptr;
    usize idx = 0;
    usize count = 0;

    T* buffer = NULL;
    usize len = 0;
};

template<class T>
bool next(Slice_Iter<T>* it){
    it->idx = it->count;
    if (it->count >= it->len) return false;

    it->count    += 1;
    it->value     = it->buffer[it->idx];
    it->value_ptr = &it->buffer[it->idx];

    return true;
}

template <class T>
Slice_Iter<T> iter(Slice<T> slice){
    return {
        .value = 0, 
        .value_ptr = NULL,
        .idx = 0,
        .count = 0,

        .buffer = slice.buffer,
        .len    = slice.len,
    };
}

template <class T>
Slice<T> copy(Slice<T> slice, Allocator allocator){
    let new_slice = create_slice_non_zero<T>(allocator, slice.len);
    For_Each(iter(new_slice)){
        *it.value_ptr = slice[it.idx];
    }
    
    return new_slice;
}

//Todo(Ferenc): Add remaining procedures
template<class T>
inline
Slice<T> slice_from(T* buffer, usize pos, usize len){
    return {
        .buffer = buffer + pos,
        .len = len,
    };
}

template <class T>
inline
void typed_memset(Slice<T> slice, T value){
    typed_memset(slice.buffer, value, slice.len);
}

/*
 * -----
 * Array
 * -----
 */

template <class T>
struct Array{
    T*    buffer;
    usize len;
    usize capacity;
    Allocator allocator;

    T& operator[](usize idx){
        return buffer[idx];
    }
};

template <class T>
Array<T> create_array(Allocator allocator, usize capacity = 1, usize len = 0){
    assert(capacity >= len);
    
    return Array<T>{
        .buffer    = allocate_array<T>(allocator, capacity),
        .len       = len,
        .capacity  = capacity,
        .allocator = allocator,
    };
}

template <class T>
void destroy(Array<T> array){
    free(array.allocator, array.buffer);
}


template <class T>
inline
bool empty(Array<T> array){
    return array.len == 0;
}

template <class T>
void resize(Array<T>* array, usize capacity){
    array->buffer = reallocate(array->allocator, array->buffer, array->capacity, capacity);
    array->capacity = capacity;
}


template <class T>
usize append(Array<T>* array, T element){
    if (array->len + 1 > array->capacity){ resize(array, array->capacity * 2); }

    let pos = array->len;
    array->buffer[pos] = element;
    array->len += 1;
    return pos;
}

template <class T>
inline
void clear(Array<T>* array){
    array->len = 0;
}

template <class T>
T pop(Array<T>* array){
    array->len--;
    return array->buffer[array->len];
}

template <class T>
Option<T> pop_safe(Array<T>* array){
    if (array->len != 0) {
        array->len--;
        return {array->buffer[array->len], true};
    }

    return {{}, false};
}

template <class T>
inline
void push(Array<T>* array, T element){
    append(array, element); 
}


template <class T>
inline
Slice_Iter<T> iter(Array<T> array){
    return {
        .buffer = array.buffer,
        .len    = array.len,
    };
}

template<class T>
Slice<T> slice_from(Array<T> array, usize pos, usize len){
    debug_assert(pos + len <= array.len && "slice_from");
    return {
        .buffer = array.buffer + pos,
        .len = len,
        .allocator = array.allocator,
    };
}

template<class T>
inline
Slice<T> slice_from(Array<T> array, usize pos = 0){ 
    return slice_from(slice_from(array, 0, array.len), pos); 
}

template<class T>
inline
Pair<Slice<T>, bool> slice_from_safe(Array<T> array, usize pos, usize len){
    return slice_from_safe(slice_from(array), pos, len);
}

template<class T>
inline
Pair<Slice<T>, bool> slice_from_safe(Array<T> array, usize pos = 0){
    return slice_from_safe(slice_from(array), pos);
}

/*
 * ----
 * Rune
 * ----
 */

// Rune is on 4 bytes representing a unicode code point
using Rune = u32;

usize encode_to_utf8(Rune rune, u8* ptr){
    u8 rune_least1_byte = 0;
    u8 rune_least2_byte = 0;
    u8 rune_least3_byte = 0;

    if (rune <= 0x7f) {
        ptr[0] = 0;
        ptr[0] |= (u8)rune;
        return 1;
    }

#if ML_BYTE_ORDER == ML_LITTLE_ENDIAN
    rune_least1_byte = ((u8*)&rune)[0];
    rune_least2_byte = ((u8*)&rune)[1];
#endif
#if ML_BYTE_ORDER == ML_BIG_ENDIAN
    rune_least1_byte = ((u8*)&rune)[3];
    rune_least2_byte = ((u8*)&rune)[2];
#endif

    if (rune <= 0x07FF) {
        let bit_6 =  rune_least1_byte & 0b111111;
        let bit_2 = (rune_least1_byte >> 6) & 0b11;
        let bit_3 =  rune_least2_byte & 0b111;
        ptr[0]    = (0b110 << 5) | (bit_3 << 2) | bit_2;
        ptr[1]    = (0b10  << 6) |  bit_6;

        return 2;
    }

    if (rune <= 0xFFFF){
        let bit_6_from_least1       =  rune_least1_byte & 0b111111;
        let bit_2_from_least1       = (rune_least1_byte >> 6) & 0b11;
        let bit_least_4_from_least2 =  rune_least2_byte & 0b1111;
        let bit_4_from_least2       = (rune_least2_byte >> 4) & 0b1111;
        ptr[0] = (0b1110 << 4) |  bit_4_from_least2;
        ptr[1] = (0b10   << 6) | (bit_least_4_from_least2 << 2) | bit_2_from_least1;
        ptr[2] = (0b10   << 6) |  bit_6_from_least1;
        return 3;
    }

#if ML_BYTE_ORDER == ML_LITTLE_ENDIAN
    rune_least3_byte = ((u8*)&rune)[2];
#endif
#if ML_BYTE_ORDER == ML_BIG_ENDIAN
    rune_least3_byte = ((u8*)&rune)[1];
#endif
    
    let bit_6_from_least1       =  rune_least1_byte       & 0b111111;
    let bit_2_from_least1       = (rune_least1_byte >> 6) & 0b11;
    let bit_least_4_from_least2 =  rune_least2_byte       & 0b1111;
    let bit_4_from_least2       = (rune_least2_byte >> 4) & 0b1111;
    let bit_2_from_least3       =  rune_least3_byte       & 0b11;
    let bit_3_from_least3       = (rune_least3_byte >> 2) & 0b111;
    ptr[0] = (0b11110 << 3) |  bit_3_from_least3;
    ptr[1] = (0b10    << 6) | (bit_2_from_least3 << 4)       | bit_4_from_least2;
    ptr[2] = (0b10    << 6) | (bit_least_4_from_least2 << 2) | bit_2_from_least1;
    ptr[3] = (0b10    << 6) | bit_6_from_least1;
    return 4;
}


usize encode_to_utf8(Rune rune, Slice<u8> slice){
    return encode_to_utf8(rune, slice.buffer);
}

inline
usize rune_len_in_byes_from_first_byte(u8 b){
    if ((b >> 7) == 0)       { return 1; }
    if ((b >> 5) == 0b110)   { return 2; }
    if ((b >> 4) == 0b1110)  { return 3; }
    if ((b >> 3) == 0b11110) { return 4; }

    panic("the byte is not utf8 encoded");
    UNREACHABLE;
}

inline
Pair<Rune, usize> decode_rune_utf8(u8* encoded){
    Rune rune = {0};
    u32 b = encoded[0];
    usize size = 0;

    if      ((b >> 7) == 0)       {
        rune = rune | b;
        size = 1;
    }
    else if ((b >> 5) == 0b110)   {
        let bit_6 =  encoded[1]       & 0b111111;
        let bit_5 =  encoded[0]       & 0b11111;
        rune = (bit_5 << 6) | bit_6;
        size = 2;
    }
    else if ((b >> 4) == 0b1110)  {
        let bit_6_2 =  encoded[2]       & 0b111111;
        let bit_6_1 =  encoded[1]       & 0b111111;
        let bit_4   =  encoded[0]       & 0b1111;
        rune = (bit_4 << 12) | (bit_6_1 << 6) | bit_6_2;
        size = 3;
    }
    else if ((b >> 3) == 0b11110) {
        let bit_6_3 =  encoded[3]       & 0b111111;
        let bit_6_2 =  encoded[2]       & 0b111111;
        let bit_6_1 =  encoded[1]       & 0b111111;
        let bit_3   =  encoded[0]       & 0b1111;
        rune = (bit_3 << 18) | (bit_6_1 << 12) | (bit_6_2 << 6) | bit_6_3;
        size = 4;
    }

    return {rune, size};
}

inline
Pair<Rune, usize> decode_rune_utf8(Slice<u8> encoded){
    return decode_rune_utf8(encoded.buffer);
}

/*
 * It reads the first rune from the c_str
 */
inline
Rune rune_from_c_str(const char* c_str){
    return decode_rune_utf8(cast(u8*) c_str).x;
}


inline
Rune rune_from(char chr){
    return chr;
}

inline
bool rune_whitespace(Rune rune){
    switch (rune) {
        case 0x000A: return true;
        case 0x0020: return true;
        case 0x00A0: return true;
        case 0x1680: return true;
        case 0x202F: return true;
        case 0x205F: return true;
        case 0x3000: return true;
        case 0x2028: return true;
        case 0x2029: return true;
    }
    if (0x2000 <= rune && rune <= 0x200A) return true;
    return false;
}

inline
bool rune_ascii_digit(Rune rune){
    return '0' <= rune && rune <= '9';
}

/*
 * ------
 * String
 * ------
 */

/*
    Strings are UTF-8 encoded
*/
struct String{
    Slice<u8> slice;

    u8& operator[](usize idx){ return this->slice[idx]; }
};

inline 
String create_string(Allocator allocator, usize len){
    return {create_slice<u8>(allocator, len)};
}

inline 
String create_string_non_zero(Allocator allocator, usize len){
    return {create_slice<u8>(allocator, len)};
}

inline
void destroy(String str){
    destroy(str.slice);
}

inline
String empty_string(){
    return {empty_slice<u8>()};
}

usize len(String str){
    return str.slice.len;
}

inline
Slice<u8> raw(String str){
    return str.slice;
}

inline
String alias(const char* c_str){
    return String{
        .slice = {
            .buffer = (u8*)c_str,
            .len = strlen(c_str),
            .allocator = {},
        }
    };
}

inline
String alias(const char* c_str, usize len){
    return String{
        .slice = {
            .buffer = (u8*)c_str,
            .len = len,
            .allocator = {},
        }
    };
}


/*
   It aliases it, does not copy
*/
inline
String string_from(Slice<u8> slice){
    return String{
        .slice = slice,
    };
}

inline
String string_from(const char* c_str, Allocator allocator){
    let len = strlen(c_str);
    let str = create_string(allocator, len);
    memcpy((void*)str.slice.buffer, (void*)c_str, len);
    return str;
}

inline
String copy(String str, Allocator allocator){
    let copyed = create_string(allocator, len(str));
    memcpy(copyed.slice.buffer, str.slice.buffer, len(str));
    return copyed;
}

inline
Slice_Iter<u8> byte_iter(String str){
    return iter(str.slice);
}

inline
String substr(String str, usize pos, usize len){
    return string_from(slice_from(str.slice, pos, len));
}

inline
String substr(String str, usize pos){
    return string_from(slice_from(str.slice, pos));
}

inline
Option<String> substr_safe(String str, usize pos, usize len){
    let [s, ok] = slice_from_safe(str.slice, pos, len);
    if (!ok) return {{}, false};
    return {string_from(s), true};
}

inline
Option<String> substr_safe(String str, usize pos){
    let [s, ok] = slice_from_safe(str.slice, pos);
    if (!ok) return {{}, false};
    return {string_from(s), true};
}

inline
char* cstr(String str, Allocator allocator){
    let c = allocate_array_non_zero<char>(allocator, len(str) + 1); 
    For_Each(byte_iter(str)){
        c[it.idx] = it.value;
    }
    c[len(str)] = '\0';
    return c;
}

/*
 * The length of the string
 * mesaured by the nr of runes in it
 *
 * O(len(str))
 *
 * https://www.rfc-editor.org/rfc/rfc3629
 */
inline
usize rune_len(String str){
    if (len(str) == 0) return 0;
    usize length = 0;   
    usize i = 0;

    while (i < len(str)) {
        let b = str.slice[i];
        if      ((b >> 7) == 0)       i += 1;
        else if ((b >> 5) == 0b110)   i += 2;
        else if ((b >> 4) == 0b1110)  i += 3;
        else if ((b >> 3) == 0b11110) i += 4;
        length++;
    }
    return length;
}

struct String_Rune_Iter{
    u8* buffer;
    usize len;

    usize count = 0;
    usize byte_idx   = 0; 
    usize read_len = 0;
    Rune  rune = 0;
    usize rune_byte_len = 0; // Last decoded runes len
};

inline
bool next(String_Rune_Iter* iter){
    if (iter->read_len >= iter->len) return false;
    iter->byte_idx = iter->read_len;
    let [rune, rune_byte_len] = decode_rune_utf8(iter->buffer + iter->byte_idx);
    iter->read_len += rune_byte_len;
    iter->rune = rune;
    iter->count++;
    iter->rune_byte_len = rune_byte_len;
    return true;
}

inline
String_Rune_Iter iter(String str){
    return {
        .buffer = str.slice.buffer,
        .len    = len(str),
        .count  = 0,
    };
}


String errno_to_str(Errno e){
    return alias(errno_to_c_str(e));
}

bool equal(String str1, String str2){
    For_Each(byte_iter(str1)){
        if (it.value != str2[it.idx]){
            return false;
        }
    }

    return true;
}

/*
   -----
   Queue
   -----
*/

template <class T>
struct Queue{
    T* buffer = NULL;
    usize capacity = 0;
    Allocator allocator;

    usize write = 0;
    usize read  = 0;
    usize len = 0;
};

template <class T>
Queue<T> create_queue(Allocator allocator, usize capacity = 1){
    return {
        .buffer = allocate_array<T>(allocator, capacity),  
        .capacity = capacity,
        .allocator = allocator,
    };
}

template <class T>
void destroy(Queue<T> q){
    free(q.allocator, q.buffer);
}

template <class T>
void resize(Queue<T>* q, usize capacity){
    let buffer = allocate_array<T>(q->allocator, capacity);

    for (usize i = 0; i < q->len; i ++){
        let pos = (q->read + i) % q->capacity;
        buffer[i] = q->buffer[pos];
    }

    q->read = 0;
    q->write = q->len;
    q->capacity = capacity;
    
    free(q->allocator, q->buffer);
    q->buffer = buffer;
}

template <class T>
void push(Queue<T>* q, T el){
    if (q->len + 1 > q->capacity){
        resize(q, q->capacity  * 2);
    }

    q->buffer[q->write] = el;
    q->write = (q->write + 1) % q->capacity;
    q->len += 1;
}

template <class T>
T pop(Queue<T>* q){
    let el = q->buffer[q->read];
    q->read = (q->read + 1) % q->capacity;
    q->len -= 1;

    return el;
}


template <class T>
Option<T> pop_safe(Queue<T>* q){
    if (q->len == 0) {
        return {{}, false};
    }

    let el = q->buffer[q->read];
    q->read = (q->read + 1) % q->capacity;
    q->len -= 1;

    return {el, true};
}

template <class T>
inline
bool empty(Queue<T> q){
    return q.len == 0;
}

/*
   -----
   Stack
   -----
*/

template <class T>
struct Stack{
    Array<T> array;
};

template <class T>
Stack<T> create_stack(Allocator allocator, usize capacity = 1){
    return Stack<T>{
        .array = create_array<T>(allocator, capacity, 0),
    };
}

template <class T>
inline
void destroy(Stack<T> s){
    destroy(s.array);
}

template <class T>
inline
void push(Stack<T>* s, T el){
    append(&s->array, el); 
}

template <class T>
inline
T pop(Stack<T>* s){
    return pop(&s->array);
}

template <class T>
inline
Option<T> pop_safe(Stack<T>* s){
    return pop_safe(&s->array);
}

template <class T>
inline
bool empty(Stack<T> s){
    return empty(s.array);
}

//Todo(Ferenc): add bucket array
