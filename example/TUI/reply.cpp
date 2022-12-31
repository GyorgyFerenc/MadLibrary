
#include <chrono>
#include <iostream>

#include "../../include/Block.hpp"
#include "../../include/Format.hpp"
#include "../../include/TUI/Core.hpp"

namespace ml = MadLibrary;

struct MyReply : ml::Reply {
    enum Type {
        FRAME_UPDATE,
    };

    Type type = FRAME_UPDATE;
};

ml::Reply* wait() {
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    return new MyReply;
}

struct MyModel : public ml::Model {
    std::string draw() override {
        return text;
    }

    ml::EventPtr init() override {
        auto call_event = ml::CallEvent::create(wait);
        return call_event;
    }

    ml::EventPtr update(ml::EventPtr& event) override {
        auto draw = ml::DrawEvent::create();
        auto batch = ml::BatchEvent::create(draw);

        switch (event->get_type()) {
            case ml::REPLY: {
                auto reply = EVENT_CAST(event, ml::ReplyEvent);

                if (!REPLY_HAS_TYPE(reply, MyReply)) {
                    break;
                }

                auto inner = REPLY_CAST(reply, MyReply);
                if (inner->type != MyReply::Type::FRAME_UPDATE) {
                    break;
                }

                this->text += this->delim;
                this->current_number_delim++;

                auto call_event = ml::CallEvent::create(wait);
                batch->add_event(std::move(call_event));

            }; break;

            default:
                break;
        }

        if (this->current_number_delim >= this->max_delim) {
            return ml::ExitEvent::create();
        }

        return batch;
    }

   private:
    std::string text = "Loading";
    std::string delim = ".";
    int         current_number_delim = 0;
    const int   max_delim = 3;
};

int main(int argc, char const* argv[]) {
    ml::TUI tui{new MyModel};
    tui.start();
}
