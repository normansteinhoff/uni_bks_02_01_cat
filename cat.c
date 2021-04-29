
// --------------------------------------------------------------------------------------

// Autor:
    // Norman Steinhoff

// Jahr
    // 2021

// --------------------------------------------------------------------------------------

// Verwendete Abkürzungen:
    // Vor. == Voraussetzung
    // Eff. == Effekt
    // Erg. == Ergebnis
    // Anm. == Anmerkung

// --------------------------------------------------------------------------------------

// Standard_Header einbinden
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>

// Konstanten
#define BUFFER_SIZE 1024
#define OK 0
#define ERROR 1

// Vor.:
// *    Die Ziel_Datei mit dem Deskriptor { destinationFileDescriptor } muss zum schreiben geöffnet sein.
// *    { byteCount <= sizeof(buffer) }
// Eff.:
// *    Die ersten { byteCount } vielen Bytes von { buffer } werden in die Datei mit dem
//      Datei-Deskriptor { destinationFileDescriptor } geschrieben.
// Erg.:
// -
// Anm.:
// -
void write_to_file(int destinationFileDescriptor, char buffer[], ssize_t byteCount) {
    
    // Anzahl geschriebener Bytes initialisieren.
    ssize_t bytesWritten = 0;

    // solange versuchen in die Ziel_Datei zu schreiben, bis alle Bytes geschieben wurden
    char *startPos;
    int bytesToWrite;
    while (bytesWritten < byteCount) {
        startPos = buffer + bytesWritten;
        bytesToWrite = byteCount - bytesWritten;
        
        // hier kann es passieren, das weniger Bytes geschrieben werden als angefragt wurde, daher die Schleife.
        bytesWritten += write(destinationFileDescriptor, startPos, bytesToWrite);
    }
}

// Vor.:
// *    Die Quell_Datei mit dem Deskriptor { sourceFileDescriptor } muss zum lesen geöffnet sein.
// *    Die Ziel_Datei mit dem Deskriptor { destinationFileDescriptor } muss zum schreiben geöffnet sein.
// Eff.:
// *    Schreibt den Inhalt der Quell_Datei mit dem Deskriptor { sourceFileDescriptor }
//      in die Ziel_Datei mit dem Deskriptor { destinationFileDescriptor }.
// Erg.:
// -
// Anm.:
// -
void copy_file(int sourceFileDescriptor, int destinationFileDescriptor) {

    // benötigte Variablen deklarieren
    char buffer[BUFFER_SIZE];
    ssize_t bytesRead;

    while (true) {

        // Aus Quell_Datei lesen und Anzahl gelesener Bytes registrieren.
        // Es wird immer mindestens ein Byte gelesen, außer das Ende der Quell_Datei wurde erreicht.
        bytesRead = read(sourceFileDescriptor, buffer, BUFFER_SIZE);

        // Anzahl der gelesenen Bytes überprüfen
        if (0 == bytesRead) { // Wenn keine Bytes gelesen wurden, ist das Ende der Quell_Datei erreicht.

            //Abbruch
            return;
        }

        // Gelesene Bytes in Ziel_Datei schreiben.
        write_to_file(destinationFileDescriptor, buffer, bytesRead);
    }
}

// Vor.:
// *    Der Wert von { file_count } entspricht der Anzahl der Elemente in { file_list }.
// Eff.:
// *    Die in { file_list } angegebenen Dateien werden der Reihe nach verarbeitet.
// Erg.:
// *    Falls ein Fehler auftrat ist { ERROR }  geliefert.
// *    Ansonsten ist { OK } geliefert.
// Anm.:
// *    { OK } und { ERROR } sind Makro_Konstanten.
int process_files(int file_count, char *file_list[]) {

    // Ergebnis initialisieren
    int result = OK;

    // Datei_Deskriptoren deklarieren bzw. initialisieren
    int sourceFileDescriptor;
    int destinationFileDescriptor = STDOUT_FILENO;

    // über angegebene Dateien iterieren
    for (int a = 1; a < file_count; ++a) {

        // versuchen die aktuelle Datei zu öffnen
        sourceFileDescriptor = open(file_list[a], O_RDONLY);

        // prüfen, ob die Datei geöffnet wurde
        if (-1 == sourceFileDescriptor) { // Die Datei konnte nicht geöffnet werden.

            // aufgetretenen Fehler registrieren
            result = ERROR;

            // Fehlermeldung ausgeben
            perror(file_list[a]);
        }
        else { // Die Datei wurde geöffnet.

            // Datei kopieren
            copy_file(sourceFileDescriptor, destinationFileDescriptor);

            // Datei schließen
            close(sourceFileDescriptor);
        }
    }
        
    // Ergebnis liefern
    return result;
}

// Vor.:
// -
// Eff.:
// *    Miau :)
// Erg.:
// *    Falls ein Fehler auftrat ist { ERROR }  geliefert.
// *    Ansonsten ist { OK } geliefert.
// Anm.:
// *    { OK } und { ERROR } sind Makro_Konstanten.
int main(int argc, char *argv[]) {

    // Ergebnis initialisieren
    int result = OK;

    // Die Anzahl der Programm_Argumente überprüfen
    if (argc < 2) {

        // direkt mit der Standardeingabe arbeiten
        copy_file(STDIN_FILENO, STDOUT_FILENO);
    }
    else {
        
        // angegebene Dateien verarbeiten
        result = process_files(argc, argv);
    }

    // Ergebnis liefern
    return result;
}
