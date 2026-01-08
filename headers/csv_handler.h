#ifndef CSV_HANDLER_H
#define CSV_HANDLER_H

#include "student.h"

int loadApplicants(Applicant a[]);
void saveApplicants(Applicant a[], int n);
void loadAdminCredentials();
void saveAdminCredentials();

#endif
