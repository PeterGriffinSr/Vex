# Vex Syntax Overview
Vex is a purely functional language with a concise, readable syntax. This guide covers the core constructs you'll encounter when writing Vex programs.

---

## Program Structure
A Vex program consists of a sequence of **statements**:

```ocaml
let square(x) = 
    x * x;

let main() =
    print(square(5));
```

---

## Statements
There are three kinds of statements in Vex:
- `let` bindings (including recursive functions)
- `type` definitions
- top-level expressions

```ocaml
let pi = 3.14;

type Option = Some(int) | None;

print("Ready!");
```

---

## Let Bindings
Use let to define values or functions:
```ocaml
let name = "Vex";

let add(a, b) = 
    a + b;

let rec factorial(n) =
    if n == 0 then 1 else n * factorial(n - 1);
```
- Use `let rec` for recursive definitions.
- Functions use the form `let name(params) = expression`.

---

## Type Definitions
Define custom types with `type`:
```ocaml
type Result = Ok(string) | Error(string);

type Option = Some(int) | None;
```
Each variant can optionally hold a value.

---

## Expressions
Expressions are the core of Vex. Here's a breakdown:

### Literals
```
42
"hello"
[1, 2, 3]
```

### If Expressions
```ocaml
let result(x) =
    if x > 0 then "positive" else "non-positive";
```
- `if` always requires an `else`.

### Match Expressions
```ocaml
let describe(opt) = 
    match opt with
    | Some(x) => "Got " + string(x)
    | None => "Nothing";
```

### Lambda Expressions
```ocaml
let square() = 
    (x) => x * x;
```
- Anonymous functions using arrow notation.

### Function Calls
```ocaml
add(1, 2);
```
- Function name followed by arguments in parentheses.

### Pipeline Expressions
```ocaml
"hello"
|> string.uppercase
|> print;
```
- Chains expressions left to right, useful for data transformations.

### Parameters and Arguments
Functions can take multiple parameters:
```ocaml
let greet(name, age) = 
    print_string "Hello " >> name;
```
Calls pass arguments in the same format:
```ocaml
greet("Alice", 30);
```

### Lists
Lists are created with square brackets:
```ocaml
let nums = [1, 2, 3, 4];
```
- Lists can be pattern matched too!

### Patterns
Patterns appear in `match` cases and can include:
- Identifiers: `x`
- Literals: `"yes"`, `42`
- Wildcards: `_`
- List patterns: `[x, y]`

### Parenthesized Expressions
Use parentheses to group expressions or override precedence:
```ocaml
let result() = 
    (2 + 3) * 4;
```
---