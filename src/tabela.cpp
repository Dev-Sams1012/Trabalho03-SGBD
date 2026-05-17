#include "tabela.hpp"

Tabela::Tabela(const Esquema &esq) : esquema(esq) {}

int Tabela::qtd_pags() const
{
    return (int)pags.size();
}

int Tabela::qtd_tuplas() const
{
    int total = 0;
    for (const Pagina &p : pags)
        total += p.qtd_ocup;
    return total;
}

const Pagina &Tabela::ler_pagina(int idx) const
{
    return pags[idx];
}

Pagina &Tabela::pagina(int idx)
{
    return pags[idx];
}

void Tabela::adicionar_pagina(const Pagina &p)
{
    pags.push_back(p);
}

void Tabela::inserir_tupla(const Tupla &t)
{
    // Cria a primeira página se a tabela estiver vazia,
    // ou se a última página estiver cheia.
    if (pags.empty() || pags.back().cheia())
    {
        pags.push_back(Pagina());
    }
    pags.back().inserir(t);
}
