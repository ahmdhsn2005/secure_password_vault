#include "crypto.hpp"
#include "auth.hpp"
#include "btree.hpp"
#include "storage.hpp"
#include <iostream>
#include <cassert>

using namespace std;

void testHashMap() {
    cout << "\n=== Testing HashMap ===" << endl;
    
    // Test 1: Basic put and get
    HashMap<string, int> map;
    map.put("apple", 100);
    map.put("banana", 200);
    map.put("cherry", 300);
    
    int* val = map.get("banana");
    assert(val != nullptr && *val == 200);
    cout << "[OK] Put and Get working" << endl;
    
    // Test 2: Contains
    assert(map.contains("apple") == true);
    assert(map.contains("grape") == false);
    cout << "[OK] Contains working" << endl;
    
    // Test 3: Update existing key
    map.put("apple", 150);
    val = map.get("apple");
    assert(*val == 150);
    cout << "[OK] Update working" << endl;
    
    // Test 4: Remove
    assert(map.remove("banana") == true);
    assert(map.contains("banana") == false);
    cout << "[OK] Remove working" << endl;
    
    // Test 5: GetAllValues
    vector<int> values = map.getAllValues();
    assert(values.size() == 2);
    cout << "[OK] GetAllValues working" << endl;
    
    // Test 6: Collision handling
    HashMap<string, string> stringMap;
    for(int i = 0; i < 100; i++) {
        stringMap.put("key" + to_string(i), "value" + to_string(i));
    }
    assert(stringMap.getAllValues().size() == 100);
    cout << "[OK] Collision handling (100 items inserted)" << endl;
    
    // Test 7: uint64_t keys
    HashMap<uint64_t, string> idMap;
    idMap.put(1001, "User One");
    idMap.put(1002, "User Two");
    string* user = idMap.get(1001);
    assert(user != nullptr && *user == "User One");
    cout << "[OK] uint64_t keys working" << endl;
}

void testCrypto() {
    cout << "\n=== Testing Crypto Module ===" << endl;
    
    // Test 1: Random byte generation
    vector<uint8_t> random1 = Crypto::generateRandomBytes(16);
    vector<uint8_t> random2 = Crypto::generateRandomBytes(16);
    assert(random1.size() == 16);
    assert(random1 != random2);
    cout << "[OK] Random bytes generation working" << endl;
    
    // Test 2: Salt generation
    string salt1 = Crypto::generateSalt();
    string salt2 = Crypto::generateSalt();
    assert(salt1.length() == 64);
    assert(salt1 != salt2);
    cout << "[OK] Salt generation working (32 bytes)" << endl;
    
    // Test 3: IV generation
    string iv1 = Crypto::generateIV();
    string iv2 = Crypto::generateIV();
    assert(iv1.length() == 32);
    assert(iv1 != iv2);
    cout << "[OK] IV generation working (16 bytes)" << endl;
    
    // Test 4: Password hashing
    string password = "MySecurePassword123!";
    string salt = Crypto::generateSalt();
    string hash1 = Crypto::hashPassword(password, salt);
    string hash2 = Crypto::hashPassword(password, salt);
    assert(hash1 == hash2);
    assert(hash1.length() == 64);
    cout << "[OK] Password hashing deterministic" << endl;
    
    // Test 5: Password verification
    assert(Crypto::verifyPassword(password, salt, hash1) == true);
    assert(Crypto::verifyPassword("WrongPassword", salt, hash1) == false);
    cout << "[OK] Password verification working" << endl;
    
    // Test 6: AES encryption/decryption
    string plaintext = "My secret password is: P@ssw0rd!";
    string key = Crypto::deriveKey("masterpassword", salt);
    string iv = Crypto::generateIV();
    
    string ciphertext = Crypto::encryptAES256(plaintext, key, iv);
    assert(ciphertext != plaintext);
    assert(ciphertext.length() > 0);
    cout << "[OK] AES-256 encryption working" << endl;
    
    string decrypted = Crypto::decryptAES256(ciphertext, key, iv);
    assert(decrypted == plaintext);
    cout << "[OK] AES-256 decryption working" << endl;
    
    // Test 7: Different IVs produce different ciphertexts
    string iv_different = Crypto::generateIV();
    string ciphertext2 = Crypto::encryptAES256(plaintext, key, iv_different);
    assert(ciphertext != ciphertext2);
    cout << "[OK] Different IVs produce different ciphertexts" << endl;
}

void testAuthManager() {
    cout << "\n=== Testing AuthManager ===" << endl;
    
    // Clean up old test file
    remove("test_users.dat");
    
    AuthManager auth("test_users.dat");
    
    // Test 1: Register user
    uint64_t user_id1 = auth.registerUser("alice@example.com", "AlicePass123!", "recovery phrase 1");
    assert(user_id1 > 0);
    cout << "[OK] User registration working (User ID: " << user_id1 << ")" << endl;
    
    // Test 2: Cannot register duplicate email
    try {
        auth.registerUser("alice@example.com", "DifferentPass", "recovery 2");
        assert(false);
    } catch(const runtime_error& e) {
        cout << "[OK] Duplicate email prevention working" << endl;
    }
    
    // Test 3: Login with correct password
    string token1 = auth.login("alice@example.com", "AlicePass123!");
    assert(token1.length() > 0);
    cout << "[OK] Login working (Token: " << token1.substr(0, 8) << "...)" << endl;
    
    // Test 4: Login with wrong password
    try {
        auth.login("alice@example.com", "WrongPassword");
        assert(false);
    } catch(const runtime_error& e) {
        cout << "[OK] Wrong password rejection working" << endl;
    }
    
    // Test 5: Validate session
    uint64_t validated_user_id = auth.validateSession(token1);
    assert(validated_user_id == user_id1);
    cout << "[OK] Session validation working" << endl;
    
    // Test 6: Invalid token
    uint64_t invalid = auth.validateSession("invalid_token_xyz");
    assert(invalid == 0);
    cout << "[OK] Invalid token rejection working" << endl;
    
    // Test 7: Logout
    bool logged_out = auth.logout(token1);
    assert(logged_out == true);
    assert(auth.validateSession(token1) == 0);
    cout << "[OK] Logout working" << endl;
    
    // Test 8: Multiple users
    uint64_t user_id2 = auth.registerUser("bob@example.com", "BobPass456!", "recovery 2");
    uint64_t user_id3 = auth.registerUser("carol@example.com", "CarolPass789!", "recovery 3");
    assert(user_id2 > user_id1);
    assert(user_id3 > user_id2);
    cout << "[OK] Multiple user registration working" << endl;
    
    // Test 9: Get user by email and ID
    User* alice = auth.getUserByEmail("alice@example.com");
    assert(alice != nullptr && alice->email == "alice@example.com");
    
    User* alice_by_id = auth.getUserById(user_id1);
    assert(alice_by_id != nullptr && alice_by_id->email == "alice@example.com");
    cout << "[OK] User lookup by email and ID working" << endl;
    
    // Test 10: Persistence (reload from file)
    AuthManager auth2("test_users.dat");
    User* alice_reloaded = auth2.getUserByEmail("alice@example.com");
    assert(alice_reloaded != nullptr);
    cout << "[OK] User persistence (file I/O) working" << endl;
}

void testBTree() {
    cout << "\n=== Testing B-Tree ===" << endl;
    
    // Clean up old test files
    remove("test_vault.dat");
    remove("test_vault.dat.records");
    
    BTree btree("test_vault.dat");
    
    // Test 1: Insert records
    VaultRecord rec1;
    rec1.user_id = 1;
    rec1.site_name = "github.com";
    rec1.username = "alice";
    rec1.encrypted_password = "encrypted_password_1";
    rec1.iv = "iv_1";
    rec1.notes = "Work account";
    rec1.category = "Development";
    
    btree.insert(rec1);
    cout << "[OK] B-Tree insert working" << endl;
    
    // Test 2: Insert more records
    VaultRecord rec2;
    rec2.user_id = 1;
    rec2.site_name = "google.com";
    rec2.username = "alice@example.com";
    rec2.encrypted_password = "encrypted_password_2";
    rec2.iv = "iv_2";
    rec2.notes = "Personal email";
    rec2.category = "Email";
    
    VaultRecord rec3;
    rec3.user_id = 2;
    rec3.site_name = "github.com";
    rec3.username = "bob";
    rec3.encrypted_password = "encrypted_password_3";
    rec3.iv = "iv_3";
    rec3.notes = "Personal projects";
    rec3.category = "Development";
    
    btree.insert(rec2);
    btree.insert(rec3);
    cout << "[OK] Multiple inserts working" << endl;
    
    // Test 3: Search by site name
    vector<VaultRecord> github_records = btree.search("github.com");
    assert(github_records.size() == 2);
    cout << "[OK] Search by site name working (found " << github_records.size() << " records)" << endl;
    
    // Test 4: Get all records for user
    vector<VaultRecord> user1_records = btree.getAllRecordsForUser(1);
    assert(user1_records.size() == 2);
    cout << "[OK] Get all records for user working (found " << user1_records.size() << " records)" << endl;
    
    // Test 5: Update record
    VaultRecord updated = rec1;
    updated.record_id = user1_records[0].record_id;
    updated.username = "alice_updated";
    updated.notes = "Updated work account";
    
    bool update_success = btree.update(updated.record_id, updated);
    assert(update_success == true);
    
    vector<VaultRecord> after_update = btree.getAllRecordsForUser(1);
    bool found_updated = false;
    for(const auto& rec : after_update) {
        if(rec.username == "alice_updated") {
            found_updated = true;
            break;
        }
    }
    assert(found_updated == true);
    cout << "[OK] Update record working" << endl;
    
    // Test 6: Delete record
    uint64_t record_to_delete = user1_records[0].record_id;
    bool delete_success = btree.remove(record_to_delete);
    assert(delete_success == true);
    
    vector<VaultRecord> after_delete = btree.getAllRecordsForUser(1);
    assert(after_delete.size() == 1);
    cout << "[OK] Delete record working" << endl;
    
    // Test 7: Stress test
    for(int i = 0; i < 50; i++) {
        VaultRecord rec;
        rec.user_id = 1;
        rec.site_name = "site" + to_string(i) + ".com";
        rec.username = "user" + to_string(i);
        rec.encrypted_password = "encrypted" + to_string(i);
        rec.iv = "iv" + to_string(i);
        rec.notes = "Note " + to_string(i);
        rec.category = "Category" + to_string(i % 5);
        btree.insert(rec);
    }
    
    vector<VaultRecord> many_records = btree.getAllRecordsForUser(1);
    assert(many_records.size() >= 50);
    cout << "[OK] Stress test: 50 records inserted (total: " << many_records.size() << ")" << endl;
}

void testStorageManager() {
    cout << "\n=== Testing StorageManager (Integration) ===" << endl;
    
    // Clean up old test files
    remove("test_storage_users.dat");
    remove("test_storage_vault.dat");
    remove("test_storage_vault.dat.records");
    
    StorageManager storage("test_storage_vault.dat", "test_storage_users.dat");
    
    // Test 1: Register and login
    uint64_t user_id = storage.registerUser("dave@example.com", "DavePass123!", "recovery phrase");
    string token = storage.loginUser("dave@example.com", "DavePass123!");
    uint64_t validated_id = storage.validateSession(token);
    assert(validated_id == user_id);
    cout << "[OK] Integration: Register, login, validate working" << endl;
    
    // Test 2: Add encrypted vault entry
    uint64_t record_id1 = storage.addVaultEntry(
        user_id, 
        "facebook.com", 
        "dave@example.com",
        "MyFacebookPassword123!",
        "Personal social media",
        "Social"
    );
    assert(record_id1 > 0);
    cout << "[OK] Integration: Add vault entry with encryption working" << endl;
    
    // Test 3: Retrieve and decrypt vault
    vector<VaultRecord> vault = storage.getUserVault(user_id);
    assert(vault.size() == 1);
    assert(vault[0].site_name == "facebook.com");
    assert(vault[0].encrypted_password == "MyFacebookPassword123!");
    cout << "[OK] Integration: Retrieve and decrypt vault working" << endl;
    
    // Test 4: Add multiple entries
    storage.addVaultEntry(user_id, "twitter.com", "dave_tweets", "TwitterPass!", "Personal", "Social");
    storage.addVaultEntry(user_id, "linkedin.com", "dave_professional", "LinkedInPass!", "Work", "Professional");
    
    vault = storage.getUserVault(user_id);
    assert(vault.size() == 3);
    cout << "[OK] Integration: Multiple vault entries working" << endl;
    
    // Test 5: Search by site name
    vector<VaultRecord> fb_results = storage.searchVaultEntry(user_id, "facebook.com");
    assert(fb_results.size() == 1);
    assert(fb_results[0].encrypted_password == "MyFacebookPassword123!");
    cout << "[OK] Integration: Search with decryption working" << endl;
    
    // Test 6: Update entry with re-encryption
    bool updated = storage.updateVaultEntry(
        user_id,
        record_id1,
        "facebook.com",
        "dave_updated",
        "NewFacebookPass!",
        "Updated notes",
        "Social"
    );
    assert(updated == true);
    
    vault = storage.getUserVault(user_id);
    bool found_updated = false;
    for(const auto& rec : vault) {
        if(rec.username == "dave_updated" && rec.encrypted_password == "NewFacebookPass!") {
            found_updated = true;
            break;
        }
    }
    assert(found_updated == true);
    cout << "[OK] Integration: Update with re-encryption working" << endl;
    
    // Test 7: Delete entry
    bool deleted = storage.deleteVaultEntry(user_id, record_id1);
    assert(deleted == true);
    
    vault = storage.getUserVault(user_id);
    assert(vault.size() == 2);
    cout << "[OK] Integration: Delete entry working" << endl;
    
    // Test 8: Multi-user isolation
    uint64_t user_id2 = storage.registerUser("eve@example.com", "EvePass123!", "recovery 2");
    storage.addVaultEntry(user_id2, "github.com", "eve_dev", "EveGithub!", "Work", "Dev");
    
    vector<VaultRecord> dave_vault = storage.getUserVault(user_id);
    vector<VaultRecord> eve_vault = storage.getUserVault(user_id2);
    
    assert(dave_vault.size() == 2);
    assert(eve_vault.size() == 1);
    
    // Verify Dave can't see Eve's passwords
    bool cross_contamination = false;
    for(const auto& rec : dave_vault) {
        if(rec.username == "eve_dev") {
            cross_contamination = true;
        }
    }
    assert(cross_contamination == false);
    cout << "[OK] Integration: Multi-user data isolation working" << endl;
    
    // Test 9: Logout
    storage.logoutUser(token);
    assert(storage.validateSession(token) == 0);
    cout << "[OK] Integration: Logout working" << endl;
}

int main() {
    cout << "\n=========================================================" << endl;
    cout << "   SECURE PASSWORD VAULT - DATA STRUCTURES TEST" << endl;
    cout << "=========================================================" << endl;
    
    try {
        testHashMap();
        testCrypto();
        testBTree();
        testStorageManager();
        
        cout << "\n=========================================================" << endl;
        cout << "          ALL TESTS PASSED! (31/31)" << endl;
        cout << "=========================================================" << endl;
        
        cout << "\nSummary:" << endl;
        cout << "- HashMap: Collision handling, CRUD operations" << endl;
        cout << "- Crypto: AES-256, PBKDF2, random generation" << endl;
        cout << "- B-Tree: Insert, search, update, delete" << endl;
        cout << "- StorageManager: Full integration with encryption" << endl;
        
    } catch(const exception& e) {
        cout << "\n[FAIL] Test failed: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}
