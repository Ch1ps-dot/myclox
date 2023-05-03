#include <stdio.h>
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

// interpret loop of bytecode
static InterpretResult
run() {
    #define READ_BYTE() (*vm.ip++) // macro to read and advance instruction
    #define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()]) // macro to read constant
    #define BINARY_OP(op) \
        do {   \
            double b = pop(); \
            double a = pop(); \
            push(a op b); \
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
            case OP_ADD:      BINARY_OP(+); break;
            case OP_SUBTRACT: BINARY_OP(-); break;
            case OP_MULTIPLY: BINARY_OP(*); break;
            case OP_DIVIDE:   BINARY_OP(/); break;
            case OP_NEGATE:   *(vm.stackTop - 1) = -*(vm.stackTop - 1); break;
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
    compile(source);
    return INTERPRET_OK;
}