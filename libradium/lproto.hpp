#ifndef LR_LPROTO_HPP
#define LR_LPROTO_HPP

#include <tins/tins.h>
#include <tins/arp.h>
#include <string>
#include <iostream>
#include <vector>

#include "lcommand.hpp"
#include "vartypes.hpp"
#include "lconst.hpp"
#include "vtdata.hpp"

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

class LIPv4{
public:
    LIPv4(){_flagsset = false; _tosset = false; _idset=false; _ttlset=0;}
    void reset();

    const static std::string s_fields[];
    const static unsigned char s_type[];
    enum s_fenum { TOS, ID, FLAGS, TTL, SRC_ADDR, DST_ADDR};

    bool assign(lcommand cmd, const Lang *lang);
    bool compare(const IP *ip);
    bool isField(std::string field, int *which);

    IPv4Address     getIP(std::string field);
    short           getShort(std::string field);
    byte            getByte(std::string field);

    void setIP(const IP *ip){ _ip = *ip; }
    void inner_pdu(PDU *pdu){ _ip.inner_pdu(*pdu); }
    IP* getIP(){ return &_ip; }

private:
    IP       _ip;
    bool     _flagsset;
    bool     _tosset;
    bool     _idset;
    bool     _ttlset;
};

class LICMP{
public:
    LICMP(){_codeset[0] = false; _codeset[1] = false; _codeset[2] = false; _codeset[3] = false; _codeset[4] = false; _codeset[5] = false;}
    void reset();

    const static std::string s_fields[];
    const static unsigned char s_type[];
    enum s_fenum { CODE, TYPE, MTU, ID, SEQ, GATEWAY  };

    bool assign(lcommand cmd, const Lang *lang);
    bool compare(const ICMP *icmp);
    bool isField(std::string field, int *which);

    short           getShort(std::string field);
    byte            getByte(std::string field);
    IPv4Address     getIP(std::string);

    void setICMP(const ICMP *icmp){ _icmp = *icmp; }
    void inner_pdu(PDU *pdu){ _icmp.inner_pdu(*pdu); }
    ICMP* getICMP(){ return &_icmp; }

private:
    ICMP     _icmp;
    bool     _codeset[6];
};

class LTCP{
public:
    LTCP(){_flagsset=false;}
    void reset();

    const static std::string s_fields[];
    const static unsigned char s_type[];
    enum s_fenum { DPORT, SPORT, SEQ, ACK_SEQ, WINDOW, FLAGS };

    bool assign(lcommand cmd, const Lang *lang);
    bool compare(const TCP *tcp);
    bool isField(std::string field, int *which);

    short           getShort(std::string field);
    byte            getByte(std::string field);
    int             getInt(std::string field);

    void setTCP(const TCP *tcp){ _tcp = *tcp; }
    void inner_pdu(PDU *pdu){ _tcp.inner_pdu(*pdu); }
    TCP* getTCP(){ return &_tcp; }

private:
    TCP     _tcp;
    bool     _flagsset;
};

class LUDP{
public:
    LUDP(){}
    void reset();

    const static std::string s_fields[];
    const static unsigned char s_type[];
    enum s_fenum { DPORT, SPORT, LENGTH };

    bool assign(lcommand cmd, const Lang *lang);
    bool compare(const UDP *udp);
    bool isField(std::string field, int *which);

    short           getShort(std::string field);

    void setUDP(const UDP *udp){ _udp = *udp; }
    void inner_pdu(PDU *pdu){ _udp.inner_pdu(*pdu); }
    UDP* getUDP(){ return &_udp; }

private:
    UDP     _udp;
};

class LDHCP{
public:
    LDHCP();
    void reset();

    const static std::string s_fields[];
    const static unsigned char s_type[];
    enum s_fenum { OPCODE, HTYPE, HLEN, HOPS, XID, CIADDR, YIADDR, SIADDR, GIADDR, CHADDR, TYPE, BPFILE, LEASE, SERVER_ID};

    bool assign(lcommand cmd, const Lang *lang);
    bool compare(const DHCP *dhcp);
    bool isField(std::string field, int *which);
    void end(){ _dhcp.end(); }

    byte            getByte(std::string field);
    int             getInt(std::string field);
    IPv4Address     getIP(std::string field);
    HWAddress<6>    getHW(std::string field);

    void setDHCP(const DHCP *dhcp){ _dhcp = *dhcp; }
    void inner_pdu(PDU *pdu){ _dhcp.inner_pdu(*pdu); }
    DHCP* getDHCP(){ return &_dhcp; }

private:
    DHCP     _dhcp;
    bool     _set[14];      // entsprechend der Anzahl der elemente in s_fenum
};

class LRaw{
public:
    LRaw()
        : _raw(RawPDU("default"))
    { _set = false; }
    void reset();

    const static std::string s_fields[];
    const static unsigned char s_type[];
    enum s_fenum {DATA};

    bool assign(lcommand cmd, const Lang *lang);
    bool compare(const RawPDU *raw);
    bool isField(std::string field, int *which);

    std::string getData(std::string field);

    void setRaw(const RawPDU *raw){ _raw = *raw; }
    void inner_pdu(PDU *pdu){ _raw.inner_pdu(*pdu); }
    RawPDU* getRaw(){ return &_raw; }

private:
    RawPDU  _raw;
    bool    _set;
};


#endif