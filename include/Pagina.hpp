#ifndef PAGINA_HPP
#define PAGINA_HPP

#include "Tupla.hpp"
#include <vector>

class Pagina
{
private:
    std::vector<Tupla> tuplas; // Define um vetor para armazenar as tuplas da página
    int qtdTuplasOcupadas;

public:
    Pagina() : qtdTuplasOcupadas(0) {}

    bool add(const Tupla &tuple);
    std::vector<Tupla> fetchAll() const { return tuplas; } // Retorna todas as tuplas da página
    int getQtdTuplas() const { return qtdTuplasOcupadas; } // Retorna a quantidade de tuplas ocupadas na página
    bool isFull() const { return qtdTuplasOcupadas >= 12; } // Verifica se a página está cheia
};

#endif