#include "lproto.hpp"

const std::string LEthernet::s_fields[] = { "SRC", "DST" };
const unsigned char LEthernet::s_type[] = { VARTYPE_HADDR, VARTYPE_HADDR };

const std::string LARP::s_fields[] = { "SENDER_IP", "SENDER_HW", "TARGET_IP", "TARGET_HW", "OPCODE" };
const unsigned char LARP::s_type[] = { VARTYPE_IPADDR, VARTYPE_HADDR, VARTYPE_IPADDR, VARTYPE_HADDR, VARTYPE_BYTE };

bool isValidProtocol(std::string p){
    if(p == LANG_PRO_ETHERNET)
        return true;
    if(p == LANG_PRO_ARP)
        return true;

    return false;
}

void LEthernet::reset(){
    NetworkInterface iface = NetworkInterface::default_interface();
    _eth.src_addr(iface.hw_address());
    _eth.dst_addr("ff:ff:ff:ff:ff:ff");
}

bool LEthernet::assign(lcommand cmd, const Lang *lang){
    if(cmd._cmd != LCOMMAND::ASSIGNMENT)
        return false;
    
    bool isfield = false;
    int f = 0;
    for(int f = 0; f < sizeof(s_fields) / sizeof(s_fields[0]); ++f){
        if(cmd._args[0] == s_fields[f]){
            isfield = true;
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

void LARP::reset(){
    NetworkInterface iface = NetworkInterface::default_interface();
    _arp.sender_ip_addr("127.0.0.1");
    _arp.sender_hw_addr(iface.hw_address());
    _arp.target_ip_addr("0.0.0.0");
    _arp.opcode(ARP::REQUEST);
}

bool LARP::assign(lcommand cmd, const Lang *lang){
    if(cmd._cmd != LCOMMAND::ASSIGNMENT)
        return false;
    
    bool isfield = false;
    int f = 0;
    for(int f = 0; f < sizeof(s_fields) / sizeof(s_fields[0]); ++f){
        if(cmd._args[0] == s_fields[f]){
            isfield = true;
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
        if(lang->getByte(cmd._args[1]) == 1)
            _arp.opcode(ARP::REQUEST);
        if(lang->getByte(cmd._args[1]) == 2)
            _arp.opcode(ARP::REPLY);
    }

    return true;
}