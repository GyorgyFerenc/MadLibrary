#pragma once

#include <signal.h>
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include "core.hpp"
#include "os.hpp"
#include "fmt.hpp"
#include "tui/canvas.hpp"

const let ESC = "\u001b";
const let STDIN  = 0;
const let STDOUT = 1;
const let STDERR = 2;

struct Tui_Key_Event{
    enum struct Kind: u8{
        Rune, // Rune
        Code, // Special
    } kind;

    enum Code{
        Up,
        Down,
        Right,
        Left,
    // Todo(Ferenc): Add the remaining
        Insert,
        Delete,  // del key in keyboard
        Home,
        End,
        PageUp,
        PageDown,
        BackTab,  // shift+tab
    } code;

    Rune rune = 0;

    // private
    Slice<u8> data;
    u8 buffer[100];
};

struct Tui_Event{
    enum Kind{
        None,
        Key,
        Stop,
        Resize,
    } kind = None;

    union {
        Tui_Key_Event key;
    };
};

struct {
    //int tty;
    struct termios old_termios;
    Thread handler;
    bool altscreen         = false;
    bool invisible_cursor  = false;
    usize nr_lines_printed = 0;

    winsize size;

    Pipe stop;
    Pipe resize;

    Queue<Tui_Event> event_queue;
    Mutex event_mutex;
    Conditional_Variable event_cond;
} tui;

void tui_push_event(Tui_Event event){
    {
        lock(&tui.event_mutex);
        defer (unlock(&tui.event_mutex));
       
        push(&tui.event_queue, event);
    }

    notify_one(&tui.event_cond);
}


/*
   returns width, height
*/
inline
Pair<usize, usize> tui_get_size(){
    return {tui.size.ws_col, tui.size.ws_row};
}

void tui_handler(Thread_Data){
    int maxfd = 0;
    fd_set set;
    FD_ZERO(&set);

    FD_SET(tui.stop.read, &set);
    if (maxfd < tui.stop.read) maxfd = tui.stop.read;

    FD_SET(STDIN, &set);
    if (maxfd < STDIN) maxfd = STDIN;

    FD_SET(tui.resize.read, &set);
    if (maxfd < tui.resize.read) maxfd = tui.resize.read;

    for (;;){
        let copy_set = set;
        select(maxfd + 1, &copy_set, NULL, NULL, NULL);
        if (FD_ISSET(tui.stop.read, &copy_set)){
            tui_push_event(Tui_Event{
                .kind = Tui_Event::Stop,
            });
            break;
        } 
        if (FD_ISSET(STDIN, &copy_set)){
            u8 buffer[100] = {0};
            let n = read(STDIN, &buffer[0], 100);
            let event = Tui_Event{
                .kind = Tui_Event::Key,
            };
            
            if (buffer[0] == '\e'){
                event.key.kind = Tui_Key_Event::Kind::Code;
                //debug
                memcpy(event.key.buffer, buffer, n);
                event.key.data = slice_from(event.key.buffer, 0, n);
                
                if ( buffer[1] == 91
                  && buffer[2] == 65){
                    event.key.code = Tui_Key_Event::Up;
                } else 
                if ( buffer[1] == 91
                  && buffer[2] == 66){
                    event.key.code = Tui_Key_Event::Down;
                } else
                if ( buffer[1] == 91
                  && buffer[2] == 67){
                    event.key.code = Tui_Key_Event::Right;
                } else
                if ( buffer[1] == 91
                  && buffer[2] == 68){
                    event.key.code = Tui_Key_Event::Left;
                } 
                // Todo(Ferenc): Add the remaining

                /*else
                if ( buffer[1] == 
                  && buffer[2] ==
                  && buffer[3] ==
                  && buffer[4] == ){
                    event.key.key_code = Tui_Key_Event::;
                }*/
            } else {
                event.key.kind = Tui_Key_Event::Kind::Rune;

                let [r, _] = decode_rune_utf8(buffer);
                event.key.rune = r;
            }

            tui_push_event(event);
        } 
        if (FD_ISSET(tui.resize.read, &copy_set)){
            bool b;
            read(tui.resize.read, &b, 1);
            ioctl(STDIN, TIOCGWINSZ, &tui.size);
            tui_push_event(Tui_Event{
                .kind = Tui_Event::Resize,
            });
        }

    }
}

void tui_resize_handler(int sig){
    bool b = true;
    write(tui.resize.write, &b, 1);
}

void tui_init(){
    //tui.tty = open("/dev/tty", O_RDWR);

    if (tcgetattr(0, &tui.old_termios) < 0) perror("init: Termios tcgetattr()");
    struct termios new_state = tui.old_termios;
    new_state.c_lflag &= ~ICANON;
    new_state.c_lflag &= ~ECHO;
    new_state.c_lflag &= ~ISIG;
    new_state.c_cc[VMIN] = 1;
    new_state.c_cc[VTIME] = 0;

    if (tcsetattr(0, TCSANOW, &new_state) < 0) perror("init: Termios tcsetattr()");

    tui.event_queue = create_queue<Tui_Event>(mallocator);
    tui.event_mutex = create_mutex();
    tui.event_cond  = create_conditional_variable();
    
    ioctl(STDIN, TIOCGWINSZ, &tui.size);
    signal(SIGWINCH, tui_resize_handler);

    init(&tui.stop);
    init(&tui.resize);
    start(&tui.handler, tui_handler);
}


void tui_enter_altscreen() {
    print_fmt("\e[?1049h\e[H");
    tui.altscreen = true;
}

void tui_leave_altscreen() {
    print_fmt("\e[?1049l");
    tui.altscreen = false;
}

void tui_save_cursor(){
    print_fmt("\e7");
}

void tui_restore_cursor(){
    print_fmt("\e8");
}

void tui_stop(){
    bool t = true;
    write(tui.stop.write, &t, 1);
}

void tui_erase_until_end_of_screen(){
    print_fmt("\e[0J");
}

void tui_erase_all(){
    print_fmt("\e[2J");
}

void tui_erase_printed_lines(){
    print_fmt("\e[%A", tui.nr_lines_printed);
    tui_erase_until_end_of_screen();
}

void tui_move_cursor_home(){
    print_fmt("\e[H");
}

void tui_set_cursor_invisible(){
    print_fmt("\e[?25l");
    tui.invisible_cursor = true;
}

void tui_set_cursor_visible(){
    print_fmt("\e[?25h");
    tui.invisible_cursor = false;
}


void tui_deinit(){
    if (tcsetattr(0, TCSADRAIN, &tui.old_termios) < 0) perror("deinit: Termios tcsetattr");

    if (tui.altscreen){ tui_leave_altscreen(); }
    if (tui.invisible_cursor){ tui_set_cursor_visible(); }

    tui_stop();
    join(tui.handler);
}

inline
void handle_style(Pixel* brush, Pixel pixel, Pixel::Style style, const char* set, const char* reset){
    if (!same_style(*brush, pixel, style)){
        if (check_style(pixel, style)){
            print_fmt(set);
            set_style(brush, style);
        } else {
            print_fmt(reset);
            unset_style(brush, style);
        }
    }
}


void tui_print(Canvas c){
    u8 buffer[5] = {0};
    let slice = Slice<u8>{buffer, 5};
    tui.nr_lines_printed = c.height;

    let brush = Pixel{};

    for (usize y = 0; y < c.height; y++){
        for (usize x = 0; x < c.width; x++){
            let pixel = *get(c, x, y);

            // Bold and Dim needs to be handrolled because they are unset with the same control code
            // Very nice design.
            if (!same_style(brush, pixel, Pixel::Bold)){
                if (check_style(pixel, Pixel::Bold)){
                    print_fmt("\e[1m");
                    set_style(&brush, Pixel::Bold);
                } else {
                    print_fmt("\e[22m");
                    unset_style(&brush, Pixel::Bold);
                    if (check_style(brush, Pixel::Dim)){
                        print_fmt("\e[2m");
                    }
                }
            }
            if (!same_style(brush, pixel, Pixel::Dim)){
                if (check_style(pixel, Pixel::Dim)){
                    print_fmt("\e[2m");
                    set_style(&brush, Pixel::Dim);
                } else {
                    print_fmt("\e[22m");
                    unset_style(&brush, Pixel::Dim);
                    if (check_style(brush, Pixel::Bold)){
                        print_fmt("\e[1m");
                    }
                }
            }
            handle_style(&brush, pixel, Pixel::Italic,       "\e[3m", "\e[23m");
            handle_style(&brush, pixel, Pixel::Underline,    "\e[4m", "\e[24m");
            handle_style(&brush, pixel, Pixel::Blinking,     "\e[5m", "\e[25m");
            handle_style(&brush, pixel, Pixel::Inverse,      "\e[7m", "\e[27m");
            handle_style(&brush, pixel, Pixel::Hidden,       "\e[8m", "\e[28m");
            handle_style(&brush, pixel, Pixel::Striketrough, "\e[9m", "\e[29m");

            switch (pixel.foreground.color_mode){
            case Pixel::None: 
                if (brush.foreground.color_mode != Pixel::Color_Mode::None){
                    print_fmt("\e[39m");
                    brush.foreground.color_mode = Pixel::Color_Mode::None;
                }
                break;
            case Pixel::Color_Mode_8:   {
                if (brush.foreground.color_mode == Pixel::Color_Mode_8 && 
                    brush.foreground.color_8 == pixel.foreground.color_8) { 
                    break; 
                } else { brush.foreground.color_mode = pixel.foreground.color_mode; }
                brush.foreground.color_8 = pixel.foreground.color_8;
                print_fmt("\e[39m");
                u8 color_code = cast(u8) pixel.foreground.color_8;
                print_fmt("\e[%m", color_code);
            } break;
            case Pixel::Color_Mode_16:  {
                if (brush.foreground.color_mode == Pixel::Color_Mode_16 && 
                    brush.foreground.color_16 == pixel.foreground.color_16) { 
                    break; 
                } else { brush.foreground.color_mode = pixel.foreground.color_mode; }
                brush.foreground.color_16 = pixel.foreground.color_16;
                print_fmt("\e[39m");
                u8 color_code = cast(u8) pixel.foreground.color_16;
                print_fmt("\e[%m", color_code);
            } break;
            case Pixel::Color_Mode_256: {
                if (brush.foreground.color_mode == Pixel::Color_Mode_256 && 
                    brush.foreground.color_256 == pixel.foreground.color_256) { 
                    break; 
                } else { brush.foreground.color_mode = pixel.foreground.color_mode; }
                brush.foreground.color_256 = pixel.foreground.color_256;
                print_fmt("\e[39m");
                print_fmt("\e[38;5;%m", brush.foreground.color_256);
            } break;
            case Pixel::Color_Mode_RGB: {
                if (brush.foreground.color_mode == Pixel::Color_Mode_RGB && 
                    brush.foreground.color_rgb.r == pixel.foreground.color_rgb.r &&
                    brush.foreground.color_rgb.g == pixel.foreground.color_rgb.g &&
                    brush.foreground.color_rgb.b == pixel.foreground.color_rgb.b) { 
                    break; 
                } else { brush.foreground.color_mode = pixel.foreground.color_mode; }
                brush.foreground.color_rgb = pixel.foreground.color_rgb;
                let rgb = pixel.foreground.color_rgb;
                print_fmt("\e[39m");
                print_fmt("\e[38;2;%;%;%m", rgb.r, rgb.g, rgb.b);
            } break;
            }

            switch (pixel.background.color_mode){
            case Pixel::None: 
                if (brush.background.color_mode != Pixel::Color_Mode::None){
                    print_fmt("\e[49m");
                    brush.background.color_mode = Pixel::Color_Mode::None;
                }
                break;
            case Pixel::Color_Mode_8:   {
                if (brush.background.color_mode == Pixel::Color_Mode_8 && 
                    brush.background.color_8 == pixel.background.color_8) { 
                    break; 
                } else { brush.background.color_mode = pixel.background.color_mode; }
                brush.background.color_8 = pixel.background.color_8;
                print_fmt("\e[49m");
                u8 color_code = cast(u8) pixel.background.color_8 + 10;
                print_fmt("\e[%m", color_code);
            } break;
            case Pixel::Color_Mode_16:  {
                if (brush.background.color_mode == Pixel::Color_Mode_16 && 
                    brush.background.color_16 == pixel.background.color_16) { 
                    break; 
                } else { brush.background.color_mode = pixel.background.color_mode; }
                brush.background.color_16 = pixel.background.color_16;
                print_fmt("\e[49m");
                u8 color_code = cast(u8) pixel.background.color_16 + 10;
                print_fmt("\e[%m", color_code);
            } break;
            case Pixel::Color_Mode_256: {
                if (brush.background.color_mode == Pixel::Color_Mode_256 && 
                    brush.background.color_256 == pixel.background.color_256) { 
                    break; 
                } else { brush.background.color_mode = pixel.background.color_mode; }
                brush.background.color_256 = pixel.background.color_256;
                print_fmt("\e[49m");
                print_fmt("\e[48;5;%m", brush.background.color_256);
            } break;
            case Pixel::Color_Mode_RGB: {
                if (brush.background.color_mode == Pixel::Color_Mode_RGB && 
                    brush.background.color_rgb.r == pixel.background.color_rgb.r &&
                    brush.background.color_rgb.g == pixel.background.color_rgb.g &&
                    brush.background.color_rgb.b == pixel.background.color_rgb.b) { 
                    break; 
                } else { brush.background.color_mode = pixel.background.color_mode; }
                brush.background.color_rgb = pixel.background.color_rgb;
                let rgb = pixel.background.color_rgb;
                print_fmt("\e[49m");
                print_fmt("\e[48;2;%;%;%m", rgb.r, rgb.g, rgb.b);
            } break;
            }

            slice.len = encode_to_utf8(pixel.rune, slice);
            print_fmt("%", string_from(slice));
        }
        print_fmt("\n");
    }

    print_fmt("\e[0m");
}

/*
   If blocking is true
   blocks until an event apears
*/
Tui_Event tui_poll_event(bool blocking = true){
    if (blocking){
        lock(&tui.event_mutex);
        defer (unlock(&tui.event_mutex));
        while (empty(tui.event_queue)){
            wait(&tui.event_cond, &tui.event_mutex);
        }
        let event = pop(&tui.event_queue);
        return event;
    } else {
        if (try_lock(&tui.event_mutex)){
            defer (unlock(&tui.event_mutex));
            let [event, ok] = pop_safe(&tui.event_queue);
            if (ok) { return event; }
            return Tui_Event{.kind = Tui_Event::None};
        } else {
            return Tui_Event{.kind = Tui_Event::None};
        }
    }

    return Tui_Event{.kind = Tui_Event::None};
}
