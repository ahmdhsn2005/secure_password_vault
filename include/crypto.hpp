#ifndef CRYPTO_HPP
#define CRYPTO_HPP

#include <string>
#include <vector>

using namespace std;

// Crypto Module - Handles all encryption and security operations
// TODO: Implement using OpenSSL library
class Crypto {
public:
    // Generate random bytes (for salt, IV, etc.)
    // TODO: Use OpenSSL RAND_bytes()
    static vector<uint8_t> generateRandomBytes(int length);
    
    // Generate a random salt (32 bytes) for password hashing
    // TODO: Call generateRandomBytes(32) and convert to hex
    static string generateSalt();
    
    // Generate a random IV (16 bytes for AES-256-CBC)
    // TODO: Call generateRandomBytes(16) and convert to hex
    static string generateIV();
    
    // Derive encryption key from password using PBKDF2-HMAC-SHA256
    // TODO: Implement using EVP_PBE_scrypt or PKCS5_PBKDF2_HMAC
    // Recommended: 100,000 iterations
    static string deriveKey(const string& password, const string& salt, int iterations = 100000);
    
    // Hash password for storage
    // TODO: Use deriveKey() function
    static string hashPassword(const string& password, const string& salt);
    
    // Verify password against stored hash
    // TODO: Hash the input password and compare with stored hash
    static bool verifyPassword(const string& password, const string& salt, const string& hash);
    
    // Encrypt plaintext using AES-256-CBC
    // TODO: Use OpenSSL EVP_EncryptInit_ex, EVP_EncryptUpdate, EVP_EncryptFinal_ex
    static string encryptAES256(const string& plaintext, const string& key, const string& iv);
    
    // Decrypt ciphertext using AES-256-CBC
    // TODO: Use OpenSSL EVP_DecryptInit_ex, EVP_DecryptUpdate, EVP_DecryptFinal_ex
    static string decryptAES256(const string& ciphertext, const string& key, const string& iv);
    
    // Utility: Convert byte array to hexadecimal string
    static string bytesToHex(const vector<uint8_t>& bytes);
    
    // Utility: Convert hexadecimal string to byte array
    static vector<uint8_t> hexToBytes(const string& hex);
};

#endif
