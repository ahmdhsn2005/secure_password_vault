#ifndef CRYPTO_HPP
#define CRYPTO_HPP

#include <string>
#include <vector>
#include <cstdint>

using namespace std;

// Crypto stuff - encryption and hashing
// TODO: Use OpenSSL
class Crypto {
public:
    // Make random bytes for salt, IV, etc.
    // TODO: Use RAND_bytes from OpenSSL
    static vector<uint8_t> generateRandomBytes(int length);
    
    // Make a 32-byte salt
    // TODO: Just call generateRandomBytes(32)
    static string generateSalt();
    
    // Make a 16-byte IV for AES
    // TODO: Call generateRandomBytes(16)
    static string generateIV();
    
    // Turn password into encryption key with PBKDF2
    // TODO: Use PKCS5_PBKDF2_HMAC, 100k iterations
    static string deriveKey(const string& password, const string& salt, int iterations = 100000);
    
    // Hash password for storing
    // TODO: Just use deriveKey()
    static string hashPassword(const string& password, const string& salt);
    
    // Check if password matches
    // TODO: Hash it and compare
    static bool verifyPassword(const string& password, const string& salt, const string& hash);
    
    // Encrypt with AES-256-CBC
    // TODO: OpenSSL EVP functions
    static string encryptAES256(const string& plaintext, const string& key, const string& iv);
    
    // Decrypt with AES-256-CBC
    // TODO: OpenSSL EVP functions
    static string decryptAES256(const string& ciphertext, const string& key, const string& iv);
    
    // Helper: bytes to hex string
    static string bytesToHex(const vector<uint8_t>& bytes);
    
    // Helper: hex string to bytes
    static vector<uint8_t> hexToBytes(const string& hex);
};

#endif
