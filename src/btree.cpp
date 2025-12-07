#include "btree.hpp"
#include <fstream>
#include <cstring>
#include <ctime>

using namespace std;

// basic constructor

BTree::BTree(const string& filename) : filename(filename), root_id(0), next_node_id(1), next_record_id(1) {
    // TODO: load from file or make new root
}

// TODO: everything else

bool BTree::insert(const VaultRecord& record) {
    // TODO: split root if full, then insertNonFull
    throw runtime_error("insert not implemented yet!");
}

vector<VaultRecord> BTree::search(const string& site_name) {
    // TODO: binary search in nodes, follow children
    throw runtime_error("search not implemented yet!");
}

vector<VaultRecord> BTree::getAllRecordsForUser(uint64_t user_id) {
    // TODO: scan all leaf nodes, filter by user_id
    throw runtime_error("getAllRecordsForUser not implemented yet!");
}

bool BTree::update(uint64_t record_id, const VaultRecord& record) {
    // TODO: find and update, might need to move if site_name changes
    throw runtime_error("update not implemented yet!");
}

bool BTree::remove(uint64_t record_id) {
    // TODO: delete is tricky, maybe just mark deleted?
    throw runtime_error("remove not implemented yet!");
}

// TODO: private helpers
// readNode, writeNode, readRecord, writeRecord
// searchNode, insertNonFull, splitChild
