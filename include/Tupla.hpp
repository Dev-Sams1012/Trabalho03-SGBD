#ifndef TUPLA_HPP
#define TUPLA_HPP

#include <string>
#include <vector>

class Tupla
{
private:
    std::vector<std::string> colunas;

public:
    Tupla() {}
    void add(const std::string &value) { colunas.push_back(value); }
    std::string get(int index) const { return colunas[index]; }
    void display() const;
};

#endif