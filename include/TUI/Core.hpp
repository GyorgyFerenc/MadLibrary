#include <termios.h>
#include <unistd.h>

#include <csignal>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include "../Logger.hpp"
#include "../String.hpp"
#include "Event.hpp"
#include "SharedQueue.hpp"

struct FileLogger : MadLibrary::Logger {
    std::ofstream out{"Log.out"};
    std::mutex    lock;

    void log(std::string text) override {
        std::unique_lock<std::mutex> mlock(this->lock);
        out << "[INFO] " << text << std::endl;
    }
};

namespace Private {
void handle_signal(int signal);
};  // namespace Private

namespace MadLibrary {

struct Model {
    virtual std::string draw() = 0;
    virtual EventPtr    update(EventPtr& event) = 0;
    virtual EventPtr    init() {
        return std::make_unique<Event>(DRAW);
    };
};

using ModelPtr = std::unique_ptr<Model>;

struct TUI {
    TUI(Model* model) {
        this->model = std::unique_ptr<Model>{model};

        REGISTER_LOGGER(INFO, new FileLogger);

        std::thread{&TUI::user_input_thread_execute, this}.detach();

        // std::signal(SIGINT, Private::handle_signal);
    }

    /*
         Starts the tui
     */
    void start() {
        LOG_INFO("TUI Started");
        this->cursor_off();
        this->no_echo();
        this->go_altscreen();

        this->init_phase();

        while (running) {  // Main loop

            LOG_INFO("Loop started");

            this->drawing_phase();
            this->event_phase();
        }

        // this->clear_screen();
        this->leave_altscreen();
        this->echo();
        this->cursor_on();

        LOG_INFO("TUI Ended");
    }

   private:
    friend void handle_signal(int signal);

    ModelPtr              model;
    bool                  running = true;
    bool                  need_redraw = true;
    SharedQueue<EventPtr> queue;

    EventPtr get_user_event() const {
        KeyType key_type = Private::get_key_type();

        auto key_event = std::make_unique<KeyEvent>(key_type);

        IF_LOGGING_ENABLED({
            char key_char = key_type;
            int  key_number = key_char;
            LOG_INFO("KeyEvent: ", key_char, " -> ", key_number);
        });

        return std::move(key_event);
    }

    void user_input_thread_execute() {
        while (running) {
            EventPtr event = this->get_user_event();
            this->queue.push_back(std::move(event));
        }
    }

    void handle_call(CallEventPtr call_event) {
        LOG_INFO("Call event started!");

        auto reply = call_event->function();
        auto reply_event = ReplyEvent::create(reply);
        this->queue.push_back(std::move(reply_event));

        LOG_INFO("Call event pushed!");
    }

    void update(EventPtr event) {
        switch (event->get_type()) {
            case EXIT:
                this->running = false;
                break;

            case DRAW:
                this->need_redraw = true;
                break;

            case CALL: {
                auto reply = EVENT_CAST(event, CallEvent);
                std::thread{&TUI::handle_call, this, std::move(reply)}.detach();
                break;
            }

            case BATCH: {
                auto batch = EVENT_CAST(event, BatchEvent);
                for (auto&& event : *batch) {
                    this->update(std::move(event));
                }

                break;
            }

            default:
                break;
        }
    }

    EventPtr get_event() {
        auto event = std::move(this->queue.front());
        this->queue.pop_front();
        return event;
    }

    void event_phase() {
        LOG_INFO("Event phase");
        // Get user event
        auto user_event = this->get_event();

        // Update the model
        auto model_event = this->model->update(user_event);

        // Update the TUI by the event got from the model
        this->update(std::move(model_event));
    }

    void init_phase() {
        LOG_INFO("Init phase");
        EventPtr model_event = this->model->init();

        // Update the TUI by the event got from the model
        this->update(std::move(model_event));
    }

    void draw(std::string canvas) {
        this->clear_screen();
        std::cout << canvas << std::flush;
    }

    void drawing_phase() {
        LOG_INFO("Drawing phase");
        if (need_redraw) {
            // Get the canvas to be drawn
            std::string canvas = this->model->draw();

            // Draws to the screen
            this->draw(canvas);

            need_redraw = false;
        }
    }

    void clear_screen() const {
        system("clear");
        LOG_INFO("Clear screen");
    }

    void go_altscreen() {
        // tput smcup
        system("tput smcup");
        LOG_INFO("Go altscreen");
        // std::cout << "\e[?1049h" << std::flush;
    }

    void leave_altscreen() {
        // tput rmcup
        system("tput rmcup");
        LOG_INFO("Leave altscreen");
        // std::cout << "\e[?1049l" << std::flush;
    }

    void cursor_off() {
        system("setterm -cursor off");
        LOG_INFO("Cursor off");
    }

    void cursor_on() {
        system("setterm -cursor on");
        LOG_INFO("Cursor on");
    }

    void no_echo() {
        struct termios old = {0};
        if (tcgetattr(0, &old) < 0) perror("tcsetattr()");
        old.c_lflag &= ~ICANON;
        old.c_lflag &= ~ECHO;
        old.c_cc[VMIN] = 1;
        old.c_cc[VTIME] = 0;
        if (tcsetattr(0, TCSANOW, &old) < 0) perror("tcsetattr ICANON");
    }

    void echo() {
        struct termios old = {0};
        if (tcgetattr(0, &old) < 0) perror("tcsetattr()");
        old.c_lflag &= ~ICANON;
        old.c_lflag &= ~ECHO;
        old.c_cc[VMIN] = 1;
        old.c_cc[VTIME] = 0;
        if (tcsetattr(0, TCSANOW, &old) < 0) perror("tcsetattr ICANON");
        old.c_cc[VMIN] = 1;
        old.c_cc[VTIME] = 0;
        old.c_lflag |= ICANON;
        old.c_lflag |= ECHO;
        if (tcsetattr(0, TCSADRAIN, &old) < 0) perror("tcsetattr ~ICANON");
    }
};

};  // namespace MadLibrary

namespace Private {

void handle_signal(int signal) {
}
};  // namespace Private