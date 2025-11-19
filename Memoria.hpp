#ifndef MEMORIA_HPP
#define MEMORIA_HPP

#include <string>
#include <cstdint>

// Estatísticas de cada nível de memória
struct Estatisticas {
    uint64_t leituras = 0;
    uint64_t escritas = 0;
    uint64_t hits_leitura = 0;
    uint64_t hits_escrita = 0;
    uint64_t misses_leitura = 0;
    uint64_t misses_escrita = 0;
};

class Memoria {
protected:
    std::string nome;
    int latencia;
    Memoria* proximo_nivel;
    Estatisticas stats;

public:
    Memoria(const std::string& nome, int latencia, Memoria* proximo_nivel = nullptr)
        : nome(nome), latencia(latencia), proximo_nivel(proximo_nivel) {}

    virtual ~Memoria() {}

    virtual int ler(uint32_t endereco) = 0;
    virtual int escrever(uint32_t endereco) = 0;
    virtual void imprimir_estatisticas() = 0;

    std::string get_nome() const { return nome; }
};
#endif