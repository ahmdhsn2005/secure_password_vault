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

// TODO: rest of the crypto stuff

vector<uint8_t> Crypto::generateRandomBytes(int length) {
    // TODO: use RAND_bytes
    throw runtime_error("generateRandomBytes not implemented yet!");
}

string Crypto::generateSalt() {
    // TODO: generateRandomBytes(32) then bytesToHex
    throw runtime_error("generateSalt not implemented yet!");
}

string Crypto::generateIV() {
    // TODO: generateRandomBytes(16) then bytesToHex
    throw runtime_error("generateIV not implemented yet!");
}

string Crypto::deriveKey(const string& password, const string& salt, int iterations) {
    // TODO: PKCS5_PBKDF2_HMAC with 100k iterations
    throw runtime_error("deriveKey not implemented yet!");
}

string Crypto::hashPassword(const string& password, const string& salt) {
    // TODO: just call deriveKey
    throw runtime_error("hashPassword not implemented yet!");
}

bool Crypto::verifyPassword(const string& password, const string& salt, const string& hash) {
    // TODO: hash it and compare strings
    throw runtime_error("verifyPassword not implemented yet!");
}

string Crypto::encryptAES256(const string& plaintext, const string& key, const string& iv) {
    // TODO: EVP_EncryptInit_ex, Update, Final
    throw runtime_error("encryptAES256 not implemented yet!");
}

string Crypto::decryptAES256(const string& ciphertext, const string& key, const string& iv) {
    // TODO: EVP_DecryptInit_ex, Update, Final
    throw runtime_error("decryptAES256 not implemented yet!");
}
