# Vex Type System

Vex has a **strong static type system** grounded in mathematical logic and designed for clarity, precision, and correctness. Every value in Vex must have a known, explicit type at compile time. This section explores the core types, function types, user-defined types, and how they relate to concepts in **type theory** and **set theory**.

---

## Primitive Types

Vex includes the following **primitive types**, each representing a well-defined mathematical set:

| Type     | Description                        | Mathematical Equivalent   |
|----------|------------------------------------|---------------------------|
| `int`    | 64-bit signed integers              | $( \mathbb{Z} )$            |
| `float`  | 64-bit floating point numbers       | $( \mathbb{R} )$            |
| `bool`   | Boolean values                      | $( { \text{true}, \text{false} } )$ |
| `char`   | A single Unicode character          | A subset of $( \Sigma )$ (Unicode set) |
| `string` | UTF-8 encoded string                | $( \text{List}(\Sigma) )$   |

Every literal you write in Vex must be typed explicitly:
```vex
val int: age = 30;
val float: pi = 3.1415;
val bool: isReady = true;
val char: letter = 'v';
val string: greeting = "hello";
```

---

## Function Types

Functions in Vex are pure mappings between sets of values and must include full type signatures.

```vex
val (int, int) -> int: add fn (a, b) => a + b;
```

This declares `add` as a function from $( \mathbb{Z} \times \mathbb{Z} \rightarrow \mathbb{Z} )$.

### Syntax Format
```
val (<arg types>) -> <return type>: <name> fn (<args>) => <expression>;
```

A function type in Vex consists of:
- **Argument types** inside a tuple
- **An arrow** (`->`) separating argument types from the return type
- **The name and implementation** that follows the type

### Example
```vex
val () -> string: hello fn () => "hi!";
```
This defines a function `hello` that takes no arguments and returns a `string`.

### Mathematical View
Multiple arguments are modeled as tuples:
$[
\text{add} : \mathbb{Z} \times \mathbb{Z} \to \mathbb{Z}
$]

A function type $((A, B) \to C$) represents a mapping from the Cartesian product of types $(A$) and $(B$) to type $(C$).

This strong type structure makes functions predictable, mathematically verifiable, and free of side effects unless explicitly modeled.

---

## Higher-Order Functions

Functions can be passed as arguments, returned from other functions, and composed.
```vex
val ((int) -> int, int) -> int: apply fn (f, x) => f(x);
```

Here, the type of `apply` is:
$[
((\mathbb{Z} \to \mathbb{Z}), \mathbb{Z}) \to \mathbb{Z}
$]

---

## Lambda Expressions

Anonymous functions (lambdas) are expressed using the same `fn` syntax inline:
```vex
val (int, int) -> int: fn (x, y) => 
    x + y;
```

---

## Recursion

Vex supports recursion for defining self-referential functions:
```vex
val (int) -> int: fact fn (n) =>
    if n <= 1 then 1 else n * fact(n - 1);
```

---

## List Types

Lists in Vex are homogeneous and have explicit type signatures:
```vex
val list<int>: nums = [1, 2, 3];
val list<string>: words = ["hi", "there"];
```

This reflects:
$[
\text{nums} \in \text{List}(\mathbb{Z})
$]

---

## Custom Types

Use `type` to define new **algebraic data types**:
```vex
type Option = Some(int) | None;
```

You can define variants with or without data:
```vex
type Result = Ok(string) | Error(string);
```

This is equivalent to a disjoint union of sets:
$[
\text{Result} = \text{Ok}(\text{String}) \cup \text{Error}(\text{String})
$]

---

## Pattern Matching

You can destructure and branch on custom types using `match`:
```vex
val Option -> string: describe fn (opt) =>
    match opt with
        | Some(x) => "Got " + string(x)
        | None => "Nothing";
```

---

## No Nulls, No Crashes

Vex prohibits null references entirely. Instead, use sum types like `Option` to handle absence:
```vex
val (int, int) -> Option: safeDivide fn (a, b) =>
    if b == 0 then None else Some(a / b);
```

This promotes safety via types, ensuring all edge cases are handled at compile-time.