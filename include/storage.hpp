#ifndef STORAGE_HPP
#define STORAGE_HPP

#include "btree.hpp"
#include "auth.hpp"
#include <string>
#include <vector>

using namespace std;

// Storage Manager - Facade that coordinates B-Tree and Auth modules
class StorageManager {
private:
    BTree btree;
    AuthManager auth_manager;
    
public:
    StorageManager(const string& vault_file, const string& users_file);
    
    // User operations
    uint64_t registerUser(const string& email, const string& password, const string& recovery_phrase);
    string loginUser(const string& email, const string& password);
    bool logoutUser(const string& token);
    uint64_t validateSession(const string& token);
    
    // Vault operations
    // TODO: Implement these methods to coordinate between auth and btree
    uint64_t addVaultEntry(uint64_t user_id, const string& site_name, 
                          const string& username, const string& password,
                          const string& notes, const string& category);
    
    vector<VaultRecord> getUserVault(uint64_t user_id);
    vector<VaultRecord> searchVaultEntry(uint64_t user_id, const string& site_name);
    bool updateVaultEntry(uint64_t user_id, uint64_t record_id, 
                         const string& site_name, const string& username,
                         const string& password, const string& notes, const string& category);
    bool deleteVaultEntry(uint64_t user_id, uint64_t record_id);
};

#endif
