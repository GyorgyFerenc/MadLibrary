#include <iostream>

#include "../../include/Format.hpp"
#include "../../include/TUI/Core.hpp"

namespace ml = MadLibrary;

struct Test : ml::Model {
    std::string text = "Close by pressing c\nLoading";

    std::string draw() override {
        return text;
    }

    ml::EventPtr update(ml::EventPtr& event) override {
        text += ".";
        if (event->get_type() == ml::KEY) {
            auto key = EVENT_CAST(event, ml::KeyEvent);

            if (key->get_key() == ml::KeyType::C_LOWER) {
                return ml::ExitEvent::create();
            }
        }

        return ml::DrawEvent::create();
    };
};

int main(int argc, char const* argv[]) {
    ml::TUI tui{new Test};
    tui.start();
}
