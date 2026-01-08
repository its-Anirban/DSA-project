#include <stdio.h>
#include <string.h>
#include "student.h"
#include "csv_handler.h"
#include "sorting.h"
#include "meritlist.h"
#include "department.h"
#include "utils.h"

/* ============================================================
   INTERNAL HELPER
   ============================================================ */
static void ensureMeritListGenerated() {
    generateMeritList();
}

/* ============================================================
   GENERATE MERIT LIST (UNCHANGED LOGIC)
   ============================================================ */
void generateMeritList() {
    Applicant a[MAX];
    int n = loadApplicants(a);

    if (n <= 0) {
        printWarning("No applicants found.");
        return;
    }

    int sortChoice;
    printf("\n========== CHOOSE SORTING ALGORITHM ==========\n");
    printf("1. Selection Sort\n");
    printf("2. Insertion Sort\n");
    printf("3. Merge Sort\n");
    printf("4. Quick Sort\n");
    printf("=============================================\n");
    printf("Enter your choice: ");
    scanf("%d", &sortChoice);
    getchar();

    printf("\nSorting applicants by JEE Rank (lower is better)...\n");

    switch (sortChoice) {
        case 1: selectionSort(a, n); printf("Using: Selection Sort\n"); break;
        case 2: insertionSort(a, n); printf("Using: Insertion Sort\n"); break;
        case 3: mergeSort(a, 0, n - 1); printf("Using: Merge Sort\n"); break;
        case 4: quickSort(a, 0, n - 1); printf("Using: Quick Sort\n"); break;
        default:
            printWarning("Invalid choice. Using Merge Sort.");
            mergeSort(a, 0, n - 1);
    }

    int seat[4] = {0};
    char deptList[4][5] = {"CSE", "IT", "TT", "APM"};
    int maxSeat = 10;

    for (int i = 0; i < n; i++) {
        a[i].allocated = 0;
        strcpy(a[i].department, "NA");
    }

    for (int i = 0; i < n; i++) {
        for (int p = 0; p < PREF_COUNT && !a[i].allocated; p++) {
            for (int d = 0; d < 4; d++) {
                if (strcmp(a[i].pref[p], deptList[d]) == 0 && seat[d] < maxSeat) {
                    seat[d]++;
                    a[i].allocated = 1;
                    strcpy(a[i].department, deptList[d]);
                    break;
                }
            }
        }
    }

    printf("\n========== MERIT LIST WITH ALLOCATIONS ==========\n");
    printf("Rank | ID   | Name              | Category | Marks | Dept | Status\n");
    printf("-------------------------------------------------------------------\n");

    for (int i = 0; i < n; i++) {
        printf("%4d | %4d | %-17s | %8s | %5d | %4s | %s\n",
               a[i].jee_rank, a[i].id, a[i].name, a[i].category,
               a[i].marks, a[i].department,
               a[i].allocated ? "SELECTED" : "WAITING");
    }

    FILE *fp = fopen("merit_list.csv", "w");
    if (fp) {
        fprintf(fp, "JEE_Rank,ID,Name,Category,Department,Marks,Status\n");
        for (int i = 0; i < n; i++) {
            fprintf(fp, "%d,%d,%s,%s,%s,%d,%s\n",
                    a[i].jee_rank, a[i].id, a[i].name, a[i].category,
                    a[i].department, a[i].marks,
                    a[i].allocated ? "SELECTED" : "WAITING");
        }
        fclose(fp);
    }

    saveApplicants(a, n);
}

/* ============================================================
   VIEW MERIT LIST
   ============================================================ */
void viewMeritList() {
    ensureMeritListGenerated();

    FILE *fp = fopen("merit_list.csv", "r");
    if (!fp) return;

    printf("\n========== MERIT LIST ==========\n");
    printf("Rank | ID   | Name              | Category | Marks | Dept | Status\n");
    printf("-------------------------------------------------------------------\n");

    char line[500];
    fgets(line, sizeof(line), fp);

    while (fgets(line, sizeof(line), fp)) {
        int rank, id, marks;
        char name[50], category[10], dept[10], status[20];

        if (sscanf(line, "%d,%d,%49[^,],%9[^,],%9[^,],%d,%19s",
                   &rank, &id, name, category, dept, &marks, status) == 7) {
            printf("%4d | %4d | %-17s | %8s | %5d | %4s | %s\n",
                   rank, id, name, category, marks, dept, status);
        }
    }
    fclose(fp);
}

/* ============================================================
   VIEW CATEGORY-WISE MERIT LIST
   ============================================================ */
void viewCategoryWiseMeritList(int category) {
    ensureMeritListGenerated();

    Applicant a[MAX];
    int n = loadApplicants(a);
    char catCode[5] = "";

    switch (category) {
        case 1: strcpy(catCode, "OBC"); break;
        case 2: strcpy(catCode, "SC"); break;
        case 3: strcpy(catCode, "ST"); break;
        case 4: strcpy(catCode, "GEN"); break;
        default: return;
    }

    printf("\n========== %s CATEGORY MERIT LIST ==========\n", catCode);

    for (int i = 0; i < n; i++) {
        if (strcmp(a[i].category, catCode) == 0) {
            printf("%4d | %4d | %-17s | %5d | %4s | %s\n",
                   a[i].jee_rank, a[i].id, a[i].name, a[i].marks,
                   a[i].department,
                   a[i].allocated ? "SELECTED" : "WAITING");
        }
    }
}

/* ============================================================
   VIEW DEPARTMENT-WISE MERIT LIST
   ============================================================ */
void viewDepartmentWiseMeritList(int deptChoice) {
    ensureMeritListGenerated();

    Applicant a[MAX];
    int n = loadApplicants(a);
    char depts[4][5] = {"CSE", "IT", "TT", "APM"};

    if (deptChoice < 1 || deptChoice > 4) return;

    for (int i = 0; i < n; i++) {
        if (strcmp(a[i].department, depts[deptChoice - 1]) == 0) {
            printf("%4d | %4d | %-17s | %8s | %5d\n",
                   a[i].jee_rank, a[i].id, a[i].name,
                   a[i].category, a[i].marks);
        }
    }
}

/* ============================================================
   VIEW WAITING LIST
   ============================================================ */
void viewWaitingList() {
    ensureMeritListGenerated();

    Applicant a[MAX];
    int n = loadApplicants(a);

    for (int i = 0; i < n; i++) {
        if (!a[i].allocated) {
            printf("%4d | %4d | %-17s | %8s\n",
                   a[i].jee_rank, a[i].id, a[i].name, a[i].category);
        }
    }
}
