#include <lang.hpp>
#include <tins/tins.h>

// g++ test.cpp -I ../ ../libradium.a -ltins -std=c++11

int main(int argc, char **argv){
    Lang langtest;
    langtest.setQuiet(true);
    langtest.loadFile("../../langfiles/example.lang");
    langtest.start();
    /*
    std::cout << std::endl;
    langtest.showVars();
    std::cout << std::endl;
    */
    langtest.update();
    
    /*std::cout << std::endl;
    langtest.showStep();
    /*
    std::cout << std::endl;
    langtest.showTrigger();
    */

    langtest.showCmds();
    return 0;
}