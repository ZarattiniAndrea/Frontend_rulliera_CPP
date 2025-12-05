#include <iostream>
#include <string>
#include "Persona.h"
using namespace std;

Persona::Persona(string n, int e) : nome(n), eta(e) {} //qui si inizializzano i membri

void Persona::saluta() const {
    cout << "Ciao, mi chiamo " << nome << " e ho " << eta << " anni." << endl;
}

void Persona::setEta(int e) {
    eta = e;
}

int Persona::getEta() const {
    return eta; 
}

void Persona::setNome(const string& n) {
    nome = n;
}

string Persona::getNome() const {
    return nome;
}