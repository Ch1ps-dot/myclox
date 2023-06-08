#include <stdio.h>
#include <string.h>

#include "memory.h"
#include "object.h"
#include "value.h"
#include "vm.h"
#include "table.h"

#define ALLOCATE_OBJ(type, objectType) \
    (type*)allocateObject(sizeof(type), objectType)

bool 
isObjType(Value value, ObjType type) {
  return IS_OBJ(value) && ((Obj*)AS_OBJ(value))->type == type;
}

// allocate memory for obj struct
static Obj* 
allocateObject(size_t size, ObjType type) {
    Obj* object = (Obj*)reallocate(NULL, 0, size);
    object->type = type;
    object->isMarked = false;

    object->next = vm.objects;
    vm.objects = object;

    #ifdef DEBUG_LOG_GC
      printf("%p allocate %zu for %d\n", (void*)object, size, type);
    #endif

    return object;
} 

// create new objclass
// return its name
ObjClass* 
newClass (ObjString* name) {
  ObjClass* klass = ALLOCATE_OBJ(ObjClass, OBJ_CLASS);
  klass->name = name; 
  return klass;
}

// create new function 
// return its memory address
ObjFunction* 
newFunction() {
    ObjFunction* function = ALLOCATE_OBJ(ObjFunction, OBJ_FUNCTION);
    function->arity = 0;
    function->name = NULL;
    function->upvalueCount = 0;
    initChunk(&function->chunk);
    return function;
}

// create new instance
// according to class
ObjInstance* 
newInstance(ObjClass* klass) {
  ObjInstance* instance = ALLOCATE_OBJ(ObjInstance, OBJ_INSTANCE);
  instance->klass = klass;
  initTable(&instance->fields);
  return instance;
}

// create new closure
ObjClosure* 
newClosure(ObjFunction* function) {
    ObjUpvalue** upvalues = ALLOCATE(ObjUpvalue*,
                                    function->upvalueCount);
    for (int i = 0; i < function->upvalueCount; i++) {
      upvalues[i] = NULL;
    }
    ObjClosure* closure = ALLOCATE_OBJ(ObjClosure, OBJ_CLOSURE);
    closure->function = function;
    closure->upvalues = upvalues;
    closure->upvalueCount = function->upvalueCount;
    return closure;
}

// create new native function
ObjNative* newNative(NativeFn function) {
  ObjNative* native = ALLOCATE_OBJ(ObjNative, OBJ_NATIVE);
  native->function = function;
  return native;
}

// allocating memory in heap for ObjString 
// and insert string into string intern table.
static ObjString* 
allocateString(char* chars, int length, uint32_t hash) {
  ObjString* string = ALLOCATE_OBJ(ObjString, OBJ_STRING); 
  string->length = length;
  string->chars = chars;
  string->hash = hash;

  // fulfill GC
  push(OBJ_VAL(string));
  tableSet(&vm.strings, string, NIL_VAL);
  pop();
  return string;
}

// hash function FNV-1a
static uint32_t hashString(const char* key, int length) {
  uint32_t hash = 2166136261u;
  for (int i = 0; i < length; i++) {
    hash ^= (uint8_t)key[i];
    hash *= 16777619;
  }
  return hash;
}

// string interning before  
// allocate memory for new string
ObjString* 
takeString(char* chars, int length) {
  uint32_t hash = hashString(chars, length);
  ObjString* interned = tableFindString(&vm.strings, chars, length,
                                        hash);
  if (interned != NULL) {
    FREE_ARRAY(char, chars, length + 1);
    return interned;
  }

  return allocateString(chars, length, hash);
}

// using chars copied from source code to create ObjString
ObjString* 
copyString(const char* chars, int length) {
  uint32_t hash = hashString(chars, length);

  // string inerning 
  ObjString* interned = tableFindString(&vm.strings, chars, length,
                                        hash);
  if (interned != NULL) return interned;

  char* heapChars = ALLOCATE(char, length + 1);
  memcpy(heapChars, chars, length);
  heapChars[length] = '\0';
  return allocateString(heapChars, length, hash);
}

// create new upvalue
ObjUpvalue* 
newUpvalue(Value* slot) {
  ObjUpvalue* upvalue = ALLOCATE_OBJ(ObjUpvalue, OBJ_UPVALUE);
  upvalue->location = slot;
  upvalue->next = NULL;
  upvalue->closed = NIL_VAL;
  return upvalue;
}

// print information of ObjFunction
static void 
printFunction(ObjFunction* function) {
  if (function->name == NULL) {
    printf("<script>");
    return;
  }
  printf("<fn %s>", function->name->chars);
}

// print multiple obj information
void 
printObject(Value value) {
  switch (OBJ_TYPE(value)) {
    case OBJ_CLASS:
      printf("%s", AS_CLASS(value)->name->chars);
      break;
    case OBJ_CLOSURE:
      printFunction(AS_CLOSURE(value)->function);
      break;
    case OBJ_FUNCTION:
      printFunction(AS_FUNCTION(value));
      break;
    case OBJ_STRING:
      printf("%s", AS_CSTRING(value));
      break;
    case OBJ_INSTANCE:
      printf("%s instance",
             AS_INSTANCE(value)->klass->name->chars);
      break;
    case OBJ_NATIVE:
      printf("<native fn>");
      break;
    case OBJ_UPVALUE:
      printf("upvalue");
      break;
  }
}