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

bool isValidHaddr(std::string address){
    for(int i = 0; i < 17; i++) {
        if(i % 3 != 2 && !std::isxdigit(address[i]))
            return false;
        if(i % 3 == 2 && address[i] != ':')
            return false;
    }
    if(address[17] != '\0')
        return false;
    return true;
}

bool isValidIPv4(std::string address){
    unsigned char buf[sizeof(struct in6_addr)];             /* ist unabdingbar. Wenn bei inet_pton kein buf angegeben wird kommt es zu Speicherzugriffsfehlern */
    return (inet_pton(AF_INET, address.c_str(), buf) == 1);
}