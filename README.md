# Secure Password Vault

A secure password management system with AES-256 encryption, custom data structures, and modern web interface.

## Features
- **Modern Web UI** - Clean, responsive interface
- **Custom HashMap** - Fast in-memory lookups with DJB2 hashing
- **B-Tree Storage** - Efficient disk-based persistence  
- **AES-256-CBC Encryption** - Military-grade password security
- **PBKDF2 Hashing** - 100,000 iterations with salt
- **Multi-user Support** - Each user has separate encrypted vault
- **REST API** - HTTP server for web/mobile access

## Quick Start

### Local Testing
```powershell
# Terminal 1: Start API server
python test_server.py

# Terminal 2: Start web UI
cd web
python -m http.server 3000

# Open browser: http://localhost:3000
```

### AWS Deployment
See [DEPLOYMENT.md](DEPLOYMENT.md) for complete AWS EC2 setup instructions.

## Project Structure
```
src/              # C++ source code
  ├── main.cpp    # CLI application
  ├── server.cpp  # REST API server
  ├── crypto.cpp  # AES-256 encryption
  ├── auth.cpp    # User authentication
  ├── btree.cpp   # B-Tree implementation
  └── storage.cpp # Storage manager

include/          # Header files
  ├── crypto.hpp
  ├── auth.hpp
  ├── btree.hpp
  └── storage.hpp

web/              # Web interface
  ├── index.html  # Main UI
  ├── styles.css  # Modern styling
  └── app.js      # Frontend logic

test_server.py    # Python mock server for testing
CMakeLists.txt    # Build configuration
DEPLOYMENT.md     # AWS deployment guide
PROJECT_REPORT.txt # Technical documentation
```

## Technology Stack

**Backend:**
- C++17 with STL
- OpenSSL for cryptography
- cpp-httplib for REST API
- nlohmann/json for JSON parsing

**Frontend:**
- Vanilla HTML5/CSS3/JavaScript
- Modern responsive design
- No frameworks needed

**Infrastructure:**
- AWS EC2 (Ubuntu)
- Nginx reverse proxy
- Let's Encrypt SSL

## Building C++ Server

```bash
mkdir build
cd build
cmake ..
make
./password_vault_server
```

## Data Structures

### Custom HashMap
- **DJB2 hash function** for string hashing
- **Separate chaining** for collision resolution
- **O(1) average** lookup time
- **Prime table size** (1009) for better distribution

### B-Tree
- **Disk-based** persistence
- **4KB nodes** for optimal I/O
- **Order 40** (up to 40 keys per node)
- **O(log n)** search/insert/delete

## Security Features

### Encryption
- **Algorithm:** AES-256-CBC
- **Key derivation:** PBKDF2-HMAC-SHA256
- **Iterations:** 100,000
- **Salt:** 32 bytes per user
- **IV:** 16 bytes per password entry

### Authentication
- Password hashing with PBKDF2
- Session-based tokens
- Secure password storage
- No plaintext passwords stored

## API Endpoints

```
POST   /api/register  - Register new user
POST   /api/login     - Authenticate and get session token
GET    /api/passwords - Get all passwords (requires auth)
POST   /api/passwords - Add new password (requires auth)
PUT    /api/passwords/:id - Update password (requires auth)
DELETE /api/passwords/:id - Delete password (requires auth)
GET    /api/health    - Health check
```

## Performance

- **Startup time:** ~2-3 seconds (loading B-Tree)
- **Login:** ~100-200ms (PBKDF2 iterations)
- **Password operations:** ~10-50ms (encryption + disk I/O)
- **Search:** ~5-20ms (B-Tree lookup)

## File Sizes
- Core C++ implementation: ~2000 lines
- Web interface: ~500 lines
- Total project: ~2500 lines

## Future Enhancements
- Password strength meter
- Password generator
- Two-factor authentication
- Password sharing (encrypted)
- Mobile app
- Export/import functionality
- Password history
- Breach detection

## License
MIT License

## Author
Built as a data structures and algorithms project demonstrating custom data structure implementation, cryptographic best practices, REST API design, modern web development, and AWS cloud deployment.
