#include <stdio.h>
#include "student.h"

/* ============ CRUD ============ */

/* CREATE */
int addStudent(int roll[], char names[][NAME_LEN], int marks[], int *count) {
    if (*count >= MAX_STUDENTS) {
        printf("Cannot add more students. Gradebook is full.\n");
        return 0;
    }

    int r, m;
    char n[NAME_LEN];

    printf("Enter roll number: ");
    scanf("%d", &r);
    printf("Enter name: ");
    scanf("%s", n);
    printf("Enter marks (0-100): ");
    scanf("%d", &m);

    roll[*count] = r;

    /* copy name char by char -- plain for loop, no string.h needed */
    int i;
    for (i = 0; n[i] != '\0' && i < NAME_LEN - 1; i++) {
        names[*count][i] = n[i];
    }
    names[*count][i] = '\0';

    marks[*count] = m;
    (*count)++;

    printf("Student added successfully.\n");
    return 1;
}

/* READ (all) */
void viewStudents(int roll[], char names[][NAME_LEN], int marks[], int count) {
    if (count == 0) {
        printf("No students to display.\n");
        return;
    }

    printf("\n%-8s %-20s %-8s %-6s\n", "Roll", "Name", "Marks", "Grade");
    printf("-------------------------------------------\n");

    for (int i = 0; i < count; i++) {
        char grade = calculateGrade(marks[i]);
        printf("%-8d %-20s %-8d %-6c\n", roll[i], names[i], marks[i], grade);
    }
}

/* UPDATE */
int updateMarks(int roll[], int marks[], int count) {
    int searchRoll;
    printf("Enter roll number to update: ");
    scanf("%d", &searchRoll);

    int i = 0;
    int found = 0;

    while (i < count) {
        if (roll[i] == searchRoll) {
            found = 1;
            break;
        }
        i++;
    }

    if (!found) {
        printf("Student not found.\n");
        return 0;
    }

    int newMarks;
    printf("Enter new marks: ");
    scanf("%d", &newMarks);
    marks[i] = newMarks;
    printf("Marks updated.\n");
    return 1;
}

/* DELETE */
int deleteStudent(int roll[], char names[][NAME_LEN], int marks[],
                   int attendance[][MAX_DAYS], int *count) {
    int searchRoll;
    printf("Enter roll number to delete: ");
    scanf("%d", &searchRoll);

    int index = findStudentIndex(roll, *count, searchRoll);

    if (index == -1) {
        printf("Student not found.\n");
        return 0;
    }

    /* shift every later element left by one, including the attendance row */
    for (int i = index; i < *count - 1; i++) {
        roll[i] = roll[i + 1];
        marks[i] = marks[i + 1];

        for (int j = 0; j < NAME_LEN; j++) {
            names[i][j] = names[i + 1][j];
        }
        for (int d = 0; d < MAX_DAYS; d++) {
            attendance[i][d] = attendance[i + 1][d];
        }
    }

    (*count)--;
    printf("Student deleted.\n");
    return 1;
}

/* ============ Search ============ */

/* returns index of roll number, or -1 if not found */
int findStudentIndex(int roll[], int count, int searchRoll) {
    for (int i = 0; i < count; i++) {
        if (roll[i] == searchRoll) {
            return i;
        }
    }
    return -1;
}

void searchStudent(int roll[], char names[][NAME_LEN], int marks[], int count) {
    char again = 'y';

    do {
        int searchRoll;
        printf("Enter roll number to search: ");
        scanf("%d", &searchRoll);

        int index = findStudentIndex(roll, count, searchRoll);

        if (index == -1) {
            printf("Student not found.\n");
        } else {
            printf("Found -> Roll: %d, Name: %s, Marks: %d, Grade: %c\n",
                   roll[index], names[index], marks[index],
                   calculateGrade(marks[index]));
        }

        printf("Search another? (y/n): ");
        scanf(" %c", &again);

    } while (again == 'y' || again == 'Y');
}

/* ============ Sort ============ */

void sortByMarks(int roll[], char names[][NAME_LEN], int marks[], int count) {
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (marks[j] < marks[j + 1]) {   /* descending: highest marks first */
                int tempMark = marks[j];
                marks[j] = marks[j + 1];
                marks[j + 1] = tempMark;

                int tempRoll = roll[j];
                roll[j] = roll[j + 1];
                roll[j + 1] = tempRoll;

                char tempName[NAME_LEN];
                for (int k = 0; k < NAME_LEN; k++) tempName[k] = names[j][k];
                for (int k = 0; k < NAME_LEN; k++) names[j][k] = names[j + 1][k];
                for (int k = 0; k < NAME_LEN; k++) names[j + 1][k] = tempName[k];
            }
        }
    }
    printf("Sorted by marks (highest first).\n");
}

/* ============ Stats ============ */

void showStats(int marks[], int count) {
    if (count == 0) {
        printf("No data available.\n");
        return;
    }

    int total = 0;
    int maxMark = marks[0];
    int minMark = marks[0];
    int passCount = 0;

    for (int i = 0; i < count; i++) {
        total += marks[i];

        if (marks[i] > maxMark) maxMark = marks[i];
        if (marks[i] < minMark) minMark = marks[i];
        if (marks[i] >= 40) passCount++;
    }

    float average = (float) total / count;

    printf("\n--- Class Stats ---\n");
    printf("Average : %.2f\n", average);
    printf("Highest : %d\n", maxMark);
    printf("Lowest  : %d\n", minMark);
    printf("Passed  : %d / %d\n", passCount, count);
}

/* ============ Grades ============ */

char calculateGrade(int mark) {
    char grade;

    switch (mark / 10) {
        case 10:
        case 9:
            grade = 'A';
            break;
        case 8:
        case 7:
            grade = 'B';
            break;
        case 6:
        case 5:
            grade = 'C';
            break;
        case 4:
            grade = 'D';
            break;
        default:
            grade = 'F';
    }
    return grade;
}

/* ============ Attendance (2D array) ============ */

void markAttendance(int roll[], int attendance[][MAX_DAYS], int count) {
    if (count == 0) {
        printf("No students yet.\n");
        return;
    }

    int day;
    printf("Enter day number to mark (1-7): ");
    scanf("%d", &day);

    if (day < 1 || day > MAX_DAYS) {
        printf("Invalid day.\n");
        return;
    }

    for (int i = 0; i < count; i++) {
        int present;
        printf("Roll %d present today? (1 = yes, 0 = no): ", roll[i]);
        scanf("%d", &present);
        attendance[i][day - 1] = present;
    }
    printf("Attendance marked for day %d.\n", day);
}

void viewAttendance(int roll[], char names[][NAME_LEN],
                     int attendance[][MAX_DAYS], int count) {
    if (count == 0) {
        printf("No students yet.\n");
        return;
    }

    printf("\n%-8s %-20s %-12s\n", "Roll", "Name", "Attendance %%");
    printf("---------------------------------------\n");

    for (int i = 0; i < count; i++) {
        int daysPresent = 0;

        for (int d = 0; d < MAX_DAYS; d++) {
            daysPresent += attendance[i][d];
        }

        float percent = (float) daysPresent / MAX_DAYS * 100;
        printf("%-8d %-20s %.1f%%\n", roll[i], names[i], percent);
    }
}
