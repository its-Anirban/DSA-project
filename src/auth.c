#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "student.h"
#include "auth.h"
#include "csv_handler.h"
#include "admin_menu.h"
#include "stud_menu.h"
#include "utils.h"

typedef struct {
    char username[100];
    char password[100];
    int emp_id;
} Admin;

/* ============ ADMIN LOGIN ============ */
void adminLogin() {
    char username[100];
    char password[100];
    int emp_id;
    int attempts = 3;
    FILE *fp;

    printf("\n------------- ADMIN LOGIN -----------\n");

    while (attempts > 0) {
        printf("Enter Employee ID: ");
        fflush(stdout);
        scanf("%d", &emp_id);
        getchar();

        // Verify employee ID from Admin_Credentials.csv
        fp = fopen("admin_credentials.csv", "r");
        if (!fp) {
            printError("Cannot open admin credentials file!");
            return;
        }

        char line[200];
        int empIdFound = 0;
        int fileEmpId;
        char fileUsername[100], filePassword[100];
        
        // Skip header line
        fgets(line, sizeof(line), fp);
        
        // Check if employee ID exists
        while (fgets(line, sizeof(line), fp)) {
            line[strcspn(line, "\n")] = 0;
            
            if (sscanf(line, "%d,%99[^,],%99[^,]", &fileEmpId, fileUsername, filePassword) == 3) {
                if (emp_id == fileEmpId) {
                    empIdFound = 1;
                    break;
                }
            }
        }
        fclose(fp);

        // Provide immediate feedback on Employee ID
        if (!empIdFound) {
            char msg[150];
            snprintf(msg, sizeof(msg), "Employee ID %d is not found!", emp_id);
            printError(msg);
            attempts--;
            if (attempts > 0) {
                char attemptMsg[50];
                snprintf(attemptMsg, sizeof(attemptMsg), "Attempts left: %d", attempts);
                printWarning(attemptMsg);
            }
            printf("\n");
            continue;
        } else {
            char msg[150];
            snprintf(msg, sizeof(msg), "Employee ID %d verified!", emp_id);
            printSuccess(msg);
        }

        // Now ask for username
        printf("Enter Admin Username: ");
        fflush(stdout);
        scanf(" %99[^\n]", username);
        getchar();

        // Now ask for password
        printf("Enter Password: ");
        fflush(stdout);
        scanf(" %99[^\n]", password);
        getchar();

        // Verify username and password match
        if (strcmp(username, fileUsername) == 0 && strcmp(password, filePassword) == 0) {
            printf("\n");
            printSuccess("Admin Login Successful!");
            printf("\n");
            adminMenu();
            return;
        } else {
            printError("Invalid credentials!");
            attempts--;
            if (attempts > 0) {
                char attemptMsg[50];
                snprintf(attemptMsg, sizeof(attemptMsg), "Attempts left: %d", attempts);
                printWarning(attemptMsg);
            }
            printf("\n");
        }
    }

    printError("Login failed after 3 attempts. Returning to main menu...");
}

/* ============ STUDENT LOGIN ============ */
void studentLogin() {
    int id;
    char name[50];
    char password[20];
    int attempts = 3;

    printf("\n------------- STUDENT LOGIN -----------\n");

    while (attempts > 0) {
        printf("Enter Student Name: ");
        scanf(" %49[^\n]", name);
        getchar();

        // Load applicants and check if name exists
        Applicant a[MAX];
        int n = loadApplicants(a);
        int nameIndex = -1;

        for (int i = 0; i < n; i++) {
            if (strcmp(a[i].name, name) == 0) {
                nameIndex = i;
                break;
            }
        }

        // Provide immediate feedback on name
        if (nameIndex == -1) {
            char msg[150];
            snprintf(msg, sizeof(msg), "Student with name '%s' is not found in the database!", name);
            printError(msg);
            attempts--;
            char attemptMsg[50];
            snprintf(attemptMsg, sizeof(attemptMsg), "Attempts left: %d", attempts);
            printWarning(attemptMsg);
            printf("\n");
            continue;
        } else {
            char msg[150];
            snprintf(msg, sizeof(msg), "Student with name '%s' found!", name);
            printSuccess(msg);
        }

        // Now ask for Application ID
        printf("Enter Application ID: ");
        scanf("%d", &id);
        getchar();

        // Check if this Application ID exists in database
        int idIndex = -1;
        for (int i = 0; i < n; i++) {
            if (a[i].id == id) {
                idIndex = i;
                break;
            }
        }

        // Provide immediate feedback on ID
        if (idIndex == -1) {
            char msg[150];
            snprintf(msg, sizeof(msg), "Application ID %d is not found in the database!", id);
            printError(msg);
            attempts--;
            char attemptMsg[50];
            snprintf(attemptMsg, sizeof(attemptMsg), "Attempts left: %d", attempts);
            printWarning(attemptMsg);
            printf("\n");
            continue;
        } else {
            char msg[150];
            snprintf(msg, sizeof(msg), "Application ID %d found!", id);
            printSuccess(msg);
        }

        // Check if name and ID match
        if (nameIndex != idIndex) {
            printError("Name and Application ID do not match!");
            attempts--;
            char attemptMsg[50];
            snprintf(attemptMsg, sizeof(attemptMsg), "Attempts left: %d", attempts);
            printWarning(attemptMsg);
            printf("\n");
            continue;
        }

        // Now ask for password
        printf("Enter Password: ");
        scanf("%s", password);
        getchar();

        // Check password
        if (strcmp(a[idIndex].password, password) == 0) {
            printf("\n");
            printSuccess("Student Login Successful!");
            printf("\n");
            studentMenu(id);
            return;
        } else {
            printError("Incorrect password!");
            attempts--;
            char attemptMsg[50];
            snprintf(attemptMsg, sizeof(attemptMsg), "Attempts left: %d", attempts);
            printWarning(attemptMsg);
            printf("\n");
        }
    }

    printf("Login failed after 3 attempts. Returning to main menu...\n");
}

/* ============ STUDENT REGISTRATION ============ */
void studentRegistration() {
    Applicant newStudent;
    Applicant a[MAX];
    int n = loadApplicants(a);
    int nextId = 1000;

    // Calculate next available ID starting from 1000
    if (n > 0) {
        int maxId = 999;
        for (int i = 0; i < n; i++) {
            if (a[i].id >= 1000 && a[i].id > maxId) {
                maxId = a[i].id;
            }
        }
        nextId = (maxId >= 1000) ? maxId + 1 : 1000;
    }

    printf("\n---------- NEW STUDENT REGISTRATION ----------\n");

    newStudent.id = nextId;
    printf("\nYour Application ID: %d\n", newStudent.id);

    printf("Enter Name: ");
    scanf(" %49[^\n]", newStudent.name);
    getchar();

    printf("Enter Password (3-9 characters): ");
    scanf("%s", newStudent.password);
    getchar();

    while (strlen(newStudent.password) < 3 || strlen(newStudent.password) > 9) {
        printf("Password must be 3-9 characters. Enter again: ");
        scanf("%s", newStudent.password);
        getchar();
    }

    printf("Enter Category (GEN/OBC/SC/ST): ");
    scanf("%s", newStudent.category);
    getchar();

    printf("Enter JEE Rank: ");
    scanf("%d", &newStudent.jee_rank);
    getchar();

    printf("Enter HS Marks: ");
    scanf("%d", &newStudent.marks);
    getchar();

    printf("\nNow choose department preferences:\n\n");
    printf("Available Departments:\n");
    printf("1. CSE\n");
    printf("2. IT\n");
    printf("3. TT\n");
    printf("4. APM\n\n");
    printf("Enter preference order (space-separated, e.g., CSE IT TT APM):\n");

    for (int i = 0; i < PREF_COUNT; i++) {
        printf("Preference %d: ", i + 1);
        scanf("%s", newStudent.pref[i]);
        getchar();
    }

    strcpy(newStudent.department, "NA");
    newStudent.allocated = 0;

    // Save to CSV
    a[n] = newStudent;
    saveApplicants(a, n + 1);

    printf("\n========================================\n");
    printSuccess("Registration Successful!");
    printf("Your Application ID: %d\n", newStudent.id);
    printf("Please note your ID for login.\n");
    printf("========================================\n\n");
}
