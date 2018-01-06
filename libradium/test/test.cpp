#include "lang.hpp"
#include <tins/tins.h>

// g++ test.cpp -I ../ ../libradium.a -ltins -std=c++11

int main(int argc, char **argv){
    Lang langtest;
    langtest.setQuiet(true);
    langtest.loadFile("../../langfiles/synflood.lang");
    langtest.start();
    
    std::cout << std::endl;
    langtest.showVars();
    //std::cout << std::endl;

    while(langtest.update());
    
    return 0;
}