#include "lang.hpp"

#include <tins/tins.h>      /* libtins für Netzwerkfunktionen etc. */

#include "vartypes.hpp"       /* Datentypdefinitionen */
#include "header.hpp"         /* Protokollheaderformate etc. */

Lang::Lang(){
    _running = false;
    _status = "Lang was initialized";
    if(!_quiet)
        std::cout << "Lang: Lang(): " << _status << std::endl;
}

Lang::Lang(std::string fpath){
    _running = false;
    _status = "Lang was initialized";
    if(!_quiet)
        std::cout << "Lang: Lang(): " << _status << std::endl;
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

    _file.open(fpath, std::ios::in);

    if(!_file.good()){
        _status = "Failed to open File: " + fpath;
        return false;
    }

    _filepath = fpath;
    return true;
}

bool Lang::start(){
    return (_running = init());                 // alle Variablen einlesen
}

bool Lang::init(){
    _file.seekg(std::ios::beg);
    std::string tmp;
    while(true){

        tmp=analyse();

        if(tmp == LANG_B_VAR){
            //analyseVar();
            _status = "VAR-Teil erreicht";
            if(!_quiet)
                std::cout << "Lang: init(): " << _status << std::endl;
        } 
        if(tmp == LANG_E_VAR){
            //analyseVar();
            _status = "VAR-End-Teil erreicht";
            if(!_quiet)
                std::cout << "Lang: init(): " << _status << std::endl;
        }    
        if(tmp == LANG_B_IMPLEMENTATION){
            _status = "IMPLEMENTATION-Teil erreicht";
            if(!_quiet)
                std::cout << "Lang: init(): " << _status << std::endl;
            break;
        }    
        if(tmp == LANG_EOF){
            _status = "File does NOT contain an IMPLEMENTATION environment -> can't be executed!";
            if(!_quiet)
                std::cout << "Lang: init(): " << _status << std::endl;
            return false;
        }
    }
    return true;
}

std::string Lang::analyse(){
    char line[1024];
    std::string word ="";

    bool ret = false;

    while(true){
        _file.clear();                      /* damit, wenn nur < 250 Byte ausgelesen werden können, trz weitergemacht wird.
        //                                       die schlechten bits wie ios::fail werden durch clear zurückgesetzt. */
        //std::getline(_file, line);
        memset(line, '\0', 1024);
        _file.getline(line, 1024);
        word.clear();
        std::cout << " > " << line << std::endl;
        
        for(int i=0; i < sizeof(line); i++){
            if(line[i] != ' ' && line[i] != '\0' && line[i] != '\t' && line[i] != ';'){             /* !!WICHTIG!! es muss auf '\0' anstelle von '\n' geprüft werden, da getline nicht
                                                                                                       den Zeilenumbruch mitliefert, sondern den String beendet, wenn es '\n' erreicht */
                word+=line[i];
            }
            else{
                if(word.size() > 2)
                    std::cout << " >>> " << word << std::endl;

                ret |= (word == LANG_B_IMPLEMENTATION);
                ret |= (word == LANG_B_VAR);
                ret |= (word == LANG_B_STEP);
                ret |= (word == LANG_B_TRIGGER);
                ret |= (word == LANG_E_IMPLEMENTATION);
                ret |= (word == LANG_E_VAR);
                ret |= (word == LANG_E_STEP);
                ret |= (word == LANG_E_TRIGGER);

                if(ret)
                    return word;

                word.clear();
            }
        }

        if(_file.eof())
            return LANG_EOF; 
    }
}
