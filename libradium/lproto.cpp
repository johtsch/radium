#include "lproto.hpp"
#include "lang.hpp"

const std::string LEthernet::s_fields[] = { "SRC", "DST" };
const unsigned char LEthernet::s_type[] = { VARTYPE_HADDR, VARTYPE_HADDR };

const std::string LARP::s_fields[] = { "SENDER_IP", "SENDER_HW", "TARGET_IP", "TARGET_HW", "OPCODE" };
const unsigned char LARP::s_type[] = { VARTYPE_IPADDR, VARTYPE_HADDR, VARTYPE_IPADDR, VARTYPE_HADDR, VARTYPE_BYTE };

bool isSupportedProtocol(std::string p){
    if(p == LANG_PRO_ETHERNET)
        return true;
    if(p == LANG_PRO_ARP)
        return true;

    return false;
}

PDU::PDUType LangProtocolToPDUType(std::string pro){
    if(pro == LANG_PRO_ETHERNET)
        return PDU::PDUType::ETHERNET_II;
    if(pro == LANG_PRO_ARP)
        return PDU::PDUType::ARP;
    if(pro == LANG_PRO_IPv4)
        return PDU::PDUType::IP;
    if(pro == LANG_PRO_ICMP)
        return PDU::PDUType::ICMP;
    if(pro == LANG_PRO_TCP)
        return PDU::PDUType::TCP;
    if(pro == LANG_PRO_UDP)
        return PDU::PDUType::UDP;
    if(pro == LANG_PRO_DHCP)
        return PDU::PDUType::DHCP;

    return PDU::PDUType::UNKNOWN;
}

void LEthernet::reset(){
    _eth.src_addr("00:00:00:00:00:00");
    _eth.dst_addr("00:00:00:00:00:00");
}

bool LEthernet::assign(lcommand cmd, const Lang *lang){
    if(cmd._cmd != LCOMMAND::ASSIGNMENT)
        return false;
    
    bool isfield = false;
    int f = 0;
    for(f = 0; f < sizeof(s_fields) / sizeof(s_fields[0]); ++f){
        if(cmd._args[0] == s_fields[f]){
            isfield = true;
            break;
        }
    }

    if(!isfield)
        return false;
    
    if(lang->getVartype(cmd._args[1]) != s_type[f])
        return false;

        
    if(f == LEthernet::SRC){
        _eth.src_addr(lang->getHW(cmd._args[1]));
    }
    else{
        _eth.dst_addr(lang->getHW(cmd._args[1]));
    }

    return true;
}

bool LEthernet::compare(const EthernetII *eth){
    bool equal = true;
    if(_eth.src_addr() != HWAddress<6>("00:00:00:00:00:00"))
        equal &= (_eth.src_addr() == eth->src_addr());
    if(_eth.dst_addr() != HWAddress<6>("00:00:00:00:00:00"))
        equal &= (_eth.dst_addr() == eth->dst_addr());

    return equal;
}

void LARP::reset(){
    _flagsset = false;
    _arp.sender_ip_addr("0.0.0.0");
    _arp.sender_hw_addr("00:00:00:00:00:00");
    _arp.target_ip_addr("0.0.0.0");
    _arp.opcode(ARP::REQUEST);
}

bool LARP::assign(lcommand cmd, const Lang *lang){
    if(cmd._cmd != LCOMMAND::ASSIGNMENT)
        return false;
    
    bool isfield = false;
    int f = 0;
    for(f = 0; f < sizeof(s_fields) / sizeof(s_fields[0]); ++f){
        if(cmd._args[0] == s_fields[f]){
            isfield = true;
            break;
        }
    }

    if(!isfield)
        return false;

    if(lang->getVartype(cmd._args[1]) != s_type[f])
        return false;
        
    if(f == LARP::SENDER_IP){
        _arp.sender_ip_addr(lang->getIP(cmd._args[1]));
    }
    else if(f == LARP::SENDER_HW){
        _arp.sender_hw_addr(lang->getHW(cmd._args[1]));
    }
    else if(f == LARP::TARGET_IP){
        _arp.target_ip_addr(lang->getIP(cmd._args[1]));
    }
    else if(f == LARP::TARGET_HW){
        _arp.target_hw_addr(lang->getHW(cmd._args[1]));
    }
    else if(f == LARP::OPCODE){
        if(lang->getByte(cmd._args[1]) == 1){
            _arp.opcode(ARP::REQUEST);
            _flagsset = true;
        }
        if(lang->getByte(cmd._args[1]) == 2){
            _arp.opcode(ARP::REPLY);
            _flagsset = true;
        }
    }

    return true;
}

bool LARP::compare(const ARP *arp){
    bool equal = true;

    if(_arp.sender_ip_addr() != IPv4Address("0.0.0.0"))
        equal &= (_arp.sender_ip_addr() == arp->sender_ip_addr());
    if(_arp.sender_hw_addr() != HWAddress<6>("00:00:00:00:00:00"))
        equal &= (_arp.sender_hw_addr() == arp->sender_hw_addr());
    if(_arp.target_ip_addr() != IPv4Address("0.0.0.0"))
        equal &= (_arp.target_ip_addr() == arp->target_ip_addr());
    if(_arp.target_hw_addr() != HWAddress<6>("00:00:00:00:00:00"))
        equal &= (_arp.target_hw_addr() == arp->target_hw_addr());
    if(_flagsset)
        equal &= (_arp.opcode() == arp->opcode());

    return equal;
}