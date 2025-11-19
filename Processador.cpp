
#include "Processador.hpp"
#include <ctime>

Processador::Processador(Memoria *memoria) : primeiro_nivel_memoria(memoria),
                                             rng(std::time(nullptr)), // Semente para o gerador
                                             dist(0, -1)              // Distribuição será reconfigurada em executar()
{
}

void Processador::executar(PadraoAcesso padrao, uint32_t tamanho_buffer,
                           uint32_t quantidade_acessos, int stride)
{

    uint32_t endereco_atual = 0;

    if (padrao == PadraoAcesso::ALEATORIO) {
        dist.param(std::uniform_int_distribution<uint32_t>::param_type(0, tamanho_buffer - 1));
    }

    for (uint32_t i = 0; i < quantidade_acessos; ++i) {
        if (padrao == PadraoAcesso::SEQUENCIAL) { // SEQUENCIAL
            endereco_atual = (endereco_atual + stride) % tamanho_buffer;
        } else { // ALEATORIO
            endereco_atual = dist(rng);
        }

        // Writing Rate
        if (rng() % 10 < 6) { // Simula uma alternância entre leitura (60%) e escrita (40%)
            total_ciclos_execucao += primeiro_nivel_memoria->ler(endereco_atual);
        } else {
            total_ciclos_execucao += primeiro_nivel_memoria->escrever(endereco_atual);
        }
    }
}