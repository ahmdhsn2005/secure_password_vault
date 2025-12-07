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

// TODO: vault operations

uint64_t StorageManager::addVaultEntry(uint64_t user_id, const string& site_name, 
                                      const string& username, const string& password,
                                      const string& notes, const string& category) {
    // TODO: get user's key, encrypt password, insert to btree
    throw runtime_error("addVaultEntry not implemented yet!");
}

vector<VaultRecord> StorageManager::getUserVault(uint64_t user_id) {
    // TODO: get from btree, decrypt passwords
    throw runtime_error("getUserVault not implemented yet!");
}

vector<VaultRecord> StorageManager::searchVaultEntry(uint64_t user_id, const string& site_name) {
    // TODO: search btree, filter by user, decrypt
    throw runtime_error("searchVaultEntry not implemented yet!");
}

bool StorageManager::updateVaultEntry(uint64_t user_id, uint64_t record_id,
                                     const string& site_name, const string& username,
                                     const string& password, const string& notes, const string& category) {
    // TODO: check ownership, re-encrypt with new IV, update btree
    throw runtime_error("updateVaultEntry not implemented yet!");
}

bool StorageManager::deleteVaultEntry(uint64_t user_id, uint64_t record_id) {
    // TODO: check ownership, delete from btree
    throw runtime_error("deleteVaultEntry not implemented yet!");
}
