#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "student.h"
#include "stud_menu.h"
#include "csv_handler.h"
#include "utils.h"

/* ============ STUDENT MENU ============ */
void studentMenu(long int studentId) {
    int choice;
    Applicant a[MAX];
    int n = loadApplicants(a);
    int studentIndex = -1;

    // Find current student
    for (int i = 0; i < n; i++) {
        if (a[i].id == studentId) {
            studentIndex = i;
            break;
        }
    }

    if (studentIndex == -1) {
        printError("Error: Student record not found.");
        return;
    }

    while (1) {
        printf("\n---------------- STUDENT MENU ----------------\n");
        printf("1. View Profile\n");
        printf("2. View Merit Rank\n");
        printf("3. View Allocation Status\n");
        printf("4. Edit Preferences\n");
        printf("5. Change Password\n");
        printf("6. Logout\n");
        printf("----------------------------------------------\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar();

        switch (choice) {
            case 1: { // View Profile
                Applicant currentStudent = a[studentIndex];
                printf("\n============== YOUR PROFILE ==============\n");
                printf("Application ID: %d\n", currentStudent.id);
                printf("Name: %s\n", currentStudent.name);
                printf("Category: %s\n", currentStudent.category);
                printf("HS Marks: %d\n", currentStudent.marks);
                printf("JEE Rank: %d\n", currentStudent.jee_rank);
                printf("\nDepartment Preferences:\n");
                for (int i = 0; i < PREF_COUNT; i++) {
                    printf("  Preference %d: %s\n", i + 1, currentStudent.pref[i]);
                }
                printf("\nDepartment Allotted: %s\n",
                       strlen(currentStudent.department) > 0 && strcmp(currentStudent.department, "NA") != 0
                           ? currentStudent.department
                           : "Not Allotted");
                printf("=========================================\n");
                break;
            }

            case 2: { // View Merit Rank
                int merit_rank = 1;
                Applicant a_temp[MAX];
                int n_temp = loadApplicants(a_temp);

                // Sort to find position
                for (int i = 0; i < n_temp; i++) {
                    if (a_temp[i].id == studentId) {
                        for (int j = 0; j < n_temp; j++) {
                            if (a_temp[j].jee_rank < a_temp[i].jee_rank) {
                                merit_rank++;
                            } else if (a_temp[j].jee_rank == a_temp[i].jee_rank &&
                                       a_temp[j].marks > a_temp[i].marks &&
                                       a_temp[j].id != studentId) {
                                merit_rank++;
                            }
                        }
                        break;
                    }
                }

                printf("\n========== YOUR MERIT POSITION ==========\n");
                printf("Overall Merit Rank: %d out of %d\n", merit_rank, n_temp);
                printf("JEE Rank: %d\n", a[studentIndex].jee_rank);
                printf("HS Marks: %d\n", a[studentIndex].marks);
                printf("=========================================\n");
                break;
            }

            case 3: { // View Allocation Status
                printf("\n========== ALLOCATION STATUS ==========\n");
                if (a[studentIndex].allocated) {
                    printf("Status: SELECTED\n");
                    printf("Allotted Department: %s\n", a[studentIndex].department);
                } else {
                    printf("Status: NOT ALLOTTED\n");
                    printf("Please wait for merit list generation.\n");
                }
                printf("======================================\n");
                break;
            }

            case 4: { // Edit Preferences
                printf("\n--- EDIT PREFERENCES ---\n");
                printf("Available Departments:\n");
                printf("1. CSE\n");
                printf("2. IT\n");
                printf("3. TT\n");
                printf("4. APM\n\n");

                const char *depts[] = {"CSE", "IT", "TT", "APM"};
                for (int i = 0; i < PREF_COUNT; i++) {
                    int valid = 0;
                    while (!valid) {
                        printf("Enter Preference %d (current: %s) [1-4]: ", i + 1, a[studentIndex].pref[i]);
                        int choice;
                        scanf("%d", &choice);
                        getchar();

                        // Validate department choice
                        if (choice >= 1 && choice <= 4) {
                            strcpy(a[studentIndex].pref[i], depts[choice - 1]);
                            valid = 1;
                        } else {
                            printError("Invalid! Enter 1 for CSE, 2 for IT, 3 for TT, or 4 for APM.");
                        }
                    }
                }

                saveApplicants(a, n);
                printSuccess("Preferences updated successfully!");
                break;
            }

            case 5: { // Change Password
                printf("\n--- CHANGE PASSWORD ---\n");
                char oldPass[20], newPass[20];

                printf("Enter current password: ");
                scanf("%19s", oldPass);
                getchar();

                if (strcmp(oldPass, a[studentIndex].password) == 0) {
                    int validPass = 0;
                    while (!validPass) {
                        printf("Enter new password (3-9 characters): ");
                        scanf("%19s", newPass);
                        getchar();

                        if (strlen(newPass) >= 3 && strlen(newPass) <= 9) {
                            strcpy(a[studentIndex].password, newPass);
                            validPass = 1;
                        } else {
                            printWarning("Password must be 3-9 characters!");
                        }
                    }

                    saveApplicants(a, n);
                    printSuccess("Password changed successfully!");
                } else {
                    printError("Current password is incorrect!");
                }
                break;
            }

            case 6: {
                printf("Logging out from student menu...\n");
                return;
            }

            default:
                printError("Invalid choice! Please try again.");
        }
    }
}
