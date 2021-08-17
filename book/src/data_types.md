# Data Types

In Blossom, we have common data types that come from many other languages. The following types are:

- [`Int`](#integers)
- [`Float`](#floats)
- [`Bool`](#booleans)
- [`String`](#strings)

Some programmers may note the missing data type: `char`. Despite having the concept of strings in Blossom, we don't have the `char` primitive.

Blossom is a `dynamically typed` language. This means we don't provide types for the variable and we don't perform type checks and buildtime. Instead, the interpreter deduces the types of variables at runtime, similar to JavaScript.

```bls
let s = "hiya";
let i = 0;
```

In the above example, the interpreter creates 2 variables named `s` and `i`. The interpreter then deduces these 2 variables as the types: `string` and `int` respectively.

## Basic Types

### Integers

Integers represent negative and positive numbers in Blossom. All numbers without a floating point are valid integers.

```bls
let some_int = 132872987298732287939897289723;
let some_neg_int = -132872987298732287939897289723;
```

Additionally, unlike many (usually compiled) languages, Blossom doesn't limit you to 32, 64, or 128 bit numbers. Any number is usable, as long as it fits in system memory.

### Floats

Floating point numbers are simply integers but with more precision as there is the ability to use the decimals (`.`). Because we use the same system for storing floats as with integers, floating point values can also be arbitrarily long with high precision.

```bls
let some_float = 13287298729873.2287939897289723;
let some_neg_float = -13287298729873.2287939897289723;
```

**Note:** Floats that don't require anything after a decimal point after can be initialized using a `.0` after the number like so:

```bls
let some_float = 5.0;
```

## Booleans

Booleans are simple values to indicate `true` and `false`. Unlike other types, `true` and `false` are actually constant variables that cannot be reassigned and are always globally available.

```bls
let t = true;
let f = false;
```

## Strings

Strings are simple data structures to hold some sort of text or the like.

Constant strings (const strings) are strings that are hardcoded into your code. These strings are written using one of the 3 available ways:

  1. Enclosing content in single quotes (`'`)
  2. Enclosing content in double quotes (`"`)
  3. Enclosing content in back ticks (`` ` ``)

Blossom also supports escape sequences such as:

```txt
\a - Bell
\b - Backspace
\f - Formfeed
\n - New line
\r - Carriage return
\t - Tab (horizontal)
\v - Tab (vertical)
\\ - Escaped Backslash
\" - Escaped Double quote
\' - Escaped Single quote
```

Here are a couple examples of using strings:

```bls
// simple double quoted string
println("hello world!")

// simple single quoted string
println('hiya there!')

// backticks too!
println(`we also support embedded quotes like the double (") and single (') quotes.`)

// same with other strings
println("don't you think 'this' is cool?")
println('i "certainly" do!')
```

Please note that the following does not work:

```bls
println("having this quote: " unescaped is invalid!")
```

```bls
println('same with singe quotes: ' uhoh!')
```

```bls
println(`backticks as well! ` scary!`)
```
