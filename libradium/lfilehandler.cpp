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
    size_t wc = 0;

    bool ret = false;

    while(true){
        _file.clear();                      /* damit, wenn nur < 250 Byte ausgelesen werden können, trz weitergemacht wird.
                                               die schlechten bits wie ios::fail werden durch clear zurückgesetzt. */
        _file.getline(line, 1024);
        word.clear();
        wc = 0;
        
        while((word=getNextWord(line, wc)).length() != 0){

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
    std::string vardec[4] = { "", "", "", "" };
    size_t wc = 0;
    while(true){
        _file.clear();
        word.clear();
        _file.getline(line, 512);
        wc=0;

        while((word=getNextWord(line, wc)).length() != 0){

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
                while((word=getNextWord(line, wc)).size() != 0){

                    if(word.length() > 0 && word != LANG_CTRL_CHAR)
                        wordcount++;
                    
                    if(wordcount > 4){
                        _lang->setStatus("analyseVar()", "Zu viele Wörter in der Zeile  (max. 4) -> kann keine Variablendeklaration sein <<<");
                        break;
                    }

                    vardec[wordcount-1]=word;

                    pos = word.find_last_of(";", word.size());
                    if( pos > word.size() - 2 && pos < word.size() && wordcount < 5){                       /* kontrollieren ob ein Semikolon am Ende des Wortes erkannt wurde, das würde nämlich die Deklaration 
                                                                                                                abschließen */
                        _lang->setStatus("analyseVar()", ">>> Variablendeklaration erkannt: " + word);
                        initVar(vardec);
                        break;
                    } 
                    else if(pos != std::string::npos){
                        _lang->setStatus("analyseVar()", "Fehler bei einer Variablendeklaration <<<");
                        return false;
                    }
                }
            }
            
            if(word == LANG_E_VAR){
                _lang->setStatus("analyseVar()", ">>> ':VAR' gefunden!");
                return true;
            }
            
            word.clear();
        }

        if(_file.eof()){
            _lang->setStatus("analyseVar()", "VARIABLENTEIL WURDE NIE BEENDET! DATEI AM ENDE -> KEINE AUSFÜHRUNG MÖGLICH");
            return false;
        }
    }
}

bool LFileHandler::initVar(std::string vardec[4]){
            
    unsigned char tmp;
    /* kontrollieren dass es sich bei vardec[0] um Vartype handelt auch, wenn dies eigentlich feststehen sollte */
    if((tmp = getVarType(vardec[0])) == VARTYPE_INVALID){
        _lang->setStatus("initVar()", "Übergebener Datentypspezifizierer \"" + vardec[0] + "\"stellt keinen Datentyp dar! Variable konnte nicht erstellt werden! <<<");
        return false;
    }

    /* Kontrollieren ob Variablenbezeichner einzigartig ist */
    if(!_lang->varNameNotUsed(vardec[1])){
        _lang->setStatus("initVar()", "Der Variablenname \"" + vardec[1] + "\" wird bereits verwendet! Variablendeklaration wird ignoriert <<<");
        return false;
    }

    /* wenn vorhanden vom letzten vardec das Semikolon entfernen */
    if(vardec[3].length() != 0){
        if(vardec[3][vardec[3].length() - 1] == ';'){
            vardec[3].pop_back();
        }
    }
    else if(vardec[1][vardec[1].length() - 1] == ';')
        vardec[1].pop_back();
    else{
        _lang->setStatus("initVar()", vardec[0] + " " + vardec[1] + " " + vardec[2] + " " + vardec[3] + " ist keine ordnungsgemäße Variablendeklaration! <<<");
        return false;       
    }

    
    /* neuen Eintrag für Variable im Datentypinfoverzeichnis anlegen... abhängig vom Datentyp */
    if(tmp == VARTYPE_HADDR){
        _lang->_dtinfo.push_back({tmp, vardec[1], _lang->_haddr.size()});

        if(vardec[3].length() == 0)                    
            _lang->_haddr.push_back(Tins::HWAddress<6>());
        else if(!isValidHaddr(vardec[3])){
            _lang->setStatus("initVar()", "Übergebene HWADDR (" + vardec[3] + ") ist nicht im richtigen Format! Standardadresse 00:00:00:00:00:00 wird verwendet <<<");
            _lang->_haddr.push_back(Tins::HWAddress<6>());
        }
        else
            _lang->_haddr.push_back(Tins::HWAddress<6>(vardec[3]));
    }
    if(tmp == VARTYPE_IPADDR){
        _lang->_dtinfo.push_back({tmp, vardec[1], _lang->_ipaddr.size()});

        if(vardec[3].length() == 0){  
            _lang->_ipaddr.push_back(Tins::IPv4Address(""));
        }
        else if(!isValidIPv4(vardec[3])){       
            _lang->setStatus("initVar()", "Übergebene IP (" + vardec[3] + ") ist nicht im richtigen Format! Standardadresse 0.0.0.0 wird verwendet <<<");
            _lang->_ipaddr.push_back(Tins::IPv4Address(""));
        }
        else{        
            _lang->_ipaddr.push_back(Tins::IPv4Address(vardec[3]));
        }
    }
    if(tmp == VARTYPE_PORT){
        _lang->_dtinfo.push_back({tmp, vardec[1], _lang->_port.size()});
        
        if(vardec[3].length() == 0){  
            _lang->_port.push_back(0);
        }
        else if(!isValidPort(vardec[3])){       
            _lang->setStatus("initVar()", "Übergebener Port (" + vardec[3] + ") ist kein gültiger Port! Standardport 0 wird eingesetzt <<<");
            _lang->_port.push_back(0);
        }
        else{        
            _lang->_port.push_back((port)(std::stoul(vardec[3])));
        }
    }
    if(tmp == VARTYPE_BYTE){
        _lang->_dtinfo.push_back({tmp, vardec[1], _lang->_byte.size()});

        if(vardec[3].length() == 0){  
            _lang->_byte.push_back(0);
        }
        else if(!isValidShort(vardec[3])){       
            _lang->setStatus("initVar()", "Übergebener Byte (" + vardec[3] + ") ist kein gültiger Byte! Standardwert 0 wird eingesetzt <<<");
            _lang->_byte.push_back(0);
        }
        else{        
            _lang->_byte.push_back((byte)(std::stoul(vardec[3])));
        }
    }
    if(tmp == VARTYPE_SHORT){
        _lang->_dtinfo.push_back({tmp, vardec[1], _lang->_short.size()});

        if(vardec[3].length() == 0){  
            _lang->_short.push_back(0);
        }
        else if(!isValidShort(vardec[3])){       
            _lang->setStatus("initVar()", "Übergebener Short (" + vardec[3] + ") ist kein gültiger Short! Standardwert 0 wird eingesetzt <<<");
            _lang->_short.push_back(0);
        }
        else{        
            _lang->_short.push_back((unsigned short)(std::stoul(vardec[3])));
        }
    }
    if(tmp == VARTYPE_INT){
        _lang->_dtinfo.push_back({tmp, vardec[1], _lang->_int.size()});
       
       if(vardec[3].length() == 0){  
            _lang->_int.push_back(0);
        }
        else if(!isValidShort(vardec[3])){       
            _lang->setStatus("initVar()", "Übergebener Int (" + vardec[3] + ") ist kein gültiger Int! Standardwert 0 wird eingesetzt <<<");
            _lang->_int.push_back(0);
        }
        else{        
            _lang->_int.push_back((unsigned int)(std::stoul(vardec[3])));
        } 
    }
    if(tmp == VARTYPE_FILE){
        _lang->_dtinfo.push_back({tmp, vardec[1], _lang->_vtfile.size()});
       
       if(vardec[3].length() == 0){  
            _lang->_vtfile.push_back("");
        }
        else if(!isValidFile(vardec[3])){       
            _lang->setStatus("initVar()", "Übergebener FILE (" + vardec[3] + ") ist kein gültiger FILE! Standardwert "" wird eingesetzt <<<");
            _lang->_vtfile.push_back("");
        }
        else{        
            _lang->_vtfile.push_back(getFileData(vardec[3]));
        } 
    }
    if(tmp == VARTYPE_DATA){
        _lang->_dtinfo.push_back({tmp, vardec[1], _lang->_vtdata.size()});
       
       if(vardec[3].length() == 0){  
            _lang->_vtdata.push_back("");
        }
        else if(!isValidData(vardec[3])){       
            _lang->setStatus("initVar()", "Übergebener DATA (" + vardec[3] + ") ist kein gültiger DATA! Standardwert \"\" wird eingesetzt <<<");
            _lang->_vtdata.push_back("");
        }
        else{        
            _lang->_vtdata.push_back(vardec[3]);
        } 
    }
    
    return true;
}

bool LFileHandler::readStep(){
    /* analyse() kann nicht verwendet werden, da dann die Headerzeile verloren ginge */

    /* zurücksetzen der Speicher */
    _lang->_assembler.clear();

    char line[1024];
    std::string word ="";
    size_t wc = 0;

    bool ret = false;
    bool begfound = false;

    while(!begfound){

        _file.getline(line, 1024);
        word.clear();
        wc = 0;
        
        while((word=getNextWord(line, wc)).length() != 0){ 

            if(word == LANG_E_IMPLEMENTATION){
                _lang->setStatus("readStep()", ">>> Ende des IMPLEMENTATION-Teils erreicht! <<<");
                _lang->_running = false;
                return false;
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

    _lang->_step.setNum((short)std::stoul(arg));

    // nach optionaler Option ausschau halten
    std::string opt = getNextOption(line);
    if(opt == LANG_NOS){
        _lang->setStatus("readStep()", "Keine Option gefunden. <<<");
    }
    else{
        _lang->setStatus("readStep()", "Option " + opt + " gefunden. <<<");
        if(!isValidInt(opt)){
            _lang->setStatus("readStep()", "STEP-Umgebung Option: \"" + opt + "\" ist kein Integer <<<");
            return false;
        }

        _lang->_step.setIntervalMS((unsigned int)std::stoul(opt));
        _lang->_step.setTimed(true);
        _lang->_step.setLastStepNow();
    }

    std::string step;
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
            step+=ol;
            step+="\n";
        }
    }

    /* die anderen Umgebungen rausschneiden und initialisieren die zu einem Step gehören */

    if(readAllAssemble(step) == false){
        _lang->setStatus("readStep()", "die ASSEMBLE-Umgebungen konnten nicht eingelesen werden! <<<");
        return false;
    }
    if(readAllReaction(step) == false){
        _lang->setStatus("readStep()", "die REACTION-Umgebungen konnten nicht eingelesen werden! <<<");
        return false;
    }
    
    /* rausschneiden der anderen Subumgebungen */
    cutOut(&step, LANG_B_ASSEMBLE, LANG_E_ASSEMBLE);
    cutOut(&step, LANG_B_REACTION, LANG_E_REACTION);

    _lang->_step.setStep(step);

    return true;
}

bool LFileHandler::readTrigger(){
    /* analyse() kann nicht verwendet werden, da dann die Headerzeile verloren ginge */

    // alle Filter des vorherigen Triggers zurücksetzen
    _lang->_filter.clear();

    char line[1024];
    std::string word ="";
    size_t wc = 0;

    bool ret = false;
    bool begfound = false;

    while(!begfound){

        _file.getline(line, 1024);
        word.clear();
        
        while((word=getNextWord(line, wc)).length() != 0){ 

            if(word == LANG_E_IMPLEMENTATION){
                _lang->setStatus("readTrigger()", ">>> Ende des IMPLEMENTATION-Teils erreicht! <<<");
                _lang->_running = false;
                return true;
            }

            if(word == LANG_B_TRIGGER)
                begfound = true;
            word.clear();
        }

        
    }

    std::string arg = getArgument(line);

    if(arg == LANG_NOS){
        _lang->setStatus("readTrigger()", "TRIGGER-Umgebung fehlte das Argument <<<");
        return false;
    }

    if(!isValidShort(arg)){
        _lang->setStatus("readTrigger()", "TRIGGER-Umgebung Argument: \"" + arg + "\" ist kein Short <<<");
        return false;
    }

    _lang->_triggernum = (short)std::stoul(arg);
    _lang->_trigger.clear();
    std::string ol;

    while(true){
        _file.clear();                      /* damit, wenn nur < 250 Byte ausgelesen werden können, trz weitergemacht wird.
                                               die schlechten bits wie ios::fail werden durch clear zurückgesetzt. */
        _file.getline(line, 1024);
        word.clear();
        ol = optLine(line);

        if(ol.find(LANG_E_TRIGGER) != std::string::npos)
            break;

        //Zeilen mit weniger als einem Zeichen werden ignoriert
        if(ol == LANG_NOS)
            continue;
        else{
            _lang->_trigger+=ol;
            _lang->_trigger+="\n";
        }
    }

    if(readAllFilter(_lang->_trigger)==false){
        _lang->setStatus("readTrigger()", "die FILTER-Umgebungen konnten nicht eingelesen werden! <<<");
        return false;
    }

    return true;
}

bool LFileHandler::readAllAssemble(std::string step){
    size_t pos1, pos2 = 0;
    std::string arg = "";
    short a;

    while(true){

        pos1 = step.find(LANG_B_ASSEMBLE, pos2);

        if(pos1==std::string::npos)
            break;

        pos2 = step.find(LANG_E_ASSEMBLE, pos1);

        if(pos2==std::string::npos)
            break;

        arg = getArgument(step.substr(pos1, pos1 + LANG_B_ASSEMBLE.length() + 10));

        if(isValidShort(arg)){
            a = (short)atoi(arg.c_str());
        }
        else
            return false;
        

        /* kontrollieren, dass in diesem Step noch kein Paket mit gleicher Nummer erstellt wurde */
        for(int i = 0; i < _lang->_assembler.size(); ++i){
            if(_lang->_assembler[i].getNum() == a)
                return false;
        }

        _lang->_assembler.push_back(LAssembler());


        if(_lang->_assembler[_lang->_assembler.size()-1].setAssembler(step.substr(pos1, pos2 - pos1), _lang)==false){
            return false;
        }


        _lang->_assembler[_lang->_assembler.size()-1].setNum(a);

    }
    

    return true;
}

bool LFileHandler::readAllReaction(std::string step){
    size_t pos1, pos2 = 0;
    std::string arg = "";
    short a;
    while(true){
        pos1 = step.find(LANG_B_REACTION, pos2);

        if(pos1==std::string::npos)
            break;

        pos2 = step.find(LANG_E_REACTION, pos1);

        if(pos2==std::string::npos)
            break;

        arg = getArgument(step.substr(pos1, LANG_B_ASSEMBLE.length() + 10));

        if(isValidShort(arg)){
            a = (short)atoi(arg.c_str());
        }
        else{
            return false;
        }
        /* kontrollieren, dass in diesem Step noch kein Paket mit gleicher Nummer erstellt wurde */
        /*for(int i = 0; i < _lang->_reaction.size(); ++i){
            if(_lang->_reaction[i].getNum() == a){
                std::cout << "2" << std::endl;
                return false;
            }
        }*/

        _lang->_reaction.push_back(LReaction());
        if(_lang->_reaction[_lang->_reaction.size()-1].setReaction(step.substr(pos1, pos2 - pos1))==false)  {  
            return false;
        }
        _lang->_reaction[_lang->_reaction.size()-1].setNum(a);
        _lang->_reaction[_lang->_reaction.size()-1].setTriggered(false);
    }

    return true;
}

bool LFileHandler::readAllFilter(std::string trigger){
    size_t pos1, pos2 = 0;
    std::string arg = "";
    short a;

    while(true){

        arg  = "";

        pos1 = trigger.find(LANG_B_FILTER, pos2);

        if(pos1==std::string::npos)
            break;

        pos2 = trigger.find(LANG_E_FILTER, pos1);

        if(pos2==std::string::npos)
            break;

        _lang->_filter.push_back(LFilter());

        //Argument auslesen
        arg = getNextArgument(trigger.substr(pos1, trigger.find(":", pos1) - pos1));
        if(arg == LANG_PASS)
            _lang->_filter[_lang->_filter.size()-1].setType(LFilter::TYPE::PASS);
        else if(arg.find(LANG_B_REACTION) != std::string::npos){
            _lang->_filter[_lang->_filter.size()-1].setType(LFilter::TYPE::REACTION);
            arg = getNextArgument(arg);
            if(!isValidShort(arg)){
                return false;
            }
            else{
                _lang->_filter[_lang->_filter.size()-1].setNum((short)atoi(arg.c_str()));
            }
        }    
        else{
            return false;
        }     
        
        if(_lang->_filter[_lang->_filter.size()-1].setFilter(trigger.substr(pos1, pos2 - pos1), _lang)==false){
            return false;
        }
    }

    return true;
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

/* ALLGEMEINE HILFSFUNKTIONEN*/

std::string getNextWord(std::string line, size_t &word_count){
    std::string word="";
    int pos_tmp = 0;
    int i = 0;
    int j = 0;

    do{
        word = "";
        for(i; i < line.size(); ++i){
            //kontrollieren, ob ein '"' gefunden wurde, da dieses möglicherweise ein besonderes Verhalten hervorruft (siehe lang.hpp Funktionsdefinition getNextWord())
            if(line[i]=='"' && (pos_tmp = line.find_first_of('"', i+1)) != std::string::npos){           // es wurde ein zweites "'" gefunden
                word = line.substr(i + 1, pos_tmp - i - 1);
                i = pos_tmp+1;
            } 

            /* auf wortbeendende Zeichen achten*/
            if(!isWordEndingChar(line[i])){             
                word+=line[i];
            }
            else{
                if(line[i] != '\t' && line[i] != ' ' && line[i] != '\n')
                    word+=line[i];

                //wenn Wort bis jetzt leer ist, dann auch '\t' anhängen, damit es nicht mit dem Fall Zeilenende verwechselt wird
                if(word.size() == 0){
                    word=LANG_CTRL_CHAR;
                }
                
                i++;    //damit beim nächsten Buchstaben weitergemacht wird und sich das Programm nicht aufhängt
                break;
            }
        }
        if(word=="")
            break;
        j++;
    }while(j < word_count);
    
    word_count++;
    return word;        /* wenn Ende der Zeile erreicht wird wird word automatisch zu "" und hat somit die Länge 0 die in vielen If-Abfragen verwendet wird um festzustellen
                           ob das Zeilenende erreicht wurde */
}          

std::string getNextArgument(std::string str, size_t pos){
    size_t pos1 = 0;
    size_t pos2 = 0;
    
    if((pos1 = str.find_first_of('[', pos)) == std::string::npos){
        return "";
    }
    if((pos2 = str.find_last_of(']')) == std::string::npos){
        return "";
    }

    if(pos2 < pos1)
        return "";

    // Steuerzeichen entfernen

    std::string tmp = str.substr(pos1+1, pos2 - pos1 - 1);

    for(int i = 0; i < tmp.length(); ++i){
        if(tmp[i] < 33)
            tmp.erase(i, 1);
    }

    return tmp;
}       

std::string getNextOption(std::string str, size_t pos){
    size_t pos1 = 0;
    size_t pos2 = 0;
    
    if((pos1 = str.find_first_of('{', pos)) == std::string::npos){
        return "";
    }
    if((pos2 = str.find_last_of('}')) == std::string::npos){
        return "";
    }

    if(pos2 < pos1)
        return "";

    // Steuerzeichen entfernen

    std::string tmp = str.substr(pos1+1, pos2 - pos1 - 1);

    for(int i = 0; i < tmp.length(); ++i){
        if(tmp[i] < 33)
            tmp.erase(i, 1);
    }

    return tmp;
}                        

std::string optLine(std::string line){
    std::string ol;
    int sc = 0;
    for(int i = 0; i < line.size(); ++i){

        if(line[i] == ' ' && sc < 1 && ol.length() > 0){
            ol += line[i];
            sc++;
        }
        if(line[i] != ' '){
            sc=0;
        }
        if(line[i] == LANG_C_COMMENT)
            break;

        if(line[i] > 32)
            ol += line[i];
    }

    return ol;
}

bool isWordEndingChar(char c){
    if(c != ' ' && c != '\0' && c != '\t' && c != ';' && c != '#'
        && c != '[' && c != '{' && c != '=' && c != '(' && c != '\n'
        && c != '+' && c != '-' && c != '*' && c != '/')
        return false;
    
    return true;
}

bool isOperand(char c){
    return (c == '+' || c == '-' || c == '*' || c == '/');
}

void cutOut(std::string *env, std::string beg, std::string end){
    size_t pos1, pos2;
    pos1 = pos2 = 0;
    while(true){
        pos1 = env->find(beg);
        pos2 = env->find(end, pos1+1);

        if(pos1 == std::string::npos || pos2 == std::string::npos)
            break;
        else{
            env->erase(pos1, pos2 + end.length() - pos1);
        }
        pos1=pos2=0;
    }
}