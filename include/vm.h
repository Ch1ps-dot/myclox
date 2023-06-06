#ifndef clox_vm_h
#define clox_vm_h

#include "chunk.h"
#include "value.h"
#include "table.h"

#define STACK_MAX 256

#define FRAMES_MAX 64
#define STACK_MAX (FRAMES_MAX * UINT8_COUNT)

// represebting a ongoing function call
typedef struct {
    ObjClosure* closure;   // function closure
    uint8_t* ip;           // point to return address
    Value* slots;          // stack of local variable
} CallFrame;

// definition of virtual-machine
typedef struct
{
    CallFrame frames[FRAMES_MAX];
    int frameCount;

    Value stack[STACK_MAX]; // run-time stack
    Value* stackTop; // point to the space uppon top element
    Table globals;   // global variable table
    Table strings;   // string table for interning
    Obj* objects;    // heap memory , an obj linklist for GC
    ObjUpvalue* openUpvalues; // open upvalue list
} VM;

typedef enum {
    INTERPRET_OK,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR
} InterpretResult;

extern VM vm;

void initVM();
void freeVM();
InterpretResult interpret(const char* source);
void push(Value value);
Value pop();

#endif