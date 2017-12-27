#ifndef LR_LREACTION_HPP
#define LR_LREACTION_HPP

#include <iostream>
#include <string>
#include <vector>

#include "lstep.hpp"
#include "lcommand.hpp"
#include "lconst.hpp"

/* LReaction ist wie ein Step nur dass es in einem Step als Subumgebung ist */

class Lang;

class LReaction : public LStep{
    friend class Lang;
public:
    LReaction():LStep(){_packet = nullptr;}

    bool process();

    bool setReaction(std::string reaction){ return setStep(reaction); }
    void setTriggered(bool t){ _triggered = t; }
    void setPDU(PDU *pdu){ _packet = pdu; }

    bool getTriggered() const { return _triggered; }

    void showRCmds(){ 
        for(int i = 0; i < _realcmd.size(); ++i){
            if(_realcmd[i]._cmd == LCOMMAND::ASSIGNMENT)
                std::cout << _realcmd[i]._args[0] << " = " << _realcmd[i]._args[1] << std::endl;
        }
    }
private:
    //std::vector<lcommand>       _cmd;               /* schon durch LStep definiert, speichert in LReaction lediglich die Pseudo-Befehle */
    std::vector<lcommand>       _realcmd;           /* entspricht den expliziten Kommandos, wobei Platzhalter wie PACKET[ARP].SENDER_IP durch die reellen Werte ersetzt werden. */
    bool                        _triggered;
    PDU                        *_packet;            /* enthält das Paket, das den Filter ausgelöst hat */

    bool analyse();
    bool manageAssignment(std::string ass);

    bool isImplicit(int index);                     /* true, wenn eine Zuweisung der Art x=PACKET[PROTOCOL].SENDER_IP */
    bool resolveImplicit(int index);                /* index beschreibt Position des Befehls in _cmd */
    
    std::string getExplicit(std::string field, const EthernetII *eth);
    std::string getExplicit(std::string field, const ARP *arp);
};

#endif