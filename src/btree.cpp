#include "btree.hpp"
#include <fstream>
#include <cstring>
#include <ctime>
#include <algorithm>

using namespace std;

// Constructor - initialize or load from file
BTree::BTree(const string& filename) : filename(filename), root_id(0), next_node_id(1), next_record_id(1) {
    ifstream file(filename, ios::binary);
    if(file.is_open()) {
        file.read(reinterpret_cast<char*>(&root_id), sizeof(root_id));
        file.read(reinterpret_cast<char*>(&next_node_id), sizeof(next_node_id));
        file.read(reinterpret_cast<char*>(&next_record_id), sizeof(next_record_id));
        file.close();
    } else {
        // Create new tree with root node
        BTreeNode root;
        root.node_id = 0;
        root.is_leaf = true;
        root.num_keys = 0;
        writeNode(root);
        saveMetadata();
    }
}

// Save tree metadata
void BTree::saveMetadata() {
    ofstream file(filename, ios::binary | ios::in | ios::out);
    if(!file.is_open()) {
        file.open(filename, ios::binary | ios::trunc);
    }
    file.seekp(0);
    file.write(reinterpret_cast<const char*>(&root_id), sizeof(root_id));
    file.write(reinterpret_cast<const char*>(&next_node_id), sizeof(next_node_id));
    file.write(reinterpret_cast<const char*>(&next_record_id), sizeof(next_record_id));
    file.close();
}

// Read node from disk
BTreeNode BTree::readNode(uint64_t node_id) {
    ifstream file(filename, ios::binary);
    if(!file.is_open()) {
        throw runtime_error("Failed to open B-Tree file for reading");
    }
    
    // Skip metadata (3 * 8 bytes) and seek to node position
    uint64_t node_offset = 24 + (node_id * 4096);
    file.seekg(node_offset);
    
    BTreeNode node;
    node.node_id = node_id;
    
    file.read(reinterpret_cast<char*>(&node.is_leaf), sizeof(node.is_leaf));
    file.read(reinterpret_cast<char*>(&node.num_keys), sizeof(node.num_keys));
    
    // Read keys
    for(int i = 0; i < 40; i++) {
        size_t key_len;
        file.read(reinterpret_cast<char*>(&key_len), sizeof(key_len));
        if(key_len > 0) {
            node.keys[i].resize(key_len);
            file.read(&node.keys[i][0], key_len);
        }
    }
    
    // Read children and record_ids
    file.read(reinterpret_cast<char*>(node.children), sizeof(node.children));
    file.read(reinterpret_cast<char*>(node.record_ids), sizeof(node.record_ids));
    
    file.close();
    return node;
}

// Write node to disk
void BTree::writeNode(const BTreeNode& node) {
    fstream file(filename, ios::binary | ios::in | ios::out);
    if(!file.is_open()) {
        file.open(filename, ios::binary | ios::trunc);
    }
    
    // Seek to node position
    uint64_t node_offset = 24 + (node.node_id * 4096);
    file.seekp(node_offset);
    
    file.write(reinterpret_cast<const char*>(&node.is_leaf), sizeof(node.is_leaf));
    file.write(reinterpret_cast<const char*>(&node.num_keys), sizeof(node.num_keys));
    
    // Write keys
    for(int i = 0; i < 40; i++) {
        size_t key_len = node.keys[i].length();
        file.write(reinterpret_cast<const char*>(&key_len), sizeof(key_len));
        if(key_len > 0) {
            file.write(node.keys[i].c_str(), key_len);
        }
    }
    
    // Write children and record_ids
    file.write(reinterpret_cast<const char*>(node.children), sizeof(node.children));
    file.write(reinterpret_cast<const char*>(node.record_ids), sizeof(node.record_ids));
    
    file.close();
}

// Write record to disk
void BTree::writeRecord(const VaultRecord& record) {
    string records_file = filename + ".records";
    
    // Open in append mode or create new file
    ofstream file(records_file, ios::binary | ios::app);
    if(!file.is_open()) {
        throw runtime_error("Failed to open records file for writing");
    }
    
    file.write(reinterpret_cast<const char*>(&record.record_id), sizeof(record.record_id));
    file.write(reinterpret_cast<const char*>(&record.user_id), sizeof(record.user_id));
    
    // Write strings
    size_t len;
    len = record.site_name.length();
    file.write(reinterpret_cast<const char*>(&len), sizeof(len));
    file.write(record.site_name.c_str(), len);
    
    len = record.username.length();
    file.write(reinterpret_cast<const char*>(&len), sizeof(len));
    file.write(record.username.c_str(), len);
    
    len = record.encrypted_password.length();
    file.write(reinterpret_cast<const char*>(&len), sizeof(len));
    file.write(record.encrypted_password.c_str(), len);
    
    len = record.iv.length();
    file.write(reinterpret_cast<const char*>(&len), sizeof(len));
    file.write(record.iv.c_str(), len);
    
    len = record.notes.length();
    file.write(reinterpret_cast<const char*>(&len), sizeof(len));
    file.write(record.notes.c_str(), len);
    
    len = record.category.length();
    file.write(reinterpret_cast<const char*>(&len), sizeof(len));
    file.write(record.category.c_str(), len);
    
    file.write(reinterpret_cast<const char*>(&record.created_at), sizeof(record.created_at));
    file.write(reinterpret_cast<const char*>(&record.modified_at), sizeof(record.modified_at));
    
    file.flush();
    file.close();
}

// Read all records from disk
vector<VaultRecord> BTree::readAllRecords() {
    vector<VaultRecord> records;
    string records_file = filename + ".records";
    ifstream file(records_file, ios::binary);
    if(!file.is_open()) return records;
    
    while(file.peek() != EOF) {
        VaultRecord record;
        
        file.read(reinterpret_cast<char*>(&record.record_id), sizeof(record.record_id));
        file.read(reinterpret_cast<char*>(&record.user_id), sizeof(record.user_id));
        
        size_t len;
        file.read(reinterpret_cast<char*>(&len), sizeof(len));
        record.site_name.resize(len);
        file.read(&record.site_name[0], len);
        
        file.read(reinterpret_cast<char*>(&len), sizeof(len));
        record.username.resize(len);
        file.read(&record.username[0], len);
        
        file.read(reinterpret_cast<char*>(&len), sizeof(len));
        record.encrypted_password.resize(len);
        file.read(&record.encrypted_password[0], len);
        
        file.read(reinterpret_cast<char*>(&len), sizeof(len));
        record.iv.resize(len);
        file.read(&record.iv[0], len);
        
        file.read(reinterpret_cast<char*>(&len), sizeof(len));
        record.notes.resize(len);
        file.read(&record.notes[0], len);
        
        file.read(reinterpret_cast<char*>(&len), sizeof(len));
        record.category.resize(len);
        file.read(&record.category[0], len);
        
        file.read(reinterpret_cast<char*>(&record.created_at), sizeof(record.created_at));
        file.read(reinterpret_cast<char*>(&record.modified_at), sizeof(record.modified_at));
        
        records.push_back(record);
    }
    
    file.close();
    return records;
}

// Split a full child node
void BTree::splitChild(BTreeNode& parent, int index) {
    BTreeNode full_child = readNode(parent.children[index]);
    BTreeNode new_child;
    new_child.node_id = next_node_id++;
    new_child.is_leaf = full_child.is_leaf;
    new_child.num_keys = 19;  // Half of 40 (minus 1 for middle)
    
    // Copy second half to new child
    for(int i = 0; i < 19; i++) {
        new_child.keys[i] = full_child.keys[i + 20];
        new_child.record_ids[i] = full_child.record_ids[i + 20];
    }
    
    if(!full_child.is_leaf) {
        for(int i = 0; i < 20; i++) {
            new_child.children[i] = full_child.children[i + 20];
        }
    }
    
    full_child.num_keys = 20;
    
    // Move parent's keys and children to make space
    for(int i = parent.num_keys; i > index; i--) {
        parent.children[i + 1] = parent.children[i];
    }
    parent.children[index + 1] = new_child.node_id;
    
    for(int i = parent.num_keys - 1; i >= index; i--) {
        parent.keys[i + 1] = parent.keys[i];
        parent.record_ids[i + 1] = parent.record_ids[i];
    }
    
    parent.keys[index] = full_child.keys[19];
    parent.record_ids[index] = full_child.record_ids[19];
    parent.num_keys++;
    
    writeNode(full_child);
    writeNode(new_child);
    writeNode(parent);
    saveMetadata();
}

// Insert into non-full node
void BTree::insertNonFull(BTreeNode& node, const VaultRecord& record) {
    int i = node.num_keys - 1;
    
    if(node.is_leaf) {
        // Insert in sorted order
        while(i >= 0 && record.site_name < node.keys[i]) {
            node.keys[i + 1] = node.keys[i];
            node.record_ids[i + 1] = node.record_ids[i];
            i--;
        }
        node.keys[i + 1] = record.site_name;
        node.record_ids[i + 1] = record.record_id;
        node.num_keys++;
        writeNode(node);
    } else {
        // Find child to insert into
        while(i >= 0 && record.site_name < node.keys[i]) {
            i--;
        }
        i++;
        
        BTreeNode child = readNode(node.children[i]);
        if(child.num_keys == 40) {
            splitChild(node, i);
            if(record.site_name > node.keys[i]) {
                i++;
            }
            child = readNode(node.children[i]);
        }
        insertNonFull(child, record);
    }
}

// Insert new password
bool BTree::insert(const VaultRecord& record_input) {
    VaultRecord record = record_input;
    record.record_id = next_record_id++;
    record.created_at = time(nullptr);
    record.modified_at = record.created_at;
    
    writeRecord(record);
    
    BTreeNode root = readNode(root_id);
    
    if(root.num_keys == 40) {
        // Root is full, create new root
        BTreeNode new_root;
        new_root.node_id = next_node_id++;
        new_root.is_leaf = false;
        new_root.num_keys = 0;
        new_root.children[0] = root_id;
        
        writeNode(new_root);
        root_id = new_root.node_id;
        
        splitChild(new_root, 0);
        insertNonFull(new_root, record);
    } else {
        insertNonFull(root, record);
    }
    
    saveMetadata();
    return true;
}

// Search for passwords by site name
vector<VaultRecord> BTree::search(const string& site_name) {
    vector<VaultRecord> results;
    vector<VaultRecord> all_records = readAllRecords();
    
    for(const auto& record : all_records) {
        if(record.site_name == site_name) {
            results.push_back(record);
        }
    }
    
    return results;
}

// Get all passwords for a user
vector<VaultRecord> BTree::getAllRecordsForUser(uint64_t user_id) {
    vector<VaultRecord> results;
    vector<VaultRecord> all_records = readAllRecords();
    
    for(const auto& record : all_records) {
        if(record.user_id == user_id) {
            results.push_back(record);
        }
    }
    
    return results;
}

// Update a password
bool BTree::update(uint64_t record_id, const VaultRecord& updated_record) {
    vector<VaultRecord> all_records = readAllRecords();
    
    for(auto& record : all_records) {
        if(record.record_id == record_id) {
            record.site_name = updated_record.site_name;
            record.username = updated_record.username;
            record.encrypted_password = updated_record.encrypted_password;
            record.iv = updated_record.iv;
            record.notes = updated_record.notes;
            record.category = updated_record.category;
            record.modified_at = time(nullptr);
            
            // Rewrite all records
            string records_file = filename + ".records";
            ofstream file(records_file, ios::binary | ios::trunc);
            for(const auto& r : all_records) {
                file.write(reinterpret_cast<const char*>(&r.record_id), sizeof(r.record_id));
                file.write(reinterpret_cast<const char*>(&r.user_id), sizeof(r.user_id));
                
                size_t len;
                len = r.site_name.length();
                file.write(reinterpret_cast<const char*>(&len), sizeof(len));
                file.write(r.site_name.c_str(), len);
                
                len = r.username.length();
                file.write(reinterpret_cast<const char*>(&len), sizeof(len));
                file.write(r.username.c_str(), len);
                
                len = r.encrypted_password.length();
                file.write(reinterpret_cast<const char*>(&len), sizeof(len));
                file.write(r.encrypted_password.c_str(), len);
                
                len = r.iv.length();
                file.write(reinterpret_cast<const char*>(&len), sizeof(len));
                file.write(r.iv.c_str(), len);
                
                len = r.notes.length();
                file.write(reinterpret_cast<const char*>(&len), sizeof(len));
                file.write(r.notes.c_str(), len);
                
                len = r.category.length();
                file.write(reinterpret_cast<const char*>(&len), sizeof(len));
                file.write(r.category.c_str(), len);
                
                file.write(reinterpret_cast<const char*>(&r.created_at), sizeof(r.created_at));
                file.write(reinterpret_cast<const char*>(&r.modified_at), sizeof(r.modified_at));
            }
            file.close();
            return true;
        }
    }
    
    return false;
}

// Delete a password
bool BTree::remove(uint64_t record_id) {
    vector<VaultRecord> all_records = readAllRecords();
    
    auto it = remove_if(all_records.begin(), all_records.end(), 
        [record_id](const VaultRecord& r) { return r.record_id == record_id; });
    
    if(it == all_records.end()) {
        return false;  // Not found
    }
    
    all_records.erase(it, all_records.end());
    
    // Rewrite all records
    string records_file = filename + ".records";
    ofstream file(records_file, ios::binary | ios::trunc);
    for(const auto& r : all_records) {
        file.write(reinterpret_cast<const char*>(&r.record_id), sizeof(r.record_id));
        file.write(reinterpret_cast<const char*>(&r.user_id), sizeof(r.user_id));
        
        size_t len;
        len = r.site_name.length();
        file.write(reinterpret_cast<const char*>(&len), sizeof(len));
        file.write(r.site_name.c_str(), len);
        
        len = r.username.length();
        file.write(reinterpret_cast<const char*>(&len), sizeof(len));
        file.write(r.username.c_str(), len);
        
        len = r.encrypted_password.length();
        file.write(reinterpret_cast<const char*>(&len), sizeof(len));
        file.write(r.encrypted_password.c_str(), len);
        
        len = r.iv.length();
        file.write(reinterpret_cast<const char*>(&len), sizeof(len));
        file.write(r.iv.c_str(), len);
        
        len = r.notes.length();
        file.write(reinterpret_cast<const char*>(&len), sizeof(len));
        file.write(r.notes.c_str(), len);
        
        len = r.category.length();
        file.write(reinterpret_cast<const char*>(&len), sizeof(len));
        file.write(r.category.c_str(), len);
        
        file.write(reinterpret_cast<const char*>(&r.created_at), sizeof(r.created_at));
        file.write(reinterpret_cast<const char*>(&r.modified_at), sizeof(r.modified_at));
    }
    file.close();
    
    return true;
}
