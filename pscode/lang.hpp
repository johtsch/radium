
#include <fstream>      //für Dateioperationen

#include "vartypes.hpp"

class Lang{
public:
    Lang();
    Lang(std::string filepath);
    ~Lang();
    void loadFile(std::string path);        // lädt eine neue .lang-Datei und bereitet Ausführung vor
    void start();                           // löst den Angriff sozusagen aus
    void update();                          // ruft die step()- und trigger()-Funktionen auf

private:
    std::string                         _filepath;
    std::fstream                        _file;  
    std::vector<struct varinfo>         _dtinfo;    // datatype info, dient dem Nachschlagen, wo eine Variable der .lang-Datei zu finden ist
    std::vector<struct haddr>           _haddr;
    std::vector<struct ipaddr>          _ipaddr;  
    std::vector<struct port>            _port;
    std::vector<byte>                   _byte;
    std::vector<short>                  _short;
    std::vector<int>                    _int;

    std::string _currS                  _step;
    std::string _currT                  _trigger;

    std::vector<unsigned char[1200]>    _packet;        // die von assemble() erstellte pakete
    
    unsigned int                        _intervall;     // die Pausenzeit in ms nach einem Step vor seiner erneuten Ausführung

    // temporäre Speicher   
    std::string                         _assemble;      // Speichert den Inhalt der aktuellen ASSEMBLE-Umgebung in "optimierter Form" (== ohne unnötige Leerzeichen, 
                                                        // Kommentare, Leerzeilen etc.)

    //Hilfsfunktionen
    void analyseVar();                      // checkt die VAR-Umgebung auf Syntaxerror und gibt gültige Variablendeklarationen in einer Zeile an initVar() weiter;
    void initVar(std::string vardec);       // liest alle in .lang Datei enthaltene 
    void step();                            // führt den in _step gespeicherten Schritt aus, wenn es wieder an der Zeit ist (->_intervall)
    void trigger();                         // kontrolliert die angekommenen Datenpakete, ob sie den Vorgaben in _trigger entsprechen
    void assemble(int index);               // baut aus der aktuellen ASSEMBLE-Umgebung ein Paket zusammen, index ist der Index der ASSEMBLE-Umgebung, damit klar ist
                                            // welches Paket zusammengebaut werden soll
};