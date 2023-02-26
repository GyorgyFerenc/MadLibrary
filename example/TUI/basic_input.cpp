#include <iostream>

#include "../../include/Format.hpp"
#include "../../include/TUI/Core.hpp"

namespace ml = MadLibrary;

struct TestInput : public ml::Model {
    std::string draw() override {
        return "test";
    }

    ml::EventPtr update(ml::EventPtr& event) override {
        return ml::DrawEvent::create();
    }
};

int main(int argc, char const* argv[]) {
    ml::TUI tui{new TestInput};
    tui.start();
}
