#ifndef TUPLA_HPP
#define TUPLA_HPP

#include <string>
#include <vector>

class Tupla
{
private:
    std::vector<std::string> colunas; // Define um vetor para armazenar os valores das colunas da tupla

public:
    Tupla() {}
    void add(const std::string &value) { colunas.push_back(value); } // Adiciona um valor à tupla
    std::string get(int index) const { return colunas[index]; } // Retorna o valor da coluna no índice especificado
    void display() const; // Exibe os valores da tupla
};

#endif