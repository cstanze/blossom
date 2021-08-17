# Additional Notes

## `print` & `println` functions

`print` and `println` are functions that you will get to know and understand more as you write more code. These functions are both considered variadic. They take any number of arguments above the minimum.

For `print`, the minimum is 1 argument. The argument can be of any type (for struct types, the struct needs to have a member function called `str`).

For `println`, the minimum is 0 arguments. This is because `println` automatically prints a newline if no arguments are specified. Otherwise, it does the same as `print` and then prints a newline.

## Semicolons

The semicolon (`;`) at the end of the `println` call is totally optional! Technically yes, they are optional. But, they are advised as they help promote good code practices! This is because certain statements such as `continue`, `return`, and `break` require a semicolon (or another expression) after its declaration.

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
