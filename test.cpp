#include "include/Test.hpp"


/*
 * To check memory usage run it with valgrind
 */

int main(){
    test_arena_allocator();
    test_page_allocator();
    test_dynamic_array();
    test_rune();
    test_string();
    test_queue();
    test_hash_set();
    test_hash_map();
    test_utf8_scanner();
}
