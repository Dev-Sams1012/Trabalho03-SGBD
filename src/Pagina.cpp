#include "Pagina.hpp"

bool Pagina::add(const Tupla &tuple) // Adiciona uma tupla à página
{
    if (isFull()) // Verifica se a página está cheia antes de adicionar
        return false;
    tuplas.push_back(tuple);
    qtdTuplasOcupadas++;
    return true;
}