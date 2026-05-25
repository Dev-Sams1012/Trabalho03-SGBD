#ifndef TABELA_HPP
#define TABELA_HPP

#include "Pagina.hpp"
#include "Esquema.hpp"
#include <vector>

class Tabela
{
private:
    std::vector<Pagina> pags;
    int qtdPags;
    Esquema esquemaTabela;

public:
    Tabela(const Esquema &schema) : qtdPags(0), esquemaTabela(schema) {}

    void add(const Pagina &page);
    void add(const Tupla &tuple);

    std::vector<Pagina> fetchPages() const { return pags; } // Retorna todas as páginas da tabela
    const Pagina &fetchPage(int idx) const { return pags.at(idx); } // Retorna a página no índice especificado
    Esquema getEsquema() const { return esquemaTabela; } // Retorna o esquema da tabela
    int getQtdPags() const { return qtdPags; } // Retorna a quantidade de páginas na tabela
};

#endif