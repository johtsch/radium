#ifndef LR_LCONST_HPP
#define LR_LCONST_HPP

/* Konstanten die für .lang-Dateien und deren Auswertung wichtig sind */

#include <iostream>
#include <string>

/* UMGEBUNGEN: */

/* _B_ == "Beginning", also der einleitende Spezifizierer */
const std::string LANG_B_IMPLEMENTATION = "IMPLEMENTATION:";
const std::string LANG_B_VAR            = "VAR:";
const std::string LANG_B_STEP           = "S[";                     /* es ist "S[", da in einer STEP-Umgebung immer ein Argument übergeben werden muss*/
const std::string LANG_B_TRIGGER        = "T[";                     /* siehe LANG_B_STEP */
const std::string LANG_B_DESCRIPTION    = "DESCRIPTION:";
const std::string LANG_B_ASSEMBLE       = "ASSEMBLE[";
const std::string LANG_B_PACKETFILTER   = "FILTER:";

const std::string LANG_B_L2             = "L2:";
const std::string LANG_B_L3             = "L3[";
const std::string LANG_B_L4             = "L4[";
const std::string LANG_B_L5             = "L5[";
const std::string LANG_B_L6             = "L6[";
const std::string LANG_B_L7             = "L7[";

const std::string LANG_PACKET         = "PACKET[";

/* _B_ == "Ending", also der abschließende Spezifizierer */
const std::string LANG_E_IMPLEMENTATION = ":IMPLEMENTATION";
const std::string LANG_E_VAR            = ":VAR";
const std::string LANG_E_STEP           = ":S";
const std::string LANG_E_TRIGGER        = ":T";
const std::string LANG_E_DESCRIPTION    = ":DESCRIPTION";
const std::string LANG_E_ASSEMBLE       = ":ASSEMBLE";
const std::string LANG_E_PACKETFILTER   = ":FILTER";

const std::string LANG_E_L2             = ":L2";
const std::string LANG_E_L3             = ":L3";
const std::string LANG_E_L4             = ":L4";
const std::string LANG_E_L5             = ":L5";
const std::string LANG_E_L6             = ":L6";
const std::string LANG_E_L7             = ":L7";


/* BESONDERE ZEICHEN */

const char        LANG_C_COMMENT   = '#';
const char        LANG_C_ASSIGN    = '=';


/* BESONDERE STRINGS */
const std::string LANG_CTRL_CHAR    = "~";         // wird bei getNextWord()-Funktion zurückgegeben, wenn Zeichen wie '\t' das Word wärenconst std::string LANG_FEXT = ".lang";
const std::string LANG_FEXT         = ".lang";
const std::string LANG_EOF          = "EOF";
const std::string LANG_NOS          = "";          /* steht für LANG_NOString. Wird genutzt um bspw festzustellen, ob bereits eine Step-/Trigger-Umgebung eingelesen wurde. Sind diese nämlich leer, bedeutet das, dass 
                                                    keine solche Umgebung da ist, und eine beim nächsten Update eingelesen werden soll */


/* UNTERSTÜTZTE PROTOKOLLE */

const std::string LANG_PRO_NONE     = "NONE";       
const std::string LANG_PRO_ETHERNET = "ETH";
const std::string LANG_PRO_IPv4     = "IPv4";
const std::string LANG_PRO_ARP      = "ARP";
const std::string LANG_PRO_ICMP     = "ICMP";
const std::string LANG_PRO_TCP      = "TCP";
const std::string LANG_PRO_UDP      = "UDP";
const std::string LANG_PRO_DHCP     = "DHCP";

const int         LANG_COD_NONE     = 00;
const int         LANG_COD_ETHERNET = 11;
const int         LANG_COD_IPv4     = 21;
const int         LANG_COD_ARP      = 31;
const int         LANG_COD_ICMP     = 41;
const int         LANG_COD_TCP      = 51;
const int         LANG_COD_UPD      = 61;
const int         LANG_COD_DHCP     = 71;

#endif