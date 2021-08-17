# Variables

Declaring variables is as simple as:

```bls
let x = 10;
```

Using the `let` keyword, we can specify a variable name and then assign a value. Since Blossom is dynamically typed, there is no need to markup the type of the variable.

## Reassigning Variables

In the following program:

```bls
let x = 10;
x = "hiya!";
```

We declare the variable `x` with the value `10`. Then, we **attempt** to reassign x with the string `"hiya!"`. We happen to get an error though...?

```txt
reassign.bls:2:3: error: type mismatch for assignment: String cannot be assigned to variable of type: Int

x = "hiya!";
  ^
```

Despite having a dynamic type system, Blossom retains some restrictions from statically typed languages. There *is* one trick to get around this. In the following program, we assign `x` to `10` and then redeclare `x`.

```bls
let x = 10;
let x = "hiya!";
```

Variable redeclaration is a viable way of changing types. It recreates a variable with the same name and different type. The old variable will no longer exist. Be sure to use this feature with the types in mind.

In the next chapter, we'll dive deeper into Blossom's [Data Types](./data_types.md).

## Scope

Scoping rules are simple and follow similar rules from other languages.

For example, this creates a variable named `x` in the global scope.

```bls
let x = 10;
```

The global scope is accessable by all and can be use anywhere. Also, the variable won't be destroyed until the end of the program.

On the other hand, if we run the following program:

```bls
{
  let a = 20;
}
println(a);
```

We'd get an error similar to the following:

```txt
scope.bls:4:9: error: variable 'a' does not exist

println(a);
        ^
```

If we declared another variable named `a` outside of the block scope, the `println` function would be able to pick up the new declaration. Such as in the following program:

```bls
{
  let a = 20;
}
let a = "hey there!";
println(a); // prints "hey there!" instead of 20
```
