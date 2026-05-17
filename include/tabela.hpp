#pragma once

#include "esquema.hpp"
#include "pagina.hpp"
#include <vector>

// Representa uma tabela em memória como uma lista de páginas.
// O Esquema é mantido por valor — sem ponteiros.
class Tabela
{
public:
    Esquema esquema;
    std::vector<Pagina> pags;

    Tabela(const Esquema &esq);

    int qtd_pags() const;
    int qtd_tuplas() const;

    const Pagina &ler_pagina(int idx) const;
    Pagina &pagina(int idx);

    void adicionar_pagina(const Pagina &p);

    // Insere uma tupla na última página disponível,
    // criando uma nova página automaticamente se necessário.
    void inserir_tupla(const Tupla &t);
};
