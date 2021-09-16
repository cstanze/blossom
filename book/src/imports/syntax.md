<!-- markdownlint-disable MD033 -->
# Learning the syntax

Imports are a vital structure for Blossom. As your project begins to grow, so will your codebase. To reduce clutter and file size, we've added support for importing modules, whether they be coded in Blossom or C++.

## Basic Imports

By default, Blossom imports the `core` and `utils` library to use. This includes many functions for primitive types. This also includes the `print` and `println` functions. Additional functions available to you are `import` and `mload` which we'll discuss in more detail. The compiler also includes a keyword *named* `import`. Notice the conflictions between the function name and keyword? We'll discuss that in more detail later.

For now, we'll use the `import` keyword to import Blossom modules.

To import a module, you must specify the `import` keyword followed by a string as the name of your module. Optionally, you may also import the module as a variable by using the `as` keyword.

```bls
import "std/vec" as vec
```

The example above imports the `std/vec` library as a variable named `vec`. `std/vec` is the standard vector library, essentially the equivalent to arrays or lists from either JavaScript or Python (respectively).

You may omit the `as` section to import a module as is. For example, the `std/str` library doesn't import any types. It simply imports some extensions to the string type:

```bls
{{#include ../../../include/std/str.bls:1:4}}
```

For these types of situations, you can import the module without assigning it to a variable:

```bls
import "std/str"
```

Then, we can use the `substr` function as we please.

```bls
import "std/str"

let x = "xxhiyaxx";

println(x.substr(2, 4)) // prints "hiya"
```

## Attribute Imports

Imagine the following code:

```bls
import "std/optional" as optional

let x = optional.Some(1)
println(x)
```

It doesn't seem like much, but when it gets to a bigger scale, having to write out `optional.Some` many many many times is exhausting and somewhat unreadable. But, with attribute imports, we can simply import `Some` itself!

```bls
// with attribute imports...
import "std/optional" with Some

// we can import Some!
let x = Some(1)
println(x)
```

Much easier! But wait, what if we wanted to use the name `Some`? That's fine, we'll just alias `Some` to another name!

```bls
import "std/optional" with Some as S

let x = S(1)
println(x)
```

Even better. With attribute imports, importing modules, types, functions, and variables is much much easier.

## Multiple Imports

This section is a bit short but here is the basic rundown of importing multiple modules in one line: we can separate imports by commas!

### Multiple Basic Imports

```bls
import "std/str", 
       "std/sys" as sys,
       "std/vec" as vec,
       "std/map" as map
```

### Multiple Attribute Imports

For attribute imports, we separate different attributes using an attribute list. We specify a parenthesized list of attributes (with support for aliases).

```bls
import "std/optional" with (Some as S, None)

let x = Some(1)
let y = None()
```

Attribute import lists with only 1 attribute is also allowed:

```bls
import "std/optional" with (Some as S)
```

### Putting it all together

Below is an example of basic imports and attribute imports working together (including their other variants):

```bls
import "std/optional" with (Some as S, None),
       "std/lang" as lang with (enum as em),
       "std/str"
```

Note the `with` after we already assigned `std/lang` to `lang`? This is completely valid.

<details>
<summary>Try to figure out what the final result is...</summary>

You've likely guessed by now what this does. When we import `std/lang`, it assigns the import to `lang`. But then, we reimport `lang` and extract the `enum` function into a variable alias of the name `em`. Awesome!
</details>
