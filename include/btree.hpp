#ifndef BTREE_HPP
#define BTREE_HPP

#include <string>
#include <vector>
#include <cstdint>

using namespace std;

// One password entry
struct VaultRecord {
    uint64_t record_id;
    uint64_t user_id;
    string site_name;           // what we search by
    string username;
    string encrypted_password;  // encrypted with AES-256
    string iv;                  // for decryption
    string notes;
    string category;
    uint64_t created_at;
    uint64_t modified_at;
};

// B-Tree node (4KB on disk)
struct BTreeNode {
    bool is_leaf;
    int num_keys;
    string keys[40];            // up to 40 site names
    uint64_t children[41];      // up to 41 child nodes
    uint64_t record_ids[40];    // points to records
    uint64_t node_id;
    
    BTreeNode() : is_leaf(true), num_keys(0), node_id(0) {
        for(int i = 0; i < 41; i++) children[i] = 0;
        for(int i = 0; i < 40; i++) record_ids[i] = 0;
    }
};

// B-Tree for storing passwords on disk
class BTree {
private:
    string filename;
    uint64_t root_id;
    uint64_t next_node_id;
    uint64_t next_record_id;
    
    // Helper functions for disk I/O
    void saveMetadata();
    BTreeNode readNode(uint64_t node_id);
    void writeNode(const BTreeNode& node);
    void writeRecord(const VaultRecord& record);
    vector<VaultRecord> readAllRecords();
    
    // B-Tree operations
    void splitChild(BTreeNode& parent, int index);
    void insertNonFull(BTreeNode& node, const VaultRecord& record);
    
public:
    BTree(const string& filename);
    
    // Add new password
    bool insert(const VaultRecord& record);
    
    // Find passwords by site name
    vector<VaultRecord> search(const string& site_name);
    
    // Get all passwords for one user
    vector<VaultRecord> getAllRecordsForUser(uint64_t user_id);
    
    // Update a password
    bool update(uint64_t record_id, const VaultRecord& record);
    
    // Delete a password
    bool remove(uint64_t record_id);
};

#endif
