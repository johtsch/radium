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
    if(word == LG_VT_FILE)
        return true;
    if(word == LG_VT_DATA)
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
    if(type == LG_VT_FILE)
        return VARTYPE_FILE;
    if(type == LG_VT_DATA)
        return VARTYPE_DATA;

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
    if(type == VARTYPE_FILE)
        return LG_VT_FILE;
    if(type == VARTYPE_DATA)
        return LG_VT_DATA;

    return LG_VT_INVALID;
}

unsigned char getVarTypeVal(std::string val){
    /** reihenfolge bei den Zahlendatentypen ist entscheidend, da jeder BYTE-Wert ein Shortwert aber nicht jeder Shortwert ein Bytewert sein kann */
    if(isValidHaddr(val))
        return VARTYPE_HADDR;
    if(isValidIPv4(val))
        return VARTYPE_IPADDR;
    if(isValidByte(val))
        return VARTYPE_BYTE;
    if(isValidShort(val))
        return VARTYPE_SHORT;
    if(isValidPort(val))
        return VARTYPE_PORT;
    if(isValidInt(val))
        return VARTYPE_INT;
    if(isValidFile(val))
        return VARTYPE_FILE;
    if(isValidData(val))
        return VARTYPE_DATA;

    return VARTYPE_INVALID;
}

std::string getFileData(std::string fpath){
    std::ifstream f;
    f.open(fpath, std::ios::in);
    
    if(!f.good() | !f.is_open())
        return LANG_NOS;

    std::string data;
    char c;
    
    while(f.get(c)){
        if(c != 0)         
            data += c;           
    }

    // komischerweise wird ans ende der Datei immer noch ein '\n' angehängt (im Bytecode) deswegen letztes Zeichen entfernen
    data.pop_back();

    f.close();
    return data;
}

bool assignVal(HWAddress<6> *eth, std::string val){
    if(!isValidHaddr(val))
        return false;
    *eth = HWAddress<6>(val.c_str());
    return true;
}

bool assignVal(IPv4Address *ip, std::string val){
    if(!isValidIPv4(val))
        return false;
    *ip = IPv4Address(val.c_str());
    return true;
}

bool assignVal(port *p, std::string val){
    if(!isValidPort(val))
        return false;
    *p = (port)atoi(val.c_str());
    return true;
}

bool assignVal(byte *b, std::string val){
    if(!isValidByte(val)){
        return false;
    }
    *b = (byte)atoi(val.c_str());
    return true;
}

bool assignVal(short *sh, std::string val){
    if(!isValidShort(val))
        return false;
    *sh = (short)atoi(val.c_str());
    return true;
}

bool assignVal(int *integer, std::string val){
    if(!isValidInt(val))
        return false;
    *integer = (int)atoi(val.c_str());
    return true;
}

bool assignData(vtdata *dat, std::string val){
    if(!isValidData(val))
        return false;
    *dat = val;
    return true;
}

bool assignFile(vtdata *fil, std::string val){
    if(!isValidFile(val))
        return false;
    *fil = val;
    return true;
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

bool isValidPort(std::string po){
    return isValidShort(po);
}

bool isValidByte(std::string by){
    unsigned long l = std::strtoul(by.c_str(), NULL, 0);                       /* strtoul gibt 0 zurück, wenn po nicht zu unsigned long konvertiert werden kann */

    if(l==0 && by != "0")
        return false;

    byte s = (byte)l;

    if(s==l)
        return true;
    else
        return false;
}

bool isValidShort(std::string sh){
    unsigned long l = std::strtoul(sh.c_str(), NULL, 0);                       /* strtoul gibt 0 zurück, wenn po nicht zu unsigned long konvertiert werden kann */

    if(l==0 && sh != "0")
        return false;

    unsigned short s = (unsigned short)l;

    if(s==l)
        return true;
    else
        return false;
}

bool isValidInt(std::string in){
    unsigned int l = std::strtoul(in.c_str(), NULL, 0);                       /* strtoul gibt 0 zurück, wenn po nicht zu unsigned long konvertiert werden kann */

    if(l==0 && in != "0")
        return false;

    unsigned int s = (unsigned int)l;

    if(s==l)
        return true;
    else
        return false;
}

bool isValidFile(std::string fi){
    std::ifstream tmp;
    tmp.open(fi, std::ios::in);
    bool va = tmp.good() | tmp.is_open();

    tmp.close();

    return va;
}

bool isValidData(std::string da){
    return true;
}