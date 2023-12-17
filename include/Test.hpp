#include "Core.hpp"
#include "Scanner.hpp"

void test_arena_allocator(){
    let distance_in_bytes = [](void* first, void* second) -> usize{
        return (u8*)first - (u8*)second;
    };

    u8 buffer[100];
    let arena = arena_allocator_create(buffer, 100);
    let alloc = arena_allocator_to_interface(&arena);

    let i =  allocator_allocate_unwrap<int>(alloc);
    let i2 = allocator_allocate_unwrap<int>(alloc);
    let i3 = allocator_allocate_unwrap<int>(alloc);
    assert(distance_in_bytes(i2, i)   >= sizeof(int));
    assert(distance_in_bytes(i3, i2)  >= sizeof(int));

    allocator_free_all(alloc);

    while (allocator_allocate_raw(alloc, 1).error == Core_Error::Correct){ }

    allocator_free_all(alloc);
    {
        let i =  allocator_allocate_unwrap<int>(alloc);
        let i2 = allocator_allocate_unwrap<int>(alloc);
        let i3 = allocator_allocate_unwrap<int>(alloc);
        assert(distance_in_bytes(i2, i)  >= sizeof(int));
        assert(distance_in_bytes(i3, i2)  >= sizeof(int));
    }

};


void test_page_allocator(){
    let mallocator     = mallocator_to_interface();
    let page_allocator = page_allocator_create(mallocator, 100);
    let allocator      = page_allocator_to_interface(&page_allocator);
    defer({
        let ret = allocator_free_all(allocator);
        assert(ret == Core_Error::Correct);
    });

    {
        let try_bytes = allocator_allocate<int>(allocator);
        assert(try_bytes.error == Core_Error::Correct);
    }
    {
        let try_bytes = allocator_allocate_array<int>(allocator, 100);
        assert(try_bytes.error == Core_Error::Correct);
    }
    {
        let try_bytes = allocator_allocate_array<int>(allocator, 200);
        assert(try_bytes.error == Core_Error::Correct);
    }
}

void test_dynamic_array(){
    let allocator = mallocator_to_interface();

    let array = dynamic_array_create<int>(allocator);
    defer(dynamic_array_destroy(&array));

    dynamic_array_append(&array, 0);
    dynamic_array_append(&array, 1);
    dynamic_array_append(&array, 2);
    assert(array.size == 3);
    assert(array[0] == 0);
    assert(array[1] == 1);
    assert(array[2] == 2);

    dynamic_array_clear(&array);
    assert(array.size == 0);
    dynamic_array_append(&array, 12);
    assert(array.size == 1);
    assert(array[0] == 12);

    dynamic_array_clear(&array);
    dynamic_array_append(&array, 69);
    dynamic_array_append(&array, 1);
    dynamic_array_append(&array, 10);
    let sum = 0;
    For_Each(dynamic_array_iter(array)){
        sum += it.value;
    }
    assert(sum == 80);

    For_Each(dynamic_array_iter(array)){
        *it.value_ptr = 12;
    }
    For_Each(dynamic_array_iter(array)){
        assert(it.value == 12);
    }

    dynamic_array_clear(&array);
    dynamic_array_insert(&array, 1, 0);
    dynamic_array_insert(&array, 2, 1);
    dynamic_array_insert(&array, 3, 2);
    dynamic_array_insert(&array, 4, 3);
    assert(array[0] == 1);
    assert(array[1] == 2);
    assert(array[2] == 3);
    assert(array[3] == 4);

    dynamic_array_insert(&array, 12, 0);
    assert(array[0] == 12);
    assert(array[1] == 1);
    assert(array[2] == 2);
    assert(array[3] == 3);
    assert(array[4] == 4);
}

void test_rune(){
    assert(rune_from_c_str("Á") == 0xC1);
    assert(rune_from_c_str("ѫ") == 0x46B);
    assert(rune_from_char('a')  == 'a');
    {
       let rune = rune_from_c_str("Á");
       assert(!rune_whitespace(rune));
    }
    {
       let rune = rune_from_c_str(" ");
       assert(rune_whitespace(rune));
    }
    {
       let rune = rune_from_char('\n');
       assert(rune_whitespace(rune));
    }
    {
        let rune = rune_from_c_str("Á"); 
        u8 bytes[4] = {0};
        rune_encode_to_utf8(rune, bytes);
        let [decoded_rune, len] = rune_decode_from_utf8(bytes);
        assert(decoded_rune == rune);
        assert(len == 2);
    }
}

void test_string(){
    let allocator = mallocator_to_interface();
    {
        let builder   = string_builder_create(allocator);
        defer(string_builder_destroy(&builder));
        
        string_builder_add_char(&builder, 't');
        string_builder_add_char(&builder, 'e');
        string_builder_add_c_str(&builder, "st");

        let str = string_builder_build(builder, allocator);
        defer(string_destroy(&str));

        assert(string_equal(str, str));
        assert(string_equal_c_str(str, "test"));
        assert(!string_equal_c_str(str, "tst"));
        assert(!string_equal_c_str(str, "tstaaaaa"));
    }
    {
        let builder   = string_builder_create(allocator);
        defer(string_builder_destroy(&builder));

        string_builder_add_c_str(&builder, "Unicode ÁÁ");
        string_builder_add_rune(&builder, rune_from_c_str("É"));
        string_builder_add_rune(&builder, rune_from_char('s'));

        let str = string_builder_build(builder, allocator);
        defer(string_destroy(&str));

        assert(string_equal(str, str));
        assert(string_equal_c_str(str, "Unicode ÁÁÉs"));
        assert(string_length_by_rune(str) == 12);
    }

    {
        let str = string_clone_from_cstr("Test string", allocator);
        defer(string_destroy(&str));
        
        assert(string_equal_c_str(str, "Test string"));
        assert(string_starts_with_c_str(str, "Test"));
        assert(string_starts_with_c_str(str, ""));

        let str2 = string_substr_alias_unsafe(str, 0, 4);
        assert(string_equal_c_str(str2, "Test"));
        assert(string_starts_with(str, str2));

        let try_str3 = string_substr_alias(str, 0, 4);
        assert(try_str3.some);
        let str3 = try_str3.value;
        assert(string_equal_c_str(str3, "Test"));

        let try_str4 = string_substr_alias(str, 0, str.size + 1);
        assert(!try_str4.some);

        let str5 = string_substr_alias_unsafe(str, 0, str.size);
        assert(string_equal(str5, str));

    }
    {
        let str = string_alias("ÁÁÁ");
        let rune = rune_from_c_str("Á");
        let count = 0;
        For_Each(string_iter_rune(str)){
            assert(it.rune == rune);
            count++;
        }   
        assert(count == 3);

        let array = string_to_rune_array(str, allocator);
        defer(array_destroy(&array));
        
        assert(array.size == 3);
        For_Each(array_iter(array)){
            assert(it.value == rune);
        }
    }
    {
        let c_str1 = "asdasdasd";
        let str = string_alias(c_str1);
        let c_str2 = string_c_str_unsafe(str, allocator);
        defer(allocator_free_array(allocator, c_str2, 10));

        assert(strcmp(c_str1, c_str2) == 0);
    }
    {
        let str = string_alias("ááá");
        assert(string_length_by_rune(str) == 3);
    }
    {
        let builder   = string_builder_create(allocator);
        defer(string_builder_destroy(&builder));
        
        string_builder_add_uint(&builder, 123);

        let str = string_builder_build(builder, allocator);
        defer(string_destroy(&str));

        assert(string_equal_c_str(str, "123"));
    }
    {
        let builder   = string_builder_create(allocator);
        defer(string_builder_destroy(&builder));
        
        string_builder_add_uint(&builder, 0);

        let str = string_builder_build(builder, allocator);
        defer(string_destroy(&str));

        assert(string_equal_c_str(str, "0"));
    }
}

void test_queue(){
    let allocator = mallocator_to_interface();

    let q = queue_create<int>(allocator);
    defer(queue_destroy(&q));
    assert(queue_empty(q));

    queue_enque(&q, 0);
    queue_enque(&q, 1);
    queue_enque(&q, 2);
    assert(q.size == 3);

    assert(queue_deque_unsafe(&q) == 0);
    assert(queue_deque_unsafe(&q) == 1);
    assert(queue_deque_unsafe(&q) == 2);
    assert(queue_empty(q));


    {
        let try_elem = queue_deque(&q);
        assert(!try_elem.some);
    }

    {
        queue_enque(&q, 12);
        let try_elem = queue_deque(&q);
        assert(try_elem.some);
        let elem = try_elem.value;
        assert(elem == 12);
    }
}

void test_hash_set(){
    let alloc = mallocator_to_interface();
    {
        let set = hash_set_create<int>(alloc);
        defer(hash_set_destroy(&set));

        hash_set_add(&set, 0);
        hash_set_add(&set, 1);
        hash_set_add(&set, 2);
        hash_set_add(&set, 3);
        hash_set_add(&set, 4);

        assert(set.size == 5);
        assert(hash_set_contains(set, 0));
        assert(hash_set_contains(set, 1));
        assert(hash_set_contains(set, 2));
        assert(hash_set_contains(set, 3));
        assert(hash_set_contains(set, 4));
        assert(!hash_set_contains(set, 5));

        assert(hash_set_remove(&set, 4));
        assert(!hash_set_contains(set, 4));
        assert(!hash_set_remove(&set, 4));
        
        assert(set.size == 4);

        hash_set_add(&set, 5);
        assert(hash_set_contains(set, 5));

        assert(set.size == 5);
    
        assert(hash_set_remove(&set, 0));
        assert(hash_set_remove(&set, 1));
        assert(hash_set_remove(&set, 2));
        assert(hash_set_remove(&set, 3));
        assert(hash_set_remove(&set, 5));

        assert(hash_set_empty(set));
    }
    {
        struct Test{
            int a = 0;
        };
        let set = hash_set_create<Test>(
            alloc, 
            1, 
            [](Test lhs) -> usize {return lhs.a;}, 
            [](Test lhs, Test rhs) -> bool { return  lhs.a == rhs.a;}
        );
        defer(hash_set_destroy(&set));

        hash_set_add(&set, {0});
        hash_set_add(&set, {1});
        hash_set_add(&set, {2});
        hash_set_add(&set, {3});
        hash_set_add(&set, {4});

        assert(set.size == 5);
        assert(hash_set_contains(set, {0}));
        assert(hash_set_contains(set, {1}));
        assert(hash_set_contains(set, {2}));
        assert(hash_set_contains(set, {3}));
        assert(hash_set_contains(set, {4}));
        assert(!hash_set_contains(set, {5}));

        assert(hash_set_remove(&set,   {4}));
        assert(!hash_set_contains(set, {4}));
        assert(!hash_set_remove(&set, {4}));
        
        assert(set.size == 4);

        hash_set_add(&set, {5});
        assert(hash_set_contains(set, {5}));

        assert(set.size == 5);
    
        assert(hash_set_remove(&set, {0}));
        assert(hash_set_remove(&set, {1}));
        assert(hash_set_remove(&set, {2}));
        assert(hash_set_remove(&set, {3}));
        assert(hash_set_remove(&set, {5}));

        assert(hash_set_empty(set));
    }
}

void test_hash_map(){
    let alloc = mallocator_to_interface();
    let map = hash_map_create<int, int>(alloc);
    defer(hash_map_destroy(&map));

    assert(hash_map_empty(map));

    {
        hash_map_set(&map, 0, 0);
        assert(hash_map_get_unwrap(map, 0) == 0);
        assert(map.size == 1);

        hash_map_set(&map, 0, 1);
        assert(hash_map_get_unwrap(map, 0) == 1);
        assert(map.size == 1);

        hash_map_set(&map, 0, 123);
        assert(hash_map_get_unwrap(map, 0) == 123);
        assert(map.size == 1);

        assert(!hash_map_get(map, 1).some);
        assert(map.size == 1);

        hash_map_set(&map, -1, -1);
        assert(map.size == 2);
        assert(hash_map_get_unwrap(map, -1) == -1);

        hash_map_remove(&map, -1);
        assert(map.size == 1);
        assert(!hash_map_get(map, -1).some);

        hash_map_remove(&map, 0);
        assert(hash_map_empty(map));
    }   
}


void test_utf8_scanner(){
    {
        struct Data{
            const char* str = "KecskeÁÁ";
            bool returned = false;
        } data;

        let scanner = Scanner{
            .user_data = &data,
            .scan_proc = [](void* ptr) -> Array<u8>{
                let data = (Data*)ptr;
                if (data->returned) {
                    return array_empty<u8>();
                }
                data->returned = true;
                return string_alias(data->str);
            },
        };
        let r = rune_from_c_str("Á");

        scanner_next(&scanner);
        assert(scanner.current == 'K');
        scanner_next(&scanner);
        assert(scanner.current == 'e');
        scanner_next(&scanner);
        assert(scanner.current == 'c');
        scanner_next(&scanner);
        assert(scanner.current == 's');
        scanner_next(&scanner);
        assert(scanner.current == 'k');
        scanner_next(&scanner);
        assert(scanner.current == 'e');
        scanner_next(&scanner);
        assert(scanner.current == r);
        scanner_next(&scanner);
        assert(scanner.current == r);
        scanner_next(&scanner);
        assert(scanner.current == 0);
        scanner_next(&scanner);
        assert(scanner.current == 0);
        scanner_next(&scanner);
    }
    {
        struct Data{
            const char* str = "123";
            bool returned = false;
        } data;

        let scanner = Scanner{
            .user_data = &data,
            .scan_proc = [](void* ptr) -> Array<u8>{
                let data = (Data*)ptr;
                if (data->returned) {
                    return array_empty<u8>();
                }
                data->returned = true;
                return string_alias(data->str);
            },
        };
        scanner_next(&scanner);

        let number = scanner_scan_uint(&scanner).unwrap();
        assert(number == 123);
    }
}
