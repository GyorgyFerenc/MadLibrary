#include "Core.hpp"
#include "Utf8_Scanner.hpp"

#include <iostream>

void test_arena_allocator(){
    let distance_in_bytes = [](void* first, void* second) -> usize{
        return (u8*)first - (u8*)second;
    };

    u8 buffer[100];
    let arena = Arena_Allocator_::create(buffer, 100);
    let alloc = Arena_Allocator_::to_interface(&arena);

    let i = Allocator_::allocate_unwrap<int>(alloc);
    let i2 = Allocator_::allocate_unwrap<int>(alloc);
    let i3 = Allocator_::allocate_unwrap<int>(alloc);
    assert(distance_in_bytes(i2, i)  >= sizeof(int));
    assert(distance_in_bytes(i3, i2)  >= sizeof(int));

    Allocator_::free_all(alloc);

    while (Allocator_::allocate_raw(alloc, 1).error == Core_Error::Correct){ }

    Allocator_::free_all(alloc);
    {
        let i = Allocator_::allocate_unwrap<int>(alloc);
        let i2 = Allocator_::allocate_unwrap<int>(alloc);
        let i3 = Allocator_::allocate_unwrap<int>(alloc);
        assert(distance_in_bytes(i2, i)  >= sizeof(int));
        assert(distance_in_bytes(i3, i2)  >= sizeof(int));
    }

};


void test_page_allocator(){
    let mallocator     = Malloc_Allocator_::to_interface();
    let page_allocator = Page_Allocator_::create(mallocator, 100);
    let allocator      = Page_Allocator_::to_interface(&page_allocator);
    defer({
        let ret = Allocator_::free_all(allocator);
        assert(ret == Core_Error::Correct);
    });

    {
        let try_bytes = Allocator_::allocate<int>(allocator);
        assert(try_bytes.error == Core_Error::Correct);
    }
    {
        let try_bytes = Allocator_::allocate_array<int>(allocator, 100);
        assert(try_bytes.error == Core_Error::Correct);
    }
    {
        let try_bytes = Allocator_::allocate_array<int>(allocator, 200);
        assert(try_bytes.error == Core_Error::Correct);
    }
}

void test_dynamic_array(){
    let allocator = Malloc_Allocator_::to_interface();

    let array = Dynamic_Array_::create<int>(allocator);
    defer(Dynamic_Array_::destroy(&array));

    Dynamic_Array_::append(&array, 0);
    Dynamic_Array_::append(&array, 1);
    Dynamic_Array_::append(&array, 2);
    assert(array.size == 3);
    assert(array[0] == 0);
    assert(array[1] == 1);
    assert(array[2] == 2);

    Dynamic_Array_::clear(&array);
    assert(array.size == 0);
    Dynamic_Array_::append(&array, 12);
    assert(array.size == 1);
    assert(array[0] == 12);

    Dynamic_Array_::clear(&array);
    Dynamic_Array_::append(&array, 69);
    Dynamic_Array_::append(&array, 1);
    Dynamic_Array_::append(&array, 10);
    let sum = 0;
    For_Each(Dynamic_Array_::iter(array)){
        sum += it.value;
    }
    assert(sum == 80);

    For_Each(Dynamic_Array_::iter(array)){
        *it.value_ptr = 12;
    }
    For_Each(Dynamic_Array_::iter(array)){
        assert(it.value == 12);
    }
}

void test_rune(){
    assert(Rune_::from_c_str("Á") == 0xC1);
    assert(Rune_::from_c_str("ѫ") == 0x46B);
    assert(Rune_::from_char('a')  == 'a');
    {
       let rune = Rune_::from_c_str("Á");
       assert(!Rune_::whitespace(rune));
    }
    {
       let rune = Rune_::from_c_str(" ");
       assert(Rune_::whitespace(rune));
    }
    {
       let rune = Rune_::from_char('\n');
       assert(Rune_::whitespace(rune));
    }
    {
        let rune = Rune_::from_c_str("Á"); 
        u8 bytes[4] = {0};
        Rune_::encode_to_utf8(rune, bytes);
        let [decoded_rune, len] = Rune_::decode_from_utf8(bytes);
        assert(decoded_rune == rune);
        assert(len == 2);
    }
}

void test_string(){
    let allocator = Malloc_Allocator_::to_interface();
    {
        let builder   = String_Builder_::create(allocator);
        defer(String_Builder_::destroy(&builder));
        
        String_Builder_::add_char(&builder, 't');
        String_Builder_::add_char(&builder, 'e');
        String_Builder_::add_c_str(&builder, "st");

        let str = String_Builder_::build(builder, allocator);
        defer(String_::destroy(&str));

        assert(String_::equal(str, str));
        assert(String_::equal_c_str(str, "test"));
        assert(!String_::equal_c_str(str, "tst"));
        assert(!String_::equal_c_str(str, "tstaaaaa"));
    }
    {
        let builder   = String_Builder_::create(allocator);
        defer(String_Builder_::destroy(&builder));

        String_Builder_::add_c_str(&builder, "Unicode ÁÁ");
        String_Builder_::add_rune(&builder, Rune_::from_c_str("É"));
        String_Builder_::add_rune(&builder, Rune_::from_char('s'));

        let str = String_Builder_::build(builder, allocator);
        defer(String_::destroy(&str));

        assert(String_::equal(str, str));
        assert(String_::equal_c_str(str, "Unicode ÁÁÉs"));
        assert(String_::length_by_rune(str) == 12);
    }

    {
        let str = String_::clone_from_cstr("Test string", allocator);
        defer(String_::destroy(&str));
        
        assert(String_::equal_c_str(str, "Test string"));
        assert(String_::starts_with_c_str(str, "Test"));
        assert(String_::starts_with_c_str(str, ""));

        let str2 = String_::substr_alias_unsafe(str, 0, 4);
        assert(String_::equal_c_str(str2, "Test"));
        assert(String_::starts_with(str, str2));

        let try_str3 = String_::substr_alias(str, 0, 4);
        assert(try_str3.some);
        let str3 = try_str3.value;
        assert(String_::equal_c_str(str3, "Test"));

        let try_str4 = String_::substr_alias(str, 0, str.size + 1);
        assert(!try_str4.some);

        let str5 = String_::substr_alias_unsafe(str, 0, str.size);
        assert(String_::equal(str5, str));

    }
    {
        let str = String_::alias("ÁÁÁ");
        let rune = Rune_::from_c_str("Á");
        let count = 0;
        For_Each(String_::iter_rune(str)){
            assert(it.rune == rune);
            count++;
        }   
        assert(count == 3);

        let array = String_::to_rune_array(str, allocator);
        defer(Array_::destroy(&array));
        
        assert(array.size == 3);
        For_Each(Array_::iter(array)){
            assert(it.value == rune);
        }
    }
    {
        let c_str1 = "asdasdasd";
        let str = String_::alias(c_str1);
        let c_str2 = String_::c_str_unsafe(str, allocator);
        defer(Allocator_::free_array(allocator, c_str2, 10));

        assert(strcmp(c_str1, c_str2) == 0);
    }
    {
        let str = String_::alias("ÁÁÁ");
        assert(String_::length_by_rune(str) == 3);
    }
}

void test_queue(){
    let allocator = Malloc_Allocator_::to_interface();

    let q = Queue_::create<int>(allocator);
    defer(Queue_::destroy(&q));
    assert(Queue_::empty(q));

    Queue_::enque(&q, 0);
    Queue_::enque(&q, 1);
    Queue_::enque(&q, 2);
    assert(q.size == 3);

    assert(Queue_::deque_unsafe(&q) == 0);
    assert(Queue_::deque_unsafe(&q) == 1);
    assert(Queue_::deque_unsafe(&q) == 2);
    assert(Queue_::empty(q));


    {
        let try_elem = Queue_::deque(&q);
        assert(!try_elem.some);
    }

    {
        Queue_::enque(&q, 12);
        let try_elem = Queue_::deque(&q);
        assert(try_elem.some);
        let elem = try_elem.value;
        assert(elem == 12);
    }
}

void test_hash_set(){
    let alloc = Malloc_Allocator_::to_interface();
    {
        let set = Hash_Set_::create<int>(alloc);
        defer(Hash_Set_::destroy(&set));

        Hash_Set_::add(&set, 0);
        Hash_Set_::add(&set, 1);
        Hash_Set_::add(&set, 2);
        Hash_Set_::add(&set, 3);
        Hash_Set_::add(&set, 4);

        assert(set.size == 5);
        assert(Hash_Set_::contains(set, 0));
        assert(Hash_Set_::contains(set, 1));
        assert(Hash_Set_::contains(set, 2));
        assert(Hash_Set_::contains(set, 3));
        assert(Hash_Set_::contains(set, 4));
        assert(!Hash_Set_::contains(set, 5));

        assert(Hash_Set_::remove(&set, 4));
        assert(!Hash_Set_::contains(set, 4));
        assert(!Hash_Set_::remove(&set, 4));
        
        assert(set.size == 4);

        Hash_Set_::add(&set, 5);
        assert(Hash_Set_::contains(set, 5));

        assert(set.size == 5);
    
        assert(Hash_Set_::remove(&set, 0));
        assert(Hash_Set_::remove(&set, 1));
        assert(Hash_Set_::remove(&set, 2));
        assert(Hash_Set_::remove(&set, 3));
        assert(Hash_Set_::remove(&set, 5));

        assert(Hash_Set_::empty(set));
    }
    {
        struct Test{
            int a = 0;
        };
        let set = Hash_Set_::create<Test>(alloc, 1, [](Test lhs) -> usize {return lhs.a;}, [](Test lhs, Test rhs) -> bool { return  lhs.a == rhs.a;});
        defer(Hash_Set_::destroy(&set));

        Hash_Set_::add(&set, {0});
        Hash_Set_::add(&set, {1});
        Hash_Set_::add(&set, {2});
        Hash_Set_::add(&set, {3});
        Hash_Set_::add(&set, {4});

        assert(set.size == 5);
        assert(Hash_Set_::contains(set, {0}));
        assert(Hash_Set_::contains(set, {1}));
        assert(Hash_Set_::contains(set, {2}));
        assert(Hash_Set_::contains(set, {3}));
        assert(Hash_Set_::contains(set, {4}));
        assert(!Hash_Set_::contains(set, {5}));

        assert(Hash_Set_::remove(&set,   {4}));
        assert(!Hash_Set_::contains(set, {4}));
        assert(!Hash_Set_::remove(&set, {4}));
        
        assert(set.size == 4);

        Hash_Set_::add(&set, {5});
        assert(Hash_Set_::contains(set, {5}));

        assert(set.size == 5);
    
        assert(Hash_Set_::remove(&set, {0}));
        assert(Hash_Set_::remove(&set, {1}));
        assert(Hash_Set_::remove(&set, {2}));
        assert(Hash_Set_::remove(&set, {3}));
        assert(Hash_Set_::remove(&set, {5}));

        assert(Hash_Set_::empty(set));
    }
}

void test_hash_map(){
    let alloc = Malloc_Allocator_::to_interface();
    let map = Hash_Map_::create<int, int>(alloc);
    defer(Hash_Map_::destroy(&map));

    assert(Hash_Map_::empty(map));

    {
        Hash_Map_::set(&map, 0, 0);
        assert(Hash_Map_::get_unwrap(map, 0) == 0);
        assert(map.size == 1);

        Hash_Map_::set(&map, 0, 1);
        assert(Hash_Map_::get_unwrap(map, 0) == 1);
        assert(map.size == 1);

        Hash_Map_::set(&map, 0, 123);
        assert(Hash_Map_::get_unwrap(map, 0) == 123);
        assert(map.size == 1);

        assert(!Hash_Map_::get(map, 1).some);
        assert(map.size == 1);

        Hash_Map_::set(&map, -1, -1);
        assert(map.size == 2);
        assert(Hash_Map_::get_unwrap(map, -1) == -1);

        Hash_Map_::remove(&map, -1);
        assert(map.size == 1);
        assert(!Hash_Map_::get(map, -1).some);

        Hash_Map_::remove(&map, 0);
        assert(Hash_Map_::empty(map));
    }   
}


void test_utf8_scanner(){
    {
        let str = String_::alias("Kecske");
        let it  = String_::iter(str);
        let scanner = Utf8_Scanner_create([](void* data) -> Option<u8>{
            let it     = ((Array_Iter<u8>*)data);
            let succes = it->next();
            return {succes, it->value};
        }, &it);

        assert(scanner.current == Rune_::from_char('K'));
        //assert(scanner.peek == Rune_::from_char('e'));

        let it2  = String_::iter_rune(str);
        it2.next();
        while (scanner.current != 0) {
            assert(it2.rune == scanner.current);
            Utf8_Scanner_advance(&scanner);
            it2.next();
        }
    }
    {
        let str = String_::alias("  Kecske");
        let it  = String_::iter(str);
        let scanner = Utf8_Scanner_create([](void* data) -> Option<u8>{
            let it     = ((Array_Iter<u8>*)data);
            let succes = it->next();
            return {succes, it->value};
        }, &it);

        let nr = Utf8_Scanner_skip_whitespace(&scanner);
        assert(nr == 2);
        assert(scanner.current == Rune_::from_char('K'));
        //assert(scanner.peek == Rune_::from_char('e'));
    }
    {
        let str = String_::alias("-12 +12 12 0xC 0xc -0xFF -0xfF 0b101 0b0000101 012 1_23 0b1010_1010");
        let it  = String_::iter(str);
        let scanner = Utf8_Scanner_create([](void* data) -> Option<u8>{
            let it     = ((Array_Iter<u8>*)data);
            let succes = it->next();
            return {succes, it->value};
        }, &it);

        {
            let i = Utf8_Scanner_scan_int(&scanner).unwrap();
            Utf8_Scanner_skip_whitespace(&scanner);
            assert(i == -12);
        }
        {
            let i = Utf8_Scanner_scan_int(&scanner).unwrap();
            Utf8_Scanner_skip_whitespace(&scanner);
            assert(i == 12);
        }
        {
            let i = Utf8_Scanner_scan_int(&scanner).unwrap();
            Utf8_Scanner_skip_whitespace(&scanner);
            assert(i == 12);
        }
        {
            let i = Utf8_Scanner_scan_int(&scanner).unwrap();
            Utf8_Scanner_skip_whitespace(&scanner);
            assert(i == 12);
        }
        {
            let i = Utf8_Scanner_scan_int(&scanner).unwrap();
            Utf8_Scanner_skip_whitespace(&scanner);
            assert(i == 12);
        }
        {
            let i = Utf8_Scanner_scan_int(&scanner).unwrap();
            Utf8_Scanner_skip_whitespace(&scanner);
            assert(i == -0xFF);
        }
        {
            let i = Utf8_Scanner_scan_int(&scanner).unwrap();
            Utf8_Scanner_skip_whitespace(&scanner);
            assert(i == -0xFF);
        }
        {
            let i = Utf8_Scanner_scan_int<u8>(&scanner).unwrap();
            Utf8_Scanner_skip_whitespace(&scanner);
            assert(i == 0b101);
        }
        {
            let i = Utf8_Scanner_scan_int<u8>(&scanner).unwrap();
            Utf8_Scanner_skip_whitespace(&scanner);
            assert(i == 0b101);
        }
        {
            let i = Utf8_Scanner_scan_int<u8>(&scanner).unwrap();
            Utf8_Scanner_skip_whitespace(&scanner);
            assert(i == 12);
        }
        {
            let i = Utf8_Scanner_scan_int(&scanner).unwrap();
            Utf8_Scanner_skip_whitespace(&scanner);
            assert(i == 123);
        }
        {
            let i = Utf8_Scanner_scan_int(&scanner).unwrap();
            Utf8_Scanner_skip_whitespace(&scanner);
            assert(i == 0b10101010);
        }
    }
    {
        let str = String_::alias("0by0xpasd");
        let it  = String_::iter(str);
        let scanner = Utf8_Scanner_create([](void* data) -> Option<u8>{
            let it     = ((Array_Iter<u8>*)data);
            let succes = it->next();
            return {succes, it->value};
        }, &it);

        {
            let try_i = Utf8_Scanner_scan_int(&scanner);
            Utf8_Scanner_skip_whitespace(&scanner);
            assert(!try_i.some);
            assert(scanner.current == 'y');
            Utf8_Scanner_advance(&scanner);
        }
        {
            let try_i = Utf8_Scanner_scan_int(&scanner);
            Utf8_Scanner_skip_whitespace(&scanner);
            assert(!try_i.some);
            assert(scanner.current == 'p');
            Utf8_Scanner_advance(&scanner);
        }
        {
            let try_i = Utf8_Scanner_scan_int(&scanner);
            Utf8_Scanner_skip_whitespace(&scanner);
            assert(!try_i.some);
            assert(scanner.current == 'a');
            Utf8_Scanner_advance(&scanner);
        }
    }
    {
        let str = String_::alias("0 12 12.0 12.3 12.03 -12.03 asd");
        let it  = String_::iter(str);
        let scanner = Utf8_Scanner_create([](void* data) -> Option<u8>{
            let it     = ((Array_Iter<u8>*)data);
            let succes = it->next();
            return {succes, it->value};
        }, &it);

        {
            let i = Utf8_Scanner_scan_float(&scanner).unwrap();
            Utf8_Scanner_skip_whitespace(&scanner);
            assert(i == 0);
        }
        {
            let i = Utf8_Scanner_scan_float(&scanner).unwrap();
            Utf8_Scanner_skip_whitespace(&scanner);
            assert(i == 12.0);
        }
        {
            let i = Utf8_Scanner_scan_float(&scanner).unwrap();
            Utf8_Scanner_skip_whitespace(&scanner);
            assert(i == 12.0);
        }
        {
            let i = Utf8_Scanner_scan_float(&scanner).unwrap();
            Utf8_Scanner_skip_whitespace(&scanner);
            assert(i == 12.3);
        }
        {
            let i = Utf8_Scanner_scan_float(&scanner).unwrap();
            Utf8_Scanner_skip_whitespace(&scanner);
            assert(i == 12.03);
        }
        {
            let i = Utf8_Scanner_scan_float(&scanner).unwrap();
            Utf8_Scanner_skip_whitespace(&scanner);
            assert(i == -12.03);
        }
        {
            let try_i = Utf8_Scanner_scan_float(&scanner);
            assert(!try_i.some);
        }
    }
        
    {
        let str = String_::alias("true");
        let it  = String_::iter(str);
        let scanner = Utf8_Scanner_create([](void* data) -> Option<u8>{
            let it     = ((Array_Iter<u8>*)data);
            let succes = it->next();
            return {succes, it->value};
        }, &it);

        {
            let i = Utf8_Scanner_scan_bool(&scanner)
                .unwrap();
            Utf8_Scanner_skip_whitespace(&scanner);
            assert(i == true);
        }
    }

    {
        let str = String_::alias("Kecskea");
        let it  = String_::iter(str);
        let scanner = Utf8_Scanner_create([](void* data) -> Option<u8>{
            let it     = ((Array_Iter<u8>*)data);
            let succes = it->next();
            return {succes, it->value};
        }, &it);

        {
            assert(Utf8_Scanner_scan_exact(&scanner, String_::alias("Kecske")));
            assert(scanner.current == 'a');
            assert(!Utf8_Scanner_scan_exact(&scanner, String_::alias("e")));
            assert(scanner.current == 'a');
        }
    }
}
