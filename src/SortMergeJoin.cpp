#include "SortMergeJoin.hpp"
#include <algorithm>
#include <iostream>

struct PageCursor
{
    const Tabela *tabela;
    int pagIdx;
    std::vector<Tupla> bufferTuplas;
    size_t tuplaIdx;

    explicit PageCursor(const Tabela *t) : tabela(t), pagIdx(0), tuplaIdx(0)
    {
        carregarPagina();
    }

    void carregarPagina()
    {
        if (pagIdx < tabela->getQtdPags())
        {
            bufferTuplas = tabela->fetchPage(pagIdx).fetchAll();
            tuplaIdx = 0;
        }
        else
        {
            bufferTuplas.clear();
        }
    }

    bool temMais() const
    {
        return !bufferTuplas.empty();
    }

    const Tupla &getCurrent() const
    {
        return bufferTuplas[tuplaIdx];
    }

    void advance()
    {
        tuplaIdx++;
        if (tuplaIdx >= bufferTuplas.size())
        {
            pagIdx++;
            carregarPagina();
        }
    }
};

SortMergeJoin::SortMergeJoin(Tabela *leftTable, Tabela *rightTable,
                             const std::string &leftCol, const std::string &rightCol)
    : tabelaEsquerda(leftTable), tabelaDireita(rightTable),
      colunaJuncaoEsquerda(leftCol), colunaJuncaoDireita(rightCol)
{
    Esquema esquemaResultado;
    
    // colunas de Grapes
    for (int i = 0; i < tabelaEsquerda->getEsquema().getQtdColunas(); i++)
        esquemaResultado.add(leftTable->getEsquema().getNomeDaColuna(i));
    // colunas de Wines
    for (int i = 0; i < tabelaDireita->getEsquema().getQtdColunas(); i++)
        esquemaResultado.add(rightTable->getEsquema().getNomeDaColuna(i));
    tabelaResultado = new Tabela(esquemaResultado);
}

SortMergeJoin::~SortMergeJoin()
{
    delete tabelaResultado;
}

void SortMergeJoin::execute()
{
    Tabela sortedGrapes = sortTable(tabelaEsquerda, colunaJuncaoEsquerda);
    Tabela sortedWines = sortTable(tabelaDireita, colunaJuncaoDireita);
    performMergeJoin(sortedGrapes, sortedWines);
}

Tabela SortMergeJoin::sortTable(Tabela *table, const std::string &joinCol)
{
    int colIndex = table->getEsquema().getIndiceDaColuna(joinCol);
    std::vector<Tabela> runs = generateRuns(table, colIndex);
    return mergeRuns(runs, colIndex, table->getEsquema());
}

std::vector<Tabela> SortMergeJoin::generateRuns(Tabela *table, int colIndex)
{
    std::vector<Tabela> runsGeradas;
    const int BUFFER_FRAMES = 5;
    int totalPags = table->getQtdPags();

    for (int i = 0; i < totalPags; i += BUFFER_FRAMES)
    {
        std::vector<Tupla> bufferFrames;

        int pagesToLoad = std::min(BUFFER_FRAMES, totalPags - i);
        for (int j = 0; j < pagesToLoad; j++)
        {
            const std::vector<Tupla> tuplasNaPag = table->fetchPage(i + j).fetchAll();
            bufferFrames.insert(bufferFrames.end(), tuplasNaPag.begin(), tuplasNaPag.end());
        }

        // Ordenacao em memoria
        std::stable_sort(bufferFrames.begin(), bufferFrames.end(),
                         [colIndex](const Tupla &a, const Tupla &b)
                         {
                             return a.get(colIndex) < b.get(colIndex);
                         });

        // Grava como uma run
        Tabela run(table->getEsquema());
        for (const Tupla &t : bufferFrames)
        {
            run.add(t);
        }
        runsGeradas.push_back(run);
    }
    return runsGeradas;
}

Tabela SortMergeJoin::mergeRuns(std::vector<Tabela> &runs, int colIndex, Esquema schema)
{
    // Intercalação externa com B=5 frames (4 entrada + 1 saída)
    const int MAX_INPUT_BUFFERS = 4;

    std::vector<Tabela> currentRuns = runs;
    int passNumber = 0;

    // Enquanto houver mais de 1 run, continuar intercalando
    while (currentRuns.size() > 1)
    {
        std::vector<Tabela> nextRuns;

        // Processa grupos de até 4 runs por vez
        for (size_t i = 0; i < currentRuns.size(); i += MAX_INPUT_BUFFERS)
        {
            std::vector<Tabela *> inputRuns;
            int runsToMerge = std::min((size_t)MAX_INPUT_BUFFERS, currentRuns.size() - i);

            for (int j = 0; j < runsToMerge; ++j)
            {
                inputRuns.push_back(&currentRuns[i + j]);
            }

            // Faz merge dos 4 runs de entrada em 1 run de saída
            Tabela mergedRun = performExternalMerge(inputRuns, colIndex, schema);
            nextRuns.push_back(mergedRun);
        }

        currentRuns = nextRuns;
        passNumber++;
    }

    return currentRuns.empty() ? Tabela(schema) : currentRuns[0];
}

Tabela SortMergeJoin::performExternalMerge(std::vector<Tabela *> &inputRuns, int colIndex, Esquema schema)
{
    // Implementa merge de múltiplas runs com buffer limitado (4 entrada + 1 saída)
    Tabela outputRun(schema);

    // Cria cursores para cada run de entrada
    std::vector<PageCursor> cursors;
    for (const auto &run : inputRuns)
    {
        cursors.emplace_back(run);
    }

    std::vector<Tupla> outputBuffer;
    const int OUTPUT_BUFFER_SIZE = 12; // Uma página cheia

    while (true)
    {
        int activeRunIdx = -1;

        // Encontra a tupla com menor valor entre todos os cursores
        std::string minValue;
        for (size_t i = 0; i < cursors.size(); ++i)
        {
            if (cursors[i].temMais())
            {
                std::string currentValue = cursors[i].getCurrent().get(colIndex);
                if (activeRunIdx == -1 || currentValue < minValue)
                {
                    minValue = currentValue;
                    activeRunIdx = i;
                }
            }
        }

        // Se nenhum cursor tem tuplas, termina
        if (activeRunIdx == -1)
            break;

        // Move tupla para buffer de saída
        outputBuffer.push_back(cursors[activeRunIdx].getCurrent());
        cursors[activeRunIdx].advance();

        // Se buffer de saída cheio (1 página = 12 tuplas), escreve na run
        if (outputBuffer.size() >= OUTPUT_BUFFER_SIZE)
        {
            for (const auto &t : outputBuffer)
            {
                outputRun.add(t);
            }
            outputBuffer.clear();
        }
    }

    // Escreve tuplas restantes no buffer de saída
    for (const auto &t : outputBuffer)
    {
        outputRun.add(t);
    }

    return outputRun;
}

void SortMergeJoin::performMergeJoin(const Tabela &sortedLeft, const Tabela &sortedRight)
{
    int idxLeft = sortedLeft.getEsquema().getIndiceDaColuna(colunaJuncaoEsquerda);
    int idxRight = sortedRight.getEsquema().getIndiceDaColuna(colunaJuncaoDireita);

    PageCursor Tr(&sortedLeft);
    PageCursor Gs(&sortedRight);
    PageCursor Ts(&sortedRight);

    int idx = 1;
    while (Tr.temMais() && Gs.temMais())
    {
        // 1. Enquanto Tri < Gsj, avança R
        while (Tr.temMais() && Gs.temMais() && Tr.getCurrent().get(idxLeft) < Gs.getCurrent().get(idxRight))
        {
            Tr.advance();
        }
        if (!Tr.temMais())
            break;

        // 2. Enquanto Tri > Gsj, avança S (início da partição)
        while (Tr.temMais() && Gs.temMais() && Tr.getCurrent().get(idxLeft) > Gs.getCurrent().get(idxRight))
        {
            Gs.advance();
        }
        if (!Gs.temMais())
            break;

        // 3. Processa a partição (Tri == Gsj)
        Ts = Gs;

        while (Tr.temMais() && Gs.temMais() && Tr.getCurrent().get(idxLeft) == Gs.getCurrent().get(idxRight))
        {

            Ts = Gs;

            while (Ts.temMais() && Ts.getCurrent().get(idxRight) == Tr.getCurrent().get(idxLeft))
            {
                // Combina todas as colunas da tupla esquerda com todas da tupla direita
                Tupla combinada;

                // Todas as colunas da tupla esquerda (Grapes)
                int nColsEsq = sortedLeft.getEsquema().getQtdColunas();
                for (int i = 0; i < nColsEsq; i++)
                    combinada.add(Tr.getCurrent().get(i));

                // Todas as colunas da tupla direita (Wines)
                int nColsDir = sortedRight.getEsquema().getQtdColunas();
                for (int i = 0; i < nColsDir; i++)
                    combinada.add(Ts.getCurrent().get(i));

                tabelaResultado->add(combinada);

                Ts.advance();
            }
            Tr.advance();
        }
        Gs = Ts;
    }
}