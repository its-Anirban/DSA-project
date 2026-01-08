#include "data_generator.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static const char *FIRST_NAMES[] = {
    "Amit", "Anirban", "Rohit", "Sourav", "Arjun", "Rahul",
    "Kunal", "Sandeep", "Vikas", "Abhishek", "Nikhil", "Pranav",
    "Priya", "Ananya", "Pooja", "Riya", "Sneha", "Shreya",
    "Kavya", "Neha", "Aishwarya", "Swati", "Ishita", "Tanvi"
};


static const char *LAST_NAMES[] = {
    "Sharma", "Verma", "Singh", "Kumar", "Gupta",
    "Agarwal", "Ghosh", "Banerjee", "Chatterjee", "Mukherjee",
    "Das", "Dutta", "Roy", "Saha", "Mandal",
    "Patel", "Mehta", "Joshi", "Iyer", "Reddy"
};


static const char *CATEGORIES[] = { "GEN", "OBC", "SC", "ST" };
static const char *DEPARTMENTS[] = { "CSE", "IT", "TT", "APM" };

#define FN_COUNT (sizeof(FIRST_NAMES) / sizeof(FIRST_NAMES[0]))
#define LN_COUNT (sizeof(LAST_NAMES) / sizeof(LAST_NAMES[0]))

static void random_password(char *out, int len) {
    const char chars[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789";

    for (int i = 0; i < len - 1; i++)
        out[i] = chars[rand() % (sizeof(chars) - 1)];

    out[len - 1] = '\0';
}

static void shuffle(char arr[][4], int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        char tmp[4];
        strcpy(tmp, arr[i]);
        strcpy(arr[i], arr[j]);
        strcpy(arr[j], tmp);
    }
}

void generate_applicant(Applicant *a) {
    a->id = 1000 + rand() % 90000;

    /* Name generation */
    const char *first = FIRST_NAMES[rand() % FN_COUNT];
    const char *last  = LAST_NAMES[rand() % LN_COUNT];

    if (rand() % 2) {
        snprintf(a->name, MAX_NAME_LEN, "%s %s", first, last);
    } else {
        snprintf(a->name, MAX_NAME_LEN, "%s %s %s",
                 first,
                 FIRST_NAMES[rand() % FN_COUNT],
                 last);
    }

    /* Password */
    random_password(a->password, 10);

    /* Category */
    strcpy(a->category, CATEGORIES[rand() % 4]);

    /* Preferences (non-repeating) */
    for (int i = 0; i < PREF_COUNT; i++)
        strcpy(a->pref[i], DEPARTMENTS[i]);

    shuffle(a->pref, PREF_COUNT);

    /* Department from preference */
    strcpy(a->department, a->pref[rand() % PREF_COUNT]);

    /* Marks, rank, allocated */
    a->marks = rand() % 101;
    a->jee_rank = 1 + rand() % 50000;
    a->allocated = rand() % 2;
}

void write_applicant_csv(FILE *fp, const Applicant *a) {
    fprintf(fp,
        "%d,%s,%s,%s,%s,%s,%s,%s,%s,%d,%d,%d\n",
        a->id,
        a->name,
        a->password,
        a->category,
        a->pref[0],
        a->pref[1],
        a->pref[2],
        a->pref[3],
        a->department,
        a->marks,
        a->jee_rank,
        a->allocated
    );
}

