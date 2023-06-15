#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "chunk.h"
#include "debug.h"
#include "vm.h"


// init debug mode
int debugCode = 0;
int debugExe = 0;
int debugGc = 0;

// start clox with repl model  
static void
repl() {
    char line[1024];
    for (;;) {
        printf("> ");

        if (!fgets(line, sizeof(line), stdin)) {
            printf("\n");
            break;
        }
        if (strcmp(line,"exit") == 0) {
            exit(0);
        }
        interpret(line);
    }
}

// start clox with reading source code
static char*
readFile(const char* path) {
    FILE* file = fopen(path, "rb");

    // handle openfile error
    if (file == NULL) {
        fprintf(stderr, "Could not open file \"%s\".\n", path);
        exit(74);
    }

    fseek(file, 0L, SEEK_END);     // move pointer to the end of file
    size_t fileSize = ftell(file); // tell program how many bytes from start to end
    rewind(file);                  // back pointer to the beginning

    char* buffer = (char*)malloc(fileSize + 1);
    size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);
    // handle error 
    if (bytesRead < fileSize) {
        fprintf(stderr, "Could not read file \"%s\".\n", path);
        exit(74);
    }

    buffer[bytesRead] = '\0';

    fclose(file);
    return buffer;
}

// execute the source code
static void
runFile(const char* path) {
    char* source = readFile(path);
    InterpretResult result = interpret(source);
    free(source);

    if (result == INTERPRET_COMPILE_ERROR) exit(65);
    if (result == INTERPRET_RUNTIME_ERROR) exit(70);
}

int
main(int argc, const char* argv[]) {
    initVM();
    
    if (argc == 1) {
        repl();
    } 
    else if (argc >= 2) {
        int flag = 0;
        for (int i = 1; i < argc; i++) {
            if (strcmp("-c", argv[i]) == 0) {
                debugCode = 1;
            }
            else if (strcmp("-e", argv[i]) == 0) {
                debugExe = 1;
            }
            else if (strcmp("-g", argv[i]) == 0) {
                debugGc = 1;
            }
            else {
                flag = 1;
                runFile(argv[i]);
                break;
            }
        }
        if (!flag) {
            repl();
        }
    } 
    else {
        fprintf(stderr, "Usage: clox [path]\n");
        exit(64);
    }
    
    freeVM();
    
    return 0;
}
