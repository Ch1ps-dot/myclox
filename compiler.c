#include <stdio.h>

#include "common.h"
#include "compiler.h"
#include "scanner.h"


// compeiler for clox
void
compile(const char* source) {
    initScanner(source);
    int line = -1;
    for (;;) {
        Token token =scanToken();
        if (token.line != line) {
            printf("%4d ", token.line);
            line = token.line;
        } else {
            printf("    | ");
        }
        printf("%2d  '%.*s'\n", token.type, token.length, token.start); // print given sized token

        if (token.type == TOKEN_EOF) break;
    }
}