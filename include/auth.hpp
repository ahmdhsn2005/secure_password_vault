#ifndef AUTH_HPP
#define AUTH_HPP

#include <string>
#include <vector>
#include <cstdint>

using namespace std;

// Hash table built from scratch (no STL!)
// TODO: separate chaining for collisions
template<typename K, typename V>
class HashMap {
private:
    static const int TABLE_SIZE = 1009;  // prime number
    
    // linked list node
    struct HashNode {
        K key;
        V value;
        HashNode* next;
        
        HashNode(const K& k, const V& v) : key(k), value(v), next(nullptr) {}
    };
    
    HashNode* table[TABLE_SIZE];
    
    // DJB2 hash
    // TODO: hash = ((hash << 5) + hash) + c
    int hashFunction(const string& key) const;
    
public:
    HashMap() {
        for(int i = 0; i < TABLE_SIZE; i++) {
            table[i] = nullptr;
        }
    }
    
    ~HashMap() {
        // TODO: free the linked lists
    }
    
    // TODO: add key-value
    void put(const K& key, const V& value);
    
    // TODO: get value (nullptr if not found)
    V* get(const K& key);
    
    // TODO: check if exists
    bool contains(const K& key) const;
    
    // TODO: delete key
    bool remove(const K& key);
    
    // TODO: get all values
    vector<V> getAllValues() const;
};

// User info
struct User {
    uint64_t user_id;
    string email;
    string password_hash;       // hashed with PBKDF2
    string salt;                // random per user
    string recovery_phrase;
    string encryption_key;      // for their vault
    uint64_t created_at;
};

// Login session
struct Session {
    string token;
    uint64_t user_id;
    uint64_t created_at;
    uint64_t expires_at;        // 24 hours
};

// Handles users and sessions
// TODO: registration, login, sessions
class AuthManager {
private:
    HashMap<string, User> users_by_email;   // lookup by email
    HashMap<uint64_t, User> users_by_id;    // lookup by ID
    HashMap<string, Session> sessions;       // lookup by token
    uint64_t next_user_id;
    string users_file;
    
    // TODO: file I/O
    // loadUsers, saveUsers
    
public:
    AuthManager(const string& users_file);
    
    // TODO: sign up new user
    // make salt, hash password, save
    uint64_t registerUser(const string& email, const string& password, const string& recovery_phrase);
    
    // TODO: log in
    // check password, make session token
    string login(const string& email, const string& password);
    
    // TODO: log out
    bool logout(const string& token);
    
    // TODO: check if token is valid
    // return user_id or 0
    uint64_t validateSession(const string& token);
    
    // helpers
    User* getUserByEmail(const string& email);
    User* getUserById(uint64_t user_id);
};

#endif
