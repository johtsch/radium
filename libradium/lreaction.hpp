#ifndef LR_LREACTION_HPP
#define LR_LREACTION_HPP

#include <iostream>
#include <string>
#include <vector>

#include "lstep.hpp"
#include "lang.hpp"
#include "lcommand.hpp"

/* LReaction ist wie ein Step nur dass es in einem Step als Subumgebung ist */


class LReaction : public LStep{
public:
    LReaction():LStep(){_packet = nullptr;}

    bool execute(Lang *lang);
    bool setReaction(std::string reaction){ return setStep(reaction); }
    void setTriggered(bool t){ _triggered = t; }
    bool getTriggered() const { return _triggered; }
private:
    std::vector<lcommand>       _cmd;               /* schon durch LStep definiert, speichert in LReaction lediglich die Pseudo-Befehle */
    std::vector<lcommand>       _realcmd;           /* entspricht den expliziten Kommandos, wobei Platzhalter wie PACKET[ARP].SENDER_IP durch die reellen Werte ersetzt werden. */
    bool                        _triggered;
    PDU                        *_packet;            /* enthält das Paket, das den Filter ausgelöst hat */
};

#endif