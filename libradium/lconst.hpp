#ifndef LR_LCONST_HPP
#define LR_LCONST_HPP

/* Konstanten die für .lang-Dateien und deren Auswertung wichtig sind */

#include <iostream>
#include <string>

const std::string LANG_FEXT = ".lang";
const std::string LANG_EOF = "EOF";
const std::string LANG_NOS = "";            /* steht für LANG_NOString. Wird genutzt um bspw festzustellen, ob bereits eine Step-/Trigger-Umgebung eingelesen wurde. Sind diese nämlich leer, bedeutet das, dass 
                                               keine solche Umgebung da ist, und eine beim nächsten Update eingelesen werden soll */

/* _B_ == "Beginning", also der einleitende Spezifizierer */
const std::string LANG_B_IMPLEMENTATION = "IMPLEMENTATION:";
const std::string LANG_B_VAR = "VAR:";
const std::string LANG_B_STEP = "S[";                   /* es ist "S[", da in einer STEP-Umgebung immer ein Argument übergeben werden muss*/
const std::string LANG_B_TRIGGER = "T[";                /* siehe LANG_B_STEP */
/* _B_ == "Ending", also der abschließende Spezifizierer */
const std::string LANG_E_IMPLEMENTATION = ":IMPLEMENTATION";
const std::string LANG_E_VAR = ":VAR";
const std::string LANG_E_STEP = ":S";
const std::string LANG_E_TRIGGER = ":T";

//besondere Zeichen
const char LANG_C_COMMAND = '#';


const std::string LANG_CTRL_CHAR = "*";         // wird bei getNextWord()-Funktion zurückgegeben, wenn Zeichen wie '\t' das Word wären


#endif