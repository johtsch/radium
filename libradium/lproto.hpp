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
PDU::PDUType LangProtocolToPDUType(std::string pro);

class Lang;

class LEthernet{
public:
    LEthernet(){}
    void reset();

    const static std::string s_fields[];
    const static unsigned char s_type[];
    enum s_fenum { SRC, DST };

    bool assign(lcommand cmd, const Lang *lang);
    bool compare(const EthernetII *eth);
    bool isField(std::string field, int *which);

    HWAddress<6> getHW(std::string field);

    void setEth(const EthernetII *eth){ _eth = *eth; }
    void inner_pdu(PDU *pdu){ _eth.inner_pdu(*pdu); };
    EthernetII* getEth(){ return &_eth; }

private:
    EthernetII      _eth;
};




class LARP{
public:
    LARP(){_flagsset = false;
    _arp.opcode(ARP::REQUEST);}
    void reset();

    const static std::string s_fields[];
    const static unsigned char s_type[];
    enum s_fenum { SENDER_IP, SENDER_HW, TARGET_IP, TARGET_HW, OPCODE };

    bool assign(lcommand cmd, const Lang *lang);
    bool compare(const ARP *arp);
    bool isField(std::string field, int *which);

    HWAddress<6>    getHW(std::string field);
    IPv4Address     getIP(std::string field);
    byte            getByte(std::string field);

    void setARP(const ARP *arp){ _arp = *arp; }
    void inner_pdu(PDU *pdu){ _arp.inner_pdu(*pdu); }
    ARP* getARP(){ return &_arp; }
    bool flagsSet(){ return _flagsset; }

private:
    ARP      _arp;
    bool     _flagsset;
};

#endif