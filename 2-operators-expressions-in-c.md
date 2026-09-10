# Operators & Expressions

*This lecture assumes Variables, Data Types & Constants has been covered. It does not assume Conditionals or Loops — no `if`/`else`, `switch`, `while`, or `for` appears in any example. Relational and logical operators are demonstrated by printing their `0`/`1` results directly, not by branching on them.*

## 1. Learning Objectives

By the end of this document, you should be able to:

- Explain the difference between storing a value (Variables) and computing a new one from existing values (Operators).
- Correctly use arithmetic, assignment (including compound forms), and increment/decrement operators, and state the difference between prefix and postfix.
- Predict the result of a multi-operator expression using precedence and associativity rules.
- Explain what relational and logical operators actually produce (`0` or `1`), not just what they informally "mean."
- Explain short-circuit evaluation and use it to avoid an unsafe operation.
- Identify the difference between `=` and `==`, and recognize an expression that modifies the same variable unsafely more than once.

## 2. Why This Lecture Matters

Variables let you store a fixed value and change it later. On their own, they don't let you ask new questions of that data or combine several values into one new result — the operators to do that are the subject of this lecture. Some of them (arithmetic) you've already glimpsed in passing; this lecture makes them precise and complete, and introduces two entirely new categories your programs cannot do without: **comparing** values and **combining logical answers**.

Consider updating a running total using only what Variables covered:

```c
#include <stdio.h>

int main(void) {
    int count = 0;

    count = count + 1;
    count = count + 1;
    count = count + 1;

    printf("count = %d\n", count);
    return 0;
}
```

This works, but `count = count + 1` is a verbose way to express a simple idea — "increase `count` by one." This lecture introduces shorthand for exactly this:

```c
#include <stdio.h>

int main(void) {
    int count = 0;

    count++;
    count++;
    count++;

    printf("count = %d\n", count);
    return 0;
}
```

Both print `count = 3`. Beyond shorthand, this lecture introduces relational operators (asking "is A greater than B?") and logical operators (asking "are both of these true?") — questions arithmetic alone cannot answer, and questions the next lecture, Conditionals, will depend on directly.

## 3. Mental Model

Every operator is a small, value-producing machine: it takes one or two operand values, applies a fixed rule, and produces exactly one resulting value. This means an entire expression — no matter how many operators it contains — is itself just a value, no different in kind from a plain literal.

| Expression | The "Machine" | Result |
|---|---|---|
| `5 + 3` | Addition | `8` |
| `5 > 3` | "Is the left greater?" | `1` |
| `!0` | "Flip true/false" | `1` |

This is the one idea behind everything in this document: arithmetic, comparison, and logic are not three different kinds of thing — they are all operators producing values, differing only in what kind of question or transformation each one performs.

## 4. Syntax and Core Mechanics

### Arithmetic Operators

| Operator | Meaning | Example | Result |
|---|---|---|---|
| `+` | Addition | `5 + 3` | `8` |
| `-` | Subtraction | `5 - 3` | `2` |
| `*` | Multiplication | `5 * 3` | `15` |
| `/` | Division | `5 / 3` | `1` (integer division truncates) |
| `%` | Modulo (remainder) | `5 % 3` | `2` — valid only when both operands are integers |

### Assignment and Compound Assignment

| Operator | Equivalent To | Example |
|---|---|---|
| `=` | — | `x = 5;` |
| `+=` | `x = x + value` | `x += 3;` |
| `-=` | `x = x - value` | `x -= 3;` |
| `*=` | `x = x * value` | `x *= 3;` |
| `/=` | `x = x / value` | `x /= 3;` |
| `%=` | `x = x % value` | `x %= 3;` |

### Increment and Decrement

| Form | Name | Behavior |
|---|---|---|
| `++x` | Prefix increment | Increments `x` first; the expression's value is the new, already-incremented value |
| `x++` | Postfix increment | The expression's value is the current value; `x` is incremented afterward |
| `--x` | Prefix decrement | Decrements `x` first; the expression's value is the new value |
| `x--` | Postfix decrement | The expression's value is the current value; `x` is decremented afterward |

### Relational Operators

| Operator | Meaning | Example | Result |
|---|---|---|---|
| `<` | Less than | `3 < 5` | `1` |
| `>` | Greater than | `3 > 5` | `0` |
| `<=` | Less than or equal | `5 <= 5` | `1` |
| `>=` | Greater than or equal | `3 >= 5` | `0` |
| `==` | Equal to | `5 == 5` | `1` |
| `!=` | Not equal to | `5 != 3` | `1` |

### Logical Operators

| Operator | Meaning | Example | Result |
|---|---|---|---|
| `&&` | AND — true only if both operands are true (non-zero) | `1 && 0` | `0` |
| `\|\|` | OR — true if at least one operand is true | `1 \|\| 0` | `1` |
| `!` | NOT — flips true to false and vice versa | `!0` | `1` |

### `sizeof`

`sizeof` (introduced in the Variables lecture) is technically an **operator**, not a function — this is why `sizeof(int)` and `sizeof x` are both valid, unlike an actual function call, which always requires parentheses.

### Bitwise Operators (Brief)

| Operator | Meaning |
|---|---|
| `&` | Bitwise AND |
| `\|` | Bitwise OR |
| `^` | Bitwise XOR |
| `~` | Bitwise NOT (complement) |
| `<<` | Left shift |
| `>>` | Right shift |

These act on the individual bits of a value rather than its numeric meaning. They are covered briefly here, as they are less commonly needed for everyday application code, but worth recognizing (Example 6).

## 5. How It Works Underneath

### Precedence and Associativity

When an expression mixes several operators, **precedence** decides which one is applied first, and **associativity** decides the order when two operators of the same precedence appear together.

| Precedence (High → Low) | Operators | Associativity |
|---|---|---|
| 1 (highest) | Unary `! ++ --`, unary `+ -` | Right to left |
| 2 | `* / %` | Left to right |
| 3 | Binary `+ -` | Left to right |
| 4 | `< > <= >=` | Left to right |
| 5 | `== !=` | Left to right |
| 6 | `&&` | Left to right |
| 7 | `\|\|` | Left to right |
| 8 (lowest) | `= += -= *= /= %=` | Right to left |

Operators higher in this table are applied before operators lower in the table, regardless of the order they're written in. For `2 + 3 * 4`, `*` (precedence 2) is applied before `+` (precedence 3), giving `2 + 12 = 14`, not `(2 + 3) * 4 = 20` (Example 2).

### Short-Circuit Evaluation

`&&` evaluates its left operand first. If that is `0` (false), the entire expression must be `0`, so the right operand is **never evaluated at all**. `||` evaluates its left operand first; if that is non-zero (true), the entire expression must be `1`, so the right operand is never evaluated. This is not merely an optimization — it is guaranteed by the language, which makes it usable as a genuine safety mechanism (Example 4).

### Implicit Conversions in Expressions

The integer-division rule from the Variables lecture is one case of a general rule: when an expression combines operands of different types, C converts the "narrower" type to match the "wider" one before applying the operator — combining an `int` with a `double`, for instance, promotes the `int` to `double` first.

## 6. Worked Examples

### Example 1: Compound Assignment and Prefix vs. Postfix

```c
#include <stdio.h>

int main(void) {
    int count = 0;

    count = count + 1;
    count = count + 1;
    count += 1;
    count++;

    printf("count = %d\n", count);

    int x = 5;
    printf("x++ gives %d\n", x++);
    printf("x is now %d\n", x);

    int y = 5;
    printf("++y gives %d\n", ++y);
    printf("y is now %d\n", y);

    return 0;
}
```

Output:
```
count = 4
x++ gives 5
x is now 6
++y gives 6
y is now 6
```

`x++` reports the value *before* incrementing; `++y` increments *before* reporting. Each variable here is modified by exactly one operator per statement — the safe way to use increment/decrement, formalized in Section 8.

### Example 2: Precedence Resolves Ambiguity

```c
#include <stdio.h>

int main(void) {
    int result1 = 2 + 3 * 4;
    int result2 = (2 + 3) * 4;

    printf("2 + 3 * 4 = %d\n", result1);
    printf("(2 + 3) * 4 = %d\n", result2);
    return 0;
}
```

Output:
```
2 + 3 * 4 = 14
(2 + 3) * 4 = 20
```

Without parentheses, `*` always binds tighter than `+`. Parentheses override the default precedence whenever the default isn't what you mean.

### Example 3: Relational Operators Are Ordinary Values

```c
#include <stdio.h>

int main(void) {
    int a = 7, b = 10;

    printf("a < b is %d\n", a < b);
    printf("a > b is %d\n", a > b);
    printf("a == b is %d\n", a == b);
    printf("a != b is %d\n", a != b);
    return 0;
}
```

Output:
```
a < b is 1
a > b is 0
a == b is 0
a != b is 1
```

Nothing here required a branch — each comparison is printed directly, because it is simply an `int` value like any other.

### Example 4: Short-Circuit Evaluation as a Safety Mechanism

```c
#include <stdio.h>

int main(void) {
    int a = 10, b = 0;

    int safeCheck = (b != 0) && (a / b > 2);

    printf("safeCheck = %d\n", safeCheck);
    return 0;
}
```

Output:
```
safeCheck = 0
```

`b` is `0`, so `(b != 0)` is false. Because `&&` short-circuits, `(a / b > 2)` is **never evaluated** — the program runs safely specifically *because* the right-hand side was skipped. Had `&&` not short-circuited, `a / b` would attempt to divide by zero, which is undefined behavior and would very likely crash the program.

### Example 5: `=` vs. `==`

```c
#include <stdio.h>

int main(void) {
    int x = 5, y = 8;

    int assignResult = (x = y);
    printf("assignResult = %d, x is now %d\n", assignResult, x);

    int compareResult = (x == 3);
    printf("compareResult = %d, x is still %d\n", compareResult, x);

    return 0;
}
```

Output:
```
assignResult = 8, x is now 8
compareResult = 0, x is still 8
```

`x = y` both **changes** `x` and **produces** the assigned value (`8`) as its own result. `x == 3` changes nothing; it only produces a `0`/`1` answer. This dual nature of `=` — action and value at once — is exactly why C allows writing `=` where `==` was meant, without any compiler error: the expression is still perfectly valid, just not the comparison intended. Once `if` is introduced in the next lecture, `if (x = 5)` will compile and always take the "true" branch, because `5` is non-zero — one of the most common bugs in C, covered fully there.

### Example 6: A Brief Tour of Bitwise Operators

```c
#include <stdio.h>

int main(void) {
    int a = 12;  /* binary: 1100 */
    int b = 10;  /* binary: 1010 */

    printf("a & b = %d\n", a & b);
    printf("a | b = %d\n", a | b);
    printf("a ^ b = %d\n", a ^ b);
    printf("a << 1 = %d\n", a << 1);
    printf("a >> 1 = %d\n", a >> 1);

    return 0;
}
```

Output:
```
a & b = 8
a | b = 14
a ^ b = 6
a << 1 = 24
a >> 1 = 6
```

`1100 & 1010 = 1000` (8), `1100 | 1010 = 1110` (14), `1100 ^ 1010 = 0110` (6); shifting left by 1 doubles the value (24), shifting right by 1 halves it (6).

## 7. Common Patterns / Idioms

| Pattern | Shape | Typical Use |
|---|---|---|
| Accumulator update | `total += value;` | Building up a running sum or product |
| Counter update | `count++;` | Tracking how many times something has happened |
| Precomputed flag | `int isValid = (score >= 0 && score <= 100);` | Computing a reusable `0`/`1` answer once, ahead of when it's needed |
| Short-circuit guard | `(denom != 0) && (value / denom > threshold)` | Preventing an unsafe operation from ever running |
| Bitwise parity check | `n & 1` | Checking whether `n` is odd (`1`) or even (`0`), as an alternative to `n % 2` |
| Toggling a flag | `flag = !flag;` | Flipping a `0`/`1` value to its opposite |

## 8. Common Mistakes and How to Debug Them

| Mistake | Symptom | Why It Happens | How to Catch It |
|---|---|---|---|
| Confusing `=` and `==` | The program compiles and runs, but produces a value that was assigned rather than compared | `=` is valid anywhere a value is expected, including where a comparison was intended (Example 5) | Read every `=` in an expression (not a plain statement) and ask whether a comparison was actually meant |
| Modifying the same variable more than once in one expression | Unpredictable or compiler-dependent results | C does not guarantee an order between two side effects on the same variable within one expression if there's no sequence point between them — this is undefined behavior, not just "confusing" | Never write an expression like `x = x++ + 1;`; give each modification of a variable its own separate statement |
| Misjudging precedence (chaining relational operators) | A comparison like `a < b < c` doesn't check whether `b` is between `a` and `c` | `<` is left-associative, so `a < b < c` evaluates as `(a < b) < c` — a `0`/`1` result compared against `c`, not a three-way range check | Never chain relational operators; write `(a < b) && (b < c)` explicitly |
| Using `%` on non-integer operands | Compiler error | `%` (modulo) is defined only for integer types in C | Use `fmod()` from `<math.h>` for a floating-point remainder, or restructure the calculation to use integers |

```c
int x = 5;
x = x++ + 1;   /* undefined behavior: do not do this, and do not try to guess "the answer" */
```

This is deliberately shown without a resolved output — different compilers may produce different results, and the correct lesson is to avoid this shape of expression entirely, not to memorize what one particular compiler happens to do with it.

## 9. When to Use Which Operator

| Situation | Best Fit |
|---|---|
| Asking a yes/no question about a value | Relational (`< > <= >= == !=`) |
| Combining two or more yes/no answers | Logical (`&& || !`) |
| Updating a variable based on its own current value | Compound assignment (`+= -= *= /= %=`) |
| Counting, or stepping by exactly one | Increment/decrement (`++ --`) |
| Manipulating individual bits for a specific, low-level reason (flags, performance-critical code) | Bitwise (`& \| ^ ~ << >>`) — otherwise, prefer the more readable arithmetic or logical equivalent |

## 10. Connection to the Bigger Picture

If Variables supplied the nouns of the language — named, typed places to keep values — this lecture supplies the verbs: the operations that combine, compare, and transform them into new values. Precedence and associativity are your first encounter with a language having a fully specified, unambiguous grammar — the same idea that makes it possible to build a compiler at all, since "what does this text mean" can never be left to guesswork. Short-circuit evaluation is your first encounter with a related idea: evaluation order is part of a language's actual meaning, not just an implementation detail — a theme that returns with real consequences once safely checking a pointer before dereferencing it matters, much later in this course.

Most directly, the `0`/`1` values produced by relational and logical operators in this lecture are exactly what the next lecture, Conditionals, tests to decide what a program does next. Operators produce the yes/no answers; conditionals act on them.

## 11. Practice Questions

### Question 1 (Trace / Predict Output)

What does the following program print?

```c
#include <stdio.h>

int main(void) {
    int a = 5, b = 2, c = 3;
    int result = a + b * c - a / b;

    printf("result = %d\n", result);
    return 0;
}
```

**Solution.** Apply precedence: `*` and `/` before `+` and `-`.

- `b * c = 2 * 3 = 6`
- `a / b = 5 / 2 = 2` (integer division)
- `a + (b * c) - (a / b) = 5 + 6 - 2 = 9`

Output: `result = 9`

### Question 2 (Write a Program)

Write a program that stores a student's marks in three subjects, computes the total using compound assignment, and prints the total along with the percentage (out of 300) as a decimal.

**Solution.**

```c
#include <stdio.h>

int main(void) {
    int marks1 = 78, marks2 = 85, marks3 = 91;
    int total = 0;

    total += marks1;
    total += marks2;
    total += marks3;

    double percentage = (total / 300.0) * 100;

    printf("Total = %d\n", total);
    printf("Percentage = %.2f\n", percentage);
    return 0;
}
```

Output:
```
Total = 254
Percentage = 84.67
```

### Question 3 (Debug)

A student wants to check whether `b` lies strictly between `a` and `c`, and writes:

```c
#include <stdio.h>

int main(void) {
    int a = 1, b = 10, c = 5;
    int between = a < b < c;

    printf("between = %d\n", between);
    return 0;
}
```

It reports `between = 1` even though `b = 10` is clearly not less than `c = 5`. Explain the bug and fix it.

**Solution.** `<` is left-associative, so `a < b < c` is evaluated as `(a < b) < c`, not as a three-way range check. Here, `(1 < 10)` is `1`, and then `1 < 5` is also `1` — so the expression reports "true" purely by coincidence of comparing a `0`/`1` result to `c`, not by actually checking whether `b` is between `a` and `c` (Section 8).

Fix by expressing the intended comparison explicitly with `&&`:

```c
int between = (a < b) && (b < c);
```

With the same values, `(1 < 10) && (10 < 5)` is `1 && 0`, giving `between = 0` — the correct answer.

### Question 4 (Combine Arithmetic, Relational, and Logical Operators)

Write a program that computes, without using any conditional statement, whether a given year is a leap year: divisible by 4, and (not divisible by 100, or divisible by 400).

**Solution.**

```c
#include <stdio.h>

int main(void) {
    int year = 2024;
    int isLeapYear = (year % 4 == 0) && ((year % 100 != 0) || (year % 400 == 0));

    printf("isLeapYear = %d\n", isLeapYear);
    return 0;
}
```

Output:
```
isLeapYear = 1
```

**Reasoning.** The entire leap-year rule is a single expression combining `%`, `==`, `!=`, `&&`, and `||` — no branching is needed to compute the yes/no answer itself, only (eventually, once Conditionals is covered) to act differently based on it.

### Question 5 (Design / Judgement — No Single Code Answer)

A colleague suggests writing `n & 1` instead of `n % 2` to check whether `n` is even, arguing it's "more efficient." Using the ideas from this document, discuss when this substitution is reasonable and when it isn't.

**Discussion.** For a non-negative integer, `n & 1` and `n % 2` produce the same result, since both isolate whether the lowest bit is set. In practice, modern compilers already perform this exact optimization automatically when compiling with optimizations enabled, so hand-writing the bitwise version rarely produces a measurable performance difference today. What it reliably does cost is readability: `n % 2 == 0` states its intent directly ("check the remainder"), while `n & 1` requires the reader to already know it's a parity check dressed up in binary. It's also worth flagging, without going into the details here, that `%` and `&` are not guaranteed to behave identically once negative numbers are involved — a subtlety this document hasn't covered. The general recommendation: prefer the more readable form unless profiling has shown this specific operation to be a genuine bottleneck, which is very rare for a single parity check — a first small taste of a broader engineering caution against optimizing before there's evidence it's needed.

## 12. Summary Table

| Aspect | Detail |
|---|---|
| Arithmetic | `+ - * / %` — `%` valid only for integer operands |
| Assignment | `=` assigns and also produces the assigned value; compound forms `+= -= *= /= %=` combine an operation with assignment |
| Increment/decrement | `++`/`--`; prefix (`++x`) changes then returns the new value, postfix (`x++`) returns the old value then changes |
| Relational | `< > <= >= == !=` — each produces `1` (true) or `0` (false) |
| Logical | `&& \|\| !` — combine `0`/`1` values; `&&` and `\|\|` short-circuit, skipping the right operand when the left already determines the result |
| Precedence (high → low, as covered here) | unary (`! ++ --`) → `* / %` → `+ -` → relational → equality → `&&` → `\|\|` → `=` |
| Most important rule | Every operator produces a value — an expression is never "just an action," it always evaluates to something that can be stored, printed, or used inside a larger expression |
