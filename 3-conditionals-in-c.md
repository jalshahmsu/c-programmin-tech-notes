# Conditionals in C

*This lecture assumes Variables, Data Types & Constants and Operators & Expressions have been covered. It does not assume Loops — no `while`, `for`, `do-while`, or loop-controlling `break`/`continue` appears in any example. `switch`'s own `break` is a different, required mechanism and is covered here on its own terms.*

## 1. Learning Objectives

By the end of this document, you should be able to:

- Explain how a conditional differs from the `0`/`1` values Operators taught you to compute — the ability to make a program's actual behavior depend on that value.
- Correctly write `if`, `if-else`, and `else if` chains, and trace which single branch of a chain runs for a given input.
- Identify and fix a dangling-else ambiguity using explicit braces.
- Use `switch` correctly, including `break` and `default`, and explain what fall-through does and why it's sometimes deliberate.
- Recognize the `=`/`==` bug inside a condition and explain precisely why it compiles without error.
- Decide, for a given branching problem, whether `if`/`else if`, `switch`, or the ternary operator is the right tool.

## 2. Why Conditionals Exist

The Operators lecture showed how to compute a `0`/`1` answer to a question:

```c
#include <stdio.h>

int main(void) {
    int marks = 32;
    int isPassing = (marks >= 40);

    printf("isPassing = %d\n", isPassing);
    return 0;
}
```

This tells you the answer, but the program's behavior is identical no matter what that answer is — it always runs the exact same `printf`, regardless of whether `isPassing` is `0` or `1`. Nothing about what actually happens changes based on the value computed.

A conditional lets that `0`/`1` value decide what runs at all:

```c
#include <stdio.h>

int main(void) {
    int marks = 32;

    if (marks >= 40) {
        printf("Result: Pass\n");
    } else {
        printf("Result: Fail\n");
    }

    return 0;
}
```

Output:
```
Result: Fail
```

The program now genuinely does something different depending on the value — this is the piece Operators alone could never provide.

## 3. Mental Model

Picture program execution as a single path, moving forward one statement at a time, until it reaches a **fork**. At the fork, a condition — an ordinary `0`/`1` value, produced exactly as in the Operators lecture — is checked. Execution follows exactly one of the paths leading out of the fork, then continues forward from wherever that path rejoins the main flow.

| Situation | What Happens |
|---|---|
| Condition is true | Execution follows the "true" path, then continues after the fork |
| Condition is false, no `else` | Execution skips the "true" path entirely, and continues after the fork |
| Condition is false, with `else` | Execution follows the "false" path instead, then continues after the fork |

This differs sharply from what the next lecture, Loops, does with this same fork: there, the "true" path loops back to recheck the condition instead of continuing onward. Everything in this document concerns a fork checked exactly once and then left behind.

## 4. Syntax and Core Mechanics

| Form | Example | Meaning |
|---|---|---|
| `if` | `if (condition) { ... }` | Runs the block only if `condition` is non-zero; otherwise skips it entirely |
| `if-else` | `if (condition) { ... } else { ... }` | Runs exactly one of the two blocks |
| `else if` chain | `if (c1) {...} else if (c2) {...} else {...}` | Checks conditions in order; runs the block for the first one that's true, and every condition after it is skipped |
| Nested `if` | An `if`/`if-else` written inside another's block | Each level's branch is independent; the final output depends on the full path taken through every level |

`switch` syntax:

```c
switch (expression) {
    case constant1:
        /* statements */
        break;
    case constant2:
        /* statements */
        break;
    default:
        /* statements */
}
```

`expression` must be an integer or character type; each `case` label must be a **constant**, not a variable; `default` is optional and runs if no `case` matches.

Ternary syntax:

```c
condition ? valueIfTrue : valueIfFalse
```

This is an **expression**, not a statement — it can be assigned, printed, or used anywhere an ordinary value is expected.

## 5. How It Works Underneath

**The dangling-else rule.** An `else` always binds to the nearest, innermost `if` that doesn't already have an `else`. This rule is purely structural — indentation is not part of the C language and has no effect on it whatsoever. When more than one `if` is nested without braces, the code can be indented in a way that visually suggests a binding that isn't what the compiler actually does (Example 5).

**`switch` mechanics.** `switch` evaluates `expression` once, then jumps directly to the matching `case` label (or `default`, or past the entire `switch` if nothing matches and there's no `default`). From that point, execution proceeds normally, statement by statement, through every subsequent line inside the `switch`'s braces, until it hits a `break` or reaches the closing brace. This is precisely why omitting `break` causes fall-through: nothing else stops execution from continuing straight into the next case's code (Example 6).

**Ternary vs. `if`/`else`.** `if`/`else` are *statements* — they control which statements run, and produce no value of their own. `?:` is an *expression* — like `+` or `>` from the Operators lecture, it takes operands (here, three: a condition and two possible results) and produces exactly one value. This is why `?:` can appear inside a larger expression, such as an argument to `printf`, while a full `if-else` cannot.

## 6. Worked Examples

### Example 1: A Basic `if`

```c
#include <stdio.h>

int main(void) {
    int marks = 75;

    if (marks >= 40) {
        printf("Result: Pass\n");
    }

    printf("Done checking.\n");
    return 0;
}
```

Output:
```
Result: Pass
Done checking.
```

With `marks = 30` instead, the `if` block is skipped entirely, and the output would be only `Done checking.` — nothing marks where the skipped line would have been; execution simply continues past it.

### Example 2: `if-else`

```c
#include <stdio.h>

int main(void) {
    int marks = 32;

    if (marks >= 40) {
        printf("Result: Pass\n");
    } else {
        printf("Result: Fail\n");
    }

    return 0;
}
```

Output:
```
Result: Fail
```

### Example 3: An `else if` Chain

```c
#include <stdio.h>

int main(void) {
    int marks = 72;

    if (marks >= 90) {
        printf("Grade: A\n");
    } else if (marks >= 75) {
        printf("Grade: B\n");
    } else if (marks >= 60) {
        printf("Grade: C\n");
    } else {
        printf("Grade: F\n");
    }

    return 0;
}
```

Output:
```
Grade: C
```

Trace: `marks >= 90` is false, `marks >= 75` is false, `marks >= 60` is true — the chain stops there. The final `else` is never reached, and no condition after a matching one is ever evaluated.

### Example 4: Nested `if`

```c
#include <stdio.h>

int main(void) {
    int age = 20;
    int hasLicense = 0;

    if (age >= 18) {
        if (hasLicense) {
            printf("Allowed to drive.\n");
        } else {
            printf("Old enough, but a license is required.\n");
        }
    } else {
        printf("Not old enough to drive.\n");
    }

    return 0;
}
```

Output:
```
Old enough, but a license is required.
```

Reaching the correct output required tracing through both levels: the outer `if` (true, since `age >= 18`) determines *which inner statement runs at all*, and the inner `if-else` (false, since `hasLicense` is `0`) determines which specific message prints.

### Example 5: The Dangling-Else Trap

```c
#include <stdio.h>

int main(void) {
    int x = -5, y = 15;

    if (x > 0)
        if (y > 10)
            printf("Both conditions true\n");
        else
            printf("This looks like it belongs to the outer if...\n");

    printf("Done.\n");
    return 0;
}
```

Output:
```
Done.
```

Nothing else prints — even though the `else` is indented to visually align with the outer `if (x > 0)`, it actually binds to the inner `if (y > 10)` (Section 5's rule). Since `x > 0` is false, the *entire* nested `if`/`else` is skipped, `else` included, because it all lives inside the outer `if`'s body.

If the intent really was "print this message whenever `x` is not greater than 0," braces fix it by structurally separating the inner `if` from the outer `if-else`:

```c
if (x > 0) {
    if (y > 10) {
        printf("Both conditions true\n");
    }
} else {
    printf("This looks like it belongs to the outer if...\n");
}
```

With the same values, this version prints `This looks like it belongs to the outer if...`, because the `else` now unambiguously belongs to the outer `if`.

### Example 6: `switch`, With and Without `break`

```c
#include <stdio.h>

int main(void) {
    int day = 2;

    printf("With break after every case:\n");
    switch (day) {
        case 1:
            printf("Monday\n");
            break;
        case 2:
            printf("Tuesday\n");
            break;
        case 3:
            printf("Wednesday\n");
            break;
    }

    printf("Without break after case 2:\n");
    switch (day) {
        case 1:
            printf("Monday\n");
        case 2:
            printf("Tuesday\n");
        case 3:
            printf("Wednesday\n");
            break;
    }

    return 0;
}
```

Output:
```
With break after every case:
Tuesday
Without break after case 2:
Tuesday
Wednesday
```

In the second `switch`, execution jumps straight to `case 2`, prints `Tuesday`, and then — with no `break` to stop it — falls straight into `case 3`'s code and prints `Wednesday` too, even though `day` was never `3`.

### Example 7: The `=` vs. `==` Bug, Realized

```c
#include <stdio.h>

int main(void) {
    int x = 10;

    if (x = 5) {
        printf("Branch taken: x looked like it equaled something, but was actually just assigned.\n");
    } else {
        printf("This branch can never run when assigning a non-zero constant.\n");
    }

    printf("x is now %d\n", x);
    return 0;
}
```

Output:
```
Branch taken: x looked like it equaled something, but was actually just assigned.
x is now 5
```

`x = 5` is a valid expression: it assigns `5` to `x` and evaluates to `5`, which `if` treats as true because it's non-zero — regardless of what `x` held before. Two things went wrong silently: `x` was overwritten, and the `if` branch runs *every time*, no matter what value `x` started with, making the `else` branch dead code. This is exactly the bug the Operators lecture warned would arise once `if` existed. The fix:

```c
if (x == 5) {
    printf("x actually equals 5.\n");
} else {
    printf("x does not equal 5.\n");
}
```

With `x = 10`, this correctly prints `x does not equal 5.`, and `x` remains `10`.

### Example 8: The Ternary Operator

```c
#include <stdio.h>

int main(void) {
    int a = 7, b = 12;
    int max = (a > b) ? a : b;

    printf("max = %d\n", max);
    printf("%s\n", (a % 2 == 0) ? "a is even" : "a is odd");

    return 0;
}
```

Output:
```
max = 12
a is odd
```

Both uses pick between exactly two values — the larger of two numbers, and one of two strings — with no separate statements needed for either branch.

## 7. Common Patterns / Idioms

| Pattern | Shape | Typical Use |
|---|---|---|
| Input validation | `if (condition) { ... } else { ... }` | Accepting or rejecting a value based on a rule |
| Classification / grading ladder | `if / else if / else if / ... / else`, ordered most to least restrictive | Sorting a value into one of several ranges or categories |
| Menu-driven branching | `switch` on a single integer/character choice | Handling one of several fixed, named options |
| Guarding with a compound condition | `if (a && b)` or `if (a || b)` | Requiring multiple conditions together, or accepting any of several |
| Quick value pick | `int result = condition ? valueIfTrue : valueIfFalse;` | Choosing between exactly two values, with no other statements needed |

## 8. Common Mistakes and How to Debug Them

| Mistake | Symptom | Why It Happens | How to Catch It |
|---|---|---|---|
| Dangling else binds to the wrong `if` | A branch runs (or doesn't) in a way that contradicts what the indentation suggested | `else` always binds to the nearest unmatched `if`, regardless of indentation (Example 5) | Use braces `{ }` around every `if`/`else` body once more than one `if` is nested, even for a single-statement body |
| Missing `break` in `switch` | More than one case's code runs, when only one was intended | Without `break`, execution simply continues into the next case (Example 6) | Add `break` at the end of every case unless fall-through is a deliberate, commented choice |
| Confusing `=` and `==` in a condition | A branch always runs (or never runs), and the "checked" variable has silently changed | `x = value` is a valid expression that assigns and evaluates to the assigned value, which `if` treats as true whenever non-zero (Example 7) | Read every condition and check whether a single `=` was meant to be `==` |
| `switch` on a non-constant or wrong-type case label | Compiler error | `case` labels must be constant expressions of an integer or character type; `switch` cannot test a `float`/`double`, and labels cannot be variables | Use `switch` only for exact matches against a small set of known integer/character constants; use `if`/`else if` for anything else |
| An earlier branch written too broadly | A later, more specific `else if` never runs, even for input it was clearly meant to handle | Once an earlier condition matches, no later condition in the chain is even checked | Order `else if` chains from most specific/restrictive condition to least (Question 4) |

## 9. When to Use `if`/`else if` vs. `switch` vs. Ternary

| Situation | Best Fit |
|---|---|
| A condition involves a comparison, a range, or a combination of multiple conditions | `if` / `else if` |
| Checking one variable/expression against several exact constant values | `switch` |
| The only goal is picking one of exactly two values to use in an expression | Ternary (`?:`) |
| The branches need to run different sequences of statements, not just produce a value | `if`/`else` or `switch` — never ternary |

## 10. Connection to the Bigger Picture

Branching (this lecture) and iteration (Loops, next) are the two fundamental control-flow tools present in essentially every programming language. This lecture's fork — check a condition, follow exactly one path, then continue — is about to be generalized directly: a loop is this same fork, except its "true" path loops back to recheck the condition instead of continuing onward past it. Conditionals are also the first thing in this course that let a program behave differently for different inputs, rather than running the exact same fixed sequence of statements every time — this is what separates a program that can make decisions from one that is just a straight-line script.

## 11. Practice Questions

### Question 1 (Trace / Predict Output)

What does the following program print?

```c
#include <stdio.h>

int main(void) {
    int a = 8, b = 3, c = 5;

    if (a > b)
        if (b > c)
            printf("Path 1\n");
        else
            printf("Path 2\n");
    else
        printf("Path 3\n");

    return 0;
}
```

**Solution.** There are two `if`s and two `else`s. By the dangling-else rule, the first `else` binds to the nearest unmatched `if` — `if (b > c)` — and the second `else` binds to the only `if` left unmatched — `if (a > b)`. So the structure is:

```c
if (a > b) {
    if (b > c) { printf("Path 1\n"); }
    else       { printf("Path 2\n"); }
} else {
    printf("Path 3\n");
}
```

Trace: `a > b` is `8 > 3`, true — enter the outer block. `b > c` is `3 > 5`, false — the inner `else` runs. The outer `else` (`Path 3`) is never reached, since the outer condition was true.

Output: `Path 2`

### Question 2 (Write a Program)

Write a program that checks whether three integers can form a valid triangle (each side must be less than the sum of the other two) and, if valid, classifies it as Equilateral, Isosceles, or Scalene.

**Solution.**

```c
#include <stdio.h>

int main(void) {
    int a = 5, b = 5, c = 8;

    if (a + b > c && b + c > a && a + c > b) {
        if (a == b && b == c) {
            printf("Equilateral triangle\n");
        } else if (a == b || b == c || a == c) {
            printf("Isosceles triangle\n");
        } else {
            printf("Scalene triangle\n");
        }
    } else {
        printf("Not a valid triangle\n");
    }

    return 0;
}
```

Output:
```
Isosceles triangle
```

**Reasoning.** The outer `if` uses a compound condition (three `&&`-joined comparisons, straight from Operators) to validate the triangle first; only once that's confirmed true does the nested `else if` chain classify it — combining nested `if`, `else if`, and compound conditions in one problem.

### Question 3 (Debug)

The following program is meant to print only the message matching `choice`, but instead prints extra lines. Find and fix the bug.

```c
#include <stdio.h>

int main(void) {
    int choice = 2;

    switch (choice) {
        case 1:
            printf("You chose option 1\n");
        case 2:
            printf("You chose option 2\n");
        case 3:
            printf("You chose option 3\n");
    }

    return 0;
}
```

**Solution.** None of the cases have a `break`, so once execution jumps to `case 2`, it falls straight through into `case 3` as well (Section 8, row 2).

Buggy output:
```
You chose option 2
You chose option 3
```

Fix by adding `break` after each case:

```c
switch (choice) {
    case 1:
        printf("You chose option 1\n");
        break;
    case 2:
        printf("You chose option 2\n");
        break;
    case 3:
        printf("You chose option 3\n");
        break;
}
```

Fixed output:
```
You chose option 2
```

### Question 4 (Debug — Branch Ordering)

A grading program is meant to assign `A` for marks ≥ 90, `B` for marks ≥ 75, `C` for marks ≥ 60, and `F` otherwise, but is written as below. Test it with `marks = 95`, explain what goes wrong, and fix it.

```c
#include <stdio.h>

int main(void) {
    int marks = 95;

    if (marks >= 60) {
        printf("Grade: C\n");
    } else if (marks >= 75) {
        printf("Grade: B\n");
    } else if (marks >= 90) {
        printf("Grade: A\n");
    } else {
        printf("Grade: F\n");
    }

    return 0;
}
```

**Solution.** The very first condition, `marks >= 60`, is already true for `marks = 95`, so the chain stops there and prints `Grade: C` — the more specific, more accurate conditions (`>= 75`, `>= 90`) are never even checked, because an earlier, broader condition already matched (Section 8, row 5).

Fix by ordering from most restrictive to least:

```c
if (marks >= 90) {
    printf("Grade: A\n");
} else if (marks >= 75) {
    printf("Grade: B\n");
} else if (marks >= 60) {
    printf("Grade: C\n");
} else {
    printf("Grade: F\n");
}
```

With the same `marks = 95`, this now correctly prints `Grade: A`.

### Question 5 (Design / Judgement — No Single Code Answer)

A programmer wants to print `"Even"` or `"Odd"` depending on a number's parity, and is deciding between a full `if-else` and a ternary expression inside `printf`. Using the criteria from Section 9, which is more appropriate here — and describe a scenario involving the same parity check where the other approach would clearly be the better choice.

**Discussion.** Printing `"Even"` or `"Odd"` is exactly the case Section 9 identifies for a ternary: the only goal is picking one of two values (here, two strings) to hand directly to `printf`. `printf("%s\n", (n % 2 == 0) ? "Even" : "Odd");` reads naturally as "pick one of two strings," while a full `if-else` would need two separate, near-identical `printf` statements to express the same single decision.

The other approach becomes clearly better the moment the two outcomes need to do more than produce one value — for example, if being even should also increment a running count of even numbers, and being odd should increment a *different* count. A ternary can only produce a single value; it cannot run two separate statements per branch. At that point the branches have diverged from "just pick a value" into "run different behavior," and `if-else` is the only appropriate tool.

## 12. Summary Table

| Aspect | Detail |
|---|---|
| `if` | `if (condition) { ... }` — condition is any expression; non-zero means true |
| `if-else` | `if (condition) { ... } else { ... }` — exactly one branch runs |
| `else if` chain | Sequential forks; the first matching branch runs, and every condition after it is skipped |
| Dangling else | `else` always binds to the nearest unmatched `if` — use braces to make nesting unambiguous |
| `switch` | `switch (expr) { case value: ...; break; default: ...; }` — tests equality against constants only |
| `switch` fall-through | Without `break`, execution continues into the next case's code |
| `?:` (ternary) | `condition ? valueIfTrue : valueIfFalse` — a conditional *expression*, produces one value, not a statement |
| Most important rule | Exactly one path through a fork ever runs — everything after a taken branch is skipped, not merely "also considered," so getting the condition and branch boundaries right matters |
