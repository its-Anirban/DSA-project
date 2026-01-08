#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "utils.h"

/* ============ CHECK IF STRING IS VALID INTEGER ============ */
int isValidInteger(char str[]) {
    if (str == NULL || strlen(str) == 0)
        return 0;

    for (int i = 0; str[i] != '\0'; i++) {
        if (!isdigit(str[i]))
            return 0;
    }
    return 1;
}

/* ============ CHECK IF STRING CONTAINS ONLY LETTERS ============ */
int isValidLetters(char str[]) {
    if (str == NULL || strlen(str) == 0)
        return 0;

    for (int i = 0; str[i] != '\0'; i++) {
        if (!isalpha(str[i]) && str[i] != ' ')
            return 0;
    }
    return 1;
}

/* ============ SAFE INTEGER INPUT ============ */
int safeIntegerInput() {
    char input[50];
    while (1) {
        if (fgets(input, sizeof(input), stdin) == NULL) {
            printError("Error reading input.");
            continue;
        }
        input[strcspn(input, "\n")] = 0; // Remove newline
        if (isValidInteger(input)) {
            return atoi(input);
        }
        printError("Invalid input! Please enter a valid integer:");
    }
}

/* ============ SAFE STRING INPUT ============ */
void safeStringInput(char str[], int maxLen) {
    if (fgets(str, maxLen, stdin) == NULL) {
        printError("Error reading input.");
        return;
    }
    str[strcspn(str, "\n")] = 0; // Remove newline
}

/* ============ VALIDATE EMAIL ============ */
int isValidEmail(char email[]) {
    int atCount = 0;
    int dotCount = 0;
    int hasAlphaNum = 0;

    if (strlen(email) < 5)
        return 0;

    for (int i = 0; email[i] != '\0'; i++) {
        if (email[i] == '@')
            atCount++;
        if (email[i] == '.')
            dotCount++;
        if (isalnum(email[i]))
            hasAlphaNum = 1;
    }

    return (atCount == 1 && dotCount >= 1 && hasAlphaNum);
}

/* ============ VALIDATE PHONE ============ */
int isValidPhone(char phone[]) {
    if (strlen(phone) != 10)
        return 0;

    for (int i = 0; i < 10; i++) {
        if (!isdigit(phone[i]))
            return 0;
    }
    return 1;
}

/* ============ CLEAR INPUT BUFFER ============ */
void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/* ============ PRINT COLORED TEXT ============ */
void printColor(const char *text) {
    printf("%s", text);
}

/* ============ PRINT ERROR MESSAGE IN RED ============ */
void printError(const char *text) {
    printf("%s%s%s\n", COLOR_RED, text, COLOR_RESET);
}

/* ============ PRINT SUCCESS MESSAGE IN GREEN ============ */
void printSuccess(const char *text) {
    printf("%s%s%s\n", COLOR_GREEN, text, COLOR_RESET);
}

/* ============ PRINT WARNING MESSAGE IN YELLOW ============ */
void printWarning(const char *text) {
    printf("%s%s%s\n", COLOR_YELLOW, text, COLOR_RESET);
}

/* ============ PRINT INFO MESSAGE IN CYAN ============ */
void printInfo(const char *text) {
    printf("%s%s%s\n", COLOR_CYAN, text, COLOR_RESET);
}

