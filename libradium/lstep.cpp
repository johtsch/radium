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
    std::cout << (int)pos1 << " " << LANG_B_DESCRIPTION.length() << " " << (int)pos2 << std::endl;
    if(pos2 != std::string::npos && pos1 != std::string::npos){
        _description = step.substr(pos1 + LANG_B_DESCRIPTION.length(), pos2 - pos1 - LANG_B_DESCRIPTION.length());
        step.erase(pos1, pos2 + LANG_E_DESCRIPTION.length() - pos1 + 1);            // + 1 für das '\n' am Ende von :DESCRIPTION
    }
    _step = step;
}

/*
void LStep::setDescription(std::string des){
    _description = des;
}
*/
void LStep::setNum(short num){
    _num = num;
}
