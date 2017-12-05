#include "vartypes.hpp"

bool isVartype(std::string word){
    if(word == LG_VT_HADDR)
        return true;
    if(word == LG_VT_IPADDR)
        return true;
    if(word == LG_VT_PORT)
        return true;
    if(word == LG_VT_BYTE)
        return true;
    if(word == LG_VT_SHORT)
        return true;
    if(word == LG_VT_INT)
        return true;

    return false;
}

unsigned char getVarType(std::string type){
    if(type == LG_VT_HADDR)
        return VARTYPE_HADDR;
    if(type == LG_VT_IPADDR)
        return VARTYPE_IPADDR;
    if(type == LG_VT_PORT)
        return VARTYPE_PORT;
    if(type == LG_VT_BYTE)
        return VARTYPE_BYTE;
    if(type == LG_VT_SHORT)
        return VARTYPE_SHORT;
    if(type == LG_VT_INT)
        return VARTYPE_INT;

    return VARTYPE_INVALID;
}

std::string getVarTypeStr(unsigned char type){
    if(type == VARTYPE_HADDR)
        return LG_VT_HADDR;
    if(type == VARTYPE_IPADDR)
        return LG_VT_IPADDR;
    if(type == VARTYPE_PORT)
        return LG_VT_PORT;
    if(type == VARTYPE_BYTE)
        return LG_VT_BYTE;
    if(type == VARTYPE_SHORT)
        return LG_VT_SHORT;
    if(type == VARTYPE_INT)
        return LG_VT_INT;

    return LG_VT_INVALID;
}