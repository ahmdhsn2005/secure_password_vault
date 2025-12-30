// API Configuration
const API_URL = 'http://localhost:8080/api';

// Session storage
let currentSession = {
    username: null,
    sessionToken: null
};

let allPasswords = [];

// Initialize
document.addEventListener('DOMContentLoaded', () => {
    setupAuthForms();
    checkSession();
    setupPasswordStrengthChecker();
});

// Password strength checker
function checkPasswordStrength(password) {
    let strength = 0;
    const feedback = [];
    
    if (password.length >= 8) strength++;
    if (password.length >= 12) strength++;
    if (/[a-z]/.test(password)) strength++;
    if (/[A-Z]/.test(password)) strength++;
    if (/[0-9]/.test(password)) strength++;
    if (/[^a-zA-Z0-9]/.test(password)) strength++;
    
    let level, color, width, text;
    if (strength < 3) {
        level = 'weak';
        color = '#ef4444';
        width = '25%';
        text = 'Weak';
    } else if (strength < 5) {
        level = 'fair';
        color = '#f59e0b';
        width = '50%';
        text = 'Fair';
    } else if (strength < 6) {
        level = 'good';
        color = '#3b82f6';
        width = '75%';
        text = 'Good';
    } else {
        level = 'strong';
        color = '#10b981';
        width = '100%';
        text = 'Strong';
    }
    
    return { level, color, width, text };
}

function setupPasswordStrengthChecker() {
    const passwordInput = document.getElementById('password');
    if (passwordInput) {
        passwordInput.addEventListener('input', (e) => {
            const password = e.target.value;
            if (password) {
                const strength = checkPasswordStrength(password);
                const fill = document.getElementById('strengthFill');
                const text = document.getElementById('strengthText');
                
                if (fill && text) {
                    fill.style.width = strength.width;
                    fill.style.background = strength.color;
                    text.textContent = strength.text;
                    text.className = `strength-text strength-${strength.level}`;
                }
            } else {
                const fill = document.getElementById('strengthFill');
                const text = document.getElementById('strengthText');
                if (fill && text) {
                    fill.style.width = '0';
                    text.textContent = '';
                }
            }
        });
    }
}

function generatePassword() {
    const length = 16;
    const charset = 'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()_+-=[]{}|;:,.<>?';
    let password = '';
    
    // Ensure at least one of each type
    password += 'abcdefghijklmnopqrstuvwxyz'[Math.floor(Math.random() * 26)];
    password += 'ABCDEFGHIJKLMNOPQRSTUVWXYZ'[Math.floor(Math.random() * 26)];
    password += '0123456789'[Math.floor(Math.random() * 10)];
    password += '!@#$%^&*()_+-='[Math.floor(Math.random() * 14)];
    
    // Fill the rest
    for (let i = password.length; i < length; i++) {
        password += charset[Math.floor(Math.random() * charset.length)];
    }
    
    // Shuffle
    password = password.split('').sort(() => Math.random() - 0.5).join('');
    
    document.getElementById('password').value = password;
    document.getElementById('password').type = 'text';
    document.getElementById('password').dispatchEvent(new Event('input'));
    
    setTimeout(() => {
        document.getElementById('password').type = 'password';
    }, 2000);
    
    showToast('Strong password generated');
}

function updateStats() {
    const total = allPasswords.length;
    let strong = 0;
    let weak = 0;
    
    allPasswords.forEach(pwd => {
        const strength = checkPasswordStrength(pwd.password);
        if (strength.level === 'strong' || strength.level === 'good') {
            strong++;
        } else {
            weak++;
        }
    });
    
    document.getElementById('totalPasswords').textContent = total;
    document.getElementById('strongPasswords').textContent = strong;
    document.getElementById('weakPasswords').textContent = weak;
}

// Page Navigation
function showLoginPage() {
    document.getElementById('loginPage').classList.remove('hidden');
    document.getElementById('registerPage').classList.add('hidden');
    document.getElementById('vaultSection').classList.add('hidden');
    document.getElementById('loginForm').reset();
    document.getElementById('loginMessage').innerHTML = '';
}

function showRegisterPage() {
    document.getElementById('loginPage').classList.add('hidden');
    document.getElementById('registerPage').classList.remove('hidden');
    document.getElementById('vaultSection').classList.add('hidden');
    document.getElementById('registerForm').reset();
    document.getElementById('registerMessage').innerHTML = '';
}

// Auth Tab Switching
function switchAuthTab(tab) {
    // Deprecated - kept for compatibility
    if (tab === 'login') {
        showLoginPage();
    } else {
        showRegisterPage();
    }
}

// Setup Auth Forms
function setupAuthForms() {
    document.getElementById('loginForm').addEventListener('submit', handleLogin);
    document.getElementById('registerForm').addEventListener('submit', handleRegister);
    document.getElementById('passwordForm').addEventListener('submit', handlePasswordSubmit);
}

// Check if user is logged in
function checkSession() {
    const username = localStorage.getItem('username');
    const sessionToken = localStorage.getItem('sessionToken');
    
    if (username && sessionToken) {
        currentSession.username = username;
        currentSession.sessionToken = sessionToken;
        showVault();
    }
}

// Handle Login
async function handleLogin(e) {
    e.preventDefault();
    
    const username = document.getElementById('loginUsername').value;
    const password = document.getElementById('loginPassword').value;
    
    try {
        const response = await fetch(`${API_URL}/login`, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify({ username, password })
        });
        
        const data = await response.json();
        
        if (data.success) {
            currentSession.username = username;
            currentSession.sessionToken = data.sessionToken;
            
            // Save to localStorage
            localStorage.setItem('username', username);
            localStorage.setItem('sessionToken', data.sessionToken);
            
            showMessage('loginMessage', 'Login successful', 'success');
            setTimeout(() => showVault(), 500);
        } else {
            showMessage('loginMessage', data.message || 'Login failed', 'error');
        }
    } catch (error) {
        showMessage('loginMessage', 'Server connection failed', 'error');
        console.error('Login error:', error);
    }
}

// Handle Register
async function handleRegister(e) {
    e.preventDefault();
    
    const username = document.getElementById('registerUsername').value;
    const password = document.getElementById('registerPassword').value;
    const confirm = document.getElementById('registerConfirm').value;
    
    if (password !== confirm) {
        showMessage('registerMessage', 'Passwords do not match', 'error');
        return;
    }
    
    try {
        const response = await fetch(`${API_URL}/register`, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify({ username, password })
        });
        
        const data = await response.json();
        
        if (data.success) {
            showMessage('registerMessage', 'Registration successful! Please login.', 'success');
            setTimeout(() => switchAuthTab('login'), 1500);
        } else {
            showMessage('registerMessage', data.message || 'Registration failed', 'error');
        }
    } catch (error) {
        showMessage('registerMessage', 'Server connection failed', 'error');
        console.error('Register error:', error);
    }
}

// Show Vault
function showVault() {
    document.getElementById('loginPage').classList.add('hidden');
    document.getElementById('registerPage').classList.add('hidden');
    document.getElementById('vaultSection').classList.remove('hidden');
    document.getElementById('usernameDisplay').textContent = currentSession.username;
    loadPasswords();
    setupPasswordStrengthChecker();
}

// Logout
function logout() {
    localStorage.removeItem('username');
    localStorage.removeItem('sessionToken');
    currentSession = { username: null, sessionToken: null };
    
    document.getElementById('vaultSection').classList.add('hidden');
    document.getElementById('authSection').classList.remove('hidden');
    
    // Clear forms
    document.getElementById('loginForm').reset();
    document.getElementById('registerForm').reset();
    
    showToast('Logged out successfully');
}

// Load Passwords
async function loadPasswords() {
    try {
        const response = await fetch(`${API_URL}/passwords`, {
            method: 'GET',
            headers: {
                'Authorization': currentSession.sessionToken,
                'X-Username': currentSession.username
            }
        });
        
        const data = await response.json();
        
        if (data.success) {
            allPasswords = data.passwords || [];
            updateStats();
            renderPasswords(allPasswords);
        } else {
            showToast('Failed to load passwords');
        }
    } catch (error) {
        showToast('Server connection failed');
        console.error('Load passwords error:', error);
    }
}

// Render Passwords
function renderPasswords(passwords) {
    const container = document.getElementById('passwordsContainer');
    const emptyState = document.getElementById('emptyState');
    
    if (passwords.length === 0) {
        container.innerHTML = '';
        emptyState.classList.remove('hidden');
        return;
    }
    
    emptyState.classList.add('hidden');
    
    container.innerHTML = passwords.map(pwd => {
        const strength = checkPasswordStrength(pwd.password);
        return `
        <div class="password-card">
            <div class="password-header">
                <div class="password-site">${escapeHtml(pwd.site)}</div>
                ${pwd.category ? `<span class="password-category">${escapeHtml(pwd.category)}</span>` : ''}
            </div>
            <div class="password-info">
                <div class="password-field"><strong>Username:</strong> ${escapeHtml(pwd.username)}</div>
                <div class="password-field">
                    <strong>Password:</strong> 
                    <span id="pwd-${pwd.id}">••••••••</span>
                    <span class="strength-text strength-${strength.level}" style="margin-left: 8px;">${strength.text}</span>
                </div>
                ${pwd.notes ? `<div class="password-field"><strong>Notes:</strong> ${escapeHtml(pwd.notes)}</div>` : ''}
            </div>
            <div class="password-actions">
                <button class="icon-btn" onclick='togglePassword("${pwd.id}", "${escapeHtml(pwd.password)}")'>Show</button>
                <button class="icon-btn" onclick='copyPassword("${escapeHtml(pwd.password)}")'>Copy</button>
                <button class="icon-btn" onclick='editPassword(${JSON.stringify(pwd).replace(/'/g, "&apos;")})'>Edit</button>
                <button class="icon-btn" onclick='deletePassword("${pwd.id}", "${escapeHtml(pwd.site)}")'>Delete</button>
            </div>
        </div>
        `;
    }).join('');
}

// Search Passwords
function searchPasswords() {
    const query = document.getElementById('searchInput').value.toLowerCase();
    
    if (!query) {
        renderPasswords(allPasswords);
        return;
    }
    
    const filtered = allPasswords.filter(pwd => 
        pwd.site.toLowerCase().includes(query) ||
        (pwd.category && pwd.category.toLowerCase().includes(query)) ||
        pwd.username.toLowerCase().includes(query)
    );
    
    renderPasswords(filtered);
}

// Toggle Password Visibility
function togglePassword(id, password) {
    const element = document.getElementById(`pwd-${id}`);
    const button = event.target;
    
    if (element.textContent === '••••••••') {
        element.textContent = password;
        button.textContent = 'Hide';
    } else {
        element.textContent = '••••••••';
        button.textContent = 'Show';
    }
}

// Copy Password
function copyPassword(password) {
    navigator.clipboard.writeText(password).then(() => {
        showToast('Password copied to clipboard');
    }).catch(() => {
        showToast('Failed to copy password');
    });
}

// Open Add Password Modal
function openAddPasswordModal() {
    document.getElementById('modalTitle').textContent = 'Add Password';
    document.getElementById('passwordForm').reset();
    document.getElementById('editPasswordId').value = '';
    document.getElementById('passwordModal').classList.remove('hidden');
    setupPasswordStrengthChecker();
}

// Edit Password
function editPassword(pwd) {
    document.getElementById('modalTitle').textContent = 'Edit Password';
    document.getElementById('editPasswordId').value = pwd.id;
    document.getElementById('siteName').value = pwd.site;
    document.getElementById('username').value = pwd.username;
    document.getElementById('password').value = pwd.password;
    document.getElementById('category').value = pwd.category || '';
    document.getElementById('notes').value = pwd.notes || '';
    document.getElementById('passwordModal').classList.remove('hidden');
    setupPasswordStrengthChecker();
    document.getElementById('password').dispatchEvent(new Event('input'));
}

// Close Password Modal
function closePasswordModal() {
    document.getElementById('passwordModal').classList.add('hidden');
}

// Handle Password Submit
async function handlePasswordSubmit(e) {
    e.preventDefault();
    
    const passwordId = document.getElementById('editPasswordId').value;
    const site = document.getElementById('siteName').value;
    const username = document.getElementById('username').value;
    const password = document.getElementById('password').value;
    const category = document.getElementById('category').value;
    const notes = document.getElementById('notes').value;
    
    const endpoint = passwordId ? `${API_URL}/passwords/${passwordId}` : `${API_URL}/passwords`;
    const method = passwordId ? 'PUT' : 'POST';
    
    try {
        const response = await fetch(endpoint, {
            method: method,
            headers: {
                'Content-Type': 'application/json',
                'Authorization': currentSession.sessionToken,
                'X-Username': currentSession.username
            },
            body: JSON.stringify({ site, username, password, category, notes })
        });
        
        const data = await response.json();
        
        if (data.success) {
            showToast(passwordId ? 'Password updated' : 'Password added');
            closePasswordModal();
            loadPasswords();
        } else {
            showToast(data.message || 'Operation failed');
        }
    } catch (error) {
        showToast('Server connection failed');
        console.error('Password submit error:', error);
    }
}

// Delete Password
async function deletePassword(id, site) {
    if (!confirm(`Delete password for ${site}?`)) {
        return;
    }
    
    try {
        const response = await fetch(`${API_URL}/passwords/${id}`, {
            method: 'DELETE',
            headers: {
                'Authorization': currentSession.sessionToken,
                'X-Username': currentSession.username
            }
        });
        
        const data = await response.json();
        
        if (data.success) {
            showToast('Password deleted');
            loadPasswords();
        } else {
            showToast(data.message || 'Delete failed');
        }
    } catch (error) {
        showToast('Server connection failed');
        console.error('Delete error:', error);
    }
}

// Show Message in Form
function showMessage(elementId, message, type) {
    const element = document.getElementById(elementId);
    element.className = `message ${type}`;
    element.textContent = message;
}

// Show Toast Notification
function showToast(message) {
    const toast = document.getElementById('toast');
    toast.textContent = message;
    toast.classList.add('show');
    
    setTimeout(() => {
        toast.classList.remove('show');
    }, 3000);
}

// Escape HTML to prevent XSS
function escapeHtml(text) {
    const div = document.createElement('div');
    div.textContent = text;
    return div.innerHTML;
}

// Logout
function logout() {
    localStorage.removeItem('username');
    localStorage.removeItem('sessionToken');
    currentSession.username = null;
    currentSession.sessionToken = null;
    allPasswords = [];
    showLoginPage();
}
