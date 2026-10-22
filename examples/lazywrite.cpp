#include <iostream>
#include "../src/inirw/inirw.h"

int main(){
    // a lazywrite preserves the content of the file, but merges conflicting values.
    // if the file is to be cleared, check generate.cpp
    INIRW::Ini toAdd;
    INIRW::read("examples/test.ini", toAdd); // must read first, or values will be lost!
    toAdd["section2"]["key"]="value"; // adding new values
    toAdd["SECTION"]["key"]="changed"; // changing existing values, comments are preserved
    toAdd.erase("TOREMOVE"); // remove a section
    toAdd["REMOVEKEY"].erase("removed"); // remove a key

    std::cout << INIRW::write("examples/test.ini", toAdd).out << "\n";
    return 0;
}
