#include <iostream>

#include "../../include/Logger.hpp"
#include "../../include/Signal.hpp"

struct Button {
    // Declare the singal pressed
    MadLibrary::Signal<void()> pressed;

    // Simulate the button press
    void press() {
        this->pressed.emit();
    }
};

struct Sender {
    const int ID;

    // Simulate sending a message
    void send_message() {
        std::cout << "Message is sent by " << ID << "!" << std::endl;
    }
};

void button_pressed() {
    std::cout << "Button is pressed" << std::endl;
}

int main(int argc, char const* argv[]) {
    Button button;
    Sender sender1{1};
    Sender sender2{2};

    // Connect the signals to the slots
    button.pressed.connect(button_pressed);
    button.pressed.connect(&Sender::send_message, &sender1);
    button.pressed.connect(&Sender::send_message, &sender2);

    // Simulate the button press
    button.press();
}
