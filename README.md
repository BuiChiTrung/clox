# clox

[![Repo Status](https://img.shields.io/badge/Status-Active-brightgreen.svg)](https://github.com/BuiChiTrung/clox) [![GitHub license](https://img.shields.io/github/license/BuiChiTrung/clox)](https://github.com/BuiChiTrung/clox/blob/main/LICENSE)

This is a C++ implementation for Lox - a dynamic type programming language, inspired by the book Crafting Interpreters.

### Build the code

Requirement: C++17 or newer.

```
./build_clox.sh
```

### Features with example snippets

**Expression and statement**: `./build/main ./demo/expression_and_statement.lox`

```
/*
    This is a block of comments
*/
print(nil and "yes");
print("Hello world");
print(5 + (3 * 2) - 4 / 2);
print(0 or 2);
var a = "Moon ";
var b = "Light";
var c;
print(c);
c = a + b;
print(c);
```

**Variable scope**: `./build/main ./demo/variable_scope.lox`

```
var a = "global a";
var b = "global b";
var c = "global c";
{
  var a = "outer a";
  var b = "outer b";
  {
    var a = "inner a";
    print(a); // inner a
    print(b); // outer b
    print(c); // global c
  }
  print(a); // outer a
  print(b); // outer b
  print(c); // global c
}
print(a); // global a
print(b); // global b
print(c); // global c
```

**Loop**: `./build/main ./demo/loop.lox`

```
// Fibonacci program
var a = 0;
var temp;

for var b = 1; a < 10000; b = temp + b; {
  print(a);
  temp = a;
  a = b;
}

var c = 1;
while c <= 10 {
    print(c % 3.0);
    c = c + 1;
}
```

**Report syntax error**:

```
var a = 1;
print a
while a <= 10 {
    a = a + 1;
```

`./build/main ./demo/parse_error_report.lox`

```
[line 3] Error at 'while': Expected ; at the end of print statement
[line 3] Error at '{': Expected close bracket '}' at the end of the block to match '{'
Parser error occurs
```

**Function**: `./build/main ./demo/function.lox`

```
// NORMAL FUNCTION
print("Fibonacci");
fun fib(n) {
  if n <= 1 {
    return n;
  }
  return fib(n - 2) + fib(n - 1);
}

for var i = 0; i < 10; i = i + 1; {
  print(fib(i));
}

// LANGUAGE NATIVE FUNCTION
print("Native function get current time: " + clock() + "s");

// CLOSURE
print("Closure");
fun make_counter() {
  var i = 0;

  fun counter() {
    i = i + 1;
    print(i);
  }

  return counter;
}

var count = make_counter();
count(); // 1
count(); // 2
```

**List**: `./build/main ./demo/list.lox`

```
// Currently, "[]" notation for list initialization, accessment is not support.
// Create list
var list = List(1, 2, 3);

// Push element to list
list.push(4);
print(list);

// Pop last element from the list
list.pop();
print(list);

// Access an element in the list
print(list.at(1));

// Check list size
print(list.size());
```

**Class**: `./build/main ./demo/class.lox`

```
class FootballPlayer {
    fun do_sth() {
        print("Messi right foot is better than Ronaldo whole career.");
    }
    fun pass() {
        print(this.skill);
    }
}
print(FootballPlayer);

// Inherit: super keyword to call superclass method in subclass in currently not supported.
class Striker : FootballPlayer {
    fun Striker(quality) {
        this.quality = quality;
    }
    fun shoot() {
        print("This is a " + this.quality + " shoot");
    }
}

var yamal = Striker("Wonderful");
print(yamal);

yamal.skill = "Through pass";
yamal.pass();
yamal.shoot();
```

**Interactive mode**: run the main without passing any lox source file `./build/main`

```
Enter lines of text (Ctrl+D or Ctrl+Z to end):
==> var src = "Hello VietNam";
==> src;
Hello VietNam
==> 1 + 1;
2
```

### Run unit-tests

Run a single unit-test

```
cd build/tests && ./<test_file>
```

Run all unit-tests

```
./run_test.sh
```
