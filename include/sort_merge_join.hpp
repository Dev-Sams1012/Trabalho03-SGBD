#pragma once

#include "tabela.hpp"
#include "disco.hpp"
#include "buffer.hpp"
#include <string>

// Executa a etapa de Merge Join sobre duas runs já ordenadas no disco simulado.
//
// Percorre sequencialmente as páginas das duas runs, carregando-as nos frames
// do buffer (frame 0 = esquerda, frame 1 = direita, frame 2 = saída).
// Quando os atributos de junção coincidem, produz o produto cartesiano
// de todas as tuplas do grupo com o mesmo valor.
// As tabelas originais não são modificadas.
class SortMergeJoin
{
    Disco &disco;
    Buffer buffer;

public:
    SortMergeJoin(Disco &d);

    // Recebe os nomes das runs ordenadas no disco e os esquemas originais.
    // Retorna uma nova Tabela com o resultado da junção.
    Tabela executar(
        const std::string &nome_run_esq,
        const std::string &nome_run_dir,
        const Esquema &esq_esq,
        const Esquema &esq_dir,
        int col_esq,
        int col_dir);
};
