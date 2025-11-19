#ifndef PROCESSADOR_HPP
#define PROCESSADOR_HPP

#include "Memoria.hpp"
#include <vector>
#include <random>

enum class PadraoAcesso { SEQUENCIAL, ALEATORIO };

class Processador {
private:
    Memoria* primeiro_nivel_memoria;
    uint64_t total_ciclos_execucao = 0;

    std::mt19937 rng;
    std::uniform_int_distribution<uint32_t> dist;

public:
    Processador(Memoria* memoria);
    void executar(PadraoAcesso padrao, uint32_t tamanho_buffer, 
                  uint32_t quantidade_acessos, int stride = 1);
    
    uint64_t get_total_ciclos() const { return total_ciclos_execucao; }
};

#endif