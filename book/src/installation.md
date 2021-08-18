# Installation

To begin installing `blossom`, you must first ensure you have `CMake`, `Ninja`, `LibGMP`, and `LibMPFR` installed on your computer.

It must be noted that Blossom does not yet support Windows. Windows support is in development.

## Automated Installation

You can automatically build Blossom and its standard library by downloading and running `build.sh`. It requires [`Git`](https://git-scm.org) and the packages listed above.

```bash
# Download the script (example with wget)
wget https://raw.githubusercontent.com/cstanze/Blossom/main/build.sh

# Run it
sh build.sh
```

## Manual Installation

Through manual installation, you're able to enable things like debug info which can prove to be very useful in debugging.

To begin, first clone the GitHub repository. Simple enough right?

Next, `cd` into the cloned repo then create a directory named `build` or whatever name you'd like - this directory must be removed or gitignored before creating a pull request. After you've created the new directory, `cd` into it and move to the next step.

At this point, make sure you have all the tools and libraries required to compile Blossom. If not, install them now. Otherwise, continue to the next step.

Run the following command in your shell:

```bash
cmake \
  -DCMAKE_BUILD_TYPE=Release \
  -G Ninja \
  ..
```

This will initialize the current directory (it should be that directory you created).

Lets explain the options though:

- `-DCMAKE_BUILD_TYPE=Release` - This sets the build type to `Release`. You may also change this to `Debug` to get debug information.
- `-G Ninja` - This sets the generator to Ninja. If you'd rather use another generator, you may pick from the CMake supported generators list.
- `-DMEM_PROFILE` - This wasn't added on the command but may prove useful to contributors. This turns on the memory profiling for the Blossom interpreter.
- `-DPREFIX_PATH=<path>` - This also wasn't added but allows you to set the install prefix. The default prefix is `$HOME/.blossom`

Then you can compile and install everything:

```bash
ninja install -j8
```

Great! You've successfully compiled and installed Blossom!
