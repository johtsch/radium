#ifndef LR_LPROTO_HPP
#define LR_LPROTO_HPP

#include <tins/tins.h>
#include <tins/arp.h>
#include <string>

#include "lcommand.hpp"
#include "vartypes.hpp"
#include "lconst.hpp"

using namespace Tins;

bool isSupportedProtocol(std::string p);

class Lang;

class LEthernet{
public:
    LEthernet(){}
    void reset();

    const static std::string s_fields[];
    const static unsigned char s_type[];
    enum s_fenum { SRC, DST };

    bool assign(lcommand cmd, const Lang *lang);

    void inner_pdu(PDU *pdu){ _eth.inner_pdu(*pdu); };
    EthernetII* getEth(){ return &_eth; }

private:
    EthernetII      _eth;
};




class LARP{
public:
    LARP(){}
    void reset();

    const static std::string s_fields[];
    const static unsigned char s_type[];
    enum s_fenum { SENDER_IP, SENDER_HW, TARGET_IP, TARGET_HW, OPCODE };

    bool assign(lcommand cmd, const Lang *lang);

    void inner_pdu(PDU *pdu){ _arp.inner_pdu(*pdu); }
    ARP* getARP(){ return &_arp; }

private:
    ARP      _arp;
};

#endif