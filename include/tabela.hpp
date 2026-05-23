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

    std::vector<Pagina> fetchPages() const { return pags; }
    Esquema getEsquema() const { return esquemaTabela; }
    int getQtdPags() const { return qtdPags; }
};

#endif