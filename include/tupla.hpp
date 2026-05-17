#pragma once

#include <string>
#include <vector>

// Representa uma linha de uma tabela.
// Os valores são armazenados como strings e interpretados conforme o Esquema.
class Tupla
{
public:
    std::vector<std::string> cols;

    Tupla() = default;
    Tupla(int qtd_cols);

    const std::string &get(int idx) const;
    void set(int idx, const std::string &val);

    bool vazia() const;
};
