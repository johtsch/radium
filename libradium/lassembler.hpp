#ifndef LR_LASSEMBLER_HPP
#define LR_LASSEMBLER_HPP

#include <iostream>
#include <string>
#include <tins/tins.h>

#include "lconst.hpp"
#include "lfilehandler.hpp"             /* getNextArgument() etc. */ 
#include "lproto.hpp"                   /* isSupportedProtocol */
#include "lcommand.hpp"                 

using namespace Tins;

class Lang;

/* speichert Informationen der ASSEMBLE-Umgebung */
class LAssembler{
public:
    LAssembler(){ _assemble = ""; }
    
    bool            reassemble(const Lang *lang){ return analyse(lang); }

    bool            setAssembler(std::string assemble, const Lang *lang);
    void            setNum(short num){ _num = num; }

    short           getNum() const { return _num; }
    bool            send();

    void            reset();

    void showPacket();
private:
    std::string     _assemble;
    std::string     _proto[6];              /* für jede OSI-Schicht außer der ersten */

    /* mögliche Protokolle (zumindest die bis jetzt implementierten) */
    LEthernet       _eth;
    LARP            _arp;
    LIPv4           _ip;
    LICMP           _icmp;
    LTCP            _tcp;
    LUDP            _udp;
    LDHCP           _dhcp;
    LRaw            _raw;

    short           _num;
    bool            analyse(const Lang *lang);                      /* selektiert die einzelnen Layer und gibt sie an analyseLayer() weiter */
    bool            analyseLayer(const Lang *lang, std::string pro, std::string layer);           /* layer ist Nummer der Schicht, pro ist der Protokollstring (siehe lconst)*/
};

#endif