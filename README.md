# clox
This is a C++ interpreter the Lox - a dynamic type programming language, inspired by the book Crafting Interpreters.

### Build the code
Requirement: C++20 or newer.
```
./build_clox.sh
```

### Run example snippets
Expression and statements: `./build/main demo/example1.lox`
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

Variable scope: `./build/main demo/example2.lox`
```
var a = "global a";
var b = "global b";
var c = "global c";
{
  var a = "outer a";
  var b = "outer b";
  {
    var a = "inner a";
    print a;
    print b;
    print c;
  }
  print a;
  print b;
  print c;
}
print a;
print b;
print c;
```

### Run unit-tests
Run a single unit-test
```
cd build/tests
./<test_file>
```

Run all unit-tests
```
./run_test.sh
```