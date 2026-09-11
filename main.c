#include <stdio.h>
#include "student.h"

int main() {
    /* ---------- shared state, owned by main, passed to every function ---------- */
    int roll[MAX_STUDENTS];
    char names[MAX_STUDENTS][NAME_LEN];
    int marks[MAX_STUDENTS];
    int attendance[MAX_STUDENTS][MAX_DAYS] = {0};   /* 2D array */
    int count = 0;

    int choice;

    do {
        printf("\n========== GRADEBOOK MENU ==========\n");
        printf("1. Add Student\n");
        printf("2. View All Students\n");
        printf("3. Update Marks\n");
        printf("4. Delete Student\n");
        printf("5. Search Student\n");
        printf("6. Sort by Marks\n");
        printf("7. Show Class Stats\n");
        printf("8. Mark Attendance (today)\n");
        printf("9. View Attendance %%\n");
        printf("0. Exit\n");
        printf("=====================================\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                addStudent(roll, names, marks, &count);
                break;
            case 2:
                viewStudents(roll, names, marks, count);
                break;
            case 3:
                updateMarks(roll, marks, count);
                break;
            case 4:
                deleteStudent(roll, names, marks, attendance, &count);
                break;
            case 5:
                searchStudent(roll, names, marks, count);
                break;
            case 6:
                sortByMarks(roll, names, marks, count);
                break;
            case 7:
                showStats(marks, count);
                break;
            case 8:
                markAttendance(roll, attendance, count);
                break;
            case 9:
                viewAttendance(roll, names, attendance, count);
                break;
            case 0:
                printf("Exiting. Goodbye!\n");
                break;
            default:
                printf("Invalid choice. Try again.\n");
        }

    } while (choice != 0);

    return 0;
}
