**Language**: simple set of rules of syntaxs to write a program
**Programming language implementation**: is a system for executing [computer programs](https://en.wikipedia.org/wiki/Computer_programs "Computer programs"). There are two general approaches: interpretation, compilication.
## C2 - Terminologies

### Parts of a language
**Scanning/Lexing**
Takes the code stream and converts it to tokens: `(`, `,`, numers, string, etc. Ex:
![|650](Images/Pasted%20image%2020241123214650.png)
**Parsing** 
Takes tokens to build the Parse tree aka AST (abstract syntax tree): 
![|675](Images/Pasted%20image%2020241123214810.png)
**Static analysis** 
We do this for each indentifier (var, declaration). This including type check (for static type languages) and other semantic check. We then store these info in 1 of following ways:
+ Store in a field in the AST node.
+ Create a symbol table with key is the identifer.
+ Most powerful: Transform the tree into entirely new data structure (explained in other section of the book).
**IR - intermediate representations** 
+ Front end of the pipeline is specific to the source language the program is written in. The back end is concerned with the final architecture where the program will run.
+ the middle presentation between the source and target language. This let you support multiple source languages and target platforms with less effort. We write one front end for each source language that produces the IR. Then one back end for  each target architecture. Ex: llvm IR ![](Images/Pasted%20image%2020240114180011.png)
**Optimization**: 
Replace the orignal program with the another one more efficient and produce the same result. Many techniques
+ Constant folding: `pennyArea = 3.14159 * (0.75 / 2) * (0.75 / 2);` ===> optimize: `pennyArea = 0.4417860938;`
**Code generation**
We are at the backend close to sth that the machine can understand. 2 choices:
+ Generate instruction for each type CPU. Overwheel
+ Generate code for a hypothetical machine, called **bytecode**, each instruction is a single byte long. We need to translate bytecode to a specific computer arch. This often done using a **virtual machine**.
**Virtual machine**
A program that emulates a hypothetical chip. Implement your VM in, say, C, and you can run your language on any platform that has a C compiler.
**Runtime**
In each language we need some services to run while the program is running. Ex: garbage collector in Java, sth to keep track type of an obj during execution in dynamic type languages, etc. These stuff called runtime. In compile language, the runtime is auto inserted into the executable file. If a VM is used, runtime lives inside the VM.

### Example of VM - Java VM
+ JVM has a specification to implement. There are many JVMs created by companies: HotSpot JVM (implemented by Oracle), written in mainly in C, C++ so that JVM can be run on any machines that has c compiler. Each OS/arch required a JVM implementation.
+ We have JVM languages - which can be compiled into java bytecode. Ex: JRuby, JPython, Kotlin, Scala. These languages are compatible with Java: can use a Java program and vice versa.
### Shortcut and alternate routes
The prev section mentions every possible phase to impl a compiler. Still, there is some shortcuts:
#### Transpilers
Used another source language as IR. Ex:
+ As most machines is UNIX: these machines already have C compiler. We can then use C as the output of our compiler.
+ Many languages has compiler that produce JS so that it can be exec in the browser.
#### JIT - Just-in-time compiler
[A crash course in just-in-time (JIT) compilers - Mozilla Hacks - the Web developer blog](https://hacks.mozilla.org/2017/02/a-crash-course-in-just-in-time-jit-compilers/)
+ Intepreter cons: 
	+ If a block of code (ex: for loop) is run multi times, intepreter has to re-translate everytime. 
	+ Cannot optimize code before execution time.
+ Compiler cons:
	+ Translation overhead before runtime, may have to translate that won't be used.
JIT: Compile at **run time** some code to improve the perf of interpreter: compile frequently used code and optimize code at run time. For ex:
+ Some browser add a monitor to Js engine, count how many times a particular block of code is exec then decide to compiled frequently used code. 
+ In MIOpen, kernel are compiled at runtime as gpu arch can only be detected at runtime. Based on gpu arch, kernel optimization is done.
### Compilers vs interpreters
Like vegetables and fruits. One is not the negation of the other.
+ Compiler: translate source to another form but not exec it.
+ Interpreter: takes src code and exec immediately.
Ex: CPython is an interpreter but has a compiler. It parse python code to internal bytecode format then immediately executed the bytecode inside VM.
![|675](Images/Pasted%20image%2020241123232100.png)
#### Others
**Single-pass compilers**
Approach in Pascal in C as in the past, memory was so precious that there wasn't enough space to store the entire source file.
**Tree-walk interpreters**
Exec code right after we get AST. Used in students project. **The 1st interpreter in the book uses this way.**
## C3 - Lox language
Dynamic type
Memory managment using GC - Garbage collector
Datatype:
+ Booleans
+ Numbers: only support double precision - 64 bits
+ String
+ Nil
Expression:
+ Arthemetic: +, -, *
+ Compare: >, <, !=, =
+ Logic: and, or, not
+ Grouping with bracket: (a + b) / 2
Statement: print
Variable: declare using var, if not assign, default value is nil
Control flow: if, while, for
Function
+ Argument: the actual value you pass to a function when you call it.
+ Parameter: the variable that holds the value of the argument inside the body of the function.
+ Func without return statement return nil by deafault.
Closure
Objects: there are 2 approaches to impl obj
+ Class based: C++, C#, Java. Choose this approach.
+ Prototype based: Javascript
Classes: field, method, instance init, inherit.
Standlib: impl built-in func: print, clock
## C4 - Scanning - Scanner
The language support 2 modes:
+ Run the whole file
+ Interactive mode: run each line we type in
### Token and lexemes
Lexeme: our code are splited to many lexemes
![](Images/Pasted%20image%2020241210234837.png)
Token: Is a wrapper of lexeme contains other info:
+ type: number, string, bracket, etc
+ line

### Scanning tokens
+ Single char: '+', '-', '(', etc
+ 2 chars: `>=`, `==`, `!=`, etc
+ Longer lexemes: 
	+ Comment: `// abc`
	+ String
	+ Number
+ Reserved words and identifier: `for`, `while`, `and`, var name

## C5 - Representing code - Parser
### Context free grammar
![](Images/Pasted%20image%2020241211174820.png)

To get the language syntax grammar, we use CFG - Context free grammar. CFG contains rules: each rule can lead to a terminate state or another rules. Example:
![](Images/Pasted%20image%2020241211175102.png)
We start with breakfast -> protein "with" breakfast "on the side" -> "poached" "eggs" "with" "sausage" "on the side"

The notation to represent CFG (->, |, and regular expression char `+`, `*`) we use above is called [EBNF](https://en.wikipedia.org/wiki/Extended_Backus%E2%80%93Naur_form). There are other notation

### Expr class design
We have a bunch of object inherit the base class `Expr` like below: rows - classes, columns - methods.
![|600](Pasted%20image%2020241214222131.png)

2 options to design here:
+ Like OOP: write methods inside class. 
	+ Pros: new class - ez. 
	+ Cons: new methods - hard as we have to update all existence class.
+ Functional programming: you don’t have classes with methods. Types and functions are totally distinct. Each funcs has switch case to handle each type.
	+ Pros: new func - ez.
	+ Cons: new type - update all existence functions
	
The book choose visitor. A design pattern to write like OOP style, but define the same method for all types in a single place. This like the combination of OOP and Functional programming method. But still, we have the pros and cons of OOP.

## C6 - Parser
### Parsing grammar
An expression can be present in multiple syntax trees which may return different results. Ex: `6 / 3 - 1`
![|400](Images/Pasted%20image%2020241224220138.png)
We need rules so that we got only 1 syntax tree. 2 rules:
+ Precedence: some ops has higher precedence, should be exec first. Do `* /` before `+ -`. 
+ Associative: in an expression with the same op, what should be exec first.
	+ Left-association: compute from the left side: 5 - 3 - 1 <=> (5 - 3) - 1
	+ Right association: compute from the right side: a = b = c <=> a = (b = c)
+ C rules with precedence from lowest to highest:![](Images/Pasted%20image%2020241224220545.png)

Complete expression grammar:
[here](https://github.com/tenstorrent/tt-metal/issues/13904)
```cpp
expression     → equality ;
equality       → comparison ( ( "!=" | "==" ) comparison )* ;
comparison     → term ( ( ">" | ">=" | "<" | "<=" ) term )* ;
term           → factor ( ( "-" | "+" ) factor )* ;
factor         → unary ( ( "/" | "*" ) unary )* ;
unary          → ( "!" | "-" ) unary
               | primary ;
primary        → NUMBER | STRING | "true" | "false" | "nil"
               | "(" expression ")" ;
```

### Recursive descent parsing
There are many parse techiniques: LL, LR, LALR. We would use recursive descent, a technique also used in gcc, v8 (the JavaScript VM in Chrome). It is considered a top-down parser: 
![](Images/Pasted%20image%2020241228171017.png)

**Similar to scanner, which process through the sequence of characters, parser process throught a sequence of tokens.**

Present a rule:
```cpp
// factor → unary ( ( "/" | "*" ) unary )* ;
std::shared_ptr<Expr> Parser::factor() {
    auto left = unary();

    while (match({TokenType::SLASH, TokenType::STAR})) {
        auto op = previous_tok();
        auto right = unary();
        left = std::make_shared<Binary>(left, op, right);
    }

    return left;
}
```
### Parsing error recovery 
A parse has 2 jobs:
+ Produce a syntax tree with a valid given sequence of tokens.
+ Detect & report err if the sequence of toks is invalid.

A good parse would:
+ Report as many errors in the code as possible.
+ Minimize cascade error: when an error I found the parser can be confuse and report ghost errors the vanish when the first error is fix. We should minimize cascade err as I make user think their code is worse than it is.

**Error recovery**: a parser responds to an error and keeps going to look for later errors. **Panic mode** is most used recovery technique.

## C7 - Evaluating expression

### Eval expr
There are many ways of a language implementation: compile it to machine code, another high level language, bytecode. The simplest way is to exec the syntax tree itself.
+ We create class following vistor design pattern to evaluate the value of expression: Literal, Group, Unary, Binary. Check the code in interpreter_visitor.cpp
### Check runtime error
in the parser code we can check find syntax/static error before running the code. Runtime err can only be detect at runtime. Ex:
```cpp

int dosth() {
	return random % 1;
}

std::cout << 1 / doSth(); // can divide by 0
```

We use exception to handle runtime err. Note that in interactive mode, when user got a static or runtime err, the session should not be exit, user can continue typing after the err is reported.

Example of runtime err we have to verify: In binary expr with `*` operator both left, right `operand` must be a number.

FYI: static type languages can check type at compile time while dynamic type languages must do it at runtime. However, most static type languages also has another checker exec at runtime to check err which can not be found at compile time. Ex:
```java
Object[] stuff = new Integer[1];
// Compile without err but err raised at runtime
stuff[0] = "not an int!";
```
## C8 - Statements and state
In previous chapters, we able to parse and evaluate result of a singler chapter. However, a program contains other things: variable (state), statements.
### Statements (stmt)
There are many stmts. For now, category as 2 types:
+ print statement
+ other statement
**Parsing statement**:
+ Add new parse rules
```
program        → statement* EOF ;

statement      → exprStmt | printStmt ;

exprStmt       → expression ";" ;
printStmt      → "print" expression ";" ;
```
+ Create a seperate class similar to Expr to represent statement. This class also use Visitor design pattern.
```cpp
class PrintStmt : public Stmt {
  public:
    std::shared_ptr<Expr> expr;
    void accept(IStmtVisitor &v) override { return v.visit_print(*this); }
};
```
+ Update `parse()` method: consume all tokens and return a list of statement
```cpp
std::vector<std::shared_ptr<Stmt>> Parser::parse_program() {
    std::vector<std::shared_ptr<Stmt>> stmts{};
    try {
        while (!consumed_all_tokens()) {
            stmts.push_back(stmt());
        }
        return stmts;
    }
    catch (ParserException &err) {
        ErrorManager::handle_parser_err(err);
        return stmts;
    }
}
```
**Evaluate statement**:
Add new methods to interpreter visitor to evaluate statement:
```cpp
# Simply return nothing
void visit_print(const PrintStmt &p) override {
	LiteralVariant val = evaluate_expr(p.expr);
	std::cout << literal_to_string(val) << std::endl;
	return;
}

void visit_expression(const ExpressionStmt &e) override {
	evaluate_expr(e.expr);
	return;
}
```
### Global variables
**Syntax**:
+ `var a;`: statement to declare a variable.
+ `var a = <value>`: declare and assign variable.
+ `a`: use variable
In the book, the rule like in C, Java is follows, not accept declare statement inside `if`, loop without bracket `{}`: `if (monday) var beverage = "espresso";` (C++ still accept this). I tend to make it obligatory that after `if`, loop bracket must be used like in Golang: 
```
if (monday) {
	var beverage = "espresso";
}
```
**Parse rules**: My new grammar rules for var declaration would be different from the book:
```
program → statement*;
statement → exprStmt | printStmt | varStmt;
```
**Present in syntax tree**: We have to create 2 additional class
+ Sub class of Expression to represent Variable
+ Sub class of Statement to represent var declare stmt.
When parse exception occur for a statement, we now use err recovery technique: panic mode mentioned in chapter 6 to continue parsing the next statement.
### Environment
Def: The place we store our variable. Simply a class: environment.hpp. This class is stored as a field of InterpreterVisitor.
**Field**: a map with key - var name, value - var value.
**Method**: 
+ `add_new_variable`: in the book, a var can be declared twice. However, I follow other languages rule and throw an err if that case happens.
+ `get_variable`: if an undeclared var is refered, there 2 options here: 1: check it at parse step an return a static syntax err (ParseException) and 2: check at runtime. For simplity, we choose to check at runtime.
```cpp
LiteralVariant Environment::get_variable(std::shared_ptr<Token> var) {
    if (!variable_table.count(var->lexeme)) {
        throw RuntimeException(var, "Error: reference to non-exist variable.");
    }

    return variable_table[var->lexeme];
}
```

Update InterpreterVisitor:
+ when evaluate var stmt: invoke `add_new_variable`. A variable without initializer: `var a;` is considered as `nil`
+ when evaluate var expr: invoke `get_variable`.
### Assignment
Used for re-assign variable. Note that some languages not allow re-assigning variable value (Haskell for ex).
**Syntax**: the left side of assignment is a l-value, right side could be both l-value or r-value.
+ Simple: `a = 1`. For now we only support this
+ Complex: `newPoint(x + 2, 0).y = 3;`
**Parsing rules**: In some language this kind of syntax is allow `print(a = 2) // print 2`: assign a to 2 and return the assigned value. Thus, the book use assignment as an expr. I follow C++, Python, not accept the above syntax => use assignment as a statement.
```
// statement →  printStmt | varStmt | assignStmt
// assignStmt -> "l_value" = "expr" ";"
```
**Present in syntax tree**:
```cpp
class AssignStmt : public Stmt {
  public:
    std::shared_ptr<Variable> var;
    std::shared_ptr<Expr> value;
```
**Evaluate node**:
```cpp
void visit_assign_stmt(const AssignStmt &a) override {
	LiteralVariant new_value = evaluate_expr(a.value);
	env->assign_new_value_to_variable(a.var->name, new_value);
	return;
}
```
### Scope
Static scope aka Lexical scope: when uses some variable, you can figure out which variable declaration it refers to just by looking at the code. In most modern languages, variable is static scope.
```java
{
  var a = "first";
  print a; // "first".
}

{
  var a = "second";
  print a; // "second".
}
```
Dynamic scope used for obj fields method, we can only know what the refer to at runtime.
```java
class Saxophone {
  play() {
    print "Careless Whisper";
  }
}

class GolfClub {
  play() {
    print "Fore!";
  }
}

fun playIt(thing) {
  thing.play();
}
```

**Nested scope:**
+ Var in inner scope shadow the var with the same name in the outer scope
```java
// How loud?
var volume = 11;

// Silence.
volume = 0;

// Calculate size of 3x4x5 cuboid.
{
  var volume = 3 * 4 * 5;
  print volume;
}
```
+ Inner scope can access var at outer scope
```java
var global = "outside";
{
  var local = "inside";
  print global + local;
}
```

To do 2 things above, we need a chain of environment. Each environment has a pointer pointed to its parent envirionment. When we get or assign a var, if the var is not found in the current scope, we traversal up through the env chain to find it.

**Parsing rules**: my rules, not like the book
```
// statement → block | exprStmt | printStmt | varStmt | assignStmt
// block → "{" statement* "}"
```
**Present in syntax tree**:
```cpp
class BlockStmt : public Stmt {
  public:
    std::vector<std::shared_ptr<Stmt>> stmts;
};
```
**Evaluate node**:
Create a new env whenever execute stmts in a block. After finish executing the block, remove all variables created inside it (automatically done for us by smart pointer).
```cpp
void visit_block_stmt(const BlockStmt &b) override {
        auto parent_scope_env = this->env;
        this->env = std::make_shared<Environment>(parent_scope_env);
        for (auto stmt : b.stmts) {
            stmt->accept(*this);
        }
        this->env = parent_scope_env;
    }
```
## Compile and linking
Compiler convert a source language to a lower level target language (the target doesn't necessary to be assembly)
Compiler triplet: naming convention for what a program can run on. Structure: machine-vendor-operatingsystem, ex: `x86_64-linux-gnu`

Steps when we type: `gcc -c main.c`
![compiler_steps](compiler_steps.md)
Preprocessor: simply handle the preprocess macro `#include`, `#define`. Just text replacement process.
Assembler: transfer the assembly code to object file (machine code). Each arch x86, arm has a specific set of assembly instructions, there is a assembler for each set of instructions.
### Relocation
Merges separate code and data sections of multiple files to single sections.
![](Images/Pasted%20image%2020240112223338.png)

ELF then loaded into the main mem:
![](Images/Pasted%20image%2020240112223844.png)
+ Kernel virtual mem is shared within all processes.
## Linker
Linker does the linking process to link multiple obj files to create prog file. `gcc main.c sum.c –o prog`

![|525](Images/Pasted%20image%2020240112220518.png)

Linker do the 2 things:
+ Symbol resolution
+ Relocation
### Symbol resolution
+ 3 types of object file: .o, .out (can be loaded into mem and exec), .so
+ All object file follow the standard of ELF format
+ 3 types of symbol:
	+ Global symbols: can be referenced by other modules. E.g., non-static C functions and non-static global var
	+ External symbols: Global symbols that are referenced but defined in other object files.
	+ Local symbols: Symbols that are defined and can be referenced only in the file. E.g., C functions and global variables defined with the static attribute. **Local vars are not local symbols.**
+ Strong vs weak symbol
+ 3 Symbol rules: (when two file define a global var with the same name, what to choose)
![](Images/Pasted%20image%2020240112222827.png)
## Others
### ...Runtime Dynamic Linking
### Library Interpositioning
Intercept the call: add hook to do sth before/after calling to a predefined function. 3 ways to do that, use `LD_PRELOAD` env is the preferred way.

### LLVM
LLVM is a library that is used to construct, optimize and produce intermediate and/or binary machine code.

LLVM can be used as a compiler & toolkit build compilers where you provide the "front-end" (parser and lexer) and the "back-end" (code that converts LLVM's representation to actual machine code). Clang is a front-end of LLVM to generate LLVM IR.
![|625](Images/Pasted%20image%2020240114175911.png)
![](Images/Pasted%20image%2020240114180011.png)

