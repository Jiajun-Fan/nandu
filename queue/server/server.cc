#include <iostream>
#include "queue.hh"
#include "sign.hh"

int main() {
    Task* t = new Task("test");
    IntParam* i1 = new IntParam(1);
    IntParam* i2 = new IntParam(2);
    IntParam* i3 = new IntParam(3);
    t->addParam(i1);
    t->addParam(i2);
    t->addParam(i3);
    Signer signer("fuck");
    std::string whatever("this is cool");
    std::cout << signer.sign(whatever.c_str(), whatever.length()) << std::endl;
    delete t;
    return 0;
}
