#ifndef DATA_GENERATOR_H
#define DATA_GENERATOR_H

#include <stdio.h>

#define MAX_NAME_LEN 100
#define MAX_PASS_LEN 16
#define PREF_COUNT 4

typedef struct {
    int id;
    char name[MAX_NAME_LEN];
    char password[MAX_PASS_LEN];
    char category[4];
    char pref[PREF_COUNT][4];
    char department[4];
    int marks;
    int jee_rank;
    int allocated;
} Applicant;

void generate_applicant(Applicant *a);
void write_applicant_csv(FILE *fp, const Applicant *a);

#endif
