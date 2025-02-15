# clox
This is a C++ implementation for Lox - a dynamic type programming language, inspired by the book Crafting Interpreters.

### Build the code
Requirement: C++17 or newer.
```
./build_clox.sh
```

### Features with example snippets
Expression and statement: `./build/main ./demo/expression_and_statement.lox`
```
print "Hello world";
print 5 + (3 * 2) - 4 / 2;
print !(false or true);
var a = "Moon ";
var b = "Light";
var c;
print c;
c = a + b;
print c;
```

Variable scope: `./build/main ./demo/variable_scope.lox`
```
var a = "global a";
var b = "global b";
var c = "global c";
{
  var a = "outer a";
  var b = "outer b";
  {
    var a = "inner a";
    print a; // inner a
    print b; // outer b
    print c; // global c
  }
  print a; // outer a
  print b; // outer b
  print c; // global c
}
print a; // global a
print b; // global b
print c; // global c
```

Loop: `./build/main ./demo/loop.lox`
```
// Fibonacci program
var a = 0;
var temp;

for var b = 1; a < 10000; b = temp + b; {
  print a;
  temp = a;
  a = b;
}
```

Report syntax error: 
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

Function: `./build/main ./demo/function.lox`
```
// NORMAL FUNCTION
print "Fibonacci";
fun fib(n) {
  if n <= 1 {
    return n;
  }
  return fib(n - 2) + fib(n - 1);
}

for var i = 0; i < 10; i = i + 1; {
  print fib(i);
}

// LANGUAGE NATIVE FUNCTION
print "Native function get current time: " + clock() + "s";
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