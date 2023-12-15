#include "Core.hpp"
#include "OS/Linux.hpp"
#include "Utf8_Scanner.hpp"
#include "OS.hpp"

const u8 SP = 32;
const u8 CR = 13;
const u8 LF = 10;
const u8 CRLF[2] = {CR, LF};

const let Content_Type_Html = String_alias("plain/html");

enum struct Request_Method {
    OPTIONS = 0,
    GET,                    
    HEAD,                   
    POST,                   
    PUT,                    
    DELETE,                 
    TRACE,                  
    CONNECT,                
};

struct Request{
    Request_Method           method;
    String                   uri;
    String                   body;
    Hash_Map<String, String> headers;
    Socket client;
};

struct Response{
    usize status_code = 500;
    String description;
    Hash_Map<String, String> headers;
    String body;
};

Response response_create(Allocator allocator){
    return {
        .headers = Hash_Map_create<String, String>(allocator, 10, String_hash, equal),
    };
}

/*
 * It sets the status code and the description if it is known
 */
void response_set_status_code(Response* response, usize status_code){
    response->status_code = status_code;

    switch (status_code) {
    case 100: response->description = String_alias("Continue"); break;
    case 101: response->description = String_alias("Switching"); break;
    case 200: response->description = String_alias("OK"); break;
    case 201: response->description = String_alias("Created"); break;
    case 202: response->description = String_alias("Accepted"); break;
    case 203: response->description = String_alias("Non"); break;
    case 204: response->description = String_alias("No"); break;
    case 205: response->description = String_alias("Reset"); break;
    case 206: response->description = String_alias("Partial"); break;
    case 300: response->description = String_alias("Multiple"); break;
    case 301: response->description = String_alias("Moved"); break;
    case 302: response->description = String_alias("Found"); break;
    case 303: response->description = String_alias("See"); break;
    case 304: response->description = String_alias("Not"); break;
    case 305: response->description = String_alias("Use"); break;
    case 307: response->description = String_alias("Temporary"); break;
    case 308: response->description = String_alias("Permanent"); break;
    case 400: response->description = String_alias("Bad"); break;
    case 401: response->description = String_alias("Unauthorized"); break;
    case 402: response->description = String_alias("Payment"); break;
    case 403: response->description = String_alias("Forbidden"); break;
    case 404: response->description = String_alias("Not"); break;
    case 405: response->description = String_alias("Method"); break;
    case 406: response->description = String_alias("Not"); break;
    case 407: response->description = String_alias("Proxy"); break;
    case 408: response->description = String_alias("Request"); break;
    case 409: response->description = String_alias("Conflict"); break;
    case 410: response->description = String_alias("Gone"); break;
    case 411: response->description = String_alias("Length"); break;
    case 412: response->description = String_alias("Precondition"); break;
    case 413: response->description = String_alias("Content"); break;
    case 414: response->description = String_alias("URI"); break;
    case 415: response->description = String_alias("Unsupported"); break;
    case 416: response->description = String_alias("Range"); break;
    case 417: response->description = String_alias("Expectation"); break;
    case 421: response->description = String_alias("Misdirected"); break;
    case 422: response->description = String_alias("Unprocessable"); break;
    case 426: response->description = String_alias("Upgrade"); break;
    case 500: response->description = String_alias("Internal"); break;
    case 501: response->description = String_alias("Not"); break;
    case 502: response->description = String_alias("Bad"); break;
    case 503: response->description = String_alias("Service"); break;
    case 504: response->description = String_alias("Gateway"); break;
    case 505: response->description = String_alias("HTTP"); break;
    }
}

void response_set_body(Response* response, Allocator allocator, String body, String content_type = Content_Type_Html){
    response->body = body;

    let builder = String_Builder_create(allocator);
    defer(destroy(&builder));

    add_uint(&builder, body.size);

    set(&response->headers, String_alias("Content-Length"), build(builder, allocator));
    //set(&response->headers, String_alias("Content-Length"), build(builder, allocator));
}


struct HTTP_Server{
    Socket listener;
};

Option<HTTP_Server> HTTP_Server_start(Address address){
    let listener = tcp_start_listener(address);
    return_none(listener);

    return {true, HTTP_Server{
        .listener = listener.value,
    }};
}

void close(HTTP_Server server){
    close(server.listener);
}

/*
 * It blocks until a request comes in
 */
Option<Request> poll_request(HTTP_Server server, Allocator allocator){
    //TODO(Ferenc): More error handleing

    let client_opt = tcp_accept(server.listener);
    return_none(client_opt);
    let client = client_opt.value;
    
    Request request;
    request.client = client;
    request.headers = Hash_Map_create<String, String>(allocator, 10, String_hash, equal);

    struct Scanner_Data{
        u8 buffer[1 * KB] = {0};
        Socket client;
    } scanner_data;
    scanner_data.client = client;


    Utf8_Scanner scanner{
        .user_data = &scanner_data,
        .scan_proc = [](void* user_data) -> Array<u8>{
            let scanner_data = (Scanner_Data*)user_data;
            let buffer = Array_alias(scanner_data->buffer, 1 * KB);
            let [nr, err] = tcp_receive(scanner_data->client, buffer);
            if (nr <= 0){ return Array_empty<u8>(); }
            return slice(buffer, 0, nr);
        }
    };


    let correct = [&](){
        next(&scanner);

        while (scanner.current != SP) {
            next(&scanner);
        }

        // I can do this slice because the scanner_data.buffer starts out with enough data
        // tho it can happen that if is received by multiple tcp_recived
        let method_str = (String) slice(scanner.array, 0, scanner.idx);
        if (equal_c_str(method_str, "OPTIONS")){
            request.method = Request_Method::OPTIONS;   
        } else if (equal_c_str(method_str, "GET")){
            request.method = Request_Method::GET;   
        } else if (equal_c_str(method_str, "HEAD")){
            request.method = Request_Method::HEAD;   
        } else if (equal_c_str(method_str, "POST")){
            request.method = Request_Method::POST;   
        } else if (equal_c_str(method_str, "DELETE")){
            request.method = Request_Method::DELETE;   
        } else if (equal_c_str(method_str, "TRACE")){
            request.method = Request_Method::TRACE;   
        } else if (equal_c_str(method_str, "CONNECT")){
            request.method = Request_Method::CONNECT;   
        } 

        let builder = String_Builder_create(allocator);
        defer(destroy(&builder));

        next(&scanner);
        while (scanner.current != SP) {
            add_rune(&builder, scanner.current);
            next(&scanner);
        }

        request.uri = build(builder, allocator);

        clear(&builder);
        next(&scanner);
        while (scanner.current != CR) {
            add_rune(&builder, scanner.current);
            next(&scanner);
        }
        let http_version = build_alias(builder);
        if (!equal_c_str(http_version, "HTTP/1.1")){
            return false;
        }
        next(&scanner); 
        if (scanner.current != LF) {
            return false;
        }

        next(&scanner);
        while (scanner.current != CR) {
            //scan 1 header
            clear(&builder);
            while (scanner.current != ':') {
                add_rune(&builder, scanner.current);
                next(&scanner);
            }
            let key = build(builder, allocator);

            if (next(&scanner) != SP){ return false; }

            clear(&builder);
            next(&scanner);
            while (scanner.current != CR) {
                add_rune(&builder, scanner.current);
                next(&scanner);
            }
            let value = build(builder, allocator);
            set(&request.headers, key, value);
        
            if (scanner.current != CR){ return false; }
            if (next(&scanner)  != LF){ return false; }
            next(&scanner);
        }
        if (next(&scanner) != LF){ return false; }

        //Content-Length or Transfer-Encoding
        let content_lenght = get(request.headers, String_alias("Content-Length"));
        if (content_lenght.some){
            let lenght = content_lenght.value;
            //TODO(Ferenc): Read body
        }

        return true;
    }();

    if (!correct){
        close(client);
    }

    return {true, request};
}

void respond(Request request, Response response, Allocator allocator){
    let builder = String_Builder_create(allocator);
    defer(destroy(&builder));
    
    add_c_str(&builder, "HTTP/1.1");
    add_c_str(&builder, " ");
    add_uint(&builder, response.status_code);
    add_c_str(&builder, " ");
    add(&builder, response.description);
    add_byte(&builder, CR);
    add_byte(&builder, LF);

    For_Each(iter(response.headers)){
        add(&builder, it.key);
        add_c_str(&builder, ": ");
        add(&builder, it.value);
        add_byte(&builder, CR);
        add_byte(&builder, LF);
    }

    //TODO(Ferenc): Add body
    add_byte(&builder, CR);
    add_byte(&builder, LF);

    add(&builder, response.body);
    tcp_send(request.client, build_alias(builder));
    close(request.client);
};
