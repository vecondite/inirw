#include <iostream>
#define INIRW_CASE_INSENSITIVE // optional: make the reader case-insensitive
#include "../src/inirw/inirw.h"

int main(){
    INIRW::Ini read;
    INIRW::read("examples/test.ini", read);
    std::cout << read["section"]["key"] << "\n"; // returns value
    std::cout << read["adjustables]"]["api=key"] << "\n"; // returns value
    return 0;
}
