# Front page snippets for Vex

## Vex
A sample of fibonacci sequence in Vex.
```ocaml
val (int) -> int: fib fn (n) =>
    if <= 1 then n else fib(n - 1) + fib(n - 2);
```
## C++
```cpp
#include <iostream>
using namespace std;

int fib(int n) {
    if (n <= 1) {
        return n;
    }

    int a = 0, b = 1, c;

    for (int i = 2; i <= n; i++) {
        c = a + b;
        a = b;
        b = c;
    }

    return b;
}

int main() {
    cout << fib(92) << endl;

    return 0;
}
```