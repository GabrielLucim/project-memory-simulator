#ifndef CACHE_HPP
#define CACHE_HPP

#include "Memoria.hpp"
#include <vector>
#include <list>

enum class PoliticaEscrita
{
    WRITE_THROUGH,
    WRITE_BACK
};

struct LinhaCache
{
    bool valida = false;
    bool suja = false; // "dirty bit" - write-back
    uint32_t tag = 0;
    uint64_t ultimo_acesso = 0; // Para a política LRU
};

class Cache : public Memoria
{
private:
    int associatividade;
    int tamanho_linha;
    int num_conjuntos;
    PoliticaEscrita politica_escrita;

    std::vector<std::vector<LinhaCache>> conjuntos;

    int bits_offset;
    int bits_indice;
    uint64_t ciclo_atual = 0;

    // Helpers
    void extrair_partes_endereco(uint32_t endereco, uint32_t &tag, uint32_t &indice);
    int encontrar_lru(uint32_t indice);
    void trazer_bloco_para_cache(uint32_t endereco, uint32_t tag, uint32_t indice, int& latencia_acumulada, bool is_write);

public:
    Cache(const std::string &nome, int latencia, Memoria *proximo_nivel,
          int tamanho_kb, int associatividade, int tamanho_linha_bytes, PoliticaEscrita politica);

    int ler(uint32_t endereco) override;
    int escrever(uint32_t endereco) override;
    void imprimir_estatisticas() override;

    int get_tamanho_kb() const;
};

#endif