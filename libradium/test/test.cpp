#include "lang.hpp"
#include <tins/tins.h>

// g++ test.cpp -I ../ ../libradium.a -ltins -std=c++11

int main(int argc, char **argv){
    Lang langtest;
    langtest.setQuiet(false);
    langtest.loadFile("../../langfiles/dhcptest2.lang");
    langtest.start();
    
    std::cout << std::endl;
    langtest.showVars();

    while(langtest.update());
    
    return 0;
}