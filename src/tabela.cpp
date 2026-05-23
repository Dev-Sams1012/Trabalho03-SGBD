#include "Tabela.hpp"

void Tabela::add(const Pagina &page)
{
    pags.push_back(page);
    qtdPags++;
}

void Tabela::add(const Tupla &tuple)
{
    if (pags.empty() || pags.back().isFull())
    {
        Pagina novaPagina;
        novaPagina.add(tuple);
        add(novaPagina);
    }
    else
    {
        pags.back().add(tuple);
    }
}