#include "Tupla.hpp"
#include <iostream>

void Tupla::display() const // Implementação do método para exibir os valores da tupla
{
    for (size_t i = 0; i < colunas.size(); ++i) // Percorre os valores das colunas para os exibir
    {
        std::cout << colunas[i];
        if (i < colunas.size() - 1)
        {
            std::cout << " | ";
        }
    }
    std::cout << std::endl;
}