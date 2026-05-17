#include "sort_merge_join.hpp"
#include "comparador.hpp"

#include <iostream>
#include <vector>

SortMergeJoin::SortMergeJoin(Disco &d) : disco(d) {}

// =============================================================================
// Cursor: controla o fluxo de leitura de uma run associada a um frame do buffer.
// Ele simula o hardware carregando dinamicamente páginas do disco para a RAM.
// =============================================================================
struct Cursor
{
    const Disco &disco;
    const std::string nome_run;
    Buffer &buffer;
    const int frame_id;

    int pag_idx;
    int tup_idx;
    int total_pags;

    Cursor(const Disco &d, std::string nome, Buffer &buf, int frame)
        : disco(d), nome_run(std::move(nome)), buffer(buf), frame_id(frame),
          pag_idx(0), tup_idx(0)
    {
        const auto &run = disco.ler_run(nome_run);
        total_pags = static_cast<int>(run.size());

        // Traz a primeira página da run para o buffer se ela não estiver vazia
        if (total_pags > 0)
        {
            buffer.frame(frame_id) = run[0];
        }
    }

    bool esgotado() const
    {
        return pag_idx >= total_pags;
    }

    const Tupla &atual() const
    {
        return buffer.frame(frame_id).tuplas[tup_idx];
    }

    void avancar()
    {
        if (esgotado())
            return;

        tup_idx++;
        // Se todas as tuplas da página atual no buffer foram consumidas
        if (tup_idx >= buffer.frame(frame_id).qtd_ocup)
        {
            pag_idx++;
            tup_idx = 0;

            // Traz a próxima página física do disco para o mesmo frame da RAM
            if (pag_idx < total_pags)
            {
                buffer.frame(frame_id) = disco.ler_run(nome_run)[pag_idx];
            }
        }
    }
};

// =============================================================================
// Merge Join
// =============================================================================
Tabela SortMergeJoin::executar(
    const std::string &nome_run_esq,
    const std::string &nome_run_dir,
    const Esquema &esq_esq,
    const Esquema &esq_dir,
    int col_esq,
    int col_dir)
{
    // Define explicitamente os frames utilizados no Buffer Manager
    const int FRAME_ESQ = 0;
    const int FRAME_DIR = 1;
    const int FRAME_SAIDA = 2;

    // Constrói o esquema final concatenando os nomes das colunas
    std::vector<std::string> nomes_resultado;
    for (const auto &nome : esq_esq.nomes)
        nomes_resultado.push_back(nome);
    for (const auto &nome : esq_dir.nomes)
        nomes_resultado.push_back(nome);

    Esquema esq_resultado(nomes_resultado);
    Tabela resultado(esq_resultado);

    // Inicializa os cursores alocando-os em seus respectivos frames do buffer
    Cursor cl(disco, nome_run_esq, buffer, FRAME_ESQ);
    Cursor cr(disco, nome_run_dir, buffer, FRAME_DIR);

    buffer.limpar_frame(FRAME_SAIDA);
    int tuplas_geradas = 0;

    // Loop principal de varredura sequencial (Merge)
    while (!cl.esgotado() && !cr.esgotado())
    {
        const std::string &chave_l = cl.atual().cols[col_esq];
        const std::string &chave_r = cr.atual().cols[col_dir];

        int cmp = comparar_chaves(chave_l, chave_r);

        if (cmp < 0)
        {
            cl.avancar();
        }
        else if (cmp > 0)
        {
            cr.avancar();
        }
        else
        {
            // Encontrou chaves iguais: isola os grupos duplicados de ambos os lados
            const std::string chave_match = chave_l;

            std::vector<Tupla> grupo_esq;
            while (!cl.esgotado() && chave_igual(cl.atual().cols[col_esq], chave_match))
            {
                grupo_esq.push_back(cl.atual());
                cl.avancar();
            }

            std::vector<Tupla> grupo_dir;
            while (!cr.esgotado() && chave_igual(cr.atual().cols[col_dir], chave_match))
            {
                grupo_dir.push_back(cr.atual());
                cr.avancar();
            }

            // Realiza o produto cartesiano local dos grupos idênticos
            for (const auto &tl : grupo_esq)
            {
                for (const auto &tr : grupo_dir)
                {
                    Tupla joined(esq_resultado.qtd_cols());
                    int k = 0;

                    // Copia colunas da esquerda e da direita usando loops limpos
                    for (const auto &col : tl.cols)
                        joined.set(k++, col);
                    for (const auto &col : tr.cols)
                        joined.set(k++, col);

                    // Acumula a linha gerada diretamente no frame de saída do buffer
                    auto &frame_saida = buffer.frame(FRAME_SAIDA);
                    frame_saida.inserir(joined);
                    tuplas_geradas++;

                    // Quando o frame de saída atinge o limite, descarrega na tabela de resultado
                    if (frame_saida.cheia())
                    {
                        resultado.adicionar_pagina(frame_saida);
                        frame_saida.limpar();
                    }
                }
            }
        }
    }

    // Descarrega qualquer dado residual que restou no buffer de saída
    auto &frame_saida = buffer.frame(FRAME_SAIDA);
    if (!frame_saida.vazia())
    {
        resultado.adicionar_pagina(frame_saida);
        frame_saida.limpar();
    }

    std::cout << "[SMJ] Join concluido. Tuplas geradas: " << tuplas_geradas
              << " em " << resultado.qtd_pags() << " pagina(s).\n";

    return resultado;
}