# Implementation Roadmap
## Password Vault Backend - Path to Completion

---

## 📊 Current Status: 30% Complete

### ✅ **What's Done**
- Project structure and folders
- All header files with complete function signatures
- Custom data structures defined (HashMap, BTreeNode)
- Basic utility functions (hex conversion)
- Build system configured (CMake)
- Clear TODOs for all remaining work

### ⏳ **What's Remaining: 70%**

---

## 🛣️ Implementation Path (6 Phases)

### **Phase 1: Crypto Module** 📅 Days 1-4 (15%)

**File:** `src/crypto.cpp`

**Tasks:**
1. ✅ `bytesToHex()` - DONE
2. ✅ `hexToBytes()` - DONE
3. ⏳ `generateRandomBytes()` - Use `RAND_bytes()` from OpenSSL
4. ⏳ `generateSalt()` - Call generateRandomBytes(32)
5. ⏳ `generateIV()` - Call generateRandomBytes(16)
6. ⏳ `deriveKey()` - Implement PBKDF2-HMAC-SHA256
7. ⏳ `hashPassword()` - Wrapper for deriveKey()
8. ⏳ `verifyPassword()` - Hash input and compare
9. ⏳ `encryptAES256()` - EVP API for encryption
10. ⏳ `decryptAES256()` - EVP API for decryption

**Resources:**
- OpenSSL documentation: https://www.openssl.org/docs/
- Example: `EVP_EncryptInit_ex`, `PKCS5_PBKDF2_HMAC`

**Testing:**
```cpp
// Test case ideas:
// 1. Generate salt and verify it's 32 bytes (64 hex chars)
// 2. Hash password twice with same salt = same hash
// 3. Encrypt then decrypt = original plaintext
// 4. Different IVs = different ciphertexts
```

---

### **Phase 2: Hash Table** 📅 Days 5-7 (10%)

**File:** `include/auth.hpp` (template implementation)

**Tasks:**
1. ⏳ `hashFunction()` - Implement DJB2 algorithm
2. ⏳ `put()` - Insert with collision handling
3. ⏳ `get()` - Traverse chain to find key
4. ⏳ `contains()` - Check if key exists
5. ⏳ `remove()` - Delete node from chain
6. ⏳ `getAllValues()` - Iterate all buckets
7. ⏳ Destructor - Free all allocated nodes

**Pseudocode for DJB2:**
```cpp
hash = 5381
for each char c in key:
    hash = ((hash << 5) + hash) + c  // hash * 33 + c
return hash % TABLE_SIZE
```

**Testing:**
```cpp
// Test case ideas:
// 1. Insert 100 items, verify all retrievable
// 2. Test collision handling (same hash bucket)
// 3. Remove items and verify they're gone
// 4. Test with different key types (string, uint64_t)
```

---

### **Phase 3: Auth Module** 📅 Days 8-11 (15%)

**File:** `src/auth.cpp`

**Tasks:**
1. ⏳ `loadUsers()` - Read binary file into hash tables
2. ⏳ `saveUsers()` - Write hash tables to binary file
3. ⏳ `registerUser()` - Create user with hashed password
4. ⏳ `login()` - Verify password and create session
5. ⏳ `logout()` - Remove session from hash table
6. ⏳ `validateSession()` - Check token and expiration
7. ⏳ `getUserByEmail()` - Lookup in users_by_email
8. ⏳ `getUserById()` - Lookup in users_by_id

**Binary File Format for `users.dat`:**
```
[uint64_t: number of users]
For each user:
  [uint64_t: user_id]
  [uint64_t: email length]
  [char[]: email]
  [uint64_t: password_hash length]
  [char[]: password_hash]
  [uint64_t: salt length]
  [char[]: salt]
  [uint64_t: recovery_phrase length]
  [char[]: recovery_phrase]
  [uint64_t: encryption_key length]
  [char[]: encryption_key]
  [uint64_t: created_at]
```

**Testing:**
```cpp
// Test case ideas:
// 1. Register user, save to file, reload, verify data
// 2. Login with correct password = success
// 3. Login with wrong password = failure
// 4. Session expires after 24 hours
// 5. Logout invalidates session
```

---

### **Phase 4: B-Tree Module** 📅 Days 12-18 (20%)

**File:** `src/btree.cpp`

**Tasks:**
1. ⏳ `readNode()` - Deserialize node from file
2. ⏳ `writeNode()` - Serialize node to file
3. ⏳ `readRecord()` - Deserialize record from file
4. ⏳ `writeRecord()` - Serialize record to file
5. ⏳ `searchNode()` - Binary search within node
6. ⏳ `insertNonFull()` - Insert into non-full node
7. ⏳ `splitChild()` - Split full child node
8. ⏳ `insert()` - Main insertion logic
9. ⏳ `search()` - Find records by site_name
10. ⏳ `getAllRecordsForUser()` - Traverse all leaf nodes
11. ⏳ `update()` - Modify existing record
12. ⏳ `remove()` - Delete record (optional: lazy deletion)

**File Layout for `vault.dat`:**
```
[Header: 512 bytes]
  - uint64_t: root_id
  - uint64_t: next_node_id
  - uint64_t: next_record_id
  - Reserved space

[Node Section: starting at offset 512]
  - Each node: exactly 4KB
  - Node at offset: 512 + (node_id * 4096)

[Record Section: starting at offset 512 + (max_nodes * 4KB)]
  - Variable-size records
  - Track record positions in separate index
```

**Node Serialization (4KB fixed):**
```cpp
// Write to buffer:
// [bool: is_leaf]
// [int: num_keys]
// [uint64_t: node_id]
// For i = 0 to 40:
//   [uint64_t: key length]
//   [char[]: key data]
// For i = 0 to 41:
//   [uint64_t: children[i]]
// For i = 0 to 40:
//   [uint64_t: record_ids[i]]
// [padding to 4096 bytes]
```

**Testing:**
```cpp
// Test case ideas:
// 1. Insert 1000 records, verify all searchable
// 2. Insert with same site_name (multiple entries)
// 3. Search returns all matching entries
// 4. Update record and verify changes persist
// 5. Test node splitting (insert 41+ records)
```

---

### **Phase 5: Storage Integration** 📅 Days 19-20 (5%)

**File:** `src/storage.cpp`

**Tasks:**
1. ⏳ `addVaultEntry()` - Get encryption key, encrypt, insert to B-Tree
2. ⏳ `getUserVault()` - Get all records, decrypt passwords
3. ⏳ `searchVaultEntry()` - Search B-Tree, filter by user, decrypt
4. ⏳ `updateVaultEntry()` - Verify ownership, re-encrypt, update B-Tree
5. ⏳ `deleteVaultEntry()` - Verify ownership, delete from B-Tree

**Implementation Pattern:**
```cpp
uint64_t StorageManager::addVaultEntry(...) {
    // 1. Get user from auth_manager
    User* user = auth_manager.getUserById(user_id);
    if (!user) throw runtime_error("Invalid user");
    
    // 2. Generate IV and encrypt password
    string iv = Crypto::generateIV();
    string encrypted = Crypto::encryptAES256(password, user->encryption_key, iv);
    
    // 3. Create record
    VaultRecord record;
    record.user_id = user_id;
    record.site_name = site_name;
    record.encrypted_password = encrypted;
    record.iv = iv;
    // ... set other fields
    
    // 4. Insert to B-Tree
    btree.insert(record);
    return record.record_id;
}
```

**Testing:**
```cpp
// Test case ideas:
// 1. Add entry, retrieve, verify password decrypts correctly
// 2. User A can't access User B's passwords
// 3. Update changes password and requires new IV
// 4. Delete removes entry permanently
```

---

### **Phase 6: REST API Server** 📅 Days 21-23 (5%)

**File:** `src/main.cpp`

**Tasks:**
1. ⏳ Initialize `StorageManager`
2. ⏳ Create `httplib::Server` instance
3. ⏳ Implement `extractBearerToken()` helper
4. ⏳ POST `/signup` - Call storage.registerUser()
5. ⏳ POST `/login` - Call storage.loginUser()
6. ⏳ POST `/logout` - Call storage.logoutUser()
7. ⏳ GET `/vault` - Validate session, get vault
8. ⏳ POST `/vault` - Validate session, add entry
9. ⏳ PUT `/vault/:id` - Validate session, update entry
10. ⏳ DELETE `/vault/:id` - Validate session, delete entry
11. ⏳ GET `/health` - Return `{"status": "ok"}`
12. ⏳ Add CORS headers
13. ⏳ Start server on port 8080

**Endpoint Template:**
```cpp
server.Post("/vault", [&](const httplib::Request& req, httplib::Response& res) {
    // 1. Extract Bearer token
    string token = extractBearerToken(req);
    if (token.empty()) {
        res.status = 401;
        res.set_content("{\"error\": \"Unauthorized\"}", "application/json");
        return;
    }
    
    // 2. Validate session
    uint64_t user_id = storage.validateSession(token);
    if (user_id == 0) {
        res.status = 401;
        res.set_content("{\"error\": \"Invalid session\"}", "application/json");
        return;
    }
    
    // 3. Parse JSON body
    json body = json::parse(req.body);
    
    // 4. Call storage operation
    uint64_t record_id = storage.addVaultEntry(
        user_id,
        body["site_name"],
        body["username"],
        body["password"],
        body["notes"],
        body["category"]
    );
    
    // 5. Return response
    json response = {{"record_id", record_id}};
    res.set_content(response.dump(), "application/json");
});
```

**Testing:**
```cpp
// Use PowerShell or Postman:
// 1. POST /signup -> get user_id
// 2. POST /login -> get token
// 3. POST /vault with Bearer token -> add password
// 4. GET /vault with Bearer token -> verify password exists
// 5. PUT /vault/:id -> update password
// 6. DELETE /vault/:id -> remove password
// 7. POST /logout -> invalidate token
// 8. Try accessing /vault with old token -> 401 error
```

---

## 📅 Timeline Summary

| Phase | Days | Completion |
|-------|------|------------|
| Crypto Module | 1-4 | 15% → 45% |
| Hash Table | 5-7 | 10% → 55% |
| Auth Module | 8-11 | 15% → 70% |
| B-Tree | 12-18 | 20% → 90% |
| Storage Integration | 19-20 | 5% → 95% |
| REST API | 21-23 | 5% → 100% |

**Total Estimated Time:** 23 days (3-4 weeks)

---

## 🧪 Testing Strategy

### **Unit Testing**
Test each module independently:
- Crypto: Encrypt/decrypt round-trip
- Hash Table: Insert, search, delete
- Auth: Register, login, sessions
- B-Tree: Insert, search, traversal
- Storage: End-to-end encryption flow
- API: Each endpoint with various inputs

### **Integration Testing**
Test workflows:
1. Register → Login → Add password → Logout
2. Login → Get vault → Update password → Delete
3. Multiple users with isolated vaults
4. Session expiration handling

### **Manual Testing**
Use Postman or PowerShell scripts:
- Test all 8 endpoints
- Test error cases (invalid token, wrong password)
- Test edge cases (empty vault, duplicate entries)

---

## 🎯 Next Steps

1. **Start with Phase 1 (Crypto)** - It's the foundation for everything
2. **Implement one function at a time** - Test as you go
3. **Commit after each working function** - Use git for version control
4. **Don't skip testing** - Bugs compound quickly
5. **Ask for help when stuck** - Don't waste time on blockers

---

## 📚 Resources

**OpenSSL Documentation:**
- https://www.openssl.org/docs/man3.0/man3/
- EVP Encryption: https://wiki.openssl.org/index.php/EVP_Symmetric_Encryption_and_Decryption
- PBKDF2: https://www.openssl.org/docs/man3.0/man3/PKCS5_PBKDF2_HMAC.html

**C++ References:**
- File I/O: https://cplusplus.com/reference/fstream/
- Templates: https://cplusplus.com/doc/oldtutorial/templates/

**HTTP Server:**
- cpp-httplib: https://github.com/yhirose/cpp-httplib

---

## ✅ Success Criteria

**Mid-Term (Current):**
- ✅ 30% complete with solid foundation

**Final:**
- ✅ All modules implemented and tested
- ✅ Server runs locally without errors
- ✅ All 8 API endpoints functional
- ✅ Passwords encrypted and decrypted correctly
- ✅ Multiple users can register and maintain separate vaults
- ✅ Sessions expire properly
- ✅ Deployed to AWS EC2
- ✅ Live demo successful

---

**You have a clear path to completion! Focus on one phase at a time. 💪**
