// ============================================================
// ADMISSION MANAGEMENT SYSTEM - JavaScript Logic
// Replicates the exact terminal flow from the C application
// ============================================================

// Global State
let currentStudent = null;
let allApplicants = [];
let adminLoggedIn = false;
let studentLoginAttempts = 3;
let adminLoginAttempts = 3;
let verifiedStudentName = null;
let verifiedStudentIndex = -1;
let verifiedAdminCredentials = null;

// Admin credentials (from admin_credentials.csv)
const adminCredentials = [
    { empId: 1001, username: 'admin', password: 'pass' }
];

// ============================================================
// SCREEN NAVIGATION
// ============================================================
function showScreen(screenId) {
    document.querySelectorAll('.screen').forEach(s => s.classList.remove('active'));
    document.getElementById(screenId).classList.add('active');
}

function goToStudentPortal() {
    showScreen('studentPortalMenu');
}

function goToAdminPortal() {
    resetAdminLogin();
    showScreen('adminPortalMenu');
}

function backToMainMenu() {
    currentStudent = null;
    adminLoggedIn = false;
    showScreen('mainMenu');
}

function backToStudentPortal() {
    resetStudentLogin();
    showScreen('studentPortalMenu');
}

function backToStudentDashboard() {
    showScreen('studentDashboard');
}

function backToAdminDashboard() {
    showScreen('adminDashboard');
}

function goToStudentLogin() {
    resetStudentLogin();
    showScreen('studentLogin');
}

function goToStudentRegistration() {
    showScreen('studentRegistration');
}

function exitApp() {
    if (confirm('Are you sure you want to exit?')) {
        document.body.innerHTML = `
            <div style="min-height:100vh;display:flex;justify-content:center;align-items:center;background:linear-gradient(135deg,#667eea 0%,#764ba2 100%);color:white;text-align:center;font-family:Inter,sans-serif;">
                <div>
                    <h1 style="font-size:3em;margin-bottom:20px;">👋 Goodbye!</h1>
                    <p style="font-size:1.2em;opacity:0.9;">Thank you for using the Admission Management System</p>
                </div>
            </div>
        `;
    }
}

// ============================================================
// STUDENT LOGIN - Step by Step (Matching Terminal Flow)
// ============================================================
function resetStudentLogin() {
    studentLoginAttempts = 3;
    verifiedStudentName = null;
    verifiedStudentIndex = -1;
    document.getElementById('loginStep1').style.display = 'block';
    document.getElementById('loginStep2').style.display = 'none';
    document.getElementById('loginStep3').style.display = 'none';
    document.getElementById('loginName').value = '';
    document.getElementById('loginId').value = '';
    document.getElementById('loginPassword').value = '';
    document.getElementById('attemptsInfo').innerHTML = '';
    document.getElementById('loginStatusMessages').innerHTML = '';
    hideAlert('loginAlert');
}

function addStatusMessage(containerId, text, type = 'info') {
    if (!text) return;
    const container = document.getElementById(containerId);
    const msg = document.createElement('div');
    msg.className = `status-message ${type}`;
    const icon = type === 'success' ? '✓' : type === 'error' ? '✗' : 'ℹ';
    msg.innerHTML = `<span>${icon}</span><span>${text}</span>`;
    container.appendChild(msg);
}

async function verifyStudentName() {
    const name = document.getElementById('loginName').value.trim();

    if (!name) {
        showAlert('loginAlert', 'Please enter your name', 'error');
        return;
    }

    await loadApplicants();

    const nameIndex = allApplicants.findIndex(a => a.name === name);

    if (nameIndex === -1) {
        addStatusMessage('loginStatusMessages', `Student "${name}" not found in database`, 'error');
        studentLoginAttempts--;
        updateAttemptsInfo('attemptsInfo', studentLoginAttempts);

        if (studentLoginAttempts <= 0) {
            showAlert('loginAlert', 'Login failed after 3 attempts. Returning to student portal...', 'error');
            setTimeout(() => backToStudentPortal(), 2000);
        }
        return;
    }

    verifiedStudentName = name;
    verifiedStudentIndex = nameIndex;
    addStatusMessage('loginStatusMessages', `Student "${name}" verified successfully`, 'success');

    document.getElementById('loginStep1').style.display = 'none';
    document.getElementById('loginStep2').style.display = 'block';
    hideAlert('loginAlert');
}

function verifyStudentId() {
    const id = parseInt(document.getElementById('loginId').value);

    if (!id) {
        showAlert('loginAlert', 'Please enter your Application ID', 'error');
        return;
    }

    const idIndex = allApplicants.findIndex(a => a.id === id);

    if (idIndex === -1) {
        addStatusMessage('loginStatusMessages', `Application ID ${id} not found in database`, 'error');
        studentLoginAttempts--;
        updateAttemptsInfo('attemptsInfo', studentLoginAttempts);

        if (studentLoginAttempts <= 0) {
            showAlert('loginAlert', 'Login failed after 3 attempts. Returning to student portal...', 'error');
            setTimeout(() => backToStudentPortal(), 2000);
        }
        return;
    }

    if (idIndex !== verifiedStudentIndex) {
        addStatusMessage('loginStatusMessages', 'Name and Application ID do not match', 'error');
        studentLoginAttempts--;
        updateAttemptsInfo('attemptsInfo', studentLoginAttempts);

        if (studentLoginAttempts <= 0) {
            showAlert('loginAlert', 'Login failed after 3 attempts. Returning to student portal...', 'error');
            setTimeout(() => backToStudentPortal(), 2000);
        }
        return;
    }

    addStatusMessage('loginStatusMessages', `Application ID ${id} verified successfully`, 'success');

    document.getElementById('loginStep2').style.display = 'none';
    document.getElementById('loginStep3').style.display = 'block';
    hideAlert('loginAlert');
}

async function performStudentLogin() {
    const password = document.getElementById('loginPassword').value;
    const name = verifiedStudentName;
    const id = allApplicants[verifiedStudentIndex].id;

    if (!password) {
        showAlert('loginAlert', 'Please enter your password', 'error');
        return;
    }

    // Use API for login verification (password is not in local data for security)
    if (useAPI) {
        try {
            const response = await fetch(`${API_BASE}/api/login/student`, {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ name, id, password })
            });
            const result = await response.json();

            if (result.success) {
                currentStudent = result.student;
                currentStudent.password = password; // Store for local operations
                addStatusMessage('loginStatusMessages', 'Login successful!', 'success');
                showAlert('loginAlert', `Welcome, ${currentStudent.name}!`, 'success');
                document.getElementById('welcomeMessage').textContent = `Welcome back, ${currentStudent.name}!`;
                setTimeout(() => showScreen('studentDashboard'), 1000);
                return;
            } else {
                addStatusMessage('loginStatusMessages', 'Incorrect password', 'error');
                studentLoginAttempts--;
                updateAttemptsInfo('attemptsInfo', studentLoginAttempts);

                if (studentLoginAttempts <= 0) {
                    showAlert('loginAlert', 'Login failed after 3 attempts. Returning to student portal...', 'error');
                    setTimeout(() => backToStudentPortal(), 2000);
                }
                return;
            }
        } catch (e) {
            console.log('API login failed, trying local fallback');
        }
    }

    // Fallback: Local verification (only works if passwords are in local data)
    if (allApplicants[verifiedStudentIndex].password !== password) {
        addStatusMessage('loginStatusMessages', 'Incorrect password', 'error');
        studentLoginAttempts--;
        updateAttemptsInfo('attemptsInfo', studentLoginAttempts);

        if (studentLoginAttempts <= 0) {
            showAlert('loginAlert', 'Login failed after 3 attempts. Returning to student portal...', 'error');
            setTimeout(() => backToStudentPortal(), 2000);
        }
        return;
    }

    currentStudent = allApplicants[verifiedStudentIndex];
    addStatusMessage('loginStatusMessages', 'Login successful!', 'success');

    showAlert('loginAlert', `Welcome, ${currentStudent.name}!`, 'success');
    document.getElementById('welcomeMessage').textContent = `Welcome back, ${currentStudent.name}!`;
    setTimeout(() => showScreen('studentDashboard'), 1000);
}

function updateAttemptsInfo(elementId, attempts) {
    const el = document.getElementById(elementId);
    if (attempts > 0) {
        el.innerHTML = `<div class="attempts-badge">⚠️ Attempts left: ${attempts}</div>`;
    } else {
        el.innerHTML = `<div class="attempts-badge" style="background:#fee2e2;color:#991b1b;">❌ No attempts remaining</div>`;
    }
}

// ============================================================
// PREFERENCE OPTIONS - Dynamic filtering
// ============================================================
const allDepartments = ['CSE', 'IT', 'TT', 'APM'];

function updatePreferenceOptions() {
    const prefSelects = [
        document.getElementById('pref1'),
        document.getElementById('pref2'),
        document.getElementById('pref3'),
        document.getElementById('pref4')
    ];

    // Get currently selected values
    const selectedValues = prefSelects.map(sel => sel.value);

    // Update each select's options
    prefSelects.forEach((select, index) => {
        const currentValue = select.value;

        // Clear all options except the first "Select" option
        while (select.options.length > 1) {
            select.remove(1);
        }

        // Add available options
        allDepartments.forEach(dept => {
            // Show option if: it's the current selection OR it's not selected elsewhere
            const isSelectedElsewhere = selectedValues.some((val, i) => val === dept && i !== index);

            if (!isSelectedElsewhere || currentValue === dept) {
                const option = document.createElement('option');
                option.value = dept;
                option.textContent = dept;
                if (currentValue === dept) {
                    option.selected = true;
                }
                select.appendChild(option);
            }
        });
    });
}

// ============================================================
// STUDENT REGISTRATION
// ============================================================
async function performStudentRegistration() {
    const name = document.getElementById('regName').value.trim();
    const password = document.getElementById('regPassword').value;
    const category = document.getElementById('regCategory').value;
    const jeeRank = parseInt(document.getElementById('regJeeRank').value);
    const marks = parseInt(document.getElementById('regMarks').value);

    if (!name || !password || !category || !jeeRank || !marks) {
        showAlert('regAlert', 'Please fill all fields', 'error');
        return;
    }

    if (password.length < 3 || password.length > 9) {
        showAlert('regAlert', 'Password must be 3-9 characters', 'error');
        return;
    }

    const prefs = [
        document.getElementById('pref1').value,
        document.getElementById('pref2').value,
        document.getElementById('pref3').value,
        document.getElementById('pref4').value
    ];

    if (prefs.some(p => !p)) {
        showAlert('regAlert', 'Please select all preferences', 'error');
        return;
    }

    const newStudent = {
        name: name,
        password: password,
        category: category,
        jee_rank: jeeRank,
        marks: marks,
        pref: prefs
    };

    // Try API first
    if (useAPI) {
        try {
            const response = await fetch(`${API_BASE}/api/register`, {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify(newStudent)
            });
            const result = await response.json();

            if (result.success) {
                showAlert('regAlert', `Registration Successful! Your Application ID: ${result.id}. Please note your ID for login.`, 'success');
                document.getElementById('registrationForm').reset();
                await loadApplicants(); // Reload from API
                setTimeout(() => backToStudentPortal(), 3000);
                return;
            } else {
                showAlert('regAlert', result.error || 'Registration failed', 'error');
                return;
            }
        } catch (e) {
            console.log('API registration failed, trying localStorage fallback');
        }
    }

    // Fallback to localStorage
    await loadApplicants();
    const nextId = allApplicants.length > 0 ? Math.max(...allApplicants.map(a => a.id)) + 1 : 1000;

    newStudent.id = nextId;
    newStudent.department = 'N/A';
    newStudent.allocated = 0;

    allApplicants.push(newStudent);
    saveApplicants();

    showAlert('regAlert', `Registration Successful! Your Application ID: ${nextId}. Please note your ID for login.`, 'success');
    document.getElementById('registrationForm').reset();

    setTimeout(() => backToStudentPortal(), 3000);
}

// ============================================================
// STUDENT MENU FUNCTIONS
// ============================================================
function viewStudentProfile() {
    if (!currentStudent) return;

    const html = `
        <h3>📋 Your Profile</h3>
        <div class="profile-item"><span class="profile-label">Application ID:</span><span class="profile-value">${currentStudent.id}</span></div>
        <div class="profile-item"><span class="profile-label">Name:</span><span class="profile-value">${currentStudent.name}</span></div>
        <div class="profile-item"><span class="profile-label">Category:</span><span class="profile-value">${currentStudent.category}</span></div>
        <div class="profile-item"><span class="profile-label">HS Marks:</span><span class="profile-value">${currentStudent.marks}</span></div>
        <div class="profile-item"><span class="profile-label">JEE Rank:</span><span class="profile-value">${currentStudent.jee_rank}</span></div>
        <div class="profile-item"><span class="profile-label">Department Preferences:</span><span class="profile-value">${currentStudent.pref.map((p, i) => `${i + 1}. ${p}`).join(', ')}</span></div>
        <div class="profile-item"><span class="profile-label">Department Allotted:</span><span class="profile-value">${currentStudent.department === 'N/A' || currentStudent.department === 'NA' ? 'Not Allotted' : currentStudent.department}</span></div>
    `;

    document.getElementById('profileContent').innerHTML = html;
    showScreen('studentProfileView');
}

function viewMeritRank() {
    if (!currentStudent) return;

    loadApplicants();
    let merit_rank = 1;

    for (const s of allApplicants) {
        if (s.jee_rank < currentStudent.jee_rank) {
            merit_rank++;
        } else if (s.jee_rank === currentStudent.jee_rank && s.marks > currentStudent.marks && s.id !== currentStudent.id) {
            merit_rank++;
        }
    }

    const html = `
        <h3>📊 Your Merit Position</h3>
        <div class="stats-grid">
            <div class="stat-card"><div class="stat-value">${merit_rank}</div><div class="stat-label">Overall Merit Rank</div></div>
            <div class="stat-card"><div class="stat-value">${allApplicants.length}</div><div class="stat-label">Total Applicants</div></div>
        </div>
        <div class="profile-item"><span class="profile-label">JEE Rank:</span><span class="profile-value">${currentStudent.jee_rank}</span></div>
        <div class="profile-item"><span class="profile-label">HS Marks:</span><span class="profile-value">${currentStudent.marks}</span></div>
    `;

    document.getElementById('meritContent').innerHTML = html;
    showScreen('meritRankView');
}

function viewAllocationStatus() {
    if (!currentStudent) return;

    const status = currentStudent.allocated ? 'SELECTED' : 'NOT ALLOTTED';
    const deptText = currentStudent.allocated ? currentStudent.department : 'Awaiting Merit List Generation';

    const html = `
        <h3>✅ Your Allocation Status</h3>
        <div class="profile-item">
            <span class="profile-label">Status:</span>
            <span class="profile-value"><span class="status-badge ${currentStudent.allocated ? 'status-selected' : 'status-waiting'}">${status}</span></span>
        </div>
        <div class="profile-item"><span class="profile-label">Allotted Department:</span><span class="profile-value">${deptText}</span></div>
    `;

    document.getElementById('allocationContent').innerHTML = html;
    showScreen('allocationStatusView');
}

function editPreferences() {
    if (!currentStudent) return;

    document.getElementById('editPref1').value = currentStudent.pref[0];
    document.getElementById('editPref2').value = currentStudent.pref[1];
    document.getElementById('editPref3').value = currentStudent.pref[2];
    document.getElementById('editPref4').value = currentStudent.pref[3];

    showScreen('editPreferencesView');
}

async function savePreferences() {
    const prefs = [
        document.getElementById('editPref1').value,
        document.getElementById('editPref2').value,
        document.getElementById('editPref3').value,
        document.getElementById('editPref4').value
    ];

    if (prefs.some(p => !p)) {
        showAlert('prefsAlert', 'Please select all preferences', 'error');
        return;
    }

    currentStudent.pref = prefs;

    // Update in allApplicants array
    const idx = allApplicants.findIndex(a => a.id === currentStudent.id);
    if (idx !== -1) {
        allApplicants[idx].pref = prefs;
    }

    // Try API first
    if (useAPI) {
        try {
            const response = await fetch(`${API_BASE}/api/applicants/${currentStudent.id}`, {
                method: 'PUT',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ pref: prefs })
            });
            const result = await response.json();
            if (result.success) {
                showAlert('prefsAlert', 'Preferences updated successfully!', 'success');
                await loadApplicants();
                setTimeout(() => backToStudentDashboard(), 1000);
                return;
            }
        } catch (e) {
            console.log('API update failed, using localStorage');
        }
    }

    saveApplicants();
    showAlert('prefsAlert', 'Preferences updated successfully!', 'success');
    setTimeout(() => backToStudentDashboard(), 1000);
}

function changePassword() {
    showScreen('changePasswordView');
}

async function updatePassword() {
    const current = document.getElementById('currentPwd').value;
    const newPwd = document.getElementById('newPwd').value;
    const confirm = document.getElementById('confirmPwd').value;

    if (!current || !newPwd || !confirm) {
        showAlert('pwdAlert', 'Please fill all fields', 'error');
        return;
    }

    if (current !== currentStudent.password) {
        showAlert('pwdAlert', 'Current password is incorrect', 'error');
        return;
    }

    if (newPwd !== confirm) {
        showAlert('pwdAlert', 'New passwords do not match', 'error');
        return;
    }

    if (newPwd.length < 3 || newPwd.length > 9) {
        showAlert('pwdAlert', 'Password must be 3-9 characters', 'error');
        return;
    }

    currentStudent.password = newPwd;

    const idx = allApplicants.findIndex(a => a.id === currentStudent.id);
    if (idx !== -1) {
        allApplicants[idx].password = newPwd;
    }

    // Try API first
    if (useAPI) {
        try {
            const response = await fetch(`${API_BASE}/api/applicants/${currentStudent.id}`, {
                method: 'PUT',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ password: newPwd })
            });
            const result = await response.json();
            if (result.success) {
                showAlert('pwdAlert', 'Password changed successfully!', 'success');
                document.getElementById('currentPwd').value = '';
                document.getElementById('newPwd').value = '';
                document.getElementById('confirmPwd').value = '';
                await loadApplicants();
                setTimeout(() => backToStudentDashboard(), 1000);
                return;
            }
        } catch (e) {
            console.log('API update failed, using localStorage');
        }
    }

    saveApplicants();
    showAlert('pwdAlert', 'Password changed successfully!', 'success');

    document.getElementById('currentPwd').value = '';
    document.getElementById('newPwd').value = '';
    document.getElementById('confirmPwd').value = '';

    setTimeout(() => backToStudentDashboard(), 1000);
}

function studentLogout() {
    if (confirm('Are you sure you want to logout?')) {
        currentStudent = null;
        backToMainMenu();
    }
}

// ============================================================
// ADMIN LOGIN - Step by Step (Matching Terminal Flow)
// ============================================================
function resetAdminLogin() {
    adminLoginAttempts = 3;
    verifiedAdminCredentials = null;
    document.getElementById('adminStep1').style.display = 'block';
    document.getElementById('adminStep2').style.display = 'none';
    document.getElementById('adminStep3').style.display = 'none';
    document.getElementById('adminEmpId').value = '';
    document.getElementById('adminUsername').value = '';
    document.getElementById('adminPassword').value = '';
    document.getElementById('adminAttemptsInfo').innerHTML = '';
    document.getElementById('adminStatusMessages').innerHTML = '';
    hideAlert('adminLoginAlert');
}

function verifyAdminEmpId() {
    const empId = parseInt(document.getElementById('adminEmpId').value);

    if (!empId) {
        showAlert('adminLoginAlert', 'Please enter Employee ID', 'error');
        return;
    }

    const admin = adminCredentials.find(a => a.empId === empId);

    if (!admin) {
        addStatusMessage('adminStatusMessages', `Employee ID ${empId} not found`, 'error');
        adminLoginAttempts--;
        updateAttemptsInfo('adminAttemptsInfo', adminLoginAttempts);

        if (adminLoginAttempts <= 0) {
            showAlert('adminLoginAlert', 'Login failed after 3 attempts. Returning to main menu...', 'error');
            setTimeout(() => backToMainMenu(), 2000);
        }
        return;
    }

    verifiedAdminCredentials = admin;
    addStatusMessage('adminStatusMessages', `Employee ID ${empId} verified successfully`, 'success');

    document.getElementById('adminStep1').style.display = 'none';
    document.getElementById('adminStep2').style.display = 'block';
    hideAlert('adminLoginAlert');
}

function verifyAdminUsername() {
    const username = document.getElementById('adminUsername').value.trim();

    if (!username) {
        showAlert('adminLoginAlert', 'Please enter username', 'error');
        return;
    }

    document.getElementById('adminStep2').style.display = 'none';
    document.getElementById('adminStep3').style.display = 'block';
}

function performAdminLogin() {
    const username = document.getElementById('adminUsername').value.trim();
    const password = document.getElementById('adminPassword').value;

    if (!password) {
        showAlert('adminLoginAlert', 'Please enter password', 'error');
        return;
    }

    if (username !== verifiedAdminCredentials.username || password !== verifiedAdminCredentials.password) {
        addStatusMessage('adminStatusMessages', 'Invalid username or password', 'error');
        adminLoginAttempts--;
        updateAttemptsInfo('adminAttemptsInfo', adminLoginAttempts);

        if (adminLoginAttempts <= 0) {
            showAlert('adminLoginAlert', 'Login failed after 3 attempts. Returning to main menu...', 'error');
            setTimeout(() => backToMainMenu(), 2000);
        }
        return;
    }

    adminLoggedIn = true;
    addStatusMessage('adminStatusMessages', 'Login successful!', 'success');

    showAlert('adminLoginAlert', 'Welcome, Admin!', 'success');
    setTimeout(() => showScreen('adminDashboard'), 1000);
}

function adminLogout() {
    if (confirm('Are you sure you want to logout?')) {
        adminLoggedIn = false;
        backToMainMenu();
    }
}

// ============================================================
// ADMIN MENU FUNCTIONS
// ============================================================
function viewAllApplicants() {
    loadApplicants();
    renderApplicantsTable(allApplicants, 'allApplicantsTable');
    showScreen('viewAllApplicantsScreen');
}

function searchApplicants() {
    showScreen('searchApplicantsScreen');
    document.getElementById('searchIdSection').style.display = 'none';
    document.getElementById('searchNameSection').style.display = 'none';
}

function showSearchById() {
    document.getElementById('searchIdSection').style.display = 'block';
    document.getElementById('searchNameSection').style.display = 'none';
}

function showSearchByName() {
    document.getElementById('searchIdSection').style.display = 'none';
    document.getElementById('searchNameSection').style.display = 'block';
}

function performSearchById() {
    const id = parseInt(document.getElementById('searchId').value);
    loadApplicants();
    const result = allApplicants.filter(a => a.id === id);
    renderApplicantsTable(result, 'searchIdResult');
}

function performSearchByName() {
    const name = document.getElementById('searchName').value.toLowerCase();
    loadApplicants();
    const result = allApplicants.filter(a => a.name.toLowerCase().includes(name));
    renderApplicantsTable(result, 'searchNameResult');
}

function viewMeritList() {
    loadApplicants();
    const selected = allApplicants.filter(a => a.allocated === 1);
    renderApplicantsTable(selected, 'meritListTable');

    const deptCounts = { CSE: 0, IT: 0, TT: 0, APM: 0 };
    selected.forEach(a => { if (deptCounts[a.department] !== undefined) deptCounts[a.department]++; });

    document.getElementById('meritStats').innerHTML = `
        <div class="stat-card"><div class="stat-value">${selected.length}</div><div class="stat-label">Total Selected</div></div>
        <div class="stat-card"><div class="stat-value">${deptCounts.CSE}</div><div class="stat-label">CSE</div></div>
        <div class="stat-card"><div class="stat-value">${deptCounts.IT}</div><div class="stat-label">IT</div></div>
        <div class="stat-card"><div class="stat-value">${deptCounts.TT}</div><div class="stat-label">TT</div></div>
        <div class="stat-card"><div class="stat-value">${deptCounts.APM}</div><div class="stat-label">APM</div></div>
    `;
    showScreen('meritListScreen');
}

function viewWaitingList() {
    loadApplicants();
    const waiting = allApplicants.filter(a => a.allocated === 0);
    renderApplicantsTable(waiting, 'waitingListTable');
    showScreen('waitingListScreen');
}

function viewCategoryWise() {
    showScreen('categoryWiseScreen');
    document.getElementById('categorySelect').value = '';
    document.getElementById('categoryMeritTable').innerHTML = '';
}

function viewSelectedCategory() {
    const category = document.getElementById('categorySelect').value;
    if (!category) return;
    loadApplicants();
    const filtered = allApplicants.filter(a => a.category === category && a.allocated === 1);
    renderApplicantsTable(filtered, 'categoryMeritTable');
}

function viewDepartmentWise() {
    showScreen('departmentWiseScreen');
    document.getElementById('departmentSelect').value = '';
    document.getElementById('departmentMeritTable').innerHTML = '';
}

function viewSelectedDepartment() {
    const dept = document.getElementById('departmentSelect').value;
    if (!dept) return;
    loadApplicants();
    const filtered = allApplicants.filter(a => a.department === dept && a.allocated === 1);
    renderApplicantsTable(filtered, 'departmentMeritTable');
}

function generateMeritList() {
    showScreen('generateMeritScreen');
}

async function performGenerateMeritList() {
    const algorithm = document.getElementById('sortAlgorithm').value || 'merge';

    // Try API first
    if (useAPI) {
        try {
            const response = await fetch(`${API_BASE}/api/generate-merit`, {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ algorithm: algorithm })
            });
            const result = await response.json();

            if (result.success) {
                showAlert('meritGenAlert',
                    `Merit list generated! ${result.allocated} students allocated across departments. ` +
                    `(CSE: ${result.seats.CSE}, IT: ${result.seats.IT}, TT: ${result.seats.TT}, APM: ${result.seats.APM})`,
                    'success');
                await loadApplicants();
                setTimeout(() => backToAdminDashboard(), 2000);
                return;
            }
        } catch (e) {
            console.log('API merit generation failed, using local fallback');
        }
    }

    // Fallback to local
    await loadApplicants();
    if (allApplicants.length === 0) {
        showAlert('meritGenAlert', 'No applicants found', 'error');
        return;
    }

    // Sort applicants by JEE rank (lower is better)
    allApplicants.sort((a, b) => a.jee_rank - b.jee_rank);

    // Allocate departments (10 seats each)
    const seatAlloc = { CSE: 0, IT: 0, TT: 0, APM: 0 };
    const maxSeats = 10;

    for (let applicant of allApplicants) {
        applicant.allocated = 0;
        applicant.department = 'NA';

        for (let dept of applicant.pref) {
            if (seatAlloc[dept] !== undefined && seatAlloc[dept] < maxSeats) {
                seatAlloc[dept]++;
                applicant.allocated = 1;
                applicant.department = dept;
                break;
            }
        }
    }

    saveApplicants();

    const allocatedCount = allApplicants.filter(a => a.allocated).length;
    showAlert('meritGenAlert', `Merit list generated using ${algorithm} sort! ${allocatedCount} students allocated across departments.`, 'success');

    setTimeout(() => backToAdminDashboard(), 2000);
}

// ============================================================
// UTILITY FUNCTIONS
// ============================================================
function renderApplicantsTable(data, containerId) {
    const container = document.getElementById(containerId);

    if (data.length === 0) {
        container.innerHTML = '<div class="no-data"><div class="no-data-icon">📭</div><p>No records found</p></div>';
        return;
    }

    let html = `<table>
        <thead><tr>
            <th>#</th><th>ID</th><th>Name</th><th>Category</th><th>JEE Rank</th><th>Marks</th><th>Dept</th><th>Status</th>
        </tr></thead><tbody>`;

    data.forEach((app, idx) => {
        html += `<tr>
            <td>${idx + 1}</td>
            <td>${app.id}</td>
            <td>${app.name}</td>
            <td>${app.category}</td>
            <td>${app.jee_rank}</td>
            <td>${app.marks}</td>
            <td>${app.department}</td>
            <td><span class="status-badge ${app.allocated ? 'status-selected' : 'status-waiting'}">${app.allocated ? 'SELECTED' : 'WAITING'}</span></td>
        </tr>`;
    });

    html += '</tbody></table>';
    container.innerHTML = html;
}

function showAlert(alertId, message, type) {
    const alert = document.getElementById(alertId);
    alert.textContent = message;
    alert.className = `alert alert-${type} show`;

    if (type === 'success' || type === 'error') {
        setTimeout(() => alert.classList.remove('show'), 5000);
    }
}

function hideAlert(alertId) {
    const alert = document.getElementById(alertId);
    alert.classList.remove('show');
}

// API Base URL - uses same origin
const API_BASE = '';

// Check if API server is available
let useAPI = false;

async function checkAPIAvailability() {
    try {
        // Try to fetch applicants - if it returns JSON array, API is available
        const response = await fetch(`${API_BASE}/api/applicants`);
        if (response.ok) {
            const data = await response.json();
            if (Array.isArray(data)) {
                useAPI = true;
                allApplicants = data; // Use the fetched data
                console.log('API server: Connected (' + data.length + ' applicants loaded)');
                return;
            }
        }
        useAPI = false;
        console.log('API server: Not available, using localStorage');
    } catch (e) {
        useAPI = false;
        console.log('API server not available, using localStorage/CSV fallback');
    }
}

async function loadApplicants() {
    if (useAPI) {
        try {
            const response = await fetch(`${API_BASE}/api/applicants`);
            if (response.ok) {
                allApplicants = await response.json();
                return;
            }
        } catch (e) {
            console.log('API fetch failed, falling back to localStorage');
        }
    }

    // Fallback to localStorage
    const stored = localStorage.getItem('applicants');
    if (stored) {
        allApplicants = JSON.parse(stored);
    } else {
        // Fetch from CSV
        try {
            const response = await fetch('applicants_full.csv');
            const csv = await response.text();
            parseApplicantsCSV(csv);
            saveApplicantsLocal();
        } catch {
            allApplicants = [];
        }
    }
}

function parseApplicantsCSV(csv) {
    const lines = csv.trim().split('\n');
    allApplicants = [];
    for (let i = 1; i < lines.length; i++) {
        const parts = lines[i].split(',');
        if (parts.length >= 12) {
            allApplicants.push({
                id: parseInt(parts[0]),
                name: parts[1],
                password: parts[2],
                category: parts[3],
                pref: [parts[4], parts[5], parts[6], parts[7]],
                department: parts[8],
                marks: parseInt(parts[9]),
                jee_rank: parseInt(parts[10]),
                allocated: parseInt(parts[11])
            });
        }
    }
}

function saveApplicantsLocal() {
    localStorage.setItem('applicants', JSON.stringify(allApplicants));
}

async function saveApplicants() {
    saveApplicantsLocal();
    // API updates are done via specific endpoints
}

async function updateApplicantAPI(id, data) {
    if (useAPI) {
        try {
            await fetch(`${API_BASE}/api/applicants/${id}`, {
                method: 'PUT',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify(data)
            });
        } catch (e) {
            console.log('API update failed');
        }
    }
}

async function registerStudentAPI(data) {
    if (useAPI) {
        try {
            const response = await fetch(`${API_BASE}/api/register`, {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify(data)
            });
            return await response.json();
        } catch (e) {
            console.log('API registration failed');
            return null;
        }
    }
    return null;
}

async function generateMeritAPI() {
    if (useAPI) {
        try {
            const response = await fetch(`${API_BASE}/api/generate-merit`, {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' }
            });
            const result = await response.json();
            if (result.success) {
                await loadApplicants(); // Reload data
                return result;
            }
        } catch (e) {
            console.log('API merit generation failed');
        }
    }
    return null;
}

// ============================================================
// INITIALIZATION
// ============================================================
window.addEventListener('load', async () => {
    await checkAPIAvailability();
    await loadApplicants();
});

