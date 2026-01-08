#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "student.h"
#include "sorting.h"

/* =====================================================
   COMPARISON FUNCTION
   Return 1 if a should come BEFORE b
   Priority:
   1. Lower JEE Rank is better (rank 1 is better than rank 2)
   2. If same JEE Rank, higher marks is better
   ===================================================== */
int isBetter(Applicant a, Applicant b) {
    if (a.jee_rank < b.jee_rank)
        return 1;

    if (a.jee_rank > b.jee_rank)
        return 0;

    // Tie breaker: higher marks is better
    if (a.marks > b.marks)
        return 1;

    return 0;
}

/* ================= SELECTION SORT ================= */
void selectionSort(Applicant a[], int n) {
    int i, j, best;

    for (i = 0; i < n - 1; i++) {
        best = i;

        for (j = i + 1; j < n; j++) {
            if (isBetter(a[j], a[best]))
                best = j;
        }

        if (best != i) {
            Applicant temp = a[i];
            a[i] = a[best];
            a[best] = temp;
        }
    }
}

/* ================= INSERTION SORT ================= */
void insertionSort(Applicant a[], int n) {
    int i, j;
    Applicant key;

    for (i = 1; i < n; i++) {
        key = a[i];
        j = i - 1;

        while (j >= 0 && isBetter(key, a[j])) {
            a[j + 1] = a[j];
            j--;
        }

        a[j + 1] = key;
    }
}

/* ================= QUICK SORT HELPER ================= */
static int partition(Applicant a[], int low, int high) {
    Applicant pivot = a[high];
    int i = low - 1;
    int j;

    for (j = low; j < high; j++) {
        if (isBetter(a[j], pivot)) {
            i++;
            Applicant temp = a[i];
            a[i] = a[j];
            a[j] = temp;
        }
    }

    Applicant temp = a[i + 1];
    a[i + 1] = a[high];
    a[high] = temp;

    return i + 1;
}

/* ================= QUICK SORT ================= */
void quickSort(Applicant a[], int low, int high) {
    if (low < high) {
        int pi = partition(a, low, high);
        quickSort(a, low, pi - 1);
        quickSort(a, pi + 1, high);
    }
}

/* ================= MERGE SORT HELPER ================= */
static void merge(Applicant a[], int l, int m, int r) {
    int n1 = m - l + 1;
    int n2 = r - m;
    int i, j, k = l;

    Applicant *L = (Applicant *)malloc(n1 * sizeof(Applicant));
    Applicant *R = (Applicant *)malloc(n2 * sizeof(Applicant));

    for (i = 0; i < n1; i++)
        L[i] = a[l + i];
    for (j = 0; j < n2; j++)
        R[j] = a[m + 1 + j];

    i = 0;
    j = 0;

    while (i < n1 && j < n2) {
        if (isBetter(L[i], R[j])) {
            a[k++] = L[i++];
        } else {
            a[k++] = R[j++];
        }
    }

    while (i < n1)
        a[k++] = L[i++];

    while (j < n2)
        a[k++] = R[j++];

    free(L);
    free(R);
}

/* ================= MERGE SORT ================= */
void mergeSort(Applicant a[], int l, int r) {
    if (l < r) {
        int m = l + (r - l) / 2;
        mergeSort(a, l, m);
        mergeSort(a, m + 1, r);
        merge(a, l, m, r);
    }
}
