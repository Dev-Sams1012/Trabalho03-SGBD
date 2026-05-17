#pragma once

#include <string>
#include <stdexcept>

// Compara dois valores de coluna.
// Tenta conversão numérica primeiro para evitar ordenação lexicográfica incorreta
// (ex: "10" < "2" lexicograficamente, mas 10 > 2 numericamente).
// Se a conversão falhar, compara como string.
inline int comparar_chaves(const std::string &a, const std::string &b)
{
    try
    {
        long long ia = std::stoll(a);
        long long ib = std::stoll(b);
        if (ia < ib)
            return -1;
        if (ia > ib)
            return 1;
        return 0;
    }
    catch (...)
    {
        return a.compare(b);
    }
}

inline bool chave_menor(const std::string &a, const std::string &b)
{
    return comparar_chaves(a, b) < 0;
}

inline bool chave_igual(const std::string &a, const std::string &b)
{
    return comparar_chaves(a, b) == 0;
}
