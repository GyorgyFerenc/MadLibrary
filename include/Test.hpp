#include "Core.hpp"
#include "Scanner.hpp"

void test_arena_allocator(){
    let distance_in_bytes = [](void* first, void* second) -> usize{
        return (u8*)first - (u8*)second;
    };

    u8 buffer[100];
    let arena = Arena_Allocator_create(buffer, 100);
    let alloc = to_interface(&arena);

    let i =  allocate_unwrap<int>(alloc);
    let i2 = allocate_unwrap<int>(alloc);
    let i3 = allocate_unwrap<int>(alloc);
    assert(distance_in_bytes(i2, i)  >= sizeof(int));
    assert(distance_in_bytes(i3, i2)  >= sizeof(int));

    free_all(alloc);

    while (allocate_raw(alloc, 1).error == Core_Error::Correct){ }

    free_all(alloc);
    {
        let i =  allocate_unwrap<int>(alloc);
        let i2 = allocate_unwrap<int>(alloc);
        let i3 = allocate_unwrap<int>(alloc);
        assert(distance_in_bytes(i2, i)  >= sizeof(int));
        assert(distance_in_bytes(i3, i2)  >= sizeof(int));
    }

};


void test_page_allocator(){
    let mallocator     = Mallocator_to_interface();
    let page_allocator = Page_Allocator_create(mallocator, 100);
    let allocator      = to_interface(&page_allocator);
    defer({
        let ret = free_all(allocator);
        assert(ret == Core_Error::Correct);
    });

    {
        let try_bytes = allocate<int>(allocator);
        assert(try_bytes.error == Core_Error::Correct);
    }
    {
        let try_bytes = allocate_array<int>(allocator, 100);
        assert(try_bytes.error == Core_Error::Correct);
    }
    {
        let try_bytes = allocate_array<int>(allocator, 200);
        assert(try_bytes.error == Core_Error::Correct);
    }
}

void test_dynamic_array(){
    let allocator = Mallocator_to_interface();

    let array = Dynamic_Array_create<int>(allocator);
    defer(destroy(&array));

    append(&array, 0);
    append(&array, 1);
    append(&array, 2);
    assert(array.size == 3);
    assert(array[0] == 0);
    assert(array[1] == 1);
    assert(array[2] == 2);

    clear(&array);
    assert(array.size == 0);
    append(&array, 12);
    assert(array.size == 1);
    assert(array[0] == 12);

    clear(&array);
    append(&array, 69);
    append(&array, 1);
    append(&array, 10);
    let sum = 0;
    For_Each(iter(array)){
        sum += it.value;
    }
    assert(sum == 80);

    For_Each(iter(array)){
        *it.value_ptr = 12;
    }
    For_Each(iter(array)){
        assert(it.value == 12);
    }

    clear(&array);
    insert(&array, 1, 0);
    insert(&array, 2, 1);
    insert(&array, 3, 2);
    insert(&array, 4, 3);
    assert(array[0] == 1);
    assert(array[1] == 2);
    assert(array[2] == 3);
    assert(array[3] == 4);

    insert(&array, 12, 0);
    assert(array[0] == 12);
    assert(array[1] == 1);
    assert(array[2] == 2);
    assert(array[3] == 3);
    assert(array[4] == 4);
}

void test_rune(){
    assert(from_c_str("Á") == 0xC1);
    assert(from_c_str("ѫ") == 0x46B);
    assert(from_char('a')  == 'a');
    {
       let rune = from_c_str("Á");
       assert(!whitespace(rune));
    }
    {
       let rune = from_c_str(" ");
       assert(whitespace(rune));
    }
    {
       let rune = from_char('\n');
       assert(whitespace(rune));
    }
    {
        let rune = from_c_str("Á"); 
        u8 bytes[4] = {0};
        encode_to_utf8(rune, bytes);
        let [decoded_rune, len] = decode_from_utf8(bytes);
        assert(decoded_rune == rune);
        assert(len == 2);
    }
}

void test_string(){
    let allocator = Mallocator_to_interface();
    {
        let builder   = String_Builder_create(allocator);
        defer(destroy(&builder));
        
        add_char(&builder, 't');
        add_char(&builder, 'e');
        add_c_str(&builder, "st");

        let str = build(builder, allocator);
        defer(destroy(&str));

        assert(equal(str, str));
        assert(equal_c_str(str, "test"));
        assert(!equal_c_str(str, "tst"));
        assert(!equal_c_str(str, "tstaaaaa"));
    }
    {
        let builder   = String_Builder_create(allocator);
        defer(destroy(&builder));

        add_c_str(&builder, "Unicode ÁÁ");
        add_rune(&builder, from_c_str("É"));
        add_rune(&builder, from_char('s'));

        let str = build(builder, allocator);
        defer(destroy(&str));

        assert(equal(str, str));
        assert(equal_c_str(str, "Unicode ÁÁÉs"));
        assert(length_by_rune(str) == 12);
    }

    {
        let str = String_clone_from_cstr("Test string", allocator);
        defer(destroy(&str));
        
        assert(equal_c_str(str, "Test string"));
        assert(starts_with_c_str(str, "Test"));
        assert(starts_with_c_str(str, ""));

        let str2 = substr_alias_unsafe(str, 0, 4);
        assert(equal_c_str(str2, "Test"));
        assert(starts_with(str, str2));

        let try_str3 = substr_alias(str, 0, 4);
        assert(try_str3.some);
        let str3 = try_str3.value;
        assert(equal_c_str(str3, "Test"));

        let try_str4 = substr_alias(str, 0, str.size + 1);
        assert(!try_str4.some);

        let str5 = substr_alias_unsafe(str, 0, str.size);
        assert(equal(str5, str));

    }
    {
        let str = String_alias("ÁÁÁ");
        let rune = from_c_str("Á");
        let count = 0;
        For_Each(iter_rune(str)){
            assert(it.rune == rune);
            count++;
        }   
        assert(count == 3);

        let array = to_rune_array(str, allocator);
        defer(destroy(&array));
        
        assert(array.size == 3);
        For_Each(iter(array)){
            assert(it.value == rune);
        }
    }
    {
        let c_str1 = "asdasdasd";
        let str = String_alias(c_str1);
        let c_str2 = c_str_unsafe(str, allocator);
        defer(free_array(allocator, c_str2, 10));

        assert(strcmp(c_str1, c_str2) == 0);
    }
    {
        let str = String_alias("ÁÁÁ");
        assert(length_by_rune(str) == 3);
    }
    {
        let builder   = String_Builder_create(allocator);
        defer(destroy(&builder));
        
        add_uint(&builder, 123);

        let str = build(builder, allocator);
        defer(destroy(&str));

        assert(equal_c_str(str, "123"));
    }
    {
        let builder   = String_Builder_create(allocator);
        defer(destroy(&builder));
        
        add_uint(&builder, 0);

        let str = build(builder, allocator);
        defer(destroy(&str));

        assert(equal_c_str(str, "0"));
    }
}

void test_queue(){
    let allocator = Mallocator_to_interface();

    let q = Queue_create<int>(allocator);
    defer(destroy(&q));
    assert(empty(q));

    enque(&q, 0);
    enque(&q, 1);
    enque(&q, 2);
    assert(q.size == 3);

    assert(deque_unsafe(&q) == 0);
    assert(deque_unsafe(&q) == 1);
    assert(deque_unsafe(&q) == 2);
    assert(empty(q));


    {
        let try_elem = deque(&q);
        assert(!try_elem.some);
    }

    {
        enque(&q, 12);
        let try_elem = deque(&q);
        assert(try_elem.some);
        let elem = try_elem.value;
        assert(elem == 12);
    }
}

void test_hash_set(){
    let alloc = Mallocator_to_interface();
    {
        let set = Hash_Set_create<int>(alloc);
        defer(destroy(&set));

        add(&set, 0);
        add(&set, 1);
        add(&set, 2);
        add(&set, 3);
        add(&set, 4);

        assert(set.size == 5);
        assert(contains(set, 0));
        assert(contains(set, 1));
        assert(contains(set, 2));
        assert(contains(set, 3));
        assert(contains(set, 4));
        assert(!contains(set, 5));

        assert(remove(&set, 4));
        assert(!contains(set, 4));
        assert(!remove(&set, 4));
        
        assert(set.size == 4);

        add(&set, 5);
        assert(contains(set, 5));

        assert(set.size == 5);
    
        assert(remove(&set, 0));
        assert(remove(&set, 1));
        assert(remove(&set, 2));
        assert(remove(&set, 3));
        assert(remove(&set, 5));

        assert(empty(set));
    }
    {
        struct Test{
            int a = 0;
        };
        let set = Hash_Set_create<Test>(alloc, 1, [](Test lhs) -> usize {return lhs.a;}, [](Test lhs, Test rhs) -> bool { return  lhs.a == rhs.a;});
        defer(destroy(&set));

        add(&set, {0});
        add(&set, {1});
        add(&set, {2});
        add(&set, {3});
        add(&set, {4});

        assert(set.size == 5);
        assert(contains(set, {0}));
        assert(contains(set, {1}));
        assert(contains(set, {2}));
        assert(contains(set, {3}));
        assert(contains(set, {4}));
        assert(!contains(set, {5}));

        assert(remove(&set,   {4}));
        assert(!contains(set, {4}));
        assert(!remove(&set, {4}));
        
        assert(set.size == 4);

        add(&set, {5});
        assert(contains(set, {5}));

        assert(set.size == 5);
    
        assert(remove(&set, {0}));
        assert(remove(&set, {1}));
        assert(remove(&set, {2}));
        assert(remove(&set, {3}));
        assert(remove(&set, {5}));

        assert(empty(set));
    }
}

void test_hash_map(){
    let alloc = Mallocator_to_interface();
    let map = Hash_Map_create<int, int>(alloc);
    defer(destroy(&map));

    assert(empty(map));

    {
        set(&map, 0, 0);
        assert(get_unwrap(map, 0) == 0);
        assert(map.size == 1);

        set(&map, 0, 1);
        assert(get_unwrap(map, 0) == 1);
        assert(map.size == 1);

        set(&map, 0, 123);
        assert(get_unwrap(map, 0) == 123);
        assert(map.size == 1);

        assert(!get(map, 1).some);
        assert(map.size == 1);

        set(&map, -1, -1);
        assert(map.size == 2);
        assert(get_unwrap(map, -1) == -1);

        remove(&map, -1);
        assert(map.size == 1);
        assert(!get(map, -1).some);

        remove(&map, 0);
        assert(empty(map));
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
                    return Array_empty<u8>();
                }
                data->returned = true;
                return String_alias(data->str);
            },
        };
        let r = from_c_str("Á");

        next(&scanner);
        assert(scanner.current == 'K');
        next(&scanner);
        assert(scanner.current == 'e');
        next(&scanner);
        assert(scanner.current == 'c');
        next(&scanner);
        assert(scanner.current == 's');
        next(&scanner);
        assert(scanner.current == 'k');
        next(&scanner);
        assert(scanner.current == 'e');
        next(&scanner);
        assert(scanner.current == r);
        next(&scanner);
        assert(scanner.current == r);
        next(&scanner);
        assert(scanner.current == 0);
        next(&scanner);
        assert(scanner.current == 0);
        next(&scanner);
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
                    return Array_empty<u8>();
                }
                data->returned = true;
                return String_alias(data->str);
            },
        };
        next(&scanner);

        let number = scanner_scan_uint(&scanner).unwrap();
        assert(number == 123);
    }
}
