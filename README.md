# Sistemas de Gerenciamento de Bancos de Dados - 2026.1

Este repositório contém o terceiro trabalho de implementação da disciplina **Sistemas de Gerenciamento de Bancos de Dados**, ministrada pelo professor **Javam Machado** no semestre **2026.1**.

## Descrição do Trabalho

O objetivo deste trabalho é implementar o operador de junção Sort-Merge (SMJ) com ordenação externa e gestão de buffer, conforme especificação do Trabalho III.

## Estrutura do Projeto

```bash
trabalho03/
├── CMakeLists.txt
├── README.md
├── data/
│   ├── grapes.csv          # Tabela de uvas
│   └── wines.csv           # Tabela de vinhos
├── include/
│   ├── buffer.hpp          # Gerenciador de buffer (B = 5 frames)
│   ├── disco.hpp           # Disco simulado em memória
│   ├── esquema.hpp         # Metadados da tabela (nomes das colunas)
│   ├── ordenacao_externa.hpp
│   ├── pagina.hpp          # Página com 12 tuplas
│   ├── sort_merge_join.hpp
│   ├── tabela.hpp
│   └── tupla.hpp
├── src/
│   ├── buffer.cpp
│   ├── disco.cpp
│   ├── esquema.cpp
│   ├── main.cpp
│   ├── ordenacao_externa.cpp
│   ├── pagina.cpp
│   ├── sort_merge_join.cpp
│   ├── tabela.cpp
│   └── tupla.cpp
└── utils/
    ├── comparador.hpp      # Comparação numérica/lexicográfica de chaves
    ├── csv_loader.hpp
    └── csv_loader.cpp
```

## Compilação e Execução

```bash
mkdir build && cd build
cmake ..
make
./smj
```

## Detalhes de Implementação

### Parâmetros fixos

- **B = 5 frames** de buffer disponíveis (cada frame = uma página).
- **12 tuplas por página**.
- Junção sobre `Grapes.chave_primaria = Wines.chave_estrangeira`.

### Fase 1 — Geração de runs (ordenação interna)

Lê até B = 5 páginas da tabela por vez, ordena as tuplas em memória usando
`std::stable_sort` e grava o bloco ordenado como uma *run* no disco simulado.

### Fase 2 — Intercalação externa

Usa **$B-1 = 4$** frames de entrada para mapear as runs ativas e **$1$** frame (Frame 4) reservado exclusivamente para o bloco de saída. Como o fator de ramificação é pequeno (**$k \le 4$**), o algoritmo emprega um loop manual de varredura sequencial direta entre as cabeças das runs em memória para selecionar iterativamente a tupla de menor chave. Esse processo elimina o overhead de estruturas complexas e se repete em múltiplas passagens (passes) até que reste uma única run consolidada.

### Merge Join

Percorre as duas runs ordenadas com dois cursores (um por tabela), avançando
o de menor chave. Quando as chaves coincidem, coleta o grupo inteiro de cada
lado e produz o produto cartesiano. O resultado é descarregado no frame de
saída (frame 2) e persistido na tabela resultado quando o frame enche.

### Comparação de chaves

A função `comparar_chaves` em `utils/comparador.hpp` tenta conversão numérica
antes de comparar lexicograficamente, garantindo ordem correta para IDs inteiros.

### Gestão de Buffer e I/O de Disco

Para simular de maneira fiel o comportamento de um SGBD real sob restrições severas de hardware, o projeto implementa regras rígidas de manipulação de memória através da classe `Buffer`:

- **Restrição de Ocupação:** O sistema respeita o limite físico de **$B = 5$ frames**. Durante a intercalação, $4$ frames são reservados exclusivamente para as páginas de entrada das *runs* e $1$ frame atua como buffer de saída (*output buffer*).

---

## Recursos Especiais da Interface (CLI)

O arquivo `main.cpp` expõe uma interface interativa de linha de comando que adiciona flexibilidade aos testes do SGBD:

1. **Filtro de Janela Dinâmica:** O usuário pode definir limites específicos de IDs (`ID Inicial` e `ID Final`). Apenas os registros contidos nesse intervalo serão processados e cruzados.
2. **Fallback Automático (ENTER):** Pressionar ENTER sem digitar valores faz com que o sistema assuma por padrão a leitura completa do dataset de 40 uvas.
3. **Impressão Customizada e Paginada:** A coluna extensa `grape_synonym` é ocultada dinamicamente no terminal para manter as colunas alinhadas. O resultado final é paginado em blocos de 12 em 12 linhas, travando a tela com uma mensagem para o usuário pressionar ENTER e simular a paginação de registros por demanda (*cursors*).

---

## Tecnologias Utilizadas

- Linguagem de Programação: C++
- Sistema de Build: CMake
- Versionamento: Git

## Autores

- **Jonathan Duarte Uchôa**
- **Samuel Augusto de Abreu**
