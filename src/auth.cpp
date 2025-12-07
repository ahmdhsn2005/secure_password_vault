#include "auth.hpp"
#include "crypto.hpp"
#include <fstream>
#include <ctime>

using namespace std;

// TODO: HashMap stuff (important!)

template<typename K, typename V>
int HashMap<K, V>::hashFunction(const string& key) const {
    // DJB2 hash
    // TODO: hash = 5381, then (hash * 33 + c) for each char
    return 0;
}

// template methods go in header or explicit instantiation here

// basic constructor

AuthManager::AuthManager(const string& users_file) 
    : users_file(users_file), next_user_id(1) {
    // TODO: load users from file
}

// TODO: all the auth methods

uint64_t AuthManager::registerUser(const string& email, const string& password, const string& recovery_phrase) {
    // TODO: make salt, hash password, save user
    throw runtime_error("registerUser not implemented yet!");
}

string AuthManager::login(const string& email, const string& password) {
    // TODO: check password, make session token
    throw runtime_error("login not implemented yet!");
}

bool AuthManager::logout(const string& token) {
    // TODO: delete session
    throw runtime_error("logout not implemented yet!");
}

uint64_t AuthManager::validateSession(const string& token) {
    // TODO: check token exists and not expired
    throw runtime_error("validateSession not implemented yet!");
}

User* AuthManager::getUserByEmail(const string& email) {
    // TODO: lookup in hash table
    return nullptr;
}

User* AuthManager::getUserById(uint64_t user_id) {
    // TODO: lookup in hash table
    return nullptr;
}

// TODO: loadUsers and saveUsers for file I/O
