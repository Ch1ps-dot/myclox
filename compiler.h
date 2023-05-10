#ifndef clox_compiler_h
#define clox_complier_h

#include "vm.h"

bool compile(const char* source, Chunk* chunk);

#endif