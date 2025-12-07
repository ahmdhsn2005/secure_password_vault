# README - Password Vault Backend (30% Complete)

## 🎯 Project Status

This is a **semester final project** currently at **30% completion** for mid-term presentation.

**What's Done:**
- ✅ Complete architecture design
- ✅ All header files with function signatures
- ✅ Custom data structures defined
- ✅ Basic skeleton implementations
- ✅ Build system configured

**What's Remaining:**
- ⏳ Implement crypto functions (OpenSSL)
- ⏳ Implement custom hash table
- ⏳ Implement auth module
- ⏳ Implement B-Tree operations
- ⏳ Complete storage integration
- ⏳ Build REST API server

---

## 📁 Project Structure

```
password-vault/
├── include/              # Header files (COMPLETE)
│   ├── crypto.hpp       # Crypto function declarations
│   ├── btree.hpp        # B-Tree data structure
│   ├── auth.hpp         # Auth + custom HashMap
│   └── storage.hpp      # Storage manager
│
├── src/                 # Source files (SKELETON ONLY)
│   ├── main.cpp         # Server skeleton
│   ├── crypto.cpp       # 10% implemented
│   ├── btree.cpp        # Constructor only
│   ├── auth.cpp         # Constructor only
│   └── storage.cpp      # Basic delegation
│
├── data/                # Binary data files (created at runtime)
├── build/               # Build output
├── docs/                # Documentation
├── third_party/         # External libraries
├── CMakeLists.txt       # Build configuration
└── .gitignore
```

---

## 🔑 Key Features (Planned)

### Security
- **AES-256-CBC** encryption for passwords
- **PBKDF2-HMAC-SHA256** for password hashing (100k iterations)
- Unique salt per user (32 bytes)
- Unique IV per password entry (16 bytes)
- Session-based authentication with 24-hour expiration

### Data Structures
- **Custom Hash Table** - Built from scratch with separate chaining
- **Disk-based B-Tree** - For efficient password indexing

### API (Planned)
- `POST /signup` - Register new user
- `POST /login` - Get session token
- `POST /logout` - Invalidate session
- `GET /vault` - Get all passwords
- `POST /vault` - Add new password
- `PUT /vault/:id` - Update password
- `DELETE /vault/:id` - Delete password
- `GET /health` - Health check

---

## 🛠️ Technology Stack

| Component | Technology |
|-----------|-----------|
| Language | C++17 |
| Build System | CMake 3.15+ |
| HTTP Server | cpp-httplib (header-only) |
| JSON Parser | nlohmann-json (header-only) |
| Cryptography | OpenSSL |
| Data Storage | Binary files |

---

## 📚 Documentation

- **`MID_TERM_PRESENTATION.md`** - Complete presentation guide for mid-term
- **`IMPLEMENTATION_ROADMAP.md`** - Step-by-step implementation plan (70% remaining)
- **`CMakeLists.txt`** - Build configuration

---

## 🚀 Quick Start (For Review)

### View the Architecture
```bash
# Open header files to see the design
cat include/crypto.hpp     # Crypto functions
cat include/auth.hpp       # Custom HashMap template
cat include/btree.hpp      # B-Tree structure
cat include/storage.hpp    # Storage manager
```

### Build System
```bash
# Install dependencies first (manual):
# 1. OpenSSL - https://slproweb.com/products/Win64OpenSSL.html
# 2. Download cpp-httplib to third_party/
# 3. Download json.hpp to third_party/

# Configure and build (won't run yet - implementations incomplete)
mkdir build
cd build
cmake ..
cmake --build .
```

---

## 📖 What to Study for Mid-Term

1. **Architecture** (`MID_TERM_PRESENTATION.md`)
   - System design and module relationships
   - Why each component was chosen

2. **Custom Hash Table** (`include/auth.hpp`)
   - Separate chaining collision resolution
   - DJB2 hash function
   - O(1) average case complexity

3. **B-Tree Structure** (`include/btree.hpp`)
   - Fixed 4KB nodes for disk efficiency
   - Order 20 (max 40 keys per node)
   - O(log n) search complexity

4. **Security Design** (`include/crypto.hpp`)
   - AES-256-CBC for data encryption
   - PBKDF2 for password hashing
   - Salt and IV generation

5. **Implementation Plan** (`IMPLEMENTATION_ROADMAP.md`)
   - 6 phases to completion
   - Estimated 23 days (3-4 weeks)

---

## 🎯 Next Steps (After Mid-Term)

Follow `IMPLEMENTATION_ROADMAP.md` for detailed step-by-step instructions:

1. **Phase 1:** Implement crypto module (Days 1-4)
2. **Phase 2:** Implement hash table (Days 5-7)
3. **Phase 3:** Implement auth module (Days 8-11)
4. **Phase 4:** Implement B-Tree (Days 12-18)
5. **Phase 5:** Complete storage integration (Days 19-20)
6. **Phase 6:** Build REST API (Days 21-23)

---

## 📊 Completion Breakdown

| Component | Status | Percentage |
|-----------|--------|------------|
| Project Structure | ✅ Complete | 5% |
| Header Files | ✅ Complete | 20% |
| Build System | ✅ Complete | 2% |
| Basic Utilities | ✅ Complete | 3% |
| **Crypto Module** | ⏳ TODO | 15% |
| **Hash Table** | ⏳ TODO | 10% |
| **Auth Module** | ⏳ TODO | 15% |
| **B-Tree** | ⏳ TODO | 20% |
| **Storage Integration** | ⏳ TODO | 5% |
| **REST API** | ⏳ TODO | 5% |
| **Total** | **30% Complete** | **30/100** |

---

## 🏆 Project Goals

### Mid-Term (Current)
- ✅ Design complete system architecture
- ✅ Define all data structures
- ✅ Create implementation plan
- ✅ Demonstrate understanding of concepts

### Final
- ⏳ Implement all modules
- ⏳ Test thoroughly
- ⏳ Deploy to AWS EC2
- ⏳ Live demo

---

## 📝 Notes

- All TODO comments in source files indicate what needs implementation
- Each function that's not implemented throws `runtime_error`
- Build system is configured but server won't run until implementations are complete
- Binary file formats are documented in `IMPLEMENTATION_ROADMAP.md`

---

## 👤 Author

Semester Final Project
Mid-Term Checkpoint: 30% Complete

---

## 📄 License

Educational project - not for production use.
