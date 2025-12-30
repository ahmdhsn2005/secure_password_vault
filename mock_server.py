#!/usr/bin/env python3
"""
Mock API Server for Password Vault
Provides endpoints for authentication and password management
"""

from http.server import HTTPServer, BaseHTTPRequestHandler
import json
import hashlib
import secrets
from urllib.parse import urlparse, parse_qs
import time

# In-memory storage
users = {}
sessions = {}
passwords = {}

# Pre-populate with demo data
def initialize_demo_data():
    """Initialize the server with demo users and passwords"""
    demo_users = [
        {'username': 'john_doe', 'password': 'SecurePass123!'},
        {'username': 'sarah_smith', 'password': 'MyPassword456@'},
        {'username': 'mike_wilson', 'password': 'StrongPwd789#'},
        {'username': 'emily_brown', 'password': 'SafePass321$'},
        {'username': 'david_jones', 'password': 'SecureKey999%'},
        {'username': 'alex_garcia', 'password': 'AlexG@2024!'},
        {'username': 'lisa_anderson', 'password': 'Lisa#Secure456'},
        {'username': 'james_taylor', 'password': 'JamesT@y789!'},
        {'username': 'maria_martinez', 'password': 'Maria#Pass321'},
        {'username': 'robert_thomas', 'password': 'Rob3rt#2024!'},
        {'username': 'jennifer_lee', 'password': 'J3nnifer@Lee!'},
        {'username': 'william_white', 'password': 'Will!@m#White'},
        {'username': 'jessica_harris', 'password': 'Jess!ca#H2024'},
        {'username': 'michael_clark', 'password': 'Mich@el#Clark!'},
        {'username': 'ashley_lewis', 'password': 'Ashl3y@Lewis24'},
        {'username': 'christopher_walker', 'password': 'Chr!s#Walk3r'},
        {'username': 'amanda_hall', 'password': 'Am@nda#Hall!'},
        {'username': 'matthew_allen', 'password': 'M@tthew#2024!'},
        {'username': 'stephanie_young', 'password': 'St3ph@nie#24'},
        {'username': 'daniel_king', 'password': 'Dan!el#King2024'},
        {'username': 'nicole_wright', 'password': 'Nic0le@Wright!'},
        {'username': 'andrew_lopez', 'password': 'Andr3w#Lopez!'},
        {'username': 'rachel_hill', 'password': 'R@chel#Hill24'},
        {'username': 'joshua_scott', 'password': 'J0shua@Scott!'},
        {'username': 'laura_green', 'password': 'L@ura#Green24!'},
        {'username': 'ryan_adams', 'password': 'Ry@n#Adams2024'},
        {'username': 'michelle_baker', 'password': 'Mich3lle@Baker!'},
        {'username': 'kevin_nelson', 'password': 'K3vin#Nelson!'},
        {'username': 'kimberly_carter', 'password': 'Kim@Carter2024!'},
        {'username': 'brian_mitchell', 'password': 'Bri@n#Mitchell!'},
        {'username': 'angela_perez', 'password': 'Ang3la@Perez24'},
        {'username': 'eric_roberts', 'password': 'Er!c#Roberts24'},
        {'username': 'samantha_turner', 'password': 'S@mantha#Turn3r'},
        {'username': 'jason_phillips', 'password': 'J@son#Phillips!'},
        {'username': 'elizabeth_campbell', 'password': 'Eliz@beth#2024'},
        {'username': 'adam_parker', 'password': 'Ad@m#Parker!24'},
        {'username': 'heather_evans', 'password': 'He@ther#Evans!'},
        {'username': 'justin_edwards', 'password': 'Just!n@Edwards'},
        {'username': 'melissa_collins', 'password': 'Mel!ssa#2024!'},
        {'username': 'brandon_stewart', 'password': 'Br@ndon#Stew@rt'},
        {'username': 'crystal_sanchez', 'password': 'Cryst@l#S2024!'},
        {'username': 'tyler_morris', 'password': 'Tyl3r@Morris!'},
        {'username': 'rebecca_rogers', 'password': 'Reb3cca#Rogers'},
        {'username': 'jacob_reed', 'password': 'J@cob#Reed2024!'},
        {'username': 'tiffany_cook', 'password': 'T!ffany@Cook24'},
        {'username': 'nathan_morgan', 'password': 'N@than#Morgan!'},
        {'username': 'brittany_bell', 'password': 'Br!ttany@Bell'},
        {'username': 'patrick_murphy', 'password': 'P@trick#Murphy!'},
        {'username': 'victoria_bailey', 'password': 'V!ctoria#2024'},
        {'username': 'sean_rivera', 'password': 'Se@n#Rivera24!'},
    ]
    
    # Common password templates
    sites = ['Gmail', 'GitHub', 'LinkedIn', 'Netflix', 'AWS Console', 'Spotify', 'Amazon', 'Facebook', 'Instagram', 'Twitter', 
             'Dropbox', 'Apple ID', 'PayPal', 'Zoom', 'Slack', 'Microsoft 365', 'Google Drive', 'Docker Hub', 'Jira', 'BitBucket']
    categories = ['Email', 'Work', 'Social', 'Entertainment', 'Banking', 'Shopping', 'Storage', 'Productivity', 'Security']
    
    demo_passwords = {}
    
    # Generate 10-15 passwords for each user
    for user in demo_users:
        username = user['username']
        demo_passwords[username] = []
        num_passwords = 10 + (hash(username) % 6)  # 10-15 passwords per user
        
        for i in range(num_passwords):
            site = sites[i % len(sites)]
            demo_passwords[username].append({
                'site': site,
                'username': f"{username}@example.com",
                'password': f"{site[:3]}@{username[:4]}{i}#2024!",
                'category': categories[i % len(categories)],
                'notes': f'Account for {site}'
            })
    
    # Create users
    for user in demo_users:
        password_hash = hashlib.sha256(user['password'].encode()).hexdigest()
        users[user['username']] = password_hash
        passwords[user['username']] = []
        print(f"Created demo user: {user['username']} with hash: {password_hash[:16]}...")
    
    # Add passwords
    for username, pwd_list in demo_passwords.items():
        for pwd in pwd_list:
            password_id = secrets.token_hex(8)
            passwords[username].append({
                'id': password_id,
                'site': pwd['site'],
                'username': pwd['username'],
                'password': pwd['password'],
                'category': pwd['category'],
                'notes': pwd['notes'],
                'created_at': int(time.time())
            })
    
    print(f"✓ Initialized {len(demo_users)} demo users with {sum(len(p) for p in demo_passwords.values())} passwords")

# Initialize on startup
initialize_demo_data()

class PasswordVaultHandler(BaseHTTPRequestHandler):
    
    def _set_headers(self, status=200):
        self.send_response(status)
        self.send_header('Content-Type', 'application/json')
        self.send_header('Access-Control-Allow-Origin', '*')
        self.send_header('Access-Control-Allow-Methods', 'GET, POST, PUT, DELETE, OPTIONS')
        self.send_header('Access-Control-Allow-Headers', 'Content-Type, Authorization, X-Username')
        self.end_headers()
    
    def do_OPTIONS(self):
        self._set_headers(200)
    
    def do_GET(self):
        parsed_path = urlparse(self.path)
        path = parsed_path.path
        
        if path == '/api/passwords':
            self.handle_get_passwords()
        else:
            self._set_headers(404)
            self.wfile.write(json.dumps({'success': False, 'message': 'Not found'}).encode())
    
    def do_POST(self):
        print(f"Received POST request to: {self.path}")
        content_length = int(self.headers.get('Content-Length', 0))
        body = self.rfile.read(content_length).decode() if content_length > 0 else '{}'
        print(f"Request body: {body}")
        
        try:
            data = json.loads(body)
        except:
            print("Failed to parse JSON")
            self._set_headers(400)
            self.wfile.write(json.dumps({'success': False, 'message': 'Invalid JSON'}).encode())
            return
        
        parsed_path = urlparse(self.path)
        path = parsed_path.path
        
        if path == '/api/register':
            self.handle_register(data)
        elif path == '/api/login':
            self.handle_login(data)
        elif path == '/api/passwords':
            self.handle_add_password(data)
        else:
            self._set_headers(404)
            self.wfile.write(json.dumps({'success': False, 'message': 'Not found'}).encode())
    
    def do_PUT(self):
        content_length = int(self.headers.get('Content-Length', 0))
        body = self.rfile.read(content_length).decode() if content_length > 0 else '{}'
        
        try:
            data = json.loads(body)
        except:
            self._set_headers(400)
            self.wfile.write(json.dumps({'success': False, 'message': 'Invalid JSON'}).encode())
            return
        
        parsed_path = urlparse(self.path)
        path = parsed_path.path
        
        if path.startswith('/api/passwords/'):
            password_id = path.split('/')[-1]
            self.handle_update_password(password_id, data)
        else:
            self._set_headers(404)
            self.wfile.write(json.dumps({'success': False, 'message': 'Not found'}).encode())
    
    def do_DELETE(self):
        parsed_path = urlparse(self.path)
        path = parsed_path.path
        
        if path.startswith('/api/passwords/'):
            password_id = path.split('/')[-1]
            self.handle_delete_password(password_id)
        else:
            self._set_headers(404)
            self.wfile.write(json.dumps({'success': False, 'message': 'Not found'}).encode())
    
    def verify_auth(self):
        auth_token = self.headers.get('Authorization', '')
        username = self.headers.get('X-Username', '')
        
        if not auth_token or not username:
            return None
        
        if username in sessions and sessions[username] == auth_token:
            return username
        return None
    
    def handle_register(self, data):
        username = data.get('username', '')
        password = data.get('password', '')
        
        if not username or not password:
            self._set_headers(400)
            self.wfile.write(json.dumps({
                'success': False,
                'message': 'Username and password required'
            }).encode())
            return
        
        if username in users:
            self._set_headers(400)
            self.wfile.write(json.dumps({
                'success': False,
                'message': 'Username already exists'
            }).encode())
            return
        
        # Hash password
        password_hash = hashlib.sha256(password.encode()).hexdigest()
        users[username] = password_hash
        passwords[username] = []
        
        # Create session
        session_token = secrets.token_hex(32)
        sessions[username] = session_token
        
        self._set_headers(200)
        self.wfile.write(json.dumps({
            'success': True,
            'sessionToken': session_token,
            'username': username
        }).encode())
    
    def handle_login(self, data):
        username = data.get('username', '')
        password = data.get('password', '')
        
        print(f"Login attempt - Username: {username}, Password length: {len(password)}")
        print(f"Users in system: {len(users)}")
        print(f"Username exists: {username in users}")
        
        if not username or not password:
            self._set_headers(400)
            self.wfile.write(json.dumps({
                'success': False,
                'message': 'Username and password required'
            }).encode())
            return
        
        if username not in users:
            print(f"Username {username} not found in users")
            self._set_headers(401)
            self.wfile.write(json.dumps({
                'success': False,
                'message': 'Invalid credentials'
            }).encode())
            return
        
        password_hash = hashlib.sha256(password.encode()).hexdigest()
        stored_hash = users[username]
        print(f"Password hash match: {stored_hash == password_hash}")
        if users[username] != password_hash:
            self._set_headers(401)
            self.wfile.write(json.dumps({
                'success': False,
                'message': 'Invalid credentials'
            }).encode())
            return
        
        # Create new session
        session_token = secrets.token_hex(32)
        sessions[username] = session_token
        
        self._set_headers(200)
        self.wfile.write(json.dumps({
            'success': True,
            'sessionToken': session_token,
            'username': username
        }).encode())
    
    def handle_get_passwords(self):
        username = self.verify_auth()
        if not username:
            self._set_headers(401)
            self.wfile.write(json.dumps({
                'success': False,
                'message': 'Unauthorized'
            }).encode())
            return
        
        user_passwords = passwords.get(username, [])
        self._set_headers(200)
        self.wfile.write(json.dumps({
            'success': True,
            'passwords': user_passwords
        }).encode())
    
    def handle_add_password(self, data):
        username = self.verify_auth()
        if not username:
            self._set_headers(401)
            self.wfile.write(json.dumps({
                'success': False,
                'message': 'Unauthorized'
            }).encode())
            return
        
        site = data.get('site', '')
        pwd_username = data.get('username', '')
        pwd_password = data.get('password', '')
        category = data.get('category', '')
        notes = data.get('notes', '')
        
        if not site or not pwd_password:
            self._set_headers(400)
            self.wfile.write(json.dumps({
                'success': False,
                'message': 'Site and password required'
            }).encode())
            return
        
        # Create new password entry
        password_id = secrets.token_hex(8)
        new_password = {
            'id': password_id,
            'site': site,
            'username': pwd_username,
            'password': pwd_password,
            'category': category,
            'notes': notes,
            'created_at': int(time.time())
        }
        
        if username not in passwords:
            passwords[username] = []
        
        passwords[username].append(new_password)
        
        self._set_headers(200)
        self.wfile.write(json.dumps({
            'success': True,
            'message': 'Password added successfully',
            'password': new_password
        }).encode())
    
    def handle_update_password(self, password_id, data):
        username = self.verify_auth()
        if not username:
            self._set_headers(401)
            self.wfile.write(json.dumps({
                'success': False,
                'message': 'Unauthorized'
            }).encode())
            return
        
        user_passwords = passwords.get(username, [])
        password_found = False
        
        for pwd in user_passwords:
            if pwd['id'] == password_id:
                pwd['site'] = data.get('site', pwd['site'])
                pwd['username'] = data.get('username', pwd['username'])
                pwd['password'] = data.get('password', pwd['password'])
                pwd['category'] = data.get('category', pwd['category'])
                pwd['notes'] = data.get('notes', pwd['notes'])
                password_found = True
                break
        
        if not password_found:
            self._set_headers(404)
            self.wfile.write(json.dumps({
                'success': False,
                'message': 'Password not found'
            }).encode())
            return
        
        self._set_headers(200)
        self.wfile.write(json.dumps({
            'success': True,
            'message': 'Password updated successfully'
        }).encode())
    
    def handle_delete_password(self, password_id):
        username = self.verify_auth()
        if not username:
            self._set_headers(401)
            self.wfile.write(json.dumps({
                'success': False,
                'message': 'Unauthorized'
            }).encode())
            return
        
        user_passwords = passwords.get(username, [])
        initial_length = len(user_passwords)
        passwords[username] = [pwd for pwd in user_passwords if pwd['id'] != password_id]
        
        if len(passwords[username]) == initial_length:
            self._set_headers(404)
            self.wfile.write(json.dumps({
                'success': False,
                'message': 'Password not found'
            }).encode())
            return
        
        self._set_headers(200)
        self.wfile.write(json.dumps({
            'success': True,
            'message': 'Password deleted successfully'
        }).encode())
    
    def log_message(self, format, *args):
        # Custom logging
        print(f"[{self.log_date_time_string()}] {format % args}")

def run_server(port=8080):
    server_address = ('', port)
    httpd = HTTPServer(server_address, PasswordVaultHandler)
    print(f'Mock API Server running on http://localhost:{port}')
    print('Press Ctrl+C to stop')
    print('\nAvailable endpoints:')
    print('  POST   /api/register')
    print('  POST   /api/login')
    print('  GET    /api/passwords')
    print('  POST   /api/passwords')
    print('  PUT    /api/passwords/{id}')
    print('  DELETE /api/passwords/{id}')
    print()
    httpd.serve_forever()

if __name__ == '__main__':
    run_server()
