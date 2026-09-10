# Functions in C

*This lecture assumes variables, data types, operators, and control flow (loops and conditionals) have already been taught. It does not assume Arrays or Pointers — Functions is covered before both in this sequence. Where relevant, this document names those topics only to forward-reference limitations they later resolve.*

## 1. Learning Objectives

By the end of this document, you should be able to:

- Explain why repeating the same logic across a program is a problem, and how a function solves it.
- Correctly write a function's prototype, definition, and call, including the `void` variations for return type and parameters.
- Predict what a function will and will not change in its caller, based on pass-by-value.
- Explain, using the call-stack model, why local variables reset on every call and why recursion works.
- Trace a recursive function by hand and identify a missing or unreachable base case.
- Decide, given a piece of logic, whether it deserves its own function or should stay inline.

## 2. Why Functions Exist

Suppose a program needs to compute the area of a circle for three different radii:

```c
#include <stdio.h>

int main(void) {
    double radius1 = 2, area1 = 3.14159 * radius1 * radius1;
    double radius2 = 5, area2 = 3.14159 * radius2 * radius2;
    double radius3 = 10, area3 = 3.14159 * radius3 * radius3;

    printf("Area 1 = %.2f\n", area1);
    printf("Area 2 = %.2f\n", area2);
    printf("Area 3 = %.2f\n", area3);
    return 0;
}
```

The formula `3.14159 * radius * radius` is duplicated three times. If it later needs to change — say, to use a more precise value of pi — every copy has to be found and updated individually, and it is easy to update some copies while missing others.

A function lets the formula be written once and reused by name:

```c
#include <stdio.h>

double computeArea(double radius) {
    return 3.14159 * radius * radius;
}

int main(void) {
    printf("Area 1 = %.2f\n", computeArea(2));
    printf("Area 2 = %.2f\n", computeArea(5));
    printf("Area 3 = %.2f\n", computeArea(10));
    return 0;
}
```

Now the formula exists in exactly one place. This is the core value of a function: it turns "the same logic, used several times" into something written once and run by name.

## 3. Mental Model

Picture a function as a machine on an assembly line. It has an input slot where raw material goes in (the parameters), a fixed internal process that always runs the same way (the function body), and an output slot where a single finished result comes out (the `return` value). Once the machine is built, it can be run again and again with different inputs, without being rebuilt each time.

| Part of the Machine | Function Equivalent |
|---|---|
| Input slot(s) | Parameters |
| Fixed internal process | The function body |
| Output slot | The `return` value |
| Running the machine again | Calling the function again |

One consequence of this model matters for the rest of this document: each "run" of the machine is self-contained. What happens inside one run cannot be seen or changed by another run, and a run cannot reach back and change something outside the machine either — unless it is deliberately given a way to do so (a limitation formalized in Section 5 and resolved later with pointers).

## 4. Syntax and Core Mechanics

A function appears in up to three distinct places:

| Location | Example | Purpose |
|---|---|---|
| Prototype (declaration) | `double computeArea(double radius);` | Tells the compiler the function's signature before it is used, if the full definition appears later in the file |
| Definition | `double computeArea(double radius) { return 3.14159 * radius * radius; }` | The actual implementation |
| Call | `computeArea(5)` | Runs the function with a specific input |

Signature forms:

| Form | Example | Meaning |
|---|---|---|
| Returns a value, takes parameters | `int add(int a, int b)` | Standard function with input and output |
| No return value | `void printBanner(void)` | Performs an action; returns nothing |
| No parameters | `int getConstant(void)` | Takes no input |
| Multiple parameters | `double average(double a, double b, double c)` | Multiple inputs, one output |

The `return` statement ends the function immediately and hands exactly one value back to the caller — a function cannot directly return more than one value (a limitation revisited when pointers are covered).

## 5. How It Works Underneath

Every time a function is called, C creates a fresh block of memory — a **stack frame** — to hold that call's parameters and local variables. This frame exists only for the duration of that one call, and is destroyed the instant the function returns. Nearly everything else in this document follows from this single fact.

- **Pass-by-value follows directly from it.** When you call `f(x)`, the *value* of `x` is copied into the new frame's parameter slot. The function works with its own private copy; the original `x` outside the frame is never touched (demonstrated in Example 2).
- **Local variable lifetime follows directly from it.** A local variable lives inside its call's frame. Once the frame is destroyed, so is the variable — this is why the same local variable does not "remember" its value from a previous call (demonstrated in Example 3), unless it is declared `static`, which stores it outside any single call's frame instead.
- **Recursion is the same mechanism, repeated.** When a function calls itself, each call receives its own independent frame, with its own independent copies of the parameters and locals. This is what allows, for example, `factorial(3)` and `factorial(2)` to exist at the same time without interfering with each other while one is waiting on the other's result (demonstrated in Example 4).

## 6. Worked Examples

### Example 1: A Basic Function, Called Multiple Times

```c
#include <stdio.h>

double computeArea(double radius) {
    return 3.14159 * radius * radius;
}

int main(void) {
    printf("Area (r=2) = %.2f\n", computeArea(2));
    printf("Area (r=5) = %.2f\n", computeArea(5));
    printf("Area (r=10) = %.2f\n", computeArea(10));
    return 0;
}
```

Output:
```
Area (r=2) = 12.57
Area (r=5) = 78.54
Area (r=10) = 314.16
```

### Example 2: Pass-by-Value in Action

```c
#include <stdio.h>

void tryToDouble(int x) {
    x = x * 2;
    printf("Inside function, x = %d\n", x);
}

int main(void) {
    int value = 7;

    tryToDouble(value);
    printf("Inside main, value = %d\n", value);
    return 0;
}
```

Output:
```
Inside function, x = 14
Inside main, value = 7
```

`value` in `main` is unchanged. `tryToDouble` received a copy of `value` in its own stack frame; doubling that copy has no effect on the original.

### Example 3: Local Variable Lifetime vs. `static`

```c
#include <stdio.h>

void counterLocal(void) {
    int count = 0;
    count++;
    printf("Local count = %d\n", count);
}

void counterStatic(void) {
    static int count = 0;
    count++;
    printf("Static count = %d\n", count);
}

int main(void) {
    counterLocal();
    counterLocal();
    counterLocal();

    counterStatic();
    counterStatic();
    counterStatic();
    return 0;
}
```

Output:
```
Local count = 1
Local count = 1
Local count = 1
Static count = 1
Static count = 2
Static count = 3
```

`counterLocal`'s `count` is recreated fresh in a new stack frame every call, so it always starts at 0. `counterStatic`'s `count` is stored outside any single frame, so it persists and accumulates across calls.

### Example 4: Recursion — Factorial

```c
#include <stdio.h>

int factorial(int n) {
    if (n == 0) {
        return 1;
    }
    return n * factorial(n - 1);
}

int main(void) {
    printf("factorial(4) = %d\n", factorial(4));
    return 0;
}
```

Output:
```
factorial(4) = 24
```

Trace, following the stack-frame model — each call waits on the next before it can finish:

| Call | Waiting On | Resolves To |
|---|---|---|
| `factorial(4)` | `4 * factorial(3)` | `4 * 6 = 24` |
| `factorial(3)` | `3 * factorial(2)` | `3 * 2 = 6` |
| `factorial(2)` | `2 * factorial(1)` | `2 * 1 = 2` |
| `factorial(1)` | `1 * factorial(0)` | `1 * 1 = 1` |
| `factorial(0)` | base case | `1` |

Each row is a separate stack frame with its own independent `n`; `factorial(0)`'s base case is what stops the chain and lets every waiting call resolve in reverse order.

### Example 5: A Predicate Function Used Directly in Control Flow

```c
#include <stdio.h>

int isPrime(int n) {
    if (n < 2) {
        return 0;
    }
    for (int i = 2; i * i <= n; i++) {
        if (n % i == 0) {
            return 0;
        }
    }
    return 1;
}

int main(void) {
    for (int i = 2; i <= 20; i++) {
        if (isPrime(i)) {
            printf("%d ", i);
        }
    }
    printf("\n");
    return 0;
}
```

Output:
```
2 3 5 7 11 13 17 19
```

`isPrime` returns a true/false-like result (`1` or `0`) that is used directly as the condition of an `if`, a common and idiomatic use of a function's return value.

## 7. Common Patterns / Idioms

| Pattern | Shape | Typical Use |
|---|---|---|
| Helper / computation function | Takes input values, returns one computed result | Formulas, conversions, calculations reused across a program |
| Predicate / validation function | Returns a true/false-like value (`1`/`0`), used directly in an `if` or loop condition | Checking a property (`isPrime`, `isValid`, `isEven`) |
| Recursive function | Calls itself with a smaller version of the problem, with a base case that stops it | Problems naturally defined in terms of smaller versions of themselves (factorial, Fibonacci) |
| Processing many inputs (preview) | A function is called once per item in a collection | Becomes concrete once Arrays is covered — the same function you write here can later run once per array element |

## 8. Common Mistakes and How to Debug Them

| Mistake | Symptom | Why It Happens | How to Catch It |
|---|---|---|---|
| Missing or mismatched prototype | Compiler warning ("implicit declaration"), or wrong behavior when types don't match | The compiler needs a function's signature before its first use if the definition appears later in the file | Declare a prototype above `main`, or define the function before it's called; compile with `-Wall` |
| Forgetting `return` in a non-void function | Function appears to run, but produces garbage or unpredictable values | Falling off the end of a non-void function without returning is undefined behavior | Ensure every code path in a non-void function reaches a `return`; compile with `-Wall` |
| Returning the address of a local variable | Compiles fine, but the returned address is unreliable or crashes when used | The local variable's stack frame is destroyed the moment the function returns — the address it held is no longer valid | Never return `&localVariable`; return the value itself (dynamically allocated memory, covered with pointers, is the correct fix when an address must be returned) |
| Infinite recursion (missing or unreachable base case) | Program crashes with a stack overflow after running for a while | Each recursive call creates a new stack frame; without a base case that is actually reached, frames pile up until memory is exhausted | Verify the base case is reachable from every recursive call, and trace a small example by hand before running |
| Assuming a function can modify the caller's variable directly | Caller's variable stays unchanged even though the function appears to update it | Arguments are passed by value — the function receives a copy | To modify a caller's variable, pass its address using a pointer (covered next) |

```c
int *getValue(void) {
    int local = 42;
    return &local;   /* local's memory no longer belongs to any active call once this returns */
}
```

Calling `*getValue()` afterward reads memory that has already been reclaimed — this is the same dangling-memory idea covered formally as a dangling pointer in the Pointers lecture.

## 9. When to Extract a Function vs. Keep Logic Inline

| Extract into a function when... | Keep inline when... |
|---|---|
| The same logic is needed more than once | The logic is a single line used exactly once |
| The task can be described in one clear sentence ("computes the area of a circle") | Splitting it out would fragment a simple, linear flow into confusing pieces |
| The logic is complex enough that naming it improves readability | The function's name would just restate what the one line already says |
| You want to test or reason about the logic independently of where it's used | The logic is too tightly tied to its one calling context to make sense on its own |

## 10. Connection to the Bigger Picture

A function is the first unit of **abstraction** and **modularity** in programming: a name that hides a process behind a clean interface of inputs and an output. This same idea scales directly upward — a module is a related group of functions, a library is a packaged set of modules, and an API is, at its core, a documented set of function signatures that other programmers call without needing to know how they're implemented inside. Thinking of a function as a **contract** — "given valid inputs, I promise this specific output" — is the same mental habit used to reason about entire software systems, not just single functions.

Two limitations named in this document point directly at the next lectures in this course. A function can only return one value directly (Section 4/9); pointers resolve this by letting a function write results directly into memory the caller provides. Pass-by-value means a function cannot modify a caller's variable directly (Section 5/8); pointers resolve this the same way. Arrays, covered separately, extend this same function model to let one function process many values of the same type at once.

## 11. Practice Questions

### Question 1 (Trace / Predict Output)

What does the following program print?

```c
#include <stdio.h>

int mystery(int n) {
    if (n <= 1) {
        return n;
    }
    return mystery(n - 1) + mystery(n - 2);
}

int main(void) {
    printf("%d\n", mystery(5));
    return 0;
}
```

**Solution.** Trace from the base cases upward:

| Call | Value |
|---|---|
| `mystery(0)` | 0 |
| `mystery(1)` | 1 |
| `mystery(2)` | `mystery(1) + mystery(0) = 1 + 0 = 1` |
| `mystery(3)` | `mystery(2) + mystery(1) = 1 + 1 = 2` |
| `mystery(4)` | `mystery(3) + mystery(2) = 2 + 1 = 3` |
| `mystery(5)` | `mystery(4) + mystery(3) = 3 + 2 = 5` |

Output: `5`

This is the Fibonacci sequence, expressed directly as a recursive definition.

### Question 2 (Write a Program)

Write a function `int gcd(int a, int b)` that computes the greatest common divisor of two integers using recursion (Euclid's algorithm: `gcd(a, 0) = a`; otherwise `gcd(a, b) = gcd(b, a % b)`). Call it from `main` with two hardcoded values and print the result.

**Solution.**

```c
#include <stdio.h>

int gcd(int a, int b) {
    if (b == 0) {
        return a;
    }
    return gcd(b, a % b);
}

int main(void) {
    printf("gcd(48, 18) = %d\n", gcd(48, 18));
    return 0;
}
```

Output:
```
gcd(48, 18) = 6
```

Trace: `gcd(48,18) → gcd(18,12) → gcd(12,6) → gcd(6,0) = 6`.

### Question 3 (Debug)

The following program is meant to count down from 5 to 0, but never stops. Find and fix the bug.

```c
#include <stdio.h>

int countDown(int n) {
    printf("%d\n", n);
    return countDown(n - 1);
}

int main(void) {
    countDown(5);
    return 0;
}
```

**Solution.** There is no base case — `countDown` calls itself with a smaller `n` forever, running through 0, -1, -2, and so on, until the program crashes with a stack overflow (Section 8, row 4). Fix by adding a base case that is actually reached:

```c
int countDown(int n) {
    if (n < 0) {
        return 0;
    }
    printf("%d\n", n);
    return countDown(n - 1);
}
```

Output:
```
5
4
3
2
1
0
```

### Question 4 (Combine Functions, Loops, and Recursion Concepts)

Write a function `int countDigits(int n)` that returns how many digits a non-negative integer has, without converting it to a string (hint: repeatedly divide by 10). Call it on five hardcoded numbers from `main` and print each number alongside its digit count.

**Solution.**

```c
#include <stdio.h>

int countDigits(int n) {
    int count = 0;
    if (n == 0) {
        return 1;
    }
    while (n != 0) {
        n = n / 10;
        count++;
    }
    return count;
}

int main(void) {
    printf("%d has %d digit(s)\n", 7, countDigits(7));
    printf("%d has %d digit(s)\n", 42, countDigits(42));
    printf("%d has %d digit(s)\n", 356, countDigits(356));
    printf("%d has %d digit(s)\n", 4527, countDigits(4527));
    printf("%d has %d digit(s)\n", 100000, countDigits(100000));
    return 0;
}
```

Output:
```
7 has 1 digit(s)
42 has 2 digit(s)
356 has 3 digit(s)
4527 has 4 digit(s)
100000 has 6 digit(s)
```

### Question 5 (Design / Judgement — No Single Code Answer)

A program needs to compute the average, maximum, and minimum of a list of numbers entered by the user. A colleague suggests writing one large function that does all three directly, arguing "it's just one task, so it doesn't need to be split up." Using the criteria from Section 9, evaluate this suggestion.

**Discussion.** "Compute the average," "find the maximum," and "find the minimum" are each independently describable in one clear sentence — exactly the kind of task Section 9 flags as a good candidate for its own function, even though all three happen to be used together in this program. Merging them into one large function mixes three separate responsibilities: a bug or change in the maximum-finding logic risks touching code that has nothing to do with it, and none of the three pieces can be reused, tested, or reasoned about on its own. The counterargument — that each piece is a short loop and might feel trivial to inline — is weaker here than it looks, because these are not one-line, single-use fragments; each is a small but complete idea in its own right. The better design is three small functions (`computeAverage`, `findMax`, `findMin`) called from `main`, giving the program three clearly named steps instead of one undifferentiated block.

## 12. Summary Table

| Aspect | Detail |
|---|---|
| Signature | `returnType functionName(paramType1 param1, ...)` |
| No return value | Use `void` as the return type |
| No parameters | Use `void` inside the parentheses: `int f(void)` |
| Parameter passing | Pass-by-value — the function receives copies of its arguments |
| Call stack | Every call gets a fresh "stack frame" for its parameters and locals; destroyed when the function returns |
| Local variable lifetime | Exists only for the duration of one call, unless declared `static` |
| `return` | Ends the function immediately and sends exactly one value back to the caller |
| Recursion | A function calling itself; requires a base case that is actually reachable |
| Most important rule | Each call is completely independent — nothing about one call's parameters or locals persists into the next, unless explicitly made `static` or global |
