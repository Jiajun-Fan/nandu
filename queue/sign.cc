#include "sign.hh"
#include <openssl/sha.h>
#include <sstream>
#include <iomanip>

std::string Signer::sign(const void* buff, size_t size) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX cipher;
    SHA256_Init(&cipher);
    SHA256_Update(&cipher, _key.c_str(), _key.length());
    SHA256_Update(&cipher, buff, size);
    SHA256_Final(hash, &cipher);
    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)(hash[i]);
    }
    return ss.str();
}
