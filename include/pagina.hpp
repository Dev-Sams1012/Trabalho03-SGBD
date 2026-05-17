#pragma once

#include "tupla.hpp"
#include <array>

// Número fixo de tuplas por página, conforme especificação do trabalho.
constexpr int TUPLAS_POR_PAGINA = 12;

// Representa uma página do disco simulado.
// Contém um array fixo de tuplas e um contador de posições ocupadas.
class Pagina
{
public:
    std::array<Tupla, TUPLAS_POR_PAGINA> tuplas;
    int qtd_ocup;

    Pagina();

    bool cheia() const;
    bool vazia() const;

    void inserir(const Tupla &t);
    void limpar();
};
