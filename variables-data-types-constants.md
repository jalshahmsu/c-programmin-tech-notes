# Variables, Data Types & Constants

*This is the first topic in the sequence — no prior C knowledge is assumed. Arithmetic in the examples below is kept to the bare minimum needed for initialization; operator precedence and relational/logical operators are covered in the next lecture, Operators & Expressions.*

## 1. Learning Objectives

By the end of this document, you should be able to:

- Explain what a variable is, and why hardcoded literal values eventually cause problems in a program.
- Correctly declare and initialize variables of type `int`, `float`, `double`, and `char`, using valid identifier names.
- Choose between a literal, a `const` variable, and (at a basic level) `#define` for a value that shouldn't change.
- Use `printf` and `scanf` with the format specifier that correctly matches each data type.
- Predict the result of integer division versus floating-point division, and use a cast to force the one you need.
- Identify the risk of using an uninitialized variable and the consequence of forgetting `&` in `scanf`.

## 2. Why Variables Exist

Consider a program that computes the area of a circle:

```c
#include <stdio.h>

int main(void) {
    printf("Area = %f\n", 3.14159 * 5 * 5);
    printf("Area = %f\n", 3.14159 * 8 * 8);
    return 0;
}
```

The values `5` and `8` are typed directly into the code with no name explaining what they represent. If the radius needs to be read from a user, reused in a later calculation, or changed, there is nowhere to put that value — it exists only as a literal, fixed at the moment the code was written.

A variable gives a value a name, a place to live, and the ability to change:

```c
#include <stdio.h>

int main(void) {
    double radius = 5;
    double area = 3.14159 * radius * radius;

    printf("Area = %f\n", area);
    return 0;
}
```

`radius` can now be read from input, reused in more than one calculation, or reassigned — none of which was possible with a bare literal. This is the foundation every other topic in this course builds on.

## 3. Mental Model

Picture a variable as a labeled box:

- **Name** — the label on the box; how you refer to it in code.
- **Type** — the box's shape and size; determines what kind of value fits inside and how much space it takes.
- **Value** — what's currently inside the box; can be replaced.
- **Address** — the specific spot the box sits at in memory. You won't compute or use this directly yet, but every box has one, and this becomes essential later: Arrays extends "one box" into "a row of boxes," and Pointers introduces a box whose contents are another box's address.

| Name | Type | Value |
|---|---|---|
| `age` | `int` | 20 |
| `price` | `float` | 49.99 |
| `grade` | `char` | 'A' |

Everything in this document is a consequence of this picture: a name bound to a typed, changeable, addressable storage location.

## 4. Syntax and Core Mechanics

### Core Data Types

| Type | Holds | Typical Size | Example |
|---|---|---|---|
| `int` | Whole numbers | 4 bytes | `int age = 20;` |
| `float` | Decimal numbers (single precision) | 4 bytes | `float price = 49.99f;` |
| `double` | Decimal numbers (double precision, more accurate) | 8 bytes | `double pi = 3.14159265;` |
| `char` | A single character | 1 byte | `char grade = 'A';` |

"Typical size" varies slightly by compiler and platform — `sizeof` (Example 6) lets you check directly on your own machine.

### Declaration and Initialization

| Form | Example | Meaning |
|---|---|---|
| Declaration only | `int age;` | Reserves space; the value is garbage until assigned |
| Declaration with initialization | `int age = 20;` | Reserves space and sets an initial value immediately |
| Multiple variables, one statement | `int x = 1, y = 2, z = 3;` | Declares and initializes three variables of the same type |
| Assignment (after declaration) | `age = 21;` | Changes the value already stored |

**Identifier rules:** a name must start with a letter or underscore, may contain letters, digits, and underscores after that, is case-sensitive, and cannot be a reserved keyword (such as `int` or `return`).

### Constants

| Form | Example | Fixed At |
|---|---|---|
| Literal constant | `5`, `3.14`, `'A'` | A raw value written directly in code |
| `const` variable | `const double PI = 3.14159;` | The compiler prevents any later reassignment |
| `#define` macro | `#define PI 3.14159` | Text substituted before compilation begins (covered fully in a later lecture on the preprocessor) |

## 5. How It Works Underneath

Every variable occupies a block of memory whose size is fixed by its type — this is why `sizeof(int)` and `sizeof(double)` typically differ. The type also determines how those bits are *interpreted*: the same four bytes of memory mean something completely different read as an `int` versus read as a `float`. A type is not just a size — it is an instruction for how to decode whatever bits are stored there.

**Implicit conversion:** when a value of one type is used where another is expected, C converts it automatically. Assigning an `int` to a `double` variable widens it losslessly: `double d = 5;` becomes `5.0`.

**Explicit conversion (casting):** `(type) expression` forces a conversion — `(double) 7 / 2` evaluates to `3.5` rather than an integer result.

**Integer division:** dividing two `int` values with `/` discards the fractional part entirely — `7 / 2` is `3`, not `3.5`. This is truncation, not rounding, and the type of the division is decided by the operand types *before* the result is ever assigned anywhere — assigning the result to a `double` afterward does not recover the lost fraction (Example 4, and Question 1).

## 6. Worked Examples

### Example 1: Declaring and Printing Each Core Type

```c
#include <stdio.h>

int main(void) {
    int age = 20;
    float price = 49.99f;
    double pi = 3.14159265;
    char grade = 'A';

    printf("age = %d\n", age);
    printf("price = %.2f\n", price);
    printf("pi = %.8f\n", pi);
    printf("grade = %c\n", grade);
    return 0;
}
```

Output:
```
age = 20
price = 49.99
pi = 3.14159265
grade = A
```

Each `printf` specifier is matched to its variable's actual type — this pairing is not optional (Section 8).

### Example 2: Named Constants Instead of Magic Numbers

```c
#include <stdio.h>

int main(void) {
    const double PI = 3.14159;
    double radius = 5;

    printf("Area = %.2f\n", PI * radius * radius);
    printf("Circumference = %.2f\n", 2 * PI * radius);
    return 0;
}
```

Output:
```
Area = 78.54
Circumference = 31.42
```

`PI` is written once, with a name that explains itself, and reused in two calculations. Attempting `PI = 3.0;` afterward would be a compiler error — `const` is a promise, enforced by the compiler, that the value will not change.

### Example 3: Basic Input with `scanf`

```c
#include <stdio.h>

int main(void) {
    int age;

    printf("Enter your age: ");
    scanf("%d", &age);

    printf("You entered: %d\n", age);
    return 0;
}
```

Sample run (input `20`):
```
Enter your age: 20
You entered: 20
```

`scanf` needs to know *where* in memory to store the value it reads — `&age` gives it `age`'s address, not its value. This is the first appearance of `&` in this course. For now, treat it as a required part of `scanf`'s syntax when reading into an ordinary variable; it is explained fully once Pointers is covered later in this sequence.

### Example 4: Integer Division vs. a Cast

```c
#include <stdio.h>

int main(void) {
    int total = 7, count = 2;

    printf("Integer division: %d\n", total / count);
    printf("Cast to double: %.1f\n", (double) total / count);
    return 0;
}
```

Output:
```
Integer division: 3
Cast to double: 3.5
```

`total / count` uses two `int` operands, so the division truncates. Casting `total` to `double` before dividing forces a floating-point division instead.

### Example 5: An Uninitialized Variable

```c
#include <stdio.h>

int main(void) {
    int mystery;

    printf("mystery = %d\n", mystery);
    return 0;
}
```

Output (illustrative — unpredictable, and varies by machine and run):
```
mystery = 32601
```

C does not automatically set local variables to zero. `mystery` holds whatever bits happened to already be sitting in that memory location. This example exists only to demonstrate the risk — always initialize a variable before reading it (Section 8).

### Example 6: Observing Type Sizes with `sizeof`

```c
#include <stdio.h>

int main(void) {
    printf("sizeof(int) = %zu\n", sizeof(int));
    printf("sizeof(float) = %zu\n", sizeof(float));
    printf("sizeof(double) = %zu\n", sizeof(double));
    printf("sizeof(char) = %zu\n", sizeof(char));
    return 0;
}
```

Output (typical on most systems):
```
sizeof(int) = 4
sizeof(float) = 4
sizeof(double) = 8
sizeof(char) = 1
```

These numbers confirm the "typical size" column in Section 4 directly on your own machine, rather than taking it on faith.

## 7. Common Patterns / Idioms

| Pattern | Shape | Typical Use |
|---|---|---|
| Meaningful naming | `double accountBalance` instead of `double x` | Making code self-explanatory without extra comments |
| Named constants over magic numbers | `const double TAX_RATE = 0.18;` used throughout, instead of `0.18` scattered through the code | A value used more than once, or one whose meaning isn't obvious on its own |
| Initialize at declaration | `int count = 0;` instead of declaring and assigning later | Avoiding the uninitialized-variable risk from the start |
| Read, then echo | `scanf` into a variable, immediately `printf` it back | A simple way to confirm input was read correctly while learning |

## 8. Common Mistakes and How to Debug Them

| Mistake | Symptom | Why It Happens | How to Catch It |
|---|---|---|---|
| Using an uninitialized variable | Unpredictable ("garbage") value printed or used in a calculation | C does not automatically set local variables to zero | Always initialize a variable at the point you declare it, or before its first use |
| Mismatched format specifier (e.g., `%d` for a `float`) | Garbage output, or a compiler warning with `-Wall` | `printf`/`scanf` trust the specifier to know how many bytes to read and how to interpret them; a mismatch misreads the underlying bits | Match every specifier to the variable's actual type: `%d` for `int`, `%f` for `float`/`double`, `%c` for `char` |
| Forgetting `&` in `scanf` | Program crashes, or silently fails to store the input | `scanf` needs the variable's address to know where to write the value it reads | Always write `&variable` in `scanf` for ordinary variables |
| Unexpected integer division | A result like `3` where `3.5` was expected | Dividing two `int` values with `/` truncates the fractional part; it does not round | Cast at least one operand to `double` before dividing, if a fractional result is needed |
| Overflow | A value wraps to something unexpected (often very negative) instead of growing larger | The value exceeded the range its type can represent | Choose a type with enough range for the values involved, and remember every type has a maximum |

## 9. Choosing a Data Type / Choosing a Constant Form

| Use this type when... | 
|---|
| `int` — counting, indexing, or any whole-number quantity |
| `double` — the default choice for decimal values; more precise than `float` at a small memory cost |
| `float` — a decimal value where memory is tight and high precision isn't critical (less common in modern practice) |
| `char` — a single character, or, later, as the building block of strings |

| Use this constant form when... |
|---|
| `const` — a named value that should never change during the program, with the compiler enforcing that and respecting the value's type |
| `#define` — a value needed before compilation even begins (for example, an array size used in multiple places) — covered fully in a later lecture |
| A plain literal — a value that appears exactly once and needs no explanation beyond its immediate context |

## 10. Connection to the Bigger Picture

The "name, type, value" model introduced here is the single idea every remaining topic in this course extends, rather than replaces. An array, covered later, is a row of same-typed boxes addressed by position instead of one box with one name. A pointer, covered later still, is a box whose value is another box's address, rather than ordinary data. Even a `struct`, further ahead, is simply several named, typed boxes bundled together under one name. Understanding a variable precisely — as a name bound to a typed, addressable storage location — is what makes each of those later ideas feel like a natural extension rather than a brand-new concept.

More broadly, a language's type system exists to catch a whole category of mistakes before a program ever runs — assigning a character where a number is expected, or treating a decimal as a whole number and silently losing information. This is a first taste of a habit that runs through all of software engineering: catching an error as early as possible is far cheaper than discovering it after the fact.

## 11. Practice Questions

### Question 1 (Trace / Predict Output)

What does the following program print?

```c
#include <stdio.h>

int main(void) {
    int a = 9, b = 4;
    double result = a / b;

    printf("result = %.2f\n", result);
    return 0;
}
```

**Solution.** `a / b` is evaluated first, using two `int` operands — this is an integer division, giving `2` (truncated from `2.25`). Only *after* that truncated result is computed does the assignment to `result` happen, which simply widens the integer `2` into `2.0`. Assigning to a `double` variable does not go back and redo the division as floating-point.

Output: `result = 2.00`

### Question 2 (Write a Program)

Write a program that declares a `double` for a shopping cart's subtotal, applies a named constant `TAX_RATE` of `0.18`, computes the total after tax, and prints both values rounded to two decimal places.

**Solution.**

```c
#include <stdio.h>

int main(void) {
    const double TAX_RATE = 0.18;
    double subtotal = 250.0;
    double total = subtotal + (subtotal * TAX_RATE);

    printf("Subtotal = %.2f\n", subtotal);
    printf("Total = %.2f\n", total);
    return 0;
}
```

Output:
```
Subtotal = 250.00
Total = 295.00
```

### Question 3 (Debug)

The following program crashes when run. Find and fix the bug.

```c
#include <stdio.h>

int main(void) {
    int score;
    printf("Enter your score: ");
    scanf("%d", score);

    printf("Score = %d\n", score);
    return 0;
}
```

**Solution.** `scanf("%d", score)` passes the *value* currently stored in `score` (garbage, since it's uninitialized) instead of its *address*. `scanf` has no valid location to write the input into, which is undefined behavior and typically crashes the program (Section 8, row 3). Fix by adding `&`:

```c
scanf("%d", &score);
```

### Question 4 (Combine Conversion and Constants)

A program stores hours and extra minutes separately, both as `int`, and needs to print the total time as a decimal number of hours (for example, 2 hours 30 minutes should print as `2.50`). Write a program that does this correctly.

**Solution.**

```c
#include <stdio.h>

int main(void) {
    int hours = 2, minutes = 30;
    double totalHours = hours + (minutes / 60.0);

    printf("Total hours = %.2f\n", totalHours);
    return 0;
}
```

Output:
```
Total hours = 2.50
```

**Reasoning.** `minutes / 60.0` divides an `int` by a `double` literal; because one operand is already a `double`, C implicitly widens `minutes` before dividing, producing `0.5` rather than a truncated integer result. This is the opposite situation from Question 1, where both operands were `int` and truncation happened before any conversion could help.

### Question 5 (Design / Judgement — No Single Code Answer)

A programmer needs to store a person's age, and is deciding between `int`, `float`, and using a plain literal directly wherever age is needed, with no variable at all. Using the ideas from this document, which would you recommend, and why? What would change if the program needed to store someone's exact bank balance instead?

**Discussion.** Age is a whole-number quantity that typically varies from person to person and is used more than once in most programs — in comparisons, calculations, or display. A plain literal cannot work at all once the value needs to vary or be reused, which is exactly the reason variables exist in the first place (Section 2). Between `int` and `float`, `int` is the correct choice: an age in years never has a meaningful fractional part, so `float` would waste the type's precision on nothing and slightly complicate later comparisons, since comparing floating-point values for exact equality is generally unreliable — a subtlety this document has only flagged, not covered in depth.

For an exact bank balance, the answer changes: a `double` is the safer default among the types covered here, since it holds a genuinely meaningful decimal value with more precision than `float`. It's worth knowing, even at this stage, that professional financial software often avoids plain floating-point types for currency entirely, for reasons of exact rounding that go beyond what this document covers.

## 12. Summary Table

| Aspect | Detail |
|---|---|
| Variable model | Name + type + value (+ an address, used directly starting with Arrays) |
| Declaration | `type name;` or `type name = value;` |
| Core types | `int` (whole numbers), `float`/`double` (decimals), `char` (single character) |
| Constants | `const type name = value;` (typed, compiler-enforced) or `#define NAME value` (preprocessor text substitution) |
| Output | `printf("format", value)` — specifier must match the variable's type |
| Input | `scanf("format", &variable)` — `&` is required for ordinary variables |
| Integer division | `int / int` truncates the fractional part; cast an operand to `double` to get a decimal result |
| Most important rule | Always initialize a variable before reading its value — an uninitialized variable's contents are unpredictable |
