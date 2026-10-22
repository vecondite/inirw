#include <iostream>
#include "../src/inirw/inirw.h"

int main(){
    INIRW::Ini read;
    INIRW::Output output = INIRW::read("non-existing.ini", read);
    std::cout << output.stat << "\n"; // boolean that states whether the command succeeded or not.
    std::cout << output.out << "\n"; // a message stating what error happened.
    if(!output){ // works because the Output class explicitly returns output.stat by default
        std::cout << "An error occured!\n" << output.out << "\n";
    }
    // the same syntax applies for write, and generate.
    return 0;
}
