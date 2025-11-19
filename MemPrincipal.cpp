#include "MemPrincipal.hpp"
#include <iostream>
#include <iomanip>

MemPrincipal::MemPrincipal(const std::string& nome, int latencia)
    : Memoria(nome, latencia, nullptr) {} // O próximo nível é sempre nulo

int MemPrincipal::ler(uint32_t endereco) {
    stats.leituras++;
    return latencia;
}

int MemPrincipal::escrever(uint32_t endereco) {
    stats.escritas++;
    return latencia;
}

void MemPrincipal::imprimir_estatisticas() {
    std::cout << "--- Estatisticas para " << nome << " ---" << std::endl;
    std::cout << "Acessos de Leitura: " << stats.leituras << std::endl;
    std::cout << "Acessos de Escrita: " << stats.escritas << std::endl;
    std::cout << std::endl;
}