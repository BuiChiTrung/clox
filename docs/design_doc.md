**Language**: simple set of rules of syntaxs to write a program
**Programming language implementation**: is a system for executing [computer programs](https://en.wikipedia.org/wiki/Computer_programs "Computer programs"). There are two general approaches: interpretation, compilication.
## Terminologies

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
## Lox language
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
## Scanner
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

## Representing code
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

## Parsing expr
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

## Evaluating expression

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
## Statements and state
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
Def: The place we store our variable. Simply a class: ast/environment.hpp. This class is stored as a field of InterpreterVisitor.
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
## Control flow
2 kind of control flow: 
+ Branching 
+ Looping
### Conditional execution
Note: there is a case like this
```cpp
if (first) if (second) whenTrue(); elsewhenFalse();
```
This can be parsed into 1 of 2 cases below: 
![|475](Images/Pasted%20image%2020250204233132.png)

Most language choose to stick `else` to nearest `if` (case 2).

**Parsing rule**: To avoid the corner case above. I follow golang rule, not like the book, make it required to have {} after the condition expression. `{` also tell the parser when the condition expr is ended instead of having to use `()` wrap around expr like in C, Java.
```
statement → block | ifStmt | exprStmt | printStmt | varStmt | assignStmt
ifStmt -> "if" expression block ("else" block)?
```
**AST Node:**
```cpp
class IfStmt : public Stmt {
  public:
    std::shared_ptr<Expr> condition;
    std::shared_ptr<Stmt> if_block;
    std::shared_ptr<Stmt> else_block;
}
```
**Evaluate node:**
```cpp
void InterpreterVisitor::visit_if_stmt(const IfStmt &i) {
    auto expr_val = evaluate_expr(i.condition);
    if (cast_literal_to_bool(expr_val)) {
        i.if_block->accept(*this);
    }
    else if (i.else_block != nullptr) {
        i.else_block->accept(*this);
    }
    return;
}
```
### Logical expr
Logic expr is special: we may only need to evaluate the left expr to know the result. Ex: `false and doSth();` => dont have to eval `doSth()`.
**Parsing rule:**
```cpp
expression → logic_or ;
logic_or → logic_and ( "or" logic_and )*
logic_and → equality ( "and" equality )*
```
### While loop
**Parsing rule**
```cpp
// whileStmt → "while" expression block
```
### For loop
Every for loop can be present as while loop. Ex: 
```cpp
for (var i = 0; i < 10; i = i + 1) print i;

// Equivalent
var i = 0;
while (i < 10) {
	print i;
	i = i + 1;
}
```
Lox doesn’t _need_ `for` loops, they just make some common code patterns more pleasant to write. These kinds of features are called **syntactic sugar**.
**Parsing rule**
```cpp
// forStmt → "for" (varStmt | assignStmt | ";") (expression)? ";" (assignStmt)? block
```
**AST Node and evaluate**
Instead of create new type of node in the syntax tree for `for(initializer; condition; increment)` loop we reuse the `while` loop: 
for_stmt = BlockStmt(initializer, WhileStmt(condition, {while_body, increment}))
## Functions
### Function call
Supported syntax: nested call `doSth(1, 2)()`. The callee: `doSth`, `doSth(1, 2)` are expression that evaluated as a function.
**Parsing rule**: function call has higher precedence than unary
```
// unary → ( "!" | "-" ) unary | call
// call → primary ("(" arguments ")")*
// arguments -> "" | (expression (","expression)*)
```
**AST Node and evaluate**
```cpp
class FuncCallExpr : public Expr {
    std::shared_ptr<Expr> callee;
    std::shared_ptr<Token> close_parenthesis; // use to report the pos of func when an err occur
    std::vector<std::shared_ptr<Expr>> args;
```
Also we define an interface for Callable obj:
```cpp
class LoxCallable {
  public:
    virtual uint get_param_num() const { return 0; }
    virtual ExprVal invoke(InterpreterVisitor *interpreter,
                           std::vector<ExprVal> &args) = 0;
    virtual std::string to_string() const = 0;
};
```
Note: 
+ When parsing func call: check number of args exceed limit (255).
+ When evaluating func call: check number of func params = number of passed args.
### Native function
Funcs the language provided like print. Example `clock()` func, which tell how many seconds passed since a particualar time.
```cpp
class ClockNativeFunc : public LoxCallable {
  public:
    ExprVal invoke(InterpreterVisitor *interpreter,
                   std::vector<ExprVal> &args) override {
        auto now = std::chrono::system_clock::now();
        std::chrono::duration<double> unix_time = now.time_since_epoch();

        return unix_time.count();
    }

    std::string to_string() const override { return "<native-fn clock>"; }
};
```

In the interpreter, we add a field to save global env. Native funcs are added to the global env.
```cpp
InterpreterVisitor::InterpreterVisitor()
    : global_env(std::make_shared<Environment>()) {
    env = global_env;
    global_env->add_new_variable("clock", std::make_shared<ClockNativeFunc>());
}
```
### Function declaration + Function object
**Parsing rule**:
```
// function → IDENTIFIER "(" parameters ")" block ;
// parameters -> "" | (IDENTIFIER (","IDENTIFIER)*)
```
**AST node:**
```cpp
class FunctionStmt : public Stmt {
    std::shared_ptr<Token> name;
    std::vector<std::shared_ptr<VariableExpr>> params;
    std::shared_ptr<Stmt> body;
```
**Eval `FunctionStmt` instance:**
Instance of `FunctionStmt` class should give us a way to invoke it => we need to implement `LoxCallale` class. To keep the `FunctionStmt` minimal (similar to other classes present node in AST tree), we create another wrapper class around `FunctionStmt`:
```cpp
class LoxFunction : public LoxCallable {
    const FunctionStmt &func_stmt;
}

void InterpreterVisitor::visit_function_decl(const FunctionStmt &w) {
    std::shared_ptr<LoxFunction> func(new LoxFunction(w));
    env->add_new_variable(w.name->lexeme, func);
}
```

Each time a func is invoked an env should be created to save var defined in the func scope and param. The env is created in the `invoke` method of `LoxFunction`. For now, the parent of that env is the global env (will be updated when we support closure).
### Return stmt
**Parsing rule:**
```
// statement → ... | returnStmt
// returnStmt → return expression? ";"
```
**AST node:**
```
class ReturnStmt : public Stmt {
    std::shared_ptr<Token> return_kw; // used for err reporting
    std::shared_ptr<Expr> expr;
}
```
**Eval node:** Assume we have a call stack like this:
```
void Interpreter.visitReturnStmt() <- throw return_val
void Interpreter.visitIfStmt()
void Interpreter.visitBlockStmt()
void Interpreter.visitWhileStmt()
void Interpreter.visitBlockStmt()
ExprVal LoxFunction.invoke() <- catch
ExprVal Interpreter.visitFuncCall()
```

+ When return stmt is exec in the body of the func, we would stop executing other stmts. We need a way to bring the return result from `visitReturnStmt` to `visitFuncCall`. As the return type in other funcs in the call stack are `void`, one of the way to do that is **using exception to throw the return result**.
```cpp
class Return : std::exception {
    ExprVal return_val;
};

void InterpreterVisitor::visit_return_stmt(const ReturnStmt &r) {
    ExprVal return_val = NIL;
    if (r.expr != nullptr) {
        return_val = evaluate_expr(r.expr);
    }

    throw Return(return_val);
}
```
+ Catch the return result in the func to `invoke` func in defined in `LoxCallable` interface.
+ Note: the visitBlockStmt() should catch `Return` as well to update interpreter env after executing a block. 
### Local func and closure
For now, the parent env of a func call is always pointed to the global func. However, our language would support closure, defining a func inside another func. Ex:
```
// CLOSURE
fun make_counter() {
  var i = 0;
  
  fun closure() {
    i = i + 1;
    print i;
  }

  return closure;
}

var count = make_counter();
count(); // 1
count(); // 2
```

The parent env of `closure` is `make_counter`, not the global env. Solution:
+ In `LoxFunction` class, add a field to save the env of outer func `make_counter`. When eval `FunctionStmt` node, assign this field as the current env of the interpreter
```cpp
class LoxFunction : public LoxCallable {
    const FunctionStmt &func_stmt;
    // Can be global env or the env of the outer func defined this func
    std::shared_ptr<Environment> parent_env;
```
+ When eval `FuncCall` node, set the parent env of func call env as this field.

## Resolve and binding
### Static scope
A **variable expression will always resolve to the same variable declaration** no matter how many times the expr is exec. In this example, we would expect that `showA()` print out "global" twice, `a` is always resolve to the global var `a`.
```
// scope0
var a = "global";

// scope1
{
  // scope2
  fun showA() {
    print a;
  }

  showA();
  var a = "block";
  showA();
}
```

However, with implementation from previous chapters our printed result would be `global` then `block` because in the 2nd `showA()` call, we find `a` from scope2 => scope1 => scope0 and found a local `a` at scope1.
=> Before interpreting, with each identifier usage we need a way to tell which identifier declaration it refers to using Semantics analysis.
### Semantics Analysis
Revise from the 1st chapter, our compiler/intepreter go through the following steps:
+ **Syntactic analysis**: check a program is grammatically correct (done by parser)
+ **Semantic analysis**: go further to tell what the program means, tell if there is any logic err (Ex: `return` stmt outside a func, a var declared twice, ...). Our problem mentioned above could be sol in this phase we by: with every variable mentioned, we figures out which declaration it refers to.
+ **Interpret** the program.
### Resolver class
We resolve variable by storing info of **how many scope we have to jump from the current scope** variable usage to the scope in which the variable is declared. This is done by implementing another class follow the visitor design pattern which travesal through all nodes in the AST before **interpreting**.
```cpp
class IdentifierResolver
	  std::shared_ptr<AstInterpreter> interpreter;
		
	  # key - variable name, 
	  # value - whether the variable is resolved or not.
    std::vector<std::unordered_map<std::string, bool>> scopes;

		void visit_block_stmt(const BlockStmt &b, std::shared_ptr<Environment> block_env = nullptr) override;
		...
    ExprVal visit_identifier(const IdentifierExpr &v) override;
```

Interesting type of nodes:
- `visit_block_stmt`: introduces a new scope.
- `visit_function_decl`: introduces a new scope for its body and binds its parameters in that scope.
- `visit_var_decl`: adds a new variable to the current scope. Special case, since this case is useless in reality, we would raise an err.
	```cpp
	var a = a;
	```
- `visit_assign_stmt`: Variable and assignment expressions need to have their variables resolved.
### Interpret resolved var
Add a map in the interpreter to store info of **how many scope we have to jump from the current scope** variable usage to the scope in which the variable is declared. This map is updated while we traversal in ast node in `IdentifierResolver` class
```cpp
// key: pointer to the variable usage node in ast.
// value: distance from the current scope of the ast node to the scope where the var is defined.
std::unordered_map<const IdentifierExpr *, int> identifier_scope_depth;
```

In the `main.cpp`, call resolver before interpreter
### Resolution errors
In the resolver state, we can detect other error
+ In the same scope, a var is declared multiple times.
+ `return` stmt invoked outside a func, `break` outside a loop, etc.
+ Many IDEs will warn if you have unreachable code after a `return` statement, or a local variable whose value is never read
## Classes
### OOP and classes
3 paths to OOP:
+ classes: C++, Go (we use this path)
+ prototypes: Javascript, Lua
+ mulmethods: CLOS, Dylan, etc
### Class declaration
```cpp
class Breakfast {
  cook() {
    print "Eggs a-fryin'!";
}
```
**Parsing rule:**
```
// declaration → varDecl | funcDecl | classDecl | statement
// classDecl -> "class" IDENTIFIER "{" function* "}"
// function -> IDENTIFIER "(" parameters ")" block
```
**AST node:**
```
class ClassDecl : public Stmt {
    std::shared_ptr<Token> name;
    std::vector<std::shared_ptr<FunctionDecl>> methods;
}
```
**Resolver + Eval node:** check the src code
### Creating instance
Support class constructor `ClassName()` by inheriting `LoxCallable`. 2 steps:
1. Alloc mem for new instance. We have another class to present class instance: `LoxInstance`
2. Run user defined initializer: `init()` if it exists.
### Properties on instance
Support new token `.` which tell us that an instance property is accessed. `.` has the same precedence as `()` in func call expr. Ex: In expressions involving both `.` and `()`, such as `a.b()` or `a().b`, the operand or operator that appears earlier in the syntax is evaluated first.
=> we put it in the grammar by updating `call` rule:
```
// call → primary ("(" arguments ")" | "." IDENTIFIER)*
```
**AST node**:
```cpp
class GetClassFieldExpr : public Expr {
    std::shared_ptr<Expr> lox_instance;
    std::shared_ptr<Token> field_token;
};
```
**Resolver:** simple, look at src code
**Eval node:** LoxInstace class provide an `unordered_map`: key - prop_name, value - prop value. When a prop is access, we find the value in this `unordered_map`

We also need to support prop update operation
**Parsing rule:** make change to assignmentStmt
```
// assignStmt -> (IDENTIFIER | call) "=" expression";" | exprStmt
```
**AST node:**
```cpp
class SetClassFieldStmt : public Stmt {
    std::shared_ptr<Expr> lox_instance;
    std::shared_ptr<Token> field_token;
    std::shared_ptr<Expr> value;
}
```
**Resolver:** simple, look at src code
**Eval node:** update the prop `unordered_map` of lox instance.
### Methods on class
Support both cases below:
```cpp
var m = object.method;
m(argument);
```

```cpp
class Box {}

fun notMethod(argument) {
  print "called function with " + argument;
}

var box = Box();
box.function = notMethod;
box.function("argument");
```

Since we've already supported finding prop, func call syntax, there is no need to add need ast node, parsing rule to handle method invocation.

Unlike the book, I created a new class `LoxMethod` inherit `LoxFunction`.

**Resolver:** in class decl stmt, resolve all methods
Update the LoxClass to store `unordered_map` of methods. When we look for an instance field, we first check the property then check methods by looking at this methods `unordered_map`.
### This
`this` evaluates to the instance that the method was called on.
**Parsing rule:**
```
// primary → IDENTIFIER | "this" | NUMBER | STRING | "true" | "false" | "nil"
```
**AST node:** Inherit `IdentifierExpr` as there are some common methods between the 2 classes.
```
class ThisExpr : public IdentifierExpr {
    std::shared_ptr<Token> name;
};
```
**Resolver:**
When resolving class, we add a new scope for the class and add identifier `this` to the class scope.
```cpp
void IdentifierResolver::visit_class_decl(const ClassDecl &class_decl_stmt) {
    addScope(); // class scope
    scopes.back()["this"] = true;
    for (auto method : class_decl_stmt.methods) { ... }
    closeScope();
}
```
**Eval node:**
Background: Both resovler and interpreter maintain a linked list of scope/env. In each scope, we have an `unordered_map` of identifier
+ resolver: key - identifier name, value - true/false (indicate if the identifier is resolved or not)
+ interpreter: key - identifier name, value - identifier value.
![|475](Pasted%20image%2020250602100225.png)
In order to correctly eval `this` kw, 2 chains of scope/env must be sync:
+ Add the class new env when interpreting the class:
```cpp
void AstInterpreter::visit_class_decl(const ClassDecl &class_decl) {
		...
    auto class_env = std::make_shared<Environment>(env);
    class_env->add_new_variable("this", NIL);

    for (auto method : class_decl.methods) {
        auto lox_method = std::make_shared<LoxMethod>(method, class_env);
				...
    }
		...
}
```
+ Update the value of this in class env whenever a method is invoked.
```cpp
// LoxInstance
ExprVal get_field(std::shared_ptr<Token> field_token) {
		...
		if (lox_class->methods.count(field_name)) {
				auto method = lox_class->get_method(field_name);
				method->bind_this_kw_to_class_method(*this);
				return method;
		}
		...
}

// LoxMethod
void bind_this_kw_to_class_method(LoxInstance &instance) {
		// Use no-op deleter to make sure that "this" is not deallocate
		// after shared_ptr run out of scope.
		// parent_env = class_env
		this->parent_env->identifier_table["this"] =
				std::shared_ptr<LoxInstance>(&instance,
																		 smart_pointer_no_op_deleter);
}
```
ThisExpr inherit IdentifierExpr, we could reuse the func to eval IdentifierExpr
```cpp
ExprVal AstInterpreter::visit_this(const ThisExpr &this_expr) {
    return evaluate_identifier(this_expr);
}
```
### Constructors and Initializers
Constructor is a pair of operations:
+ Allocate memory for new instance: done in Create instance section
+ Run the user-defined constructor if it exists: call `init()`
Src code: `LoxClass::invoke`

Not allow using `return` in `init()`. Check in resolver.
## Inheritance
### Superclass and subclass
Term: superclass (base class), subclass (derived class)
**Parsing rule:** `:` used to inherit.
```
// classDecl -> "class" IDENTIFIER (: IDENTIFIER)? "{" function* "}"
```
**AST node:**
```cpp
class ClassDecl : public Stmt {
    std::shared_ptr<Token> name;
    std::shared_ptr<IdentifierExpr> superclass;
    std::vector<std::shared_ptr<FunctionDecl>> methods;
}
```
**Resolver:** add a line of code to resolve superclass and check edge case: a class inherit itself.
**Eval node:**
+ Store `superclass` in `LoxClass`
+ Check if super class is defined and is a valid LoxClass. This cannot be done in resolver because we can only evaluate superclass at runtime.
```cpp
var NotAClass = "I am totally not a class";
class Subclass < NotAClass {} // ?!
```

```cpp
void AstInterpreter::visit_class_decl(const ClassDecl &class_decl) {
		...
    if (class_decl.superclass != nullptr) {
        ExprVal superclass_expr_val =
            class_decl.superclass->accept(*this); // evaluate super class expr
        superclass = cast_expr_val_to_lox_class(superclass_expr_val);
        if (!superclass) {
            throw RuntimeException(class_decl.superclass->token,
                                   "Superclass must be a defined class.");
        }
    }
		...
}
```
### Inherit method
```cpp
class Doughnut {
  fun cook() {
    print("Fry until golden brown.");
  }
}

class BostonCream : Doughnut {}

BostonCream().cook();
```

In dynamic type language, it's pretty simple as we don't have to worry about memory layout as in static type language. 
```cpp
// class.hpp
std::shared_ptr<LoxMethod> get_method(std::string name) {
		// Check method exist in the current class
		if (methods.count(name) > 0) {
				return methods[name];
		}

		// Check method exist in the superclass
		if (superclass != nullptr) {
				return superclass->get_method(name);
		}

		return nullptr;
}
```
### Calling super method
`super` keyword: Use to call overriden method in superclass in the method of subclass.
```cpp
class Doughnut {
  cook() {
    print "Fry until golden brown.";
  }
}

class BostonCream < Doughnut {
  cook() {
    super.cook();
    print "Pipe full of custard and coat with chocolate.";
  }
}

BostonCream().cook();
```
**Parsing rule:** super must go with `.<method>`
```
// primary → IDENTIFIER | "this" | NUMBER | STRING | "true" | "false" | "nil" |
// "super".IDENTIFIER | "(" expression ")"
```
**AST Node:** Inherit IdentifierExpr to inherit `resolve_identifier` method
```cpp
class SuperExpr : public IdentifierExpr {
    std::shared_ptr<IdentifierExpr> method;
}
```
**Resolver:** resolve it to the superclass of the class containing the `super`.
Similar to `this`, we add `super` to the class env if it's a subclass.
```cpp
void IdentifierResolver::visit_class_decl(const ClassDecl &class_decl_stmt) {
		...
    addScope(); // class scope
    scopes.back()["this"] = true;
    if (class_decl_stmt.superclass != nullptr) {
        current_class_type = ResolveClassType::SUBCLASS;
        scopes.back()["super"] = true;
    }
    closeScope();
		...
}

```
Utilize `resolve_identifier` method to resolve `IdentifierExpr`: look for `super` in the chain of scope.
```cpp
ExprVal IdentifierResolver::visit_super(const SuperExpr &super_expr) {
    resolve_identifier(super_expr);
    return NIL;
}
```
**Eval node:** Similar to how we handle `this`. 2 chains of scope/env in resolver, interpreter must be sync. We add `super` to the class env in interpreter while interpreting class declaration. However, different from `this`, `super` always point to the same class.
```cpp
void AstInterpreter::visit_class_decl(const ClassDecl &class_decl) {
		...
    auto class_env = std::make_shared<Environment>(env);
    class_env->add_identifier("this", NIL);

    // Check if super class is defined and is a valid LoxClass
    if (class_decl.superclass != nullptr) {
				ExprVal superclass_expr_val =
            class_decl.superclass->accept(*this); // evaluate super class expr
        superclass = cast_expr_val_to_lox_class(superclass_expr_val);
        class_env->add_identifier("super", superclass);
    }
		...
}
```
When interpreting `super` kw, there are 3 steps:
+ Find the superclass `super` refer to by looking at the scope/env chain (similar to other identifier).
+ Find the class instance which invoke the method call. In other word, `this`. As we define `this`, `super` in the same class env. In the prev step, we already know the scope different from the current scope in which `super` keyword is used in the class env => we can get `this` - class instance.
+ Find method invoked.
Check the func `AstInterpreter::visit_super` for detail.
**Check invalid usage of `super`**:
+ Used in a non subclass
+ Used outside a method body
Check in resolver:
```cpp
ExprVal IdentifierResolver::visit_super(const SuperExpr &super_expr) {
    if (current_class_type == ResolveClassType::NONE) {
        ErrorManager::handle_err(
            *super_expr.token, "Can't use 'super' outside a subclass method.");
    } else if (current_class_type != ResolveClassType::SUBCLASS) {
        ErrorManager::handle_err(
            *super_expr.token,
            "Can't use 'super' inside a class with no super class.");
    }

    resolve_identifier(super_expr);
    return NIL;
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

