#include <iostream>
#include "rulliera.h"
#include "ModbusClient.h"
using namespace std;

Rulliera::Rulliera(const int id):
    id_rulliera(id),
    pezzi(0),
    client_socket(-1),
    front_result(false),
    back_result(false),
    front_prectoggle(false),
    back_prectoggle(false),
    current_front_toggle(false),
    current_back_toggle(false),
    client(new ModbusClient("192.168.200.170", 502)) {}

Rulliera::~Rulliera() {
    // Distruttore
    elimina_rulliera();
}

// Lettura toggle iniziale dei pezzi 
void Rulliera::pezzi_iniziali() {
    // Mi collego al server Modbus
    if(client->connetti()){
        vector<bool> coils; // Vettore per memorizzare i coil con lunghezza dinamica
        if(client->readCoils(0, 2, coils)){
            if(!coils.empty()){
                front_result = coils[0]; // coil anteriore
                back_result = coils[1]; // coil posteriore
                // Salvo lo stato iniziale dei toggle
                front_prectoggle = front_result;
                back_prectoggle = back_result;
            }
        }
    }else{
        cerr << "Connessione al server Modbus fallita!" << endl;
    }
    client->disconnetti();
}

void Rulliera::pezzi_presenti() {
    // Mi collego al server Modbus
    if(client->connetti()){
        while(true){
            vector<bool> coils; // vettore per memorizzare i coil
            if(client->readCoils(0, 2, coils)){
                if(!coils.empty()){
                    front_result = coils[0]; // coil anteriore
                    back_result = coils[1]; // coil posteriore
                    cout << "Rulliera " << id_rulliera << " - Valore frontale: " << front_result << ", Valore posteriore: " << back_result << endl;
                    current_front_toggle = front_result;
                    current_back_toggle = back_result;
                    if(current_front_toggle != front_prectoggle){
                        front_prectoggle = current_front_toggle;
                        pezzi++;
                    }
                    if(current_back_toggle != back_prectoggle){
                        back_prectoggle = current_back_toggle;
                        pezzi--;
                    }
                }
            }
        }
    }
    client->disconnetti();
}

void Rulliera::elimina_rulliera() {
    if(client != nullptr){
        delete client;
        client = nullptr;
    }
}