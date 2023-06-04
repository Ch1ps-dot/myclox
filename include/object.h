#ifndef clox_object_h
#define clox_object_h

#include "common.h"
#include "value.h"
#include "chunk.h"


// extract the object tag types from a given Value
#define OBJ_TYPE(value)        (AS_OBJ(value)->type)

// macro to validate obj pointer cast
#define IS_STRING(value)       isObjType(value, OBJ_STRING)
#define IS_FUNCTION(value)     isObjType(value, OBJ_FUNCTION)
#define IS_NATIVE(value)       isObjType(value, OBJ_NATIVE)

// convert lox obj to c type
#define AS_STRING(value)       ((ObjString*)AS_OBJ(value))
#define AS_FUNCTION(value)     ((ObjFunction*)AS_OBJ(value))
#define AS_CSTRING(value)      (((ObjString*)AS_OBJ(value))->chars)
#define AS_NATIVE(value) \
    (((ObjNative*)AS_OBJ(value))->function)

// typedef enum {
//   OBJ_FUNCTION,
//   OBJ_STRING,
// } ObjType;

// typedef struct Obj {
//   ObjType type;
//   struct Obj* next;
// } Obj;

// typedef struct ObjString
// {
//     Obj obj;       // base struct
//     int length;
//     char* chars;
//     uint32_t hash; // hash value
// } ObjString;

// typedef struct ObjFunction
// {
//     Obj obj;      // base struct
//     int arity;
//     Chunk chunk;
//     ObjString* name;
// } ObjFunction;

ObjFunction* newFunction();
ObjNative* newNative(NativeFn function);
ObjString* takeString(char* chars, int length);
ObjString* copyString(const char* chars, int length);

bool isObjType(Value value, ObjType type); 

#endif