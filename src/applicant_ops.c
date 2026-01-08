#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "student.h"
#include "applicant_ops.h"
#include "csv_handler.h"
#include "utils.h"

/* ============ ADD APPLICANT ============ */
void addApplicant() {
    Applicant a[MAX];
    int n = loadApplicants(a);

    if (n >= MAX) {
        printError("Cannot add more applicants. Database is full.");
        return;
    }

    printf("\n--- ADD APPLICANT ---\n");

    printf("Enter ID: ");
    scanf("%d", &a[n].id);
    getchar();

    printf("Enter Name: ");
    scanf(" %49[^\n]", a[n].name);
    getchar();

    printf("Set Password (3-9 chars): ");
    scanf("%s", a[n].password);
    getchar();

    printf("Enter Category (GEN/OBC/SC/ST): ");
    scanf("%s", a[n].category);
    getchar();

    printf("Enter JEE Rank: ");
    scanf("%d", &a[n].jee_rank);
    getchar();

    printf("Enter HS Marks: ");
    scanf("%d", &a[n].marks);
    getchar();

    printf("Enter Preferences:\n");
    for (int i = 0; i < PREF_COUNT; i++) {
        printf("Preference %d (CSE/IT/TT/APM): ", i + 1);
        scanf("%s", a[n].pref[i]);
        getchar();
    }

    strcpy(a[n].department, "NA");
    a[n].allocated = 0;

    saveApplicants(a, n + 1);
    printSuccess("Applicant Added Successfully");
}

/* ============ EDIT APPLICANT ============ */
void editApplicant() {
    Applicant a[MAX];
    int n = loadApplicants(a);
    int id, found = -1;

    printf("\nEnter Applicant ID to Edit: ");
    scanf("%d", &id);
    getchar();

    for (int i = 0; i < n; i++) {
        if (a[i].id == id) {
            found = i;
            break;
        }
    }

    if (found == -1) {
        printError("Applicant Not Found!");
        return;
    }

    printf("\n--- EDIT APPLICANT ---\n");

    printf("New Name (current: %s): ", a[found].name);
    scanf(" %49[^\n]", a[found].name);
    getchar();

    printf("New Category (current: %s): ", a[found].category);
    scanf("%s", a[found].category);
    getchar();

    printf("New JEE Rank (current: %d): ", a[found].jee_rank);
    scanf("%d", &a[found].jee_rank);
    getchar();

    printf("New HS Marks (current: %d): ", a[found].marks);
    scanf("%d", &a[found].marks);
    getchar();

    printf("Update Preferences:\n");
    for (int i = 0; i < PREF_COUNT; i++) {
        printf("Preference %d (current: %s): ", i + 1, a[found].pref[i]);
        scanf("%s", a[found].pref[i]);
        getchar();
    }

    strcpy(a[found].department, "NA");
    a[found].allocated = 0;

    saveApplicants(a, n);
    printSuccess("Applicant Updated Successfully");
}

/* ============ DELETE APPLICANT ============ */
void deleteApplicant() {
    Applicant a[MAX];
    int n = loadApplicants(a);
    int id, found = -1;

    printf("\nEnter Applicant ID to Delete: ");
    scanf("%d", &id);
    getchar();

    for (int i = 0; i < n; i++) {
        if (a[i].id == id) {
            found = i;
            break;
        }
    }

    if (found == -1) {
        printError("Applicant Not Found!");
        return;
    }

    // Shift remaining records
    for (int i = found; i < n - 1; i++) {
        a[i] = a[i + 1];
    }

    saveApplicants(a, n - 1);
    printSuccess("Applicant Deleted Successfully");
}

/* ============ SEARCH BY ID ============ */
void searchApplicantByID() {
    Applicant a[MAX];
    int n = loadApplicants(a);
    int target;
    int found = 0;

    printf("\nEnter Applicant ID: ");
    scanf("%d", &target);
    getchar();

    for (int i = 0; i < n; i++) {
        if (a[i].id == target) {
            printf("\n--- RECORD FOUND ---\n");
            printf("ID: %d | Name: %s | Category: %s\n", a[i].id, a[i].name, a[i].category);
            printf("Marks: %d | JEE Rank: %d\n", a[i].marks, a[i].jee_rank);
            printf("Department: %s | Status: %s\n", a[i].department,
                   a[i].allocated ? "SELECTED" : "WAITING/NOT ALLOTTED");
            found = 1;
            break;
        }
    }

    if (!found)
        printError("Applicant ID not found.");
}

/* ============ SEARCH BY NAME ============ */
void searchApplicantByName() {
    Applicant a[MAX];
    int n = loadApplicants(a);
    char key[50];
    int found = 0;

    printf("\nEnter Applicant Name (partial match okay): ");
    scanf(" %49[^\n]", key);
    getchar();

    printf("\n--- MATCHING RECORDS ---\n");
    for (int i = 0; i < n; i++) {
        if (strstr(a[i].name, key) != NULL) {
            printf("ID: %d | Name: %s | Category: %s | Marks: %d | JEE Rank: %d | Dept: %s\n",
                   a[i].id, a[i].name, a[i].category, a[i].marks, a[i].jee_rank, a[i].department);
            found = 1;
        }
    }

    if (!found)
        printError("No matching name found.");
}

/* ============ VIEW SINGLE APPLICANT ============ */
void viewSingleApplicant() {
    Applicant a[MAX];
    int n = loadApplicants(a);
    int id;

    printf("\nEnter Applicant ID: ");
    scanf("%d", &id);
    getchar();

    for (int i = 0; i < n; i++) {
        if (a[i].id == id) {
            printf("\n========== FULL APPLICANT DETAILS ==========\n");
            printf("Application ID: %d\n", a[i].id);
            printf("Name: %s\n", a[i].name);
            printf("Category: %s\n", a[i].category);
            printf("HS Marks: %d\n", a[i].marks);
            printf("JEE Rank: %d\n", a[i].jee_rank);
            printf("Department Preferences:\n");
            for (int j = 0; j < PREF_COUNT; j++) {
                printf("  Preference %d: %s\n", j + 1, a[i].pref[j]);
            }
            printf("Allotted Department: %s\n", a[i].department);
            printf("Allocation Status: %s\n", a[i].allocated ? "SELECTED" : "WAITING/NOT ALLOTTED");
            printf("==========================================\n");
            return;
        }
    }

    printError("Applicant not found.");
}

/* ============ LIST ALL APPLICANTS ============ */
void listAllApplicants() {
    Applicant a[MAX];
    int n = loadApplicants(a);

    if (n <= 0) {
        printWarning("No Applicants Found!");
        return;
    }

    printf("\n========== ALL APPLICANTS ==========\n");
    for (int i = 0; i < n; i++) {
        printf("%d. ID: %d | %s | Cat: %s | Marks: %d | Rank: %d | Dept: %s | Status: %s\n",
               i + 1,
               a[i].id,
               a[i].name,
               a[i].category,
               a[i].marks,
               a[i].jee_rank,
               a[i].department,
               a[i].allocated ? "SELECTED" : "WAITING");
    }
    printf("====================================\n");
}
