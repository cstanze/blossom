# Importing external modules

Importing external modules usually consists of importing code you created. To import a local file:

```bls
// main.bls
import "./more_string_funcs"

println("".test())
```

Then in the other file:

```bls
// more_string_funcs.bls
impl String.test() {
  return self + "test"
}
```

The example above, when run, will output `test` to the console.

## Additional Notes

Please note that it is not advised to extend builtin types. Only when necessary is it advised.

Overriding methods is also not allowed for program safety.
