#include "storage.hpp"
#include "crypto.hpp"
#include <ctime>

using namespace std;

// IMPLEMENTED: Constructor and delegation methods (10% of storage module)

StorageManager::StorageManager(const string& vault_file, const string& users_file)
    : btree(vault_file), auth_manager(users_file) {
}

uint64_t StorageManager::registerUser(const string& email, const string& password, const string& recovery_phrase) {
    return auth_manager.registerUser(email, password, recovery_phrase);
}

string StorageManager::loginUser(const string& email, const string& password) {
    return auth_manager.login(email, password);
}

bool StorageManager::logoutUser(const string& token) {
    return auth_manager.logout(token);
}

uint64_t StorageManager::validateSession(const string& token) {
    return auth_manager.validateSession(token);
}

// TODO: Implement vault operations

uint64_t StorageManager::addVaultEntry(uint64_t user_id, const string& site_name, 
                                      const string& username, const string& password,
                                      const string& notes, const string& category) {
    // TODO: Implement
    // Steps:
    // 1. Get user's encryption key from auth_manager
    // 2. Generate IV using Crypto::generateIV()
    // 3. Encrypt password using Crypto::encryptAES256()
    // 4. Create VaultRecord with encrypted data
    // 5. Insert into B-Tree
    // 6. Return record_id
    throw runtime_error("addVaultEntry not implemented yet!");
}

vector<VaultRecord> StorageManager::getUserVault(uint64_t user_id) {
    // TODO: Implement
    // Steps:
    // 1. Get all records for user from B-Tree
    // 2. Get user's encryption key
    // 3. Decrypt password in each record
    // 4. Return decrypted records
    throw runtime_error("getUserVault not implemented yet!");
}

vector<VaultRecord> StorageManager::searchVaultEntry(uint64_t user_id, const string& site_name) {
    // TODO: Implement
    // Steps:
    // 1. Search B-Tree by site_name
    // 2. Filter results by user_id
    // 3. Decrypt passwords
    // 4. Return matching records
    throw runtime_error("searchVaultEntry not implemented yet!");
}

bool StorageManager::updateVaultEntry(uint64_t user_id, uint64_t record_id,
                                     const string& site_name, const string& username,
                                     const string& password, const string& notes, const string& category) {
    // TODO: Implement
    // Steps:
    // 1. Verify user owns this record
    // 2. Get user's encryption key
    // 3. Generate new IV
    // 4. Encrypt new password
    // 5. Update record in B-Tree
    throw runtime_error("updateVaultEntry not implemented yet!");
}

bool StorageManager::deleteVaultEntry(uint64_t user_id, uint64_t record_id) {
    // TODO: Implement
    // Steps:
    // 1. Verify user owns this record
    // 2. Delete from B-Tree
    throw runtime_error("deleteVaultEntry not implemented yet!");
}
