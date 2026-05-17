#include "ordenacao_externa.hpp"
#include "comparador.hpp"

#include <algorithm>
#include <iostream>
#include <queue>

OrdenacaoExterna::OrdenacaoExterna(Disco &d) : disco(d) {}

// =============================================================================
// FASE 1 — Geração de runs iniciais
// =============================================================================
std::vector<std::string> OrdenacaoExterna::gerar_runs(
    const Tabela &tabela,
    int col_join,
    const std::string &prefixo)
{
    std::vector<std::string> nomes_runs;
    const int total_pags = tabela.qtd_pags();
    int run_id = 0;

    for (int inicio = 0; inicio < total_pags; inicio += B)
    {
        const int fim = std::min(inicio + B, total_pags);
        std::vector<Tupla> tuplas;

        // Carrega as páginas estritamente dentro do limite do Buffer (B)
        for (int p = inicio; p < fim; p++)
        {
            const int frame_idx = p - inicio;
            buffer.frame(frame_idx) = tabela.ler_pagina(p);
            const auto &pag = buffer.frame(frame_idx);

            for (int t = 0; t < pag.qtd_ocup; t++)
            {
                tuplas.push_back(pag.tuplas[t]);
            }
        }

        // Ordenação em memória (In-memory Sort)
        std::stable_sort(tuplas.begin(), tuplas.end(),
                         [col_join](const Tupla &a, const Tupla &b)
                         {
                             return chave_menor(a.cols[col_join], b.cols[col_join]);
                         });

        // Monta as novas páginas e grava a run de forma direta
        std::vector<Pagina> run_pags;
        Pagina pag_saida;
        for (const auto &tup : tuplas)
        {
            pag_saida.inserir(tup);
            if (pag_saida.cheia())
            {
                run_pags.push_back(pag_saida);
                pag_saida.limpar();
            }
        }
        if (!pag_saida.vazia())
        {
            run_pags.push_back(pag_saida);
        }

        std::string nome = prefixo + "_run_" + std::to_string(run_id++);
        disco.gravar_run(nome, run_pags);
        nomes_runs.push_back(nome);

        buffer.limpar_todos();

        std::cout << "[Ord] Run gravada: " << nome
                  << " (" << run_pags.size() << " pag(s), "
                  << tuplas.size() << " tupla(s))\n";
    }

    return nomes_runs;
}

// =============================================================================
// FASE 2 — Intercalação externa (k-way merge)
// =============================================================================
std::string OrdenacaoExterna::intercalar_runs(
    const std::vector<std::string> &nomes_runs,
    int col_join,
    const std::string &nome_saida)
{
    const int n = static_cast<int>(nomes_runs.size());
    const int FRAME_SAIDA = B - 1; // Frame 4 reservado para saída

    std::vector<int> pag_idx(n, 0);
    std::vector<int> tup_idx(n, 0);
    std::vector<bool> run_esgotada(n, false);

    // Carrega a página 0 de cada run ativa nos frames de entrada (0 a 3)
    int runs_ativas = 0;
    for (int i = 0; i < n; i++)
    {
        const auto &run_completa = disco.ler_run(nomes_runs[i]);
        if (!run_completa.empty())
        {
            buffer.frame(i) = run_completa[0];
            if (buffer.frame(i).qtd_ocup == 0)
            {
                run_esgotada[i] = true;
            }
            else
            {
                runs_ativas++;
            }
        }
        else
        {
            run_esgotada[i] = true;
        }
    }

    buffer.limpar_frame(FRAME_SAIDA);
    std::vector<Pagina> saida_pags;

    // Loop principal: executa enquanto houver runs com dados
    while (runs_ativas > 0)
    {
        int melhor_run = -1;
        std::string menor_chave = "";

        // Loop manual para encontrar a menor tupla entre as runs ativas (Substitui a Heap)
        for (int i = 0; i < n; i++)
        {
            if (run_esgotada[i])
                continue;

            std::string chave_atual = buffer.frame(i).tuplas[tup_idx[i]].cols[col_join];

            if (melhor_run == -1 || chave_menor(chave_atual, menor_chave))
            {
                menor_chave = chave_atual;
                melhor_run = i;
            }
        }

        // Se não achou ninguém, encerra (segurança)
        if (melhor_run == -1)
            break;

        // Copia a tupla vencedora para o buffer de saída
        const auto &tup_vencedora = buffer.frame(melhor_run).tuplas[tup_idx[melhor_run]];
        auto &frame_saida = buffer.frame(FRAME_SAIDA);
        frame_saida.inserir(tup_vencedora);

        if (frame_saida.cheia())
        { // [cite: 38]
            saida_pags.push_back(frame_saida);
            frame_saida.limpar();
        }

        // Avança o ponteiro da run que venceu
        tup_idx[melhor_run]++;

        // Se a página atual dessa run acabou, tenta ler a próxima página do disco
        if (tup_idx[melhor_run] >= buffer.frame(melhor_run).qtd_ocup)
        {
            pag_idx[melhor_run]++;
            tup_idx[melhor_run] = 0;

            const auto &run_completa = disco.ler_run(nomes_runs[melhor_run]);
            if (pag_idx[melhor_run] < static_cast<int>(run_completa.size()))
            {
                buffer.frame(melhor_run) = run_completa[pag_idx[melhor_run]];
                if (buffer.frame(melhor_run).qtd_ocup == 0)
                {
                    run_esgotada[melhor_run] = true;
                    runs_ativas--;
                }
            }
            else
            {
                // A run acabou completamente no disco
                run_esgotada[melhor_run] = true;
                runs_ativas--;
            }
        }
    }

    // Descarrega o restante do buffer de saída residual
    auto &frame_saida = buffer.frame(FRAME_SAIDA);
    if (!frame_saida.vazia())
    {
        saida_pags.push_back(frame_saida);
        frame_saida.limpar();
    }

    disco.gravar_run(nome_saida, saida_pags); // [cite: 38]
    return nome_saida;
}

// =============================================================================
// Ponto de entrada público
// =============================================================================
std::string OrdenacaoExterna::ordenar(
    const Tabela &tabela,
    int col_join,
    const std::string &prefixo)
{
    std::cout << "[Ord] Iniciando ordenacao externa de '" << prefixo << "' ("
              << tabela.qtd_pags() << " pag(s))...\n";

    std::vector<std::string> runs = gerar_runs(tabela, col_join, prefixo);

    if (runs.size() <= 1)
    {
        std::cout << "[Ord] Apenas 1 run gerada, sem intercalacao necessaria.\n";
        return runs.empty() ? "" : runs[0];
    }

    int passagem = 0;
    while (runs.size() > 1)
    {
        std::cout << "[Ord] Passagem de intercalacao " << passagem << " (" << runs.size() << " runs)...\n";

        std::vector<std::string> novas_runs;
        int grupo_id = 0;

        // Agrupa de (B - 1) em (B - 1) para o k-way merge
        for (size_t i = 0; i < runs.size(); i += (B - 1))
        {
            auto fim = std::min(runs.begin() + i + (B - 1), runs.end());
            std::vector<std::string> grupo(runs.begin() + i, fim);

            std::string nome_saida = prefixo + "_p" + std::to_string(passagem) + "_r" + std::to_string(grupo_id++);
            intercalar_runs(grupo, col_join, nome_saida);
            novas_runs.push_back(nome_saida);
        }

        runs = std::move(novas_runs); // Evita cópias desnecessárias de vetores
        passagem++;
    }

    std::cout << "[Ord] Ordenacao concluida. Run final: " << runs[0] << "\n";
    return runs[0];
}