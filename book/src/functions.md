<!-- markdownlint-disable MD042 -->
# Functions

In this section, we'll dive into the world of functions -- including member functions! We'll cover the following topics:

- [Simple Functions](#simple-functions)
- [Default Arguments](#default-arguments)
- [Variadic Functions](#variadics)
- [Keyword Arguments](#keyword-arguments)

## Simple Functions

Simple functions can be created using the `func` keyword followed by a function name, some arguments, and then a block.

```bls
func example_function(with, some, args) {
  println("Do something ", with, some, args, "!");
}
```

Functions take arguments of any type, so that may be something you'd want to watch out for when writing your own functions.

## Default Arguments

Functions are allowed to have defaults arguments by assigning a default value to arguments that don't *require* values.

For example, in the following function, we define 3 arguments: `x`, `y`, and `z`. But, we assign a default value to `z`.

```bls
func default_args(x, y, z = 0) {
  if x == 0 {
    println("x is 0");
  }

  if y == 0 {
    println("x is 0");
  }

  if z == 0 {
    println("x is 0 (default)");
  }
}
```

When we call the function `default_args` we don't need to include `z` as a parameter. It instead defaults to `0`!

## Variadics

Functions that can take any amount of arguments are called variadic functions. Variadic functions are cool because they allow for an infinite amount of arguments to be passed to the function. Again note, that these arguments are of any type which may be something to watch out for.

```bls
import "std/vec" as vec

func variadic_func(x...) {
  if x.empty() {
    println("x is empty!");
    return;
  }
  println("found x: ", x);
}
```

The above example shows a variadic function called `variadic_func`. It takes an arbitrary number of arguments and combines it into a `vec` argument named `x`. This is why we also need to import `std/vec`. We'll talk about imports in the next chapter. We define variadic arguments by appending `...` to the end of an argument name.

Please note that variadics **must** be the last argument in a list of arguments. For example, the following will not compile:

```bls
import "std/vec" as vec

func invalid_var_args(x..., y) {
  println(x)
  println(y)
}
```

Instead we're greeted with the following error:

```txt
var_args.bls:3:29: error: cannot have any argument after variadic arg declaration
func invalid_var_args(x..., y) {
                            ^
```

## Keyword Arguments

Functions are already very powerful in their uses. But, we have one more thing to add and that is keyword arguments. Keyword arguments are arguments that accept named values into a `map`. This powerful feature also requires `std/map` to fully leverage. To define a keyword argument, we prepend `.` to an argument name.

```bls
import "std/map" as map

func kw_args(x, .kw) {
  println(x)
  println(kw["y"])
}
```

In the example, we define a function called `kw_args` with 2 arguments. A regular arg named `x` and a keyword arg named `kw`. When we pass the arguments, we can define named arguments in the call:

```bls
kw_args(20, y = 10)
```

We give `x` the value of `20`, then we create a named variable called `y` with the value `10`. When run, this code will print `20` and `10` to the terminal. It is also important to note that if a keyword argument attempts to access a member without a value, it will return `nil`.

Next chapter, we'll go into the wide world of imports and modules!
