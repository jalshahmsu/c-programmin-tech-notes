# Strings in C

*This topic comes after Pointers — every prior construct (functions, loops, arrays, pointers) is used freely below. Several ideas here explicitly reuse and extend material from the Arrays and Pointers lectures rather than starting over.*

## 1. Learning Objectives

By the end of this document, you should be able to:

- Explain what a string actually is in C — a `char` array or pointer with no built-in length, relying entirely on a `'\0'` terminator to mark its end.
- Correctly declare, initialize, read, and print strings, and explain why `scanf("%s", ...)` needs no `&` while `scanf("%c", &c)` does.
- Distinguish `strlen` from `sizeof` on a string buffer, and explain why they usually give different numbers.
- Explain why `==` cannot compare string content, and use `strcmp` correctly instead.
- Write a manual sentinel-traversal loop to implement a string operation `<string.h>` doesn't provide.
- Choose between `char[]`, `char *`, a library function, and a manual loop for a given string task.

## 2. Why This Convention Exists

The Arrays lecture established a rule every plain-array function has had to work around ever since: an array has no way to know how much of itself is meaningfully filled — that size must be tracked separately and passed alongside it. Text is exactly the situation where this becomes especially awkward, since a name might be `"Al"` or `"Alexandria"` — wildly different lengths held in buffers that are often larger than either.

```c
#include <stdio.h>

void printChars(char arr[], int length) {
    for (int i = 0; i < length; i++) {
        printf("%c", arr[i]);
    }
    printf("\n");
}

int main(void) {
    char name[20] = {'A', 'l', 'i'};  /* only 3 meaningful characters */
    printChars(name, 3);              /* the length has to be tracked and passed separately */
    return 0;
}
```

C's answer for text is a convention, not a new language feature: reserve one special character value, `'\0'` (the null terminator), to mean "nothing meaningful past this point." Anything that walks through the array can then discover where the text ends just by reading through it:

```c
#include <stdio.h>

int main(void) {
    char name[20] = "Ali";  /* the string marks its own end with '\0' */
    printf("%s\n", name);   /* no length is passed - printf reads until '\0' */
    return 0;
}
```

Both print `Ali`, but the second version never needed a separately tracked length at all.

## 3. Mental Model

Reuse the Arrays lecture's row-of-lockers picture directly, with one addition: immediately after the last meaningful character sits one more, hidden locker, holding `'\0'`. A string of `n` visible characters always occupies `n + 1` bytes — the visible text, plus the terminator marking where it stops.

| Index | 0 | 1 | 2 | 3 |
|---|---|---|---|---|
| Content | `'c'` | `'a'` | `'t'` | `'\0'` |

Everything in this document follows from this one fact: a string's length is never stored anywhere — it is *discovered*, by walking forward until this hidden locker is found.

## 4. Syntax and Core Mechanics

| Form | Example | Meaning |
|---|---|---|
| Literal via pointer | `char *greeting = "Hi";` | Points to `'H','i','\0'` in read-only memory (recap: Pointers lecture) |
| Sized array from a literal | `char greeting[10] = "Hi";` | Copies `'H','i','\0'` into a writable buffer; the remaining bytes are zero-filled, exactly as with any partially initialized array (recap: Arrays lecture) |
| Unsized array from a literal | `char greeting[] = "Hi";` | The compiler sizes the array to exactly fit the text plus terminator — 3 bytes |
| Character by character | `char greeting[3] = {'H', 'i', '\0'};` | Each character placed manually, terminator included explicitly |

**A genuine trap in the last row:** only initializing *from a string literal* (`"text"`) implicitly includes the `'\0'`. Building a `char` array from individual character literals (`{'H', 'i'}`) does **not** add one automatically — without explicitly including `'\0'` as the final element, the result is just a two-element `char` array, not a valid C string, and any code that expects a terminator (like `printf("%s", ...)` or `strlen`) will read past its bounds looking for one that isn't guaranteed to be there.

**Reading and printing:**

```c
char name[20];
scanf("%s", name);        /* no & — name already decays to a pointer, exactly like an array does when passed to any function */

char initial;
scanf(" %c", &initial);   /* & required — initial is a plain char, not an array */
```

`%s` reads until the next whitespace character and stops. It performs no bounds checking against the destination buffer's actual size — the same "no bounds checking" rule from the Arrays lecture applies here directly (Section 8).

## 5. How It Works Underneath

**How `strlen` actually works.** `strlen` doesn't have a string's length stored anywhere to look up — it discovers it by walking forward from the given address, one byte at a time, until it finds `'\0'`, then reports how many bytes it passed (not counting the terminator itself). This is exactly the manual loop in Example 4; `strlen` is not fundamentally different from code you could write yourself, only already written and optimized.

**Why `strlen` and `sizeof` usually disagree.** `sizeof` on an array is a compile-time fact about how much memory was reserved — it never looks at the array's contents. `strlen` is a runtime scan of the contents, stopping at the first `'\0'` regardless of how much memory is actually available beyond it. A 50-byte buffer holding `"hello"` reports `sizeof` = 50 and `strlen` = 5 (Example 3) — one describes the container, the other describes what's currently inside it.

**Why `==` compares addresses, not content.** A `char[]` or `char*` behaves in a comparison exactly as any array or pointer did in the Arrays and Pointers lectures: it decays to, or already is, an address. `==` checks whether two values refer to the *same* memory, not whether the characters stored there match. Two separate arrays holding identical text still have different addresses, so `==` reports them as unequal (Example 5) — `strcmp` is needed specifically because it dereferences and compares the actual characters, one at a time, instead of comparing addresses.

## 6. Worked Examples

### Example 1: The Hidden Terminator, Made Visible

```c
#include <stdio.h>

int main(void) {
    char array[] = "cat";

    printf("Visible text: %s\n", array);
    printf("Bytes actually stored (%zu total):\n", sizeof(array));

    for (int i = 0; i < (int) sizeof(array); i++) {
        if (array[i] == '\0') {
            printf("  index %d: '\\0' (the terminator)\n", i);
        } else {
            printf("  index %d: '%c'\n", i, array[i]);
        }
    }

    return 0;
}
```

Output:
```
Visible text: cat
Bytes actually stored (4 total):
  index 0: 'c'
  index 1: 'a'
  index 2: 't'
  index 3: '\0' (the terminator)
```

`"cat"` looks like 3 characters, but it occupies 4 bytes — the hidden locker from Section 3, made concrete.

### Example 2: Reading Strings vs. Reading a Single Character

```c
#include <stdio.h>

int main(void) {
    char name[20];
    char initial;

    printf("Enter your name: ");
    scanf("%s", name);

    printf("Enter your initial: ");
    scanf(" %c", &initial);

    printf("Hello, %s! Initial: %c\n", name, initial);
    return 0;
}
```

Sample run (input `Alice` then `A`):
```
Enter your name: Alice
Enter your initial: A
Hello, Alice! Initial: A
```

`name` needs no `&` — it already decays to a pointer, exactly as an array does when passed to any function. `initial` is a plain `char`, so `&initial` is required, exactly as with any ordinary variable since the Variables lecture.

### Example 3: `strlen` vs. `sizeof`

```c
#include <stdio.h>
#include <string.h>

int main(void) {
    char buffer[50] = "hello";

    printf("strlen(buffer) = %zu\n", strlen(buffer));
    printf("sizeof(buffer) = %zu\n", sizeof(buffer));

    return 0;
}
```

Output:
```
strlen(buffer) = 5
sizeof(buffer) = 50
```

`sizeof` reports the entire declared buffer; `strlen` reports only up to the first `'\0'`. Both are correct — they're answering different questions.

### Example 4: A Manual `strlen`

```c
#include <stdio.h>

int myStrlen(const char *s) {
    int count = 0;
    while (s[count] != '\0') {
        count++;
    }
    return count;
}

int main(void) {
    char word[] = "programming";

    printf("myStrlen(\"%s\") = %d\n", word, myStrlen(word));
    return 0;
}
```

Output:
```
myStrlen("programming") = 11
```

This is precisely what the library's `strlen` does internally: walk forward, counting, until `'\0'` appears. The `const` on the parameter (recap: Pointers lecture) documents that this function only reads the string, never modifies it.

### Example 5: Why `==` Isn't String Comparison

```c
#include <stdio.h>
#include <string.h>

int main(void) {
    char a[] = "cat";
    char b[] = "cat";

    printf("a == b is %d\n", a == b);
    printf("strcmp(a, b) is %d\n", strcmp(a, b));

    return 0;
}
```

Output:
```
a == b is 0
strcmp(a, b) is 0
```

Both lines print `0`, but they mean opposite things. `a` and `b` are two separate arrays; `a == b` compares their addresses, which differ, so the result is `0` — meaning **not equal** (as pointers). `strcmp(a, b)` compares their actual characters and finds them identical, returning `0` — meaning **equal** (as content). Don't let the matching `0` suggest these two lines agree with each other.

### Example 6: Reversing a String in Place

```c
#include <stdio.h>
#include <string.h>

void reverse(char s[]) {
    int start = 0;
    int end = strlen(s) - 1;

    while (start < end) {
        char temp = s[start];
        s[start] = s[end];
        s[end] = temp;
        start++;
        end--;
    }
}

int main(void) {
    char word[] = "hello";

    reverse(word);
    printf("%s\n", word);
    return 0;
}
```

Output:
```
olleh
```

This is the exact two-index reverse pattern from the Arrays lecture — the only difference is that `strlen(s)` finds the boundary here, instead of a fixed `n` passed in separately.

### Example 7: Palindrome Check

```c
#include <stdio.h>
#include <string.h>

int isPalindrome(char s[]) {
    int start = 0;
    int end = strlen(s) - 1;

    while (start < end) {
        if (s[start] != s[end]) {
            return 0;
        }
        start++;
        end--;
    }
    return 1;
}

int main(void) {
    char word1[] = "madam";
    char word2[] = "hello";

    printf("%s is palindrome: %d\n", word1, isPalindrome(word1));
    printf("%s is palindrome: %d\n", word2, isPalindrome(word2));
    return 0;
}
```

Output:
```
madam is palindrome: 1
hello is palindrome: 0
```

The same two-index scan as reversal, but comparing instead of swapping, and stopping early the moment a mismatch is found.

### Example 8: Two Ways to Store Several Strings

```c
#include <stdio.h>

int main(void) {
    char names1[3][10] = {"Amy", "Bo", "Carlos"};
    char *names2[3] = {"Amy", "Bo", "Carlos"};

    for (int i = 0; i < 3; i++) {
        printf("names1[%d] = %s\n", i, names1[i]);
    }
    for (int i = 0; i < 3; i++) {
        printf("names2[%d] = %s\n", i, names2[i]);
    }

    printf("sizeof(names1) = %zu\n", sizeof(names1));
    printf("sizeof(names2) = %zu\n", sizeof(names2));

    return 0;
}
```

Output (pointer size assumed to be 8 bytes, typical on a 64-bit system):
```
names1[0] = Amy
names1[1] = Bo
names1[2] = Carlos
names2[0] = Amy
names2[1] = Bo
names2[2] = Carlos
sizeof(names1) = 30
sizeof(names2) = 24
```

`names1` is a genuine 2D `char` array — 3 rows of 10 bytes each (extending the Arrays lecture's row-major model), fixed and writable, but wasting several bytes on every name shorter than 9 characters. `names2` is an array of 3 pointers (extending the Pointers lecture's pointer-array ideas) — no wasted padding, but each pointer typically points at a read-only literal stored elsewhere.

## 7. Common Patterns / Idioms

| Pattern | Shape | Typical Use |
|---|---|---|
| Manual sentinel traversal | `while (s[i] != '\0') { ...; i++; }` | Any operation `<string.h>` doesn't provide directly |
| Two-index scan | `start`/`end` indices moving toward each other, bounded by `strlen` | Reversal, palindrome checks |
| Safely sized buffer | `char buffer[SIZE];`, sized comfortably larger than expected input | Reading input with `scanf("%s", buffer)` |
| Building a string manually | Write characters into successive indices, then place `'\0'` explicitly at the end | Constructing a string not given as a single literal |
| Fixed small set of labels | `char *options[] = {"Yes", "No", "Maybe"};` | Menus, fixed labels, simple lookup tables |

## 8. Common Mistakes and How to Debug Them

| Mistake | Symptom | Why It Happens | How to Catch It |
|---|---|---|---|
| Forgetting room for `'\0'` | Buffer overflow, garbage characters appended, or a crash | A string of `n` visible characters needs `n + 1` bytes; sizing a buffer to exactly the visible length leaves no room for the terminator | Always allocate at least one byte more than the longest expected text |
| Using `==` to compare string content | Two identical-looking strings compare as "not equal" | `==` on array names or `char*` values compares addresses (Example 5), not the characters stored there | Use `strcmp(a, b) == 0` to test for equal content |
| Buffer overflow from `scanf("%s", ...)`, `strcpy`, or `strcat` | Crash, or silently overwritten nearby variables | None of these check whether the destination is large enough — the same "no bounds checking" rule from the Arrays lecture, applied here | Ensure the destination has enough room before reading or copying; consider a width limit such as `scanf("%19s", buffer)` for a 20-byte buffer |
| Modifying a string literal through a `char *` | Crash or undefined behavior | A literal (`char *p = "cat";`) points to read-only memory (recap: Pointers lecture) | Use `char array[] = "cat";` instead whenever the text needs to change |
| Building a `char` array from individual character literals without `'\0'` | `strlen`/`printf("%s", ...)` reads past the array into unrelated memory | Only string-literal initialization implicitly appends `'\0'`; a list of individual char literals does not | Always include `'\0'` explicitly as the final element, or use a string literal initializer instead |
| Calling `strcpy`/`strcat` into a destination without enough space | Buffer overflow, silently corrupting nearby memory | Both write as many bytes as the source needs, with no check against the destination's actual size | Confirm (or guarantee by sizing) that the destination has room for the full result plus its terminator |

## 9. When to Use `char[]` vs. `char *` vs. Library Functions vs. a Manual Loop

| Situation | Best Fit |
|---|---|
| Fixed text that will never change during the program | `char *` pointing to a literal |
| Text that will be modified, built up, or read into at runtime | `char[]`, sized with room for the longest expected content plus `'\0'` |
| A standard, well-defined operation: length, copy, concatenate, compare | The appropriate `<string.h>` function — don't reinvent these |
| An operation the standard library doesn't provide | A manual loop using the sentinel-traversal pattern |

## 10. Connection to the Bigger Picture

A string is this course's first example of a **self-describing** structure: rather than tracking its length separately and passing it around, as every plain-array function has had to do since the Arrays lecture, it carries its own end marker inline, so anything that walks through it can discover where it ends without being told. This is a genuine design trade-off, not a strictly better idea — self-description costs one extra byte and a linear scan just to *find* the length (`strlen` walks the whole string every time it's called, unlike an array's `sizeof`, which is instant), while explicit length-tracking costs an extra parameter but makes the length available immediately. Both strategies reappear throughout real software, in file formats and data structures that each choose one or the other for the same underlying reason.

Strings are also a preview of the next major topic in this course: once you can imagine bundling several *different* related pieces of data — not just a sequence of the same character type — under one name, you arrive at `struct`, which generalizes exactly this "several typed boxes bundled together" idea beyond the single-array case a string represents.

## 11. Practice Questions

### Question 1 (Trace / Predict Output)

What does the following program print?

```c
#include <stdio.h>
#include <string.h>

int main(void) {
    char word[10] = "cat";

    printf("strlen = %zu\n", strlen(word));
    printf("sizeof = %zu\n", sizeof(word));

    word[3] = 's';

    printf("word = %s\n", word);
    printf("strlen after change = %zu\n", strlen(word));

    return 0;
}
```

**Solution.** `word` is a 10-byte buffer initialized with `"cat"`: indices 0–2 hold `'c','a','t'`, and — because the array is larger than the initializer — every remaining index, including index 3, is zero-filled (`'\0'`), the same partial-initializer rule from the Arrays lecture.

`strlen(word) = 3`, `sizeof(word) = 10`.

`word[3] = 's';` overwrites the terminator at index 3 with `'s'`. The buffer now holds `c, a, t, s, '\0', '\0', ...` — index 4 is still `'\0'` from the original zero-fill, so `strlen` now finds the *next* terminator, at index 4.

Output:
```
strlen = 3
sizeof = 10
word = cats
strlen after change = 4
```

Overwriting a terminator silently extends the "logical" string — this only worked safely here because the buffer happened to already contain another `'\0'` shortly after.

### Question 2 (Write a Program)

Write a function `int countVowels(const char *s)` that counts vowels (`a, e, i, o, u`, case-insensitive) in a string, and call it from `main` on a hardcoded sentence.

**Solution.**

```c
#include <stdio.h>

int countVowels(const char *s) {
    int count = 0;
    int i = 0;

    while (s[i] != '\0') {
        char c = s[i];
        if (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u' ||
            c == 'A' || c == 'E' || c == 'I' || c == 'O' || c == 'U') {
            count++;
        }
        i++;
    }
    return count;
}

int main(void) {
    char sentence[] = "The Quick Brown Fox";

    printf("Vowel count = %d\n", countVowels(sentence));
    return 0;
}
```

Output:
```
Vowel count = 5
```

### Question 3 (Debug)

The following program is meant to build the greeting `"Hello, World!"`, but crashes or produces garbage output. Find and fix the bug.

```c
#include <stdio.h>
#include <string.h>

int main(void) {
    char first[] = "Hello";
    char second[] = ", World!";
    char result[10];

    strcpy(result, first);
    strcat(result, second);

    printf("%s\n", result);
    return 0;
}
```

**Solution.** `result` is only 10 bytes. The final text, `"Hello, World!"`, needs 14 bytes (13 characters plus a terminator) — `strcat` writes past the end of `result`'s actual storage, corrupting nearby memory (Section 8, rows 3 and 6).

Fix by sizing `result` generously enough:

```c
char result[20];
```

Output (fixed):
```
Hello, World!
```

### Question 4 (Combine Arrays of Strings, Functions, and `strcmp`)

Write a function `int findWord(char *words[], int n, char *target)` that searches an array of strings for an exact match to `target`, returning the matching index or `-1`.

**Solution.**

```c
#include <stdio.h>
#include <string.h>

int findWord(char *words[], int n, char *target) {
    for (int i = 0; i < n; i++) {
        if (strcmp(words[i], target) == 0) {
            return i;
        }
    }
    return -1;
}

int main(void) {
    char *fruits[4] = {"apple", "banana", "cherry", "date"};

    int index = findWord(fruits, 4, "cherry");
    printf("Found at index: %d\n", index);

    int missing = findWord(fruits, 4, "grape");
    printf("Found at index: %d\n", missing);

    return 0;
}
```

Output:
```
Found at index: 2
Found at index: -1
```

**Reasoning.** This is the linear-search pattern from the Arrays lecture, adapted with `strcmp` in place of `==`, since these are string comparisons, not numeric ones (Example 5).

### Question 5 (Design / Judgement — No Single Code Answer)

A program needs to store exactly 5 fixed day names, `"Monday"` through `"Friday"`, which never change. The programmer is deciding between `char days[5][10]` and `char *days[5]`. Which would you recommend, and why? Would your answer change if the program needed to let the user rename a day at runtime?

**Discussion.** For text that never changes, `char *days[5]` initialized from literals is the better fit: Section 9 identifies fixed, unchanging text as exactly the case for a pointer to a literal, and it avoids the wasted space `char days[5][10]` would incur padding every name out to a fixed 10-byte row, when the actual names range from 6 to 8 characters.

If the program instead needs to let the user rename a day at runtime, the calculus changes. Each pointer in `char *days[5]` currently points at read-only literal memory — writing into it would repeat the exact mistake from the Pointers lecture's `literal[0] = 'H';` example. `char days[5][10]` (or another writable buffer sized for the longest expected replacement plus its terminator) becomes necessary instead, trading the earlier wasted padding for the ability to safely modify content in place.

## 12. Summary Table

| Aspect | Detail |
|---|---|
| What a string is | A `char` array (or pointer to one) whose meaningful content ends at the first `'\0'` |
| Storage cost | `n` visible characters need `n + 1` bytes — always leave room for the terminator |
| Literal vs. array | `char *p = "text";` — read-only; `char arr[] = "text";` — a writable copy |
| Printing/reading | `%s` for both `printf` and `scanf`; no `&` needed for an array name (it already decays to a pointer) |
| Length | `strlen(s)` counts up to but not including `'\0'`; `sizeof(s)` on an array gives the entire declared buffer size |
| Comparison | Never use `==` for content; use `strcmp(a, b) == 0` |
| Multiple strings | `char names[n][m]` — fixed row width, writable, some wasted space; `char *names[n]` — flexible length, less memory, typically points to read-only literals |
| Most important rule | A string is only as "long" as the distance to its first `'\0'` — everything from `strlen` to `printf("%s", ...)` depends on that terminator being exactly where it should be |
