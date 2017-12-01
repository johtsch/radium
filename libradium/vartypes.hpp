#ifndef LR_VARTYPES_HPP
#define LR_VARTYPES_HPP

#include <iostream>
#include <string>

#define VARTYPE_HADDR   1       // Hardwareaddresse
#define VARTYPE_IPADDR  2       // IPv4-Addresse
#define VARTYPE_PORT    3       // TCP/UDP-Port
#define VARTYPE_BYTE    4
#define VARTYPE_SHORT   5
#define VARTYPE_INT     6

const std::string LG_VT_HADDR = "HADDR";
const std::string LG_VT_IPADDR = "IP";
const std::string LG_VT_PORT = "PORT";
const std::string LG_VT_BYTE = "BYTE";
const std::string LG_VT_SHORT = "SHORT";
const std::string LG_VT_INT = "INT"; 

struct varinfo{
    char _type;
    std::string _name;
    short _index;                   // der Index der Variable innerhalb der Vector-Struktur in der alle Variablen des gleichen Typs gespeichert werden
};

struct port{
    std::string _info;
    unsigned short _port; 
};

typedef unsigned char byte;

//Funktionen

bool isVartype(std::string word);

#endif