#include "tupla.hpp"

Tupla::Tupla(int qtd_cols) : cols(qtd_cols) {}

const std::string &Tupla::get(int idx) const
{
    return cols[idx];
}

void Tupla::set(int idx, const std::string &val)
{
    cols[idx] = val;
}

bool Tupla::vazia() const
{
    return cols.empty();
}
