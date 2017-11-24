<h1>Dokumentation *Hier Namen für die Sprache einfügen* (Projektname Lang)</h1>

<h2>Vorwort:</h2>
Diese "Sprache" und das damit in Verbindung stehende Programm sollen keine professionelle Lösung sein und erheben auch keineswegs den Anspruch vollständig zu sein. Viel mehr geht es mir hierbei darum zu testen, wie eine Sprache aussehen könnte mit der man netzwerkbasierte Angriffe durchführen kann. Der Umfang und auch die Funktionalität der implementierten Möglichkeiten sind abhängig von meinen Fähigkeiten und Erfahrungen und deswegen stark begrenzt.

Desweiteren werden in den einzelnen Abschnitten immer mal wieder Worte zur Implementierung auftauchen. Diese sind hauptsächlich für mich als Gedankenstütze gedacht und können, sollten sie wirr wirken ruhig ignoriert werden.

<h2>Grundlegende Gedanken zur Umsetzung:</h2>
Die .lang-Dateien an sich sollen einfach in einem Texteditor geschrieben werden können. Möchte man nun eine dieser Dateien verwenden, um einen Angriff durchzuführen benutzt man dazu das Programm. Ich nenne es provisorisch einfach mal "Radium", weil ich kurz bevor ich mit dem ausdenken dieser Sprache ein Bild von der Struktur eines Radium Atoms nach Bohr gesehen habe. Dieses Programm soll nun eine .lang-Datei einlesen können und zur Laufzeit auswerten können. Deswegen ist es auch leichter, wenn Variablen zu Beginn deklariert werden. Umgesetzt werden soll Radium mit C++ (& libtins) und Qt. Es soll eine Klasse existieren (hier provisorisch als Langklasse bezeichnet). Diese besteht aus Listen für jeden Variablentypen und Header der implementiert ist. Der Variablenteil der .lang-Datei wird nun ausgewertet und entsprechende Einträge in der Liste gemacht. Vielleicht bietet sich auch eine Map an, da ansonsten dass Problem besteht Listeneinträge ihren Variablen in der .lang-Datei zuzuweisen. Einzelne Datentypen, wie beispielsweise der IPADDR-Datentyp (<a href="#der-var-bezeichner">Variablentypen</a>), sollen dabei zwei Werte im C++-Code haben. Einmal den Bitwert, mit dem gearbeitet werden soll und dann noch den Stringwert, dessen Nutzen sich mir jetzt noch nicht erschließt aber das wird sich noch ergeben.

<h2>Grundstruktur:</h2>
Eine .lang-Datei besteht aus unterschiedlichen Bereichen. Ähnlich wie bei Markupsprachen, wie HTML oder LaTex werden diese Bereiche durch Codewörter gekennzeichnet. Bei Lang geschieht dies durch den "Abschnittsbezeichner" gefolgt von einem Doppelpunkt, um den Bereich zu öffnen. Geschlossen wird der Bereich durch einen Doppelpunkt gefolgt vom Abschnittsbezeichner. Beispielsweise sähe der Abschnitt für Variablendeklarationen wie folgt aus:

VAR:
    vartype varname1 varvalue1;
    vartype varname2 varvalue2;
:VAR

Geplant sind bis jetzt die Abschnittsbezeichner VAR, IMPLEMENTATION, S (steht für "Step"), T (steht für "trigger"), DESCRIPTION, ASSEMBLE, Ln (wobei n eine Zahl von 1-7 ist), PACKETFILTER sowie einzelne Bereiche für die einzelnen Header.

<h2>Die Abschnittsbezeichner im Detail:<h2>
<h3>Der VAR-Bezeichner:</h3>
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