# Optionals

In this chapter, we'll discuss Optionals and their uses across Blossom:

## An Introduction

Optional types aren't a new concept but they've been gaining popularity in computer science and computer programming due to their safety compared to traditional `null` types. In Blossom, the `nil` type is **not** the equivalent to Optionals. But, we had safety in mind. The `std/optional` library includes a new `Option` type and a few new functions to interact with this type.

## `Option`, `Some`, and `None`

The new function `Option` provides a wrapper for the `OptionInner` type:

```bls
{{#include ../../../include/std/optional.bls:17:21}}
```

**Note:** Despite using `ptr` for the `Option` type, we use `_optionCopy` to copy the value before storing it in a `ptr`. No need to worry about changing values.

The reason for using `ptr` instead of the raw type is best explained in [Data Types](../data_types.md), where we cover certain type restrictions. These type restrictions also apply to structs. Using `ptr` allows us to get around these restrictions. Essentially creating a "type generic" for `OptionInner`.

The `Option` function creates an `OptionInner` and returns it. The `Some` function does the same except the argument does not have a default. The `None` function creates an `OptionInner` without any value.

```bls
{{#include ../../../include/std/optional.bls:23:25}}
```

```bls
{{#include ../../../include/std/optional.bls:28:31}}
```

More information can be found in the standard library documentation. Below is an example using Optionals:

```bls
{{#include ../examples/std/optional.bls}}
```
