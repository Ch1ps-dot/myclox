#include <stdio.h>
#include <string.h>

#include "memory.h"
#include "object.h"
#include "value.h"
#include "vm.h"

#define ALLOCATE_OBJ(type, objectType) \
    (type*)allocateObject(sizeof(type), objectType)

bool isObjType(Value value, ObjType type) {
  return IS_OBJ(value) && ((Obj*)AS_OBJ(value))->type == type;
}

// basic function of heap memory allocation
static Obj* allocateObject(size_t size, ObjType type) {
  Obj* object = (Obj*)reallocate(NULL, 0, size);
  object->type = type;

  object->next = vm.objects;
  vm.objects = object;
  return object;
} 

// allocating memory to and initializing ObjString 
static ObjString* allocateString(char* chars, int length) {
  ObjString* string = ALLOCATE_OBJ(ObjString, OBJ_STRING); 
  string->length = length;
  string->chars = chars;
  return string;
}

// allocate memory for a new string
ObjString* takeString(char* chars, int length) {
  return allocateString(chars, length);
}

// using chars copied from constant table to create Objstring
ObjString* copyString(const char* chars, int length) {
  char* heapChars = ALLOCATE(char, length + 1);
  memcpy(heapChars, chars, length);
  heapChars[length] = '\0';
  return allocateString(heapChars, length);
}

// print multiple obj information
void printObject(Value value) {
  switch (OBJ_TYPE(value)) {
    case OBJ_STRING:
      printf("%s", AS_CSTRING(value));
      break;
  }
}