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
    
    bool isExpl = false;
    int f = 0;
    bool isfield = isField(cmd._args[0], &f);

    if(!isfield)
        return false;
    
    // herausfinden ob es eine Variable oder ein expliziter wert ist
    isExpl = (lang->getVartype(cmd._args[1]) == VARTYPE_INVALID);

    HWAddress<6> addr;

    if(isExpl){
        if(assignVal(&addr, cmd._args[1]) == false)
            return false;
    }
        
    if(f == LEthernet::SRC){
        if(isExpl)
            _eth.src_addr(addr);
        else
            _eth.src_addr(lang->getHW(cmd._args[1]));
    }
    else{
        if(isExpl)
            _eth.dst_addr(addr);
        else
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


bool LEthernet::isField(std::string field, int *which){
    for(int i = 0; i < sizeof(s_fields) / sizeof(s_fields[0]); ++i){
        if(field == s_fields[i]){
            if(which!=nullptr)
                *which=i;
            return true;
        }
    }
    return false;
}

HWAddress<6> LEthernet::getHW(std::string field){
    int f = 0;
    bool isfield = isField(field, &f);

    if(!isfield)
        return HWAddress<6>("00:00:00:00:00:00");
    
    if(f == LEthernet::SRC){
        return _eth.src_addr();
    }
    else{
        return _eth.dst_addr();
    }
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

    bool isExpl = false;
    int f = 0;
    bool isfield = isField(cmd._args[0], &f);

    if(!isfield)
        return false;

    // herausfinden ob es eine Variable oder ein expliziter wert ist
    isExpl = (lang->getVartype(cmd._args[1]) == VARTYPE_INVALID);

    unsigned char t;
    IPv4Address ip;
    HWAddress<6> hw;
    byte b;

    if(isExpl){
        t = (unsigned char)getVarTypeVal(cmd._args[1]);

        if(t==VARTYPE_HADDR)
            if(assignVal(&hw, cmd._args[1]) == false)
                return false;
        if(t==VARTYPE_IPADDR)
            if(assignVal(&ip, cmd._args[1]) == false)
                return false;
        if(t==VARTYPE_BYTE){
            if(assignVal(&b, cmd._args[1]) == false)
                return false;
        }
    }
        
    if(f == LARP::SENDER_IP){
        if(isExpl)
            _arp.sender_ip_addr(ip);
        else
            _arp.sender_ip_addr(lang->getIP(cmd._args[1]));
    }
    else if(f == LARP::SENDER_HW){
        if(isExpl)
            _arp.sender_hw_addr(hw);
        else
            _arp.sender_hw_addr(lang->getHW(cmd._args[1]));
    }
    else if(f == LARP::TARGET_IP){
        if(isExpl)
            _arp.target_ip_addr(ip);
        else
            _arp.target_ip_addr(lang->getIP(cmd._args[1]));
    }
    else if(f == LARP::TARGET_HW){
        if(isExpl)
            _arp.target_hw_addr(hw);
        else
            _arp.target_hw_addr(lang->getHW(cmd._args[1]));
    }
    else if(f == LARP::OPCODE){
        if(isExpl){
            if(b==1){
                _arp.opcode(ARP::REQUEST);
                _flagsset = true;
            }
            if(b==2){
                _arp.opcode(ARP::REPLY);
                _flagsset = true;
            }
        }
        else{
            if(lang->getByte(cmd._args[1]) == 1){
                _arp.opcode(ARP::REQUEST);
                _flagsset = true;
            }
            if(lang->getByte(cmd._args[1]) == 2){
                _arp.opcode(ARP::REPLY);
                _flagsset = true;
            }
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

bool LARP::isField(std::string field, int *which){
    for(int i = 0; i < sizeof(s_fields) / sizeof(s_fields[0]); ++i){
        if(field == s_fields[i]){
            if(which!=nullptr)
                *which=i;
            return true;
        }
    }
    return false;
}

HWAddress<6> LARP::getHW(std::string field){
    int f = 0;
    bool isfield = isField(field, &f);

    if(!isfield)
        return HWAddress<6>("00:00:00:00:00:00");
    if(s_type[f] != VARTYPE_HADDR)
        return HWAddress<6>("00:00:00:00:00:00");
    
    if(f == LARP::SENDER_HW){
        return _arp.sender_hw_addr();
    }
    else if(f == LARP::TARGET_HW){
        return _arp.target_hw_addr();
    }
}

IPv4Address LARP::getIP(std::string field){
    int f = 0;
    bool isfield = isField(field, &f);

    if(!isfield)
        return IPv4Address("0.0.0.0");
    if(s_type[f] != VARTYPE_IPADDR)
        return IPv4Address("0.0.0.0");

    if(f == LARP::SENDER_IP){
        return _arp.sender_ip_addr();
    }
    else if(f == LARP::TARGET_IP){
        return _arp.target_ip_addr();
    }
}

byte LARP::getByte(std::string field){
    int f = 0;
    bool isfield = isField(field, &f);

    if(!isfield)
        return 1;
    if(s_type[f] != VARTYPE_BYTE)
        return 1;

    if(f == LARP::OPCODE){
        return _arp.opcode();
    }
}