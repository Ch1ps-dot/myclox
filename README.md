# myclox

## LOX语言的简单实现

- 根据[crafting interpreters](http://www.craftinginterpreters.com/)，一书中lox语言解释器的C语言版本的实现，添加了一些注释和作者书中所提及但未实现的功能。
- ./include包含了所有xxx.h文件；./src下包含了所有xxx.c文件；./testfile文件下包含了一些测试用lox程序，文件名即对应的章节，如果能成功运行测试文件，一般就代表对应章节通过。
- 项目采用cmake链接和编译，正常情况下把目录下的./build文件夹清空，在./build下运行命令行命令**cmake ..**而后直接**make**，如果不出意外就能得到可执行的clox解释器。
- 项目笔记：https://www.zhihu.com/question/21515496/answer/3046022315

---

## Lox语法规则

Lox语言是一门简单的教学语言，它具有动态类型、面向对象的特性。接下来我会介绍一些Lox的语法规则。

### 运行方式

clox解释器在不传递文件地址的情况下是以REPL方式运行的，每句语句后需要加上分号。如果传递给它源文件的地址，他就会直接解释并执行源文件（寻址方式和当前操作系统一致）。在REPL模式下如果输入exit，程序就会退出。

```c
// linux
./clox; // repl模式
./clox test.txt // 运行源代码文件
```

### 数据类型

Lox是一门动态语言，总共有三种数据类型，真值、数值、字符串。

- 真值：支持真和假
- 数值：支持整数和十进制小数。
- 字符串：支持用双引号包括的字符串。
- 空值：用nil表示没有被初始化的值，即空值。

```c
true;
false;
1234;
12.34;
"i am a string"
```

### 表达式

表达式的作用是产生值。Lox支持基本的四则运算：加减乘除、支持负号、支持基本的比较负号。除此之外还支持涉及真值的逻辑运算，有与或运算。Lox的运算符合常见的优先级。

```c
add + me;
subtract - me;
multiply * me;
divide / me;

-negateMe;

less < than;
lessThan <= orEqual;
greater > than;
greaterThan >= orEqual;

!true;  // false.
!false; // true.

false or false; // false.
true or false;  // true.
```

### 句子

句子的作用是产生影响，例如改变状态、输入输出。Lox将print语句视作一个基本语句，而不是一个函数。每个句子都要以分号结尾，除此之外Lox中还支持语句块。

```c
print "Hello, world!";

{
  print "One statement.";
  print "Two statements.";
}
```

### 变量

Lox中的变量用var句子声明，未初始化的变量值为nil。

```C
var breakfast = "bagels";
print breakfast; // "bagels".
breakfast = "beignets";
print breakfast; // "beignets".
```

### 控制流

Lox支持各种控制流关键词，包括if和else、while循环、for循环。

```c
if (condition) {
  print "yes";
} else {
  print "no";
}

var a = 1;
while (a < 10) {
  print a;
  a = a + 1;
}

for (var a = 1; a < 10; a = a + 1) {
  print a;
}
```

### 函数

Lox中的函数调用和C中的类似，而声明需要用到fun关键字。默认情况下函数返回nil。

```c
fun returnSum(a, b) {
  return a + b;
}
returnSum(1, 2) // 3
```

### 闭包

Lox中的函数支持嵌套和闭包调用，即函数可以调用和它在同一个作用域内的本地变量。闭包是一个很有意思的性质，它是从lisp中借鉴来的。

```c
fun returnFunction() {
  var outside = "outside";

  fun inner() {
    print outside;
  }

  return inner;
}

var fn = returnFunction();
fn(); // outside
```

### 类

Lox中的类用关键词class声明。Lox的类里没有字段，只包括各种方法。像很多动态语言一样，Lox的对象可以随意地加入新的变量。除此之外Lox中地类还可以包括一个特殊的init方法。如果你声明了这个方法，你就必须在实例化类的时候初始化一些字段。

```c
class Breakfast {
  cook() {
    print "Eggs a-fryin'!";
  }

  serve(who) {
    print "Enjoy your breakfast, " + who + ".";
  }
}

// Store it in variables.
var someVariable = Breakfast;

// Pass it to functions.
someFunction(Breakfast);
```

```C
class Breakfast {
  serve(who) {
    print "Enjoy your " + this.meat + " and " +
        this.bread + ", " + who + ".";
  }

  // ...
}

var breakfast = Breakfast;
breakfast.meat = "sausage";
breakfast.bread = "sourdough";
```

```C
class Breakfast {
  init(meat, bread) {
    this.meat = meat;
    this.bread = bread;
  }

  // ...
}

var baconAndToast = Breakfast("bacon", "toast");
baconAndToast.serve("Dear Reader");
// "Enjoy your bacon and toast, Dear Reader."
```

### 标准库函数

Lox中内置的函数目前只有两个print和clock。

```c
// print value
var a = 1;
print a;

// return numbers of second since program started
clock()
```

---

## UPDATE
- 2023-06-14
  - finish methods and initializers

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