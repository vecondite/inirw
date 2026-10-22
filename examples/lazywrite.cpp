#include <iostream>
#include "../src/inirw/inirw.h"

int main(){
    // a lazywrite preserves the content of the file, but merges conflicting values.
    // if the file is to be cleared, check generate.cpp
    INIRW::Ini toAdd;
    toAdd["section2"]["key"]="value"; // adding new values
    toAdd["SECTION"]["key"]="changed"; // changing existing values, comments are preserved
    std::cout << INIRW::write("examples/test.ini", toAdd).out << "\n";
    return 0;
}
