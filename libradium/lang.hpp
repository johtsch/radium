#ifndef LR_LANG_HPP
#define LR_LANG_HPP

#include <iostream>
#include <string>           /* std::string etc */
#include <fstream>          /* Dateioperationen */
#include <vector>           /* std::vector<T> */
#include <tins/tins.h>      /* libtins für Netzwerkfunktionen etc. */

#include "vartypes.h"       /* Datentypdefinitionen */
#include "header.h"         /* Protokollheaderformate etc. */

class Lang{
public:
    Lang();
    Lang(std::string fpath);
    ~Lang();
    
    void loadFile(std::string fpath);
    void start();                           /* Starten des Angriffs */
    void pause();                           /* Angriffausführung pausieren */
    void stop();                            /* Angriff stoppen, beim nächsten Start beginnt der Angriff wieder von vorne */

    void update();                          /* Die Angriffloop... managed Steps und Trigger etc. */
private:
    std::string                         _filepath;          /* Pfad zur aktuellen Angriffsdatei */
    std::fstream                        _file;              /* Filehandler */
    std::vector<struct varinfo>         _dtinfo;            /* datatype info, dient dem Nachschlagen, wo eine Variable der .lang-Datei zu finden ist */
    std::vector<struct HWAddress<6>>    _haddr;             /* speichert alle in der .lang-Datei verwendeten Variablen des Typs Hardwareaddresse */
    std::vector<struct IPv4Address>     _ipaddr;            /* speichert alle in der .lang-Datei verwendeten Variablen des Typs IPv4-ddresse */
    std::vector<struct port>            _port;              /* speichert alle in der .lang-Datei verwendeten Variablen des Typs Port */
    std::vector<byte>                   _byte;              /* speichert alle in der .lang-Datei verwendeten Variablen des Typs Byte */
    std::vector<short>                  _short;             /* speichert alle in der .lang-Datei verwendeten Variablen des Typs Short */
    std::vector<int>                    _int;               /* speichert alle in der .lang-Datei verwendeten Variablen des Typs Integer */

    std::string                         _step;              /* speichert den Inhalt der aktuellen S[n]-Umgebung der .lang-Datei in optimierter Form */
    std::string                         _trigger;           /* speichert den Inhalt der aktuellen T[n]-Umgebung der .lang-Datei */

    std::vector<unsigned char[1200]>    _packet;            /* die von assemble() erstellte pakete */
    
    unsigned int                        _intervall;         /* die Pausenzeit in s nach einem Step vor seiner erneuten Ausführung */

    // temporäre Speicher   
    std::string                         _assemble;          /* Speichert den Inhalt der aktuellen ASSEMBLE-Umgebung in "optimierter Form" (== ohne unnötige Leerzeichen, 
                                                               Kommentare, Leerzeilen etc.) */

    //Hilfsfunktionen
    void analyseVar();                      /* checkt die VAR-Umgebung auf Syntaxerror und gibt gültige Variablendeklarationen in einer Zeile an initVar() weiter; */
    void initVar(std::string vardec);       /* speichert alle in .lang-Datei verwendeten Datentypen in entsprechender Form in den Datenstrukturen dieser Klasse */
    void readStep();                        /* liest die aktuelle S-Umgebung ein */
    void readTrigger();                     /* liest die aktuelle T-Umgebung ein */
    void readAssemble();                    /* liest die aktuelle ASSEMBLE-Umgebung ein */
    void step();                            /* führt den in _step gespeicherten Schritt aus, wenn es wieder an der Zeit ist (->_intervall) */
    void trigger();                         /* kontrolliert die angekommenen Datenpakete, ob sie den Vorgaben in _trigger entsprechen */
    void assemble(int index);               /* baut aus der aktuellen ASSEMBLE-Umgebung ein Paket zusammen, index ist der Index der ASSEMBLE-Umgebung, damit klar ist
                                               welches Paket zusammengebaut werden soll */
    
};

#endif