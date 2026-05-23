#ifndef PAGINA_HPP
#define PAGINA_HPP

#include "Tupla.hpp"
#include <vector>

class Pagina
{
private:
    std::vector<Tupla> tuplas;
    int qtdTuplasOcupadas;

public:
    Pagina() : qtdTuplasOcupadas(0) {}

    bool add(const Tupla &tuple);
    std::vector<Tupla> fetchAll() const { return tuplas; }
    int getQtdTuplas() const { return qtdTuplasOcupadas; }
    bool isFull() const { return qtdTuplasOcupadas >= 12; }
};

#endif