#include <lang.hpp>
#include <tins/tins.h>

// g++ test.cpp -I ../ ../libradium.a -ltins -std=c++11

int main(int argc, char **argv){
    Lang langtest;
    langtest.setQuiet(false);
    langtest.loadFile("../../langfiles/arpatt.lang");
    langtest.start();
    
    std::cout << std::endl;
    langtest.showVars();
    std::cout << std::endl;
    
    langtest.update();
    
    std::cout << std::endl;
    langtest.showStep();/*
    /*
    std::cout << std::endl;
    langtest.showTrigger();
    */
    std::cout << std::endl;
    langtest.showCmds();
    std::cout << std::endl;
    langtest.showPacket();
    return 0;
}