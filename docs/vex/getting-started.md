# Getting started with Vex

Welcome to Vex! This guide will help you install Vex, write your first program, and understand the basics to get up and running quickly.

---

## Installation

To install Vex, follow these steps:

### Using Prebuilt Binaries (Recommended)
1. Visit the [official Vex website](#) and download the latest release for your operating system.
2. Extract the archive and add the `vex` binary to your system's `PATH`.

### Building from Source
If you prefer building from source:

```
git clone https://github.com/PeterGriffinSr/vex.git
cd vex
meson setup build
meson compile -C build
cd build
meson install
```
Note: Ensure you have `meson`, a C compiler (like `gcc` or `clang`), and `flex/bison` installed.

---

# Your First Vex Program
Create a file called `main.vex` with the following content:
```
let main() =
    print("Hello, world");
```
To run it:
```
vex run main.vex
```
Or to compile and run:
```
vex build main.vex -o hello
./hello
```

---

# Running the REPL
You can experiment with Vex interactively using the REPL:
```
vex repl
```

Try this:
```
> let square(x: int) = x * x;
> sqaure(4)
> 16
```

---

# What's Next?
Now that you’re set up, explore the rest of the documentation:
- [Syntax Overview](/docs/vex/syntax.md)
- [Type System](docs/vex/type-system.md)
- [Functions](docs/vex/function.md)