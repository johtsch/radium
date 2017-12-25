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

bool LAssembler::send(){
    NetworkInterface iface = NetworkInterface::default_interface();
    PacketSender sender;

    sender.send(*((PDU*)_eth.getEth()), iface);
    return true;
}

bool LAssembler::analyse(const Lang *lang){
    std::string arg;
    size_t pos1, pos2;

    // die statische Variable in analyse Layer zurücksetzen
    analyseLayer(lang, "", "");

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
    else
        return false;

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
    else
        return true;

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
    else
        return true;

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
    else
        return true;

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
    else
        return true;

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
            
            if(wrd.length()==1)                     //Leerzeichen vor dem = sorgt dafür dass wrd nur aus = besteht, was natürlich falsch wäre
                return false;

            //Steuer- und Leerzeichen überspringen
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
        }   
    }

    if(prevpro=="" && pro != LANG_PRO_ETHERNET){
        _eth.inner_pdu(pdu);
    }
    if(prevpro==LANG_PRO_ETHERNET){
        _eth.inner_pdu(pdu);
    }
    /*
    if(prevpro==LANG_PRO_ETHERNET)
        _eth.inner_pdu(pdu);
    if(prevpro==LANG_PRO_ETHERNET)
        _eth.inner_pdu(pdu);
    if(prevpro==LANG_PRO_ETHERNET)
        _eth.inner_pdu(pdu);
    */

    prevpro = pro;
    return true;
}