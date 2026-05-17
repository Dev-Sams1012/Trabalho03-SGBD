#pragma once

#include <string>
#include <vector>

// Descreve os metadados de uma tabela: nomes das colunas e mapeamento nome→índice.
// A busca é linear, pois o número de colunas é sempre pequeno (≤ 6 neste trabalho).
class Esquema
{
public:
    std::vector<std::string> nomes;

    Esquema(const std::vector<std::string> &cols);

    // Retorna o índice da coluna com o nome dado, ou -1 se não existir.
    int indice(const std::string &nome) const;

    int qtd_cols() const;
};
