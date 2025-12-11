#pragma once
#ifndef MODBUSCLIENT_H
#define MODBUSCLIENT_H
#ifdef _WIN32
    #include <winsock2.h>
    #pragma comment(lib, "ws2_32.lib") //linko la libreria di Winsock
    typedef SOCKET socketType;
#else
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    typedef int socketType;
#endif
#include <cstdint>
#include <vector>
#include <string>
using namespace std;

class ModbusClient {
    public:
        ModbusClient(const string& ipAddress, uint16_t port); // questo è il costruttore
        ~ModbusClient(); // questo è il distruttore (viene chiamato quando l'oggetto viene distrutto)

        bool connetti();
        void disconnetti();
        bool èConnesso() const;
        bool readCoils(uint16_t startAddress, uint16_t quantity, vector<bool>& coils);
    
    private:
        string ipAddress_;
        uint16_t port_;
        socketType sock_;
        bool connected_;
};
#endif // MODBUSCLIENT_H