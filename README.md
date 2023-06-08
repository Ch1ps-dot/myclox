# myclox

## LOX语言的简单实现

- 根据[crafting interpreters](http://www.craftinginterpreters.com/)，一书中lox语言解释器的C语言版本的实现，添加了一些注释和作者书中所提及但未实现的功能。
- ./include包含了所有xxx.h文件；./src下包含了所有xxx.c文件；./testfile文件下包含了一些测试用lox程序，文件名即对应的章节，如果能成功运行测试文件，一般就代表对应章节通过。
- 项目采用cmake链接和编译，正常情况下把目录下的./build文件夹情况，在./build下运行命令行命令**cmake ..**而后直接**make**，如果不出意外就能得到可执行的clox解释器。

---

## UPDATE
- 2023-06-08
  - finish garbage collector
- 2023-06-05
  - finish closure

- 2023-06-04
  - finish function
  
- 2023-05-30
  - finish jump forth and back
  
- 2023-05-29
  - finish local variable

- 2023-05-29
  - finish global variable

- 2023-05-26
  - finish hash table
  
- 2023-05-25
  - finish string operation

- 2023-05-13
  - add some new types included boolean, nil and related instruction parsing functions.
  - add functions to parse equalty operators.

- 2023-05-10
  - completed the chapter of parser.

- 2023-05-03
  - completed the structure of scanner. Initially setting up the tokens analyse section. 
  
- 2023-05-01
  - completed the structure of virtual machine, and the core of vm to dispatch instructions.
  - completed all chllenge under this chapter. Tbn, they are mostly answering by text but not code.

- 2023-04-27
  - completed the definition and manipulation of bytecode chunk.
  - completed the challenge one of chapter 14: implementation the struct to store line information by run-line code as well as functions to allocate memory and resolve for that. 