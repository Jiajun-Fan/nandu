#pragma once
#include <iostream>

inline std::ostream& Error() {
    return std::cerr << "[ Error ]: ";
}

inline std::ostream& Warning() {
    return std::cerr << "[Warning]: ";
}

inline std::ostream& Info() {
    return std::cerr << "[ Info  ]: ";
}

inline std::ostream& Note() {
    return std::cerr << "[ Note  ]: ";
}

inline std::ostream& Debug() {
    return std::cerr << "[ Debug ]: ";
}
