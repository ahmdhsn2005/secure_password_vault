#include "btree.hpp"
#include <fstream>
#include <cstring>
#include <ctime>

using namespace std;

// PARTIALLY IMPLEMENTED: Constructor and basic structure (5% of btree module)

BTree::BTree(const string& filename) : filename(filename), root_id(0), next_node_id(1), next_record_id(1) {
    // TODO: Load metadata from file if exists
    // TODO: Create initial root node if new file
}

// TODO: Implement all the methods below

bool BTree::insert(const VaultRecord& record) {
    // TODO: Implement B-Tree insertion
    // Steps:
    // 1. If root is full, split it
    // 2. Call insertNonFull on appropriate node
    // 3. Write updated nodes to disk
    throw runtime_error("insert not implemented yet!");
}

vector<VaultRecord> BTree::search(const string& site_name) {
    // TODO: Implement B-Tree search
    // Steps:
    // 1. Start from root node
    // 2. Binary search within node
    // 3. If not found and not leaf, recurse to appropriate child
    // 4. Return matching records
    throw runtime_error("search not implemented yet!");
}

vector<VaultRecord> BTree::getAllRecordsForUser(uint64_t user_id) {
    // TODO: Traverse entire tree and collect records for this user
    // This is an O(n) operation - scan all leaf nodes
    throw runtime_error("getAllRecordsForUser not implemented yet!");
}

bool BTree::update(uint64_t record_id, const VaultRecord& record) {
    // TODO: Find record by ID and update it
    // You may need to rebuild part of the tree if site_name changes
    throw runtime_error("update not implemented yet!");
}

bool BTree::remove(uint64_t record_id) {
    // TODO: Implement B-Tree deletion
    // This is the most complex operation!
    // Consider implementing "lazy deletion" first (mark as deleted)
    throw runtime_error("remove not implemented yet!");
}

// TODO: Implement private helper methods:
// - readNode(uint64_t node_id) -> BTreeNode
// - writeNode(const BTreeNode& node)
// - readRecord(uint64_t record_id) -> VaultRecord
// - writeRecord(const VaultRecord& record)
// - searchNode(const BTreeNode& node, const string& key)
// - insertNonFull(BTreeNode& node, const VaultRecord& record)
// - splitChild(BTreeNode& parent, int index)
