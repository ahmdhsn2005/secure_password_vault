#include "crypto.hpp"
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/sha.h>
#include <sstream>
#include <iomanip>
#include <stdexcept>

using namespace std;

// IMPLEMENTED: Utility functions (10% of crypto module)

string Crypto::bytesToHex(const vector<uint8_t>& bytes) {
    stringstream ss;
    ss << hex << setfill('0');
    for(uint8_t byte : bytes) {
        ss << setw(2) << static_cast<int>(byte);
    }
    return ss.str();
}

vector<uint8_t> Crypto::hexToBytes(const string& hex) {
    vector<uint8_t> bytes;
    for(size_t i = 0; i < hex.length(); i += 2) {
        string byteString = hex.substr(i, 2);
        uint8_t byte = static_cast<uint8_t>(stoi(byteString, nullptr, 16));
        bytes.push_back(byte);
    }
    return bytes;
}

// TODO: Implement the remaining functions

vector<uint8_t> Crypto::generateRandomBytes(int length) {
    // TODO: Implement using OpenSSL RAND_bytes
    // Example code structure:
    // vector<uint8_t> bytes(length);
    // if (RAND_bytes(bytes.data(), length) != 1) {
    //     throw runtime_error("Failed to generate random bytes");
    // }
    // return bytes;
    
    throw runtime_error("generateRandomBytes not implemented yet!");
}

string Crypto::generateSalt() {
    // TODO: Call generateRandomBytes(32) and convert to hex
    throw runtime_error("generateSalt not implemented yet!");
}

string Crypto::generateIV() {
    // TODO: Call generateRandomBytes(16) and convert to hex
    throw runtime_error("generateIV not implemented yet!");
}

string Crypto::deriveKey(const string& password, const string& salt, int iterations) {
    // TODO: Implement PBKDF2-HMAC-SHA256
    // Use OpenSSL's PKCS5_PBKDF2_HMAC function
    // Return 32-byte key as hex string
    throw runtime_error("deriveKey not implemented yet!");
}

string Crypto::hashPassword(const string& password, const string& salt) {
    // TODO: Call deriveKey() with password and salt
    throw runtime_error("hashPassword not implemented yet!");
}

bool Crypto::verifyPassword(const string& password, const string& salt, const string& hash) {
    // TODO: Hash the input password and compare with stored hash
    throw runtime_error("verifyPassword not implemented yet!");
}

string Crypto::encryptAES256(const string& plaintext, const string& key, const string& iv) {
    // TODO: Implement AES-256-CBC encryption using OpenSSL
    // Use EVP_EncryptInit_ex, EVP_EncryptUpdate, EVP_EncryptFinal_ex
    // Return ciphertext as hex string
    throw runtime_error("encryptAES256 not implemented yet!");
}

string Crypto::decryptAES256(const string& ciphertext, const string& key, const string& iv) {
    // TODO: Implement AES-256-CBC decryption using OpenSSL
    // Use EVP_DecryptInit_ex, EVP_DecryptUpdate, EVP_DecryptFinal_ex
    // Return plaintext
    throw runtime_error("decryptAES256 not implemented yet!");
}
