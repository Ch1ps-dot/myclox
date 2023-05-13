#include <stdio.h>
#include <stdarg.h>

#include "common.h"
#include "vm.h"
#include "debug.h"
#include "compiler.h"

VM vm;

// initialize the stack
static void 
resetStack() {
    vm.stackTop = vm.stack;
}

// deal with runtime error
static void
runtimeError(const char* format,...) {
    // special struct to deal with a varing number of arguments
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fputc("\n",stderr);

    size_t instruction = vm.ip - vm.chunk->code - 1;
    int line = vm.chunk->lines[instruction];
    fprintf(stderr, "[line %d] in script\n", line);
    resetStack();
}

void 
initVM() {
    resetStack();
}

void
freeVM() {

}

// push elememt into stack
void 
push(Value value) {
    *vm.stackTop = value;
    vm.stackTop++;
}

Value 
pop() {
    vm.stackTop--;
    return *vm.stackTop;
}

static Value
peek(int distance) {
    return vm.stackTop[-1-distance];
}

// nil and false are falsey and other value behaves like true
static bool
isFalsey(Value value) {
    return IS_NIL(value) || (IS_BOOL(value) && !AS_BOOL(value));
}

// interpret loop of bytecode
static InterpretResult
run() {
    #define READ_BYTE() (*vm.ip++) // macro to read and advance instruction
    #define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()]) // macro to read constant
    #define BINARY_OP(valueType, op) \
    do { \
        if (!IS_NUMBER(peek(0)) || !IS_NUMBER(peek(1))) { \
            runtimeError("Operands must be numbers."); \
            return INTERPRET_RUNTIME_ERROR; \
        } \
        double b = AS_NUMBER(pop()); \
        double a = AS_NUMBER(pop()); \
        push(valueType(a op b)); \
    } while (false)


    for (;;) {

        // debug execution
        #ifdef DEBUG_TRACE_EXECUTION
            // display state of stack
            printf("        ");
            for (Value* slot = vm.stack; slot < vm.stackTop; slot++) {
                printf("[ ");
                printValue(*slot);
                printf(" ]");
            }
            printf("\n");

            // display instruction
            disassembleInstruction(vm.chunk,
                (int)(vm.ip - vm.chunk->code));
        #endif

        // instructions dispatch
        uint8_t instruction;
        switch (instruction = READ_BYTE())
        {
            case OP_CONSTANT: {
                Value constant = READ_CONSTANT();
                push(constant);
                break;
            }
            case OP_NIL:      push(NIL_VAL); break;
            case OP_TRUE:     push(BOOL_VAL(true)); break;
            case OP_FALSE:    push(BOOL_VAL(false));break;
            case OP_EQUAL: {
                Value b = pop();
                Value a = pop();
                push(BOOL_VAL(valuesEqual(a,b)));
                break;
            }
            case OP_GREATER:  BINARY_OP(BOOL_VAL, >);  break;
            case OP_LESS:     BINARY_OP(BOOL_VAL, <);  break;
            case OP_ADD:      BINARY_OP(NUMBER_VAL,+); break;
            case OP_SUBTRACT: BINARY_OP(NUMBER_VAL,-); break;
            case OP_MULTIPLY: BINARY_OP(NUMBER_VAL,*); break;
            case OP_DIVIDE:   BINARY_OP(NUMBER_VAL,/); break;
            case OP_NOT:
                push(BOOL_VAL(isFalsey(pop())));
                break;
            case OP_NEGATE:
                if (!IS_NUMBER(peek(0))) {
                    runtimeError("Operand must be a number");
                    return INTERPRET_RUNTIME_ERROR;
                }
                *(vm.stackTop - 1) = NUMBER_VAL(-AS_NUMBER(*(vm.stackTop - 1))); 
                break;
            case OP_RETURN: {
                printValue(pop());
                printf("\n");
                return INTERPRET_OK;
            }
        }
    }
    
    #undef READ_BYTE
    #undef READ_CONSTANT
    #undef BINARY_OP
}

// interpret instructions and run in vm
InterpretResult 
interpret(const char* source) {
    Chunk chunk;
    initChunk(&chunk);
    
    if (!compile(source, &chunk)) {
        freeChunk(&chunk);
        return INTERPRET_COMPILE_ERROR;
    }

    vm.chunk = &chunk;
    vm.ip = vm.chunk->code;

    InterpretResult result = run();

    freeChunk(&chunk);
    return result;
}