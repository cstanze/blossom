# Naming Conventions

## Language-specific Rules

Blossom defines specific rules on how you can name variables. These rules are similar to other languages like Rust, Swift, etc. The rules are as follows:

  1. Must begin with an alphabetic character (A-Z, case insensitive) or an underscore.
  2. Can contain numbers anywhere except the first character
  3. Cannot contain any symbol (except for the rule below) other than alphanumeric characters or underscores
  4. Variables (usually function names) can end with `?` or `!`.

## Standard Conventions

Blossom tries to keep a solid and standard naming convention. Similarly to Rust, variable names should be in `snake_case` and struct/enum names should be in `PascalCase`.  Functions (including member functions) should also follow the `snake_case` naming convention.

For functions that throw exceptions, we append `!` to the end of the function name.

For functions that return a boolean value, we append `?` to the end of the function name.

Below are examples of these naming conventions.

```bls
let this_is_a_var_name = 20;

func this_function_throws!() {
  raise("Wow!");
}

func is_it_true?() {
  return true;
}

import "std/lang" as lang
let SomeStruct = lang.struct(
  hello_structs = 10
);
```

~~To simplify things, we've created `blossom-format` to aid in keeping a consistent code style.~~

**For the time being,** `blossom-format` **is not available**
