#ifndef ESQUEMA_HPP
#define ESQUEMA_HPP

#include <string>
#include <vector>
#include <map>

class Esquema
{
private:
    int qtdColunas;
    std::map<std::string, int> nomeParaIndice;
    std::vector<std::string> indiceparaNome; 

public:
    Esquema() : qtdColunas(0) {}

    void add(const std::string &columnName);
    int getIndiceDaColuna(const std::string &columnName) const;
    std::string getNomeDaColuna(int index) const; 
    int getQtdColunas() const { return qtdColunas; }
};

#endif