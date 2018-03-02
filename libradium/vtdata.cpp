#include "vtdata.hpp"

vtdata::vtdata(){
    _data.clear();
}

vtdata::vtdata(const std::string &str){
    operator=(str);
}

vtdata::vtdata(const char *chr){
    operator=(std::string(chr));
}

std::string vtdata::str(){
    std::string str = "";

    for(int i = 0; i < _data.size(); ++i){
        if(_data[i] != '\0')
            str+=_data[i];
        else    
            break;
    }
    return str;
}

vtdata& vtdata::operator=(const std::string &str){
    _data.clear();
    for(int i = 0;  i < str.length(); ++i){
        _data.push_back(str[i]);
    }
}

/* Überlade cout-Operator */

std::ostream& operator<<(std::ostream &out, const vtdata &data){
    for(int i = 0; i < data.size(); ++i){
        
        if( (data[i] >= 32 && data[i] <= 126) || data[i] == 10)       // darstellbare zeichen darstellen
            out << data[i];
        else    
            out << ".";                             // die anderen mit einem punkt darstellenc
    }

    return out;
}