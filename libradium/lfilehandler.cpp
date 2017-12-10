#include "lfilehandler.hpp"

#include "lang.hpp"

LFileHandler::LFileHandler(){
    _filepath = "";
    _lang = nullptr;
}

LFileHandler::~LFileHandler(){
    _file.close();
}

bool LFileHandler::linkLang(Lang *lang){
    if(lang == nullptr)
        return false;

    _lang = lang;

    return true;
}

bool LFileHandler::loadFile(std::string fpath){

    if(!_lang)
        return false;

    if(!fpath.find(LANG_FEXT)){
       _lang->setStatus("loadFile()", ("File does not have the " + LANG_FEXT + " file extension -> File wasn't opened!"));
       return false;
    }

    _file.open(fpath, std::ios::in);

    if(!_file.good()){
        _lang->setStatus("loadFile()", ("Failed to open File: " + fpath));
        return false;
    }

    _filepath = fpath;
    return true;
}

bool LFileHandler::init(){
    _file.seekg(std::ios::beg);
    std::string tmp;

    while(true){
        tmp=analyse();

        if(tmp == LANG_B_VAR){
            _lang->setStatus("init()", ">>> VAR-Teil erreicht");
            analyseVar();
        } 
        if(tmp == LANG_E_VAR){
            //analyseVar();
            _lang->setStatus("init()", ">>> VAR-End-Teil erreicht");
        }    
        if(tmp == LANG_B_IMPLEMENTATION){
            _lang->setStatus("init()", ">>> IMPLEMENTATION-Teil erreicht");
            break;
        }    
        if(tmp == LANG_EOF){
            _lang->setStatus("init()", "Kein IMPLEMENTATION-Teil gefunden -> Datei kann nicht ausgeführt werden! <<<");
            return false;
        }
    }
    return true;
}

std::string LFileHandler::analyse(){
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

bool LFileHandler::analyseVar(){
    char line[512];
    std::string word ="";
    std::string vardec[3] = { "", "", "" };

    while(true){
        _file.clear();
        word.clear();
        _file.getline(line, 512);

        while((word=getNextWord(line)).length() != 0){

            

            if(word == LANG_B_IMPLEMENTATION){
                _lang->setStatus("analyseVar()", "kein ':VAR' vor 'IMPLEMENTATION:' gefunden <<<");
                return false;
            }
            if(isVartype(word)){
                vardec[0].clear();vardec[1].clear();vardec[2].clear();
                _lang->setStatus("analyseVar()", ">>> Vartype " + word + " gefunden");
                vardec[0] = word;
                int wordcount = 1;          /* zählt die Wörter in der Zeile -> max. 3*/ 
                word.clear();               /* kann gleich wiederverwendet werden */
                size_t pos;
                while((word=getNextWord(line)).size() != 0){

                    if(word.length() > 0 && word != LANG_CTRL_CHAR)
                        wordcount++;
                    
                    if(wordcount > 3){
                        _lang->setStatus("analyseVar()", "Zu viele Wörter in der Zeile  (max. 3) -> kann keine Variablendeklaration sein <<<");
                        break;
                    }

                    vardec[wordcount-1]=word;

                    pos = word.find_last_of(";", word.size());
                    if( pos > word.size() - 2 && pos < word.size() && wordcount < 4){                       /* kontrollieren ob ein Semikolon am Ende des Wortes erkannt wurde, das würde nämlich die Deklaration 
                                                                                                                abschließen */
                        _lang->setStatus("analyseVar()", ">>> Variablendeklaration erkannt: " + word);
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
                _lang->setStatus("analyseVar()", ">>> ':VAR' gefunden!");
                return true;
            }
            /*ret |= (word == LANG_E_STEP);
            ret |= (word == LANG_E_TRIGGER);*/
            word.clear();
        }

        if(_file.eof()){
            _lang->setStatus("analyseVar()", "VARIABLENTEIL WURDE NIE BEENDET! DATEI AM ENDE -> KEINE AUSFÜHRUNG MÖGLICH");
            return false;
        }
    }
}

bool LFileHandler::initVar(std::string vardec[3]){
    /*
        NUR EINE GEDANKENSTÜTZE: 

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
        _lang->setStatus("initVar()", "Übergebener Datentypspezifizierer stellt keinen Datentyp dar! Variable konnte nicht erstellt werden! <<<");
        return false;
    }

    /* Kontrollieren ob Variablenbezeichner einzigartig ist */
    if(!_lang->varNameNotUsed(vardec[1])){
        _lang->setStatus("initVar()", "Der Variablenname \"" + vardec[1] + "\" wird bereits verwendet! Variablendeklaration wird ignoriert <<<");
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
        _lang->_dtinfo.push_back({tmp, vardec[1], _lang->_haddr.size()});

        if(vardec[2].length() == 0)                    
            _lang->_haddr.push_back(Tins::HWAddress<6>());
        else if(!isValidHaddr(vardec[2])){
            _lang->setStatus("initVar()", "Übergebene HWADDR (" + vardec[2] + ") ist nicht im richtigen Format! Standardadresse 00:00:00:00:00:00 wird verwendet <<<");
            _lang->_haddr.push_back(Tins::HWAddress<6>());
        }
        else
            _lang->_haddr.push_back(Tins::HWAddress<6>(vardec[2]));
    }
    if(tmp == VARTYPE_IPADDR){
        _lang->_dtinfo.push_back({tmp, vardec[1], _lang->_ipaddr.size()});

        if(vardec[2].length() == 0){  
            _lang->_ipaddr.push_back(Tins::IPv4Address(""));
        }
        else if(!isValidIPv4(vardec[2])){       
            _lang->setStatus("initVar()", "Übergebene IP (" + vardec[2] + ") ist nicht im richtigen Format! Standardadresse 0.0.0.0 wird verwendet <<<");
            _lang->_ipaddr.push_back(Tins::IPv4Address(""));
        }
        else{        
            _lang->_ipaddr.push_back(Tins::IPv4Address(vardec[2]));
        }
    }
    if(tmp == VARTYPE_PORT){
        _lang->_dtinfo.push_back({tmp, vardec[1], _lang->_port.size()});
        
        if(vardec[2].length() == 0){  
            _lang->_port.push_back(0);
        }
        else if(!isValidPort(vardec[2])){       
            _lang->setStatus("initVar()", "Übergebener Port (" + vardec[2] + ") ist kein gültiger Port! Standardport 0 wird eingesetzt <<<");
            _lang->_port.push_back(0);
        }
        else{        
            _lang->_port.push_back((port)(std::stoul(vardec[2])));
        }
    }
    if(tmp == VARTYPE_BYTE){
        _lang->_dtinfo.push_back({tmp, vardec[1], _lang->_byte.size()});

        if(vardec[2].length() == 0){  
            _lang->_byte.push_back(0);
        }
        else if(!isValidShort(vardec[2])){       
            _lang->setStatus("initVar()", "Übergebener Byte (" + vardec[2] + ") ist kein gültiger Byte! Standardwert 0 wird eingesetzt <<<");
            _lang->_byte.push_back(0);
        }
        else{        
            _lang->_byte.push_back((byte)(std::stoul(vardec[2])));
        }
    }
    if(tmp == VARTYPE_SHORT){
        _lang->_dtinfo.push_back({tmp, vardec[1], _lang->_short.size()});

        if(vardec[2].length() == 0){  
            _lang->_short.push_back(0);
        }
        else if(!isValidShort(vardec[2])){       
            _lang->setStatus("initVar()", "Übergebener Short (" + vardec[2] + ") ist kein gültiger Short! Standardwert 0 wird eingesetzt <<<");
            _lang->_short.push_back(0);
        }
        else{        
            _lang->_short.push_back((unsigned short)(std::stoul(vardec[2])));
        }
    }
    if(tmp == VARTYPE_INT){
        _lang->_dtinfo.push_back({tmp, vardec[1], _lang->_int.size()});
       
       if(vardec[2].length() == 0){  
            _lang->_int.push_back(0);
        }
        else if(!isValidShort(vardec[2])){       
            _lang->setStatus("initVar()", "Übergebener Int (" + vardec[2] + ") ist kein gültiger Int! Standardwert 0 wird eingesetzt <<<");
            _lang->_int.push_back(0);
        }
        else{        
            _lang->_int.push_back((unsigned int)(std::stoul(vardec[2])));
        } 
    }
    
    return true;
}

bool LFileHandler::readStep(){
    /* analyse() kann nicht verwendet werden, da dann die Headerzeile verloren ginge */

    char line[1024];
    std::string word ="";

    bool ret = false;
    bool begfound = false;

    while(!begfound){

        _file.getline(line, 1024);
        word.clear();
        
        while((word=getNextWord(line)).length() != 0){ 

            if(word == LANG_E_IMPLEMENTATION){
                _lang->setStatus("readStep()", ">>> Ende des IMPLEMENTATION-Teils erreicht! <<<");
                _lang->_running = false;
                return true;
            }

            if(word == LANG_B_STEP)
                begfound = true;
            word.clear();
        }

        
    }

    std::string arg = getArgument(line);

    if(arg == LANG_NOS){
        _lang->setStatus("readStep()", "STEP-Umgebung fehlte das Argument <<<");
        return false;
    }

    if(!isValidShort(arg)){
        _lang->setStatus("readStep()", "STEP-Umgebung Argument: \"" + arg + "\" ist kein Short <<<");
        return false;
    }

    _lang->_stepnum = (short)std::stoul(arg);

    std::string ol;
    while(true){
        _file.clear();                      /* damit, wenn nur < 250 Byte ausgelesen werden können, trz weitergemacht wird.
                                               die schlechten bits wie ios::fail werden durch clear zurückgesetzt. */
        _file.getline(line, 1024);
        word.clear();
        ol = optLine(line);

        if(ol == LANG_E_STEP)
            break;

        //Zeilen mit weniger als einem Zeichen werden ignoriert
        if(ol == LANG_NOS)
            continue;
        else{
            _lang->_step+=ol;
            _lang->_step+="\n";
        }
    }

    return true;
}

std::string LFileHandler::getNextWord(std::string line){
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
        if(line[i] != ' ' && line[i] != '\0' && line[i] != '\t' && line[i] != ';' && line[i] != '#'
            && line[i] != '[' && line[i] != '{'){             
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

std::string LFileHandler::getArgument(std::string arg){
    int pos1 = 0, pos2 = 0;

    if((pos1 = arg.find_first_of('[')) == std::string::npos){
        _lang->setStatus("getArgument()", "Kein Argument in: \"" + arg + "\" gefunden. <<<");
        return "";
    }

    if((pos2 = arg.find_first_of(']')) == std::string::npos){
        _lang->setStatus("getArgument()", "Argument in: \"" + arg + "\" wurde geöffnet aber nicht geschlossen. <<<");
        return "";
    }

    return arg.substr(pos1+1, pos2 - (pos1 + 1));
}   

std::string LFileHandler::getOption(std::string opt){
    return LANG_NOS;
}                                     

std::string LFileHandler::optLine(std::string line){
    std::string ol;
    int sc = 0;
    for(int i = 0; i < line.size(); ++i){
        if(line[i] == ' ' && sc < 1){
            ol += line[i];
            sc++;
        }
        if(line[i] != ' '){
            sc=0;
        }
        if(line[i] == LANG_C_COMMAND)
            break;

        if(line[i] > 32)
            ol += line[i];
    }

    return ol;
}