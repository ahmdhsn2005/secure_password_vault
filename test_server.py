#!/usr/bin/env python3
"""
Mock API Server for Local Testing
This is a simplified Python server that mimics the C++ API endpoints
for testing the web UI locally before AWS deployment.
"""

from http.server import HTTPServer, BaseHTTPRequestHandler
import json
import hashlib
import secrets
from urllib.parse import urlparse, parse_qs
import os

# In-memory storage for testing
users = {}  # username -> {"password_hash": str, "passwords": []}
sessions = {}  # session_token -> username

class VaultAPIHandler(BaseHTTPRequestHandler):
    
    def _send_cors_headers(self):
        self.send_header('Access-Control-Allow-Origin', '*')
        self.send_header('Access-Control-Allow-Methods', 'GET, POST, PUT, DELETE, OPTIONS')
        self.send_header('Access-Control-Allow-Headers', 'Content-Type, Authorization, X-Username')
    
    def _send_json_response(self, status, data):
        self.send_response(status)
        self.send_header('Content-Type', 'application/json')
        self._send_cors_headers()
        self.end_headers()
        self.wfile.write(json.dumps(data).encode())
    
    def _get_request_body(self):
        content_length = int(self.headers.get('Content-Length', 0))
        if content_length > 0:
            return json.loads(self.rfile.read(content_length))
        return {}
    
    def _get_session_user(self):
        auth_token = self.headers.get('Authorization')
        username = self.headers.get('X-Username')
        if auth_token in sessions and sessions[auth_token] == username:
            return username
        return None
    
    def do_OPTIONS(self):
        self.send_response(200)
        self._send_cors_headers()
        self.end_headers()
    
    def do_GET(self):
        parsed_path = urlparse(self.path)
        
        # Health check
        if parsed_path.path == '/api/health':
            self._send_json_response(200, {"status": "ok"})
            return
        
        # Get all passwords
        if parsed_path.path == '/api/passwords':
            username = self._get_session_user()
            if not username:
                self._send_json_response(401, {"success": False, "message": "Unauthorized"})
                return
            
            passwords = users.get(username, {}).get('passwords', [])
            self._send_json_response(200, {"success": True, "passwords": passwords})
            return
        
        self._send_json_response(404, {"success": False, "message": "Not found"})
    
    def do_POST(self):
        parsed_path = urlparse(self.path)
        body = self._get_request_body()
        
        # Register
        if parsed_path.path == '/api/register':
            username = body.get('username')
            password = body.get('password')
            
            if not username or not password:
                self._send_json_response(400, {"success": False, "message": "Username and password required"})
                return
            
            if username in users:
                self._send_json_response(400, {"success": False, "message": "Username already exists"})
                return
            
            # Hash password (simplified)
            password_hash = hashlib.sha256(password.encode()).hexdigest()
            users[username] = {"password_hash": password_hash, "passwords": []}
            
            self._send_json_response(200, {"success": True, "message": "Registration successful"})
            return
        
        # Login
        if parsed_path.path == '/api/login':
            username = body.get('username')
            password = body.get('password')
            
            if not username or not password:
                self._send_json_response(400, {"success": False, "message": "Username and password required"})
                return
            
            user = users.get(username)
            password_hash = hashlib.sha256(password.encode()).hexdigest()
            
            if not user or user['password_hash'] != password_hash:
                self._send_json_response(401, {"success": False, "message": "Invalid credentials"})
                return
            
            # Create session
            session_token = secrets.token_hex(32)
            sessions[session_token] = username
            
            self._send_json_response(200, {"success": True, "sessionToken": session_token})
            return
        
        # Add password
        if parsed_path.path == '/api/passwords':
            username = self._get_session_user()
            if not username:
                self._send_json_response(401, {"success": False, "message": "Unauthorized"})
                return
            
            site = body.get('site')
            pwd_username = body.get('username')
            password = body.get('password')
            category = body.get('category', '')
            notes = body.get('notes', '')
            
            if not site or not pwd_username or not password:
                self._send_json_response(400, {"success": False, "message": "Site, username, and password required"})
                return
            
            # Generate unique ID
            password_id = secrets.token_hex(8)
            
            password_entry = {
                "id": password_id,
                "site": site,
                "username": pwd_username,
                "password": password,
                "category": category,
                "notes": notes
            }
            
            users[username]['passwords'].append(password_entry)
            
            self._send_json_response(200, {"success": True, "message": "Password added", "id": password_id})
            return
        
        self._send_json_response(404, {"success": False, "message": "Not found"})
    
    def do_PUT(self):
        parsed_path = urlparse(self.path)
        
        # Update password
        if parsed_path.path.startswith('/api/passwords/'):
            username = self._get_session_user()
            if not username:
                self._send_json_response(401, {"success": False, "message": "Unauthorized"})
                return
            
            password_id = parsed_path.path.split('/')[-1]
            body = self._get_request_body()
            
            passwords = users[username]['passwords']
            for pwd in passwords:
                if pwd['id'] == password_id:
                    pwd['site'] = body.get('site', pwd['site'])
                    pwd['username'] = body.get('username', pwd['username'])
                    pwd['password'] = body.get('password', pwd['password'])
                    pwd['category'] = body.get('category', pwd['category'])
                    pwd['notes'] = body.get('notes', pwd['notes'])
                    
                    self._send_json_response(200, {"success": True, "message": "Password updated"})
                    return
            
            self._send_json_response(404, {"success": False, "message": "Password not found"})
            return
        
        self._send_json_response(404, {"success": False, "message": "Not found"})
    
    def do_DELETE(self):
        parsed_path = urlparse(self.path)
        
        # Delete password
        if parsed_path.path.startswith('/api/passwords/'):
            username = self._get_session_user()
            if not username:
                self._send_json_response(401, {"success": False, "message": "Unauthorized"})
                return
            
            password_id = parsed_path.path.split('/')[-1]
            
            passwords = users[username]['passwords']
            for i, pwd in enumerate(passwords):
                if pwd['id'] == password_id:
                    passwords.pop(i)
                    self._send_json_response(200, {"success": True, "message": "Password deleted"})
                    return
            
            self._send_json_response(404, {"success": False, "message": "Password not found"})
            return
        
        self._send_json_response(404, {"success": False, "message": "Not found"})
    
    def log_message(self, format, *args):
        # Log requests in a clean format
        print(f"[{self.command}] {self.path} - {args[1]}")

def run_server(port=8080):
    server_address = ('', port)
    httpd = HTTPServer(server_address, VaultAPIHandler)
    print(f"\n{'='*60}")
    print(f"  Mock Password Vault API Server")
    print(f"{'='*60}")
    print(f"  Server running on http://localhost:{port}")
    print(f"  API endpoints available at http://localhost:{port}/api/")
    print(f"\n  Available endpoints:")
    print(f"    POST   /api/register")
    print(f"    POST   /api/login")
    print(f"    GET    /api/passwords")
    print(f"    POST   /api/passwords")
    print(f"    PUT    /api/passwords/:id")
    print(f"    DELETE /api/passwords/:id")
    print(f"\n  Press Ctrl+C to stop the server")
    print(f"{'='*60}\n")
    
    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        print("\n\nShutting down server...")
        httpd.shutdown()

if __name__ == '__main__':
    run_server()
