# Additional Notes

## `print` & `println` functions

`print` and `println` are functions that you will get to know and understand more as you write more code. These functions are both considered variadic. They take any number of arguments above the minimum.

For `print`, the minimum is 1 argument. The argument can be of any type (for struct types, the struct needs to have a member function called `str`).

For `println`, the minimum is 0 arguments. This is because `println` automatically prints a newline if no arguments are specified. Otherwise, it does the same as `print` and then prints a newline.

## Semicolons

The semicolon (`;`) at the end of the `println` call is totally optional! Technically no, they're not optional. Although the compiler won't throw a tantrum about semicolons, they are advised as they help promote good code and are essential (as we'll see later)! This is because certain statements such as `continue`, `return`, and `break` require a semicolon (or another expression) after its declaration.

For example, the following snippet is valid:

```bls
func x() {
  return 0
}
```

but the next snippet isn't:

```bls
func x() {
  return
}
```

<!-- markdownlint-disable-next-line MD026 -->
### But I'm Stubborn And Hate Semicolons!

**Note:** There is some very technical stuff ahead. If you don't understand it, heres a tl;dr: Use semicolons, it'll save you from pulling out your hair trying to understand an issue.

It's fine I guess. But, try running the next piece of code:

```bls
func this_function_kills(x, y) {
  let i = x
  for _ in range(1, y) {
    i += 4
  }
  return i
}

println(this_function_kills(3, 2)) // should print '7' to the console
```

As you may notice, you'll get this odd error:

```txt
stubborn.bls:9:8: error: 'Int' is not a function or struct definition
println(test(3, 2))
       ^
```

#### An in-depth look at how the VM works

Uhh... what does that mean? Essentially, the line:

```bls
i += 4
```

is 3 `LOAD` instructions in the VM to load `i` as a variable, `+=` as the inner function, and `4` as a constant number (We'll discuss these in a later chapter). Next, it runs a `MEM_FN` instruction (member function call) on `i` with `+=` and `4` as the parameter. In the end, the stack looks something like this before the `MEM_FN` instruction:

| BLS Stack                    |
|------------------------------|
| `println` function reference |
| `i` variable reference       |
| `+=` function reference      |
| `4` constant number          |

Once the `MEM_FN` instruction is executed, the stack looks a bit different:


| BLS Stack                    |
|------------------------------|
| `println` function reference |
| `i` variable reference       |

Notice how the `i` variable reference sticks around? This is because it wasn't properly unloaded with a `ULOAD` instruction.

Here's why this is a big issue: when we `return` the `i` variable. We **add** that onto the stack. Essentially making it look like the following.

| BLS Stack                                    |
|----------------------------------------------|
| `println` function reference                 |
| `i` variable reference                       |
| `i` variable reference (the second one)      |

Well, thats not too bad right? If you agreed, you're sadly wrong. This is because when we run the `FNCL` (function call) instruction, it doesn't know what exactly the function is, only whether or not an argument unpacks a vec into variadic args, and arg count (with a boolean attribute per arg). We'll cover the VM in depth in another chapter. 

The VM finds the instruction and looks at the arg count and pops an appropriate amount to store in the args for later use. In this case, the parser marked 1 argument to the `println` function so it takes the second `i` variable reference as the argument. Then, it needs to take a reference to the function so it pops from the stack yet again! It obviously does checks to see if it is a member function or *is callable*. But, an `Int` is NOT callable. In the end, it throws the error and exits. 

#### The amazing, yet simple fix

In the codegen code, we have a section dedicated to fixing this issue with... semicolons!

```cpp
{{#include ../../../src/Compiler/CodeGen/Expr.cpp:166:167}}
```

This 2 line section of code adds a `ULOAD` instruction which instantly cleans up our leftover `i` reference.

So now if we add semicolons and run the program again:

```bls
func this_function_kills(x, y) {
  let i = x;
  for _ in range(1, y) {
    i += 4;
  }
  return i;
}

println(this_function_kills(3, 2)); // should print '7' to the console
```

It works! It prints `7` to the console as it should.
