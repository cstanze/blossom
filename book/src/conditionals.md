# Conditionals

In Blossom, like most languages, conditionals follow the format:

```bls
if <some expression> {
  // do something
} elif <maybe another expression> {
  // do something else
} else {
  // nothing else worked, let's fallback to this.
}
```

As of Blossom v0.1.0, the following style is also supported:

```bls
if <something> {
  // do something
} else if <something else> {
  // do another thing
} else {
  // try one final thing
}
```

Do note that the `elif`/`else if` and `else` portions are not mandatory. But, the order is absolute. A conditional can have any number `elif`/`else if` sections.

## Logical Operators

Logical Operators are operators that perform logical comparisons. They're interpreted from left to right, so if one fails (in a logical `and`), the others won't be interpreted. For example,

```bls
let x = false;
func test() {
  println("test!");
  return true;
}

// won't print "test!" to the console
// because its not interpreted
if x && test() {
  println("good!")
} else {
  println("not good!")
}
```

Below is a short list of logical operators:

- `&&` - Checks if both sides evaluate to the `true` boolean value.
  - `true && true` - True
  - `false && true` - False
  - `true && false` - False
  - `false && false` - False
- `||` - Checks if at least one side evaluates to the `true` boolean value.
  - `true || true` - True
  - `true || false` - True
  - `false || true` - True
  - `false || false` - False
- `!` - Gets the opposite boolean value.
  - `!true` - False
  - `!false` - True
