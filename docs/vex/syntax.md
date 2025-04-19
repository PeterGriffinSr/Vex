# Vex Syntax Overview
Vex is a purely functional language with a concise, readable syntax. This guide covers the core constructs you'll encounter when writing Vex programs.

---

## Variable Declaration
In Vex, all variables are **immutable by default**. This reflects the functional language philosophy where values are treated as fixed, mathematical entities rather than mutable containers. This section explains our variable declaration syntax in detail.

- [Syntax Examples](#syntax-examples)
- [Why we chose this syntax](#why-we-chose-this-syntax)

### Syntax examples
To define a variable in Vex, use the `val` keyword followed by the **type**, a colon, the **variable name**, an equals sign, and the **assigned value**:
```
val int: x = 10;
```
This declares a variable named `x` of type `int` with the value `10`.

Here are more examples:
```
val float: pi = 3.1415;
val string: name = "Vex";
val bool: flag = true;
val list<int>: nums = [1, 2, 3, 4, 5];
```

### Why we chose this syntax
The syntax design of Vex is inspired by **mathematical rigor**, with the goal of being both expressive and accessible to new programmers.

#### **Type-first, Name-second**
We place the type before the variable name:
```
val int: x = 10;
```
This mirrors how types are annotated in mathematics.

Meaning: "`x` is a value of type `int` equal to `10`."

To understand the reasoning behind the `val int: x = 10;` syntax, it's helpful to explore its roots in **set theory** and **type theory**—two pillars of mathematical logic and functional programming. Vex intentionally reflects these mathematical traditions to promote clarity, correctness, and immutability.

### Set Theory: Variables as Elements of Sets
In mathematics, we write:
    $$x \in \mathbb{Z}$$ 

This means:
> "x is an element of the set of integers."

We borrow the notation from set theory and type theory. Some key parallels:
| Math Symbol | Meaning | Vex Equivalent | 
|-------------|---------|----------------|
| $\mathbb{Z}$| The set of all [integers](https://en.wikipedia.org/wiki/Integer) | `int` |
| $\in$       | "is an element of" | `:` (in `int: x`) |
| $\Rightarrow$| Implies / leads to | `=` (value assignment) |

So val `int: x = 10;` can be read as:

> "x is a value of type int, equal to 10."

> Or mathematically: $x \in \mathbb{Z} \text{ and } x = 10$

This syntax aligns perfectly with a **mathematically rigorous, declarative** way of programming.

### Type Theory: Types as Sets of Values
In **type theory**, types can be understood as **sets of values**—just like in set theory. But instead of abstract mathematical sets, we refer to **concrete types** in a programming language.

- `int` is the type of all integers → analogous to $\mathbb{Z}$
- `bool` is the type containing only `{true, false}`
- `string` is the set of all valid string values
- `list<int>` is a set of all lists whose elements are of type `int`

So the declaration:
```
val list<int>: xs = [1, 2, 3];
```
Is equivalent to the type-theoretic idea:

> "xs is an element of the type (set) of lists of integers."

> Or formally: $xs \in \text{List}(\mathbb{Z})$

### Why This Makes Sense for Functional Programming
Functional programming languages are deeply rooted in mathematics. They embrace concepts like:

- **Purity**: functions return the same result for the same inputs (no side effects)
- **Immutability**: once defined, values never change
- **Referential Transparency**: expressions can be substituted by their values without changing behavior

These principles are **mathematical in nature**, and Vex’s syntax reflects that.

**In Functional Languages**:
You don’t `assign` to a variable like in imperative languages—you **define** a binding between a name and a value.

So instead of saying “put 10 into x,” you're saying:
> "x is defined as the integer 10 and cannot be changed."

Which is conceptually the same as:
$$x = 10 \space \text{where} \space x \in \mathbb{Z}$$ 

---

## Function Definitions in Vex

Functions in Vex are **first-class**, **pure**, and **immutable**. That means they can be passed around like any other value, never mutate state, and always produce the same result for the same inputs—just like mathematical functions

In this section, we’ll explain:
- [Syntax Overview](#syntax-overview)
- [Theoretical Foundation](#theoretical-foundation)
- [Type Annotations](#type-annotations)
- [Examples and Reasoning](#examples-and-reasoning)

### Syntax 
The canonical function definition in Vex looks like this:
```
val (int, int) -> int: add fn (a, b) => a + b;
```
This declares a value add of type (int, int) -> int, meaning a function that takes two integers and returns an integer.

#### Let’s break it down:
| **Section** | **Meaning** |
|-------------|-------------|
| `val`       | Immutable declaration keyword |
| `(int, int) -> int` | The function’s full type signature |
| `:`         | "Has the type of..." |
| `add`       | The name of the function |
| `fn (a, b)` | Function parameters |
| `=>`        | Function body definition (like => in math: "maps to") |
| `a + b`	  | The return expression |

This reads as:
> "Declare a value named add which is a function from (int, int) to int, defined as fn (a, b) => a + b."

### Theoretical Foundation
**In Set Theory and Type Theory**

In mathematics, a function from two integers to an integer is written like this:
    $$f : \mathbb{Z} \times \mathbb{Z} \to \mathbb{Z}$$

This is the exact meaning of:
```
val (int, int) -> int: f fn (a, b) => ...
```
Let’s explain the notation:
- $\mathbb{Z}$ = set of integers
- $\times$ = Cartesian product (a tuple of values)
- $\rightarrow$ = a mapping from input to output
- $f$ = a mapping function

In this form, a function is not a “sequence of steps” like in imperative programming—it’s a **mapping** between values in sets.

So when we say:
```
val (int, int) -> int: add fn (a, b) => a + b;
```
We mean:

> "add is a function in the set: $(\mathbb{Z} \times \mathbb{Z}) \rightarrow \mathbb{Z}$"

> It is a pure transformation of data, not a command.

### Type Annotations
Vex's function syntax includes **explicit types**, because types:
- Describe the contract of a function
- Ensure correctness at compile-time

**Generic Form:**
```
val (T1, T2, ..., Tn) -> R: name fn (a1, a2, ..., an) => body;
```

Where:
- `T1` to `Tn` are input type
- `R` is the return type
- `name` is the identifier being bound
- `body` is the return expression

If a function takes no arguments:
```
val () -> string: greet fn () => "hello!";
```

### Examples and Reasoning

**Recursive Function (e.g. factorial)**
```
val (int) -> int: fact fn (n) =>
    if n <= 1 then 1 else n * fact(n - 1);
```
This matches:

> $f(n) = \begin{cases}1 & \text{if } n \leq 1 \\ n \cdot f(n-1) & \text{otherwise}\end{cases}$

---

## Conditionals

Vex uses expressions for branching. The syntax:
```
if condition then expr1 else expr2
```
Always evaluates to a value.

> $$\text{if } p \text{ then } x \text{ else } y$$

This matches mathematical case expressions:
> $f(x) = \begin{cases}a & \text{if } p \\ b & \text{otherwise}\end{cases}$

Conditionals are **expressions**, not statements—meaning they always return a value and don’t mutate state.

---

## Pattern Matching

Vex has powerful pattern matching, allowing functions and expressions to deconstruct values declaratively:
```
match value with
    | pattern1 => expr1
    | pattern2 => expr2
```

It mirrors the case analysis in logic and algebra.

> $$\text{match } v \text{ with } \begin{cases}p_1 \Rightarrow e_1 \\ p_2 \Rightarrow e_2 \end{cases}$$

Used for unpacking data, handling enums, and clean branching.

---

## Lambda Expressions

Anonymous functions are defined like this:
```
val (int, int) -> int: fn (x, y) => 
    x + y;
```

This corresponds to:
> $\lambda(x, y).\ x + y$

Lambdas are expressions, and they evaluate to a function value.

---

## Pipeline Expressions

Vex allows chaining expressions using `|>`:
```
value |> fn1 |> fn2
```

Which is equivalent to:
```
fn2(fn1(value))
```

Mathematically:
> $f \circ g \circ h (x)$ = $h(g(f(x)))$

Pipelines emphasize flow of data, avoiding nested calls.

---

## Higher-Order Functions

Vex supports higher-order functions—functions that take other functions as parameters or return them as results:
```
val ((int) -> int, list<int>) -> list<int>: map fn (f, xs) => ...;
```

This means:
> `map` is a function that takes a function and a list, and returns a transformed list.

Type-theoretically:
> $$\text{map} : (A \to B, \text{List}(A)) \to \text{List}(B)$$

Functions are **first-class citizens** and can be composed, passed, and returned like any value.

---

## Recursion

Since Vex lacks loops, recursion is the standard way to express iteration.

Example:
```
val (list<int>) -> int: sum fn (xs) =>
    match xs with
    | [] => 0;
    | x :: rest => x + sum(rest);
```

This is equivalent to mathematical recurrence:
> $$\text{sum}(xs) = \begin{cases}0 & \text{if } xs = [] \\ x + \text{sum}(rest) & \text{otherwise}\end{cases}$$

Recursion is natural and idiomatic in Vex due to immutability and pure functions.

---