#include "lreaction.hpp"
#include "lang.hpp"

bool LReaction::process(){
    _realcmd = _cmd;
    
    std::cout << "DIE COMMANDS WERDEN BEHANDELT " << _cmd.size() << " > " <<  _realcmd.size() << std::endl;
    
    for(int i = 0; i < _realcmd.size();++i){
        std::cout << "HEY" << std::endl;

        if(_realcmd[i]._cmd == LCOMMAND::ASSIGNMENT){
            /* herausfinden ob ein ASSIGNMENT implizit oder explizit ist */
            if(isImplicit(i)){
                std::cout << "RESOLVE " << _realcmd[i]._args[0] << " = " << _realcmd[i]._args[1] << std::endl;
                if(!resolveImplicit(i)){
                    std::cout << "FAILED!";
                    return false;
                }
            }
        }
        else if (_realcmd[i]._cmd == LCOMMAND::SEND){
            std::cout << "ATTACKE" << std::endl;
        }
    }

    return true;
}

bool LReaction::analyse(){
    std::string wrd;
    std::string tmp;
    std::string ass;
    size_t wc = 0;

    size_t pos=0;
    size_t pos1, pos2; 

    while((wrd=getNextWord(_step, wc)).length() > 0){
        pos1 = pos2 = 0;
        tmp = ass = "";
        /* eine Zuweisung wurde gefunden */
        if(wrd.find(LANG_C_ASSIGN, 0) != std::string::npos){
            std::cout << "DAS WROT: " << wrd << std::endl;
            pos1 = pos;
            if(pos1==std::string::npos)
                pos1 = 0;
                
            pos2 = _step.find(';', pos1);
            if(pos2==std::string::npos)
                return false;

            // sollte sich eine implizite Anweisung andeuten dann wird diese extra behandelt. Wichtig, da LReaction auf LStep aufbaut und diese impliziten Zuweisungen dort eine Rolle spielen. Für LStep sind sie hingegen irrelevant
            pos2 = _step.find(LCOMMAND_ARG_PACKET, pos1);
            if(pos2 != std::string::npos){
                pos1 = pos2;
                pos2 = _step.find(';', pos);
                ass = _step.substr(pos1, pos2 - pos1);
            }

            if(manageAssignment(wrd + ass) == false)
                return false;
        }   
        if(wrd==LCOMMAND_STR_SEND){
            pos1=_step.find(')', pos);
            pos2=_step.find("\n", pos+wrd.length());
            if(pos1 > pos2)
                return false;
            pos2 = pos1;
            pos1 = _step.find("(", pos) + 1;
            if(manageSend(_step.substr( pos1, pos2 - pos1)) == false)
                return false;

            wc++;   //sonst würde zweimal manageSend aufgerufen werden ;)
        }
        pos+=wrd.length();                  /* Position im String mitzählen */
        if(wrd.length() == 0)
            pos+=1;
    }

    return true;
}

bool LReaction::manageAssignment(std::string ass){
    std::string wrd;
    std::string lastvar = "";
    size_t wc = 0;
    _cmd.push_back(lcommand());
    _cmd[_cmd.size()-1]._cmd = LCOMMAND::ASSIGNMENT;
    std::cout << "DAS ASSIGNMENT: " << ass << std::endl;
    /* Einlesen des Linken Teils der Zuweisung */
    while((wrd=getNextWord(ass, wc)).length() > 0){
        if(wrd.find("=") != std::string::npos){
            if(wrd == "=")
                _cmd[_cmd.size()-1]._args.push_back(lastvar);
            else{    
                _cmd[_cmd.size()-1]._args.push_back(wrd.substr(0, wrd.length()-1));
            }
            
            break;
        }
        else if(wrd.length()>0 && wrd != LANG_CTRL_CHAR)
            lastvar = wrd;
    }
    wc++;
    
    ass.pop_back();                 /* Semikolon entfernen */

    size_t pos1;
    pos1 = ass.find("=");
    _cmd[_cmd.size()-1]._args.push_back(ass.substr(pos1 + 1, ass.length() - 1 - pos1));
    
    return true;
}

bool LReaction::isImplicit(int index){
    if(_realcmd[index]._args[1].find(LANG_PACKET) != std::string::npos)
        return true;
    else
        return false;
}

bool LReaction::resolveImplicit(int index){
    size_t pos1, pos2;

    if((pos1=_realcmd[index]._args[1].find(LANG_PACKET)) == std::string::npos){
        std::cout << "1" << std::endl;
        return false;
    }

    std::string arg = "";
    std::string field = "";
    std::string tmp = "";
    arg = getNextArgument(_realcmd[index]._args[1], pos1);
    

    if((pos1=_realcmd[index]._args[1].find("].")) != std::string::npos){
        field = _realcmd[index]._args[1].substr(pos1+2, _realcmd[index]._args[1].length() - pos1 - 2);
        std::cout << "FIELD = " << field << "|" << std::endl;
    } 
    else{
        std::cout << "2: " << _realcmd[index]._args[1] << std::endl;
        return false;
    }

    if(arg==LANG_PRO_ETHERNET){
        EthernetII *eth;
        try{ 
            eth = &_packet->rfind_pdu<EthernetII>();
        }
        catch(pdu_not_found e){
            std::cout << "3" << std::endl;
            return false;
        }
        tmp = getExplicit(field, eth);
        if(tmp == LANG_NOS){
            std::cout << "4" << std::endl;
            return false;
        }
        _realcmd[index]._args[1] = tmp;
    }
    else if(arg==LANG_PRO_ARP){
        ARP *arp;
        try{ 
            arp = &_packet->rfind_pdu<ARP>();
        }
        catch(pdu_not_found e){
            std::cout << "5 " << e.what() << std::endl;
            return false;
        }
 
        tmp = getExplicit(field, arp);
        if(tmp == LANG_NOS){
            std::cout << "6" << std::endl;
            return false;
        }
        _realcmd[index]._args[1] = tmp;
    }
    else{
        std::cout << "7" << std::endl;
        return false;
    }

    return true;
}

std::string LReaction::getExplicit(std::string field, const EthernetII *eth){
    int f = -1;
    LEthernet p;
    p.setEth(eth);

    if(!p.isField(field, &f))
        return "";

    HWAddress<6> hw = p.getHW(field);

    return hw.to_string();
}

std::string LReaction::getExplicit(std::string field, const ARP *arp){
    int f = -1;
    LARP p;
    p.setARP(arp);
    std::string str;

    if(!p.isField(field, &f))
        return "";

    switch(LARP::s_type[f]){
        case VARTYPE_HADDR:
        {
            HWAddress<6> hw = p.getHW(field);
            str = hw.to_string();
        break;
        }
        case VARTYPE_IPADDR:
        {
            IPv4Address ip = p.getIP(field);
            str = ip.to_string();
        break;
        }
        case VARTYPE_BYTE:
        {
            byte b = p.getByte(field);
            str = std::to_string((int)b);
        break;
        }
        default:
        {
            std::cout << "DEFALTDEFAULT" << std::endl;
        }
    };
    std::cout << "DAS IST DER GETEXPLICIT STRIN: " << str << std::endl;
    return str;
}