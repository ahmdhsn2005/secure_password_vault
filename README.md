# Password Vault Backend

A secure password management system built in C++17 with REST API, custom data structures, and military-grade encryption.

---

## 🔑 Features

### Security
- **AES-256-CBC** encryption for password storage
- **PBKDF2-HMAC-SHA256** password hashing with 100,000 iterations
- Unique salt per user (32 bytes)
- Unique IV per password entry (16 bytes)
- Session-based authentication with Bearer tokens
- 24-hour session expiration

### Data Structures
- **Custom Hash Table** - Built from scratch with separate chaining for collision resolution
- **Disk-based B-Tree** - Efficient password indexing with 4KB nodes

### REST API
- `POST /signup` - Register new user
- `POST /login` - Authenticate and get session token
- `POST /logout` - Invalidate session
- `GET /vault` - Retrieve all passwords
- `POST /vault` - Store new password
- `PUT /vault/:id` - Update existing password
- `DELETE /vault/:id` - Delete password
- `GET /health` - Server health check

---

## 🛠️ Technology Stack

| Component | Technology |
|-----------|-----------|
| Language | C++17 |
| Build System | CMake 3.15+ |
| HTTP Server | cpp-httplib (header-only) |
| JSON Parser | nlohmann-json (header-only) |
| Cryptography | OpenSSL |
| Data Storage | Binary files (custom format) |

---

## 📁 Project Structure

```
password-vault/
├── include/              # Header files
│   ├── crypto.hpp       # Cryptography functions
│   ├── btree.hpp        # B-Tree data structure
│   ├── auth.hpp         # Authentication & HashMap
│   └── storage.hpp      # Storage manager
│
├── src/                 # Implementation files
│   ├── main.cpp         # HTTP server & endpoints
│   ├── crypto.cpp       # Crypto implementation
│   ├── btree.cpp        # B-Tree operations
│   ├── auth.cpp         # User & session management
│   └── storage.cpp      # Storage coordination
│
├── data/                # Runtime data files
│   ├── users.dat        # User database (binary)
│   └── vault.dat        # Password vault (binary)
│
├── third_party/         # External libraries
├── build/               # Build output
└── CMakeLists.txt       # Build configuration
```

---

## 🏗️ Architecture

### System Design
```
┌─────────────────────────────────┐
│    REST API (HTTP Server)       │
│  8 endpoints with Bearer auth   │
└────────────┬────────────────────┘
             ↓
┌─────────────────────────────────┐
│    Storage Manager              │
│  Coordinates all operations     │
└─────┬──────────────────┬────────┘
      ↓                  ↓
┌──────────┐      ┌──────────────┐
│   Auth   │      │   B-Tree     │
│ Manager  │      │   Index      │
│          │      │              │
│ 3x Hash  │      │ Disk-based   │
│ Tables   │      │ 4KB nodes    │
└────┬─────┘      └──────┬───────┘
     ↓                   ↓
┌──────────┐      ┌──────────────┐
│users.dat │      │  vault.dat   │
└──────────┘      └──────────────┘
```

### Custom Hash Table
- **Implementation:** Separate chaining with linked lists
- **Size:** 1009 buckets (prime number)
- **Hash Function:** DJB2 algorithm
- **Complexity:** O(1) average case for insert/search/delete
- **Usage:** User lookup (by email, by ID), session management

### B-Tree Structure
- **Order:** 20 (max 40 keys per node)
- **Node Size:** 4KB (disk-optimized)
- **Storage:** Binary file format
- **Complexity:** O(log n) for insert/search/delete
- **Usage:** Password indexing by site name

---

## 🔐 Security Implementation

### Password Storage Flow
```
User's Master Password
    ↓
[Generate Random Salt (32 bytes)]
    ↓
[PBKDF2-HMAC-SHA256, 100k iterations]
    ↓
[Store Hash + Salt in users.dat]
```

### Vault Entry Encryption Flow
```
Plain Password
    ↓
[Get User's Encryption Key]
    ↓
[Generate Random IV (16 bytes)]
    ↓
[AES-256-CBC Encryption]
    ↓
[Store Ciphertext + IV in vault.dat]
```

### Session Management
- Generate 256-bit random token on login
- Store in hash table with user_id and expiration
- Validate on each API request
- Auto-expire after 24 hours

---

## 🚀 Getting Started

### Prerequisites
- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- CMake 3.15 or higher
- OpenSSL library
- cpp-httplib (header-only, included)
- nlohmann-json (header-only, included)

### Installation

#### Windows
```powershell
# Install OpenSSL
# Download from: https://slproweb.com/products/Win64OpenSSL.html

# Download dependencies
Invoke-WebRequest -Uri "https://github.com/yhirose/cpp-httplib/raw/master/httplib.h" -OutFile "third_party/httplib.h"
Invoke-WebRequest -Uri "https://github.com/nlohmann/json/releases/download/v3.11.2/json.hpp" -OutFile "third_party/json.hpp"

# Build
mkdir build
cd build
cmake ..
cmake --build . --config Release

# Run
.\Release\PasswordVault.exe
```

#### Linux
```bash
# Install dependencies
sudo apt-get install libssl-dev cmake g++

# Download libraries
wget -O third_party/httplib.h https://github.com/yhirose/cpp-httplib/raw/master/httplib.h
wget -O third_party/json.hpp https://github.com/nlohmann/json/releases/download/v3.11.2/json.hpp

# Build
mkdir build && cd build
cmake ..
make

# Run
./PasswordVault
```

---

## 📡 API Usage

### Register User
```bash
curl -X POST http://localhost:8080/signup \
  -H "Content-Type: application/json" \
  -d '{"email":"user@example.com","password":"secure123","recovery_phrase":"backup phrase"}'
```

### Login
```bash
curl -X POST http://localhost:8080/login \
  -H "Content-Type: application/json" \
  -d '{"email":"user@example.com","password":"secure123"}'
# Returns: {"token":"..."}
```

### Add Password
```bash
curl -X POST http://localhost:8080/vault \
  -H "Authorization: Bearer YOUR_TOKEN" \
  -H "Content-Type: application/json" \
  -d '{"site_name":"github.com","username":"myuser","password":"secret123","notes":"work account","category":"work"}'
```

### Get All Passwords
```bash
curl -X GET http://localhost:8080/vault \
  -H "Authorization: Bearer YOUR_TOKEN"
```

### Update Password
```bash
curl -X PUT http://localhost:8080/vault/1 \
  -H "Authorization: Bearer YOUR_TOKEN" \
  -H "Content-Type: application/json" \
  -d '{"site_name":"github.com","username":"myuser","password":"newsecret456","notes":"updated","category":"work"}'
```

### Delete Password
```bash
curl -X DELETE http://localhost:8080/vault/1 \
  -H "Authorization: Bearer YOUR_TOKEN"
```

---

## 🧪 Testing

Run the included test suite:
```powershell
# Windows
.\test-api.ps1

# Linux
./test-api.sh
```

---

## 📊 Performance

| Operation | Hash Table | B-Tree |
|-----------|------------|--------|
| Insert | O(1) avg | O(log n) |
| Search | O(1) avg | O(log n) |
| Delete | O(1) avg | O(log n) |

**Estimated Capacity:**
- Users: ~1 million (200 MB memory)
- Passwords: Unlimited (disk-limited)
- Concurrent Requests: ~100 (thread pool)

---

## 🔒 Security Notes

- All passwords encrypted with AES-256-CBC
- Master passwords hashed with PBKDF2 (100k iterations)
- Sessions expire after 24 hours
- No plaintext passwords stored anywhere
- Each password entry uses unique IV

**For Production Use:**
- Add HTTPS support
- Implement rate limiting
- Add comprehensive logging
- Set up proper backup strategy
- Use environment variables for config

---

## 📚 Documentation

- **Architecture Details:** See `docs/ARCHITECTURE.md`
- **AWS Deployment:** See `docs/AWS_DEPLOYMENT.md`
- **Implementation Guide:** See `IMPLEMENTATION_ROADMAP.md`

---

## 🤝 Contributing

This is an educational project. Contributions welcome!

---

## 📄 License

Educational project - for learning purposes.
