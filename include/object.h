#ifndef clox_object_h
#define clox_object_h

#include "common.h"
#include "value.h"

typedef struct Value Value;

// extract the object tag types from a given Value
#define OBJ_TYPE(value)        (AS_OBJ(value)->type)

// macro to validate obj pointer cast
#define IS_STRING(value)       isObjType(value, OBJ_STRING)

// convert lox obj to c type
#define AS_STRING(value)       ((ObjString*)AS_OBJ(value))
#define AS_CSTRING(value)      (((ObjString*)AS_OBJ(value))->chars)

typedef enum {
  OBJ_STRING,
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

ObjString* takeString(char* chars, int length);
ObjString* copyString(const char* chars, int length);

bool isObjType(Value value, ObjType type); 

#endif