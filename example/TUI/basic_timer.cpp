#include <iostream>

#include "../../include/Format.hpp"
#include "../../include/TUI/Core.hpp"

namespace ml = MadLibrary;

struct FrameUpdate : public ml::Reply {};

struct Test : ml::Model {
    int         dot = 0;
    int         max_dots = 3;
    std::string text = "Loading";
    int64_t     time_to_sleep = 1000;

    std::string draw() override {
        return text;
    }

    ml::EventPtr init() override {
        return ml::generate_reply(time_to_sleep, new FrameUpdate);
    }

    ml::EventPtr update(ml::EventPtr& event) override {
        ml::EventPtr      draw = ml::DrawEvent::create();
        ml::BatchEventPtr batch = ml::BatchEvent::create(draw);

        if (event->get_type() == ml::REPLY) {
            auto reply = EVENT_CAST(event, ml::ReplyEvent);

            if (REPLY_HAS_TYPE(reply, FrameUpdate)) {
                dot++;
                text += ".";
                batch->add_event(ml::generate_reply(time_to_sleep, new FrameUpdate));
            }
        }

        if (dot >= max_dots) {
            return ml::ExitEvent::create();
        }

        return batch;
    };
};

int main(int argc, char const* argv[]) {
    ml::TUI tui{new Test};
    tui.start();
}
