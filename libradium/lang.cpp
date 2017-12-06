#include "lang.hpp"

#include <tins/tins.h>      /* libtins für Netzwerkfunktionen etc. */

#include "vartypes.hpp"       /* Datentypdefinitionen */
#include "header.hpp"         /* Protokollheaderformate etc. */

const std::string LANG_CTRL_CHAR = "*";         // wird bei getNextWord()-Funktion zurückgegeben, wenn Zeichen wie '\t' das Word wären

Lang::Lang(){
    _running = false;
    _status = "Lang was initialized";
    if(!_quiet)
        std::cout << "Lang: Lang(): " << _status << std::endl;
}

Lang::Lang(std::string fpath){
    _running = false;
    if(loadFile(fpath))
        setStatus("Lang()", "Lang was initialized");
    else
        setStatus("Lang()", "Lang wasn't correctly initialized");
}

Lang::~Lang(){
    _file.close();
}

bool Lang::loadFile(std::string fpath){

    if(!fpath.find(LANG_FEXT)){
       setStatus("loadFile()", ("File does not have the " + LANG_FEXT + " file extension -> File wasn't opened!"));
       return false;
    }

    _file.open(fpath, std::ios::in);

    if(!_file.good()){
        setStatus("loadFile()", ("Failed to open File: " + fpath));
        return false;
    }

    _filepath = fpath;
    return true;
}

bool Lang::start(){
    return (_running = init());                 // alle Variablen einlesen
}

void Lang::showVars(){
    std::cout << "Variablenliste \"" << _filepath << "\": " << std::endl;
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
            default:
                std::cout << std::endl;
        }
    }
}

bool Lang::init(){
    _file.seekg(std::ios::beg);
    std::string tmp;
    while(true){

        tmp=analyse();

        if(tmp == LANG_B_VAR){
            setStatus("init()", ">>> VAR-Teil erreicht");
            analyseVar();
        } 
        if(tmp == LANG_E_VAR){
            //analyseVar();
            setStatus("init()", ">>> VAR-End-Teil erreicht");
        }    
        if(tmp == LANG_B_IMPLEMENTATION){
            setStatus("init()", ">>> IMPLEMENTATION-Teil erreicht");
            break;
        }    
        if(tmp == LANG_EOF){
            setStatus("init()", "Kein IMPLEMENTATION-Teil gefunden -> Datei kann nicht ausgeführt werden! <<<");
            return false;
        }
    }
    return true;
}

std::string Lang::analyse(){
    char line[1024];
    std::string word ="";

    bool ret = false;

    while(true){
        _file.clear();                      /* damit, wenn nur < 250 Byte ausgelesen werden können, trz weitergemacht wird.
                                               die schlechten bits wie ios::fail werden durch clear zurückgesetzt. */
        _file.getline(line, 1024);
        word.clear();
        
        while((word=getNextWord(line)).length() != 0){

            if(word != LANG_CTRL_CHAR)
                std::cout << "\"" <<  word << "\"" << std::endl;
            ret |= (word == LANG_B_IMPLEMENTATION);
            ret |= (word == LANG_B_VAR);
            ret |= (word == LANG_B_STEP);
            ret |= (word == LANG_B_TRIGGER);
            ret |= (word == LANG_E_IMPLEMENTATION);
            ret |= (word == LANG_E_VAR);
            ret |= (word == LANG_E_STEP);
            ret |= (word == LANG_E_TRIGGER);

            if(ret)
                return word;

            word.clear();
        }

        if(_file.eof())
            return LANG_EOF; 
    }
}

bool Lang::analyseVar(){
    char line[512];
    std::string word ="";
    std::string vardec[3] = { "", "", "" };

    while(true){
        _file.clear();
        word.clear();
        _file.getline(line, 512);

        while((word=getNextWord(line)).length() != 0){

            

            if(word == LANG_B_IMPLEMENTATION){
                setStatus("analyseVar()", "kein ':VAR' vor 'IMPLEMENTATION:' gefunden <<<");
                return false;
            }
            if(isVartype(word)){
                vardec[0].clear();vardec[1].clear();vardec[2].clear();
                setStatus("analyseVar()", ">>> Vartype " + word + " gefunden");
                vardec[0] = word;
                int wordcount = 1;          /* zählt die Wörter in der Zeile -> max. 3*/ 
                word.clear();               /* kann gleich wiederverwendet werden */
                size_t pos;
                while((word=getNextWord(line)).size() != 0){

                    if(word.length() > 0 && word != LANG_CTRL_CHAR)
                        wordcount++;
                    
                    if(wordcount > 3){
                        setStatus("analyseVar()", "Zu viele Wörter in der Zeile  (max. 3) -> kann keine Variablendeklaration sein <<<");
                        break;
                    }

                    vardec[wordcount-1]=word;

                    pos = word.find_last_of(";", word.size());
                    if( pos > word.size() - 2 && pos < word.size() && wordcount < 4){                       /* kontrollieren ob ein Semikolon am Ende des Wortes erkannt wurde, das würde nämlich die Deklaration 
                                                                                                                abschließen */
                        setStatus("analyseVar()", ">>> Variablendeklaration erkannt: " + word);
                        initVar(vardec);
                        break;
                    } 
                }
            }
            
            /*ret |= (word == LANG_B_STEP);
            ret |= (word == LANG_B_TRIGGER);
            ret |= (word == LANG_E_IMPLEMENTATION);
            */
            if(word == LANG_E_VAR){
                setStatus("analyseVar()", ">>> ':VAR' gefunden!");
                return true;
            }
            /*ret |= (word == LANG_E_STEP);
            ret |= (word == LANG_E_TRIGGER);*/
            word.clear();
        }

        if(_file.eof()){
            setStatus("analyseVar()", "VARIABLENTEIL WURDE NIE BEENDET! DATEI AM ENDE -> KEINE AUSFÜHRUNG MÖGLICH");
            return false;
        }
    }
}

bool Lang::initVar(std::string vardec[3]){
    /*
        const unsigned char VARTYPE_HADDR   = 1;       // Hardwareaddresse
        const unsigned char VARTYPE_IPADDR  = 2;       // IPv4-Addresse
        const unsigned char VARTYPE_PORT    = 3;       // TCP/UDP-Port
        const unsigned char VARTYPE_BYTE    = 4;
        const unsigned char VARTYPE_SHORT   = 5;
        const unsigned char VARTYPE_INT     = 6;

        struct varinfo{
        unsigned char _type;            // siehe VARTYPE_*
        std::string _name;              // Name der Variable innerhalb der .lang-Datei
        short _index;                   // der Index der Variable innerhalb der Vector-Struktur in der alle Variablen des gleichen Typs gespeichert werden

        std::vector<struct varinfo>         _dtinfo;             datatype info, dient dem Nachschlagen, wo eine Variable der .lang-Datei zu finden ist 
        std::vector<Tins::HWAddress<6>>     _haddr;              speichert alle in der .lang-Datei verwendeten Variablen des Typs Hardwareaddresse 
        std::vector<Tins::IPv4Address>      _ipaddr;             speichert alle in der .lang-Datei verwendeten Variablen des Typs IPv4-ddresse 
        std::vector<struct port>            _port;               speichert alle in der .lang-Datei verwendeten Variablen des Typs Port 
        std::vector<byte>                   _byte;               speichert alle in der .lang-Datei verwendeten Variablen des Typs Byte 
        std::vector<short>                  _short;              speichert alle in der .lang-Datei verwendeten Variablen des Typs Short 
        std::vector<int>                    _int;                speichert alle in der .lang-Datei verwendeten Variablen des Typs Integer 
    */
            
    unsigned char tmp;
    /* kontrollieren dass es sich bei vardec[0] um Vartype handelt auch, wenn dies eigentlich feststehen sollte */
    if((tmp = getVarType(vardec[0])) == VARTYPE_INVALID){
        setStatus("initVar()", "Übergebener Datentypspezifizierer stellt keinen Datentyp dar! Variable konnte nicht erstellt werden!");
        return false;
    }

    /* wenn vorhanden vom letzten vardec das Semikolon entfernen */
    if(vardec[2].length() != 0){
        if(vardec[2][vardec[2].length() - 1] == ';'){
            vardec[2].pop_back();
        }
    }
    else if(vardec[1][vardec[1].length() - 1] == ';')
        vardec[1].pop_back();

    
    /* neuen Eintrag für Variable im Datentypinfoverzeichnis anlegen... abhängig vom Datentyp */
    if(tmp == VARTYPE_HADDR){
        _dtinfo.push_back({tmp, vardec[1], _haddr.size()});

        if(vardec[2].length() == 0)                    
            _haddr.push_back(Tins::HWAddress<6>());
        else if(!isValidHaddr(vardec[2])){
            setStatus("initVar()", "Übergebene HWADDR (" + vardec[2] + ") ist nicht im richtigen Format! Standardadresse 00:00:00:00:00:00 wird verwendet <<<");
            _haddr.push_back(Tins::HWAddress<6>());
        }
        else
            _haddr.push_back(Tins::HWAddress<6>(vardec[2]));
    }
    if(tmp == VARTYPE_IPADDR){
        _dtinfo.push_back({tmp, vardec[1], _ipaddr.size()});

        if(vardec[2].length() == 0){  
            _ipaddr.push_back(Tins::IPv4Address(""));
        }
        else if(!isValidIPv4(vardec[2])){       
            setStatus("initVar()", "Übergebene IP (" + vardec[2] + ") ist nicht im richtigen Format! Standardadresse 0.0.0.0 wird verwendet <<<");
            _ipaddr.push_back(Tins::IPv4Address(""));
        }
        else{        
            _ipaddr.push_back(Tins::IPv4Address(vardec[2]));
        }
    }
    if(tmp == VARTYPE_PORT){
        _dtinfo.push_back({tmp, vardec[1], _port.size()});
        //type.push_back()...
    }
    if(tmp == VARTYPE_BYTE){
        _dtinfo.push_back({tmp, vardec[1], _byte.size()});
        //type.push_back()...
    }
    if(tmp == VARTYPE_SHORT){
        _dtinfo.push_back({tmp, vardec[1], _short.size()});
        //type.push_back()...
    }
    if(tmp == VARTYPE_INT){
        _dtinfo.push_back({tmp, vardec[1], _int.size()});
        //type.push_back()...
    }
    
    return true;
}

void Lang::setStatus(std::string fct_name, std::string s){
    _status = s;
    if(!_quiet)
        std::cout << "Lang: " << fct_name << ": " << _status << std::endl;
}
    
std::string Lang::getNextWord(std::string line){
    static std::string lastline="";
    static int i=0;
    std::string word="";
    int pos_tmp = 0;

    //wenn eine neue Line kommt, die Werte zurücksetzen
    if(lastline != line){
        i = 0; 
        lastline = line;
    }

    for(i; i < line.size(); ++i){

        //kontrollieren, ob ein '"' gefunden wurde, da dieses möglicherweise ein besonderes Verhalten hervorruft (siehe lang.hpp Funktionsdefinition getNextWord())
        if(line[i]=='"' && (pos_tmp = line.find_first_of('"', i+1)) != std::string::npos){           // es wurde ein zweites "'" gefunden
            word = line.substr(i + 1, pos_tmp - i - 1);
            i = pos_tmp + 1;
        } 

        /* auf wortbeendende Zeichen achten*/
        if(line[i] != ' ' && line[i] != '\0' && line[i] != '\t' && line[i] != ';' && line[i] != '#'){             
            word+=line[i];
        }
        else{
            if(line[i] != '\t' && line[i] != ' ')
                word+=line[i];

            //wenn Wort bis jetzt leer ist, dann auch '\t' anhängen, damit es nicht mit dem Fall Zeilenende verwechselt wird
            if(word.size() == 0)
                word=LANG_CTRL_CHAR;
            
            i++;    //damit beim nächsten Buchstaben weitergemacht wird und sich das Programm nicht aufhängt
            return word;
        }
    }

    return word;        /* wenn Ende der Zeile erreicht wird wird word automatisch zu "" und hat somit die Länge 0 die in vielen If-Abfragen verwendet wird um festzustellen
                           ob das Zeilenende erreicht wurde */
}
    