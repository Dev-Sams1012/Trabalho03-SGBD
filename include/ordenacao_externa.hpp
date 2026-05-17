#pragma once

#include "tabela.hpp"
#include "disco.hpp"
#include "buffer.hpp"
#include <string>
#include <vector>

// Implementa a ordenação externa de uma tabela usando B = 5 frames de buffer.
//
// Fase 1 — Geração de runs:
//   Lê até B páginas por vez, ordena em memória pelo atributo de junção
//   e grava cada bloco ordenado como uma run no disco simulado.
//
// Fase 2 — Intercalação externa (merge):
//   Usa B-1 = 4 frames de entrada e 1 frame de saída.
//   Intercala grupos de até 4 runs por passagem até restar uma única run ordenada.
class OrdenacaoExterna
{
    Disco &disco;
    Buffer buffer;

    // Fase 1: lê a tabela original em blocos de B páginas,
    // ordena cada bloco e grava no disco como runs iniciais.
    // Retorna os nomes das runs geradas.
    std::vector<std::string> gerar_runs(
        const Tabela &tabela,
        int col_join,
        const std::string &prefixo);

    // Fase 2: faz k-way merge de até (B-1) runs por vez usando uma min-heap.
    // Usa frames 0..n-1 como entrada e frame B-1 como saída.
    // Quando o frame de saída enche, descarrega no disco.
    std::string intercalar_runs(
        const std::vector<std::string> &nomes_runs,
        int col_join,
        const std::string &nome_saida);

public:
    OrdenacaoExterna(Disco &d);

    // Ponto de entrada público.
    // Retorna o nome da run final gravada no disco simulado.
    std::string ordenar(
        const Tabela &tabela,
        int col_join,
        const std::string &prefixo);
};
