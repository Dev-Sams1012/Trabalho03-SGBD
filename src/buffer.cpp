#include "buffer.hpp"

Buffer::Buffer()
{
    // Os frames são inicializados pelos construtores padrão de Pagina (qtd_ocup = 0).
}

Pagina &Buffer::frame(int i)
{
    return frames[i];
}

const Pagina &Buffer::frame(int i) const
{
    return frames[i];
}

void Buffer::limpar_frame(int i)
{
    frames[i].limpar();
}

void Buffer::limpar_todos()
{
    for (Pagina &f : frames)
        f.limpar();
}
