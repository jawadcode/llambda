#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"
#include "vec.h"

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

void run(const char *source) {
    Lexer lexer = new_lexer(source);
    TokenVec tokens = TokenVec_new();

    while (true) {
        Token token = next_token(&lexer);
        puts(token_to_string(source, token));
        TokenVec_push(&tokens, token);
        if (token.kind == TK_EOF)
            break;
    }
}

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

void run_file(const char *path) {
    FILE *file = fopen(path, "rb");
    if (file == NULL) {
        fputs("Could not access file ", stdout);
        puts(path);
        return;
    }

    fseek(file, 0L, SEEK_END);
    size_t file_size = ftell(file);
    rewind(file);

    char *buffer = (char *)malloc(file_size + 1);
    size_t bytes_read = fread(buffer, sizeof(char), file_size, file);
    buffer[bytes_read] = '\0';

    fclose(file);
    run(buffer);
    free(buffer);
}

int main(int argc, char **argv) {
    if (argc > 1) {
        const char *path = argv[1];
        run_file(path);
    } else {
        puts("Llambda Interpreter:\n");
        repl();
    }
    return 0;
}