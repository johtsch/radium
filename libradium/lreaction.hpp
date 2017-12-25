#ifndef LR_LREACTION_HPP
#define LR_LREACTION_HPP

#include <iostream>
#include <string>
#include <vector>

#include "lstep.hpp"
#include "lang.hpp"

/* LReaction ist wie ein Step nur dass es in einem Step als Subumgebung ist */


class LReaction : public LStep{
public:
    LReaction():LStep(){}

    bool setReaction(std::string reaction){ return setStep(reaction); }
    void setTriggered(bool t){ _triggered = t; }
    bool getTriggered() const { return _triggered; }
private:
    bool                        _triggered;
};

#endif