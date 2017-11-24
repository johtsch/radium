#Dokumentation *Hier Namen für die Sprache einfügen* (Projektname Lang)

##Vorwort:
Diese "Sprache" und das damit in Verbindung stehende Programm sollen keine professionelle Lösung sein und erheben auch keineswegs den Anspruch vollständig zu sein. Viel mehr geht es mir hierbei darum zu testen, wie eine Sprache aussehen könnte mit der man netzwerkbasierte Angriffe durchführen kann. Der Umfang und auch die Funktionalität der implementierten Möglichkeiten sind abhängig von meinen Fähigkeiten und Erfahrungen und deswegen stark begrenzt.

Desweiteren werden in den einzelnen Abschnitten immer mal wieder Worte zur Implementierung auftauchen. Diese sind hauptsächlich für mich als Gedankenstütze gedacht und können, sollten sie wirr wirken ruhig ignoriert werden.

##Grundlegende Gedanken zur Umsetzung:
Die .lang-Dateien an sich sollen einfach in einem Texteditor geschrieben werden können. Möchte man nun eine dieser Dateien verwenden, um

##Grundstruktur:
Eine .lang-Datei besteht aus unterschiedlichen Bereichen. Ähnlich wie bei Markupsprachen, wie HTML oder LaTex werden diese Bereiche durch Codewörter gekennzeichnet. Bei Lang geschieht dies durch den "Abschnittsbezeichner" gefolgt von einem Doppelpunkt, um den Bereich zu öffnen. Geschlossen wird der Bereich durch einen Doppelpunkt gefolgt vom Abschnittsbezeichner. Beispielsweise sähe der Abschnitt für Variablendeklarationen wie folgt aus:

VAR:
    vartype varname1 varvalue1;
    vartype varname2 varvalue2;
:VAR

Geplant sind bis jetzt die Abschnittsbezeichner VAR, IMPLEMENTATION, S (steht für "Step"), T (steht für "trigger"), DESCRIPTION, ASSEMBLE, Ln (wobei n eine Zahl von 1-7 ist), PACKETFILTER sowie einzelne Bereiche für die einzelnen Header.

##Die Abschnittsbezeichner im Detail:

###>Der VAR-Bezeichner:
Wie bereits gesagt, erfolgt im VAR-Teil die Variablendeklaration. Dabei sollen bestimmte Datentypen festgelegt werden, die verwendet werden können. Wertzuweisungen sollen im IMPLEMENTATION-Teil, so wie direkt im VAR-Teil erfolgen können. 
Mögliche Datentypen könnten sein:
<ul>
    <li>HADDR       (MAC-Adressen [bitform][textform])</li>
    <li>IPADDR      (IPv4-Adressen [bitform][textform])</li>
    <li>PORT        (also 2-Byte-Werte die aber besonders behandelt werden sollen [HBO][NBO])</li>
    <li>HEADER      (dazu später noch ein paar genauere Worte)</li>
    <li>1B          (1-Byte-Werte)</li>
    <li>2B          (2-Byte-Werte)</li>
    <li>4B          (4-Byte-Werte)</li>
    <li>STRING</li>
</ul>


Allgemein denke ich ist es sinnvoll die Datentypen, wenn sie dann zu 

Zu den HEADER-Datentypen: Vielleicht wäre es sinnvoll sich noch ein Format für Headerdefinitionen in Lang auszudenken. Da, wie man in example.lang sehen kann, es für Header Felder geben soll, die dann zum Filtern genutzt werden, wäre es vielleicht sogar möglich ein Programm zu schreiben, dass eine Headerformat-Datei nimmt und eine C++-Headerdatei anfertigt in welcher eine Struktur nach vorbild des beschriebenen Headers angefertigt wird.
Oder aber man muss die einzelnen Header manuell implementieren und in C++-Code schreiben.