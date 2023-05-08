#include <chrono>
#include <functional>
#include <future>
#include <memory>
#include <vector>

#include "Key.hpp"

#define REPLY_HAS_TYPE(reply, type) (dynamic_cast<type*>(reply->reply.get()) != nullptr)
#define REPLY_CAST(reply, type) dynamic_cast<type*>(reply->reply.get())
#define EVENT_CAST(event, type) \
    std::unique_ptr<type> { (type*)event.release() }

namespace MadLibrary {

enum EventType {
    EXIT,
    DRAW,
    KEY,
    BATCH,
    REPLY,
    CALL,
};

struct Event;
using EventPtr = std::unique_ptr<Event>;

struct ExitEvent;
using ExitEventPtr = std::unique_ptr<ExitEvent>;

struct DrawEvent;
using DrawEventPtr = std::unique_ptr<DrawEvent>;

struct KeyEvent;
using KeyEventPtr = std::unique_ptr<KeyEvent>;

struct BatchEvent;
using BatchEventPtr = std::unique_ptr<BatchEvent>;

struct CallEvent;
using CallEventPtr = std::unique_ptr<CallEvent>;

struct ReplyEvent;
using ReplyEventPtr = std::unique_ptr<ReplyEvent>;

struct Event {
    static EventPtr create(EventType type) {
        return std::make_unique<Event>(type);
    }

    Event(EventType type) {
        this->type = type;
    }

    virtual EventType get_type() {
        return this->type;
    }

    virtual ~Event() {
    }

   private:
    EventType type;
};

struct ExitEvent {
    static EventPtr create() {
        return Event::create(EXIT);
    }
};

struct DrawEvent {
    static EventPtr create() {
        return Event::create(DRAW);
    }
};

struct KeyEvent : public Event {
    static KeyEventPtr create(KeyType key) {
        return std::make_unique<KeyEvent>(key);
    }

    KeyEvent(KeyType key) : Event{KEY} {
        this->key = key;
    }

    KeyType get_key() {
        return this->key;
    }

   private:
    KeyType key;
};

struct BatchEvent : public Event {
    template <typename... Args>
    static BatchEventPtr create(Args&... events) {
        auto b = std::make_unique<BatchEvent>();

        (b->add_event(std::move(events)), ...);

        return b;
    }

    static BatchEventPtr create() {
        return std::make_unique<BatchEvent>();
    }

    BatchEvent() : Event{BATCH} {
    }

    void add_event(EventPtr event) {
        this->events.push_back(std::move(event));
    }

    auto begin() {
        return this->events.begin();
    }

    auto end() {
        return this->events.end();
    }

   private:
    std::vector<EventPtr> events;
};

struct Reply {
    virtual ~Reply() {
    }
};
using ReplyPtr = std::unique_ptr<Reply>;

struct CallEvent : public Event {
    using Function = std::function<Reply*()>;

    CallEvent(Function function) : Event{CALL} {
        this->function = function;
    }

    static CallEventPtr create(Function function) {
        return std::make_unique<CallEvent>(function);
    }

    Function function;
};

struct ReplyEvent : public Event {
    ReplyEvent(Reply* reply) : Event{REPLY} {
        this->reply = ReplyPtr{reply};
    }

    static ReplyEventPtr create(Reply* reply) {
        return std::make_unique<ReplyEvent>(reply);
    }

    ReplyPtr reply;
};

/* HELPER FUNCTIONS*/

/*
    Generates a CallEvent wich will reply after the
    given miliseconds, with the given reply
*/
CallEventPtr generate_reply(int64_t ms, Reply* reply) {
    std::function<Reply*()> f = [=]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
        return reply;
    };

    return CallEvent::create(f);
}

}  // namespace MadLibrary
