// the headers provide with NULL, size_t, bool and uint8_t.
#ifndef clox_common_h
#define clox_common_h

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

// #define DEBUG_PRINT_CODE
// #define DEBUG_TRACE_EXECUTION

#define UINT8_COUNT (UINT8_MAX + 1)

typedef struct Obj Obj;

// data type
typedef enum {
  VAL_BOOL,
  VAL_NIL, 
  VAL_NUMBER,
  VAL_OBJ
} ValueType;

// structure of clox Value
typedef struct Value{
  ValueType type;
  union {
    bool boolean;
    double number;
    Obj* obj;
  } as; 
} Value;

typedef struct {
  int capacity;
  int count;
  Value* values;
} ValueArray;

typedef struct Chunk
{
    int count;     // the amount of elemnet in array
    int capacity;  // the capacity of dynamic array
    uint8_t* code; // memory space stores instructions
    int* lines;
    
    uint16_t* runlines;   // the map between weight and line number
                          // higher 8 bits represent line number, 
                          // lower 8 bits represent instruction number

    int cnt_runlines;     // the count of run-lines code
    int cur_line;         // current line
    ValueArray constants; // constant pool
} Chunk;

typedef enum {
  OBJ_FUNCTION,
  OBJ_STRING,
  OBJ_NATIVE,
} ObjType;

typedef struct Obj {
  ObjType type;
  struct Obj* next;
} Obj;

typedef struct ObjString
{
    Obj obj;       // base struct
    int length;
    char* chars;
    uint32_t hash; // hash value
} ObjString;

typedef Value (*NativeFn)(int argCount, Value* args);
typedef struct {
    Obj obj;
    NativeFn function;
} ObjNative;

typedef struct ObjFunction
{
    Obj obj;      // base struct
    int arity;
    Chunk chunk;
    ObjString* name;
} ObjFunction;

#endif
