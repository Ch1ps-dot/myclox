# myclox

## LOX语言的简单实现

- 根据[crafting interpreters](http://www.craftinginterpreters.com/)，一书中lox语言解释器的C语言版本的实现，添加了一些注释和作者书中所提及但未实现的功能。由于文件都放在master分支下，克隆仓库后请切换至master分支。
- Its annoying to install input method in chinese on Unbuntu so that i will write in English (maybe chinglish).
- In order to set up the project, i have written cmake file for it. Pretended that you have installed cmake on computer, **cd** into the build directory and delete all files under this directiry. After that, run command line  **cmake ..** then **make**.
- This project finished on Unbuntu 22.04. If there are some errors about compile, at least the source code is fine. Try your best to compile these code by yourself.

---

## UPDATE
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