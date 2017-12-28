#ifndef LR_LANG_HPP
#define LR_LANG_HPP

#include <iostream>
#include <string>           /* std::string etc */
#include <fstream>          /* Dateioperationen */
#include <vector>           /* std::vector<T> */
#include <tins/tins.h>      /* libtins für Netzwerkfunktionen etc. */

#include "vartypes.hpp"         /* Datentypdefinitionen */
#include "lconst.hpp"           /* Konstanten die für .lang-Dateien und deren Auswertung wichtig sind */
#include "lassembler.hpp"
#include "lfilter.hpp"
#include "lfilehandler.hpp"
#include "lstep.hpp"
#include "lreaction.hpp"

void showARP(const ARP*_arp);

class Lang{

    friend class LFileHandler;                    /* Die LFileHandler-Klasse braucht Zugriff auf die Variablen der Lang-Klasse um diese zu initialisieren */

public:
    Lang();
    Lang(std::string fpath);
    
    bool loadFile(std::string fpath);
    bool start();                           /* Starten des Angriffs */
    void pause();                           /* Angriffausführung pausieren */
    void stop();                            /* Angriff stoppen, beim nächsten Start beginnt der Angriff wieder von vorne */

    bool update();                          /* Die Angriffloop... managed Steps und Trigger etc., gibt false zurück wenn keine neue Step/Trigger-Umgebung eingelesen werden konnte */

    bool assign(lcommand cmd);              /* führt ein assign Kommando aus */
    bool send(lcommand cmd);                /* führt ein versenden Kommando aus */

    /* Setter*/
    void setQuiet(bool q){ _quiet = q; }

    /* Getter */
    std::string             getStatus() const { return _status; };
    unsigned char           getVartype(std::string name) const;
    Tins::HWAddress<6>      getHW(std::string name) const;
    Tins::IPv4Address       getIP(std::string name) const;
    port                    getPort(std::string name) const;
    byte                    getByte(std::string name) const;
    short                   getShort(std::string name) const;
    int                     getInt(std::string name) const;

    /* Analyzer / Shower */
    void showVars();
    void showStep();
    void showTrigger();
    void showCmds(){ _step.showCmds();}
    void showPacket();               
    void showFilter();
private:
    Sniffer                             _sniffer;           /* um reinkommende Pakete auszulesen */
    LFileHandler                        _handler;           /* kümmert sich um das Auslesen der Datei */
    
    bool                                _quiet;             /* Statusmeldungen werden nicht angezeigt (ist Standard) */
    bool                                _running;           /* dient unter Anderem dazu festzustellen ob die Datei überhaupt ausführbar ist. Sollte im init-Teil etwas schief gehen 
                                                               wird _running nie true */
    bool                                _forward;           /* gibt an ob ein Filter zugeschlagen hat (also ob eine neue Step Trigger kombi eingelesen werden soll */
    std::string                         _status;            /* speichert Statusmeldungen */
    std::vector<struct varinfo>         _dtinfo;            /* datatype info, dient dem Nachschlagen, wo eine Variable der .lang-Datei zu finden ist */
    std::vector<Tins::HWAddress<6>>     _haddr;             /* speichert alle in der .lang-Datei verwendeten Variablen des Typs Hardwareaddresse */
    std::vector<Tins::IPv4Address>      _ipaddr;            /* speichert alle in der .lang-Datei verwendeten Variablen des Typs IPv4-ddresse */
    std::vector<port>                   _port;              /* speichert alle in der .lang-Datei verwendeten Variablen des Typs Port */
    std::vector<byte>                   _byte;              /* speichert alle in der .lang-Datei verwendeten Variablen des Typs Byte */
    std::vector<short>                  _short;             /* speichert alle in der .lang-Datei verwendeten Variablen des Typs Short */
    std::vector<int>                    _int;               /* speichert alle in der .lang-Datei verwendeten Variablen des Typs Integer */

    //std::vector<unsigned char[1200]>    _packet;            /* die von assemble() erstellte pakete */
    
    unsigned int                        _intervall;         /* die Pausenzeit in ms nach einem Step vor seiner erneuten Ausführung */

    // temporäre Speicher   
    LStep                               _step;              /* speichert den Inhalt der aktuellen S[n]-Umgebung der .lang-Datei in optimierter Form */
    std::vector<LAssembler>             _assembler;         /* speichert alle Assembler Umgebungen des aktuellen Steps */
    std::vector<LReaction>              _reaction;          /* speichert alle Reaction Umgebungen des aktuellen Steps */
    std::vector<LFilter>                _filter;            /* speichert alle Filter Umgebungen des aktuellen Triggers */  
    std::string                         _trigger;           /* speichert den Inhalt der aktuellen T[n]-Umgebung der .lang-Datei */
    short                               _triggernum;


    //Hilfsfunktionen
    void step();                            /* führt den in _step gespeicherten Schritt aus, wenn es wieder an der Zeit ist (->_intervall) */
    bool trigger();                         /* kontrolliert die angekommenen Datenpakete, ob sie den Vorgaben in _trigger entsprechen */

    bool varNameNotUsed(std::string name);
    void setStatus(std::string fct_name, std::string s);                        /* steht nicht bei den Settern, da nur die Klasse Statusmeldungen produzieren darf. fct_name ist der Name
                                                                                   der Funktion in der Statusmeldung verursacht wurde */
};

#endif
