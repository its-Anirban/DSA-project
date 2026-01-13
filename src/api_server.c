/*
 * Admission Management System - REST API Server
 * Uses Mongoose library for HTTP server functionality
 * Connects frontend to CSV data files
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include "../mongoose/mongoose.h"
#include "../headers/student.h"
#include "../headers/csv_handler.h"
#include "../headers/sorting.h"

#define HTTP_PORT "8080"
#define LOG_FILE "logs/api_server.log"

// Log file pointer
static FILE *log_fp = NULL;

// Forward declarations
static void handle_api_applicants(struct mg_connection *c, struct mg_http_message *hm);
static void handle_api_login_student(struct mg_connection *c, struct mg_http_message *hm);
static void handle_api_login_admin(struct mg_connection *c, struct mg_http_message *hm);
static void handle_api_register(struct mg_connection *c, struct mg_http_message *hm);
static void handle_api_generate_merit(struct mg_connection *c, struct mg_http_message *hm);
static void handle_api_update_applicant(struct mg_connection *c, struct mg_http_message *hm);

// Initialize logging
static void init_logging(void) {
    // Create logs directory if it doesn't exist
    mkdir("logs", 0755);
    
    log_fp = fopen(LOG_FILE, "a");
    if (log_fp) {
        time_t now = time(NULL);
        char timestamp[64];
        strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&now));
        fprintf(log_fp, "\n========================================\n");
        fprintf(log_fp, "Server started at %s\n", timestamp);
        fprintf(log_fp, "========================================\n");
        fflush(log_fp);
    }
}

// Log an API request
static void log_request(const char *method, const char *uri, int status, const char *details) {
    if (!log_fp) return;
    
    time_t now = time(NULL);
    char timestamp[64];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&now));
    
    fprintf(log_fp, "[%s] %s %s -> %d", timestamp, method, uri, status);
    if (details && strlen(details) > 0) {
        fprintf(log_fp, " | %s", details);
    }
    fprintf(log_fp, "\n");
    fflush(log_fp);
}

// Close logging
static void close_logging(void) {
    if (log_fp) {
        time_t now = time(NULL);
        char timestamp[64];
        strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&now));
        fprintf(log_fp, "Server stopped at %s\n", timestamp);
        fclose(log_fp);
        log_fp = NULL;
    }
}

// CORS headers for all API responses
static const char *cors_headers = 
    "Access-Control-Allow-Origin: *\r\n"
    "Access-Control-Allow-Methods: GET, POST, PUT, DELETE, OPTIONS\r\n"
    "Access-Control-Allow-Headers: Content-Type\r\n";

// JSON helper - escape string for JSON
static void json_escape(char *dest, const char *src, size_t max) {
    size_t j = 0;
    for (size_t i = 0; src[i] && j < max - 2; i++) {
        if (src[i] == '"' || src[i] == '\\') {
            dest[j++] = '\\';
        }
        dest[j++] = src[i];
    }
    dest[j] = '\0';
}

// Convert applicant to JSON
static void applicant_to_json(char *buf, size_t size, Applicant *a) {
    char name_escaped[100], dept_escaped[20];
    json_escape(name_escaped, a->name, sizeof(name_escaped));
    json_escape(dept_escaped, a->department, sizeof(dept_escaped));
    
    snprintf(buf, size,
        "{\"id\":%d,\"name\":\"%s\",\"category\":\"%s\","
        "\"pref\":[\"%s\",\"%s\",\"%s\",\"%s\"],"
        "\"department\":\"%s\",\"marks\":%d,\"jee_rank\":%d,\"allocated\":%d}",
        a->id, name_escaped, a->category,
        a->pref[0], a->pref[1], a->pref[2], a->pref[3],
        dept_escaped, a->marks, a->jee_rank, a->allocated);
}

// HTTP event handler
static void ev_handler(struct mg_connection *c, int ev, void *ev_data) {
    if (ev == MG_EV_HTTP_MSG) {
        struct mg_http_message *hm = (struct mg_http_message *) ev_data;
        
        // Extract method and URI for logging
        char method[10] = {0};
        char uri[256] = {0};
        snprintf(method, sizeof(method), "%.*s", (int)hm->method.len, hm->method.buf);
        snprintf(uri, sizeof(uri), "%.*s", (int)hm->uri.len, hm->uri.buf);
        
        // Handle CORS preflight
        if (mg_match(hm->method, mg_str("OPTIONS"), NULL)) {
            mg_http_reply(c, 204, cors_headers, "");
            return;
        }
        
        // API Routes
        if (mg_match(hm->uri, mg_str("/api/applicants"), NULL)) {
            log_request(method, uri, 200, "Fetching applicants");
            handle_api_applicants(c, hm);
        }
        else if (mg_match(hm->uri, mg_str("/api/applicants/*"), NULL)) {
            log_request(method, uri, 200, "Updating applicant");
            handle_api_update_applicant(c, hm);
        }
        else if (mg_match(hm->uri, mg_str("/api/login/student"), NULL)) {
            log_request(method, uri, 200, "Student login attempt");
            handle_api_login_student(c, hm);
        }
        else if (mg_match(hm->uri, mg_str("/api/login/admin"), NULL)) {
            log_request(method, uri, 200, "Admin login attempt");
            handle_api_login_admin(c, hm);
        }
        else if (mg_match(hm->uri, mg_str("/api/register"), NULL)) {
            log_request(method, uri, 201, "New student registration");
            handle_api_register(c, hm);
        }
        else if (mg_match(hm->uri, mg_str("/api/generate-merit"), NULL)) {
            log_request(method, uri, 200, "Generating merit list");
            handle_api_generate_merit(c, hm);
        }
        else {
            // Serve static files from current directory
            struct mg_http_serve_opts opts = {.root_dir = "."};
            mg_http_serve_dir(c, hm, &opts);
        }
    }
}

// GET /api/applicants - Get all applicants
// POST /api/applicants - Add new applicant
static void handle_api_applicants(struct mg_connection *c, struct mg_http_message *hm) {
    Applicant applicants[MAX];
    int n = loadApplicants(applicants);
    
    if (mg_match(hm->method, mg_str("GET"), NULL)) {
        // Build JSON array
        char *response = malloc(n * 300 + 100);
        if (!response) {
            mg_http_reply(c, 500, cors_headers, "{\"error\":\"Memory allocation failed\"}");
            return;
        }
        
        strcpy(response, "[");
        for (int i = 0; i < n; i++) {
            char applicant_json[300];
            applicant_to_json(applicant_json, sizeof(applicant_json), &applicants[i]);
            strcat(response, applicant_json);
            if (i < n - 1) strcat(response, ",");
        }
        strcat(response, "]");
        
        mg_http_reply(c, 200, 
            "Content-Type: application/json\r\n"
            "Access-Control-Allow-Origin: *\r\n",
            "%s", response);
        free(response);
    }
    else {
        mg_http_reply(c, 405, cors_headers, "{\"error\":\"Method not allowed\"}");
    }
}

// POST /api/login/student - Student login
static void handle_api_login_student(struct mg_connection *c, struct mg_http_message *hm) {
    if (!mg_match(hm->method, mg_str("POST"), NULL)) {
        mg_http_reply(c, 405, cors_headers, "{\"error\":\"Method not allowed\"}");
        return;
    }
    
    // Parse JSON body
    char name[50] = "", password[20] = "";
    int id = 0;
    
    struct mg_str body = hm->body;
    
    // Simple JSON parsing
    char *name_start = strstr(body.buf, "\"name\"");
    char *id_start = strstr(body.buf, "\"id\"");
    char *pass_start = strstr(body.buf, "\"password\"");
    
    if (name_start) {
        name_start = strchr(name_start, ':');
        if (name_start) {
            name_start = strchr(name_start, '"');
            if (name_start) {
                name_start++;
                char *name_end = strchr(name_start, '"');
                if (name_end) {
                    size_t len = name_end - name_start;
                    if (len < sizeof(name)) {
                        strncpy(name, name_start, len);
                        name[len] = '\0';
                    }
                }
            }
        }
    }
    
    if (id_start) {
        id_start = strchr(id_start, ':');
        if (id_start) {
            id = atoi(id_start + 1);
        }
    }
    
    if (pass_start) {
        pass_start = strchr(pass_start, ':');
        if (pass_start) {
            pass_start = strchr(pass_start, '"');
            if (pass_start) {
                pass_start++;
                char *pass_end = strchr(pass_start, '"');
                if (pass_end) {
                    size_t len = pass_end - pass_start;
                    if (len < sizeof(password)) {
                        strncpy(password, pass_start, len);
                        password[len] = '\0';
                    }
                }
            }
        }
    }
    
    // Validate
    Applicant applicants[MAX];
    int n = loadApplicants(applicants);
    
    for (int i = 0; i < n; i++) {
        if (applicants[i].id == id && 
            strcmp(applicants[i].name, name) == 0 &&
            strcmp(applicants[i].password, password) == 0) {
            
            char response[500];
            applicant_to_json(response, sizeof(response), &applicants[i]);
            
            mg_http_reply(c, 200,
                "Content-Type: application/json\r\n"
                "Access-Control-Allow-Origin: *\r\n",
                "{\"success\":true,\"student\":%s}", response);
            return;
        }
    }
    
    mg_http_reply(c, 401,
        "Content-Type: application/json\r\n"
        "Access-Control-Allow-Origin: *\r\n",
        "{\"success\":false,\"error\":\"Invalid credentials\"}");
}

// POST /api/login/admin - Admin login
static void handle_api_login_admin(struct mg_connection *c, struct mg_http_message *hm) {
    if (!mg_match(hm->method, mg_str("POST"), NULL)) {
        mg_http_reply(c, 405, cors_headers, "{\"error\":\"Method not allowed\"}");
        return;
    }
    
    // Parse JSON body
    char username[100] = "", password[100] = "";
    int empId = 0;
    
    struct mg_str body = hm->body;
    
    char *emp_start = strstr(body.buf, "\"empId\"");
    char *user_start = strstr(body.buf, "\"username\"");
    char *pass_start = strstr(body.buf, "\"password\"");
    
    if (emp_start) {
        emp_start = strchr(emp_start, ':');
        if (emp_start) empId = atoi(emp_start + 1);
    }
    
    if (user_start) {
        user_start = strchr(user_start, ':');
        if (user_start) {
            user_start = strchr(user_start, '"');
            if (user_start) {
                user_start++;
                char *end = strchr(user_start, '"');
                if (end) {
                    size_t len = end - user_start;
                    if (len < sizeof(username)) {
                        strncpy(username, user_start, len);
                        username[len] = '\0';
                    }
                }
            }
        }
    }
    
    if (pass_start) {
        pass_start = strchr(pass_start, ':');
        if (pass_start) {
            pass_start = strchr(pass_start, '"');
            if (pass_start) {
                pass_start++;
                char *end = strchr(pass_start, '"');
                if (end) {
                    size_t len = end - pass_start;
                    if (len < sizeof(password)) {
                        strncpy(password, pass_start, len);
                        password[len] = '\0';
                    }
                }
            }
        }
    }
    
    // Check admin credentials from CSV
    FILE *fp = fopen("admin_credentials.csv", "r");
    if (!fp) {
        mg_http_reply(c, 500, cors_headers, "{\"error\":\"Cannot read admin credentials\"}");
        return;
    }
    
    char line[256];
    fgets(line, sizeof(line), fp); // Skip header
    
    while (fgets(line, sizeof(line), fp)) {
        int fileEmpId;
        char fileUsername[100], filePassword[100];
        
        if (sscanf(line, "%d,%99[^,],%99[^\n\r]", &fileEmpId, fileUsername, filePassword) == 3) {
            if (empId == fileEmpId && 
                strcmp(username, fileUsername) == 0 && 
                strcmp(password, filePassword) == 0) {
                fclose(fp);
                mg_http_reply(c, 200,
                    "Content-Type: application/json\r\n"
                    "Access-Control-Allow-Origin: *\r\n",
                    "{\"success\":true,\"admin\":{\"empId\":%d,\"username\":\"%s\"}}",
                    empId, username);
                return;
            }
        }
    }
    
    fclose(fp);
    mg_http_reply(c, 401,
        "Content-Type: application/json\r\n"
        "Access-Control-Allow-Origin: *\r\n",
        "{\"success\":false,\"error\":\"Invalid credentials\"}");
}

// POST /api/register - Register new student
static void handle_api_register(struct mg_connection *c, struct mg_http_message *hm) {
    if (!mg_match(hm->method, mg_str("POST"), NULL)) {
        mg_http_reply(c, 405, cors_headers, "{\"error\":\"Method not allowed\"}");
        return;
    }
    
    Applicant applicants[MAX];
    int n = loadApplicants(applicants);
    
    if (n >= MAX) {
        mg_http_reply(c, 400, cors_headers, "{\"error\":\"Database full\"}");
        return;
    }
    
    // Parse JSON body (simplified parsing)
    Applicant newStudent = {0};
    struct mg_str body = hm->body;
    
    // Find max ID and increment
    int maxId = 999;
    for (int i = 0; i < n; i++) {
        if (applicants[i].id > maxId) maxId = applicants[i].id;
    }
    newStudent.id = maxId + 1;
    
    // Parse fields from JSON (simplified)
    char *ptr;
    
    if ((ptr = strstr(body.buf, "\"name\""))) {
        ptr = strchr(ptr, ':');
        if (ptr) {
            ptr = strchr(ptr, '"');
            if (ptr) {
                ptr++;
                char *end = strchr(ptr, '"');
                if (end && (end - ptr) < 50) {
                    strncpy(newStudent.name, ptr, end - ptr);
                }
            }
        }
    }
    
    if ((ptr = strstr(body.buf, "\"password\""))) {
        ptr = strchr(ptr, ':');
        if (ptr) {
            ptr = strchr(ptr, '"');
            if (ptr) {
                ptr++;
                char *end = strchr(ptr, '"');
                if (end && (end - ptr) < 20) {
                    strncpy(newStudent.password, ptr, end - ptr);
                }
            }
        }
    }
    
    if ((ptr = strstr(body.buf, "\"category\""))) {
        ptr = strchr(ptr, ':');
        if (ptr) {
            ptr = strchr(ptr, '"');
            if (ptr) {
                ptr++;
                char *end = strchr(ptr, '"');
                if (end && (end - ptr) < 5) {
                    strncpy(newStudent.category, ptr, end - ptr);
                }
            }
        }
    }
    
    if ((ptr = strstr(body.buf, "\"jee_rank\""))) {
        ptr = strchr(ptr, ':');
        if (ptr) newStudent.jee_rank = atoi(ptr + 1);
    }
    
    if ((ptr = strstr(body.buf, "\"marks\""))) {
        ptr = strchr(ptr, ':');
        if (ptr) newStudent.marks = atoi(ptr + 1);
    }
    
    // Parse preferences array
    if ((ptr = strstr(body.buf, "\"pref\""))) {
        ptr = strchr(ptr, '[');
        if (ptr) {
            for (int i = 0; i < 4; i++) {
                ptr = strchr(ptr, '"');
                if (ptr) {
                    ptr++;
                    char *end = strchr(ptr, '"');
                    if (end && (end - ptr) < 5) {
                        strncpy(newStudent.pref[i], ptr, end - ptr);
                        ptr = end + 1;
                    }
                }
            }
        }
    }
    
    strcpy(newStudent.department, "N/A");
    newStudent.allocated = 0;
    
    // Validate
    if (strlen(newStudent.name) == 0 || strlen(newStudent.password) < 3) {
        mg_http_reply(c, 400, cors_headers, "{\"error\":\"Invalid data\"}");
        return;
    }
    
    // Save
    applicants[n] = newStudent;
    saveApplicants(applicants, n + 1);
    
    char response[300];
    applicant_to_json(response, sizeof(response), &newStudent);
    
    mg_http_reply(c, 201,
        "Content-Type: application/json\r\n"
        "Access-Control-Allow-Origin: *\r\n",
        "{\"success\":true,\"id\":%d,\"student\":%s}", newStudent.id, response);
}

// PUT /api/applicants/:id - Update applicant
static void handle_api_update_applicant(struct mg_connection *c, struct mg_http_message *hm) {
    if (!mg_match(hm->method, mg_str("PUT"), NULL)) {
        mg_http_reply(c, 405, cors_headers, "{\"error\":\"Method not allowed\"}");
        return;
    }
    
    // Extract ID from URL
    int id = 0;
    char *id_start = strrchr(hm->uri.buf, '/');
    if (id_start) {
        id = atoi(id_start + 1);
    }
    
    Applicant applicants[MAX];
    int n = loadApplicants(applicants);
    
    int found = -1;
    for (int i = 0; i < n; i++) {
        if (applicants[i].id == id) {
            found = i;
            break;
        }
    }
    
    if (found == -1) {
        mg_http_reply(c, 404, cors_headers, "{\"error\":\"Applicant not found\"}");
        return;
    }
    
    struct mg_str body = hm->body;
    char *ptr;
    
    // Update password if provided
    if ((ptr = strstr(body.buf, "\"password\""))) {
        ptr = strchr(ptr, ':');
        if (ptr) {
            ptr = strchr(ptr, '"');
            if (ptr) {
                ptr++;
                char *end = strchr(ptr, '"');
                if (end && (end - ptr) < 20) {
                    strncpy(applicants[found].password, ptr, end - ptr);
                    applicants[found].password[end - ptr] = '\0';
                }
            }
        }
    }
    
    // Update preferences if provided
    if ((ptr = strstr(body.buf, "\"pref\""))) {
        ptr = strchr(ptr, '[');
        if (ptr) {
            for (int i = 0; i < 4; i++) {
                ptr = strchr(ptr, '"');
                if (ptr) {
                    ptr++;
                    char *end = strchr(ptr, '"');
                    if (end && (end - ptr) < 5) {
                        strncpy(applicants[found].pref[i], ptr, end - ptr);
                        applicants[found].pref[i][end - ptr] = '\0';
                        ptr = end + 1;
                    }
                }
            }
        }
    }
    
    saveApplicants(applicants, n);
    
    char response[300];
    applicant_to_json(response, sizeof(response), &applicants[found]);
    
    mg_http_reply(c, 200,
        "Content-Type: application/json\r\n"
        "Access-Control-Allow-Origin: *\r\n",
        "{\"success\":true,\"student\":%s}", response);
}

// POST /api/generate-merit - Generate merit list
static void handle_api_generate_merit(struct mg_connection *c, struct mg_http_message *hm) {
    if (!mg_match(hm->method, mg_str("POST"), NULL)) {
        mg_http_reply(c, 405, cors_headers, "{\"error\":\"Method not allowed\"}");
        return;
    }
    
    Applicant applicants[MAX];
    int n = loadApplicants(applicants);
    
    if (n <= 0) {
        mg_http_reply(c, 400, cors_headers, "{\"error\":\"No applicants found\"}");
        return;
    }
    
    // Sort by JEE rank (using merge sort from sorting.c)
    mergeSort(applicants, 0, n - 1);
    
    // Allocate departments (10 seats each)
    int seatAlloc[4] = {0}; // CSE, IT, TT, APM
    char deptList[4][5] = {"CSE", "IT", "TT", "APM"};
    int maxSeats = 10;
    
    for (int i = 0; i < n; i++) {
        applicants[i].allocated = 0;
        strcpy(applicants[i].department, "NA");
    }
    
    for (int i = 0; i < n; i++) {
        for (int p = 0; p < PREF_COUNT && !applicants[i].allocated; p++) {
            for (int d = 0; d < 4; d++) {
                if (strcmp(applicants[i].pref[p], deptList[d]) == 0 && seatAlloc[d] < maxSeats) {
                    seatAlloc[d]++;
                    applicants[i].allocated = 1;
                    strcpy(applicants[i].department, deptList[d]);
                    break;
                }
            }
        }
    }
    
    // Save to CSV
    saveApplicants(applicants, n);
    
    // Also save merit list
    FILE *fp = fopen("merit_list.csv", "w");
    if (fp) {
        fprintf(fp, "JEE_Rank,ID,Name,Category,Department,Marks,Status\n");
        for (int i = 0; i < n; i++) {
            fprintf(fp, "%d,%d,%s,%s,%s,%d,%s\n",
                applicants[i].jee_rank, applicants[i].id, applicants[i].name,
                applicants[i].category, applicants[i].department, applicants[i].marks,
                applicants[i].allocated ? "SELECTED" : "WAITING");
        }
        fclose(fp);
    }
    
    int allocated = 0;
    for (int i = 0; i < n; i++) {
        if (applicants[i].allocated) allocated++;
    }
    
    mg_http_reply(c, 200,
        "Content-Type: application/json\r\n"
        "Access-Control-Allow-Origin: *\r\n",
        "{\"success\":true,\"message\":\"Merit list generated\",\"allocated\":%d,\"total\":%d,"
        "\"seats\":{\"CSE\":%d,\"IT\":%d,\"TT\":%d,\"APM\":%d}}",
        allocated, n, seatAlloc[0], seatAlloc[1], seatAlloc[2], seatAlloc[3]);
}

int main(void) {
    struct mg_mgr mgr;
    
    // Initialize logging
    init_logging();
    
    mg_mgr_init(&mgr);
    
    printf("==============================================\n");
    printf("  ADMISSION MANAGEMENT SYSTEM - API SERVER\n");
    printf("==============================================\n\n");
    
    struct mg_connection *c = mg_http_listen(&mgr, "http://0.0.0.0:" HTTP_PORT, ev_handler, NULL);
    
    if (c == NULL) {
        printf("Error: Cannot start server on port %s\n", HTTP_PORT);
        close_logging();
        return 1;
    }
    
    printf("Server started at http://localhost:%s\n", HTTP_PORT);
    printf("Frontend available at http://localhost:%s/index.html\n\n", HTTP_PORT);
    printf("Logs written to: %s\n\n", LOG_FILE);
    printf("API Endpoints:\n");
    printf("  GET  /api/applicants      - Get all applicants\n");
    printf("  POST /api/login/student   - Student login\n");
    printf("  POST /api/login/admin     - Admin login\n");
    printf("  POST /api/register        - Register new student\n");
    printf("  PUT  /api/applicants/:id  - Update applicant\n");
    printf("  POST /api/generate-merit  - Generate merit list\n\n");
    printf("Press Ctrl+C to stop the server\n\n");
    
    for (;;) {
        mg_mgr_poll(&mgr, 1000);
    }
    
    close_logging();
    mg_mgr_free(&mgr);
    return 0;
}
