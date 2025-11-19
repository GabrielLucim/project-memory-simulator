#include "Cache.hpp"
#include <iostream>
#include <cmath>
#include <iomanip>

bool is_power_of_two(int n)
{
    return (n > 0) && ((n & (n - 1)) == 0);
}

Cache::Cache(const std::string &nome, int latencia, Memoria *proximo_nivel,
             int tamanho_kb, int associatividade, int tamanho_linha_bytes, PoliticaEscrita politica)
    : Memoria(nome, latencia, proximo_nivel), associatividade(associatividade),
      tamanho_linha(tamanho_linha_bytes), politica_escrita(politica)
{

    if (!is_power_of_two(tamanho_linha_bytes))
    {
        throw std::invalid_argument("Tamanho da linha deve ser uma potência de 2.");
    }

    int tamanho_total_bytes = tamanho_kb * 1024;
    this->num_conjuntos = tamanho_total_bytes / (tamanho_linha_bytes * associatividade);

    if (!is_power_of_two(num_conjuntos))
    {
        throw std::invalid_argument("O número de conjuntos resultante não é uma potência de 2. Verifique os parâmetros da cache.");
    }

    conjuntos.resize(num_conjuntos, std::vector<LinhaCache>(associatividade));
    bits_offset = log2(tamanho_linha_bytes);
    bits_indice = log2(num_conjuntos);
}

void Cache::extrair_partes_endereco(uint32_t endereco, uint32_t &tag, uint32_t &indice)
{
    uint32_t numero_do_bloco = endereco >> bits_offset;
    indice = numero_do_bloco % num_conjuntos;
    tag = numero_do_bloco / num_conjuntos;
}

int Cache::encontrar_lru(uint32_t indice)
{
    int lru_index = -1;
    uint64_t min_ciclo = -1; 

    for (int i = 0; i < associatividade; ++i)
    {
        if (!conjuntos[indice][i].valida)
        {
            return i; 
        }
        if (conjuntos[indice][i].ultimo_acesso < min_ciclo)
        {
            min_ciclo = conjuntos[indice][i].ultimo_acesso;
            lru_index = i;
        }
    }
    return lru_index;
}

void Cache::trazer_bloco_para_cache(uint32_t endereco, uint32_t tag, uint32_t indice, int &latencia_acumulada, bool is_write)
{
    latencia_acumulada += proximo_nivel->ler(endereco);

    int indice_substituicao = encontrar_lru(indice);
    LinhaCache &linha_para_substituir = conjuntos[indice][indice_substituicao];

    if (politica_escrita == PoliticaEscrita::WRITE_BACK && linha_para_substituir.valida && linha_para_substituir.suja)
    {
        uint32_t numero_bloco_antigo = linha_para_substituir.tag * num_conjuntos + indice;
        uint32_t endereco_antigo = numero_bloco_antigo << bits_offset;
        latencia_acumulada += proximo_nivel->escrever(endereco_antigo);
    }

    linha_para_substituir.valida = true;
    linha_para_substituir.suja = is_write;
    linha_para_substituir.tag = tag;
    linha_para_substituir.ultimo_acesso = ciclo_atual;
}

int Cache::ler(uint32_t endereco)
{
    ciclo_atual++;
    stats.leituras++;

    uint32_t tag, indice;
    extrair_partes_endereco(endereco, tag, indice);

    for (int i = 0; i < associatividade; ++i)
    {
        if (conjuntos[indice][i].valida && conjuntos[indice][i].tag == tag)
        {
            stats.hits_leitura++;
            conjuntos[indice][i].ultimo_acesso = ciclo_atual;
            return latencia;
        }
    }

    stats.misses_leitura++;
    int latencia_total = latencia;

    trazer_bloco_para_cache(endereco, tag, indice, latencia_total, false);

    return latencia_total;
}

int Cache::escrever(uint32_t endereco)
{
    ciclo_atual++;
    stats.escritas++;

    uint32_t tag, indice;
    extrair_partes_endereco(endereco, tag, indice);

    // Procurar por um HIT
    for (int i = 0; i < associatividade; ++i)
    {
        if (conjuntos[indice][i].valida && conjuntos[indice][i].tag == tag)
        {
            stats.hits_escrita++;
            conjuntos[indice][i].ultimo_acesso = ciclo_atual;

            if (politica_escrita == PoliticaEscrita::WRITE_THROUGH)
            {
                return latencia + proximo_nivel->escrever(endereco);
            }
            else // WRITE_BACK
            {
                conjuntos[indice][i].suja = true;
                return latencia;
            }
        }
    }

    // MISS de escrita (Política Write-Allocate)
    stats.misses_escrita++;
    int latencia_total = latencia;

    trazer_bloco_para_cache(endereco, tag, indice, latencia_total, true);

    if (politica_escrita == PoliticaEscrita::WRITE_THROUGH)
    {
       latencia_total += proximo_nivel->escrever(endereco);
    }

    return latencia_total;
}

int Cache::get_tamanho_kb() const
{
    return (num_conjuntos * associatividade * tamanho_linha) / 1024;
}

void Cache::imprimir_estatisticas()
{
    uint64_t total_hits = stats.hits_leitura + stats.hits_escrita;
    uint64_t total_misses = stats.misses_leitura + stats.misses_escrita;
    uint64_t total_acessos = total_hits + total_misses;
    double taxa_miss = (total_acessos == 0) ? 0.0 : (double)total_misses / total_acessos;

    std::cout << "--- Estatisticas para " << nome << " ---" << std::endl;
    std::cout << "Tamanho: " << get_tamanho_kb() << " KBytes" << std::endl;
    std::cout << "Acessos de Leitura: " << stats.leituras << std::endl;
    std::cout << "Acessos de Escrita: " << stats.escritas << std::endl;
    std::cout << "Total de Acessos: " << total_acessos << std::endl;
    std::cout << "Hits: " << total_hits << std::endl;
    std::cout << "Misses: " << total_misses << std::endl;
    std::cout << "Taxa de Miss: " << std::fixed << std::setprecision(4) << taxa_miss * 100.0 << "%" << std::endl;
    std::cout << std::endl;
}