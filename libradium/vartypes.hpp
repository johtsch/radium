#ifndef LR_VARTYPES_HPP
#define LR_VARTYPES_HPP

#include <iostream>
#include <string>

/* unterschiedliche Variablentyp-Codes */
const unsigned char VARTYPE_INVALID = 0;       // wenn Datentyp nicht zugeordnet werden kann
const unsigned char VARTYPE_HADDR   = 1;       // Hardwareaddresse
const unsigned char VARTYPE_IPADDR  = 2;       // IPv4-Addresse
const unsigned char VARTYPE_PORT    = 3;       // TCP/UDP-Port
const unsigned char VARTYPE_BYTE    = 4;
const unsigned char VARTYPE_SHORT   = 5;
const unsigned char VARTYPE_INT     = 6;

const std::string LG_VT_INVALID     = "INVALID";
const std::string LG_VT_HADDR       = "HADDR";
const std::string LG_VT_IPADDR      = "IP";
const std::string LG_VT_PORT        = "PORT";
const std::string LG_VT_BYTE        = "BYTE";
const std::string LG_VT_SHORT       = "SHORT";
const std::string LG_VT_INT         = "INT"; 

struct varinfo{
    unsigned char _type;            // siehe VARTYPE_*
    std::string _name;              // Name der Variable innerhalb der .lang-Datei
    long unsigned int _index;                   // der Index der Variable innerhalb der Vector-Struktur in der alle Variablen des gleichen Typs gespeichert werden
};

struct port{
    std::string _info;
    unsigned short _port; 
};

typedef unsigned char byte;

//Funktionen

bool isVartype(std::string word);

unsigned char getVarType(std::string type);

std::string getVarTypeStr(unsigned char type);

#endif