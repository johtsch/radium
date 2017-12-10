#include <lang.hpp>
#include <tins/tins.h>

// g++ test.cpp -I ../ ../libradium.a -ltins -std=c++11

int main(int argc, char **argv){
    Lang langtest;
    langtest.setQuiet(false);
    langtest.loadFile("../../langfiles/example.lang");
    langtest.start();
    std::cout << "HIER" << std::endl;
    std::cout << std::endl;
    langtest.showVars();
    std::cout << std::endl;
    langtest.update();
    std::cout << std::endl;
    langtest.showStep();
    return 0;
}