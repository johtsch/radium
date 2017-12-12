#ifndef LR_LFILEHANDLER_HPP
#define LR_LFILEHANDLER_HPP

/* Der Lang-File-Handler kümmert sich um das Auslesen und Auswerten von .lang-Dateien */

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <tins/tins.h>      /* libtins für Netzwerkfunktionen etc. */
#include "lconst.hpp"
#include "lstep.hpp"
#include "vartypes.hpp"

/* allgemeine Hilfsfunktionen */
std::string getNextWord(std::string line);                                  /* gibt nächstes Wort zurück + das Wortbeendende Zeichen (bspw. ';') außer es ist ein Leerzeichen oder Steuerzeichen,
                                                                                   beginnt das nächste Wort mit '"' wird - bei Vorhandensein eines zweiten '"' in der selben Zeile - die gesamte davon 
                                                                                   eingeschlossene Zeichensequenz zurückgegeben (ohne die umschließenden '"'). Ist nur ein '"' vorhanden wird es wie 
                                                                                   ein wortbeendendes Zeichen behandelt und einzeln zurückgegeben */ 
std::string optLine(std::string line);                                      /* gibt eine übergebene Zeile in optimierter Form zurück: keine unnötigen Steuer-/Leerzeichen und keine Kommentare */

class Lang;                     /* um circular references aus dem Weg zu gehen*/

class LFileHandler{
public: 
    LFileHandler();
    ~LFileHandler();

    bool linkLang(Lang *lang);                          /* legt die Lang Instanz fest für die die .lang-Datei ausgewertet werden soll */

    /* damit folgende Funktionen ausgeführt werden können, muss erst eine Lang-Klasse gelinkt sein */ 
    bool loadFile(std::string fpath);
    bool init();                            /* gibt false zurück, wenn keine "IMPLEMENTATION"-Umgebung gefunden wurde */
    std::string analyse();                  /* geht die .lang-Datei Schritt für Schritt durch... damit ist gemeint, dass diese Funktion den nächsten Bezeichner ausfindig macht und 
                                               die entsprechende Funktion aufruft. Gibt den gefundenen Bezeichner als String ohne Doppelpunkt zurück (bspw. "VAR", 
                                               "IMPLEMENTATION", "S", "T", etc.) oder "EOF", wenn das Dateiende erreicht wurde */
    bool analyseVar();                      /* checkt die VAR-Umgebung auf Syntaxerror und gibt gültige Variablendeklarationen in einer Zeile an initVar() weiter; Wenn kein VAR-End-
                                               Spezifizierer folgt wird false zurückgegeben. Wenn falsche Bezeichner entdeckt werden wird eine Warnung in _status gespeichert */
    bool initVar(std::string vardec[3]);    /* speichert alle in .lang-Datei verwendeten Datentypen in entsprechender Form in den Datenstrukturen dieser Klasse 
                                               wenn etwas schief geht wird false zurückgegeben. Bspw. könnte es sein, dass ein Variablenname zweimal auftritt oder ähnliches. _status 
                                               wird entsprechend angepasst */
    bool readStep();                        /* liest die aktuelle S-Umgebung ein */
    bool readTrigger();                     /* liest die aktuelle T-Umgebung ein */
    bool readDescription();                 /* liest Beschreibung eines Steps ein */

      
    std::string getArgument(std::string arg);                                   /* arg darf höchstens ein Argument beinhalten, gekennzeichnet durch eine []-Umschließung */ 
    std::string getOption(std::string opt);                                     /* opt darf höchstens eine Option beinhalten, gekennzeichnet durch eine {}-Umschließung */                     

    std::string getPath() const { return _filepath; }
private:
    std::string                         _filepath;          /* Pfad zur aktuellen Angriffsdatei */
    std::ifstream                       _file;
    Lang                               *_lang;              /* Die gelinkte Lang-Klasse, "in" die ausgewertet wird */
};

#endif