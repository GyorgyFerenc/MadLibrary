#include <iostream>

struct PrinterInterface {
    void* base;
    void (*print)(void* self);
};
void print(PrinterInterface& printer) {
    printer.print(printer.base);
}

struct MyPrinter {
    int a;
};

void print(MyPrinter& printer) {
    std::cout << printer.a << std::endl;
}

PrinterInterface to_interface(MyPrinter& printer) {
    return {
        .base = &printer,
        .print = [](void* self) { print(*(MyPrinter*)self); },
    };
}
