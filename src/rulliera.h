#ifndef RULLIERA_H
#define RULLEIRA_H
#include "AZModbus/ModbusClient.h"

class Rulliera {
    public:
        Rulliera(const int id);
        ~Rulliera();

        void pezzi_iniziali();
        void pezzi_presenti();
        void elimina_rulliera();
    
    private:
        int id_rulliera;
        int pezzi;
        int client_socket;
        bool front_result;
        bool back_result;
        bool front_prectoggle;
        bool back_prectoggle;
        bool current_front_toggle;
        bool current_back_toggle;
        ModbusClient* client; // Puntatore al client Modbus
};
#endif // RULLIERA_H