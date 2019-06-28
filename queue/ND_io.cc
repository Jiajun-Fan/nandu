#include "ND_io.hh"

PackageReasonCode NanduReaderWriter::read(NanduOperation& op, Package& package) {
    Package raw;
    PackageReasonCode code = readPackage(raw);
    if (code == PKG_OK) {
        op = *(NanduOperation*)(raw.data());
        package.resize(raw.size()-sizeof(op));
        memcpy(package.data(), raw.data()+sizeof(op), package.size());
    }
    return code;
}

PackageReasonCode NanduReaderWriter::write(const NanduOperation& op, Package& package) {
    Package raw;
    raw.resize(package.size()+sizeof(op));
    memcpy(raw.data(), &op, sizeof(op));
    memcpy(raw.data()+sizeof(op), package.data(), package.size());
    return writePackage(raw);
}

PackageReasonCode NanduReaderWriter::read(NanduOperation& op, std::string& msg) {
    Package raw;
    PackageReasonCode code = readPackage(raw);
    if (code == PKG_OK) {
        raw.push_back('\0');
        op = *(NanduOperation*)(raw.data());
        msg = std::string((const char*)(raw.data()+sizeof(op)));
    }
    return code;
}

PackageReasonCode NanduReaderWriter::write(const NanduOperation& op, const std::string& msg) {
    Package raw;
    raw.resize(msg.length()+sizeof(op));
    memcpy(raw.data(), &op, sizeof(op));
    memcpy(raw.data()+sizeof(op), msg.c_str(), msg.length());
    return writePackage(raw);
}
