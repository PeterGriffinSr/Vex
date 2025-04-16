# Vex Type System

Vex has a **strong static type system** designed to catch bugs early and promote correctness in functional programming. This page walks you through the built-in types, type inference, custom types, and patterns.

---

## Primitive Types

Vex supports the following primitive types:

| Type   | Description                        | Example               |
|--------|------------------------------------|-----------------------|
| `int`  | 64-bit signed integer              | `42`, `-7`, `0`       |
| `float`| 64-bit floating point              | `3.14`, `-0.5`        |
| `bool` | Boolean value                      | `true`, `false`       |
| `char` | A single character                 | `'a'`, `'%'`          |
| `string` | UTF-8 string                     | `"hello"`             |

---

## Type Inference

Vex can usually infer the type of a value or function:

```ocaml
let x = 5;               # int
let greeting = "Hi!";    # string

let double(n) = n * 2;   # inferred: int -> int
```

You can add type annotations if desired:
```ocaml
let double(n: int): int = n * 2;
```

---

## Function Types
Functions are first-class and have arrow types:
```ocaml
let add(a: int, b: int): int = 
    a + b;
```
- Type: `(int, int) -> int`

Higher-order functions are supported:
```ocaml
let apply(f: (int) -> int, x: int) = f(x);
```

---

## Custom Types
Use `type` to define your own data types:
```ocaml
type Option = Some(int) | None;
```
Each variant can optionally carry data. Here's a more complex one:
```ocaml
type Result = Ok(string) | Error(string);
```

### Pattern Matching with Types
You can match custom types easily:
```ocaml
let describe(opt: Option): string = 
    match opt with
        | Some(x) => "Got " + string(x)
        | None => "Nothing";
```

---

## List Types
Lists are homogeneous, and the type is inferred:
```ocaml
let nums = [1, 2, 3];         # List of int: int[]
let names = ["a", "b", "c"];  # string[]
```

---

## No Nulls, No Crashes
Vex avoids nulls entirely. Use types like Option instead:
```ocaml
type Option = Some(int) | None;

let safeDivide(a, b) =
    if b == 0 then None else Some(a / b);
```