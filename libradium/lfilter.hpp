#ifndef LR_LFILTER_HPP
#define LR_LFILTER_HPP

#include <iostream>
#include <string>
#include <tins/tins.h>

#include "lconst.hpp"
#include "lfilehandler.hpp"             /* getNextArgument() etc. */ 
#include "lproto.hpp"                   /* isSupportedProtocol */
#include "lcommand.hpp"                 


class Lang;


class LFilter{
public:
    LFilter(){_filter="";}

    enum TYPE { PASS, REACTION };

    bool            setFilter(std::string pfilter, const Lang *lang);
    void            setType(TYPE t){ _type = t; }
    TYPE            getType() const{ return _type; }
    void            setNum(short n){ _num = n;}
    short           getNum() const { return _num; }
    bool            compare(const PDU *pdu);

    void showFilter();
private:
    /* speichert den Filtertyp */
    TYPE            _type;
    /* speichert die Nummer der REACTION die ausgeführt werden soll, wenn der Filter anschlägt
       nur wenn _type == TYPE::REACTION */
    short           _num;
    /* speichert den Text der aktuellen PACKETFILTER-Umgebung */
    std::string     _filter;
    /* speichert die Reihenfolge der Protokolle, wie sie im Filter festgelegt werden (6 Schichten des OSI-Modells sind möglich)*/
    std::string     _layer[6];
    /* für jedes mögliche Protokoll eine Member-Instanz anlegen, die im Falle, dass sie benötigt wird festgelegt werden kann */
    LEthernet       _ethernet;
    LARP            _arp;      
    LIPv4           _ip;

    bool            analyse(const Lang *lang);                                                    /* selektiert die einzelnen Layer und gibt sie an analyseLayer() weiter */
    bool            analyseLayer(const Lang *lang, std::string pro, std::string layer, int num);  /* pro ist der Protokollstring (siehe lconst); num speichert nummer der Schicht im OSI-Modell */
};

#endif