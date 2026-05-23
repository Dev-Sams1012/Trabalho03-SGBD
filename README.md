# Sort-Merge Join - SGBD Trabalho 03

## Descrição do Trabalho

O objetivo deste trabalho é implementar o operador da álgebra relacional Sort-Merge Join para a junção de duas tabelas (Grapes e Wines), sem a utilização de SGBDs externos. O algoritmo realiza a ordenação externa com limite estrito de 5 frames de buffer e executa a junção através da fase de intercalação (Merge Phase) e do método Merge Join.

## Estrutura do Projeto

```bash
├── data/
│   ├── grapes.csv                 # Tabela Grapes
│   └── wines.csv                  # Tabela Wines
├── include/
│   ├── SortMergeJoin.hpp          # Operador principal
│   ├── Tabela.hpp                  # Estrutura de tabela
│   ├── Pagina.hpp                  # Página com 12 tuplas
│   ├── Tupla.hpp                   # Estrutura de tupla
│   └── Esquema.hpp                 # Metadados da tabela
└── src/
    ├── main.cpp                    # Programa principal
    ├── SortMergeJoin.cpp           # Implementação do SMJ
    ├── Tabela.cpp
    ├── Pagina.cpp
    ├── Tupla.cpp
    └── Esquema.cpp
```

## Funcionamento do Sort-Merge Join

### O algoritmo é dividido em três fases obrigatórias

* **Sort Phase**: As tabelas são divididas em blocos de 5 páginas. Cada bloco é carregado no buffer, ordenado em memória e persistido como uma run no disco.

* **Merge Phase**: Realiza a intercalação das runs ordenadas utilizando 4 buffers de entrada e 1 buffer de saída, garantindo que o limite de 5 frames seja respeitado.

* **Merge Join**: Percorre as tabelas ordenadas de forma sequencial. Quando encontra chaves iguais, processa as partições (blocos de chaves idênticas) para gerar todas as combinações possíveis entre as tabelas.

## Gestão de Buffer

A gestão de buffer é realizada através da estrutura PageCursor, que implementa o carregamento on-demand. O sistema solicita páginas individuais ao objeto Tabela conforme a necessidade do algoritmo, garantindo que o consumo de memória nunca exceda o limite definido de 5 frames (Páginas).

## Como Compilar e Executar

### Pré-requisitos

* Compilador C++ com suporte a C++17
* CMake 3.10 ou superior

### Compilação

```bash
mkdir build
cd build
cmake ..
make
```

### Execução

Antes de executar, certifique-se de que os arquivos `grapes.csv` e `wines.csv` estejam presentes na pasta `data/`.

```bash
./SortMergeJoin
```

## Tecnologias Utilizadas

* Linguagem de Programação: C++
* Sistema de Build: CMake
* Versionamento: Git

## Autores

* **Jonathan Duarte Uchôa**
* **Samuel Augusto de Abreu**
