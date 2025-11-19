#include <iostream>
#include <vector>
#include <memory>
#include "Processador.hpp"
#include "Cache.hpp"
#include "MemPrincipal.hpp"

// g++ main.cpp Processador.cpp Cache.cpp MemPrincipal.cpp -o simulador -std=c++17


int main()
{
    try
    {
        // --- Configuração da Hierarquia de Memória ---
        std::vector<std::unique_ptr<Memoria>> hierarquia;
        uint16_t tamLinha = 64; // Tamanho da Linha

        // Memória Principal
        hierarquia.push_back(std::make_unique<MemPrincipal>("RAM", 200));

        // Cache L3
        hierarquia.push_back(std::make_unique<Cache>("L3", 16, hierarquia.back().get(),
                                                     512, 4, tamLinha, PoliticaEscrita::WRITE_BACK)); // L3: 512KB, 4-way, 64B/linha, WB
                                                     // Name, Latency, Next Level, Size (KB), Associativity, Line Size (BYTE), Policy

        // Cache L2
        hierarquia.push_back(std::make_unique<Cache>("L2", 4, hierarquia.back().get(),
                                                     128, 4, tamLinha, PoliticaEscrita::WRITE_BACK)); // L2: 128KB, 4-way, 64B/linha, WB

        // Cache L1
        hierarquia.push_back(std::make_unique<Cache>("L1", 1, hierarquia.back().get(),
                                                     32, 4, tamLinha, PoliticaEscrita::WRITE_BACK)); // L1: 32KB, 4-way, 64B/linha, WB

        Memoria *primeiro_nivel = hierarquia.back().get();

        // --- Configuração do Processador e Simulação ---
        Processador processador(primeiro_nivel);

        PadraoAcesso padrao = PadraoAcesso::ALEATORIO; // Access Standard
        uint32_t tamanho_buffer = 1024000; // Buffer
        uint32_t quantidade_acessos = 10000000; // Number of Accesses
        int stride = 32; // Stride

        std::cout << "\n--- Executando simulacao com padrao de acesso ---"
                  << (padrao == PadraoAcesso::SEQUENCIAL ? "SEQUENCIAL" : "ALEATORIO")
                  << "..." << std::endl;

        processador.executar(padrao, tamanho_buffer, quantidade_acessos, stride);

        std::cout << "\n--- Resultados da Simulacao ---" << std::endl;
        std::cout << "Tempo total de execucao: " << processador.get_total_ciclos() << " ciclos." << std::endl;
        std::cout << "--------------------------------\n"
                  << std::endl;

        // Imprimir estatísticas de todos os níveis
        for (int i = hierarquia.size() - 1; i >= 0; --i)
        {
            hierarquia[i]->imprimir_estatisticas();
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Erro: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}