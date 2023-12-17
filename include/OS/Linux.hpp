#pragma  once

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cerrno>

#include "../Core.hpp"
#include "Shared.hpp" // IWYU pragma: export


Option<Socket> tcp_start_listener(Address address, usize listen_nr = 50, bool non_blocking = false){
    int type = SOCK_STREAM;
    if (non_blocking) type |= SOCK_NONBLOCK;
    Socket s;
    s.address = address;
    s.socket_descriptor = socket(AF_INET, type, 0);
    if (s.socket_descriptor < 0){
        return {false};
    }

    int opt = 1;
    let optname = SO_REUSEADDR | SO_REUSEPORT;
    let ret = setsockopt(s.socket_descriptor, SOL_SOCKET, optname, &opt, sizeof(opt));
    if (ret != 0){
        return {false};
    }


    struct sockaddr_in sockaddress;
    sockaddress.sin_family = AF_INET;
    sockaddress.sin_addr.s_addr = htonl(*(u32*)address.ip);
    sockaddress.sin_port = htons(address.port);
    ret = bind(s.socket_descriptor, (struct sockaddr*)&sockaddress, sizeof(sockaddress));
    if (ret < 0){
        return {false};
    }

    ret = listen(s.socket_descriptor, listen_nr);
    if (ret < 0){
        return {false};
    }

    return {true, s};
}

void close(Socket socket){
    shutdown(socket.socket_descriptor, SHUT_RDWR);
    ::close(socket.socket_descriptor);
}

namespace Private{

Address address_from_sockaddr_in(struct sockaddr_in sockaddress){
    Address adr;
    memcpy(adr.ip, &sockaddress.sin_addr.s_addr, sizeof(u32));
    adr.port = sockaddress.sin_port;
    return adr;
}

};

Option<Socket> tcp_accept(Socket listener){
    struct sockaddr_in sockaddress;
    socklen_t len = sizeof(sockaddress); // WHYYYYYYYYYYYYYYYYYyy
    
    let new_socket_descriptor = ::accept(listener.socket_descriptor, (struct sockaddr*)&sockaddress, &len);   
    if (new_socket_descriptor < 0) {
        return {false};
    }

    return {true, {
        .socket_descriptor = new_socket_descriptor,
        .address = Private::address_from_sockaddr_in(sockaddress),
    }};
}


Option<Socket> tcp_dial(Address address, bool non_blocking = false){
    int type = SOCK_STREAM;
    if (non_blocking) type |= SOCK_NONBLOCK;
    Socket s;
    s.address = address;
    s.socket_descriptor = socket(AF_INET, type, 0);
    if (s.socket_descriptor < 0){
        return {false};
    }

    struct sockaddr_in sockaddress;
    sockaddress.sin_family = AF_INET;
    sockaddress.sin_addr.s_addr = *(u32*)address.ip;
    sockaddress.sin_port = htons(address.port);
    let ret = connect(s.socket_descriptor, (struct sockaddr*)&sockaddress, sizeof(sockaddress));
    if (ret < 0){
        return {false};
    }

    return {true, s};
}

bool tcp_send(Socket socket, Array<u8> data){
    // Todo(Ferenc): implement better errors
    let ret = ::send(socket.socket_descriptor, data.ptr, data.size, 0);
    return ret <= 0;
}

Pair<ssize, int> tcp_receive(Socket socket, Array<u8>* data){
    let nr = recv(socket.socket_descriptor, data->ptr, data->size, 0);
    let err = 0;
    if (nr < 0){
        err = errno;
    }
    data->size = nr;
    return {nr, err};
}



struct File{
    enum Mode{
        Read,
        Write,  // Creates if doesn't exists
        Append, // Creates if doesn't exists
    };
    int fd;
};

Option<File> file_open_c_str(const char* path, File::Mode mode = File::Mode::Read){
    File file;

    switch (mode) {
    case File::Mode::Read:{
        file.fd = open(path, O_RDONLY);
    }break;
    case File::Mode::Write:{
        file.fd = open(path, O_RDONLY | O_CREAT);
    }break;
    case File::Mode::Append:{
        file.fd = open(path, O_RDONLY | O_CREAT | O_APPEND);
    }break;
    }

    if (file.fd < 0){
        return {false, file};
    }

    return {true, file};
}


Option<File> file_open(String str, Allocator allocator, File::Mode mode = File::Mode::Read){
    let cstr = string_c_str(str, allocator);
    if (cstr.error != Core_Error::Correct) return {false};
    return file_open_c_str(cstr.value, mode);
}


usize file_read(File file, Array<u8>* array){
    let nr = read(file.fd, array->ptr, array->size);
    array->size = nr;
    return nr;
}


usize file_write(File file, Array<u8> array){
    return write(file.fd, array.ptr, array.size);
}


usize file_size(File file){
    struct stat s;
    fstat(file.fd, &s);
    return s.st_size;
}


Array<u8> file_read_all(File file, Allocator allocator){
    let size = file_size(file);
    let array = array_create<u8>(allocator, size);
    file_read(file, &array);
    return array;
}
