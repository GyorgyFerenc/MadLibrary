#pragma once

#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include "core.hpp"
#include "os.hpp"
#include "tui/canvas.hpp"

const let ESC = "\u001b";
const let STDIN  = 0;
const let STDOUT = 1;
const let STDERR = 2;

struct Tui_Event{
    enum Kind{
        None,
        Key,
        Stop,
    } kind = None;

    union {
        struct {
            Rune rune;
        } key;
    };
};

struct {
    int tty;
    struct termios old_termios;
    bool altscreen = false;
    Thread handler;

    Pipe stop;

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

void tui_handler(Thread_Data){
    int maxfd = 0;
    fd_set set;
    FD_ZERO(&set);

    FD_SET(tui.stop.read, &set);
    if (maxfd < tui.stop.read) maxfd = tui.stop.read;

    FD_SET(STDIN, &set);
    if (maxfd < STDIN) maxfd = tui.stop.read;

    for (;;){
        let copy_set = set;
        int ret = select(maxfd + 1, &copy_set, NULL, NULL, NULL);
        if (FD_ISSET(tui.stop.read, &copy_set)){
            tui_push_event(Tui_Event{
                .kind = Tui_Event::Stop,
            });
            break;
        } else if (FD_ISSET(STDIN, &copy_set)){
            u8 buffer[100] = {0};
            let n = read(STDIN, &buffer[0], 100);
            
            if (buffer[0] == '\e'){
                //Todo(Ferenc): implement escaped characters like F1 
            } else {
                let [r, _] = decode_rune_utf8(buffer);

                tui_push_event(Tui_Event{
                    .kind = Tui_Event::Key,
                    .key = {
                        .rune = r,
                    },
                });
            }
        }
    }
}

void tui_init(){
    tui.tty = open("/dev/tty", O_RDWR);

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

    init(&tui.stop);
    start(&tui.handler, tui_handler);
}

void tui_out(String str){
    let r = raw(str);
    //Todo(Ferenc): Move this to the os
    write(STDOUT, r.buffer, r.len);
}

void enter_altscreen() {
    tui_out(alias("\e[?1049h\e[H"));
    tui.altscreen = true;
}

void leave_altscreen() {
    tui_out(alias("\e[?1049l"));
    tui.altscreen = true;
}

void save_cursor(){
    tui_out(alias("\e[s"));
}

void restore_cursor(){
    tui_out(alias("\e[u"));
}

void tui_stop(){
    bool t = true;
    write(tui.stop.write, &t, 1);
}

void tui_deinit(){
    if (tcsetattr(0, TCSADRAIN, &tui.old_termios) < 0) perror("deinit: Termios tcsetattr");

    if (tui.altscreen){
        leave_altscreen();
    }

    tui_stop();
    join(tui.handler);
}


void tui_print(Canvas c){
    u8 buffer[5] = {0};
    let slice = Slice<u8>{buffer, 5};

    for (let y = 0; y < c.height; y++){
        for (let x = 0; x < c.width; x++){
            let pixel = *get(c, x, y);
            slice.len = encode_to_utf8(pixel.rune, slice);
            tui_out(string_from(slice));
        }
        tui_out(alias("\n"));
    }
}

/*
   If blocking is true
   blocks until an event apears
*/
Tui_Event tui_poll_event(bool blocking = true){
    if (blocking){
        for (;;){
            lock(&tui.event_mutex);
            wait(&tui.event_cond, &tui.event_mutex);
            defer (unlock(&tui.event_mutex));

            let [event, ok] = pop_safe(&tui.event_queue);
            if (ok) {
                return event;
            }
        }
    } else {
        if (try_lock(&tui.event_mutex)){
            defer (unlock(&tui.event_mutex));
            let [event, ok] = pop_safe(&tui.event_queue);
            if (ok) {
                return event;
            }

            return Tui_Event{.kind = Tui_Event::None};
        } else {
            return Tui_Event{.kind = Tui_Event::None};
        }
    }

    UNREACHABLE;
}
