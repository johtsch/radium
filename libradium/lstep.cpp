#include "lstep.hpp"

LStep::LStep(){
    _step = LANG_NOS;
    _description = LANG_NOS;
    _num = 0;
}

void LStep::setStep(std::string step){
    size_t pos1, pos2;

    pos1 = step.find(LANG_B_DESCRIPTION);
    pos2 = step.find(LANG_E_DESCRIPTION, pos1 + 1);
    
    if(pos2 != std::string::npos && pos1 != std::string::npos){
        _description = step.substr(pos1 + LANG_B_DESCRIPTION.length(), pos2 - pos1 - LANG_B_DESCRIPTION.length());
        step.erase(pos1, pos2 + LANG_E_DESCRIPTION.length() - pos1 + 1);            // + 1 für das '\n' am Ende von :DESCRIPTION
    }
    _step = step;
    analyse();
}
/*
void LStep::setDescription(std::string des){
    _description = des;
}
*/
void LStep::setNum(short num){
    _num = num;
}

bool LStep::analyse(){
    std::string wrd;
    std::string cmd;

    //reset get next Word
    getNextWord("");
    size_t pos=0;
    size_t pos1, pos2; 

    while((wrd=getNextWord(_step)) != "\0"){
        /* eine Zuweisung wurde gefunden */
        if(wrd.find(LANG_C_ASSIGN, 0) != std::string::npos){
            pos1 = pos;
            if(pos1==std::string::npos)
                pos1 = 0;
            else    
                pos1+=1;
            
            pos2 = _step.find(';', pos1);
            if(pos2==std::string::npos)
                pos2 = _step.length()-1;
            if(manageAssignment(_step.substr(pos1, pos2 - pos1)) == false)
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
        }
        pos+=wrd.length();                  /* Position im String mitzählen */
    }

    return true;
}

bool LStep::manageAssignment(std::string ass){
    std::cout << "LStep::manageAssignment(): " << std::endl;
    std::cout << "\t" << ass << std::endl;
    return true;
}

bool LStep::manageSend(std::string arg){    
    std::cout << "LStep::manageSend(): " << std::endl;
    std::cout << "\t" << arg << std::endl;
    return true;
}