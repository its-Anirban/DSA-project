#include "../headers/data_generator.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define TOTAL_APPLICANTS 100

int main(void) {
    srand((unsigned)time(NULL));

    FILE *fp = fopen("applicants_full.csv", "w");
    if (!fp) {
        perror("Unable to create applicants_full.csv");
        return 1;
    }

    fprintf(fp,
        "ID,Name,Password,Category,Pref1,Pref2,Pref3,Pref4,"
        "Department,Marks,JEE_Rank,Allocated\n");

    for (int i = 0; i < TOTAL_APPLICANTS; i++) {
        Applicant a;
        generate_applicant(&a);
        write_applicant_csv(fp, &a);
    }

    fclose(fp);

    printf("applicants_full.csv generated successfully\n");
    return 0;
}
