#ifndef clox_object_h
#define clox_object_h

#include "common.h"
#include "value.h"
#include "chunk.h"


// extract the object tag types from a given Value
#define OBJ_TYPE(value)        (AS_OBJ(value)->type)

// macro to validate obj pointer cast
#define IS_CLASS(value)        isObjType(value, OBJ_CLASS)
#define IS_CLOSURE(value)      isObjType(value, OBJ_CLOSURE)
#define IS_STRING(value)       isObjType(value, OBJ_STRING)
#define IS_FUNCTION(value)     isObjType(value, OBJ_FUNCTION)
#define IS_NATIVE(value)       isObjType(value, OBJ_NATIVE)
#define IS_INSTANCE(value)     isObjType(value, OBJ_INSTANCE)

// convert lox obj to c type
#define AS_CLASS(value)        ((ObjClass*)AS_OBJ(value))
#define AS_CLOSURE(value)      ((ObjClosure*)AS_OBJ(value))
#define AS_STRING(value)       ((ObjString*)AS_OBJ(value))
#define AS_FUNCTION(value)     ((ObjFunction*)AS_OBJ(value))
#define AS_CSTRING(value)      (((ObjString*)AS_OBJ(value))->chars)
#define AS_INSTANCE(value)     ((ObjInstance*)AS_OBJ(value))
#define AS_NATIVE(value) \
    (((ObjNative*)AS_OBJ(value))->function)

/**
 * I moved some code to <common.h>,
 * because of some compiling errors caused by bad structure of header file. 
*/

ObjClass* newClass(ObjString* name);
ObjFunction* newFunction();
ObjInstance* newInstance(ObjClass* klass);
ObjClosure* newClosure(ObjFunction* function);
ObjNative* newNative(NativeFn function);
ObjString* takeString(char* chars, int length);
ObjString* copyString(const char* chars, int length);
ObjUpvalue* newUpvalue(Value* slot);

bool isObjType(Value value, ObjType type); 

#endif