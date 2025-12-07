#ifndef BTREE_HPP
#define BTREE_HPP

#include <string>
#include <vector>
#include <cstdint>

using namespace std;

// Vault Record - Represents a single password entry
struct VaultRecord {
    uint64_t record_id;
    uint64_t user_id;
    string site_name;           // The key we'll index by
    string username;
    string encrypted_password;  // AES-256 encrypted
    string iv;                  // Initialization vector for decryption
    string notes;
    string category;
    uint64_t created_at;
    uint64_t modified_at;
};

// B-Tree Node - Fixed size for disk storage
struct BTreeNode {
    bool is_leaf;
    int num_keys;
    string keys[40];            // Max 40 keys (site names)
    uint64_t children[41];      // Max 41 children
    uint64_t record_ids[40];    // Record IDs for leaf nodes
    uint64_t node_id;
    
    BTreeNode() : is_leaf(true), num_keys(0), node_id(0) {
        for(int i = 0; i < 41; i++) children[i] = 0;
        for(int i = 0; i < 40; i++) record_ids[i] = 0;
    }
};

// B-Tree - Disk-based indexing structure
// TODO: Implement insert, search, delete operations
class BTree {
private:
    string filename;
    uint64_t root_id;
    uint64_t next_node_id;
    uint64_t next_record_id;
    
    // TODO: Implement helper methods
    // - readNode(uint64_t node_id) -> BTreeNode
    // - writeNode(BTreeNode node)
    // - readRecord(uint64_t record_id) -> VaultRecord
    // - writeRecord(VaultRecord record)
    // - searchNode(BTreeNode node, string key) -> vector<VaultRecord>
    // - insertNonFull(BTreeNode node, VaultRecord record)
    // - splitChild(BTreeNode parent, int index)
    
public:
    BTree(const string& filename);
    
    // TODO: Insert a new vault entry
    bool insert(const VaultRecord& record);
    
    // TODO: Search for entries by site name
    vector<VaultRecord> search(const string& site_name);
    
    // TODO: Get all entries for a specific user
    vector<VaultRecord> getAllRecordsForUser(uint64_t user_id);
    
    // TODO: Update an existing entry
    bool update(uint64_t record_id, const VaultRecord& record);
    
    // TODO: Delete an entry
    bool remove(uint64_t record_id);
};

#endif
