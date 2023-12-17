#include "Core.hpp"
#include "Scanner.hpp"
#include "OS.hpp"

const u8 SP = 32;
const u8 CR = 13;
const u8 LF = 10;
const u8 CRLF[2] = {CR, LF};

const let Content_Type_Html = string_alias("text/html");

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
    Array<u8>                body;
    Hash_Map<String, String> headers;
    Socket client;
};

struct Response{
    usize status_code = 500;
    String description;
    Hash_Map<String, String> headers;
    Array<u8> body;
};

Response response_create(Allocator allocator){
    return {
        .headers = hash_map_create<String, String>(allocator, 10, string_hash, string_equal),
    };
}

/*
 * It sets the status code and the description if it is known
 */
void response_set_status_code(Response* response, usize status_code){
    response->status_code = status_code;

    switch (status_code) {
    case 100: response->description = string_alias("Continue"); break;
    case 101: response->description = string_alias("Switching"); break;
    case 200: response->description = string_alias("OK"); break;
    case 201: response->description = string_alias("Created"); break;
    case 202: response->description = string_alias("Accepted"); break;
    case 203: response->description = string_alias("Non"); break;
    case 204: response->description = string_alias("No"); break;
    case 205: response->description = string_alias("Reset"); break;
    case 206: response->description = string_alias("Partial"); break;
    case 300: response->description = string_alias("Multiple"); break;
    case 301: response->description = string_alias("Moved"); break;
    case 302: response->description = string_alias("Found"); break;
    case 303: response->description = string_alias("See"); break;
    case 304: response->description = string_alias("Not"); break;
    case 305: response->description = string_alias("Use"); break;
    case 307: response->description = string_alias("Temporary"); break;
    case 308: response->description = string_alias("Permanent"); break;
    case 400: response->description = string_alias("Bad"); break;
    case 401: response->description = string_alias("Unauthorized"); break;
    case 402: response->description = string_alias("Payment"); break;
    case 403: response->description = string_alias("Forbidden"); break;
    case 404: response->description = string_alias("Not"); break;
    case 405: response->description = string_alias("Method"); break;
    case 406: response->description = string_alias("Not"); break;
    case 407: response->description = string_alias("Proxy"); break;
    case 408: response->description = string_alias("Request"); break;
    case 409: response->description = string_alias("Conflict"); break;
    case 410: response->description = string_alias("Gone"); break;
    case 411: response->description = string_alias("Length"); break;
    case 412: response->description = string_alias("Precondition"); break;
    case 413: response->description = string_alias("Content"); break;
    case 414: response->description = string_alias("URI"); break;
    case 415: response->description = string_alias("Unsupported"); break;
    case 416: response->description = string_alias("Range"); break;
    case 417: response->description = string_alias("Expectation"); break;
    case 421: response->description = string_alias("Misdirected"); break;
    case 422: response->description = string_alias("Unprocessable"); break;
    case 426: response->description = string_alias("Upgrade"); break;
    case 500: response->description = string_alias("Internal"); break;
    case 501: response->description = string_alias("Not"); break;
    case 502: response->description = string_alias("Bad"); break;
    case 503: response->description = string_alias("Service"); break;
    case 504: response->description = string_alias("Gateway"); break;
    case 505: response->description = string_alias("HTTP"); break;
    }
}

/*
 * content_type is not cloned.
 * body is not cloned.
 */
void response_set_body(Response* response, Allocator allocator, Array<u8> body, String content_type = Content_Type_Html){
    response->body = body;

    let builder = string_builder_create(allocator);
    defer(string_builder_destroy(&builder));

    string_builder_add_uint(&builder, body.size);

    hash_map_set(&response->headers, string_alias("Content-Length"), string_builder_build(builder, allocator));
    hash_map_set(&response->headers, string_alias("Content-Type"),   content_type);
}


struct HTTP_Server{
    Socket listener;
};


Option<HTTP_Server> http_server_start(Address address){
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
    //TODO(Ferenc): Better error handleing

    let client_opt = tcp_accept(server.listener);
    return_none(client_opt);
    let client = client_opt.value;
    
    Request request;
    request.client = client;
    request.headers = hash_map_create<String, String>(allocator, 10, string_hash, string_equal);

    struct Scanner_Data{
        u8 buffer[1 * KB] = {0};
        Socket client;
    } scanner_data;
    scanner_data.client = client;

    Scanner scanner{
        .user_data = &scanner_data,
        .scan_proc = [](void* user_data) -> Array<u8>{
            let scanner_data = (Scanner_Data*)user_data;
            let buffer = array_alias(scanner_data->buffer, 1 * KB);
            let [nr, err] = tcp_receive(scanner_data->client, &buffer);
            if (nr <= 0){ return array_empty<u8>(); }
            return buffer;
        }
    };

    let correct = [&](){
        scanner_next(&scanner);

        while (scanner.current != SP) {
            scanner_next(&scanner);
        }

        // I can do this slice because the scanner_data.buffer starts out with enough data
        // tho it can happen that if is received by multiple tcp_recived
        let method_str = (String) array_slice(scanner.array, 0, scanner.idx);
        if (string_equal_c_str(method_str, "OPTIONS")){
            request.method = Request_Method::OPTIONS;   
        } else if (string_equal_c_str(method_str, "GET")){
            request.method = Request_Method::GET;   
        } else if (string_equal_c_str(method_str, "HEAD")){
            request.method = Request_Method::HEAD;   
        } else if (string_equal_c_str(method_str, "POST")){
            request.method = Request_Method::POST;   
        } else if (string_equal_c_str(method_str, "DELETE")){
            request.method = Request_Method::DELETE;   
        } else if (string_equal_c_str(method_str, "TRACE")){
            request.method = Request_Method::TRACE;   
        } else if (string_equal_c_str(method_str, "CONNECT")){
            request.method = Request_Method::CONNECT;   
        } 

        let builder = string_builder_create(allocator);
        defer(string_builder_destroy(&builder));

        scanner_next(&scanner);
        while (scanner.current != SP) {
            string_builder_add_rune(&builder, scanner.current);
            scanner_next(&scanner);
        }

        request.uri = string_builder_build(builder, allocator);

        string_builder_clear(&builder);
        scanner_next(&scanner);
        while (scanner.current != CR) {
            string_builder_add_rune(&builder, scanner.current);
            scanner_next(&scanner);
        }
        let http_version = string_builder_build_alias(builder);
        if (!string_equal_c_str(http_version, "HTTP/1.1")){
            return false;
        }
        scanner_next(&scanner); 
        if (scanner.current != LF) {
            return false;
        }

        scanner_next(&scanner);
        while (scanner.current != CR) {
            //scan 1 header
            string_builder_clear(&builder);
            while (scanner.current != ':') {
                string_builder_add_rune(&builder, scanner.current);
                scanner_next(&scanner);
            }
            let key = string_builder_build(builder, allocator);

            if (scanner_next(&scanner) != SP){ return false; }

            string_builder_clear(&builder);
            scanner_next(&scanner);
            while (scanner.current != CR) {
                string_builder_add_rune(&builder, scanner.current);
                scanner_next(&scanner);
            }
            let value = string_builder_build(builder, allocator);
            hash_map_set(&request.headers, key, value);
        
            if (scanner.current != CR){ return false; }
            if (scanner_next(&scanner)  != LF){ return false; }
            scanner_next(&scanner);
        }
        if (scanner_next(&scanner) != LF){ return false; }


        let content_length = hash_map_get(request.headers, string_alias("Content-Length"));
        if (content_length.some){
            scanner_next(&scanner);

            let try_length = string_parse_uint(content_length.value);
            if (!try_length.some) return false;
            let length = try_length.value;
            request.body = array_create<u8>(allocator, length);

            let scanner_remaining = array_slice_remaining(scanner.array, scanner.idx);
            For_Each(array_iter(scanner_remaining)){
                request.body[it.idx] = it.value;
            }
            let offset = scanner_remaining.size;

            while (offset < length) {
                let buffer = array_alias(scanner_data.buffer, 1 * KB);
                let [nr, err] = tcp_receive(scanner_data.client, &buffer);

                if (nr <= 0) 
                    return false; 

                For_Each(array_iter(buffer)){
                    request.body[offset + it.idx] = it.value;
                }

                offset += buffer.size;
            }
        }

        return true;
    }();

    if (!correct){
        close(client);
    }

    return {correct, request};
}

void respond(Request request, Response response, Allocator allocator){
    let builder = string_builder_create(allocator);
    defer(string_builder_destroy(&builder));
    
    string_builder_add_c_str(&builder, "HTTP/1.1");
    string_builder_add_c_str(&builder, " ");
    string_builder_add_uint(&builder, response.status_code);
    string_builder_add_c_str(&builder, " ");
    string_builder_add(&builder, response.description);
    string_builder_add_byte(&builder, CR);
    string_builder_add_byte(&builder, LF);

    For_Each(hash_map_iter(response.headers)){
        string_builder_add(&builder, it.key);
        string_builder_add_c_str(&builder, ": ");
        string_builder_add(&builder, it.value);
        string_builder_add_byte(&builder, CR);
        string_builder_add_byte(&builder, LF);
    }

    string_builder_add_byte(&builder, CR);
    string_builder_add_byte(&builder, LF);
    string_builder_add(&builder, response.body);

    tcp_send(request.client, string_builder_build_alias(builder));
    close(request.client);
};
