#include "lang.hpp"

bool LFilter::setFilter(std::string pfilter, const Lang *lang){
    _filter = pfilter;
    return analyse(lang);
}

bool LFilter::compare(const PDU *pdu){

    PDU *tmp = pdu->clone();
    PDU *del = tmp;
    bool equal = true;
    for(int i = 0; i < sizeof(_layer)/sizeof(_layer[0]); ++i){
        
        if(i > 0)
            tmp = tmp->inner_pdu(); 

        if(tmp == nullptr){
            delete del;
            return equal;
        }

        if(_layer[i] != LANG_PRO_NONE){
            if(tmp->pdu_type() == LangProtocolToPDUType(_layer[i])){

                if(tmp->pdu_type() == PDU::PDUType::ETHERNET_II)
                    equal &= _ethernet.compare((EthernetII*)tmp);
                if(tmp->pdu_type() == PDU::PDUType::ARP)
                    equal &= _arp.compare((ARP*)tmp);
                if(tmp->pdu_type() == PDU::PDUType::IP){
                    equal &= _ip.compare((IP*)tmp);
                }if(tmp->pdu_type() == PDU::PDUType::ICMP)
                    equal &= _icmp.compare((ICMP*)tmp);
                if(tmp->pdu_type() == PDU::PDUType::TCP){
                    equal &= _tcp.compare((TCP*)tmp);
                }if(tmp->pdu_type() == PDU::PDUType::UDP)
                    equal &= _udp.compare((UDP*)tmp);
            }
            else    
                equal = false;
        }
    }

    delete del;
    return equal;
}

bool LFilter::analyse(const Lang *lang){
    
    std::string arg;
    size_t pos1, pos2;

    // die vorher festgelegten Protokollreihenfolge löschen
    for(int i = 0; i < sizeof(_layer) / sizeof(_layer[0]); ++i)
        _layer[i] = LANG_PRO_NONE;

    pos1 = _filter.find(LANG_B_L2);

    if(pos1 != std::string::npos){
        pos2 = _filter.find(LANG_E_L2);
        if(pos2 == std::string::npos)
            return false;
        if(analyseLayer(lang, LANG_PRO_ETHERNET, _filter.substr(pos1 + LANG_B_L2.length(), pos2 - 1 - (pos1 + LANG_B_L2.length())), 2)==false)
            return false;
    }
    pos1 = pos2 = 0;

    pos1 = _filter.find(LANG_B_L3);

    if(pos1 != std::string::npos){
        pos2 = _filter.find(LANG_E_L3);
        if(pos2 == std::string::npos)
            return false;

        arg = getNextArgument(_filter, pos1);
        if(!isSupportedProtocol(arg))
            return false;

        if(analyseLayer(lang, arg, _filter.substr(pos1 + LANG_B_L3.length(), pos2 - 1 - (pos1 + LANG_B_L3.length())), 3)==false)
            return false;
    }

    if(pos2 == std::string::npos)
        pos2 = 0;

    pos1 = _filter.find(LANG_B_L4, pos2);

    if(pos1 != std::string::npos){
        pos2 = _filter.find(LANG_E_L4);
        if(pos2 == std::string::npos)
            return false;

        arg = getNextArgument(_filter, pos1);

        
        if(!isSupportedProtocol(arg))
            return false;

        if(analyseLayer(lang, arg, _filter.substr(pos1 + LANG_B_L4.length(), pos2 - 1 - (pos1 + LANG_B_L4.length())), 4)==false)
            return false;
    }

    if(pos2 == std::string::npos)
        pos2 = 0;

    pos1 = _filter.find(LANG_B_L5, pos2);

    if(pos1 != std::string::npos){
        pos2 = _filter.find(LANG_E_L5);
        if(pos2 == std::string::npos)
            return false;

        arg = getNextArgument(_filter, pos1);

        if(!isSupportedProtocol(arg))
            return false;

        if(analyseLayer(lang, arg, _filter.substr(pos1 + LANG_B_L5.length(), pos2 - 1 - (pos1 + LANG_B_L5.length())), 5)==false)
            return false;
    }

    if(pos2 == std::string::npos)
        pos2 = 0;

    pos1 = _filter.find(LANG_B_L6, pos2);

    if(pos1 != std::string::npos){
        pos2 = _filter.find(LANG_E_L6);
        if(pos2 == std::string::npos)
            return false;

        arg = getNextArgument(_filter, pos1);

        if(!isSupportedProtocol(arg))
            return false;

        if(analyseLayer(lang, arg, _filter.substr(pos1 + LANG_B_L6.length(), pos2 - 1 - (pos1 + LANG_B_L6.length())), 6)==false)
            return false;
    }

    if(pos2 == std::string::npos)
        pos2 = 0;

    pos1 = _filter.find(LANG_B_L7, pos2);

    if(pos1 != std::string::npos){
        pos2 = _filter.find(LANG_E_L7);
        if(pos2 == std::string::npos)
            return false;

        arg = getNextArgument(_filter, pos1);

        if(!isSupportedProtocol(arg))
            return false;

        if(analyseLayer(lang, arg, _filter.substr(pos1 + LANG_B_L7.length(), pos2 - 1 - (pos1 + LANG_B_L7.length())), 7)==false)
            return false;
    }

    return true;
}

bool LFilter::analyseLayer(const Lang *lang, std::string pro, std::string layer, int num){
    PDU *pdu;
    std::string wrd;
    std::string tmp;
    size_t wc = 0;

    size_t pos=0;
    size_t pos1, pos2; 

    if(lang == nullptr)
        return false;

    while((wrd=getNextWord(layer, wc)).length() > 0){
        /* eine Zuweisung wurde gefunden */
        if(wrd.find(LANG_C_ASSIGN, 0) != std::string::npos){
            
            if(wrd.length()==1)                     //Leerzeichen vor dem = sorgt dafür dass wrd nur aus = besteht, was natürlich falsch wäre
                return false;

            while((tmp = getNextWord(layer, wc))==LANG_CTRL_CHAR);
            if(tmp.find(";") == std::string::npos)          // nur Zuweisungen der Art x=y; zulässig, KEINE mehrteiligen der Art x=y+z;
                return false;
            lcommand ass;
            ass._cmd=LCOMMAND::ASSIGNMENT;
            wrd.pop_back();                       /* das letzte Zeichen ist ein '=' */
            tmp.pop_back();                       /* das letzte Zeichen ist ein ';' */
            ass._args.push_back(wrd);
            ass._args.push_back(tmp);

            /* je nach protocol die richtige PDU-Wrapper-Klasse mit Ausführung des Kommandos beauftragen */
            if(pro == LANG_PRO_ETHERNET){
                if(!_ethernet.assign(ass, lang)){
                    return false;
                }
            }
            if(pro == LANG_PRO_ARP){
                if(!_arp.assign(ass, lang)){
                    return false;
                }
                pdu = (PDU*)&_arp;
            }
            if(pro == LANG_PRO_IPv4){
                if(!_ip.assign(ass, lang)){
                    return false;
                }
                pdu = (PDU*)&_ip;
            }
            if(pro == LANG_PRO_ICMP){
                if(!_icmp.assign(ass, lang)){
                    return false;
                }
                pdu = (PDU*)&_icmp;
            }
            if(pro == LANG_PRO_TCP){
                if(!_tcp.assign(ass, lang)){
                    return false;
                }
                pdu = (PDU*)&_tcp;
            }
            if(pro == LANG_PRO_UDP){
                if(!_udp.assign(ass, lang)){
                    return false;
                }
                pdu = (PDU*)&_udp;
            }
            if(pro == LANG_PRO_DHCP){
                if(!_dhcp.assign(ass, lang)){
                    return false;
                }
                pdu = (PDU*)&_dhcp;
            }
            if(pro == LANG_PRO_RAW){
                if(!_raw.assign(ass, lang)){
                    return false;
                }
                pdu = (PDU*)&_raw;
            }
        }   
    }

    if(num < 2 || num > 7)
        return false;

    _layer[num-2] = pro;
    
    return true;
}
