#include "lang.hpp"

void showARP(const ARP*_arp){
    std::cout << "ARP:" << std::endl;
    std::cout << "sender_hw: " << _arp->sender_hw_addr() << std::endl;
    std::cout << "sender_ip: " << _arp->sender_ip_addr() << std::endl;
    std::cout << "target_hw: " << _arp->target_hw_addr() << std::endl;
    std::cout << "target_ip: " << _arp->target_ip_addr() << std::endl;
}

Lang::Lang()
    : _sniffer(Sniffer(NetworkInterface::default_interface().name()))
{
    SnifferConfiguration conf;
    conf.set_immediate_mode(true);
    conf.set_promisc_mode(true);
    _sniffer = Sniffer(NetworkInterface::default_interface().name(), conf);
    _running = false;
    _trigger = LANG_NOS;
    _status = "Lang was initialized";
    _handler.linkLang(this);
    if(!_quiet)
        std::cout << "Lang: Lang(): " << _status << std::endl;
}

Lang::Lang(std::string fpath)
    : _sniffer(Sniffer(NetworkInterface::default_interface().name()))
{
    SnifferConfiguration conf;
    conf.set_immediate_mode(true);
    conf.set_promisc_mode(true);
    _sniffer = Sniffer(NetworkInterface::default_interface().name(), conf);
    _running = false;
    _trigger = LANG_NOS;
    _handler.linkLang(this);
    if(_handler.loadFile(fpath))
        setStatus("Lang()", "Lang was initialized");
    else
        setStatus("Lang()", "Lang wasn't correctly initialized");
}


bool Lang::loadFile(std::string fpath){
    return _handler.loadFile(fpath);
}

bool Lang::start(){
    return (_running = _handler.init());                 // alle Variablen einlesen
}

bool Lang::update(){
    /* nur die Funktion ausführen, wenn _running true ist. Also wenn alle Vorbereitungen getroffen sind und eine Ausführung gewünscht ist. */
    if(!_running){
        setStatus("update()", "Ausführung wurde nie gestartet <<<");
        return false;
    }

    /* kontrollieren, ob neue Umgebungen eingelesen werden müssen, dabei ist nur step entscheidend, da eine leere Trigger-Umgebung vollkommen legitim ist und lediglich bedeutet, dass
        der TRIGGER sofort erfüllt ist und mit dem nächsten STEP weitergemacht werden kann */
    if(_step.getStep() == LANG_NOS && _trigger == LANG_NOS){
        setStatus("update()", ">>> neue Step-/Trigger-Umgebungen werden eingelesen.");
        if(!_handler.readStep() || !_handler.readTrigger()){
            setStatus("update()", "Konnte nächste STEP-/TRIGGER-Umgebung nicht einlesen. Ausführung beendet <<<");
            _running = false;
            return false;
        }
        else{
            showPacket();
            showFilter();
        }
    }

    /*später noch Timing-Mechanismus einfügen */
    step();

    for(int j = 0; j < _reaction.size(); ++j)
        _reaction[j].setTriggered(false);

    /* Wenn Bedingung erfüllt, dann wird das nächste STEP/TRIGGER-Paar eingelesen, dazu werden die gespeicherten Umgebungen zurückgesetzt */
    if(trigger()){
        _trigger == LANG_NOS;
        _step.setStep(LANG_NOS);
        std::cout << "Angekommen!" << std::endl;
    }

    return true;
}

unsigned char Lang::getVartype(std::string name) const{
    for(int i = 0; i < _dtinfo.size(); ++i){
        if(_dtinfo[i]._name == name)
            return _dtinfo[i]._type;
    }
    return VARTYPE_INVALID;
}

Tins::HWAddress<6> Lang::getHW(std::string name) const{
    for(int i = 0; i < _dtinfo.size(); ++i){
        if(_dtinfo[i]._name == name)
            return _haddr[_dtinfo[i]._index];
    }

    return Tins::HWAddress<6>("00:00:00:00:00:00");
}

Tins::IPv4Address Lang::getIP(std::string name) const{
    for(int i = 0; i < _dtinfo.size(); ++i){
        if(_dtinfo[i]._name == name)
            return _ipaddr[_dtinfo[i]._index];
    }

    return Tins::IPv4Address("0.0.0.0");
}

port Lang::getPort(std::string name) const{
    for(int i = 0; i < _dtinfo.size(); ++i){
        if(_dtinfo[i]._name == name)
            return _port[_dtinfo[i]._index];
    }

    return -1;
}

byte Lang::getByte(std::string name) const{
    for(int i = 0; i < _dtinfo.size(); ++i){
        if(_dtinfo[i]._name == name)
            return _byte[_dtinfo[i]._index];
    }

    return -1;
}

short Lang::getShort(std::string name) const{
    for(int i = 0; i < _dtinfo.size(); ++i){
        if(_dtinfo[i]._name == name)
            return _short[_dtinfo[i]._index];
    }

    return 0;
}
int Lang::getInt(std::string name) const{
    for(int i = 0; i < _dtinfo.size(); ++i){
        if(_dtinfo[i]._name == name)
            return _int[_dtinfo[i]._index];
    }

    return 0;
}

void Lang::showVars(){
    std::cout << "Variablenliste \"" << _handler.getPath() << "\": " << std::endl;
    for(int i = 0; i < _dtinfo.size(); ++i){
        std::cout << ">";
        std::cout << "\tType:\t" << getVarTypeStr(_dtinfo[i]._type) << std::endl;
        std::cout << "\tName:\t" << _dtinfo[i]._name << std::endl;
        std::cout << "\tIndex:\t" << _dtinfo[i]._index << std::endl;
        std::cout << "\tValue:\t";

        switch(_dtinfo[i]._type){
            case VARTYPE_HADDR:
                std::cout << _haddr[_dtinfo[i]._index].to_string() << std::endl;
                break;
            case VARTYPE_IPADDR:
                std::cout << _ipaddr[_dtinfo[i]._index].to_string() << std::endl;
                break;
            case VARTYPE_PORT:
                std::cout << _port[_dtinfo[i]._index] << std::endl;
                break;
            case VARTYPE_BYTE:
                std::cout << (int)_byte[_dtinfo[i]._index] << std::endl;        // Konvertierung nötig, da sonst das char zeichen angezeigt werden würde
                break;
            case VARTYPE_SHORT:
                std::cout << _short[_dtinfo[i]._index] << std::endl;
                break;
            case VARTYPE_INT:
                std::cout << _int[_dtinfo[i]._index] << std::endl;
                break;
            default:
                std::cout << std::endl;
        }
    }
}


void Lang::showStep(){
    std::cout << "Der aktuelle Step (Nummer " << _step.getNum() << "):" << std::endl;
    std::cout << _step.getStep() << std::endl;
    std::cout << "Beschreibung des aktuellen Steps: " << std::endl;
    std::cout << _step.getDescription() << std::endl;
}

void Lang::showTrigger(){
    std::cout << "Der aktuelle Trigger (Nummer " << _triggernum << "):" << std::endl;
    std::cout << _trigger << std::endl;
}

void Lang::showPacket(){ 
    std::cout << ">PAKETLISTE:" << std::endl;
    for(int i = 0; i < _assembler.size(); ++i){
        _assembler[i].showPacket();
    }
}                 
void Lang::showFilter(){ 
    std::cout << ">FILTERLISTE:" << std::endl;
    for(int i = 0; i < _filter.size(); ++i){
        _filter[i].showFilter();
    }
}                       

void Lang::step(){
    for(int i = 0; i < _step._cmd.size();++i){
        if(_step._cmd[i]._cmd == LCOMMAND::ASSIGNMENT)
            assign(_step._cmd[i]);
        else if (_step._cmd[i]._cmd == LCOMMAND::SEND)
            send(_step._cmd[i]);
    }

    /* zusätzlich noch alle getriggerten REACTIONs ausführen */
    for(int i = 0; i < _reaction.size();++i){
        if(_reaction[i].getTriggered()){
            for(int j = 0; j < _reaction[i]._cmd.size();++j){
                    if(_reaction[i]._cmd[j]._cmd == LCOMMAND::ASSIGNMENT)
                        assign(_reaction[i]._cmd[j]);
                    else if (_reaction[i]._cmd[j]._cmd == LCOMMAND::SEND)
                        send(_reaction[i]._cmd[j]);
            }
        }
    }
}

bool Lang::trigger(){

    PDU *pdu;
    bool triggered = false;

    pdu = _sniffer.next_packet();
    
    for(int i = 0; i < _filter.size(); ++i){
        /* handelt es sich um einen PASS Filter kann auch der Trigger ausgelöst werden*/
        if(_filter[i].getType() == LFilter::TYPE::PASS)
            triggered |= _filter[i].compare(pdu);
        else if(_filter[i].getType() == LFilter::TYPE::REACTION){
            if(_filter[i].compare(pdu)){
                int r = _filter[i].getNum();

                for(int j = 0; j < _reaction.size(); ++j){
                    _reaction[j].setTriggered(false);
                    if(r == _reaction[j].getNum())
                        _reaction[j].setTriggered(true);
                }
            }
        }
    }
    
    delete pdu;
    return triggered;
}

/* vorerst werden nur einfache Zuweisungen der Form x=y unterstützt */
bool Lang::assign(lcommand cmd){
    if(cmd._cmd != LCOMMAND::ASSIGNMENT)
        return false;

    long unsigned int index = -1;
    unsigned char type = VARTYPE_INVALID;
    for(int i = 0; i < _dtinfo.size(); ++i){
        if(_dtinfo[i]._name == cmd._args[0]){
            index = _dtinfo[i]._index;
            type = _dtinfo[i]._type;
        }
    }

    if(index == -1 || type == VARTYPE_INVALID)
        return false;

    switch(type){
        case VARTYPE_HADDR:
            _haddr[index] = HWAddress<6>(cmd._args[1]);
            break;
        case VARTYPE_IPADDR:
            _ipaddr[index] = IPv4Address(cmd._args[1]);
            break;
        case VARTYPE_PORT:
            _port[index] = (port)atoi(cmd._args[1]);
            break;
        case VARTYPE_BYTE:
            _byte[index] = (byte)atoi(cmd._args[1]);
            break;
        case VARTYPE_SHORT:
            _short[index] = (short)atoi(cmd._args[1]);
            break;
        case VARTYPE_INT:
            _int[index] = (int)atoi(cmd._args[1]);
            break;
    };
}   

bool Lang::send(lcommand cmd){

    if(cmd._cmd != LCOMMAND::SEND)
        return;

    for(int i = 0; i < _assembler.size(); ++i){
        if(_assembler[i].getNum() == (short)atoi(cmd._args[0].c_str())){
            _assembler[i].send();
            std::cout << "SENDEN!" << std::endl;
        }
    }
}          

bool Lang::varNameNotUsed(std::string name){
    for(int i = 0; i < _dtinfo.size(); ++i){
        if(_dtinfo[i]._name == name)
            return false;                   // Name wird bereits verwendet
    }
    return true;                            // Name wird noch nicht verwendet
}

void Lang::setStatus(std::string fct_name, std::string s){
    _status = s;
    if(!_quiet)
        std::cout << "Lang: " << fct_name << ": " << _status << std::endl;
}
    
