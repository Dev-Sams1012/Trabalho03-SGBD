#pragma once

#include "pagina.hpp"
#include <array>

// Número total de frames de buffer disponíveis, conforme especificação (B = 5).
constexpr int B = 5;

// Gerencia os B frames de buffer em memória principal.
// Durante a ordenação externa:
//   - Frames 0..B-2 são usados como entrada (até 4 runs simultâneas).
//   - Frame B-1 é o buffer de saída.
// Durante o merge join:
//   - Frame 0: página atual da tabela esquerda.
//   - Frame 1: página atual da tabela direita.
//   - Frame 2: buffer de saída do resultado.
class Buffer
{
public:
    std::array<Pagina, B> frames;

    Buffer();

    Pagina &frame(int i);
    const Pagina &frame(int i) const;

    void limpar_frame(int i);
    void limpar_todos();
};
