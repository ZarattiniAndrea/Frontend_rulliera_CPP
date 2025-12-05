#include <iostream>
#include <string>
#include <thread>
#include "Persona.h"
#include "dbSet.h"
using namespace std;

//creo una funzione
void for_thread(Persona obj){
    cout << "Thread in esecuzione. " << endl;
    obj.saluta();
}

int main() {
    thread t1; //dichiaro un thread
    Persona p("Andrea", 26);
    dbSet(); //inizializzo il database
    cout << "Sono il main thread." << endl;
    t1 = thread(for_thread, p); //avvio il thread passando la funzione e l'oggetto
    t1.join(); //aspetto la fine del thread
    //p.saluta();

    return 0;
}