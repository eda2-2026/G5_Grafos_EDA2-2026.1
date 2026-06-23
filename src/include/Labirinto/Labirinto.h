#pragma once
#include "../EstruturasDeDados/VetorDinamico.h"
#include "../Grafo/Grafo.h"
#include <string>

/**
 * Tipo de cada célula do labirinto.
 *   - Mud (lama): célula caminhável, porém com CUSTO de entrada alto.
 *     É o que permite ao Dijkstra encontrar um caminho diferente do BFS,
 *     já que o BFS ignora pesos e a lama "empurra" o Dijkstra para desvios.
 */
enum class TiposDeCelula { Wall, Path, Start, End, Mud };

/**
 * Maze (Labirinto)
 * ----------------
 * Representa o labirinto como uma GRADE rows x cols. Cada célula tem:
 *   - um tipo (parede, caminho, início ou fim);
 *   - um custo de entrada (usado pelo Dijkstra para simular terrenos:
 *     ex.: 1 = piso normal, 5 = lama). Por padrão todo custo é 1.
 *
 * Mapeamento célula <-> nó do grafo (bijeção):
 *     id  = linha * cols + coluna
 *     linha = id / cols      coluna = id % cols
 *
 * O método toGraph() é o coração da modelagem: transforma a grade em um
 * Graph ligando cada célula caminhável às vizinhas caminháveis.
 */
class Labirinto {
private:
    int linhas_;
    int colunas_;
    VetorDinamico<TiposDeCelula> grade_; // tamanho linhas*colunas
    VetorDinamico<int>      custo_; // custo de entrar em cada célula
    int idInicio_;
    int idFim_;

public:
    // Construtor padrão (labirinto "vazio"): necessário apenas para que o
    // Maze possa ser guardado dentro de um VetorDinamico (ver MazeRenderer,
    // que mantém vários labirintos para alternar na tela).
    Labirinto() : linhas_(0), colunas_(0), idInicio_(-1), idFim_(-1) {}

    Labirinto(int linhas, int colunas)
        : linhas_(linhas),
          colunas_(colunas),
          grade_(static_cast<std::size_t>(linhas) * colunas, TiposDeCelula::Path),
          custo_(static_cast<std::size_t>(linhas) * colunas, 1),
          idInicio_(-1),
          idFim_(-1) {}

    int linhas() const { return linhas_; }
    int colunas() const { return colunas_; }

    int id(int linha, int coluna) const { return linha * colunas_ + coluna; }
    int rowOf(int id) const { return id / colunas_; }
    int colOf(int id) const { return id % colunas_; }

    TiposDeCelula cellAt(int id) const { return grade_[id]; }
    int      costAt(int id) const { return custo_[id]; }

    int startId() const { return idInicio_; }
    int endId()   const { return idFim_; }

    void setCell(int linha, int coluna, TiposDeCelula tipo) {
        int i = id(linha, coluna);
        grade_[i] = tipo;
        if (tipo == TiposDeCelula::Start) idInicio_ = i;
        if (tipo == TiposDeCelula::End)   idFim_ = i;
    }

    void setCost(int linha, int coluna, int custo) {
        custo_[id(linha, coluna)] = custo;
    }

    bool isWalkable(int linha, int coluna) const {
        if (linha < 0 || linha >= linhas_ || coluna < 0 || coluna >= colunas_)
            return false;
        return grade_[id(linha, coluna)] != TiposDeCelula::Wall;
    }

    /**
     * Constrói o grafo correspondente ao labirinto.
     * Vizinhança de 4 direções; o peso de cada aresta é o custo de
     * entrar na célula de destino.
     */
    Grafo toGraph() const {
        Grafo g(linhas_ * colunas_);
        const int dl[4] = {-1, 1, 0, 0};
        const int dc[4] = {0, 0, -1, 1};
        for (int l = 0; l < linhas_; ++l) {
            for (int c = 0; c < colunas_; ++c) {
                if (!isWalkable(l, c)) continue;
                for (int k = 0; k < 4; ++k) {
                    int nl = l + dl[k];
                    int nc = c + dc[k];
                    if (isWalkable(nl, nc)) {
                        g.adicionarArestaDirecionada(id(l, c), id(nl, nc), custo_[id(nl, nc)]);
                    }
                }
            }
        }
        return g;
    }

    /**
     * Cria um labirinto a partir de linhas de texto:
     *   '#' = parede   'S' = início   'E' = fim   (qualquer outro) = caminho
     * Útil para testar com labirintos fixos.
     */
    static Labirinto fromText(const VetorDinamico<std::string>& linhasTxt) {
        int nLinhas = static_cast<int>(linhasTxt.size());
        int nColunas = 0;
        for (int i = 0; i < nLinhas; ++i)
            if (static_cast<int>(linhasTxt[i].size()) > nColunas)
                nColunas = static_cast<int>(linhasTxt[i].size());

        Labirinto m(nLinhas, nColunas);
        for (int i = 0; i < nLinhas; ++i) {
            for (int j = 0; j < nColunas; ++j) {
                char ch = (j < static_cast<int>(linhasTxt[i].size())) ? linhasTxt[i][j] : '#';
                switch (ch) {
                    case '#': m.setCell(i, j, TiposDeCelula::Wall);  break;
                    case 'S': m.setCell(i, j, TiposDeCelula::Start); break;
                    case 'E': m.setCell(i, j, TiposDeCelula::End);   break;
                    case 'M': m.setCell(i, j, TiposDeCelula::Mud);
                              m.setCost(i, j, 30);              break;
                    default:  m.setCell(i, j, TiposDeCelula::Path);  break;
                }
            }
        }
        return m;
    }
};
