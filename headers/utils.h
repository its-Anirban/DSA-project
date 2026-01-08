#ifndef UTILS_H
#define UTILS_H

/* ============ COLOR CODES FOR UBUNTU/LINUX COMPATIBILITY ============ */
#define COLOR_RED     "\033[1;31m"      /* Bold Red */
#define COLOR_GREEN   "\033[1;32m"      /* Bold Green */
#define COLOR_YELLOW  "\033[1;33m"      /* Bold Yellow */
#define COLOR_BLUE    "\033[1;34m"      /* Bold Blue */
#define COLOR_MAGENTA "\033[1;35m"      /* Bold Magenta */
#define COLOR_CYAN    "\033[1;36m"      /* Bold Cyan */
#define COLOR_RESET   "\033[0m"         /* Reset to default */

/* ============ FUNCTION DECLARATIONS ============ */
int isValidInteger(char str[]);
int isValidLetters(char str[]);
int safeIntegerInput();
void safeStringInput(char str[], int maxLen);
int isValidEmail(char email[]);
int isValidPhone(char phone[]);
void clearInputBuffer();

/* ============ COLOR PRINT FUNCTIONS ============ */
void printColor(const char *text);
void printError(const char *text);
void printSuccess(const char *text);
void printWarning(const char *text);
void printInfo(const char *text);

#endif
