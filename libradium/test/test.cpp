#include "lang.hpp"
#include <tins/tins.h>

// g++ test.cpp -I ../ ../libradium.a -ltins -std=c++11

int main(int argc, char **argv){
    Lang langtest;
    langtest.setQuiet(false);
    langtest.loadFile("../../langfiles/conflict.lang");
    langtest.start();
    
    std::cout << std::endl;
    //langtest.showVars();
    //std::cout << std::endl;
    
    std::cout << std::endl;
    langtest.showCmds();

    while(langtest.update());
    
    /*
    std::cout << std::endl;
    langtest.showTrigger();
    */
    std::cout << std::endl;
    langtest.showPacket();
    std::cout << std::endl;
    langtest.showFilter();
    //std::cout << std::endl;
    //langtest.showTrigger();
    return 0;
}