#include "lproto.hpp"
#include "lang.hpp"

const std::string LEthernet::s_fields[] = { "SRC", "DST" };
const unsigned char LEthernet::s_type[] = { VARTYPE_HADDR, VARTYPE_HADDR };

const std::string LARP::s_fields[] = { "SENDER_IP", "SENDER_HW", "TARGET_IP", "TARGET_HW", "OPCODE" };
const unsigned char LARP::s_type[] = { VARTYPE_IPADDR, VARTYPE_HADDR, VARTYPE_IPADDR, VARTYPE_HADDR, VARTYPE_BYTE };

const std::string LIPv4::s_fields[] = { "TOS", "ID", "FLAGS", "TTL", "SRC_ADDR", "DST_ADDR" };
const unsigned char LIPv4::s_type[] = { VARTYPE_BYTE, VARTYPE_SHORT, VARTYPE_BYTE, VARTYPE_BYTE, VARTYPE_IPADDR, VARTYPE_IPADDR };

const std::string LICMP::s_fields[] = { "CODE", "TYPE", "MTU" };
const unsigned char LICMP::s_type[] = { VARTYPE_BYTE, VARTYPE_BYTE, VARTYPE_SHORT };

const std::string LTCP::s_fields[] = { "DPORT", "SPORT", "SEQ", "SEQ_ACK", "WINDOW", "FLAGS" };
const unsigned char LTCP::s_type[] = { VARTYPE_SHORT, VARTYPE_SHORT, VARTYPE_INT, VARTYPE_INT, VARTYPE_SHORT, VARTYPE_BYTE };

const std::string LUDP::s_fields[] = { "DPORT", "SPORT", "LENGTH" };
const unsigned char LUDP::s_type[] = { VARTYPE_SHORT, VARTYPE_SHORT, VARTYPE_SHORT };

const std::string LDHCP::s_fields[] = { "HTYPE", "HLEN", "HOPS", "XID", "CIADDR", "YIADDR", "SIADDR", "GIADDR", "CHADDR", "TYPE" };
const unsigned char LDHCP::s_type[] = { VARTYPE_BYTE, VARTYPE_BYTE, VARTYPE_BYTE, VARTYPE_INT, VARTYPE_IPADDR, VARTYPE_IPADDR, VARTYPE_IPADDR, VARTYPE_IPADDR, VARTYPE_HADDR, VARTYPE_BYTE };


bool isSupportedProtocol(std::string p){
    if(p == LANG_PRO_ETHERNET)
        return true;
    if(p == LANG_PRO_ARP)
        return true;
    if(p == LANG_PRO_IPv4)
        return true;
    if(p == LANG_PRO_ICMP)
        return true;
    if(p == LANG_PRO_TCP)
        return true;
    if(p == LANG_PRO_UDP)
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

void LIPv4::reset(){
    _ip.tos(0);
    _ip.ttl(0);
    _ip.flags(IP::FLAG_RESERVED);
    _flagsset = false;
    _ip.id(0);
    _ip.src_addr(IPv4Address("0.0.0.0"));
    _ip.dst_addr(IPv4Address("0.0.0.0"));
}

bool LIPv4::assign(lcommand cmd, const Lang *lang){
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
    short s;

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
        if(t==VARTYPE_SHORT){
            if(assignVal(&s, cmd._args[1]) == false)
                return false;
        }
    }
        
    if(f == LIPv4::TOS){
        if(isExpl)
            _ip.tos(b);
        else
            _ip.tos(lang->getByte(cmd._args[1]));
    }
    else if(f == LIPv4::ID){
        if(isExpl)
            _ip.id(s);
        else
            _ip.id(lang->getShort(cmd._args[1]));
    }
    else if(f == LIPv4::FLAGS){
        byte a;
        if(isExpl)
            a = b;
        else
            a =lang->getByte(cmd._args[1]);
        
        switch(a){
            case 0:
                _ip.flags(IP::FLAG_RESERVED);
                break;
            case 1:
                _ip.flags(IP::MORE_FRAGMENTS);
            case 2:
                _ip.flags(IP::DONT_FRAGMENT);
        }
        _flagsset = true;
    }
    else if(f == LIPv4::TTL){
        if(isExpl)
            _ip.ttl(b);
        else
            _ip.ttl(lang->getByte(cmd._args[1]));
    }
    else if(f == LIPv4::SRC_ADDR){
        if(isExpl)
            _ip.src_addr(ip);
        else
            _ip.src_addr(lang->getIP(cmd._args[1]));
    }
    else if(f == LIPv4::DST_ADDR){
        if(isExpl)
            _ip.dst_addr(ip);
        else
            _ip.dst_addr(lang->getIP(cmd._args[1]));
    }

    return true;
}

bool LIPv4::compare(const IP *ip){
    bool equal = true;

    if(_ip.tos() != 0)
        equal &= (_ip.tos() == ip->tos());
    if(_ip.id() != 0)
        equal &= (_ip.id() == ip->id());
    if(_flagsset)
        equal &= (_ip.flags() == ip->flags());
    if(_ip.ttl() != 0)
        equal &= (_ip.ttl() == ip->ttl());
    if(_ip.src_addr() != IPv4Address("0.0.0.0"))
        equal &= (_ip.src_addr() == ip->src_addr());
    if(_ip.dst_addr() != IPv4Address("0.0.0.0"))
        equal &= (_ip.dst_addr() == ip->dst_addr());

    return equal;
}

bool LIPv4::isField(std::string field, int *which){
    for(int i = 0; i < sizeof(s_fields) / sizeof(s_fields[0]); ++i){
        if(field == s_fields[i]){
            if(which!=nullptr)
                *which=i;
            return true;
        }
    }
    return false;
}

IPv4Address LIPv4::getIP(std::string field){
    int f = 0;
    bool isfield = isField(field, &f);

    if(!isfield)
        return IPv4Address("0.0.0.0");
    if(s_type[f] != VARTYPE_IPADDR)
        return IPv4Address("0.0.0.0");

    if(f == LIPv4::SRC_ADDR){
        return _ip.src_addr();
    }
    else if(f == LIPv4::DST_ADDR){
        return _ip.dst_addr();
    }

    return IPv4Address("0.0.0.0");
}

byte LIPv4::getByte(std::string field){
    int f = 0;
    bool isfield = isField(field, &f);

    if(!isfield)
        return 1;
    if(s_type[f] != VARTYPE_BYTE)
        return 1;

    if(f == LIPv4::TOS){
        return _ip.tos();
    }
    else if(f == LIPv4::FLAGS){
        IP::Flags a = _ip.flags();
        if(a==IP::FLAG_RESERVED)
            return 0;
        else if(a==IP::MORE_FRAGMENTS)
            return 1;
        else if(a==IP::DONT_FRAGMENT)
            return 2;
    }
    else if(f == LIPv4::TTL)
        return _ip.ttl();
}

short LIPv4::getShort(std::string field){
    int f = 0;
    bool isfield = isField(field, &f);

    if(!isfield)
        return 0;
    if(s_type[f] != VARTYPE_SHORT)
        return 0;

    if(f == LIPv4::ID){
        return _ip.id();
    }
}

void LICMP::reset(){
    _icmp.code(0);
    _icmp.mtu(0);
    _codeset[0]=false;
    _codeset[1]=false;
    _codeset[2]=false;
}

bool LICMP::assign(lcommand cmd, const Lang *lang){
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
    byte b;
    short s;

    if(isExpl){
        t = (unsigned char)getVarTypeVal(cmd._args[1]);
        if(t==VARTYPE_BYTE){
            if(assignVal(&b, cmd._args[1]) == false)
                return false;
        }
        if(t==VARTYPE_SHORT){
            if(assignVal(&s, cmd._args[1]) == false)
                return false;
        }
    }
        
    if(f == LICMP::CODE){
        if(isExpl)
            _icmp.code(b);
        else
            _icmp.code(lang->getByte(cmd._args[1]));
        _codeset[0] = true;
    }
    else if(f == LICMP::TYPE){
        byte a;
        if(isExpl)
            a = b;
        else
            a = lang->getByte(cmd._args[1]);

        switch(a){
            case 0: _icmp.type(ICMP::ECHO_REPLY); 
            break;
            case 3: _icmp.type(ICMP::DEST_UNREACHABLE); 
            break;
            case 4: _icmp.type(ICMP::SOURCE_QUENCH); 
            break;
            case 5: _icmp.type(ICMP::REDIRECT); 
            break;
            case 8: _icmp.type(ICMP::ECHO_REQUEST); 
            break;
            case 11: _icmp.type(ICMP::TIME_EXCEEDED); 
            break;
            case 12: _icmp.type(ICMP::PARAM_PROBLEM); 
            break;
            case 13: _icmp.type(ICMP::TIMESTAMP_REQUEST); 
            break;
            case 14: _icmp.type(ICMP::TIMESTAMP_REPLY); 
            break;
            case 15: _icmp.type(ICMP::INFO_REQUEST); 
            break;
            case 16: _icmp.type(ICMP::INFO_REPLY); 
            break;
            case 17: _icmp.type(ICMP::ADDRESS_MASK_REQUEST); 
            break;
            case 18: _icmp.type(ICMP::ADDRESS_MASK_REPLY); 
            break;
        };
        _codeset[1] = true;
    }
    else if(f == LICMP::MTU){
        if(isExpl)
            _icmp.mtu(s);
        else
            _icmp.mtu(lang->getShort(cmd._args[1]));
        _codeset[2] = true;
    }

    return true;
}

bool LICMP::compare(const ICMP *icmp){
    bool equal = true;

    if(_codeset[LICMP::CODE])
        equal &= (_icmp.code() == icmp->code());
    if(_codeset[LICMP::TYPE])
        equal &= (_icmp.type() == icmp->type());
    if(_codeset[LICMP::MTU])
        equal &= (_icmp.mtu() == icmp->mtu());

    return equal;
}

bool LICMP::isField(std::string field, int *which){
    for(int i = 0; i < sizeof(s_fields) / sizeof(s_fields[0]); ++i){
        if(field == s_fields[i]){
            if(which!=nullptr)
                *which=i;
            return true;
        }
    }
    return false;
}

byte LICMP::getByte(std::string field){
    int f = 0;
    bool isfield = isField(field, &f);

    if(!isfield)
        return 1;
    if(s_type[f] != VARTYPE_BYTE)
        return 1;

    if(f == LICMP::CODE){
        return _icmp.code();
    }
    else if(f == LICMP::TYPE){
        if(_icmp.type()==ICMP::ECHO_REPLY)
            return 0;
        if(_icmp.type()==ICMP::DEST_UNREACHABLE)
            return 3;
        if(_icmp.type()==ICMP::SOURCE_QUENCH)
            return 4;
        if(_icmp.type()==ICMP::REDIRECT)
            return 5;
        if(_icmp.type()==ICMP::ECHO_REQUEST)
            return 8;
        if(_icmp.type()==ICMP::TIME_EXCEEDED)
            return 11;
        if(_icmp.type()==ICMP::PARAM_PROBLEM)
            return 12;
        if(_icmp.type()==ICMP::TIMESTAMP_REQUEST)
            return 13;
        if(_icmp.type()==ICMP::TIMESTAMP_REPLY)
            return 14;
        if(_icmp.type()==ICMP::INFO_REQUEST)
            return 15;
        if(_icmp.type()==ICMP::INFO_REPLY)
            return 16;
        if(_icmp.type()==ICMP::ADDRESS_MASK_REQUEST)
            return 17;
        if(_icmp.type()==ICMP::ADDRESS_MASK_REPLY)
            return 18;
    }
}

short LICMP::getShort(std::string field){
    int f = 0;
    bool isfield = isField(field, &f);

    if(!isfield)
        return 0;
    if(s_type[f] != VARTYPE_SHORT)
        return 0;

    if(f == LICMP::MTU){
        return _icmp.mtu();
    }
}

void LTCP::reset(){
    _tcp.dport(0);
    _tcp.sport(0);
    _tcp.seq(0);
    _tcp.ack_seq(0);
    _tcp.window(0);
    _tcp.flags(0);                                              // vielleicht Fehlerquelle -> Dokumentation libtins
    _flagsset = false;
}

bool LTCP::assign(lcommand cmd, const Lang *lang){
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
    byte b;
    short s;
    int i;

    if(isExpl){
        t = (unsigned char)getVarTypeVal(cmd._args[1]);
        if(t==VARTYPE_BYTE){
            if(assignVal(&b, cmd._args[1]) == false)
                return false;
        }
        if(t==VARTYPE_SHORT){
            if(assignVal(&s, cmd._args[1]) == false)
                return false;
        }
        if(t==VARTYPE_INT){
            if(assignVal(&i, cmd._args[1]) == false)
                return false;
        }
        
    }
        
    if(f == LTCP::DPORT){
        if(isExpl)
            _tcp.dport(s);
        else
            _tcp.dport(lang->getShort(cmd._args[1]));
    }
    else if(f == LTCP::SPORT){
        if(isExpl)
            _tcp.sport(s);
        else
            _tcp.sport(lang->getShort(cmd._args[1]));
    }
    else if(f == LTCP::SEQ){
        if(isExpl)
            _tcp.seq(i);
        else
            _tcp.seq(lang->getInt(cmd._args[1]));
    }
    else if(f == LTCP::ACK_SEQ){
        if(isExpl)
            _tcp.ack_seq(i);
        else
            _tcp.ack_seq(lang->getInt(cmd._args[1]));
    }
    else if(f == LTCP::WINDOW){
        if(isExpl)
            _tcp.window(s);
        else
            _tcp.window(lang->getShort(cmd._args[1]));
    }
    else if(f == LTCP::FLAGS){
        if(isExpl)
            _tcp.flags(b);
        else
            _tcp.flags(lang->getByte(cmd._args[1]));
        _flagsset = true;
    }

    return true;
}

bool LTCP::compare(const TCP *tcp){
    bool equal = true;

    if(_tcp.dport() != 0)
        equal &= (_tcp.dport() == tcp->dport());
    if(_tcp.sport() != 0)
        equal &= (_tcp.sport() == tcp->sport());
    if(_tcp.seq() != 0)
        equal &= (_tcp.seq() == tcp->seq());
    if(_tcp.ack_seq() != 0)
        equal &= (_tcp.ack_seq() == tcp->ack_seq());
    if(_tcp.window()!=0)
        equal &= (_tcp.window() == tcp->window());
    if(_tcp.flags()!=0)
        equal &= (_tcp.flags() == tcp->flags());

    return equal;
}

bool LTCP::isField(std::string field, int *which){
    for(int i = 0; i < sizeof(s_fields) / sizeof(s_fields[0]); ++i){
        if(field == s_fields[i]){
            if(which!=nullptr)
                *which=i;
            return true;
        }
    }
    return false;
}

byte LTCP::getByte(std::string field){
    int f = 0;
    bool isfield = isField(field, &f);

    if(!isfield)
        return 0;
    if(s_type[f] != VARTYPE_BYTE)
        return 0;

    if(f == LTCP::FLAGS){
        return _tcp.flags();
    }
    
}


short LTCP::getShort(std::string field){
    int f = 0;
    bool isfield = isField(field, &f);

    if(!isfield)
        return 0;
    if(s_type[f] != VARTYPE_SHORT)
        return 0;

    if(f == LTCP::DPORT){
        return _tcp.dport();
    }
    if(f == LTCP::SPORT){
        return _tcp.sport();
    }
    if(f == LTCP::WINDOW){
        return _tcp.window();
    }
}

int LTCP::getInt(std::string field){
    int f = 0;
    bool isfield = isField(field, &f);

    if(!isfield)
        return 0;
    if(s_type[f] != VARTYPE_INT)
        return 0;

    if(f == LTCP::SEQ){
        return _tcp.seq();
    }
    if(f == LTCP::ACK_SEQ){
        return _tcp.ack_seq();
    }
}

void LUDP::reset(){
    _udp.dport(0);
    _udp.sport(0);
    _udp.length(0);
}

bool LUDP::assign(lcommand cmd, const Lang *lang){
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
    short s;

    if(isExpl){
        t = (unsigned char)getVarTypeVal(cmd._args[1]);
        if(t==VARTYPE_SHORT){
            if(assignVal(&s, cmd._args[1]) == false)
                return false;
        }
        
    }
        
    if(f == LUDP::DPORT){
        if(isExpl)
            _udp.dport(s);
        else
            _udp.dport(lang->getShort(cmd._args[1]));
    }
    else if(f == LUDP::SPORT){
        if(isExpl)
            _udp.sport(s);
        else
            _udp.sport(lang->getShort(cmd._args[1]));
    }
    else if(f == LUDP::LENGTH){
        if(isExpl)
            _udp.length(s);
        else
            _udp.length(lang->getShort(cmd._args[1]));
    }

    return true;
}

bool LUDP::compare(const UDP *udp){
    bool equal = true;

    if(_udp.dport() != 0)
        equal &= (_udp.dport() == udp->dport());
    if(_udp.sport() != 0)
        equal &= (_udp.sport() == udp->sport());
    if(_udp.length() != 0)
        equal &= (_udp.length() == udp->length());

    return equal;
}

bool LUDP::isField(std::string field, int *which){
    for(int i = 0; i < sizeof(s_fields) / sizeof(s_fields[0]); ++i){
        if(field == s_fields[i]){
            if(which!=nullptr)
                *which=i;
            return true;
        }
    }
    return false;
}

short LUDP::getShort(std::string field){
    int f = 0;
    bool isfield = isField(field, &f);

    if(!isfield)
        return 0;
    if(s_type[f] != VARTYPE_SHORT)
        return 0;

    if(f == LUDP::DPORT){
        return _udp.dport();
    }
    if(f == LUDP::SPORT){
        return _udp.sport();
    }
    if(f == LUDP::LENGTH){
        return _udp.length();
    }
}

LDHCP::LDHCP(){
    for(int i = 0; i < sizeof(s_type) / sizeof(s_type[0]);++i)
        _set[i] = false;
}

void LDHCP::reset(){
    _dhcp.opcode(0);
    _dhcp.htype(0);
    _dhcp.hlen(0);
    _dhcp.hops(0);
    _dhcp.xid(0);
    _dhcp.ciaddr(IPv4Address("0.0.0.0"));
    _dhcp.yiaddr(IPv4Address("0.0.0.0"));
    _dhcp.siaddr(IPv4Address("0.0.0.0"));
    _dhcp.giaddr(IPv4Address("0.0.0.0"));
    _dhcp.chaddr(HWAddress<6>("00:00:00:00:00:00"));
    _dhcp.type(DHCP::DISCOVER);

    for(int i = 0; i < sizeof(s_type) / sizeof(s_type[0]);++i)
        _set[i] = false;
}

bool LDHCP::assign(lcommand cmd, const Lang *lang){
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
    byte b;
    int i;
    IPv4Address ip;
    HWAddress<6> hw;

    if(isExpl){
        t = (unsigned char)getVarTypeVal(cmd._args[1]);
        if(t==VARTYPE_BYTE){
            if(assignVal(&b, cmd._args[1]) == false)
                return false;
        }
        if(t==VARTYPE_INT){
            if(assignVal(&i, cmd._args[1]) == false)
                return false;
        }
        if(t==VARTYPE_IPADDR){
            if(assignVal(&ip, cmd._args[1]) == false)
                return false;
        }
        if(t==VARTYPE_HADDR){
            if(assignVal(&hw, cmd._args[1]) == false)
                return false;
        }
    }
        
    if(f == LDHCP::OPCODE){
        if(isExpl)
            _dhcp.opcode(b);
        else
            _dhcp.opcode(lang->getByte(cmd._args[1]));
        _set[LDHCP::OPCODE] = true;
    }
    else if(f == LDHCP::HTYPE){
        if(isExpl)
            _dhcp.htype(b);
        else
            _dhcp.htype(lang->getByte(cmd._args[1]));
        _set[LDHCP::HTYPE] = true;
    }
    else if(f == LDHCP::HLEN){
        if(isExpl)
            _dhcp.hlen(b);
        else
            _dhcp.hlen(lang->getByte(cmd._args[1]));
        _set[LDHCP::HLEN] = true;
    }
    else if(f == LDHCP::HOPS){
        if(isExpl)
            _dhcp.hops(b);
        else
            _dhcp.hops(lang->getByte(cmd._args[1]));
        _set[LDHCP::HOPS] = true;
    }
    else if(f == LDHCP::XID){
        if(isExpl)
            _dhcp.xid(i);
        else
            _dhcp.xid(lang->getInt(cmd._args[1]));
        _set[LDHCP::XID] = true;
    }
    else if(f == LDHCP::CIADDR){
        if(isExpl)
            _dhcp.ciaddr(ip);
        else
            _dhcp.ciaddr(lang->getIP(cmd._args[1]));
        _set[LDHCP::CIADDR] = true;
    }
    else if(f == LDHCP::YIADDR){
        if(isExpl)
            _dhcp.yiaddr(ip);
        else
            _dhcp.yiaddr(lang->getIP(cmd._args[1]));
        _set[LDHCP::YIADDR] = true;
    }
    else if(f == LDHCP::SIADDR){
        if(isExpl)
            _dhcp.siaddr(ip);
        else
            _dhcp.siaddr(lang->getIP(cmd._args[1]));
        _set[LDHCP::SIADDR] = true;
    }
    else if(f == LDHCP::GIADDR){
        if(isExpl)
            _dhcp.giaddr(ip);
        else
            _dhcp.giaddr(lang->getIP(cmd._args[1]));
        _set[LDHCP::GIADDR] = true;
    }
    else if(f == LDHCP::CHADDR){
        if(isExpl)
            _dhcp.chaddr(hw);
        else
            _dhcp.chaddr(lang->getHW(cmd._args[1]));
        _set[LDHCP::CHADDR] = true;
    }
    else if(f == LDHCP::TYPE){
        byte a;
        if(isExpl)
            a = b;
        else
            a = lang->getByte(cmd._args[1]);

        switch(a){
            case 1: _dhcp.type(DHCP::DISCOVER);
            break;
            case 2: _dhcp.type(DHCP::OFFER);
            break;
            case 3: _dhcp.type(DHCP::REQUEST);
            break;
            case 4: _dhcp.type(DHCP::DECLINE);
            break;
            case 5: _dhcp.type(DHCP::ACK);
            break;
            case 6: _dhcp.type(DHCP::NAK);
            break;
            case 7: _dhcp.type(DHCP::RELEASE);
            break;
            case 8: _dhcp.type(DHCP::INFORM);
            break;
        };
        _set[LDHCP::TYPE] = true;
    }

    return true;
}

bool LDHCP::compare(const DHCP *dhcp){
    bool equal = true;

    if(_set[OPCODE])
        equal &= (_dhcp.opcode() == dhcp->opcode());
    if(_set[HTYPE])
        equal &= (_dhcp.htype() == dhcp->htype());
    if(_set[HLEN])
        equal &= (_dhcp.hlen() == dhcp->hlen());
    if(_set[HOPS])
        equal &= (_dhcp.hops() == dhcp->hops());
    if(_set[XID])
        equal &= (_dhcp.xid() == dhcp->xid());
    if(_set[CIADDR])
        equal &= (_dhcp.ciaddr() == dhcp->ciaddr());
    if(_set[YIADDR])
        equal &= (_dhcp.yiaddr() == dhcp->yiaddr());
    if(_set[SIADDR])
        equal &= (_dhcp.siaddr() == dhcp->siaddr());
    if(_set[GIADDR])
        equal &= (_dhcp.giaddr() == dhcp->giaddr());
    if(_set[CHADDR])
        equal &= (_dhcp.chaddr() == dhcp->chaddr());
    if(_set[TYPE])
        equal &= (_dhcp.type() == dhcp->type());

    return equal;
}

bool LDHCP::isField(std::string field, int *which){
    for(int i = 0; i < sizeof(s_fields) / sizeof(s_fields[0]); ++i){
        if(field == s_fields[i]){
            if(which!=nullptr)
                *which=i;
            return true;
        }
    }
    return false;
}

byte LDHCP::getByte(std::string field){
    int f = 0;
    bool isfield = isField(field, &f);

    if(!isfield)
        return 1;
    if(s_type[f] != VARTYPE_BYTE)
        return 1;

    if(f == LDHCP::OPCODE){
        return _dhcp.opcode();
    }
    else if(f == LDHCP::HTYPE){
        return _dhcp.htype();
    }
    else if(f == LDHCP::HLEN){
        return _dhcp.hlen();
    }
    else if(f == LDHCP::HOPS){
        return _dhcp.hops();
    }
    else if(f == LDHCP::TYPE){
        return _dhcp.type();
    }
}

int LDHCP::getInt(std::string field){
    int f = 0;
    bool isfield = isField(field, &f);

    if(!isfield)
        return 0;
    if(s_type[f] != VARTYPE_INT)
        return 0;

    if(f == LDHCP::XID){
        return _dhcp.xid();
    }
}

IPv4Address LDHCP::getIP(std::string field){
    int f = 0;
    bool isfield = isField(field, &f);

    if(!isfield)
        return IPv4Address("0.0.0.0");
    if(s_type[f] != VARTYPE_IPADDR)
        return IPv4Address("0.0.0.0");

    if(f == LDHCP::CIADDR){
        return _dhcp.ciaddr();
    }
    else if(f == LDHCP::YIADDR){
        return _dhcp.yiaddr();
    }
    else if(f == LDHCP::SIADDR){
        return _dhcp.siaddr();
    }
    else if(f == LDHCP::GIADDR){
        return _dhcp.giaddr();
    }

    return IPv4Address("0.0.0.0");
}

HWAddress<6> LDHCP::getHW(std::string field){
    int f = 0;
    bool isfield = isField(field, &f);

    if(!isfield)
        return HWAddress<6>("00:00:00:00:00:00");
    if(s_type[f] != VARTYPE_HADDR)
        return HWAddress<6>("00:00:00:00:00:00");
    
    if(f == LDHCP::CHADDR){
        return _dhcp.chaddr();
    }
}