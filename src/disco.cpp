#include "disco.hpp"
#include <stdexcept>

void Disco::gravar_run(const std::string &nome, const std::vector<Pagina> &pags)
{
    storage[nome] = pags;
}

const std::vector<Pagina> &Disco::ler_run(const std::string &nome) const
{
    auto it = storage.find(nome);
    if (it == storage.end())
    {
        throw std::runtime_error("Disco: run nao encontrada: " + nome);
    }
    return it->second;
}

bool Disco::existe(const std::string &nome) const
{
    return storage.count(nome) > 0;
}

void Disco::remover_run(const std::string &nome)
{
    storage.erase(nome);
}

int Disco::total_paginas() const
{
    int total = 0;
    for (const auto &par : storage)
    {
        total += (int)par.second.size();
    }
    return total;
}
