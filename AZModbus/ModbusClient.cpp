#ifdef _WIN32 //se siamo su Windows
    //typedef int ssize_t; // su Windows ssize_t non è definito, quindi lo definisco come int
    #include <winsock2.h>
    #pragma comment(lib, "ws2_32.lib") //linko la libreria di Winsock
#else //se siamo su Linux
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <unistd.h>
#endif
//Se siamo su Windows, definisco sock_ come SOCKET, altrimenti come int. 
#ifdef _WIN32
    SOCKET sock_;
#else
    int sock_;
#endif
#include "ModbusClient.h"
#include <string>
#include <iostream>
#include <cstdint>
#include <cstddef>
#include <vector>
using namespace std; 
#define AF_INET 2
#define SOCK_STREAM 1

ModbusClient::ModbusClient(const string& ipAddress, uint16_t port):
    ipAddress_(ipAddress),
    port_(port),
    sock_(-1),
    connected_(false) {}

ModbusClient::~ModbusClient() {
    disconnetti();
}

// CONNESSIONE AL SERVER MODBUS

bool ModbusClient::connetti() {
    cout << "Connessione al server Modbus " << ipAddress_ << " sulla porta " << port_ << "..." << endl;
    #ifdef _WIN32 // Se stiamo usando Windows
        WSADATA wsaData; // Struttura dei dati di Winsock (necessaria per l'inizializzazione)
        WSAStartup(MAKEWORD(2, 2), &wsaData); // Inizializzo Winsock
    #endif

    sock_ = ::socket(AF_INET, SOCK_STREAM, 0); // Creo il socket. AF_INET --> IPv4,
                                             // SOCK_STREAM --> i pacchetti sono di tipo stream (TCP)
    if(sock_ < 0){
        cerr << "Errore nella creazione della socket" << endl;
        return false;
    }
    if(sock_ == INVALID_SOCKET){
        cerr << "Errore nella creazione della socket: " << WSAGetLastError() << endl;
        return false;
    }
    
    struct sockaddr_in serverAddr; // Struttura dell'indirizzo del server
    serverAddr.sin_family = AF_INET; // L'indirizzo del server è di tipo IPv4
    serverAddr.sin_port = htons(port_); // Imposto la porta del server (htons converte in formato network
    serverAddr.sin_addr.s_addr = inet_addr(ipAddress_.c_str()); // Imposto l'indirizzo IP del server

    // Connessione al server
    if(::connect(sock_, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0){ // a connect passo la socket, 
        // l'indirizzo del server e la dimensione dell'indirizzo
        cerr << "Errore nella connessione al server" << endl;
        return false;
    }

    cout << "Connessione al server Modbus avvenuta con successo!" << endl;
    connected_ = true;
    return true;
}

// DISCONNESSIONE DAL SERVER MODBUS

void ModbusClient::disconnetti() {
    if(connected_){
        #ifdef _WIN32 // Se stiamo usando Windows
            closesocket(sock_); // Chiudo la socket
            WSACleanup(); // Pulisco Winsock
        #else // Se usiamo linux
            close(sock_); // Chiudo la socket
        #endif
        connected_ = false;
    }
}

bool ModbusClient::èConnesso() const { // il const indica che questo metodo non modifica lo stato dell'oggetto
    return connected_;
}

// Lettura dei coil
bool ModbusClient::readCoils(uint16_t startAddress, uint16_t quantity, vector<bool>& coils){
    if(!connected_){
        cerr << "Errore connessione al server Modbus con indirizzo " << ipAddress_ << " sulla porta " << port_ << endl;
        return false; 
    }
    // Costruzione del MBAP (Modbus Application Protocol Header)
    /*
    FORMATO DEL MBAP:
    - Transaction Identifier: 2 byte (identificativo della transazione, abbina richieste e risposte)
    - Protocol Identifier: 2 byte (identifica il protocollo)
    - Length: 2 byte (lunghezza del messaggio che segue)
    - Unit Identifier: 1 byte (identifica il dispositivo slave)
    */
    unsigned char mbap[7];
    mbap[0] = 0x0000; // Id alto
    mbap[1] = 0x0001; // Id basso
    mbap[2] = 0x0000; // Protocol Id alto
    mbap[3] = 0x0000; // Protocol Id basso
    mbap[4] = 0x0000; // Lunghezza alto
    mbap[5] = 0x0006; // Lunghezza basso (6 byte seguono: 1 unit ide e 5 PDU)
    mbap[6] = 0x01;
    // Costruzione PDU: function code + start address + quantità (PDU = Protocol Data Unit, sostanzialmente il pacchetto di dati)
    /* FORMATO DEL MESSAGGIO di richiesta Read Coils: 
    - Function Code: 1 byte (0x01 per Read Coils)
    - Starting Address: 2 byte
    - Quantity of Coils: 2 byte
    */
    unsigned char pdu[5]; // PDU di 5 byte (Protocol Data Unit)
    pdu[0] = 0x01; // Function Code per Read Coils
    // Byte alto dell'indirizzo di partenza: 
    // shift a destra di startAddress, ovvero i primi 8 bit vengono spostati a destra, 
    // perdendo tutti gli altri. 
    // & 0xFF serve per assicurarsi che rimanga solo l'ultimo byte (8 bit)
    pdu[1] = (startAddress >> 8) & 0xFF; 
    pdu[2] = startAddress & 0xFF; // Byte basso dell'indirizzo di partenza
    pdu[3] = (quantity >> 8) & 0xFF; // Byte alto della quantità di coil da leggere
    pdu[4] = quantity & 0xFF; // Byte basso della quantità di coil da leggere

    // Prima di inviare i dati, mi assicuro che la socket sia valida
    if(sock_ == INVALID_SOCKET){
        cerr << "Socket non valida" << endl;
        return false;
    }

    // Costruzione del messaggio completo (MBAP + PDU)
    unsigned char message[sizeof(mbap) + sizeof(pdu)]; // definiso la dimensione totale
    memcpy(message, mbap, sizeof(mbap)); // copio il MBAP all'inizio del messaggio 
    memcpy(message + sizeof(mbap), pdu, sizeof(pdu)); // Copio il PDU a seguire

    // Invio della richiesta al server
    int totalbytes = 0; // per tenere traccia del numero totale di byte inviati
    for(; totalbytes < sizeof(message);){
        int bytesInviati = send(sock_, reinterpret_cast<const char*>(message), sizeof(message), 0); // reinterpret_cast serve per convertire il puntatore a pdu in un puntatore a char
        if(bytesInviati < 0){
            cerr << "Errore nell'invio della richiesta di Read Coils al server: " << endl;
            int err = WSAGetLastError();
            cerr << "Codice errore: " << err << endl;
            return false;
        }
        totalbytes += bytesInviati;
    }
    cout << "Richista di read coils inviata con successo." << endl;


    // Ricezione della risposta dal server
    // La risposta ha il seguente formato:
    /* FORMATO DEL MESSAGGIO di risposta Read Coils:
    - Function Code: 1 byte (0x01 per Read Coils)
    - Byte Count: 1 byte (numero di byte dei dati)
    - Stato dei Coil: N byte (ogni bit è lo stato di un coil)
    */
    unsigned char responseHeader[7]; // MBAP header
    int bytesRicevutiHeader = recv(sock_, reinterpret_cast<char*>(responseHeader), sizeof(responseHeader), 0);
    if(bytesRicevutiHeader != sizeof(responseHeader)){
        cerr << "Errore nella ricezione dell'header del messaggio di risposta dal server" << endl;
        return false;
    }
    /*
    Lo shifting di responseHeader di 8 bit serve per fare "spazio" al successivo byte basso. 
    La successiva operazione di OR fa in modo di mantenere i bit a 1 nella parte alta del byte intero (primi 8 bit) e di inserire ciò 
    che serve nella parte bassa del byte intero (successivi 8 bit), in questo modo combinandoli.
    */
    int TransactionID = (responseHeader[0] << 8) | responseHeader[1];
    int ProtocolID = (responseHeader[2] << 8) | responseHeader[3];
    int Lunghezza = (responseHeader[4] << 8) | responseHeader[5];
    int UnitID = responseHeader[6];

    // Ora leggo il PDU della risposta
    // - Function Code: 1 byte
    // - Byte Count: 1 byte
    cout << "Ricevo la risposta dal server..." << endl;
    unsigned char header[2]; // Function Code + Byte Count
    int bytesRicevuti = recv(sock_, reinterpret_cast<char*>(header), sizeof(header), 0);
    if(bytesRicevuti != sizeof(header)){
        cerr << "Errore nella ricezione dell'header del messaggio di risposta dal server" << endl;
        return false;
    }
    
    unsigned char byteCount = header[1]; // Numero di byte dei dati
    vector<unsigned char> data(byteCount);
    bytesRicevuti = recv(sock_, reinterpret_cast<char*>(data.data()), static_cast<int>(byteCount), 0);
    if(bytesRicevuti != static_cast<int>(byteCount)){
        cerr << "Errore nella ricezione dei dati dal server" << endl;
        return false;
    }

    // Leggo lo stato dei coil dai dati ricevuti 
    coils.clear(); // Pulisco il vettore dei coil perchè potrebbe contenere dati vecchi
    for(uint16_t i = 0; i < quantity; i++){
        uint16_t byteIndex = i / 8; // Indice del byte nel vettore data
        uint16_t bitIndex = i % 8; // Indice del bit nel byte
        bool coilState = data[byteIndex] & (1 << bitIndex); 
        /*
        data - array di byte ricevuti dal server
        byteIndex - indice del byte corrente
        bitIndex - indice del bit corrente
        (1 << bitIndex) - prendo il numero 1 e lo sposta a sinistra di bitIndex posizioni, 
        creando così una maschera che, quando applicata con l'operatore AND (&) al byte corrente,
        mi permette di isolare il bit di interesse.
        */
        coils.push_back(coilState); // Aggiungo lo stato del coil al vettore
    }
    return true;
}
