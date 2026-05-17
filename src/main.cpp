#include "tabela.hpp"
#include "disco.hpp"
#include "ordenacao_externa.hpp"
#include "sort_merge_join.hpp"
#include "csv_loader.hpp"

#include <iostream>
#include <iomanip>
#include <string>
#include <limits>

// Imprime todas as tuplas de uma tabela no stdout, paginando a cada 12 linhas
// Imprime todas as tuplas de uma tabela no stdout, ocultando 'grape_synonym' e paginando
// Imprime todas as tuplas de uma tabela no stdout, ocultando 'grape_synonym' e paginando
static void imprimir_tabela(const Tabela &tab, const std::string &titulo)
{
    std::cout << "\n=== " << titulo << " ===\n";

    // Cabeçalho (Pula se o nome for exatamente "grape_synonym")
    int qtd_cols_visiveis = 0;
    for (int i = 0; i < tab.esquema.qtd_cols(); i++)
    {
        if (tab.esquema.nomes[i] == "grape_synonym")
            continue; // Pula o dado se a coluna for o sinônimo

        if (tab.esquema.nomes[i] == "grape_country")
            continue; // Pula o dado se a coluna for o país

        if (tab.esquema.nomes[i] == "origem")
            continue;

        if (tab.esquema.nomes[i] == "tipo")
            continue;

        if (tab.esquema.nomes[i] == "estilo")
            continue;

        std::cout << std::setw(18) << std::left << tab.esquema.nomes[i] << " ";
        qtd_cols_visiveis++;
    }
    std::cout << "\n";

    // Ajusta o tamanho da linha separadora dinamicamente
    std::cout << std::string(qtd_cols_visiveis * 19, '-') << "\n";

    // Tuplas com Paginação
    int total = 0;
    for (int p = 0; p < tab.qtd_pags(); p++)
    {
        const Pagina &pag = tab.ler_pagina(p);
        for (int t = 0; t < pag.qtd_ocup; t++)
        {
            // Loop pelas colunas da tupla baseando-se no nome do esquema correspondente
            for (int i = 0; i < tab.esquema.qtd_cols(); i++)
            {
                if (tab.esquema.nomes[i] == "grape_synonym")
                    continue; // Pula o dado se a coluna for o sinônimo

                if (tab.esquema.nomes[i] == "grape_country")
                    continue; // Pula o dado se a coluna for o país

                if (tab.esquema.nomes[i] == "origem")
                    continue;

                if (tab.esquema.nomes[i] == "tipo")
                    continue;

                if (tab.esquema.nomes[i] == "estilo")
                    continue;

                // Garante que o índice exista na tupla atual antes de tentar ler
                if (i < static_cast<int>(pag.tuplas[t].cols.size()))
                {
                    std::cout << std::setw(18) << std::left << pag.tuplas[t].cols[i] << " ";
                }
            }
            std::cout << "\n";
            total++;

            // Pausa a cada página de banco cheia (12 tuplas)
            if (total % 12 == 0)
            {
                std::cout << "\n[Pressione ENTER para ver a proxima pagina...]";
                std::cin.get();
            }
        }
    }
    std::cout << "\nTotal: " << total << " tupla(s) em "
              << tab.qtd_pags() << " pagina(s).\n";
}

int main()
{
    std::cout << "==================================================\n";
    std::cout << "         SGBD SIMULADO - CLI DE INTERVALOS        \n";
    std::cout << "==================================================\n\n";

    // Intervalo padrão amplo (lê tudo por padrão)
    long long id_inicio = 0;
    long long id_fim = 999999;
    std::string entrada;

    std::cout << "-> Digite o ID INICIAL desejado (ou pressione ENTER para o padrao 0): ";
    std::getline(std::cin, entrada);
    if (!entrada.empty())
    {
        try
        {
            id_inicio = std::stoll(entrada);
        }
        catch (...)
        {
        }
    }

    std::cout << "-> Digite o ID FINAL desejado (ou pressione ENTER para o padrao total/40): ";
    std::getline(std::cin, entrada);
    if (!entrada.empty())
    {
        try
        {
            id_fim = std::stoll(entrada);
        }
        catch (...)
        {
        }
    }
    else
    {
        id_fim = 40; // O tamanho real de uvas detetado no ficheiro TAB
    }

    // ADICIONE ESTA LINHA BEM AQUI PARA LIMPAR O BUFFER:
    if (std::cin.rdbuf()->in_avail() > 0) {
        std::cin.ignore();
    }

    std::cout << "\n==================================================\n";
    std::cout << "Processando intervalo de IDs de Grapes: [" << id_inicio << " ate " << id_fim << "]\n";
    std::cout << "==================================================\n\n";

    // -------------------------------------------------------------------------
    // 1. Carrega as tabelas a partir dos arquivos CSV aplicando o filtro.
    // -------------------------------------------------------------------------
    std::cout << "Carregando tabelas...\n";

    // Passamos os limites coletados na CLI para o CsvLoader
    Tabela grapes = CsvLoader::carregar("../data/grapes.csv", id_inicio, id_fim);
    Tabela wines = CsvLoader::carregar("../data/wines.csv", id_inicio, id_fim);

    std::cout << "Grapes filtradas: " << grapes.qtd_tuplas() << " tupla(s) em "
              << grapes.qtd_pags() << " pagina(s).\n";
    std::cout << "Wines filtrados:  " << wines.qtd_tuplas() << " tupla(s) em "
              << wines.qtd_pags() << " pagina(s).\n";

    if (grapes.qtd_tuplas() == 0)
    {
        std::cout << "\nNenhuma tupla encontrada no intervalo selecionado. Encerrando.\n";
        return 0;
    }

    // Índices das colunas de junção em cada tabela.
    int col_grapes = grapes.esquema.indice("chave_primaria");
    int col_wines = wines.esquema.indice("chave_estrangeira");

    if (col_grapes == -1 || col_wines == -1)
    {
        std::cerr << "ERRO: coluna de juncao nao encontrada no esquema.\n";
        return 1;
    }

    // -------------------------------------------------------------------------
    // 2. Disco simulado e ordenação externa de cada tabela.
    // -------------------------------------------------------------------------
    std::cout << "\n--- Ordenacao Externa ---\n";
    Disco disco;

    OrdenacaoExterna sorter(disco);
    std::string run_grapes = sorter.ordenar(grapes, col_grapes, "grapes");
    std::string run_wines = sorter.ordenar(wines, col_wines, "wines");

    // -------------------------------------------------------------------------
    // 3. Sort-Merge Join sobre as runs ordenadas.
    // -------------------------------------------------------------------------
    std::cout << "\n--- Sort-Merge Join ---\n";
    SortMergeJoin smj(disco);
    Tabela resultado = smj.executar(
        run_grapes,
        run_wines,
        grapes.esquema,
        wines.esquema,
        col_grapes,
        col_wines);

    // -------------------------------------------------------------------------
    // 4. Exibe o resultado.
    // -------------------------------------------------------------------------
    imprimir_tabela(resultado, "Resultado do Sort-Merge Join");

    // ADICIONE ESTAS LINHAS AQUI:
    std::cout << "\n==================================================\n";
    std::cout << "Programa finalizado. Pressione ENTER para sair...";
    std::cin.get(); // Aguarda o utilizador pressionar Enter antes de fechar

    return 0;
}