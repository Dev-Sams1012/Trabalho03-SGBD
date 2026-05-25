#include "Esquema.hpp"

void Esquema::add(const std::string &columnName)
{
    nomeParaIndice[columnName] = qtdColunas;
    indiceparaNome.push_back(columnName);
    qtdColunas++;
}

std::string Esquema::getNomeDaColuna(int index) const
{
    return indiceparaNome[index];
}

int Esquema::getIndiceDaColuna(const std::string &columnName) const
{
    auto it = nomeParaIndice.find(columnName);
    if (it != nomeParaIndice.end())
    {
        return it->second;
    }
    return -1;
}