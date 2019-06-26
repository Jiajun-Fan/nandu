#pragma once
#include <string>

class Signer {
public:
    explicit Signer(const std::string& key) : _key(key) {}
    virtual ~Signer() {}
    std::string sign(const void* buff, size_t size);
private:
    std::string                         _key;
};
