#include <iostream>
#include "../src/inirw/inirw.h"

int main(){
    INIRW::Ini read;
    INIRW::read("examples/test.ini", read);
    std::cout << read["SECTION"]["key"] << "\n"; // returns value
    std::cout << read["adjustables]"]["api=key"] << "\n"; // returns value
    return 0;
}
