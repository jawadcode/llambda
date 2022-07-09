#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void run_cmd(const char *cmd) {
    switch (cmd[0]) {
    case 'e':
        if (strcmp(cmd + 1, "xit\n") == 0) {
            puts("Bye bye...\n");
            exit(0);
            break;
        } else {
            goto UNKNOWN_COMMAND;
        }
    case 'h':
        if (strcmp(cmd + 1, "elp\n") == 0) {
            puts("Commands:\n"
                 "    :exit - Exit the REPL\n"
                 "    :help - Display this message\n"
                 "    :quit - Quit the REPL\n");
            break;
        } else {
            goto UNKNOWN_COMMAND;
        }
    case 'q':
        if (strcmp(cmd + 1, "uit\n") == 0) {
            puts("Bye bye...\n");
            exit(0);
            break;
        } else {
            goto UNKNOWN_COMMAND;
        }
    UNKNOWN_COMMAND:
    default:
        fputs("Error: Unknown command ", stdout);
        puts(cmd);
        break;
    }
}

void run(const char *source) { puts(source); }

void repl() {
    char *buffer;
    size_t length;
    ssize_t len_chars;

    while (true) {
        fputs("$ ", stdout);
        fflush(stdout);
        buffer = NULL;
        len_chars = getline(&buffer, &length, stdin);
        if (len_chars >= 1 && buffer[0] == ':') {
            run_cmd(buffer + 1);
        } else {
            run(buffer);
        }
        free(buffer);
    }
}

int main(void) {
    puts("Llambda Interpreter:\n");
    repl();
    return 0;
}