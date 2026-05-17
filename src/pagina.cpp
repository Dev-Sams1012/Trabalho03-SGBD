#include "pagina.hpp"

Pagina::Pagina() : qtd_ocup(0) {}

bool Pagina::cheia() const
{
    return qtd_ocup == TUPLAS_POR_PAGINA;
}

bool Pagina::vazia() const
{
    return qtd_ocup == 0;
}

void Pagina::inserir(const Tupla &t)
{
    tuplas[qtd_ocup++] = t;
}

void Pagina::limpar()
{
    qtd_ocup = 0;
}
