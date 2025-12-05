#include <string>
#include <iostream>
extern "C" {
    #include "sqlite3.h"
}
#include "dbSet.h"
using namespace std;

int show(void* NotUsed, int argc, char** argv, char** azColName){
    for(int i = 0; i < argc; i++){
        cout << azColName[i] << ": " << (argv[i] ? argv[i] : "NULL") << endl;
    }
    cout << endl;
    return 0;
}

void dbSet(){
    // Creo un database
    sqlite3 *db;
    char *errMsg;
    int exit = sqlite3_open("modbus_toggle.db", &db); // apro (o creo se non esiste) il database
    if(exit){
        cerr << "Errore nell'apertura del database: " << sqlite3_errmsg(db) << endl;
        return;
    }
    cout << "Database aperto con successo!" << endl;
    /*
    Formato dei dati:
    - id_rulliera: identificativo della rulliera (intero, chiave primaria)
    - pezzi: indica il numero di pezzi presenti sulla rulliera (intero)
    - time: indica il timestamp dell'ultima modifica (TIMESTAMP)
    - stato: indica lo stato della rulliera (testo, es: OK, SOTTO SOGLIA, VUOTA)
    - mittente: numero della rulliera che invia l'ordine (intero)
    - quantità: numero di pezzi richiesti nell'ordine (intero)
    - stato_ordine: stato dell'ordine (testo, es: IN PAUSA, IN CORSO, RICEVUTO, TERMINATO)
    */

    // Creo la tabella RULLIERE se non esiste già
    const char *sql = "CREATE TABLE IF NOT EXISTS rulliera("
                      "id_rulliera INTEGER PRIMARY KEY,"
                      "pezzi INTEGER,"
                      "time TIMESTAMP DEFAULT CURRENT_TIMESTAMP,"
                      "stato TEXT,"
                      "mittente INTEGER,"
                      "quantita INTEGER,"
                      "stato_ordine TEXT"
                      ");";
    exit = sqlite3_exec(db, sql, nullptr, 0, &errMsg);
    if(exit != SQLITE_OK){
        cerr << "Errore nella creazione della tabella: " << errMsg << endl;
        sqlite3_free(errMsg); // libero la memoria allocata per il messaggio di errore
    }else{
        cout << "Tabella RULLIERE creata con successo (o già esistente)." << endl;
    }

    sql = "INSERT INTO rulliera VALUES (1, 0, CURRENT_TIMESTAMP, 'VUOTA', 0, 0, 'TERMINATO');";
    exit = sqlite3_exec(db, sql, nullptr, 0, &errMsg);
    if(exit != SQLITE_OK){
        cerr << "Errore nell'inserimento dei dati: " << errMsg << endl;
        sqlite3_free(errMsg); // libero la memoria allocata per il messaggio di errore
    }else{
        cout << "Dati inseriti con successo nella tabella rulliere." << endl;
    }

        //Controllo che la tabella sia stata creata correttamente
    sql = "SELECT * FROM rulliera";
    exit = sqlite3_exec(db, sql, show, 0, &errMsg);
    if(exit != SQLITE_OK){
        cerr << "Errore nella selezione dei dati: " << errMsg << endl;
        sqlite3_free(errMsg); // libero la memoria allocata per il messaggio di errore
    }else{
        cout << "Selezione dei dati eseguita con successo." << endl;
    }

    cout << "Chiusura del database." << endl;

    sqlite3_close(db); // chiudo il database

    return;

}