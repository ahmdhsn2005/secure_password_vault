#include "crypto.hpp"
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/sha.h>
#include <sstream>
#include <iomanip>
#include <stdexcept>

using namespace std;

// DONE: hex conversion helpers

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

// Generate cryptographically secure random bytes
vector<uint8_t> Crypto::generateRandomBytes(int length) {
    vector<uint8_t> bytes(length);
    if(RAND_bytes(bytes.data(), length) != 1) {
        throw runtime_error("Failed to generate random bytes");
    }
    return bytes;
}

// Generate 32-byte salt for password hashing
string Crypto::generateSalt() {
    return bytesToHex(generateRandomBytes(32));
}

// Generate 16-byte IV for AES encryption
string Crypto::generateIV() {
    return bytesToHex(generateRandomBytes(16));
}

// Derive encryption key from password using PBKDF2-HMAC-SHA256
string Crypto::deriveKey(const string& password, const string& salt, int iterations) {
    vector<uint8_t> salt_bytes = hexToBytes(salt);
    vector<uint8_t> key(32);  // 256 bits
    
    if(PKCS5_PBKDF2_HMAC(
        password.c_str(), password.length(),
        salt_bytes.data(), salt_bytes.size(),
        iterations,
        EVP_sha256(),
        32, key.data()) != 1) {
        throw runtime_error("PBKDF2 key derivation failed");
    }
    
    return bytesToHex(key);
}

// Hash password for storage
string Crypto::hashPassword(const string& password, const string& salt) {
    return deriveKey(password, salt, 100000);
}

// Verify password against stored hash
bool Crypto::verifyPassword(const string& password, const string& salt, const string& hash) {
    string computed_hash = hashPassword(password, salt);
    return computed_hash == hash;
}

// Encrypt plaintext using AES-256-CBC
string Crypto::encryptAES256(const string& plaintext, const string& key, const string& iv) {
    vector<uint8_t> key_bytes = hexToBytes(key);
    vector<uint8_t> iv_bytes = hexToBytes(iv);
    
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if(!ctx) throw runtime_error("Failed to create cipher context");
    
    // Initialize encryption
    if(EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key_bytes.data(), iv_bytes.data()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw runtime_error("Failed to initialize encryption");
    }
    
    // Allocate output buffer
    vector<uint8_t> ciphertext(plaintext.length() + EVP_CIPHER_block_size(EVP_aes_256_cbc()));
    int len = 0, ciphertext_len = 0;
    
    // Encrypt
    if(EVP_EncryptUpdate(ctx, ciphertext.data(), &len, 
                         reinterpret_cast<const unsigned char*>(plaintext.c_str()), 
                         plaintext.length()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw runtime_error("Encryption failed");
    }
    ciphertext_len = len;
    
    // Finalize
    if(EVP_EncryptFinal_ex(ctx, ciphertext.data() + len, &len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw runtime_error("Encryption finalization failed");
    }
    ciphertext_len += len;
    
    EVP_CIPHER_CTX_free(ctx);
    
    ciphertext.resize(ciphertext_len);
    return bytesToHex(ciphertext);
}

// Decrypt ciphertext using AES-256-CBC
string Crypto::decryptAES256(const string& ciphertext, const string& key, const string& iv) {
    vector<uint8_t> key_bytes = hexToBytes(key);
    vector<uint8_t> iv_bytes = hexToBytes(iv);
    vector<uint8_t> ciphertext_bytes = hexToBytes(ciphertext);
    
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if(!ctx) throw runtime_error("Failed to create cipher context");
    
    // Initialize decryption
    if(EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key_bytes.data(), iv_bytes.data()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw runtime_error("Failed to initialize decryption");
    }
    
    // Allocate output buffer
    vector<uint8_t> plaintext(ciphertext_bytes.size() + EVP_CIPHER_block_size(EVP_aes_256_cbc()));
    int len = 0, plaintext_len = 0;
    
    // Decrypt
    if(EVP_DecryptUpdate(ctx, plaintext.data(), &len, 
                         ciphertext_bytes.data(), 
                         ciphertext_bytes.size()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw runtime_error("Decryption failed");
    }
    plaintext_len = len;
    
    // Finalize
    if(EVP_DecryptFinal_ex(ctx, plaintext.data() + len, &len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw runtime_error("Decryption finalization failed");
    }
    plaintext_len += len;
    
    EVP_CIPHER_CTX_free(ctx);
    
    return string(reinterpret_cast<char*>(plaintext.data()), plaintext_len);
}
