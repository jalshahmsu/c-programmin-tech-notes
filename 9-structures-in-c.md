# Structures in C

## 1. Learning Objectives

By the end of this document, you should be able to:

- Declare a struct type, declare variables of that type, and initialize them using both aggregate and designated initialization.
- Predict the exact memory layout of a struct, including where compiler-inserted padding will appear and why.
- Choose correctly between `.` and `->` when accessing struct members through a plain variable versus a pointer, and explain the precedence reason the parenthesized form `(*p).field` requires parentheses.
- Explain, with a cost argument, why passing a struct to a function by value copies the entire struct, and rewrite such a function to take a pointer instead.
- Reorganize a collection of records between Array of Structures (AoS) and Structure of Arrays (SoA) form, and justify which layout is better for a given access pattern.
- Identify the five most common struct-related bugs from the shape of the broken code, without running it.

## 2. Why This Exists

Recall the two organizing ideas you already have:

- A **variable** is one labeled box holding one value of one type.
- An **array** is many boxes of the *same* type, addressed by position (an index).

Neither tool solves the following problem. Suppose you are tracking a student record: a roll number (`int`), a name (`char[50]`), and a GPA (`float`). Without structs, using only what you already know, you are forced into one of two bad options.

**Option A: separate variables.**

```c
#include <stdio.h>

int main(void) {
    int roll1 = 101;
    char name1[50] = "Asha Rao";
    float gpa1 = 8.7;

    int roll2 = 102;
    char name2[50] = "Vikram Shah";
    float gpa2 = 7.9;

    printf("%d %s %.1f\n", roll1, name1, gpa1);
    printf("%d %s %.1f\n", roll2, name2, gpa2);
    return 0;
}
/* Output:
101 Asha Rao 8.7
102 Vikram Shah 7.9
*/
```

This does not scale. For 100 students you would need 300 separate variable names, and nothing in the language tells you that `roll1`, `name1`, and `gpa1` belong together. You are tracking the relationship in your head, not in the code.

**Option B: parallel arrays.**

```c
#include <stdio.h>

int main(void) {
    int roll[100];
    char name[100][50];
    float gpa[100];

    roll[0] = 101;
    /* name[0] and gpa[0] would be set the same way */

    /* The relationship "index 0 of roll, name, and gpa describe
       the same student" is not enforced by the compiler at all.
       Nothing stops you from writing roll[0] and gpa[5] by mistake
       and getting a silently wrong record. */
    return 0;
}
```

Parallel arrays scale in count, but the grouping is still implicit. If you sort `roll` by value, you must remember to sort `name` and `gpa` identically, in the same statement, every time, or the records silently desynchronize.

A struct solves the grouping problem directly: it lets you bundle several values of *different* types under one name, so the language itself enforces that they travel together. This is the third and final piece of a progression this course has been building:

| Construct | What it groups | Types allowed | Addressed by |
|---|---|---|---|
| Variable | One value | One | Name |
| Array | Many values | Same type | Position (index) |
| Struct | Several values | Different types allowed | Name (member name) |

## 3. Mental Model

Picture a struct as a small filing card with labeled fields, laid out left to right in memory, in the order you declared them.

```
struct Student {
    int   roll;
    char  name[50];
    float gpa;
};

Memory layout of one struct Student variable:

  Address:   1000        1004                  1054
             +-----------+---------------------+---------+
  Field:     |   roll    |        name         |   gpa   |
             |  (4 bytes)|      (50 bytes)      |(4 bytes)|
             +-----------+---------------------+---------+
```

Three things to internalize from this picture immediately:

1. All members live inside one contiguous block of memory, in declaration order (subject to padding rules — Section 6).
2. The struct itself has one starting address; every member's address is that starting address plus some fixed offset.
3. Unlike an array, you cannot walk from one member to the next with `+1` in a loop, because the members are not the same type or size. You reach a member by name, not by index.

Keep this filing-card image in mind; every later section (padding, `->`, AoS vs. SoA) is a variation on it.

## 4. Syntax and Core Mechanics

### 4.1 Declaring a struct type

```c
struct Student {
    int roll;
    char name[50];
    float gpa;
};
```

This line alone does **not** create any variable. It defines a new type name, `struct Student`, that the compiler now knows about. No memory is allocated until you declare a variable of that type.

### 4.2 Declaring variables

```c
struct Student s1;          /* one struct Student variable, uninitialized */
struct Student s2, s3;      /* two more */
```

In plain C (no `typedef`), you must write the keyword `struct` every time you name the type. Writing `Student s1;` without `struct` is a compile error.

### 4.3 Initialization forms

| Form | Example | Notes |
|---|---|---|
| Aggregate (positional) | `struct Student s1 = {101, "Asha Rao", 8.7};` | Values assigned to members in declaration order. Error-prone if member order is forgotten. |
| Designated initializer | `struct Student s1 = {.roll = 101, .name = "Asha Rao", .gpa = 8.7};` | Explicit, order-independent, self-documenting. Same idea as designated array initializers from the Arrays lecture. |
| Partial initialization | `struct Student s1 = {.gpa = 8.7};` | Members not listed (`roll`, `name`) are zero-initialized. |
| Member-by-member assignment | `s1.roll = 101;` after declaration | Used when the values aren't known at declaration time. |

### 4.4 Accessing members: the dot operator

```c
#include <stdio.h>

struct Student {
    int roll;
    char name[50];
    float gpa;
};

int main(void) {
    struct Student s1 = {.roll = 101, .name = "Asha Rao", .gpa = 8.7};
    printf("%d %s %.1f\n", s1.roll, s1.name, s1.gpa);
    s1.gpa = 9.0;
    printf("Updated GPA: %.1f\n", s1.gpa);
    return 0;
}
/* Output:
101 Asha Rao 8.7
Updated GPA: 9.0
*/
```

### 4.5 `typedef` with structs

`typedef` does not change behavior. It only lets you drop the `struct` keyword at the point of use.

```c
typedef struct {
    int roll;
    char name[50];
    float gpa;
} Student;

int main(void) {
    Student s1 = {101, "Asha Rao", 8.7};   /* no "struct" needed here */
    return 0;
}
```

| Without typedef | With typedef |
|---|---|
| `struct Student s1;` | `Student s1;` |
| Type name is `struct Student` | Type name is `Student` |
| Behavior, memory layout, member access: identical | Behavior, memory layout, member access: identical |

### 4.6 Nested structs

A struct member can itself be a struct.

```c
#include <stdio.h>

struct Date {
    int day, month, year;
};

struct Student {
    int roll;
    struct Date enrolled;
};

int main(void) {
    struct Student s1 = {101, {12, 8, 2024}};
    printf("Roll %d enrolled on %d-%d-%d\n",
           s1.roll, s1.enrolled.day, s1.enrolled.month, s1.enrolled.year);
    return 0;
}
/* Output:
Roll 101 enrolled on 12-8-2024
*/
```

Access chains with the dot operator: `s1.enrolled.day` reads as "go into `s1`, then into its `enrolled` field, then into that field's `day` field."

### 4.7 `enum` as a struct field (brief)

An `enum` names a small set of related integer constants, useful as a struct field when a value should be restricted to a fixed set of options.

```c
enum Grade { PASS, FAIL, HONORS };

struct Student {
    int roll;
    enum Grade grade;
};
```

This is not a full treatment of `enum` — only enough to recognize it as a valid, common field type.

## 5. Struct Members That Are Arrays

A struct field can itself be an array. This is not a new mechanic — it is an ordinary field whose type happens to be an array type. `name` in every example above (`char name[50]`) is already this case.

```c
struct Matrix3 {
    int values[3];   /* an array field, nothing more */
};
```

Keep this distinct from Section 8 (Arrays of Structures) and Section 9 (Structure of Arrays), which are about how you organize a *collection* of records, not about a single struct having one array-typed field.

## 6. How It Works Underneath

### 6.1 Members are laid out in declaration order

A struct's members occupy one contiguous region of memory, placed in the order they were declared, starting at the struct's own address.

### 6.2 Padding: why `sizeof` is not the naive sum

Most CPUs read multi-byte values (like a 4-byte `int`) fastest when they start at an address that is a multiple of their size (4-byte alignment for a 4-byte `int`). To guarantee this, the compiler may insert unused padding bytes between members.

```c
#include <stdio.h>

struct Example {
    char  c;   /* 1 byte */
    int   i;   /* 4 bytes */
    char  d;   /* 1 byte */
};

int main(void) {
    printf("%zu\n", sizeof(struct Example));
    return 0;
}
/* Output (typical, 64-bit system):
12
*/
```

Naive sum: `1 + 4 + 1 = 6` bytes. Actual size: `12` bytes on a typical system.

```
Offset:   0     1  2  3     4  5  6  7     8      9 10 11
         +-----+--+--+--+  +--+--+--+--+  +------+--+--+--+
Field:   |  c  |  pad(3) |  |     i      |  |  d  |pad(3)  |
         +-----+---------+  +------------+  +-----+--------+
```

The 3 padding bytes after `c` exist so that `i` starts at offset 4 (a multiple of 4). The 3 bytes after `d` exist so that if this struct were placed in an array, the *next* struct's `c` would also start at a multiple of 4.

| Rule of thumb | Explanation |
|---|---|
| `sizeof(struct)` ≥ sum of member sizes | Padding only adds bytes, never removes them. |
| Padding is compiler- and platform-dependent | Do not hardcode assumed struct sizes; always use `sizeof`. |
| Reordering members can reduce padding | Grouping same-size members together often shrinks total size (not covered further here — this is a brief, non-exhaustive mention). |

This is intentionally not exhaustive. The only fact you must retain is: **never assume `sizeof(struct)` equals the sum of its members' sizes.**

## 7. Structs and Functions

### 7.1 Passing by value: the whole struct is copied

Recall from the Functions lecture: when you pass an argument by value, the function receives a copy on its own stack frame. For an `int`, that copy is cheap — 4 bytes. For a struct, the copy is the *entire* struct, member by member, padding included.

```c
#include <stdio.h>

struct Student {
    int roll;
    char name[50];
    float gpa;
};

void printStudent(struct Student s) {   /* s is a full COPY of the argument */
    printf("%d %s %.1f\n", s.roll, s.name, s.gpa);
}

int main(void) {
    struct Student s1 = {101, "Asha Rao", 8.7};
    printStudent(s1);
    return 0;
}
/* Output:
101 Asha Rao 8.7
*/
```

If `struct Student` were 60 bytes, every call to `printStudent` copies all 60 bytes onto the stack, even though the function only reads the data. This is the concrete cost the Pointers lecture referred to when it mentioned large structs as a motivating case for pointer parameters — Section 7.3 resolves it.

### 7.2 Structs and pointers: `->`

Given a pointer to a struct, you must dereference it before accessing a member. There are two equivalent ways to write this.

```c
#include <stdio.h>

struct Student {
    int roll;
    float gpa;
};

int main(void) {
    struct Student s1 = {101, 8.7};
    struct Student *p = &s1;

    printf("%d\n", (*p).roll);   /* long form */
    printf("%d\n", p->roll);     /* short form, identical meaning */
    return 0;
}
/* Output:
101
101
*/
```

The parentheses in `(*p).roll` are not optional. Recall the Operators lecture's precedence table: `.` binds *tighter* than the unary `*`. Without parentheses, `*p.roll` would be parsed as `*(p.roll)` — "dereference the result of `p.roll`" — which is wrong (and a compile error here, since `p` is a pointer, not a struct, and has no `.roll` member). The parentheses force `*p` to happen first.

| Expression | Meaning | Requires parentheses? |
|---|---|---|
| `s1.roll` | `s1` is a struct; access member directly | No |
| `(*p).roll` | Dereference `p` first, then access member | Yes — `.` binds tighter than `*` |
| `p->roll` | Same as `(*p).roll`, provided as a convenience operator | No — `->` is a single operator |

### 7.3 Passing structs by pointer

Passing a pointer instead of the struct itself copies only the pointer (4 or 8 bytes), regardless of how large the struct is.

```c
#include <stdio.h>

struct Student {
    int roll;
    char name[50];
    float gpa;
};

void raiseGpa(struct Student *s, float amount) {
    s->gpa += amount;   /* modifies the ORIGINAL struct, not a copy */
}

int main(void) {
    struct Student s1 = {101, "Asha Rao", 8.7};
    raiseGpa(&s1, 0.2);
    printf("%.1f\n", s1.gpa);
    return 0;
}
/* Output:
9.0
*/
```

This is the coded payoff of the scenario the Pointers lecture only discussed conceptually: a large struct passed by pointer avoids the copy cost from Section 7.1, and as a side effect also lets the function modify the caller's original data (useful when that is the intent; when it is *not* the intent, pass a `const struct Student *` instead, which permits reading but not writing through the pointer).

| Passing style | Cost per call | Can function modify caller's original? |
|---|---|---|
| By value (`struct Student s`) | Full struct size, copied | No |
| By pointer (`struct Student *s`) | Size of one pointer | Yes |
| By `const` pointer (`const struct Student *s`) | Size of one pointer | No (compiler-enforced) |

## 8. Arrays of Structures (AoS)

The default, "obvious" way to store a collection of records is an array whose element type is a struct: one array slot per full record.

```c
#include <stdio.h>

struct Student {
    int roll;
    float gpa;
};

int main(void) {
    struct Student class[3] = {
        {101, 8.7},
        {102, 7.9},
        {103, 9.1}
    };

    for (int i = 0; i < 3; i++) {
        printf("%d: %.1f\n", class[i].roll, class[i].gpa);
    }
    return 0;
}
/* Output:
101: 8.7
102: 7.9
103: 9.1
*/
```

`class[i]` gives you one entire record at once — every field for student `i` is one indexing operation away. This is a direct application of a prior-taught idea (arrays, looped with `for`) to a new element type (struct) — nothing new mechanically.

## 9. Structure of Arrays (SoA)

Structure of Arrays reorganizes the *same* collection as one struct containing several parallel arrays — one array per field, all indexed by the same position to represent "the same record" split across arrays.

```c
#include <stdio.h>

#define N 3

struct ClassRecords {
    int roll[N];
    float gpa[N];
};

int main(void) {
    struct ClassRecords class = {
        .roll = {101, 102, 103},
        .gpa  = {8.7, 7.9, 9.1}
    };

    for (int i = 0; i < N; i++) {
        printf("%d: %.1f\n", class.roll[i], class.gpa[i]);
    }
    return 0;
}
/* Output:
101: 8.7
102: 7.9
103: 9.1
*/
```

Here, "student `i`" is not one contiguous block anymore. It is `class.roll[i]` and `class.gpa[i]` — the same index into two separate arrays.

## 10. AoS vs. SoA, Concretely Compared

The two layouts store identical information. The difference is memory arrangement, and it matters when you do the *same computation* — summing one field across all records — both ways.

**AoS: summing `gpa` across all records**

```c
#include <stdio.h>

#define N 3

struct Student {
    int roll;
    float gpa;
};

int main(void) {
    struct Student class[N] = {{101, 8.7}, {102, 7.9}, {103, 9.1}};
    float total = 0;

    for (int i = 0; i < N; i++) {
        total += class[i].gpa;
    }
    printf("%.1f\n", total);
    return 0;
}
/* Output:
25.7
*/
```

**SoA: the same sum**

```c
#include <stdio.h>

#define N 3

struct ClassRecords {
    int roll[N];
    float gpa[N];
};

int main(void) {
    struct ClassRecords class = {{101, 102, 103}, {8.7, 7.9, 9.1}};
    float total = 0;

    for (int i = 0; i < N; i++) {
        total += class.gpa[i];
    }
    printf("%.1f\n", total);
    return 0;
}
/* Output:
25.7
*/
```

Both print the same answer. The difference is in memory layout during that loop:

```
AoS memory (roll, gpa interleaved per record):
[roll0][gpa0][roll1][gpa1][roll2][gpa2]
        ^ touched   ^ touched   ^ touched   <- gpa values scattered, roll in between

SoA memory (all rolls together, then all gpas together):
[roll0][roll1][roll2][gpa0][gpa1][gpa2]
                       ^^^^^^^^^^^^^^^^  <- gpa values fully contiguous
```

In AoS, each `gpa` value sits next to an unrelated `roll` value; the loop touches `gpa`, skips over `roll`, touches the next `gpa`, and so on — the field you care about is scattered. In SoA, every `gpa` value is packed back-to-back in memory, with nothing unrelated between them, because all `gpa` values live in one dedicated array.

| Property | AoS | SoA |
|---|---|---|
| One full record together in memory | Yes | No |
| One field, across all records, contiguous in memory | No | Yes |
| Best when you usually process | One whole record at a time | One field across every record at a time |
| Adding a new record | Append one struct | Append one value to *every* field array |

## 11. Common Patterns / Idioms

| When you see this problem shape | Reach for this pattern |
|---|---|
| "Represent one real-world entity with several attributes" | A single struct, one instance per entity |
| "Store and process a collection, usually one whole entity at a time" | Array of Structures (Section 8) |
| "Store and process a collection, usually one attribute across all entities at a time" | Structure of Arrays (Section 9) |
| "A function needs to read or produce many values as one unit" | A struct as the function's return type or parameter, not several separate out-parameters |
| "A function must modify a struct, or the struct is large" | Pass a pointer to the struct (Section 7.3), not the struct by value |

## 12. Common Mistakes and How to Debug Them

| Mistake | Symptom | Why it happens | How to catch it |
|---|---|---|---|
| Forgetting `struct` keyword without `typedef` | Compile error: unknown type name | C requires `struct Tag` every time unless a `typedef` alias exists | Read the error location; add `struct` or introduce a `typedef` |
| Using `.` on a pointer, or `->` on a plain struct | Compile error (`.` on pointer) or (`->` on non-pointer) | `.` expects a struct value; `->` expects a pointer to a struct | Check the variable's declared type; pointers use `->` or `(*p).` |
| Forgetting the full-copy cost of pass-by-value | Program runs, but changes inside the function don't appear in the caller's original struct | The function received a copy, not the original | If the function should modify the caller's data, pass a pointer instead |
| Using `==` to compare two structs | Compile error: invalid operands | C has no built-in structural equality for structs, unlike strings (which at least compile with `==`, comparing addresses, giving a *wrong but non-erroring* result) | Compare member-by-member explicitly, or write a comparison function |
| Assuming `sizeof(struct)` equals the sum of member sizes | No compiler error; code that hardcodes or assumes an exact byte size behaves incorrectly or silently reads wrong data (e.g., in manual memory arithmetic or file I/O) | Compiler-inserted alignment padding (Section 6) | Always call `sizeof(struct Type)` directly; never hardcode a computed total |

The last row is the one example in this document of a mistake that produces **no compiler error** but leads to wrong or undefined behavior — it will not stop compilation, and it may not even crash; it can silently corrupt data read from a file or a network buffer that assumed no padding.

## 13. When to Use / When Not To

| Use a struct when... | Reach for something else when... |
|---|---|
| You have several values of different types that describe one real-world entity and always travel together | You have many values of the *same* type — use a plain array instead |
| You want the compiler to enforce that related fields move, copy, and get passed together | The values are genuinely independent and unrelated — separate variables are clearer |
| You are about to define a record type that will be reused across many functions | You need dynamic, resizable collections of records — arrays of structs have fixed size; a resizable structure (not yet taught) is more appropriate |
| You need one struct to reference another instance of its own type (a preview of Linked Lists, not yet taught) | You only need to bundle data briefly within a single function — local variables may be simpler |

**AoS vs. SoA decision rule**, restated plainly: choose AoS when your code usually processes one whole record at a time (print a student, update one student's GPA); choose SoA when your code usually processes one field across every record at a time (sum all GPAs, find the maximum roll number).

## 14. Connection to the Bigger Picture

The struct is the third and final "box" idea in this course's progression: a variable is one labeled box; an array is many same-typed boxes addressed by position; a struct is several boxes, possibly of different types, addressed by name. Together these three ideas cover how C represents data in memory at the level this course teaches.

A struct is also your first real taste of a **programmer-defined type**. Up to now, every type (`int`, `float`, `char`, arrays of those) was built into the language. `struct Student` is a type *you* designed, with a shape and meaning specific to your program — this is the beginning of thinking about data modeling, not just computation.

The AoS/SoA choice in Sections 8–10 is a small, concrete preview of a much larger software engineering idea: **the same data can be laid out more than one way, and the right layout depends on how the data will be accessed, not just on what the data is.** This exact trade-off (contiguity of one field vs. contiguity of one record) reappears, at much larger scale, in database design (row-oriented vs. column-oriented storage) and in performance-critical systems (cache-friendly data layout).

Finally, a struct containing a pointer to another struct of its own type — not covered in depth here — is the missing piece for this course's capstone topic, Linked Lists. Once a struct can point to another instance of itself, you can chain records together without needing a fixed-size array at all.

## 15. Practice Questions

**Question 1 (Trace/Predict Output).**

What does the following program print?

```c
#include <stdio.h>

struct Point {
    int x;
    int y;
};

void shift(struct Point p) {
    p.x += 10;
}

int main(void) {
    struct Point pt = {1, 2};
    shift(pt);
    printf("%d %d\n", pt.x, pt.y);
    return 0;
}
```

*Solution.*
`shift` takes `struct Point p` by value, so `p` inside `shift` is a full copy of `pt`. Modifying `p.x` inside `shift` changes only that copy; `pt` in `main` is untouched. Output:
```
1 2
```

**Question 2 (Apply to a Small Problem).**

Write a program that stores 3 books, each with a title (`char[50]`) and a price (`float`), using an array of structs. Print the title of the most expensive book.

*Solution.*

```c
#include <stdio.h>
#include <string.h>

struct Book {
    char title[50];
    float price;
};

int main(void) {
    struct Book books[3] = {
        {"C Programming", 350.0},
        {"Data Structures", 420.0},
        {"Digital Logic", 300.0}
    };

    int maxIndex = 0;
    for (int i = 1; i < 3; i++) {
        if (books[i].price > books[maxIndex].price) {
            maxIndex = i;
        }
    }

    printf("Most expensive: %s\n", books[maxIndex].title);
    return 0;
}
/* Output:
Most expensive: Data Structures
*/
```

Reasoning: this is Arrays of Structures (Section 8) — one array slot per full record — combined with a standard linear-scan-for-maximum pattern from the loops unit, applied to a struct field instead of a plain array element.

**Question 3 (Debug a Broken Snippet).**

The following code is intended to double a student's GPA using a pointer, but it fails to compile. Find and fix the bug.

```c
#include <stdio.h>

struct Student {
    int roll;
    float gpa;
};

void doubleGpa(struct Student *s) {
    s.gpa = s.gpa * 2;
}

int main(void) {
    struct Student s1 = {101, 4.0};
    doubleGpa(&s1);
    printf("%.1f\n", s1.gpa);
    return 0;
}
```

*Solution.*
The bug is `s.gpa` inside `doubleGpa`. `s` is declared as `struct Student *s` — a pointer, not a struct — so `.` is invalid on it (this matches the second row of the Common Mistakes table, Section 12). It must use `->`, or the explicit dereferenced form.

```c
void doubleGpa(struct Student *s) {
    s->gpa = s->gpa * 2;   /* fixed: use -> on a pointer */
}
```

With the fix, output:
```
8.0
```

**Question 4 (Combine with a Prior Topic).**

Write a function that takes an array of `struct Student` (each with `int roll` and `float gpa`) and its size, and returns the average GPA as a `float`. Use a pointer parameter for the array to avoid copying it, and call the function from `main`.

*Solution.*

```c
#include <stdio.h>

struct Student {
    int roll;
    float gpa;
};

float averageGpa(struct Student *arr, int n) {
    float total = 0;
    for (int i = 0; i < n; i++) {
        total += arr[i].gpa;
    }
    return total / n;
}

int main(void) {
    struct Student class[3] = {{101, 8.0}, {102, 7.0}, {103, 9.0}};
    printf("%.2f\n", averageGpa(class, 3));
    return 0;
}
/* Output:
8.00
*/
```

Reasoning: an array name already decays to a pointer to its first element (prior topic), so `struct Student *arr` correctly receives `class`. Inside the loop, `arr[i].gpa` combines array indexing (prior topic) with struct member access (this topic) — this is the intended fusion of the two ideas, and it also avoids copying the entire array of structs on every call, extending the pass-by-pointer cost argument from Section 7.3 to a whole collection.

**Question 5 (Open-Ended Design Question).**

You are building a system that tracks 100,000 sensor readings, each with a `timestamp`, a `temperature`, and a `sensorId`. Most of your program's processing is: "compute the average temperature across all 100,000 readings, once per minute." Occasionally, a maintenance tool needs to print one full reading (all three fields) for a specific `sensorId`, for debugging. Would you organize this collection as an Array of Structures or a Structure of Arrays? Justify your answer using the concepts from Sections 9 and 10, and state what you are trading away by choosing the layout you pick.

*Solution.*
This should be organized as a Structure of Arrays. The dominant, high-frequency operation is "compute the average temperature across all readings" — scanning one field (`temperature`) across every record — which is exactly the access pattern SoA is built for: all `temperature` values sit contiguously in memory, so that scan touches nothing unrelated (Section 10). Doing this same scan on an Array of Structures would force the CPU to step over `timestamp` and `sensorId` between every `temperature` value it actually needs, since AoS interleaves all three fields per record.

The trade-off being accepted: the occasional debugging task ("print one full reading for a given `sensorId`") becomes slightly less convenient under SoA — you must read `timestamp[i]`, `temperature[i]`, and `sensorId[i]` separately rather than getting one struct back in a single access, as AoS would give directly. Since this operation is rare and not performance-critical (it is a maintenance tool, not the main workload), that cost is acceptable in exchange for making the frequent, large-scale averaging operation efficient. This mirrors the general decision rule from Section 13: layout choice should follow the dominant access pattern, not the less frequent one.

## 16. Summary Table

| Concept | Form / Rule |
|---|---|
| Define a struct type | `struct Tag { type member; ... };` |
| Declare a variable | `struct Tag var;` (or `Tag var;` with `typedef`) |
| Aggregate init | `struct Tag v = {val1, val2, ...};` (positional) |
| Designated init | `struct Tag v = {.member = val, ...};` (order-independent) |
| Access via struct value | `var.member` |
| Access via pointer | `ptr->member`, equivalent to `(*ptr).member` |
| Pass by value | Copies entire struct — costly for large structs |
| Pass by pointer | Copies only the pointer — cheap, allows modification |
| Struct size | `sizeof(struct Tag)` — may exceed sum of member sizes due to padding; never assume, always measure |
| Equality | No `==` for structs; compare members individually |
| AoS | One array of full records — best for whole-record access |
| SoA | One struct of parallel arrays — best for single-field, all-records access |

**Single most important rule:** a struct's members always occupy one contiguous block in declaration order, but the compiler may insert padding for alignment — so member access is by name, not by fixed arithmetic offset you compute yourself, and `sizeof` must always be measured, never assumed.
