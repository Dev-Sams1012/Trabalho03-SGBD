#pragma once

#include "tabela.hpp"
#include <string>

// Carrega um arquivo CSV em uma Tabela.
// A primeira linha deve conter os nomes das colunas (cabeçalho).
// Os campos são separados por vírgula; espaços em branco nas bordas são removidos.
class CsvLoader
{
public:
    static Tabela carregar(const std::string &caminho, long long id_ini = 0, long long id_fim = 999999);

private:
    static std::string trim(const std::string &s);
};
