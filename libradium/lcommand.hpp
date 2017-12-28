#ifndef LR_LCOMMAND_HPP
#define LR_LCOMMAND_HPP

/* Datenstruktur + Konstanten die einen Befehl in einer STEP-Umgebung einer .lang-Datei darstellen */

#include <iostream>
#include <string>
#include <vector>

/* Stellen Basisbefehle dar, keine Umgebungen! */

enum LCOMMAND{
    ASSIGNMENT,         /* durch ein besonderes Zeichen spezifiziert -> lconst.hpp*/
    SEND,               /* siehe Command-Strings für ID-String*/
    PASS,               /* das PASS-Kommando ermöglicht einen STEP zu beenden und den nächsten einzulesen */
};

// Command-Strings
const std::string LCOMMAND_STR_SEND                 = "SEND("; 
const std::string LCOMMAND_ARG_PACKET               = "PACKET[";
const std::string LCOMMAND_STR_PASS                 = "PASS;"; 

/* im Falle, dass _cmd == LCOMMAND::ASSIGNMENT enthält _args abwechselnd Bezeichner/Wert und Operation */
struct lcommand{
    LCOMMAND _cmd;
    std::vector<std::string> _args;
};

#endif