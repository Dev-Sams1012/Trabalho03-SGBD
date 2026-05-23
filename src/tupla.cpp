#include "Tupla.hpp"
#include <iostream>

void Tupla::display() const
{
    for (size_t i = 0; i < colunas.size(); ++i)
    {
        std::cout << colunas[i];
        if (i < colunas.size() - 1)
        {
            std::cout << " | ";
        }
    }
    std::cout << std::endl;
}