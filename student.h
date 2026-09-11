#ifndef STUDENT_H
#define STUDENT_H

#define MAX_STUDENTS 50
#define MAX_DAYS 7
#define NAME_LEN 30

/* ---------- CRUD ---------- */
int  addStudent(int roll[], char names[][NAME_LEN], int marks[], int *count);
void viewStudents(int roll[], char names[][NAME_LEN], int marks[], int count);
int  updateMarks(int roll[], int marks[], int count);
int  deleteStudent(int roll[], char names[][NAME_LEN], int marks[],
                    int attendance[][MAX_DAYS], int *count);

/* ---------- Search ---------- */
int findStudentIndex(int roll[], int count, int searchRoll);
void searchStudent(int roll[], char names[][NAME_LEN], int marks[], int count);

/* ---------- Sort ---------- */
void sortByMarks(int roll[], char names[][NAME_LEN], int marks[], int count);

/* ---------- Stats ---------- */
void showStats(int marks[], int count);

/* ---------- Grades ---------- */
char calculateGrade(int mark);

/* ---------- Attendance (2D array) ---------- */
void markAttendance(int roll[], int attendance[][MAX_DAYS], int count);
void viewAttendance(int roll[], char names[][NAME_LEN],
                     int attendance[][MAX_DAYS], int count);

#endif
