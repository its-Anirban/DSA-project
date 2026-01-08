#ifndef STUDENT_H
#define STUDENT_H

#define MAX 1000
#define PREF_COUNT 4   // CSE, IT, TT, APM

typedef struct {
    int id;
    char name[50];
    char password[20];
    char category[5];          // GEN/OBC/SC/ST
    char pref[PREF_COUNT][5];  // Preferences: pref[0] highest
    char department[5];        // ALLOTTED department
    int marks;
    int jee_rank;              // FIXED NAME (no space)
    int allocated;             // 1 = Selected, 0 = Waiting
} Applicant;

#endif
