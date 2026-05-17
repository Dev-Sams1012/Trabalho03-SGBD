#include "csv_loader.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

std::string CsvLoader::trim(const std::string& str) {
    if (str.empty()) return str;

    size_t start = 0;
    // Remove o BOM UTF-8 se existir
    if (str.size() >= 3 && 
        (unsigned char)str[0] == 0xEF && 
        (unsigned char)str[1] == 0xBB && 
        (unsigned char)str[2] == 0xBF) {
        start = 3;
    }

    // Avança enquanto encontrar espaços comuns, tabs, quebras de linha ou caracteres de controle ASCII (0 a 32)
    while (start < str.size() && (static_cast<unsigned char>(str[start]) <= 32 || str[start] == ' ')) {
        start++;
    }

    if (start == str.size()) return "";

    size_t fim = str.size() - 1;
    // Recua enquanto encontrar espaços comuns ou caracteres de controle no final
    while (fim > start && (static_cast<unsigned char>(str[fim]) <= 32 || str[fim] == ' ')) {
        fim--;
    }

    return str.substr(start, (fim - start + 1));
}

Tabela CsvLoader::carregar(const std::string &caminho_csv, long long id_ini, long long id_fim)
{
    std::ifstream arquivo(caminho_csv);
    if (!arquivo.is_open())
    {
        std::cerr << "ERRO: Nao foi possivel abrir o arquivo: " << caminho_csv << std::endl;
        return Tabela(Esquema({}));
    }

    std::string linha, campo;

    // 1. Ler e processar a primeira linha como cabeçalho de metadados
    if (!std::getline(arquivo, linha))
    {
        return Tabela(Esquema({}));
    }

    char separador = (linha.find('\t') != std::string::npos) ? '\t' : ',';
    std::vector<std::string> colunas;
    std::stringstream ss_header(linha);
    while (std::getline(ss_header, campo, separador))
    {
        colunas.push_back(trim(campo));
    }

    Esquema esq(colunas);
    Tabela tabela(esq);

    // Identificar se estamos carregando Grapes ou Wines
    bool eh_grapes = (caminho_csv.find("grapes.csv") != std::string::npos);

    int idx_fk = esq.indice("chave_estrangeira");
    long long contador_linhas_dados = 0;

    // Mapeamento dinâmico: associa o nome da uva ao seu ID de linha (0 a 39)
    // para podermos filtrar os vinhos baseados no intervalo da CLI
    static std::vector<std::string> mapa_uvas_ordem;
    if (eh_grapes)
    {
        mapa_uvas_ordem.clear(); // Reinicia para garantir consistência
    }

    // 2. Leitura dos dados
    while (std::getline(arquivo, linha))
    {
        if (linha.empty())
            continue;

        Tupla t;
        t.cols.resize(esq.qtd_cols());

        std::stringstream ss_linha(linha);
        int col = 0;
        while (std::getline(ss_linha, campo, separador) && col < esq.qtd_cols())
        {
            t.set(col++, trim(campo));
        }

        if (eh_grapes)
        {
            // Para uvas, o "ID" é a ordem física de aparição dela no arquivo
            long long id_uva_atual = contador_linhas_dados;
            std::string nome_uva = t.cols[0]; // chave_primaria

            contador_linhas_dados++;

            // Aplica o filtro de intervalo da CLI [id_inicio ate id_fim]
            if (id_uva_atual < id_ini || id_uva_atual > id_fim)
            {
                continue;
            }

            // Guarda a uva aceita no mapa para filtrar os vinhos depois
            mapa_uvas_ordem.push_back(nome_uva);
        }
        else
        {
            // Para vinhos, checamos se a uva associada (chave_estrangeira) está no intervalo aceito
            if (idx_fk != -1 && idx_fk < (int)t.cols.size())
            {
                std::string uva_do_vinho = t.cols[idx_fk];

                // Se o mapa de uvas válidas não estiver vazio, o vinho só entra se a uva dele foi aceita
                if (!mapa_uvas_ordem.empty())
                {
                    auto it = std::find(mapa_uvas_ordem.begin(), mapa_uvas_ordem.end(), uva_do_vinho);
                    if (it == mapa_uvas_ordem.end())
                    {
                        continue; // Vinho não pertence ao intervalo de uvas selecionado
                    }
                }
            }
        }

        tabela.inserir_tupla(t);
    }

    arquivo.close();
    return tabela;
}