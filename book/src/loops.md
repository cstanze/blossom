# Loops

If we attempted to make a multiplication table of 12 from 1 to 12 and wrote it by hand, it would like something like this:

```bls
println("12 * 1 = ", 12 * 1);
println("12 * 2 = ", 12 * 2);
println("12 * 3 = ", 12 * 3);
println("12 * 4 = ", 12 * 4);
println("12 * 5 = ", 12 * 5);
println("12 * 6 = ", 12 * 6);
println("12 * 7 = ", 12 * 7);
println("12 * 8 = ", 12 * 8);
println("12 * 9 = ", 12 * 9);
println("12 * 10 = ", 12 * 10);
println("12 * 11 = ", 12 * 11);
println("12 * 12 = ", 12 * 12);
```

Well, that works I suppose. But, is it convenient? Nope. But is it hard? Well, also no. But this is just from 1 to 12. What about 1 to 100? Or 1 to 1000? It becomes far more incovenient.

This is where loops come in. Using a loop, the above program can be written as:

```bls
for let i = 1; i <= 12; ++i {
  println("12 * ", i, " = ", 12 * i);
}
```

Phew! That was much easier to write! If we wanted to do this `1000` times, all we need to change is `i <= 12` to `i <= 1000`! This is a really basic example, but it should suffice.

Blossom has 3 kinds of loops:
  
  1. [`for`](#for-loops) loops
  2. [`for in`](#for-in-loops) loops
  3. [`while`](#while-loops) loops

## `for` loops

This is the most basic, common, and most used loop - for a good reason. The `for` loop is the basis for all other loops! This form of loop contains 4 parts:

  1. Initialization - This is used for initializing the loop, only run once before the loop runs
  2. Execution Condition - This is used to put a condition to determine when a loop should exit. In the example above, as long as `i <= 12` evaluates to `true`, the loop block will continue to run!
  3. Modification - This component allows for modification to some value - often to update the variable being evaluated in the condition. In the example above, we increment `i` by `1`
  4. Loop Block - This is the block of code being executed as long as the condition is `true`.

Other than the Loop block, all other components are optional and we can skip them to make an infinite loop.

## `for in` loops

This loop is also commonly called a `range based for` loop because it works for a range of items. Although very helpful as well as readable, it is not always feasible. For example, skipping initialization or modification is not an option while using a `for in` loop. In any case, it is available for when it is required. The following program is the first example in a `for in` structure:

```bls
for i in range(1, 13) {
  println("12 * ", i, " = ", 12 * i);
}
```

Note that we use `1` to `13` in the `range` function. This is because the `for in` loop runs until the variable is **less than** the end expression. When translated into the `for` loop format:

```bls
for let i = 1; i < 13; ++i {
  println("12 * ", i, " = ", 12 * i);
}
```

It will go up to but not include 13. This translates to "from `1` to `12`".

## `while` loops

Imagine a `for` loop but remove initialization and modification components. That is the `while` loop. Yes, it can be represented by a `for` loop, but its so much easier to use a `while` loop. It avoids the awkwardness of having two useless semicolons (`;`) that we have to deal with if we use a `for` loop.

An example of the `while` using vectors can be as so:

```bls
import "std/vec" as vec

let v = vec.new(1, 2, 3, 4);

while !v.empty() {
  v.pop();
}
```

All the above program does is delete the last element in a vector until it is empty.

The same program example using `for` loops would be:

```bls
import "std/vec" as vec

let v = vec.new(1, 2, 3, 4);

for ;!v.empty(); {
  v.pop();
}
```

Not as pleasing is it?
