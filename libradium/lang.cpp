#include "lang.hpp"

Lang::Lang(){
    _running = false;
    _status = "Lang was initialized";
}

Lang::Lang(std::string fpath){
    _running = false;
    _status = "Lang was initialized";
    loadFile(fpath);
}

Lang::~Lang(){
    _file.close();
}

bool Lang::loadFile(std::string fpath){

    if(!fpath.find(LANG_FEXT)){
       _status = "File does not have the " + LANG_FEXT + " file extension -> File wasn't opened!";
       return false;
    }

    _file.open(fpath);

    if(!_file){
        _status = "Failed to open File: " + fpath;
        return false;
    }

    _filepath = fpath;
    return true;
}

void Lang::start(){
    _running = true;
    init();                 // alle Variablen einlesen
}

bool Lang::init(){
    std::string tmp;
    while((tmp=analyse())!=LANG_IMPLEMENTATION){
        if(tmp == LANG_VAR)
            analyseVar();
        if(tmp == EOF){
            _status = "File does NOT contain an IMPLEMENTATION environment -> can't be executed!";
        }
    }
}