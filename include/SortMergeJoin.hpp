#ifndef SORT_MERGE_JOIN_HPP
#define SORT_MERGE_JOIN_HPP

#include "Tabela.hpp"
#include <string>
#include <vector>

class SortMergeJoin
{
protected:
    Tabela *tabelaEsquerda;
    Tabela *tabelaDireita;
    std::string colunaJuncaoEsquerda;
    std::string colunaJuncaoDireita;
    Tabela *tabelaResultado;

public:
    SortMergeJoin(Tabela *leftTable, Tabela *rightTable,
                  const std::string &leftCol, const std::string &rightCol);
    ~SortMergeJoin();

    void execute();
    Tabela *getResult() const { return tabelaResultado; }

private:
    Tabela sortTable(Tabela *table, const std::string &joinCol);
    std::vector<Tabela> generateRuns(Tabela *table, int colIndex);
    Tabela mergeRuns(std::vector<Tabela> &runs, int colIndex, Esquema schema);
    Tabela performExternalMerge(std::vector<Tabela*> &inputRuns, int colIndex, Esquema schema);
    void performMergeJoin(const Tabela &sortedLeft, const Tabela &sortedRight);
};

#endif