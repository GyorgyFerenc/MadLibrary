#pragma once
#include <sys/wait.h>
#include <unistd.h>

#include "Core.hpp"

struct Process {
    pid_t child_pid;
    int   from_child, to_child;

    static Option<Process> run(std::function<void()> func) {
        Process proc;

        // pipe[0] -> read, pipe[1] -> write
        int pipe_stdin[2], pipe_stdout[2];

        if (pipe(pipe_stdin)) return Option<Process>::None();
        if (pipe(pipe_stdout)) return Option<Process>::None();

        pid_t p = fork();
        if (p < 0) {
            // Fork failed
            return Option<Process>::None();
        } else if (p == 0) {
            // child
            close(pipe_stdin[1]);
            dup2(pipe_stdin[0], 0);
            close(pipe_stdout[0]);
            dup2(pipe_stdout[1], 1);
            func();
            exit(99);
        }

        proc.child_pid = p;
        proc.to_child = pipe_stdin[1];
        proc.from_child = pipe_stdout[0];

        close(pipe_stdin[0]);
        close(pipe_stdout[1]);

        return Option<Process>::Some(proc);
    }

    static Option<Process> open_no_capture(const char* cmdline) {
        // TODO: dont depend on /bin/sh
        // TODO: Do it better

        Process proc;

        pid_t p = fork();
        if (p < 0) {
            // Fork failed
            return Option<Process>::None();
        } else if (p == 0) {
            // child
            execl("/bin/sh", "sh", "-c", cmdline, NULL);
            perror("execl");
            exit(99);
        }

        return Option<Process>::Some(proc);
    }

    static Option<Process> open(const char* cmdline) {
        // TODO: dont depend on /bin/sh

        Process proc;

        // pipe[0] -> read, pipe[1] -> write
        int pipe_stdin[2], pipe_stdout[2];

        if (pipe(pipe_stdin)) return Option<Process>::None();
        if (pipe(pipe_stdout)) return Option<Process>::None();

        pid_t p = fork();
        if (p < 0) {
            // Fork failed
            return Option<Process>::None();
        } else if (p == 0) {
            // child
            close(pipe_stdin[1]);
            dup2(pipe_stdin[0], 0);
            close(pipe_stdout[0]);
            dup2(pipe_stdout[1], 1);
            execl("/bin/sh", "sh", "-c", cmdline, NULL);
            perror("execl");
            exit(99);
        }

        proc.child_pid = p;
        proc.to_child = pipe_stdin[1];
        proc.from_child = pipe_stdout[0];

        close(pipe_stdin[0]);
        close(pipe_stdout[1]);

        return Option<Process>::Some(proc);
    }

    void join() {
        int waitstatus;
        waitpid(child_pid, &waitstatus, 0);
    }

    void destroy() {
        close(to_child);
        close(from_child);
    }
};
