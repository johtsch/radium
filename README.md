# radium

<h1>Grundideen für Lang (Projektname für die Sprache)</h1>


<h2>Inhalt:</h2>

<ol>
    <li><a href="#vorwort">Vorwort</a>
    <li><a href="#grundlegende-gedanken-zur-umsetzung">Grundlegende Gedanken zur Umsetzung</a></li>
    <li><a href="#grundstruktur">Grundstruktur</a></li>
    <li>
        <a href="#die-abschnittsbezeichner-im-detail">Die Abschnittsbezeichner im Detail</a><br>
        <ol>
            <li><a href="#der-var-bezeichner">Der VAR-Bezeichner</a></li>
            <li><a href="#der-implementation-bezeichner">Der IMPLEMENTATION-Bezeichner</a></li>
            <li><a href="#der-s-bezeichner">Der S-Bezeichner</a></li>
            <li><a href="#der-t-bezeichner">Der T-Bezeichner</a></li>
            <li><a href="#der-description-bezeichner">Der DESCRIPTION-Bezeichner</a></li>
            <li><a href="#der-assemble-bezeichner">Der ASSEMBLE-Bezeichner</a></li>
            <li><a href="#der-l-bezeichner">Der L-Bezeichner</a></li>
            <li><a href="#der-packetfilter-bezeichner">Der PACKETFILTER-Bezeichner</a></li>
        </ol>
    </li>
    <li><a href="#die-radium-library">Die Radium-Library</a></li>
</ol>

<h2>Vorwort:</h2>
Diese "Sprache" und das damit in Verbindung stehende Programm sollen keine professionelle Lösung sein und erheben auch keineswegs den Anspruch vollständig zu sein. Viel mehr geht es mir hierbei darum zu testen, wie eine Sprache aussehen könnte mit der man netzwerkbasierte Angriffe durchführen kann. Der Umfang und auch die Funktionalität der implementierten Möglichkeiten sind abhängig von meinen Fähigkeiten und Erfahrungen und deswegen stark begrenzt.

Desweiteren werden in den einzelnen Abschnitten immer mal wieder Worte zur Implementierung auftauchen. Diese sind hauptsächlich für mich als Gedankenstütze gedacht und können, sollten sie wirr wirken ruhig ignoriert werden.

<a href="https://github.com/johtsch/radium/tree/master/langfiles">hier</a> klicken um beispielhafte .lang-Dateien zu sehen.

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

Die Datentypen sollen hardcoded sein in den C++-Dateien, weswegen neue Datentypen nicht erstellt werden können.

Zu den HEADER-Datentypen: Vielleicht wäre es sinnvoll sich noch ein Format für Headerdefinitionen in Lang auszudenken. Da, wie man in example.lang sehen kann, es für Header Felder geben soll, die dann zum Filtern genutzt werden, wäre es vielleicht sogar möglich ein Programm zu schreiben, dass eine Headerformat-Datei nimmt und eine C++-Headerdatei anfertigt in welcher eine Struktur nach vorbild des beschriebenen Headers angefertigt wird.
Oder aber man muss die einzelnen Header manuell implementieren und in C++-Code schreiben.

<h3>Der IMPLEMENTATION-Bezeichner</h3>
Der IMPLEMENTATION-Bezeichner dient dazu festzulegen, ab wann keine VAR-Umgebung mehr existieren kann bzw., um festzuhalten wo das Hauptprogramm definiert ist. 

<h3>Der S-Bezeichner</h3>
Genauer genommen müsste es der S[n]-Bezeichner heißen, wobei n eine natürliche Zahl beginnend bei 0. Diese Umgebung grenzt den Bereich ein in dem die aktuellen Befehle stehen. In diesem Abschnitt werden mithilfe von anderen Befehlsumgebungen und einzelnen Befehlen Aktionen beschrieben die ausgeführt werden sollen. Beispielsweise kann mit Hilfe von ASSEMBLE[n] und send(PACKET[n]) ein Paket zusammengesetzt und danach abgeschickt werden. Desweiteren soll die Möglichkeit bestehen durch setzen von geschweiften Klammern hinter dem Eingangsbezeichner die Intervallrate festgelegt werden, in welcher der Schritt wiederholt wird. Diese wird in ms angegeben. Steht keine Angabe dahinter wird der Schritt nur einmal durchgeführt. Bspw S[0]{50}::S[0] wäre ein Step der nach der Vollendung eines Durchgangs 50ms wartet und dann erneut startet.

Zur Implementation. Die Implementation in diesem Bereich soll prinzipiell so ablaufen, dass eine Klasse - die Langklasse - existiert, welche das Skript und seine Variablen repräsentiert. Die Variablen werden eingelesen und festgelegt. Die Befehle innerhalb eines Steps sind Funktionen innerhalb dieser Klasse, die es ermöglichen mit den Membervariablen der Klasse (die ja den im Skript festgelegten entsprechen) die gewünschten Aktionen durchzuführen.

<h3>Der T-Bezeichner</h3>
Auch hier wäre es besser T[n]-Bezeichner zu schreiben. In dieser Umgebung werden einfach die Filter festgelegt. Kommt nun ein Paket an, das alle Anforderungen des Filters erfüllt, wird der aktuelle Step beendet und mit dem nächsten fortgefahren.

<h3>Der DESCRIPTION-Bezeichner</h3>
Wird wahrscheinlich erst bei einem Programm mit GUI sinnvoll. In dieser Umgebung kann der aktuelle Schritt in Worten beschrieben werden, was eine Auswertung des Angriffs später erleichtern soll.

<h3>Der ASSEMBLE-Bezeichenr</h3>
Innerhalb der ASSEMBLE-Umgebung können mithilfe der L-Bezeichner (siehe nächster Abschnitt) Pakete erstellt werden. Auch die ASSEMBLE-Umgebung wird dabei durchindiziert (ASSEMBLE[n]). Dabei soll muss 0 <= n < 5 gelten. Diese Grenze steht erstmal, da die ASSEMBLE-Befehlsumgebung ein Paket erzeugt. ASSEMBLE[n] erzeugt das Paket PACKET[n], welches dann verschickt werden kann. Diese Begrenzung ist spontan gewählt und soll erstmal provisorisch gelten, da ich denke, dass mehr als 5 Pakete in einem Schritt zu versenden nicht nötig ist. Außerdem hoffe ich es so leichter implementieren zu können.

<h3>Der L-Bezeichner</h3>
Wie schon beim T-Bezeichner existiert auch beim L-Bezeichner ein Optionsfeld. Man könnte schreiben Ln[Protocol], wobei n eine Zahl zwischen 1 und 7 ist und den Layer im OSI-Schichtenmodell kennzeichnet. Protocol hingegen ist ebenfalls ein Pflichtfeld und gibt an, welcher Protokollheader auf dieser Schicht implementiert ist. Dadurch kann festgestellt werden, welche Datenfelder im Headerformat existieren und zugewiesen werden können. Durch die Zuweisung der einzelnen Layer können Datenpakete beschrieben werden. Innerhalb der ASSEMBLE-Umgebung können damit Pakete erstellt werden. In der PACKETFILTER-Umgebung können damit Paketfilter erstellt werden, die nur ausschlagen, wenn ein solches Paket empfangen wird.

<h3>Der PACKETFILTER-Bezeichner</h3>
Wie schon erwähnt kann mit der PACKETFILTER-Umgebung ein Paketfilter erstellt werden, der es ermöglicht eingehende Pakete zu erkennen. Dabei werden nur Felder betrachtet die in den einzelen L-Umgebungen zugewiesen werden -> wenn eine leere PACKETFILTER-Umgebung erstellt wird, löst der Trigger bei jedem einkommenden Paket aus. 

<h2>Die Radium-Library</h2>
Da ich es wahrscheinlich nicht vollbringen werde gleich das Radiumprogramm (so soll das Programm mit GUI heißen mit dem man Angriffe ausführen kann) zu programmieren, beginne ich damit den nötigen Code, um mit .lang-Dateien umgehen zu können, in eine Library zu packen. Mit dieser hoffe ich dann relativ einfach das richtige Programm schreiben zu können. Wenn alles gut läuft und ich die Zeit finde, wird es für die Library auch eine kleine Dokumentation geben. 
