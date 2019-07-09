#pragma once
#include <string>

class Hasher {
public:
    explicit Hasher(const std::string& key) : _key(key) {}
    virtual ~Hasher() {}
    std::string hash(const void* buff, size_t size);
private:
    std::string                         _key;
};
