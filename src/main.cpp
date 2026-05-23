#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "Esquema.hpp"
#include "Tabela.hpp"
#include "Tupla.hpp"
#include "SortMergeJoin.hpp"

#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_RESET "\x1b[0m"
#define PATH_GRAPE "../data/grapes.csv"
#define PATH_WINE "../data/wines.csv"

#ifdef _WIN32
#include <windows.h>
#endif

void divider()
{
    std::cout << "--------------------------------------------------------" << std::endl;
}

void title(const std::string &text)
{
    divider();
    std::cout << ANSI_COLOR_CYAN << text << ANSI_COLOR_RESET << std::endl;
    divider();
}

// Exibe o menu e retorna os índices das colunas escolhidas pelo usuário
std::vector<int> escolherColunas(const Esquema &esq)
{
    int qtd = esq.getQtdColunas();

    std::cout << "Colunas disponíveis:" << std::endl;
    for (int i = 0; i < qtd; i++)
        std::cout << "  [" << i << "] " << esq.getNomeDaColuna(i) << std::endl;

    std::cout << "Digite os índices separados por espaço (ex: 0 1 3): ";

    std::vector<int> escolhas;
    std::string linha;
    std::getline(std::cin, linha);
    std::istringstream ss(linha);

    int idx;
    while (ss >> idx)
    {
        if (idx >= 0 && idx < qtd)
            escolhas.push_back(idx);
        else
            std::cout << "Índice " << idx << " ignorado (fora do intervalo)." << std::endl;
    }

    return escolhas;
}

// Exibe o resultado com índice visual e apenas as colunas escolhidas
void exibirResultado(const Tabela *tabela, const std::vector<int> &colunasExibir)
{
    int idx = 1;
    for (const auto &pag : tabela->fetchPages())
    {
        for (const auto &tup : pag.fetchAll())
        {
            std::cout << idx++;
            for (int col : colunasExibir)
                std::cout << " | " << tup.get(col);
            std::cout << std::endl;
        }
    }
}

// Função para ler linhas de CSV ignorando vírgulas dentro de aspas
std::vector<std::string> parseCSVLine(const std::string &linha)
{
    std::vector<std::string> colunas;
    std::string valorAtual = "";
    bool dentroDeAspas = false;

    for (size_t i = 0; i < linha.length(); ++i)
    {
        char c = linha[i];

        if (c == '"')
        {
            dentroDeAspas = !dentroDeAspas; // Alterna o estado ao encontrar aspas
        }
        else if (c == ',' && !dentroDeAspas)
        {
            colunas.push_back(valorAtual);
            valorAtual = "";
        }
        else
        {
            valorAtual += c;
        }
    }
    colunas.push_back(valorAtual); // Adiciona a última coluna da linha
    return colunas;
}

// Função auxiliar para limpar a string (remover espaços, \r, \n e aspas extras)
std::string trim(const std::string &str)
{
    // Procura o primeiro caractere que NÃO seja espaço, tab, \r, \n ou aspas
    size_t first = str.find_first_not_of(" \t\r\n\"");
    if (first == std::string::npos)
        return ""; // Retorna vazio se a string for só sujeira

    // Procura o último caractere válido
    size_t last = str.find_last_not_of(" \t\r\n\"");

    return str.substr(first, (last - first + 1));
}

// Função auxiliar de E/S no main para ler os arquivos CSV
bool carregarDeCSV(const std::string &caminhoArquivo, Tabela *tabela)
{
    std::ifstream arquivo(caminhoArquivo);

    if (!arquivo.is_open())
    {
        std::cout << ANSI_COLOR_RED << "Erro ao abrir o arquivo: " << caminhoArquivo << ANSI_COLOR_RESET << std::endl;
        return false;
    }

    std::string linha;

    // Lê e descarta a primeira linha (cabeçalho das colunas)
    std::getline(arquivo, linha);

    int totalCarregado = 0;

    while (std::getline(arquivo, linha))
    {
        if (linha.empty())
            continue; // Pula linhas em branco

        std::vector<std::string> colunas = parseCSVLine(linha);
        Tupla tupla;

        // Limpa e adiciona todas as colunas processadas na tupla
        for (const std::string &valor : colunas)
        {
            tupla.add(trim(valor));
        }

        tabela->add(tupla);
        totalCarregado++;
    }

    arquivo.close();
    std::cout << "Arquivo '" << caminhoArquivo << "' carregado. Total de tuplas: " << totalCarregado << std::endl;
    return true;
}

int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
    title("Trabalho 03 - SGBD");

    // 1. Definição dos Esquemas
    Esquema esquemaGrapes;
    esquemaGrapes.add("chave_primaria");
    esquemaGrapes.add("grape_synonym");
    esquemaGrapes.add("grape_country");

    Esquema esquemaWines;
    esquemaWines.add("vinho");
    esquemaWines.add("chave_estrangeira");
    esquemaWines.add("origem");
    esquemaWines.add("tipo");
    esquemaWines.add("estilo");

    Tabela *grapes = new Tabela(esquemaGrapes);
    Tabela *wines = new Tabela(esquemaWines);

    std::cout << "Lendo arquivos de dados..." << std::endl;
    bool sucessoGrapes = carregarDeCSV(PATH_GRAPE, grapes);
    bool sucessoWines = carregarDeCSV(PATH_WINE, wines);

    if (!sucessoGrapes || !sucessoWines)
    {
        std::cout << ANSI_COLOR_RED << "Encerrando execução devido a erro de leitura dos arquivos." << ANSI_COLOR_RESET << std::endl;
        delete grapes;
        delete wines;
        return 1;
    }

    // 3. Execução do Operador Sort-Merge Join
    title("Executando Operação de Junção...");
    SortMergeJoin *smj = new SortMergeJoin(grapes, wines, "chave_primaria", "chave_estrangeira");
    smj->execute();

    Tabela *resultado = smj->getResult();

    // 4. Exibição dos Resultados
    title("Resultado do Sort-Merge Join");

    std::vector<int> colunas = escolherColunas(resultado->getEsquema());
    exibirResultado(resultado, colunas);

    std::cout << std::endl;
    int totalTuplas = 0;
    for (const auto &pag : resultado->fetchPages())
        totalTuplas += pag.getQtdTuplas();

    std::cout << "Total de tuplas no resultado: " << totalTuplas << std::endl;

    // 5. Limpeza de Memória (Explicita conforme o guia de estilo)
    delete smj;
    delete grapes;
    delete wines;

    return 0;
}