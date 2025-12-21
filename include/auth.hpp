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
    
    // DJB2 hash function - converts key to integer hash
    int hashFunction(const K& key) const {
        return hashHelper(key);
    }
    
    // Helper function for string keys
    int hashHelper(const string& key) const {
        unsigned long hash = 5381;
        for (char c : key) {
            hash = ((hash << 5) + hash) + c;  // hash * 33 + c
        }
        return hash % TABLE_SIZE;
    }
    
    // Helper function for uint64_t keys
    int hashHelper(uint64_t key) const {
        return key % TABLE_SIZE;
    }
    
public:
    HashMap() {
        for(int i = 0; i < TABLE_SIZE; i++) {
            table[i] = nullptr;
        }
    }
    
    ~HashMap() {
        // Free all linked lists in each bucket
        for(int i = 0; i < TABLE_SIZE; i++) {
            HashNode* current = table[i];
            while(current != nullptr) {
                HashNode* temp = current;
                current = current->next;
                delete temp;
            }
        }
    }
    
    // Insert or update key-value pair
    void put(const K& key, const V& value) {
        int index = hashFunction(key);
        HashNode* current = table[index];
        
        // Check if key already exists - update it
        while(current != nullptr) {
            if(current->key == key) {
                current->value = value;
                return;
            }
            current = current->next;
        }
        
        // Key doesn't exist - insert at head of chain
        HashNode* newNode = new HashNode(key, value);
        newNode->next = table[index];
        table[index] = newNode;
    }
    
    // Get value by key (returns nullptr if not found)
    V* get(const K& key) {
        int index = hashFunction(key);
        HashNode* current = table[index];
        
        while(current != nullptr) {
            if(current->key == key) {
                return &(current->value);
            }
            current = current->next;
        }
        return nullptr;
    }
    
    // Check if key exists
    bool contains(const K& key) const {
        int index = hashFunction(key);
        HashNode* current = table[index];
        
        while(current != nullptr) {
            if(current->key == key) {
                return true;
            }
            current = current->next;
        }
        return false;
    }
    
    // Remove key-value pair
    bool remove(const K& key) {
        int index = hashFunction(key);
        HashNode* current = table[index];
        HashNode* prev = nullptr;
        
        while(current != nullptr) {
            if(current->key == key) {
                if(prev == nullptr) {
                    // Removing head of chain
                    table[index] = current->next;
                } else {
                    // Removing middle/end of chain
                    prev->next = current->next;
                }
                delete current;
                return true;
            }
            prev = current;
            current = current->next;
        }
        return false;
    }
    
    // Get all values from the hash table
    vector<V> getAllValues() const {
        vector<V> values;
        for(int i = 0; i < TABLE_SIZE; i++) {
            HashNode* current = table[i];
            while(current != nullptr) {
                values.push_back(current->value);
                current = current->next;
            }
        }
        return values;
    }
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
class AuthManager {
private:
    HashMap<string, User> users_by_email;   // lookup by email
    HashMap<uint64_t, User> users_by_id;    // lookup by ID
    HashMap<string, Session> sessions;       // lookup by token
    uint64_t next_user_id;
    string users_file;
    
    // File I/O functions
    void loadUsers();
    void saveUsers();
    
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
