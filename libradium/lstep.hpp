#ifndef LR_LSTEP_HPP
#define LR_LSTEP_HPP

#include <iostream>
#include <string>
#include <ctime>

#include "lconst.hpp"
#include "lcommand.hpp"
#include "lfilehandler.hpp"
#include "ltimer.hpp"

//use ltimer.hpp

/* Speichert Informationen zu STEP-Umgebungen */
class LStep{

    friend class Lang;
    
public:
    LStep();

    bool            setStep(std::string);          /* Der Inhalt der Step-Umgebung*/
    void            setNum(short);
    void            setLastStepNow(){ _timer.reset(); }
    void            setTimed(bool b){ _timed=b; }
    void            setIntervalMS(unsigned int i);

    std::string     getStep()           const { return _step; }
    std::string     getDescription()    const { return _description; }
    short           getNum()            const { return _num; }
    double          getIntervallS()     const { return _interval; }
    double          getIntervallMS()    const { return _interval * 1000; }
    
    bool            isTimed()           const { return _timed; }

    /* gibt an ob schon Zeit für den nächsten Step ist*/
    bool            isTimeForNextStep();

    void showCmds();

protected:
    std::vector<lcommand>       _cmd;
    std::string                 _step;
    std::string                 _description;
    short                       _num;
    bool                        _timed;
    double                      _interval;
    LTimer                      _timer;
    bool                        _firstRep;

    virtual bool analyse();
    virtual bool manageAssignment(std::string ass);
    virtual bool manageSend(std::string arg);
};
#endif