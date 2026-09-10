# Arrays in C

## 1. Learning Objectives

By the end of this document, you should be able to:

- Explain why arrays exist and what problem they solve compared to using separate variables.
- Predict the memory address of any element of a 1D or 2D array given its base address.
- Correctly declare and initialize arrays using all standard forms.
- Explain why an array parameter inside a function behaves like a pointer, and why its size must be passed separately.
- Identify and fix the most common array bugs: off-by-one errors, uninitialized elements, and out-of-bounds access.
- Decide, given a problem description, whether a plain array is the right data structure or not.

## 2. Why Arrays Exist

Suppose you need to store the marks of 5 students and compute their average, using only tools you already know (variables, loops, functions):

```c
int m1 = 78, m2 = 85, m3 = 62, m4 = 90, m5 = 74;
int sum = m1 + m2 + m3 + m4 + m5;
printf("Average = %.2f\n", sum / 5.0);
```

This works for 5 students. It breaks down for 100 students, not because typing 100 variable names is merely tedious, but because of a structural limitation: **a loop needs a way to ask for "the i-th value," and separate variables have no numeric relationship connecting them.** `m1`, `m2`, `m3` are just names to the compiler — there is no way to compute `m1` from `m2` the way you can compute `arr[1]` from `arr[0]`.

An array solves this by giving a group of values of the same type:
- one shared name,
- a fixed set of memory slots,
- and a numeric index that lets you compute which slot you mean.

```c
int marks[5] = {78, 85, 62, 90, 74};
int sum = 0;

for (int i = 0; i < 5; i++) {
    sum += marks[i];
}

printf("Average = %.2f\n", sum / 5.0);
```

The loop now scales to 100 or 10,000 students by changing one number. This is the core value of an array: it turns "a group of related values" into something a loop can walk through.

## 3. Mental Model

Picture a row of lockers, numbered consecutively, all the same size, standing side by side with no gaps between them. Each locker holds exactly one value of the same type. The row has one fixed starting position — the address of the first locker — and every other locker's position can be computed by counting forward from it.

This is precisely what an array is in memory: a contiguous block, divided into equal-sized slots, starting at one fixed base address.

For `int arr[5] = {10, 20, 30, 40, 50};` on a machine where `int` is 4 bytes, if the base address happens to be `2000`, the layout looks like:

| Index | Address | Value |
|---|---|---|
| 0 | 2000 | 10 |
| 1 | 2004 | 20 |
| 2 | 2008 | 30 |
| 3 | 2012 | 40 |
| 4 | 2016 | 50 |

Every fact about arrays covered in this document — indexing, pointer arithmetic, 2D layout, why bounds aren't checked — follows directly from this one picture: **a starting address, plus equal-sized slots, plus a formula to get from index to address.**

## 4. Syntax and Core Mechanics

| Form | Example | Behavior |
|---|---|---|
| Sized declaration, no initializer | `int arr[5];` | Reserves 5 slots; values are indeterminate (garbage) until assigned |
| Sized, fully initialized | `int arr[5] = {1, 2, 3, 4, 5};` | Each index gets the corresponding listed value |
| Unsized, initializer given | `int arr[] = {1, 2, 3, 4, 5};` | Compiler counts the initializer list and sets size to 5 |
| Partially initialized | `int arr[5] = {1, 2};` | Index 0 and 1 set to 1 and 2; indices 2, 3, 4 automatically set to 0 |
| Designated initializer (C99) | `int arr[5] = {[2] = 9, [4] = 7};` | Index 2 set to 9, index 4 set to 7; all other indices default to 0 |

Two rules follow directly from the mental model in Section 3:

- **Valid indices always run from `0` to `size - 1`.** There is no locker numbered `size`.
- **All elements of an array share one type**, because the "equal-sized slots" assumption is what makes the address formula (Section 5) work at all. An array cannot mix `int` and `float` slots.

## 5. How It Works Underneath

### Address Formula (1D)

Given a base address and an element size, the address of any index is:

```
address(i) = base_address + i * sizeof(type)
```

For `int arr[5]` with base address `2000` and `sizeof(int) = 4`:

```
address(3) = 2000 + 3 * 4 = 2012
```

This matches the table in Section 3. This formula is also *why* C indexes from 0 rather than 1: index 0 must map to the base address itself with no offset. If indexing started at 1, every access would need an extra subtraction, for no benefit.

### Address Formula (2D, Row-Major Layout)

A 2D array `int matrix[rows][cols]` is not stored as a grid — it is stored as one long contiguous block, row after row (this is called **row-major order**). The address of `matrix[i][j]` is:

```
address(i, j) = base_address + (i * ncols + j) * sizeof(type)
```

For `int matrix[2][3]` with base address `3000` and `sizeof(int) = 4`, `matrix[1][2]` sits at:

```
address(1, 2) = 3000 + (1 * 3 + 2) * 4 = 3000 + 20 = 3020
```

The memory itself has no notion of "rows" and "columns" — that structure exists only in how you compute the offset. This is a recurring idea in C: higher-level structure is a convention applied to flat memory, not something the hardware enforces.

## 6. Worked Examples

### Example 1: Basic Declaration, Traversal, Aggregation

```c
#include <stdio.h>

int main(void) {
    int marks[5] = {78, 85, 62, 90, 74};
    int sum = 0;

    for (int i = 0; i < 5; i++) {
        sum += marks[i];
    }

    printf("Sum = %d\n", sum);
    printf("Average = %.2f\n", sum / 5.0);
    return 0;
}
```

Output:
```
Sum = 389
Average = 77.80
```

### Example 2: Confirming Contiguity by Printing Addresses

```c
#include <stdio.h>

int main(void) {
    int arr[5] = {10, 20, 30, 40, 50};

    for (int i = 0; i < 5; i++) {
        printf("arr[%d] = %d, address = %p\n", i, arr[i], (void *)&arr[i]);
    }
    return 0;
}
```

Output (actual addresses vary by machine and run; the pattern does not):
```
arr[0] = 10, address = 0x7ffee4a1c020
arr[1] = 20, address = 0x7ffee4a1c024
arr[2] = 30, address = 0x7ffee4a1c028
arr[3] = 40, address = 0x7ffee4a1c02c
arr[4] = 50, address = 0x7ffee4a1c030
```

Each address increases by exactly 4 — `sizeof(int)` — confirming the model in Section 3 is not just a metaphor.

### Example 3: Passing Arrays to Functions (and Why `sizeof` Changes)

```c
#include <stdio.h>

double average(int a[], int n) {
    int sum = 0;
    for (int i = 0; i < n; i++) {
        sum += a[i];
    }
    return sum / (double)n;
}

void checkSize(int a[]) {
    printf("Inside function, sizeof(a) = %zu\n", sizeof(a));
}

int main(void) {
    int marks[5] = {78, 85, 62, 90, 74};

    printf("Inside main, sizeof(marks) = %zu\n", sizeof(marks));
    checkSize(marks);
    printf("Average = %.2f\n", average(marks, 5));
    return 0;
}
```

Output (typical 64-bit system):
```
Inside main, sizeof(marks) = 20
Inside function, sizeof(a) = 8
Average = 77.80
```

Inside `main`, `marks` is the actual array (20 bytes: 5 × 4). The moment it is passed to a function, it **decays** into a pointer to its first element — inside `checkSize`, `sizeof(a)` gives the size of a pointer (8 bytes on a 64-bit system), not the array. This is why `average` must receive `n` as a separate argument; the array parameter alone cannot tell the function how many elements it has.

### Example 4: Array Indexing Is Pointer Arithmetic

```c
#include <stdio.h>

int main(void) {
    int arr[4] = {10, 20, 30, 40};

    for (int i = 0; i < 4; i++) {
        printf("arr[%d] = %d, *(arr + %d) = %d\n", i, arr[i], i, *(arr + i));
    }
    return 0;
}
```

Output:
```
arr[0] = 10, *(arr + 0) = 10
arr[1] = 20, *(arr + 1) = 20
arr[2] = 30, *(arr + 2) = 30
arr[3] = 40, *(arr + 3) = 40
```

`arr[i]` is not special syntax — it is defined to mean `*(arr + i)`. This is the same address formula from Section 5, expressed through pointer arithmetic instead of manual multiplication; the compiler already multiplies by `sizeof(type)` for you.

### Example 5: A 2D Array in Practice

```c
#include <stdio.h>

int main(void) {
    int matrix[2][3] = {
        {1, 2, 3},
        {4, 5, 6}
    };

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 3; j++) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
    return 0;
}
```

Output:
```
1 2 3 
4 5 6 
```

Internally this is one flat block `[1, 2, 3, 4, 5, 6]` — the nested loop and `[i][j]` syntax are the convention that reimposes row/column structure on it, exactly as described in Section 5.

## 7. Common Patterns / Idioms

| Pattern | Shape of the Solution | Typical Use |
|---|---|---|
| Linear search | Loop through indices, compare each element, return index or -1 | Finding whether a value exists, and where |
| Min / Max | Initialize a candidate to `arr[0]`, loop from index 1, replace candidate when a better value is found | Finding the largest/smallest value |
| Sum / Average | Accumulate into a running total inside a loop, divide once after the loop ends | Any aggregate statistic |
| Reverse in place | Swap element at `start` with element at `end`, move both inward until they meet | Reversing order without extra memory |
| Frequency counting | Use a second array indexed by *value* (not position) to tally occurrences | Counting how often each value appears |

```c
int search(int a[], int n, int target) {
    for (int i = 0; i < n; i++) {
        if (a[i] == target) {
            return i;
        }
    }
    return -1;
}

int findMax(int a[], int n) {
    int max = a[0];
    for (int i = 1; i < n; i++) {
        if (a[i] > max) {
            max = a[i];
        }
    }
    return max;
}

void reverse(int a[], int n) {
    int start = 0, end = n - 1;
    while (start < end) {
        int temp = a[start];
        a[start] = a[end];
        a[end] = temp;
        start++;
        end--;
    }
}

void countFrequency(int a[], int n) {
    int freq[10] = {0};
    for (int i = 0; i < n; i++) {
        freq[a[i]]++;
    }
    for (int d = 0; d < 10; d++) {
        if (freq[d] > 0) {
            printf("%d occurs %d time(s)\n", d, freq[d]);
        }
    }
}
```

Nearly every array problem you encounter in this course is a variation of one of these five shapes.

## 8. Common Mistakes and How to Debug Them

| Mistake | Symptom | Why It Happens | How to Catch It |
|---|---|---|---|
| Off-by-one loop bound (`i <= n` instead of `i < n`) | Crash, garbage output, or silent corruption | Valid indices run `0` to `n-1`; `<=` reads/writes one slot past the array | Always pair size `n` with the condition `i < n`; check this every time you write a loop |
| Assuming uninitialized elements are zero | Garbage values appear unpredictably | Local arrays are not auto-initialized in C; only global/static arrays default to 0 | Explicitly initialize with `= {0}` or fill with a loop before reading |
| Using `sizeof(a) / sizeof(a[0])` inside a function that received the array | Element count computes to 1 or 2, not the real size | The array parameter has decayed to a pointer; `sizeof` on a pointer gives the pointer's size | Always pass the element count as a separate function argument |
| Confusing array size with the last valid index | Off-by-one read/write | Size `n` means indices `0` through `n-1`; the last valid index is `n-1`, not `n` | Say the range out loud: "valid indices are 0 to size minus 1" |
| Writing past array bounds | No compiler error; program appears to work, then fails elsewhere or crashes later | C performs no bounds checking; out-of-bounds access is undefined behavior, not a caught error | Compile with `-Wall`, and mentally verify every index expression before running |

Notice that the last row has no compiler safety net at all — this is not an oversight, it is discussed directly in the next section.

## 9. When to Use / When Not To

| Use a plain array when... | Look elsewhere when... |
|---|---|
| The number of elements is known before the program runs (or a safe upper bound is known) | The number of elements is unknown in advance, or must grow/shrink during execution |
| All elements are the same type | You need to mix types together |
| Access is mostly "give me element at position i" | Access mostly involves inserting or deleting elements in the middle |
| You need the fastest possible, most predictable access pattern | You need to add/remove often and shifting all following elements would be wasteful |

C does not stop you from writing past the end of an array (Section 8) because arrays are deliberately a thin, minimal abstraction — a fixed block of memory with an index formula, nothing more. This is a genuine trade-off, not a flaw: it makes arrays extremely fast and predictable, at the cost of giving the programmer full responsibility for staying in bounds and for the array's size being fixed at compile time. When a program's data needs to grow arbitrarily at runtime, arrays alone are not the tool — that requires requesting memory dynamically while the program is running, a mechanism introduced later in this course.

## 10. Connection to the Bigger Picture

An array is the first real **data structure** you learn, and it teaches a habit that runs through all of software engineering: **the right way to store data depends on how you plan to access it.** Arrays are excellent when access is "by position, known size, mostly sequential," and poor when access is "size unknown, frequent insertion/removal." Every more advanced data structure you will meet later — dynamic arrays, linked lists, stacks, queues, hash tables — exists because some access pattern makes plain arrays a bad fit, and each one makes a different trade-off between speed, memory use, and flexibility.

Two concrete threads worth noticing now:
- A C string is, underneath, just a `char` array with a convention (a `'\0'` terminator marking the end) layered on top of it — the same "raw memory plus convention" idea seen in Section 5's 2D layout.
- Fixed-capacity structures like stacks and queues are frequently *built* using a plain array as their underlying storage, with extra logic added on top to manage which slots are "in use."

The broader lesson: an array is not just a C feature to memorize — it is your first exposure to the engineering question "what shape does my data need, and what does that decision cost me?"

## 11. Practice Questions

### Question 1 (Trace / Predict Output)

What does the following program print?

```c
#include <stdio.h>

int main(void) {
    int arr[5] = {2, 4, 6, 8, 10};
    int i;

    for (i = 1; i < 5; i++) {
        arr[i] = arr[i] + arr[i - 1];
    }

    for (i = 0; i < 5; i++) {
        printf("%d ", arr[i]);
    }
    return 0;
}
```

**Solution.** Trace the array after each iteration of the first loop:

| Step | Operation | Array State |
|---|---|---|
| Start | — | 2, 4, 6, 8, 10 |
| i = 1 | arr[1] = 4 + 2 | 2, 6, 6, 8, 10 |
| i = 2 | arr[2] = 6 + 6 | 2, 6, 12, 8, 10 |
| i = 3 | arr[3] = 8 + 12 | 2, 6, 12, 20, 10 |
| i = 4 | arr[4] = 10 + 20 | 2, 6, 12, 20, 30 |

Output: `2 6 12 20 30`

Each element becomes the running total of everything before it — this pattern is called a prefix sum.

### Question 2 (Write a Program)

Write a C program that reads temperatures for 7 days of a week into an array, then prints how many of those days had a temperature above the weekly average.

**Solution.**

```c
#include <stdio.h>

int main(void) {
    int temp[7];
    int sum = 0;

    printf("Enter temperatures for 7 days:\n");
    for (int i = 0; i < 7; i++) {
        scanf("%d", &temp[i]);
        sum += temp[i];
    }

    double average = sum / 7.0;
    int countAbove = 0;

    for (int i = 0; i < 7; i++) {
        if (temp[i] > average) {
            countAbove++;
        }
    }

    printf("Average temperature = %.2f\n", average);
    printf("Days above average = %d\n", countAbove);
    return 0;
}
```

**Reasoning.** This needs two separate passes over the array: the average cannot be known until *all* values have been read, so the comparison pass must happen strictly after the sum is fully computed, not combined into the same loop.

### Question 3 (Debug)

The following program is meant to sum 5 scores but produces an incorrect (and inconsistent) total. Find and fix the bug.

```c
#include <stdio.h>

int main(void) {
    int scores[5] = {55, 67, 72, 84, 91};
    int total = 0;

    for (int i = 0; i <= 5; i++) {
        total += scores[i];
    }

    printf("Total = %d\n", total);
    return 0;
}
```

**Solution.** The loop condition `i <= 5` allows `i` to reach `5`, but valid indices for `scores[5]` only run from `0` to `4`. `scores[5]` reads one slot past the end of the array — memory that does not belong to this array — which is undefined behavior (this is the off-by-one mistake from Section 8, row 1). The fix:

```c
for (int i = 0; i < 5; i++) {
    total += scores[i];
}
```

With this fix, `Total = 369` consistently.

### Question 4 (Combine with Functions and Loops)

Write a function `int countEven(int arr[], int n)` that returns how many elements of the array are even. In `main`, declare an array of 10 integers, fill index `i` with the value `i * i` using a loop, then call `countEven` and print the result.

**Solution.**

```c
#include <stdio.h>

int countEven(int arr[], int n) {
    int count = 0;
    for (int i = 0; i < n; i++) {
        if (arr[i] % 2 == 0) {
            count++;
        }
    }
    return count;
}

int main(void) {
    int squares[10];

    for (int i = 0; i < 10; i++) {
        squares[i] = i * i;
    }

    int evenCount = countEven(squares, 10);
    printf("Number of even squares = %d\n", evenCount);
    return 0;
}
```

Output:
```
Number of even squares = 5
```

The squares of 0–9 are `0, 1, 4, 9, 16, 25, 36, 49, 64, 81`; the even ones are `0, 4, 16, 36, 64` — five values.

### Question 5 (Design / Judgement — No Single Code Answer)

A shop owner wants to record how many orders were placed on each day of a fixed 30-day month, then find the busiest day at month's end. Would a plain C array be a good fit for this? Justify your answer using the criteria from Section 9. Then suppose the owner instead wants to keep an open-ended, ever-growing daily log across many months, with no fixed end date. Would an array still be the right tool? Explain.

**Discussion.** For the 30-day case, a plain array fits well: the size (30) is known in advance, every entry is the same type (a count), access is by straightforward position (day 1 → index 0, and so on), and the only operations needed are filling values in sequence and scanning once for the maximum — no insertions or deletions in the middle are required. This matches every criterion in the "use a plain array" column of Section 9.

For the open-ended, ever-growing log, a plain array is a poor fit. Its size must be fixed at compile time (Section 9); an unbounded log has no such fixed size. Choosing a large fixed size in advance either wastes memory (if the log stays short) or eventually runs out of space (if it grows past the chosen bound). This is precisely the limitation named in Section 9 — a structure that can request more memory as needed, at runtime, is the appropriate tool here, and is covered later in this course.

## 12. Summary Table

| Aspect | Detail |
|---|---|
| Declaration | `type name[size];` |
| Initialization | `type name[size] = {v0, v1, ...};` or `type name[] = {v0, v1, ...};` |
| Valid index range | `0` to `size - 1` |
| Address formula (1D) | `address(i) = base + i * sizeof(type)` |
| Address formula (2D, row-major) | `address(i, j) = base + (i * ncols + j) * sizeof(type)` |
| Indexing/pointer equivalence | `arr[i]` is defined as `*(arr + i)` |
| Behavior when passed to a function | Decays to a pointer to the first element; size is lost and must be passed separately |
| Bounds checking | None — out-of-bounds access is undefined behavior, not a caught error |
| Most important rule | Know your valid index range, `[0, size - 1]`, at all times — nearly every bug in this document traces back to violating it |
