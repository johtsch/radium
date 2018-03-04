#include "lassembler.hpp"
#include "lang.hpp"

void LAssembler::showPacket(){
    std::cout << "PAKET " << _num << std::endl;
    std::cout << "ETHERNET:" << std::endl;
    std::cout << "src: " << _eth.getEth()->src_addr() << std::endl;
    std::cout << "dst: " << _eth.getEth()->dst_addr() << std::endl;
    std::cout << "ARP:" << std::endl;
    std::cout << "Type: ";
    if(_arp.getARP()->opcode()==ARP::REPLY)
        std::cout << "REPLY" << std::endl;
    else if(_arp.getARP()->opcode()==ARP::REQUEST)
        std::cout << "REQUEST" << std::endl;
    std::cout << "sender_hw: " << _arp.getARP()->sender_hw_addr() << std::endl;
    std::cout << "sender_ip: " << _arp.getARP()->sender_ip_addr() << std::endl;
    std::cout << "target_hw: " << _arp.getARP()->target_hw_addr() << std::endl;
    std::cout << "target_ip: " << _arp.getARP()->target_ip_addr() << std::endl;
}

bool LAssembler::setAssembler(std::string assemble, const Lang *lang){
    _assemble = assemble;
    return analyse(lang);
}

bool LAssembler::send(PacketSender *sender, NetworkInterface *iface){
    sender->send(*((PDU*)_eth.getEth()), *iface);
    return true;
}

void LAssembler::reset(){
    _eth.reset();
    _arp.reset();
    _ip.reset();
    _icmp.reset();
    _tcp.reset();
    _udp.reset();
    _dhcp.reset();
}

bool LAssembler::analyse(const Lang *lang){
    std::string arg;
    size_t pos1, pos2;

    // die statische Variable in analyse Layer zurücksetzen
    analyseLayer(lang, "", "");
    // alle protokolle zurücksetzen
    reset();

    pos1 = _assemble.find(LANG_B_L2);

    if(pos1 != std::string::npos){
        pos2 = _assemble.find(LANG_E_L2);
        if(pos2 == std::string::npos)
            return false;
        if(analyseLayer(lang, LANG_PRO_ETHERNET, _assemble.substr(pos1 + LANG_B_L2.length(), pos2 - 1 - (pos1 + LANG_B_L2.length())))==false)
            return false;
    }
    pos1 = pos2 = 0;

    pos1 = _assemble.find(LANG_B_L3);

    if(pos1 != std::string::npos){
        pos2 = _assemble.find(LANG_E_L3);
        if(pos2 == std::string::npos)
            return false;

        arg = getNextArgument(_assemble.substr(pos1, pos2 - pos1));
        if(!isSupportedProtocol(arg))
            return false;

        if(analyseLayer(lang, arg, _assemble.substr(pos1 + LANG_B_L3.length(), pos2 - 1 - (pos1 + LANG_B_L3.length())))==false)
            return false;
    }

    pos1 = _assemble.find(LANG_B_L4, pos2);

    if(pos1 != std::string::npos){
        pos2 = _assemble.find(LANG_E_L4);
        if(pos2 == std::string::npos)
            return false;

        arg = getNextArgument(_assemble.substr(pos1, pos2 - pos1));

        if(!isSupportedProtocol(arg))
            return false;

        if(analyseLayer(lang, arg, _assemble.substr(pos1 + LANG_B_L4.length(), pos2 - 1 - (pos1 + LANG_B_L4.length())))==false)
            return false;
    }

    pos1 = _assemble.find(LANG_B_L5, pos2);

    if(pos1 != std::string::npos){
        pos2 = _assemble.find(LANG_E_L5);
        if(pos2 == std::string::npos)
            return false;

        arg = getNextArgument(_assemble.substr(pos1, pos2 - pos1));

        if(!isSupportedProtocol(arg))
            return false;

        if(analyseLayer(lang, arg, _assemble.substr(pos1 + LANG_B_L5.length(), pos2 - 1 - (pos1 + LANG_B_L5.length())))==false)
            return false;
    }

    pos1 = _assemble.find(LANG_B_L6, pos2);

    if(pos1 != std::string::npos){
        pos2 = _assemble.find(LANG_E_L6);
        if(pos2 == std::string::npos)
            return false;

        arg = getNextArgument(_assemble.substr(pos1, pos2 - pos1));

        if(!isSupportedProtocol(arg))
            return false;

        if(analyseLayer(lang, arg, _assemble.substr(pos1 + LANG_B_L6.length(), pos2 - 1 - (pos1 + LANG_B_L6.length())))==false)
            return false;
    }

    pos1 = _assemble.find(LANG_B_L7, pos2);

    if(pos1 != std::string::npos){
        pos2 = _assemble.find(LANG_E_L7);
        if(pos2 == std::string::npos)
            return false;

        arg = getNextArgument(_assemble.substr(pos1, pos2 - pos1));

        if(!isSupportedProtocol(arg))
            return false;

        if(analyseLayer(lang, arg, _assemble.substr(pos1 + LANG_B_L7.length(), pos2 - 1 - (pos1 + LANG_B_L7.length())))==false)
            return false;
    }

    return true;
}

bool LAssembler::analyseLayer(const Lang *lang, std::string pro, std::string layer){
    static std::string prevpro="";
    PDU *pdu;
    std::string wrd;
    std::string tmp;
    size_t wc = 0;

    size_t pos=0;
    size_t pos1, pos2; 

    if(pro == ""){
        prevpro = "";
        return true;
    }

    if(lang == nullptr)
        return false;


    while((wrd=getNextWord(layer, wc)).length() > 0){
        /* eine Zuweisung wurde gefunden */
        if(wrd.find(LANG_C_ASSIGN, 0) != std::string::npos){

            if(wrd.length() == 1){
                size_t wct = wc;
                std::string tmp = "";
                /* das erste Wort vor dem = finden, das kein Leerzeichen war */
                for(int i = 2; i < 10; ++i){
                    wct = wc - i;

                    if((int)wct < 0){     // wenn Gleichheitszeichen in Zeile das erste Wort war schlägt analyse fehl
                        return false;
                        }

                    tmp = getNextWord(layer, wct);

                    if(tmp != LANG_CTRL_CHAR){
                        wrd = tmp + wrd;        // wrd ist zu diesem Zeitpunkt ja nur "="
                        break;  
                    }                        
                }
                if(tmp == LANG_CTRL_CHAR) {      // wenn mehr als zehn Steuerzeichen da waren auch fehler angeben
                    return false;   }
            }

            //Steuer- und Leerzeichen überspringen
            while((tmp = getNextWord(layer, wc))==LANG_CTRL_CHAR);

            if(tmp.find(";") == std::string::npos)           // nur Zuweisungen der Art x=y; zulässig, KEINE mehrteiligen der Art x=y+z;
                return false;
            lcommand ass;
            ass._cmd=LCOMMAND::ASSIGNMENT;
            wrd.pop_back();                       /* das letzte Zeichen ist ein '=' */
            tmp.pop_back();                       /* das letzte Zeichen ist ein ';' */
            ass._args.push_back(wrd);
            ass._args.push_back(tmp);

            /* je nach protocol die richtige PDU-Wrapper-Klasse mit Ausführung des Kommandos beauftragen */
            if(pro == LANG_PRO_ETHERNET){
                if(!_eth.assign(ass, lang)){
                    return false;
                }
            } 
            else if(pro == LANG_PRO_ARP){
                if(!_arp.assign(ass, lang)){
                    return false;
                }
                pdu = (PDU*)&_arp;
            }
            else if(pro == LANG_PRO_IPv4){
                if(!_ip.assign(ass, lang)){
                    return false;
                }
                pdu = (PDU*)&_ip;
            }
            else if(pro == LANG_PRO_ICMP){
                if(!_icmp.assign(ass, lang)){
                    return false;
                }
                pdu = (PDU*)&_icmp;
            }
            else if(pro == LANG_PRO_TCP){
                if(!_tcp.assign(ass, lang)){
                    return false;
                }
                pdu = (PDU*)&_tcp;
            }
            else if(pro == LANG_PRO_UDP){
                if(!_udp.assign(ass, lang)){
                    return false;
                }
                pdu = (PDU*)&_udp;
            }
            else if(pro == LANG_PRO_DHCP){
                if(!_dhcp.assign(ass, lang)){
                    return false;
                }
                pdu = (PDU*)&_dhcp;
            }
            else if(pro == LANG_PRO_RAW){
                if(!_raw.assign(ass, lang)){
                    return false;
                }
                pdu = (PDU*)&_raw;
            }
        }   
    }

    if(pro==LANG_PRO_DHCP)
        _dhcp.end();

    if(prevpro=="" && pro != LANG_PRO_ETHERNET){
        _eth.inner_pdu(pdu);
    }
    if(prevpro==LANG_PRO_ETHERNET){
        _eth.inner_pdu(pdu);
    }
    if(prevpro==LANG_PRO_IPv4)
        _ip.inner_pdu(pdu);
    if(prevpro==LANG_PRO_ICMP)
        _icmp.inner_pdu(pdu);
    if(prevpro==LANG_PRO_ARP)
        _arp.inner_pdu(pdu);
    if(prevpro==LANG_PRO_TCP)
        _tcp.inner_pdu(pdu);
    if(prevpro==LANG_PRO_UDP)
        _udp.inner_pdu(pdu);
    if(prevpro==LANG_PRO_DHCP)
        _dhcp.inner_pdu(pdu);
    if(prevpro==LANG_PRO_RAW)
        _raw.inner_pdu(pdu);
        
    prevpro = pro;
    return true;
}