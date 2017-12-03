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

    //wenn eine neue Line kommt, die Werte zurücksetzen
    if(lastline != line){
        i = 0; 
        lastline = line;
    }

    for(i; i < line.size(); ++i){
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
    