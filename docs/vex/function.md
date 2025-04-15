# Functions in Vex

In Vex, functions are first-class citizens. You can define, pass, return, and compose functions in elegant and expressive ways.

---

## Defining Functions

Functions are defined using `let`:

```ocaml
let add(a, b) = a + b;
```
- Parameters are comma-separated inside parentheses.

- The body is a single expression (no `return` needed).

### Recursive Functions
Use let rec for recursion:
```ocaml
let rec factorial(n) =
    if n == 0 then 1 else n * factorial(n - 1);
```
---

## Function Types
You can annotate function types:
```ocaml
let square(x: int): int = x * x;
```
Multiple parameters are written as tuples:
```ocaml
let concat(a: string, b: string): string = a + b;
```

---

## Anonymous Functions (Lambdas)
Vex supports concise lambda syntax:
```ocaml
let double = (x) => x * 2;
let add = (a, b) => a + b;
```
Lambdas are useful for passing functions:
```ocaml
apply((x) => x + 1, 10);
```

---

## Higher-Order Functions
You can pass functions as arguments or return them:
```ocaml
let apply(f, x) = f(x);
let makeAdder(n) = (x) => x + n;
let add5 = makeAdder(5);
add5(3);  # => 8
```
