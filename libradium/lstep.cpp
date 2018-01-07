#include "lstep.hpp"

LStep::LStep(){
    _step = LANG_NOS;
    _description = LANG_NOS;
    _num = 0;
    _interval = 0;
    _timed = false;
    _cmd.clear();
}

bool LStep::setStep(std::string step){
    size_t pos1, pos2;

    pos1 = step.find(LANG_B_DESCRIPTION);
    pos2 = step.find(LANG_E_DESCRIPTION, pos1 + 1);
    
    if(pos2 != std::string::npos && pos1 != std::string::npos){
        _description = step.substr(pos1 + LANG_B_DESCRIPTION.length(), pos2 - pos1 - LANG_B_DESCRIPTION.length());
        step.erase(pos1, pos2 + LANG_E_DESCRIPTION.length() - pos1 + 1);            // + 1 für das '\n' am Ende von :DESCRIPTION
    }
    
    _cmd.clear();
    _step = step;
    return analyse();
}

void LStep::setNum(short num){
    _num = num;
}

void LStep::setIntervalMS(unsigned int i){
    _firstRep = true;
    _interval = (double)(i / 1000);
}

bool LStep::isTimeForNextStep() { 
    if(_firstRep){
        _firstRep = false;
        return true;
    }
    if(!_timed)
        return true;
    return (_timer.elapsed()) > (_interval); 
}

void LStep::showCmds(){
    for(int i = 0; i < _cmd.size(); ++i){
        if(_cmd[i]._cmd == LCOMMAND::ASSIGNMENT)
            std::cout << "ASSIGNMENT: " << std::endl;
        
        for(int j =0; j < _cmd[i]._args.size(); ++j){
            std::cout << _cmd[i]._args[j] << ", ";
        }
        std::cout << std::endl;
    }
}

bool LStep::analyse(){
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
            pos1 = pos;
            if(pos1==std::string::npos)
                pos1 = 0;
            
            pos2 = _step.find(';', pos1);
            if(pos2==std::string::npos)
                return false;

            // sollte sich eine implizite Anweisung andeuten dann wird diese extra behandelt. Wichtig, da LReaction auf LStep aufbaut und diese impliziten Zuweisungen dort eine Rolle spielen. Für LStep sind sie hingegen irrelevant
            pos2 = _step.find(LCOMMAND_ARG_PACKET, pos1);
            if(pos2 != std::string::npos && pos2 < _step.find(";", pos1)){
                pos2 = _step.find(';', pos1);
                ass = _step.substr(pos1 + wrd.length(), pos2 - pos1 - wrd.length());
            }
            else{
                while(true){
                    tmp=getNextWord(_step, wc);
                    if(tmp == LANG_CTRL_CHAR)
                        tmp = " ";
                    if(isOperand(tmp[0]))
                        tmp = " " + tmp;                    // da ansonsten das Leerzeichen vor einem Operanden übersprungen werden würde
                    ass+=tmp;
                    if(tmp.find(";", 0) != std::string::npos)
                    break;
                }
            }

            if(manageAssignment(wrd + ass) == false)
                return false;
        }   
        else if(wrd==LCOMMAND_STR_SEND){
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
        else if(wrd==LCOMMAND_STR_PASS){
            _cmd.push_back(lcommand());
            _cmd[_cmd.size()-1]._cmd = LCOMMAND::PASS;
        }
        pos+=wrd.length();                  /* Position im String mitzählen */
        if(wrd.length() == 0)
            pos+=1;
    }

    return true;
}

bool LStep::manageAssignment(std::string ass){
    std::string wrd;
    std::string lastvar = "";
    
    size_t wc = 0;
    _cmd.push_back(lcommand());
    _cmd[_cmd.size()-1]._cmd = LCOMMAND::ASSIGNMENT;
    
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
    wc++;                           /* muss sein sonst wird das Word mit dem '=' am Ende zweimal eingelesen */

    bool lastWasOperand = true;
    ass.pop_back();                 /* Semikolon entfernen */
    while((wrd=getNextWord(ass, wc)).length() > 0){

        if(wrd!=LANG_CTRL_CHAR){


            if(wrd.length() == 1 && isOperand(wrd[0])){
                if(lastWasOperand)              /* folgen zwei Operanden aufeinander ist die Zuweisung fehlerhaft! */
                    return false;
                _cmd[_cmd.size()-1]._args.push_back(wrd);
                lastWasOperand=true;
            }
            if(wrd.length() > 1 && lastWasOperand){

                /* Pfadangaben besonders behandeln */
                if(wrd[wrd.length()-1] == '/'){
                    _cmd[_cmd.size()-1]._args.push_back(ass);
                    lastWasOperand == false;
                    break;
                }

                _cmd[_cmd.size()-1]._args.push_back(wrd);
                lastWasOperand=false;
            }
        }

    }
    
    return true;
}

bool LStep::manageSend(std::string arg){    
    size_t pos1;
    short num;
    std::string numstr;

    pos1 = arg.find(LCOMMAND_ARG_PACKET);

    if(pos1 == std::string::npos)
        return false;

    numstr = getNextArgument(arg, pos1);

    if(!isValidShort(numstr))
        return false;

    num = (short)atoi(numstr.c_str());

    _cmd.push_back(lcommand());
    _cmd[_cmd.size()-1]._cmd = LCOMMAND::SEND;
    _cmd[_cmd.size()-1]._args.push_back(numstr);

    return true;
} 