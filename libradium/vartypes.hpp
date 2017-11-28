#ifndef LR_VARTYPES_HPP
#define LR_VARTYPES_HPP

#include <string.h>

#define VARTYPE_HADDR   1       // Hardwareaddresse
#define VARTYPE_IPADDR  2       // IPv4-Addresse
#define VARTYPE_PORT    3       // TCP/UDP-Port
#define VARTYPE_BYTE    4
#define VARTYPE_SHORT   5
#define VARTYPE_INT     6

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

#endif