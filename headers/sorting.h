#ifndef SORTING_H
#define SORTING_H

#include "student.h"

int isBetter(Applicant a, Applicant b);
void selectionSort(Applicant a[], int n);
void insertionSort(Applicant a[], int n);
void quickSort(Applicant a[], int low, int high);
void mergeSort(Applicant a[], int l, int r);

#endif
