#ifndef LR_LANG_HPP
#define LR_LANG_HPP

#include <iostream>
#include <string>           /* std::string etc */
#include <fstream>          /* Dateioperationen */
#include <vector>           /* std::vector<T> */
#include <tins/tins.h>      /* libtins für Netzwerkfunktionen etc. */

#include "vartypes.hpp"       /* Datentypdefinitionen */
#include "header.hpp"         /* Protokollheaderformate etc. */

const std::string LANG_FEXT = ".lang";
const std::string LANG_EOF = "EOF";
/* _B_ == "Beginning", also der einleitende Spezifizierer */
const std::string LANG_B_IMPLEMENTATION = "IMPLEMENTATION:";
const std::string LANG_B_VAR = "VAR:";
const std::string LANG_B_STEP = "S:";
const std::string LANG_B_TRIGGER = "T:";
/* _B_ == "Ending", also der abschließende Spezifizierer */
const std::string LANG_E_IMPLEMENTATION = ":IMPLEMENTATION";
const std::string LANG_E_VAR = ":VAR";
const std::string LANG_E_STEP = ":S";
const std::string LANG_E_TRIGGER = ":T";

class Lang{
public:
    Lang();
    Lang(std::string fpath);
    ~Lang();
    
    bool loadFile(std::string fpath);
    bool start();                           /* Starten des Angriffs */
    void pause();                           /* Angriffausführung pausieren */
    void stop();                            /* Angriff stoppen, beim nächsten Start beginnt der Angriff wieder von vorne */

    void update();                          /* Die Angriffloop... managed Steps und Trigger etc. */

    /* Setter*/
    void setQuiet(bool q){ _quiet = q; }

    /* Getter */
    std::string getStatus() const { return _status; };
private:
    bool                                _quiet;             /* Statusmeldungen werden nicht angezeigt (ist Standard) */
    bool                                _running;           /* dient unter Anderem dazu festzustellen ob die Datei überhaupt ausführbar ist. Sollte im init-Teil etwas schief gehen 
                                                               wird _running nie true */
    std::string                         _status;            /* speichert Statusmeldungen */
    std::string                         _filepath;          /* Pfad zur aktuellen Angriffsdatei */
    std::ifstream                       _file;              /* Filehandler */
    std::vector<struct varinfo>         _dtinfo;            /* datatype info, dient dem Nachschlagen, wo eine Variable der .lang-Datei zu finden ist */
    std::vector<Tins::HWAddress<6>>     _haddr;             /* speichert alle in der .lang-Datei verwendeten Variablen des Typs Hardwareaddresse */
    std::vector<Tins::IPv4Address>      _ipaddr;            /* speichert alle in der .lang-Datei verwendeten Variablen des Typs IPv4-ddresse */
    std::vector<struct port>            _port;              /* speichert alle in der .lang-Datei verwendeten Variablen des Typs Port */
    std::vector<byte>                   _byte;              /* speichert alle in der .lang-Datei verwendeten Variablen des Typs Byte */
    std::vector<short>                  _short;             /* speichert alle in der .lang-Datei verwendeten Variablen des Typs Short */
    std::vector<int>                    _int;               /* speichert alle in der .lang-Datei verwendeten Variablen des Typs Integer */

    std::vector<unsigned char[1200]>    _packet;            /* die von assemble() erstellte pakete */
    
    unsigned int                        _intervall;         /* die Pausenzeit in s nach einem Step vor seiner erneuten Ausführung */

    // temporäre Speicher   
    std::string                         _assemble;          /* Speichert den Inhalt der aktuellen ASSEMBLE-Umgebung in "optimierter Form" (== ohne unnötige Leerzeichen, 
                                                               Kommentare, Leerzeilen etc.) */
    std::string                         _step;              /* speichert den Inhalt der aktuellen S[n]-Umgebung der .lang-Datei in optimierter Form */
    std::string                         _trigger;           /* speichert den Inhalt der aktuellen T[n]-Umgebung der .lang-Datei */


    //Hilfsfunktionen
    bool init();                            /* gibt false zurück, wenn keine "IMPLEMENTATION"-Umgebung gefunden wurde*/
    std::string analyse();                  /* geht die .lang-Datei Schritt für Schritt durch... damit ist gemeint, dass diese Funktion den nächsten Bezeichner ausfindig macht und 
                                               die entsprechende Funktion aufruft. Gibt den gefundenen Bezeichner als String ohne Doppelpunkt zurück (bspw. "VAR", 
                                               "IMPLEMENTATION", "S", "T", etc.) oder "EOF", wenn das Dateiende erreicht wurde */
    bool analyseVar();                      /* checkt die VAR-Umgebung auf Syntaxerror und gibt gültige Variablendeklarationen in einer Zeile an initVar() weiter; Wenn kein VAR-End-
                                               Spezifizierer folgt wird false zurückgegeben. Wenn falsche Bezeichner entdeckt werden wird eine Warnung in _status gespeichert */
    bool initVar(std::string vardec[3]);       /* speichert alle in .lang-Datei verwendeten Datentypen in entsprechender Form in den Datenstrukturen dieser Klasse 
                                               wenn etwas schief geht wird false zurückgegeben. Bspw. könnte es sein, dass ein Variablenname zweimal auftritt oder ähnliches. _status 
                                               wird entsprechend angepasst*/
    void readStep();                        /* liest die aktuelle S-Umgebung ein */
    void readTrigger();                     /* liest die aktuelle T-Umgebung ein */
    void readAssemble();                    /* liest die aktuelle ASSEMBLE-Umgebung ein */
    void step();                            /* führt den in _step gespeicherten Schritt aus, wenn es wieder an der Zeit ist (->_intervall) */
    void trigger();                         /* kontrolliert die angekommenen Datenpakete, ob sie den Vorgaben in _trigger entsprechen */
    void assemble(int index);               /* baut aus der aktuellen ASSEMBLE-Umgebung ein Paket zusammen, index ist der Index der ASSEMBLE-Umgebung, damit klar ist
                                               welches Paket zusammengebaut werden soll */


    void setStatus(std::string fct_name, std::string s);                        /* steht nicht bei den Settern, da nur die Klasse Statusmeldungen produzieren darf. fct_name ist der Name
                                                                                   der Funktion in der Statusmeldung verursacht wurde */
    std::string getNextWord(std::string line);                                  /* gibt nächstes Wort zurück + das Wortbeendende Zeichen (bspw. ';') außer es ist ein Leerzeichen */                                        
    
};

#endif
