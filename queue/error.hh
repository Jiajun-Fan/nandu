#pragma once
#include <iostream>

std::ostream& Error() {
    return std::cerr << "[ Error ]:";
}

std::ostream& Warning() {
    return std::cerr << "[Warning]:";
}

std::ostream& Info() {
    return std::cerr << "[ Info  ]:";
}

std::ostream& Note() {
    return std::cerr << "[ Note  ]:";
}

std::ostream& Debug() {
    return std::cerr << "[ Debug ]:";
}
