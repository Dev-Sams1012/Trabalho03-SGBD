#pragma once

#include "pagina.hpp"
#include <string>
#include <unordered_map>
#include <vector>

// Simula o disco em memória.
// Armazena conjuntos de páginas (runs ou tabelas ordenadas) indexados por nome.
// O nome de uma run segue o padrão: "<prefixo>_run_<id>".
class Disco
{
public:
    std::unordered_map<std::string, std::vector<Pagina>> storage;

    void gravar_run(const std::string &nome, const std::vector<Pagina> &pags);

    const std::vector<Pagina> &ler_run(const std::string &nome) const;

    bool existe(const std::string &nome) const;

    void remover_run(const std::string &nome);

    // Retorna o número total de páginas gravadas em todas as runs.
    int total_paginas() const;
};
