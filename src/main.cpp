#include <iostream>
#include <string>
#include <thread>
#include "Persona.h"
#include "dbSet.h"
#include "rulliera.h"
using namespace std;

//creo una funzione
/*
void for_thread(Rulliera rull){
    cout << "Passo la rulliera al thread." << endl;
    rull.pezzi_iniziali();
    rull.pezzi_presenti();
}
*/
int main() {
    thread t1; //dichiaro un thread
    Rulliera rull(1);
    dbSet(); //inizializzo il database
    cout << "Sono il main thread." << endl;
    cout << "Inizializzo i pezzi della rulliera." <<endl;
    rull.pezzi_iniziali();
    cout << "Avvio la lettura dei pezzi presenti sulla rulliera." << endl;
    rull.pezzi_presenti();
    //t1.join(); //aspetto la fine del thread
    //p.saluta();

    return 0;
}