#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "student.h"
#include "auth.h"
#include "admin_menu.h"
#include "stud_menu.h"
#include "csv_handler.h"
#include "utils.h"

int main() {
    int choice;
    int loopFlag = 1;

    printf("===================================================\n");
    printf("      ADMISSION MANAGEMENT SYSTEM\n");
    printf("===================================================\n\n");

    while (loopFlag) {
        printf("1. Student Portal\n");
        printf("2. Admin Portal\n");
        printf("3. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        clearInputBuffer();

        switch (choice) {
            case 1: {
                // Student Portal
                int studentChoice;
                int studentLoop = 1;
                while (studentLoop) {
                    printf("\n------- STUDENT PORTAL -------\n");
                    printf("1. Student Login\n");
                    printf("2. New Student Registration\n");
                    printf("3. Back to Main Menu\n");
                    printf("Enter your choice: ");
                    scanf("%d", &studentChoice);
                    clearInputBuffer();

                    switch (studentChoice) {
                        case 1:
                            studentLogin();
                            break;
                        case 2:
                            studentRegistration();
                            break;
                        case 3:
                            studentLoop = 0;
                            break;
                        default:
                            printError("Invalid choice! Please try again.");
                    }
                }
                break;
            }
            case 2:
                adminLogin();
                break;
            case 3:
                printf("\n");
                printSuccess("Thank you for using the system. Goodbye!");
                loopFlag = 0;
                break;
            default:
                printError("Invalid choice! Please try again.");
        }
    }

    return 0;
}
