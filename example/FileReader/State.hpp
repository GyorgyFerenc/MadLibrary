#include <ranges>
#include <string>
#include <vector>

#include "Extension.hpp"
#include "Logic.hpp"

// Place where all the program state resides
namespace State {

// The list of todos
std::vector<std::string> todos;

// Adds a todo to the list
proc add_todo(std::string todo) {
    todos.push_back(todo);
}

// Removes the todo from the list
proc remove_todo(std::size_t index) {
    todos.erase(todos.begin() + index);
}

// Upercases all the worlds in the list
// Words are separeted by space
proc uppercase_all_word()->void {
    for (std::size_t i = 0; i < todos.size(); i++) {
        let todo = todos[i];

        let words = split(todo, ' ');
        let upwords = map(words, uppercase_word);
        let combined = combine(upwords, ' ');

        todos[i] = combined;
    }
}

}  // namespace State