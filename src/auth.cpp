#include "auth.hpp"
#include "crypto.hpp"
#include <fstream>
#include <ctime>
#include <sstream>
#include <random>

using namespace std;

// Helper function to generate session token
static string generateToken() {
    static const char charset[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    static random_device rd;
    static mt19937 gen(rd());
    static uniform_int_distribution<> dis(0, sizeof(charset) - 2);
    
    string token;
    for(int i = 0; i < 32; i++) {
        token += charset[dis(gen)];
    }
    return token;
}

// Constructor - load users from file if exists
AuthManager::AuthManager(const string& users_file) 
    : users_file(users_file), next_user_id(1) {
    loadUsers();
}

// Load users from binary file
void AuthManager::loadUsers() {
    ifstream file(users_file, ios::binary);
    if(!file.is_open()) return;  // File doesn't exist yet
    
    uint64_t user_count;
    file.read(reinterpret_cast<char*>(&user_count), sizeof(user_count));
    file.read(reinterpret_cast<char*>(&next_user_id), sizeof(next_user_id));
    
    for(uint64_t i = 0; i < user_count; i++) {
        User user;
        
        // Read user_id
        file.read(reinterpret_cast<char*>(&user.user_id), sizeof(user.user_id));
        
        // Read email
        size_t email_len;
        file.read(reinterpret_cast<char*>(&email_len), sizeof(email_len));
        user.email.resize(email_len);
        file.read(&user.email[0], email_len);
        
        // Read password_hash
        size_t hash_len;
        file.read(reinterpret_cast<char*>(&hash_len), sizeof(hash_len));
        user.password_hash.resize(hash_len);
        file.read(&user.password_hash[0], hash_len);
        
        // Read salt
        size_t salt_len;
        file.read(reinterpret_cast<char*>(&salt_len), sizeof(salt_len));
        user.salt.resize(salt_len);
        file.read(&user.salt[0], salt_len);
        
        // Read recovery_phrase
        size_t recovery_len;
        file.read(reinterpret_cast<char*>(&recovery_len), sizeof(recovery_len));
        user.recovery_phrase.resize(recovery_len);
        file.read(&user.recovery_phrase[0], recovery_len);
        
        // Read encryption_key
        size_t key_len;
        file.read(reinterpret_cast<char*>(&key_len), sizeof(key_len));
        user.encryption_key.resize(key_len);
        file.read(&user.encryption_key[0], key_len);
        
        // Read created_at
        file.read(reinterpret_cast<char*>(&user.created_at), sizeof(user.created_at));
        
        // Store in hash tables
        users_by_email.put(user.email, user);
        users_by_id.put(user.user_id, user);
    }
    
    file.close();
}

// Save users to binary file
void AuthManager::saveUsers() {
    ofstream file(users_file, ios::binary | ios::trunc);
    if(!file.is_open()) {
        throw runtime_error("Failed to open users file for writing");
    }
    
    vector<User> all_users = users_by_id.getAllValues();
    uint64_t user_count = all_users.size();
    
    file.write(reinterpret_cast<const char*>(&user_count), sizeof(user_count));
    file.write(reinterpret_cast<const char*>(&next_user_id), sizeof(next_user_id));
    
    for(const User& user : all_users) {
        // Write user_id
        file.write(reinterpret_cast<const char*>(&user.user_id), sizeof(user.user_id));
        
        // Write email
        size_t email_len = user.email.length();
        file.write(reinterpret_cast<const char*>(&email_len), sizeof(email_len));
        file.write(user.email.c_str(), email_len);
        
        // Write password_hash
        size_t hash_len = user.password_hash.length();
        file.write(reinterpret_cast<const char*>(&hash_len), sizeof(hash_len));
        file.write(user.password_hash.c_str(), hash_len);
        
        // Write salt
        size_t salt_len = user.salt.length();
        file.write(reinterpret_cast<const char*>(&salt_len), sizeof(salt_len));
        file.write(user.salt.c_str(), salt_len);
        
        // Write recovery_phrase
        size_t recovery_len = user.recovery_phrase.length();
        file.write(reinterpret_cast<const char*>(&recovery_len), sizeof(recovery_len));
        file.write(user.recovery_phrase.c_str(), recovery_len);
        
        // Write encryption_key
        size_t key_len = user.encryption_key.length();
        file.write(reinterpret_cast<const char*>(&key_len), sizeof(key_len));
        file.write(user.encryption_key.c_str(), key_len);
        
        // Write created_at
        file.write(reinterpret_cast<const char*>(&user.created_at), sizeof(user.created_at));
    }
    
    file.close();
}

// Register new user
uint64_t AuthManager::registerUser(const string& email, const string& password, const string& recovery_phrase) {
    // Check if user already exists
    if(users_by_email.contains(email)) {
        throw runtime_error("User with this email already exists");
    }
    
    User user;
    user.user_id = next_user_id++;
    user.email = email;
    user.salt = Crypto::generateSalt();
    user.password_hash = Crypto::hashPassword(password, user.salt);
    user.recovery_phrase = recovery_phrase;
    user.encryption_key = Crypto::deriveKey(password, user.salt);  // User's vault encryption key
    user.created_at = time(nullptr);
    
    // Store in both hash tables
    users_by_email.put(user.email, user);
    users_by_id.put(user.user_id, user);
    
    // Persist to disk
    saveUsers();
    
    return user.user_id;
}

// Login user
string AuthManager::login(const string& email, const string& password) {
    User* user = users_by_email.get(email);
    if(!user) {
        throw runtime_error("Invalid email or password");
    }
    
    // Verify password
    if(!Crypto::verifyPassword(password, user->salt, user->password_hash)) {
        throw runtime_error("Invalid email or password");
    }
    
    // Create session
    Session session;
    session.token = generateToken();
    session.user_id = user->user_id;
    session.created_at = time(nullptr);
    session.expires_at = session.created_at + (24 * 3600);  // 24 hours
    
    sessions.put(session.token, session);
    
    return session.token;
}

// Logout user
bool AuthManager::logout(const string& token) {
    return sessions.remove(token);
}

// Validate session and return user_id
uint64_t AuthManager::validateSession(const string& token) {
    Session* session = sessions.get(token);
    if(!session) {
        return 0;  // Invalid token
    }
    
    uint64_t current_time = time(nullptr);
    if(current_time > session->expires_at) {
        sessions.remove(token);  // Expired, remove it
        return 0;
    }
    
    return session->user_id;
}

// Get user by email
User* AuthManager::getUserByEmail(const string& email) {
    return users_by_email.get(email);
}

// Get user by ID
User* AuthManager::getUserById(uint64_t user_id) {
    return users_by_id.get(user_id);
}
