#ifndef PERSONA_H
#define PERSONA_H
#include <string>

class Persona {
    private: 
        std::string nome;
        int eta;

    public:
        Persona(std::string n, int e);
        void saluta() const;
        void setEta(int e);
        int getEta() const;
        void setNome(const std::string& n);
        std::string getNome() const;
};
#endif