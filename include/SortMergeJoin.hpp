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
                  const std::string &leftCol, const std::string &rightCol); // Construtor para inicializar as tabelas e colunas de junção
    ~SortMergeJoin(); // Destrutor para liberar a memória alocada para a tabela de resultado

    void execute(); // Executa o algoritmo de Sort-Merge Join
    Tabela *getResult() const { return tabelaResultado; }

private:
    Tabela sortTable(Tabela *table, const std::string &joinCol); // Ordena a tabela com base na coluna de junção
    std::vector<Tabela> generateRuns(Tabela *table, int colIndex); // Gera os "runs" para o merge externo
    Tabela mergeRuns(std::vector<Tabela> &runs, int colIndex, Esquema schema); // Mescla os runs ordenados
    Tabela performExternalMerge(std::vector<Tabela*> &inputRuns, int colIndex, Esquema schema); // Realiza o merge externo
    void performMergeJoin(const Tabela &sortedLeft, const Tabela &sortedRight); // Realiza a junção merge entre as tabelas ordenadas
};

#endif