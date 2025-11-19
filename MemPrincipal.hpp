#ifndef MEM_PRINCIPAL_HPP
#define MEM_PRINCIPAL_HPP

#include "Memoria.hpp"

class MemPrincipal : public Memoria {
public:
    MemPrincipal(const std::string& nome, int latencia);
    int ler(uint32_t endereco) override;
    int escrever(uint32_t endereco) override;
    void imprimir_estatisticas() override;
};

#endif