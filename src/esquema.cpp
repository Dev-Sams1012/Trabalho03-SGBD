#include "esquema.hpp"

Esquema::Esquema(const std::vector<std::string> &cols) : nomes(cols) {}

int Esquema::indice(const std::string &nome) const
{
    for (int i = 0; i < (int)nomes.size(); i++)
    {
        if (nomes[i] == nome)
            return i;
    }
    return -1;
}

int Esquema::qtd_cols() const
{
    return (int)nomes.size();
}
