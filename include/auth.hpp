#ifndef AUTH_HPP
#define AUTH_HPP

#include <string>
#include <vector>
#include <cstdint>

using namespace std;

// Custom Hash Table Implementation - Built from scratch!
// TODO: Implement hash table with separate chaining
template<typename K, typename V>
class HashMap {
private:
    static const int TABLE_SIZE = 1009;  // Prime number for better distribution
    
    // Node for separate chaining
    struct HashNode {
        K key;
        V value;
        HashNode* next;
        
        HashNode(const K& k, const V& v) : key(k), value(v), next(nullptr) {}
    };
    
    HashNode* table[TABLE_SIZE];
    
    // DJB2 hash function
    // TODO: Implement: hash = ((hash << 5) + hash) + c for each char
    int hashFunction(const string& key) const;
    
public:
    HashMap() {
        for(int i = 0; i < TABLE_SIZE; i++) {
            table[i] = nullptr;
        }
    }
    
    ~HashMap() {
        // TODO: Free all nodes
    }
    
    // TODO: Insert key-value pair
    void put(const K& key, const V& value);
    
    // TODO: Get value by key (return nullptr if not found)
    V* get(const K& key);
    
    // TODO: Check if key exists
    bool contains(const K& key) const;
    
    // TODO: Remove key-value pair
    bool remove(const K& key);
    
    // TODO: Get all values (for iteration)
    vector<V> getAllValues() const;
};

// User structure
struct User {
    uint64_t user_id;
    string email;
    string password_hash;       // PBKDF2 hash
    string salt;                // Random salt
    string recovery_phrase;
    string encryption_key;      // For encrypting vault entries
    uint64_t created_at;
};

// Session structure
struct Session {
    string token;
    uint64_t user_id;
    uint64_t created_at;
    uint64_t expires_at;        // 24 hours from creation
};

// Authentication Manager
// TODO: Implement user registration, login, and session management
class AuthManager {
private:
    HashMap<string, User> users_by_email;   // email -> User
    HashMap<uint64_t, User> users_by_id;    // user_id -> User
    HashMap<string, Session> sessions;       // token -> Session
    uint64_t next_user_id;
    string users_file;
    
    // TODO: Implement helper methods
    // - loadUsers() - Load from users.dat file
    // - saveUsers() - Save to users.dat file
    
public:
    AuthManager(const string& users_file);
    
    // TODO: Register a new user
    // - Generate salt
    // - Hash password with PBKDF2
    // - Generate encryption key
    // - Save to hash table and file
    uint64_t registerUser(const string& email, const string& password, const string& recovery_phrase);
    
    // TODO: Login user
    // - Find user by email
    // - Verify password
    // - Generate session token
    // - Store session with 24-hour expiration
    string login(const string& email, const string& password);
    
    // TODO: Logout user (invalidate session)
    bool logout(const string& token);
    
    // TODO: Validate session token
    // - Check if token exists
    // - Check if not expired
    // - Return user_id
    uint64_t validateSession(const string& token);
    
    // Helper methods
    User* getUserByEmail(const string& email);
    User* getUserById(uint64_t user_id);
};

#endif
