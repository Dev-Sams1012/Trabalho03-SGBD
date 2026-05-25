#include "Pagina.hpp"

bool Pagina::add(const Tupla &tuple)
{
    if (isFull())
        return false;
    tuplas.push_back(tuple);
    qtdTuplasOcupadas++;
    return true;
}