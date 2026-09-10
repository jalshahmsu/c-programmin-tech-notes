# Pointers in C

*This lecture builds directly on Arrays in C. The address-formula mental model introduced there (`address(i) = base + i * sizeof(type)`) is reused and generalized throughout this document, not re-derived from scratch.*

## 1. Learning Objectives

By the end of this document, you should be able to:

- Explain the difference between a variable's value and a variable's address, and read `&` and `*` correctly in both declarations and expressions.
- Predict what a broken pass-by-value function will (and won't) change, and fix it using a pointer parameter.
- Compute the result of pointer arithmetic (`p + i`) using the same address formula learned for arrays.
- State precisely how an array differs from a pointer, even though an array decays into one.
- Allocate and free memory at runtime using `malloc` and `free`, and explain what "ownership" of allocated memory means.
- Identify the most common pointer bugs — wild pointers, NULL dereference, leaks, dangling pointers, double free — from a code snippet.
- Decide, given a scenario, whether a pointer is the right tool or whether a plain value/array is simpler and safer.

## 2. Why Pointers Exist

Consider a function meant to increment a variable:

```c
#include <stdio.h>

void increment(int x) {
    x = x + 1;
}

int main(void) {
    int value = 5;
    increment(value);
    printf("value = %d\n", value);
    return 0;
}
```

Output:
```
value = 5
```

The value did not change. `increment` received a **copy** of `value` — everything in C is passed by value — so modifying `x` inside the function has no effect on `value` in `main`. This is not a bug in the code; it is how function calls work, and it means a function can never modify a caller's plain variable directly.

A pointer solves this by letting a function receive the variable's *address* instead of a copy of its value, so it can reach back into the caller's memory and change it directly:

```c
#include <stdio.h>

void increment(int *x) {
    *x = *x + 1;
}

int main(void) {
    int value = 5;
    increment(&value);
    printf("value = %d\n", value);
    return 0;
}
```

Output:
```
value = 6
```

This is the first and most immediate reason pointers exist. Two further reasons are covered later in this document: allocating memory whose size isn't known until the program runs (Section 6, dynamic memory), and avoiding the cost of copying large data every time it's passed to a function (Section 9).

## 3. Mental Model

You already have the core idea from Arrays: every variable lives at some address, and that address can be computed. A pointer takes this one step further: **a pointer is a variable whose own value is an address, rather than ordinary data.**

Continue the locker analogy: an ordinary variable is a locker holding a value. A pointer is a *different* locker, one whose contents happen to be another locker's number, not a piece of data.

| Name | Address | Value |
|---|---|---|
| `value` | 3000 | 5 |
| `p` | 3100 | 3000 |

Here `p` lives at address `3100`, and the value stored inside it is `3000` — the address of `value`. Writing `*p` means "go to the address stored in `p` (3000), and get the value sitting there (5)." Writing `&value` means "give me the address of `value`" — the number `3000` itself.

Everything about pointers in this document is a consequence of this one picture: a pointer's value is somebody else's address.

## 4. Syntax and Core Mechanics

| Form | Example | Meaning |
|---|---|---|
| Pointer declaration | `int *p;` | `p` is a pointer to an `int`; its value is currently garbage (uninitialized) |
| Address-of | `p = &value;` | Assigns `p` the address of `value` |
| Dereference | `*p = 10;` | Assigns `10` to whatever `p` currently points to |
| NULL initialization | `int *p = NULL;` | `p` deliberately points to nothing; safe to check before use |
| Multiple declaration (gotcha) | `int *p, q;` | `p` is a pointer to `int`; `q` is a plain `int`, **not** a pointer |

**Reading rule — "declaration mirrors use":** in `int *p;`, read it as "`*p` is an `int`" — that is, "when I dereference `p`, I get an `int` back." This is exactly how `*p` behaves later in an expression. It also explains the gotcha above: the `*` binds to the variable name, not to the type keyword, so `int *p, q;` only makes `p` a pointer.

**NULL vs. uninitialized pointers:** an uninitialized pointer holds whatever garbage address happened to be left in memory. Dereferencing it is undefined behavior — it may crash immediately, or silently corrupt unrelated memory. `NULL` is a deliberate, checkable value meaning "points to nothing." Always initialize a pointer to either a valid address or `NULL`, and check `if (p != NULL)` before dereferencing one that might not have been set.

## 5. How It Works Underneath

### Pointer Types and Arithmetic Step Size

A pointer's declared type determines how far `p + 1` actually moves in memory, using the same formula from the Arrays lecture:

```
address(p + i) = address(p) + i * sizeof(type)
```

| Pointer type | `sizeof(type)` | `p + 1` moves forward by |
|---|---|---|
| `int *` | 4 bytes (typical) | 4 bytes |
| `char *` | 1 byte | 1 byte |
| `double *` | 8 bytes (typical) | 8 bytes |

This is why pointer types matter: `p + 1` on an `int *` and `p + 1` on a `char *` move by different amounts, even though both are "add one" in the code.

### Formalizing `arr[i] == *(arr + i)`

The Arrays lecture demonstrated this equivalence as a fact. Now it can be explained precisely: when an array is used in most expressions, it **decays** into a pointer to its first element. So `arr + i` computes:

```
address(arr) + i * sizeof(type)
```

— exactly the Arrays address formula — and `*(arr + i)` dereferences that computed address to retrieve the value. `arr[i]` is defined to mean exactly this; it is not separate syntax.

### Pointers vs. Arrays: Overlap and Differences

| Aspect | Array | Pointer |
|---|---|---|
| What it stores | The actual data, contiguously | An address of data stored elsewhere |
| Size | Fixed at compile time, part of its type | Just the size of an address (e.g. 8 bytes on a 64-bit system), regardless of what it points to |
| Reassignable | No — the name always refers to the same memory | Yes — can be pointed at different data over its lifetime |
| `sizeof` behavior | `sizeof(arr)` gives the total array size in bytes | `sizeof(p)` gives only the pointer's own size |
| Relationship | Decays to a pointer to its first element when passed to a function | Can be initialized from an array's decayed pointer |

This directly explains the `sizeof(marks) = 20` vs. `sizeof(a) = 8` result from the Arrays lecture: `marks` in `main` is the real array, but once passed to a function it has already decayed to a plain pointer.

## 6. Worked Examples

### Example 1: Pass-by-Reference — The Classic Swap

```c
#include <stdio.h>

void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

int main(void) {
    int x = 10, y = 20;

    printf("Before swap: x = %d, y = %d\n", x, y);
    swap(&x, &y);
    printf("After swap: x = %d, y = %d\n", x, y);
    return 0;
}
```

Output:
```
Before swap: x = 10, y = 20
After swap: x = 20, y = 10
```

`swap` receives the addresses of `x` and `y`, so `*a` and `*b` refer directly to the caller's variables — this is impossible without pointers, for the reason shown in Section 2.

### Example 2: Pointer Arithmetic

```c
#include <stdio.h>

int main(void) {
    int arr[4] = {10, 20, 30, 40};
    int *p = arr;

    for (int i = 0; i < 4; i++) {
        printf("*(p + %d) = %d, address = %p\n", i, *(p + i), (void *)(p + i));
    }
    return 0;
}
```

Output (illustrative — actual addresses vary by machine):
```
*(p + 0) = 10, address = 0x7ffee4a1c020
*(p + 1) = 20, address = 0x7ffee4a1c024
*(p + 2) = 30, address = 0x7ffee4a1c028
*(p + 3) = 40, address = 0x7ffee4a1c02c
```

Each step advances by 4 bytes — `sizeof(int)` — the same pattern seen with array indexing in the Arrays lecture, now produced through explicit pointer arithmetic instead of `[]`.

### Example 3: Dynamic Memory Allocation

```c
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    int n;
    printf("How many integers? ");
    scanf("%d", &n);

    int *arr = malloc(n * sizeof(int));
    if (arr == NULL) {
        printf("Allocation failed\n");
        return 1;
    }

    for (int i = 0; i < n; i++) {
        arr[i] = i * i;
    }

    for (int i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");

    free(arr);
    return 0;
}
```

Sample output (input `5`):
```
0 1 4 9 16
```

Unlike `int arr[n]` with a fixed compile-time `n`, `malloc(n * sizeof(int))` requests exactly the memory needed based on a value read at runtime — this is what resolves the "fixed size at compile time" limitation named in the Arrays lecture. Every successful `malloc` must be matched with exactly one `free`.

### Example 4: Double Pointers — Allocating Memory Inside a Function

```c
#include <stdio.h>
#include <stdlib.h>

void createArray(int **arr, int n) {
    *arr = malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) {
        (*arr)[i] = i * 2;
    }
}

int main(void) {
    int *numbers = NULL;
    createArray(&numbers, 5);

    for (int i = 0; i < 5; i++) {
        printf("%d ", numbers[i]);
    }
    printf("\n");

    free(numbers);
    return 0;
}
```

Output:
```
0 2 4 6 8
```

`main`'s `numbers` starts as `NULL`. For `createArray` to make `numbers` itself point at newly allocated memory, it needs the *address of the pointer* — `&numbers`, of type `int **`. This is the motivating case for double indirection: a single pointer parameter could only let the function write through an already-existing pointer, not change what the caller's pointer points to.

### Example 5: Pointers and Strings

```c
#include <stdio.h>

int main(void) {
    char *literal = "hello";   /* points to a read-only string literal */
    char array[] = "hello";    /* a modifiable copy, stored in array */

    array[0] = 'H';             /* fine: array owns its own memory */
    printf("%s\n", array);

    /* literal[0] = 'H'; */     /* undefined behavior: modifies a string literal */

    printf("%s\n", literal);
    return 0;
}
```

Output:
```
Hello
hello
```

`array` owns its own writable memory, so modifying it is safe. `literal` points at a read-only string literal; the commented-out line would attempt to modify that literal, which is undefined behavior — it may crash, corrupt other data, or silently do nothing, depending on the platform.

## 7. Common Patterns / Idioms

| Pattern | Shape | Typical Use |
|---|---|---|
| Swap via pointers | Two pointer parameters; dereference to exchange values | Reordering algorithms, sorting |
| Output parameters | Function takes pointer parameter(s), writes results through them, returns a status code | Returning more than one value from a function |
| Dynamically sized array | `malloc` sized by a runtime variable, used through pointer indexing, `free`d when done | Data whose size isn't known until the program runs |
| Allocate-for-caller via double pointer | Pass `&pointer` so the callee can set what the caller's pointer points to | A function needs to hand back newly allocated memory |
| Linked structures (preview — not yet taught) | Each "node" stores data plus a pointer to the next node | Data that needs to grow or shrink one piece at a time — the natural next step once pointers and `malloc` combine |

```c
void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

int divide(int a, int b, int *result) {
    if (b == 0) {
        return 0; /* failure */
    }
    *result = a / b;
    return 1; /* success */
}

int *createArray(int n) {
    return malloc(n * sizeof(int));
}

void createArrayInto(int **arr, int n) {
    *arr = malloc(n * sizeof(int));
}
```

## 8. Common Mistakes and How to Debug Them

| Mistake | Symptom | Why It Happens | How to Catch It |
|---|---|---|---|
| Dereferencing an uninitialized (wild) pointer | Crash, or silent corruption of unrelated memory | The pointer holds a leftover garbage address | Always initialize pointers to `NULL` or a valid address before use |
| Dereferencing `NULL` | Immediate crash (segmentation fault) | `NULL` deliberately points to no valid memory | Check `if (p != NULL)` before dereferencing, especially after `malloc` |
| Forgetting `&` when an address is expected | Compiler warning, or the function silently modifies a copy instead of the caller's variable | Passing `x` instead of `&x` gives a value, not an address | Match the parameter type: if it's `int *`, pass `&x`, not `x` |
| Memory leak | Memory usage grows over time; no error message | `malloc`'d memory was never `free`'d | Pair every `malloc`/`calloc` with exactly one `free`; decide who "owns" the memory |
| Dangling pointer (use-after-free) | Unpredictable behavior — appears to work, then corrupts data or crashes | The pointer still holds the address of memory that has already been freed | Set pointers to `NULL` immediately after `free`; never use a pointer after freeing it |
| Double free | Crash or heap corruption | `free` called twice on the same pointer | Free memory exactly once; setting the pointer to `NULL` after freeing makes a second `free` harmless |

## 9. When to Use / When Not To

| Use a pointer when... | Reconsider when... |
|---|---|
| A function needs to modify the caller's variable | A function only needs to read a value — pass it by value instead |
| The amount of memory needed isn't known until runtime | The size is fixed and known at compile time — a plain array is simpler and safer |
| Passing large data and copying it would be wasteful | The data is small (a single `int`/`float`) — copying is cheap and avoids indirection risk |
| Building a structure that needs to grow or shrink piece by piece | A fixed-size, sequential structure already fits — arrays remain the simpler choice |

The last row points directly at the next step in this course: structures that grow and shrink at runtime (linked lists and similar) are built using exactly the two tools introduced here — pointers and dynamic memory.

## 10. Connection to the Bigger Picture

A pointer is your first encounter with **indirection**: a value whose purpose is to refer to another value, rather than to be data itself. This single idea reappears throughout computing — references in other programming languages, file/socket handles in operating systems, and virtual memory itself (an address the CPU sees that is translated through another layer of addresses before reaching real hardware).

Combined with dynamic memory allocation (Section 6), pointers give you the two ingredients needed to build data structures that arrays alone cannot: structures whose size and shape change while the program is running. Every one of those structures — linked lists, trees, graphs — is, at its core, "some data, plus a pointer to more data." Understanding pointers well is what makes all of them learnable later without relearning this chapter.

## 11. Practice Questions

### Question 1 (Trace / Predict Output)

What does the following program print?

```c
#include <stdio.h>

int main(void) {
    int a = 5, b = 10;
    int *p = &a;
    int *q = &b;

    *p = *p + *q;
    p = q;
    *p = 100;

    printf("a = %d, b = %d\n", a, b);
    return 0;
}
```

**Solution.** Trace step by step:

| Step | Operation | State |
|---|---|---|
| Start | — | `a = 5`, `b = 10`, `p → a`, `q → b` |
| `*p = *p + *q;` | `a = a + b = 5 + 10` | `a = 15`, `b = 10` |
| `p = q;` | `p` now points at `b`, not `a` | `p → b` |
| `*p = 100;` | writes through `p`, which now points at `b` | `b = 100`, `a` unchanged |

Output: `a = 15, b = 100`

The key insight: reassigning `p` (`p = q;`) changes what `p` points to, but does not affect `a`, which `p` no longer refers to.

### Question 2 (Write a Program)

Write a function `void addOne(int *arr, int n)` that increments every element of an array by 1, using pointer arithmetic (`*(arr + i)`) rather than array indexing. Call it from `main` on a hardcoded array of 5 integers and print the result.

**Solution.**

```c
#include <stdio.h>

void addOne(int *arr, int n) {
    for (int i = 0; i < n; i++) {
        *(arr + i) = *(arr + i) + 1;
    }
}

int main(void) {
    int values[5] = {1, 2, 3, 4, 5};

    addOne(values, 5);

    for (int i = 0; i < 5; i++) {
        printf("%d ", values[i]);
    }
    printf("\n");
    return 0;
}
```

Output:
```
2 3 4 5 6
```

**Reasoning.** `addOne` modifies the caller's array directly, with no `&` needed, because an array already decays to a pointer to its first element when passed to a function.

### Question 3 (Debug)

Find and fix the bug in the following program.

```c
#include <stdio.h>

int main(void) {
    int *p;
    *p = 42;
    printf("%d\n", *p);
    return 0;
}
```

**Solution.** `p` is declared but never given a valid address — it is a wild pointer holding a garbage value. `*p = 42;` writes to whatever address that garbage happens to be, which is undefined behavior and will typically crash the program (matches Section 8, row 1). Fix by pointing `p` at real memory before dereferencing it:

```c
#include <stdio.h>

int main(void) {
    int value;
    int *p = &value;

    *p = 42;
    printf("%d\n", *p);
    return 0;
}
```

Output: `42`

### Question 4 (Combine Pointers, Functions, and Dynamic Memory)

Write a function `int *filterEven(int *arr, int n, int *outCount)` that dynamically allocates a new array containing only the even elements of `arr`, sets `*outCount` to the number of even elements found, and returns a pointer to the new array. Call it from `main`, print the filtered array, and free the allocated memory.

**Solution.**

```c
#include <stdio.h>
#include <stdlib.h>

int *filterEven(int *arr, int n, int *outCount) {
    int *result = malloc(n * sizeof(int));
    int count = 0;

    for (int i = 0; i < n; i++) {
        if (arr[i] % 2 == 0) {
            result[count] = arr[i];
            count++;
        }
    }

    *outCount = count;
    return result;
}

int main(void) {
    int values[8] = {3, 8, 15, 20, 7, 4, 11, 6};
    int count;

    int *evens = filterEven(values, 8, &count);

    for (int i = 0; i < count; i++) {
        printf("%d ", evens[i]);
    }
    printf("\n");

    free(evens);
    return 0;
}
```

Output:
```
8 20 4 6
```

This combines an output parameter (`outCount`), a dynamically sized result whose exact length isn't known until the loop finishes, and correct ownership: `main` is responsible for freeing what `filterEven` allocated.

### Question 5 (Design / Judgement — No Single Code Answer)

A function needs to process a large struct (say, one holding a 1000-element array) many times inside a loop. A colleague suggests passing the struct by value each call, since "it's simpler, and pointers seem risky." Using the criteria from Section 9, evaluate this suggestion and recommend an approach.

**Discussion.** Passing the struct by value copies its entire contents on every call — for a 1000-element array, that is a real, repeated cost in both time and stack space, especially inside a loop where it happens on every iteration. Section 9 names exactly this situation — large data where copying would be wasteful — as a good reason to use a pointer instead of a plain value.

The recommendation: pass a pointer to the struct. If the function only needs to *read* the struct's contents and should not be able to modify the caller's data, use a pointer-to-const (`const struct Data *`, Section 4's `const` forms extended to structs) — this keeps the efficiency of passing an address while preserving the safety of pass-by-value for the caller's data. "Pointers seem risky" is a reasonable instinct, but the risk is manageable with `const` and disciplined ownership, whereas the cost of by-value copying is unavoidable as long as by-value is used.

## 12. Summary Table

| Aspect | Detail |
|---|---|
| Declaration | `type *p;` |
| Address-of | `&variable` — gives the address of `variable` |
| Dereference | `*p` — gives the value stored at the address `p` holds |
| NULL | `int *p = NULL;` — deliberately points to nothing; check before dereferencing |
| Pointer arithmetic | `p + i` computes `address(p) + i * sizeof(type)` — same formula as array indexing |
| Array/pointer relationship | An array decays to a pointer to its first element when passed to a function; unlike a pointer, an array cannot be reassigned |
| `const` forms | `const int *p` (data fixed), `int *const p` (address fixed), `const int *const p` (both fixed) |
| Dynamic memory | `malloc`/`calloc` request memory at runtime; every allocation must be matched with exactly one `free` |
| Most important rule | A pointer must point somewhere valid (or be `NULL`) before you dereference it — nearly every pointer bug traces back to violating this |
