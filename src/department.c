#include <stdio.h>
#include <string.h>
#include "student.h"
#include "department.h"

char departments[4][5] = {"CSE", "IT", "TT", "APM"};
char departmentNames[4][50] = {
    "Computer Science and Engineering",
    "Information Technology",
    "Textile Technology",
    "Advanced Production Management"
};

/* ============ LIST ALL DEPARTMENTS ============ */
void listDepartments() {
    printf("\n========== DEPARTMENTS ==========\n");
    for (int i = 0; i < 4; i++)
        printf("%d. %s (%s) - Seats: 10\n", i + 1, departments[i], departmentNames[i]);
    printf("=================================\n");
}

/* ============ VALIDATE DEPARTMENT ============ */
int isValidDepartment(char dept[]) {
    for (int i = 0; i < 4; i++)
        if (strcmp(dept, departments[i]) == 0)
            return 1;
    return 0;
}

/* ============ GET DEPARTMENT INDEX ============ */
int getDeptIndex(char dept[]) {
    for (int i = 0; i < 4; i++)
        if (strcmp(dept, departments[i]) == 0)
            return i;
    return -1;
}

/* ============ GET DEPARTMENT NAME ============ */
char* getDeptName(int index) {
    if (index >= 0 && index < 4)
        return departmentNames[index];
    return "Unknown";
}
