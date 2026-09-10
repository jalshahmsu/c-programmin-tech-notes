# Loops in C

*This lecture assumes variables, data types, operators, and conditionals (`if`/`else`) have already been taught. It does not assume Functions — Loops is covered before Functions in this sequence, so no examples use user-defined functions; only `main()` and standard library calls (`printf`, `scanf`) appear.*

## 1. Learning Objectives

By the end of this document, you should be able to:

- Explain what problem loops solve compared to writing repeated statements by hand.
- Correctly write a `while`, `do-while`, and `for` loop, and state precisely when each one checks its condition.
- Predict the difference in behavior between a `while` and a `do-while` loop when the condition starts false.
- Use `break` and `continue` correctly, including inside nested loops, and explain which loop each one affects.
- Trace a nested loop by hand and predict its full output.
- Identify and fix the most common loop bugs: off-by-one bounds, infinite loops, and a stray semicolon after a loop header.
- Decide, given a task, which of the three loop types fits best, and whether a loop is the right tool at all.

## 2. Why Loops Exist

Suppose a program needs to print "Hello" five times:

```c
#include <stdio.h>

int main(void) {
    printf("Hello\n");
    printf("Hello\n");
    printf("Hello\n");
    printf("Hello\n");
    printf("Hello\n");
    return 0;
}
```

This works for five repetitions. It breaks down for 500 — and it cannot work at all if the number of repetitions is only known once the program is running (say, a value the user types in), because there is no way to "copy-paste" a statement a variable number of times.

A loop solves this by describing the repeated action once, along with a condition for how many times to repeat it:

```c
#include <stdio.h>

int main(void) {
    for (int i = 0; i < 5; i++) {
        printf("Hello\n");
    }
    return 0;
}
```

Changing this to run 500 times — or a number read from the user — now means changing one value, not rewriting the program.

## 3. Mental Model

Picture a loop as a gate placed in the flow of the program. Before the body can run, execution must pass through the gate, where a condition is checked. If the condition is true, the body runs once, and execution is sent back to the gate to be checked again. If the condition is false, execution exits past the loop entirely and continues with whatever comes next.

The three loop types differ only in **where the gate sits relative to the body**:

| Loop Type | Gate Position | Consequence |
|---|---|---|
| `while` | Before the body | Body may run zero times, if the condition starts false |
| `do-while` | After the body | Body always runs at least once, regardless of the condition |
| `for` | Before the body (same as `while`), with setup and update folded into the header | Body may run zero times; setup and update are kept together with the condition for readability |

Every fact in this document — including how `break` and `continue` behave — follows from this one picture of execution repeatedly passing through a gate.

## 4. Syntax and Core Mechanics

| Form | Example | Meaning |
|---|---|---|
| `while` | `while (condition) { body }` | Body runs repeatedly as long as `condition` is true, checked before each run |
| `do-while` | `do { body } while (condition);` | Body runs once, then repeats as long as `condition` is true, checked after each run |
| `for` | `for (init; condition; update) { body }` | `init` runs once; then `condition` is checked, `body` runs, and `update` runs, repeating until `condition` is false |
| `break` | `break;` | Immediately exits the nearest enclosing loop |
| `continue` | `continue;` | Skips the rest of the current iteration's body |

Note the required semicolon after a `do-while`'s condition (`} while (condition);`) — this is the one loop form that ends with a semicolon.

## 5. How It Works Underneath

**`while`:**
1. Check the condition.
2. If false, exit — jump to the statement after the closing brace.
3. If true, run the body.
4. Go back to step 1.

**`do-while`:**
1. Run the body.
2. Check the condition.
3. If true, go back to step 1.
4. If false, exit.

The only structural difference from `while` is that the first pass runs the body unconditionally, before the condition is even consulted — this is exactly why a `do-while` always executes its body at least once.

**`for`:** `for (init; condition; update) { body }` is precisely equivalent to:

```c
init;
while (condition) {
    body;
    update;
}
```

This equivalence matters for two reasons covered later: it explains exactly what `for` is doing underneath its compact syntax (Example 3), and it explains precisely why `continue` behaves differently inside a `for` loop than inside a `while` loop (Section 8).

**Nested loops:** each level of nesting has its own independent gate. Every single pass through the outer loop's body causes the inner loop to start over completely — if the inner loop is a `for` loop, its `init` runs again from scratch on every outer iteration.

## 6. Worked Examples

### Example 1: A Basic `while` Loop

```c
#include <stdio.h>

int main(void) {
    int i = 1;

    while (i <= 5) {
        printf("%d ", i);
        i++;
    }
    printf("\n");
    return 0;
}
```

Output:
```
1 2 3 4 5 
```

### Example 2: `while` vs. `do-while` When the Condition Starts False

```c
#include <stdio.h>

int main(void) {
    int i;

    i = 10;
    printf("while:    ");
    while (i <= 5) {
        printf("%d ", i);
        i++;
    }
    printf("(body never ran)\n");

    i = 10;
    printf("do-while: ");
    do {
        printf("%d ", i);
        i++;
    } while (i <= 5);
    printf("\n");

    return 0;
}
```

Output:
```
while:    (body never ran)
do-while: 10 
```

Both loops share the same condition, `i <= 5`, which is false from the very first check (`i` starts at 10). The `while` loop never runs its body at all. The `do-while` loop runs its body once regardless, printing `10`, before checking the condition and exiting.

### Example 3: `for` as a `while` Loop in Disguise

```c
#include <stdio.h>

int main(void) {
    for (int i = 1; i <= 5; i++) {
        printf("%d ", i);
    }
    printf("\n");

    int j = 1;
    while (j <= 5) {
        printf("%d ", j);
        j++;
    }
    printf("\n");

    return 0;
}
```

Output:
```
1 2 3 4 5 
1 2 3 4 5 
```

Both loops produce identical output because they are the same gate model — the `for` loop simply keeps the initialization, condition, and update together in one header instead of scattering them around the loop.

### Example 4: `break` — Stopping Early

```c
#include <stdio.h>

int main(void) {
    for (int i = 1; i <= 100; i++) {
        if (i % 7 == 0) {
            printf("First multiple of 7 found: %d\n", i);
            break;
        }
    }
    return 0;
}
```

Output:
```
First multiple of 7 found: 7
```

Without `break`, this loop would run all the way to 100 even though the answer is found immediately. `break` exits the loop the moment the condition is met, skipping every remaining iteration.

### Example 5: `continue` — Skipping an Iteration

```c
#include <stdio.h>

int main(void) {
    for (int i = 1; i <= 10; i++) {
        if (i % 3 == 0) {
            continue;
        }
        printf("%d ", i);
    }
    printf("\n");
    return 0;
}
```

Output:
```
1 2 4 5 7 8 10 
```

Multiples of 3 (3, 6, 9) are skipped. Because this is a `for` loop, `continue` still runs the `update` step (`i++`) every time, exactly as the `while`-equivalence in Section 5 predicts — this is why the loop still terminates normally. Section 8 shows what goes wrong when the same pattern is written with a `while` loop instead.

### Example 6: Nested Loops — A Multiplication Table

```c
#include <stdio.h>

int main(void) {
    for (int row = 1; row <= 3; row++) {
        for (int col = 1; col <= 3; col++) {
            printf("%d ", row * col);
        }
        printf("\n");
    }
    return 0;
}
```

Output:
```
1 2 3 
2 4 6 
3 6 9 
```

For every single pass of the outer `row` loop, the inner `col` loop restarts completely from `col = 1` — this is the "independent gate per level" idea from Section 5 made concrete.

### Example 7: `break` Inside Nested Loops Only Affects the Inner Loop

```c
#include <stdio.h>

int main(void) {
    for (int row = 1; row <= 3; row++) {
        for (int col = 1; col <= 3; col++) {
            if (col == 2) {
                break;
            }
            printf("row %d, col %d\n", row, col);
        }
    }
    return 0;
}
```

Output:
```
row 1, col 1
row 2, col 1
row 3, col 1
```

The `break` fires every time `col` reaches 2, but it only exits the inner (`col`) loop — the outer (`row`) loop is completely unaffected and continues through all three of its own iterations. This is one of the most common points of confusion with nested loops: `break` and `continue` never reach past the loop they are directly written inside.

## 7. Common Patterns / Idioms

| Pattern | Shape | Typical Use |
|---|---|---|
| Counting loop | `for` loop with a counter from a start value to an end value | Repeating an action a known number of times |
| Accumulation | Loop body adds or multiplies into a running total | Sums, products, averages over a range |
| Sentinel-controlled input | A "priming" read before the loop, then another read at the end of each iteration, stopping when a special value appears | Reading an unknown amount of input from a user |
| `do-while` input validation | Loop that keeps asking until input passes a check | Forcing valid input before continuing (menu-driven programs) |
| Early-exit search | Loop with an `if` + `break` once a target is found | Stopping as soon as an answer is found, instead of always running the full range |
| Nested loop for grids | Outer loop for rows, inner loop for columns | Tables, multiplication grids, printed patterns |

```c
/* Sentinel-controlled input */
int value, sum = 0;
scanf("%d", &value);
while (value != -1) {
    sum += value;
    scanf("%d", &value);
}
```

```c
/* do-while input validation */
int choice;
do {
    printf("Enter a choice between 1 and 3: ");
    scanf("%d", &choice);
} while (choice < 1 || choice > 3);
```

The `do-while` validation pattern exists specifically because you need to prompt the user *at least once* before there is anything to check — a direct application of the "body always runs at least once" property from Section 5.

## 8. Common Mistakes and How to Debug Them

| Mistake | Symptom | Why It Happens | How to Catch It |
|---|---|---|---|
| Off-by-one bound (`<=` vs. `<`) | Loop runs one time too many or too few | Confusing "up to and including" with "up to but excluding" | Decide up front whether the last value should be included, and match the operator to that decision |
| Forgetting to update the loop variable | Infinite loop | The condition never becomes false, because nothing inside the body changes the variable it depends on | Check that every variable in the condition is updated somewhere inside the loop body |
| Stray semicolon after a loop header (`for (...);`) | The loop appears to "do nothing," and code meant to be the loop body runs only once, after the loop finishes | The semicolon becomes an empty statement — the loop's entire body — so the real body underneath runs only once, unconditionally, after the loop is already done | Never place a semicolon immediately after a loop header unless an empty body is genuinely intended |
| Assuming `while` and `do-while` always behave the same | Program is correct except in the one case where the condition is false from the very start | They differ only in when the condition is checked — before the body (`while`) or after it (`do-while`) | Ask: "should this run at least once even if the condition starts false?" If yes, use `do-while` |
| `continue` inside a `while`/`do-while` skipping the update | Infinite loop | `continue` jumps straight to the condition check; if the code that updates the loop variable sits after the `continue`, it never runs | Place the update before any `continue`, or use a `for` loop, where the update always runs regardless of `continue` |

```c
int i = 1;
while (i <= 10) {
    if (i % 3 == 0) {
        continue;   /* jumps back to the condition check; i++ below never runs when this fires */
    }
    printf("%d ", i);
    i++;
}
```

When `i` reaches 3, `continue` sends execution straight back to `i <= 10` with `i` still equal to 3 — `i++` is never reached, so `i` never changes again, and the loop runs forever. Compare this against Example 5, which does the same skipping safely inside a `for` loop.

## 9. When to Use Which Loop / When Not to Loop at All

| Situation | Best Fit |
|---|---|
| The number of iterations, or a counting variable, is central to the loop | `for` — keeps initialization, condition, and update together in one place |
| The loop should keep running based on a condition, and a counter isn't the natural way to describe it | `while` |
| The body must run at least once, no matter what, before the condition is even meaningful to check | `do-while` |

| Use a loop when... | Avoid a loop when... |
|---|---|
| The same action repeats across a variable or large number of steps | The task is a small, fixed number of genuinely different actions — separate statements are clearer |
| The number of repetitions depends on data, input, or a condition evaluated at runtime | The "repetition" is really just two or three distinct steps that don't share the same shape |

Once Arrays is covered, loops become the primary tool for processing every element in a collection — the exact same condition-gate model from this document reappears unchanged for that purpose.

## 10. Connection to the Bigger Picture

Iteration (looping) and branching (`if`/`else`, already covered) are the two fundamental control-flow tools present in essentially every programming language. The specific keywords differ — `for`, `while`, `foreach`, and others — but the underlying idea, "check a condition, then decide what runs next," does not. These two tools together are powerful enough, in principle, to express any computable process; every more advanced control structure met later in your programming education is built as convenience on top of these two ideas, not as something fundamentally beyond them.

## 11. Practice Questions

### Question 1 (Trace / Predict Output)

What does the following program print?

```c
#include <stdio.h>

int main(void) {
    int i = 1, total = 0;

    while (i <= 6) {
        if (i % 2 == 0) {
            total += i;
        }
        i++;
    }

    printf("total = %d\n", total);
    return 0;
}
```

**Solution.** Trace each pass:

| i | i % 2 == 0? | total after this pass |
|---|---|---|
| 1 | no | 0 |
| 2 | yes | 2 |
| 3 | no | 2 |
| 4 | yes | 6 |
| 5 | no | 6 |
| 6 | yes | 12 |

The loop then checks `i <= 6` with `i = 7` and exits.

Output: `total = 12`

### Question 2 (Write a Program)

Write a program that reads integers using a `while` loop, stopping when the user enters `-1`, and prints the sum of all entered numbers (not counting `-1`).

**Solution.**

```c
#include <stdio.h>

int main(void) {
    int value, sum = 0;

    printf("Enter numbers, -1 to stop:\n");
    scanf("%d", &value);

    while (value != -1) {
        sum += value;
        scanf("%d", &value);
    }

    printf("Sum = %d\n", sum);
    return 0;
}
```

Sample run (input `5 10 15 -1`):
```
Sum = 30
```

**Reasoning.** A sentinel-controlled loop needs one "priming" read before the condition can be checked for the first time, and another read at the end of each iteration — otherwise the loop would either never check the very first value, or would read the same value forever.

### Question 3 (Debug)

The following program is meant to print the numbers 1 through 5, one per line, but instead prints only `6`. Find and fix the bug.

```c
#include <stdio.h>

int main(void) {
    int i;

    for (i = 1; i <= 5; i++);
    {
        printf("%d\n", i);
    }

    return 0;
}
```

**Solution.** The semicolon immediately after `for (i = 1; i <= 5; i++)` is a stray empty statement — it *is* the loop's entire body. The `for` loop silently runs five times doing nothing, incrementing `i` each time, until `i` becomes 6 and the condition fails. The `{ printf(...); }` block below is not part of the loop at all; it is an ordinary block that runs exactly once, after the loop has already finished, printing whatever `i` ended up as: `6`.

Fix by removing the semicolon so the block becomes the actual loop body:

```c
for (i = 1; i <= 5; i++) {
    printf("%d\n", i);
}
```

Output:
```
1
2
3
4
5
```

### Question 4 (Combine Nested Loops with `break`)

Using nested loops, print every pair `(i, j)` with `1 <= i <= 9` and `1 <= j <= 9` whose product is exactly 24, but stop the entire search — both loops — as soon as 3 such pairs have been found.

**Solution.**

```c
#include <stdio.h>

int main(void) {
    int count = 0;
    int stop = 0;

    for (int i = 1; i <= 9 && !stop; i++) {
        for (int j = 1; j <= 9; j++) {
            if (i * j == 24) {
                printf("(%d, %d)\n", i, j);
                count++;
                if (count == 3) {
                    stop = 1;
                    break;
                }
            }
        }
    }
    return 0;
}
```

Output:
```
(3, 8)
(4, 6)
(6, 4)
```

**Reasoning.** As shown in Example 7, `break` only exits the inner (`j`) loop — it cannot reach the outer (`i`) loop by itself. The `stop` flag is set the moment the third pair is found, and the outer loop's own condition (`i <= 9 && !stop`) is what actually ends the search entirely on the next check.

### Question 5 (Design / Judgement — No Single Code Answer)

A programmer wants to print `"Loading...\n"` exactly three times and writes:

```c
for (int i = 0; i < 3; i++) {
    printf("Loading...\n");
}
```

A colleague argues a loop is overkill for just three fixed `printf` calls, and that writing `printf("Loading...\n");` three times in a row would be clearer. Using the criteria from Section 9, evaluate both views — and say whether your answer would change if the number of times to print depended on a value entered by the user.

**Discussion.** Section 9's "avoid a loop" criterion is meant for a small number of genuinely *different* steps, where forcing them into a loop would obscure what's happening. That is not the situation here: all three lines are the exact same action, repeated — precisely what the "use a loop" criterion describes, regardless of how small the count is. A loop makes the intent ("repeat this the same way") explicit and trivially adjustable, so it remains the better default even at just three repetitions; three literal `printf` calls is defensible only as a matter of house style for a genuinely fixed, never-changing count, not because it's clearer.

The moment the count depends on a value entered by the user, the loop stops being a stylistic preference and becomes necessary — three literal statements cannot express "however many times the user asked for" at all. This is exactly the case Section 9 identifies as an unambiguous reason to use a loop: the number of repetitions depends on data evaluated at runtime.

## 12. Summary Table

| Aspect | Detail |
|---|---|
| `while` | `while (condition) { body }` — condition checked before every iteration; may run zero times |
| `do-while` | `do { body } while (condition);` — condition checked after every iteration; body always runs at least once |
| `for` | `for (init; condition; update) { body }` — equivalent to `init; while (condition) { body; update; }` |
| `break` | Immediately exits the nearest enclosing loop entirely |
| `continue` | Skips the rest of the current iteration; jumps to the condition check (`while`/`do-while`) or the update step (`for`) |
| Nested loops | A loop inside another loop's body; each level has its own independent gate; `break`/`continue` affect only the nearest enclosing loop |
| Most important rule | Every loop needs a condition that can eventually become false (or a reachable `break`) — otherwise it never terminates |
