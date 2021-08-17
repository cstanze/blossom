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

## Multiple Imports

This section is a bit short but here is the basic rundown of importing multiple modules in one line.

We can separate imports by commas! Note, we added the newlines to make it easier to read. They're obviously optional

```bls
import "std/str", 
       "std/sys" as sys,
       "std/vec" as vec,
       "std/map" as map
```
