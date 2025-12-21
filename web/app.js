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
});

// Auth Tab Switching
function switchAuthTab(tab) {
    const tabs = document.querySelectorAll('.tab');
    const loginForm = document.getElementById('loginForm');
    const registerForm = document.getElementById('registerForm');
    
    tabs.forEach(t => t.classList.remove('active'));
    
    if (tab === 'login') {
        tabs[0].classList.add('active');
        loginForm.classList.remove('hidden');
        registerForm.classList.add('hidden');
    } else {
        tabs[1].classList.add('active');
        loginForm.classList.add('hidden');
        registerForm.classList.remove('hidden');
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
    document.getElementById('authSection').classList.add('hidden');
    document.getElementById('vaultSection').classList.remove('hidden');
    document.getElementById('usernameDisplay').textContent = currentSession.username;
    loadPasswords();
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
    
    container.innerHTML = passwords.map(pwd => `
        <div class="password-card">
            <div class="password-header">
                <div class="password-site">${escapeHtml(pwd.site)}</div>
                ${pwd.category ? `<span class="password-category">${escapeHtml(pwd.category)}</span>` : ''}
            </div>
            <div class="password-info">
                <div class="password-field"><strong>Username:</strong> ${escapeHtml(pwd.username)}</div>
                <div class="password-field"><strong>Password:</strong> <span id="pwd-${pwd.id}">••••••••</span></div>
                ${pwd.notes ? `<div class="password-field"><strong>Notes:</strong> ${escapeHtml(pwd.notes)}</div>` : ''}
            </div>
            <div class="password-actions">
                <button class="icon-btn" onclick='togglePassword("${pwd.id}", "${escapeHtml(pwd.password)}")'>Show</button>
                <button class="icon-btn" onclick='copyPassword("${escapeHtml(pwd.password)}")'>Copy</button>
                <button class="icon-btn" onclick='editPassword(${JSON.stringify(pwd).replace(/'/g, "&apos;")})'>Edit</button>
                <button class="icon-btn" onclick='deletePassword("${pwd.id}", "${escapeHtml(pwd.site)}")'>Delete</button>
            </div>
        </div>
    `).join('');
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
