#include <iostream>
#include "../src/inirw/inirw.h"

int main(){
    INIRW::Ini toAdd;
    toAdd["GENSECT"]["newkey"]="newval";
    std::cout << INIRW::generate("examples/generated.ini", toAdd).out << "\n";
    return 0;
}
