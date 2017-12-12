#ifndef LR_LCOMMAND_HPP
#define LR_LCOMMAND_HPP

/* Datenstruktur + Konstanten die einen Befehl in einer STEP-Umgebung einer .lang-Datei darstellen */

#include <iostream>
#include <string>
#include <vector>

/* Stellen Basisbefehle dar, keine Umgebungen! */

enum LCOMMAND{
    ASSIGNMENT,
    SEND,
    SLEEP
};

struct lcommand{
    int _cmd;
    std::vector<std::string> _args;
};

#endif