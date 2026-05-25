#include "Esquema.hpp"

void Esquema::add(const std::string &columnName)
{
    nomeParaIndice[columnName] = qtdColunas; // Atribui o índice atual à coluna
    indiceparaNome.push_back(columnName); // Armazena o nome da coluna na posição do índice
    qtdColunas++;
}

std::string Esquema::getNomeDaColuna(int index) const
{
    return indiceparaNome[index]; // Retorna o nome da coluna com base no índice
}

int Esquema::getIndiceDaColuna(const std::string &columnName) const 
{
    auto it = nomeParaIndice.find(columnName);
    if (it != nomeParaIndice.end())
    {
        return it->second; // Retorna o índice da coluna
    }
    return -1;
}