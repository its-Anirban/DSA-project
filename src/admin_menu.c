#include <stdio.h>
#include <string.h>
#include "student.h"
#include "admin_menu.h"
#include "applicant_ops.h"
#include "csv_handler.h"
#include "meritlist.h"
#include "utils.h"

/* ============ VIEW ALL APPLICANTS ============ */
void viewAllApplicants() {
    listAllApplicants();
}

/* ============ SEARCH BY ID ============ */
void searchByID() {
    searchApplicantByID();
}

/* ============ SEARCH BY NAME ============ */
void searchByName() {
    searchApplicantByName();
}

/* ============ MAIN ADMIN MENU ============ */
void adminMenu() {
    int ch, sub;

    while (1) {
        printf("\n------------------ ADMIN MENU -------------------\n");
        printf("1. View All Applicants\n");
        printf("2. Search Applicants\n");
        printf("3. View Merit List\n");
        printf("4. View Waiting List\n");
        printf("5. View Category-wise Merit List\n");
        printf("6. View Department-wise Merit List\n");
        printf("7. Logout\n");
        printf("-------------------------------------------------\n");
        printf("Enter your choice: ");
        scanf("%d", &ch);
        getchar();

        switch (ch) {
            case 1:
                viewAllApplicants();
                break;

            case 2:
                printf("\n1 Search by Application ID\n");
                printf("2 Search by Name\n");
                printf("Enter sub-choice: ");
                scanf("%d", &sub);
                getchar();

                if (sub == 1) {
                    searchByID();
                } else if (sub == 2) {
                    searchByName();
                } else {
                    printError("Invalid choice!");
                }
                break;

            case 3:
                viewMeritList();
                break;

            case 4:
                viewWaitingList();
                break;

            case 5:
                printf("\n1 OBC Category\n");
                printf("2 SC Category\n");
                printf("3 ST Category\n");
                printf("Enter sub-choice: ");
                scanf("%d", &sub);
                getchar();

                if (sub >= 1 && sub <= 3) {
                    viewCategoryWiseMeritList(sub);
                } else {
                    printError("Invalid choice!");
                }
                break;

            case 6:
                printf("\n1 CSE\n");
                printf("2 IT\n");
                printf("3 TT\n");
                printf("4 APM\n");
                printf("Enter sub-choice: ");
                scanf("%d", &sub);
                getchar();

                if (sub >= 1 && sub <= 4) {
                    viewDepartmentWiseMeritList(sub);
                } else {
                    printError("Invalid choice!");
                }
                break;

            case 7:
                printf("Logging out from admin menu...\n");
                return;

            default:
                printError("Invalid choice! Please try again.");
        }
    }
}
