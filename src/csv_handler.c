#include <stdio.h>
#include <string.h>
#include "student.h"
#include "csv_handler.h"
#include "utils.h"

/* ============ LOAD APPLICANTS FROM CSV ============ */
int loadApplicants(Applicant a[]) {
    FILE *fp = fopen("applicants_full.csv", "r");
    if (!fp) return 0;

    int i = 0;
    char line[500];

    // Skip header if exists
    if (fgets(line, sizeof(line), fp)) {
        // Check if first line is header
        if (strstr(line, "ID,Name") != NULL || strstr(line, "id,name") != NULL) {
            // It's a header, skip it
        } else {
            // Not a header, rewind
            rewind(fp);
        }
    }

    while (fgets(line, sizeof(line), fp) && i < MAX) {
        // Parse CSV: ID,Name,Password,Category,Pref1,Pref2,Pref3,Pref4,Department,Marks,JEE_Rank,Allocated
        int fields = sscanf(line,
            "%d,%49[^,],%19[^,],%4[^,],%4[^,],%4[^,],%4[^,],%4[^,],%4[^,],%d,%d,%d",
            &a[i].id,
            a[i].name,
            a[i].password,
            a[i].category,
            a[i].pref[0],
            a[i].pref[1],
            a[i].pref[2],
            a[i].pref[3],
            a[i].department,
            &a[i].marks,
            &a[i].jee_rank,
            &a[i].allocated
        );

        if (fields == 12) {
            i++;
        }
    }

    fclose(fp);
    return i;
}

/* ============ SAVE APPLICANTS TO CSV ============ */
void saveApplicants(Applicant a[], int n) {
    FILE *fp = fopen("applicants_full.csv", "w");
    if (!fp) return;

    // Write header
    fprintf(fp, "ID,Name,Password,Category,Pref1,Pref2,Pref3,Pref4,Department,Marks,JEE_Rank,Allocated\n");

    // Write data
    for (int i = 0; i < n; i++) {
        fprintf(fp,
            "%d,%s,%s,%s,%s,%s,%s,%s,%s,%d,%d,%d\n",
            a[i].id,
            a[i].name,
            a[i].password,
            a[i].category,
            a[i].pref[0],
            a[i].pref[1],
            a[i].pref[2],
            a[i].pref[3],
            a[i].department,
            a[i].marks,
            a[i].jee_rank,
            a[i].allocated
        );
    }

    fclose(fp);
}

/* ============ LOAD ADMIN CREDENTIALS ============ */
void loadAdminCredentials() {
    FILE *fp = fopen("Admin_Credentials.csv", "r");
    if (!fp) {
        printWarning("Admin credentials file not found.");
        return;
    }
    fclose(fp);
}

/* ============ SAVE ADMIN CREDENTIALS ============ */
void saveAdminCredentials() {
    // Admin credentials should be read-only
}
