#include <iostream>
#include "../src/inirw/inirw.h"

int main(){
    INIRW::Ini read;
    INIRW::Output out = INIRW::read("examples/test.ini", read);
    if(out.stat){
        std::cout << out.out << "\n";
    }else{
        std::cerr << out.out << "\n";
        return 1;
    }
    for(const INIRW::Ini::Entry& section : read){
        std::cout << "SECTION: " << section.first << "\n";
        for(const INIRW::SectData::Entry& kV : section.second){
            std::cout << "KEY: " << kV.first << " || " << "VALUE: " << kV.second << "\n";
        }
    }
}
