#include <stdlib.h>

#include "memory.h"
#include "vm.h"

// dynamic memory management
void* reallocate(void* pointer, size_t oldSize, size_t newSize) {
    if (newSize == 0) {
        free(pointer);
        return NULL;
    }

    // realloc is implementation of malloc, it returns the first address of new memory.
    // If the memory after the block is unused,
    // realloc return the same pointer. Otherwise,
    // it allocates a new block of memory of the desired size.
    void* result = realloc(pointer, newSize);
    if (result == NULL) exit(1); 
    return result;
}


static void freeObject(Obj* object) {
  switch (object->type) {
    case OBJ_FUNCTION: {
      ObjFunction* function = (ObjFunction*)object;
      freeChunk(&function->chunk);
      FREE(ObjFunction, object);
      break;
    }
    case OBJ_STRING: {
      ObjString* string = (ObjString*)object;
      FREE_ARRAY(char, string->chars, string->length + 1);
      FREE(ObjString, object);
      break;
    }
    case OBJ_NATIVE: {
      FREE(OBJ_NATIVE, object);
      break;
    }
  }
}

// baby GC
void freeObjects() {
  Obj* object = vm.objects;
  while (object != NULL) {
    Obj* next = object->next;
    freeObject(object);
    object = next;
  }
}