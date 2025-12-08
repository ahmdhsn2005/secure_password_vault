#include "storage.hpp"
#include "crypto.hpp"
#include <stdexcept>
#include <ctime>

using namespace std;

// basic setup

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

// Add new vault entry with encryption
uint64_t StorageManager::addVaultEntry(uint64_t user_id, const string& site_name, 
                                      const string& username, const string& password,
                                      const string& notes, const string& category) {
    // Get user's encryption key
    User* user = auth_manager.getUserById(user_id);
    if(!user) {
        throw runtime_error("User not found");
    }
    
    // Generate IV and encrypt password
    string iv = Crypto::generateIV();
    string encrypted_password = Crypto::encryptAES256(password, user->encryption_key, iv);
    
    // Create vault record
    VaultRecord record;
    record.user_id = user_id;
    record.site_name = site_name;
    record.username = username;
    record.encrypted_password = encrypted_password;
    record.iv = iv;
    record.notes = notes;
    record.category = category;
    
    // Insert into B-Tree (it will set the record_id)
    btree.insert(record);
    
    // Get the record_id that was assigned (it's the last one)
    vector<VaultRecord> user_records = btree.getAllRecordsForUser(user_id);
    if(!user_records.empty()) {
        // Return the most recently added record's ID
        uint64_t max_id = 0;
        for(const auto& rec : user_records) {
            if(rec.record_id > max_id) {
                max_id = rec.record_id;
            }
        }
        return max_id;
    }
    
    return 0;
}

// Get all vault entries for user
vector<VaultRecord> StorageManager::getUserVault(uint64_t user_id) {
    // Get user's encryption key
    User* user = auth_manager.getUserById(user_id);
    if(!user) {
        throw runtime_error("User not found");
    }
    
    // Get all records for this user
    vector<VaultRecord> records = btree.getAllRecordsForUser(user_id);
    
    // Decrypt passwords
    for(auto& record : records) {
        try {
            record.encrypted_password = Crypto::decryptAES256(
                record.encrypted_password, 
                user->encryption_key, 
                record.iv
            );
        } catch(const exception& e) {
            record.encrypted_password = "[Decryption failed]";
        }
    }
    
    return records;
}

// Search vault entries by site name
vector<VaultRecord> StorageManager::searchVaultEntry(uint64_t user_id, const string& site_name) {
    // Get user's encryption key
    User* user = auth_manager.getUserById(user_id);
    if(!user) {
        throw runtime_error("User not found");
    }
    
    // Search by site name
    vector<VaultRecord> records = btree.search(site_name);
    
    // Filter by user_id and decrypt
    vector<VaultRecord> user_records;
    for(auto& record : records) {
        if(record.user_id == user_id) {
            try {
                record.encrypted_password = Crypto::decryptAES256(
                    record.encrypted_password, 
                    user->encryption_key, 
                    record.iv
                );
            } catch(const exception& e) {
                record.encrypted_password = "[Decryption failed]";
            }
            user_records.push_back(record);
        }
    }
    
    return user_records;
}

// Update vault entry
bool StorageManager::updateVaultEntry(uint64_t user_id, uint64_t record_id,
                                     const string& site_name, const string& username,
                                     const string& password, const string& notes, const string& category) {
    // Get user's encryption key
    User* user = auth_manager.getUserById(user_id);
    if(!user) {
        throw runtime_error("User not found");
    }
    
    // Verify ownership
    vector<VaultRecord> all_records = btree.getAllRecordsForUser(user_id);
    bool owns_record = false;
    for(const auto& rec : all_records) {
        if(rec.record_id == record_id) {
            owns_record = true;
            break;
        }
    }
    
    if(!owns_record) {
        throw runtime_error("Unauthorized: Record does not belong to this user");
    }
    
    // Generate new IV and encrypt password
    string iv = Crypto::generateIV();
    string encrypted_password = Crypto::encryptAES256(password, user->encryption_key, iv);
    
    // Create updated record
    VaultRecord updated_record;
    updated_record.record_id = record_id;
    updated_record.user_id = user_id;
    updated_record.site_name = site_name;
    updated_record.username = username;
    updated_record.encrypted_password = encrypted_password;
    updated_record.iv = iv;
    updated_record.notes = notes;
    updated_record.category = category;
    
    return btree.update(record_id, updated_record);
}

// Delete vault entry
bool StorageManager::deleteVaultEntry(uint64_t user_id, uint64_t record_id) {
    // Verify ownership
    vector<VaultRecord> all_records = btree.getAllRecordsForUser(user_id);
    bool owns_record = false;
    for(const auto& rec : all_records) {
        if(rec.record_id == record_id) {
            owns_record = true;
            break;
        }
    }
    
    if(!owns_record) {
        throw runtime_error("Unauthorized: Record does not belong to this user");
    }
    
    return btree.remove(record_id);
}
