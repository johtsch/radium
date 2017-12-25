#ifndef LR_LSTEP_HPP
#define LR_LSTEP_HPP

#include <iostream>
#include <string>
#include <time.h>

#include "lconst.hpp"
#include "lcommand.hpp"
#include "lfilehandler.hpp"

/* Speichert Informationen zu STEP-Umgebungen */
class LStep{

    friend class Lang;
    
public:
    LStep();

    bool            setStep(std::string);          /* Der Inhalt der Step-Umgebung*/
    void            setNum(short);

    std::string     getStep()           const { return _step; }
    std::string     getDescription()    const { return _description; }
    short           getNum()            const { return _num; }

    void showCmds();

protected:
    std::vector<lcommand>       _cmd;
    std::string                 _step;
    std::string                 _description;
    short                       _num;
    clock_t                     _intervall;

    bool analyse();
    bool manageAssignment(std::string ass);
    bool manageSend(std::string arg);
};
#endif