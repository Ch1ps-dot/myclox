cmake_minimum_required(VERSION 3.10)

project(clox)

add_executable(clox chunk.c debug.c main.c memory.c value.c vm.c compiler.c scanner.c)