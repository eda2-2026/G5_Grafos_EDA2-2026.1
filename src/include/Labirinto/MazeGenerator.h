#pragma once
#include "./Labirinto.h"
#include "../EstruturasDeDados/Pilha.h"
#include "../EstruturasDeDados/VetorDinamico.h"
#include <cstdlib>
#include <algorithm>

/**
 * MazeType
 * --------
 * Os diferentes ALGORITMOS/ESTILOS de geração de labirinto disponíveis.
 * Cada um produz uma "textura" diferente, o que torna a comparação entre
 * BFS, DFS e Dijkstra muito mais interessante:
 *
 *   RecursiveBacktracker - corredores longos e sinuosos (poucos becos).
 *   Prim                 - muito ramificado, vários becos curtos.
 *   Kruskal              - perfeito e uniforme (via union-find).
 *   RecursiveDivision    - aspecto de "salas" retangulares.
 *   BinaryTree           - simples, com forte viés diagonal.
 *   Sidewinder           - viés horizontal, corredor livre no topo.
 *   HuntAndKill          - perfeito, textura distinta do backtracker.
 *   Wilson               - passeio aleatório sem viés (uniforme).
 *   Eller                - gerado linha a linha (muito eficiente).
 *   Braided              - COM CICLOS: vários caminhos possíveis!
 *   WeightedTerrain      - com poças de lama (custo alto) -> Dijkstra != BFS.
 *   Rooms                - masmorra: salas conectadas por corredores.
 *
 * Os tipos "Braided", "WeightedTerrain" e "Rooms" são os que mais
 * evidenciam as diferenças entre os algoritmos, pois deixam de ter
 * caminho único entre início e fim.
 */
enum class MazeType {
    RecursiveBacktracker = 0,
    Prim,
    Kruskal,
    RecursiveDivision,
    BinaryTree,
    Sidewinder,
    HuntAndKill,
    Wilson,
    Eller,
    Braided,
    WeightedTerrain,
    Rooms,
    COUNT // sentinela: quantidade de tipos (mantém sempre por último)
};

/**
 * MazeGenerator
 * -------------
 * Fábrica de labirintos. Um labirinto de h x w células LÓGICAS é mapeado
 * para uma grade FÍSICA (2h+1) x (2w+1):
 *   - posições (ímpar, ímpar) são as células;
 *   - posições pares entre duas células são as paredes/passagens.
 *
 * Assim, "abrir a parede" entre duas células vizinhas é apenas marcar a
 * posição intermediária como caminho.
 *
 * A maior parte dos algoritmos usa as nossas próprias estruturas de dados
 * (Stack, VetorDinamico) — mais um uso prático delas no projeto.
 */
class MazeGenerator {
public:
    // -------- API principal --------

    // Gera um labirinto do tipo desejado.
    static Labirinto generate(MazeType tipo, int h, int w, unsigned seed = 12345) {
        std::srand(seed);
        Labirinto m(2 * h + 1, 2 * w + 1);

        switch (tipo) {
            case MazeType::RecursiveBacktracker: gerarBacktracker(m, h, w); break;
            case MazeType::Prim:                 gerarPrim(m, h, w);        break;
            case MazeType::Kruskal:              gerarKruskal(m, h, w);     break;
            case MazeType::RecursiveDivision:    gerarDivisao(m, h, w);     break;
            case MazeType::BinaryTree:           gerarArvoreBinaria(m, h, w); break;
            case MazeType::Sidewinder:           gerarSidewinder(m, h, w);  break;
            case MazeType::HuntAndKill:          gerarHuntKill(m, h, w);    break;
            case MazeType::Wilson:               gerarWilson(m, h, w);      break;
            case MazeType::Eller:                gerarEller(m, h, w);       break;
            case MazeType::Braided:              gerarBraided(m, h, w);     break;
            case MazeType::WeightedTerrain:      gerarTerrenoPesado(m, h, w); break;
            case MazeType::Rooms:                gerarSalas(m, h, w);       break;
            default:                             gerarBacktracker(m, h, w); break;
        }

        definirInicioFim(m, h, w);
        return m;
    }

    // Sobrecarga compatível com a versão antiga (Recursive Backtracker).
    static Labirinto generate(int h, int w, unsigned seed = 12345) {
        return generate(MazeType::RecursiveBacktracker, h, w, seed);
    }

    // Nome legível do tipo (para títulos e relatórios).
    static const char* typeName(MazeType t) {
        switch (t) {
            case MazeType::RecursiveBacktracker: return "Recursive Backtracker";
            case MazeType::Prim:                 return "Prim";
            case MazeType::Kruskal:              return "Kruskal";
            case MazeType::RecursiveDivision:    return "Divisao Recursiva";
            case MazeType::BinaryTree:           return "Arvore Binaria";
            case MazeType::Sidewinder:           return "Sidewinder";
            case MazeType::HuntAndKill:          return "Hunt and Kill";
            case MazeType::Wilson:               return "Wilson";
            case MazeType::Eller:                return "Eller";
            case MazeType::Braided:              return "Braided (com ciclos)";
            case MazeType::WeightedTerrain:      return "Terreno com Lama";
            case MazeType::Rooms:                return "Salas (masmorra)";
            default:                             return "Desconhecido";
        }
    }

    // Quantidade de tipos disponíveis.
    static int typeCount() { return static_cast<int>(MazeType::COUNT); }

    // Converte um índice (0..typeCount-1) em MazeType.
    static MazeType typeAt(int i) { return static_cast<MazeType>(i); }

private:
    // ============================================================
    //  Utilidades de baixo nível (grade física)
    // ============================================================

    // Marca todas as posições como parede.
    static void preencherParedes(Labirinto& m) {
        for (int l = 0; l < m.linhas(); ++l)
            for (int c = 0; c < m.colunas(); ++c)
                m.setCell(l, c, TiposDeCelula::Wall);
    }

    // Abre (vira caminho) a célula lógica (r, c).
    static void abrirCelula(Labirinto& m, int r, int c) {
        m.setCell(2 * r + 1, 2 * c + 1, TiposDeCelula::Path);
    }

    // Abre a parede entre duas células lógicas adjacentes.
    static void abrirParede(Labirinto& m, int r1, int c1, int r2, int c2) {
        int pl = (2 * r1 + 1 + 2 * r2 + 1) / 2;
        int pc = (2 * c1 + 1 + 2 * c2 + 1) / 2;
        m.setCell(pl, pc, TiposDeCelula::Path);
    }

    // Define início (canto superior esquerdo) e fim (inferior direito).
    static void definirInicioFim(Labirinto& m, int h, int w) {
        m.setCell(1, 1, TiposDeCelula::Start);
        m.setCell(2 * h - 1, 2 * w - 1, TiposDeCelula::End);
    }

    // Embaralha um vetor de 4 direções (Fisher-Yates).
    static void embaralhar4(int ordem[4]) {
        for (int i = 3; i > 0; --i) {
            int j = std::rand() % (i + 1);
            int t = ordem[i]; ordem[i] = ordem[j]; ordem[j] = t;
        }
    }

    // ============================================================
    //  1) Recursive Backtracker (DFS iterativo com a nossa Stack)
    // ============================================================
    static void gerarBacktracker(Labirinto& m, int h, int w) {
        preencherParedes(m);
        VetorDinamico<bool> vis(static_cast<std::size_t>(h) * w, false);
        Pilha<int> pilha;

        vis[0] = true;
        abrirCelula(m, 0, 0);
        pilha.empilha(0);

        const int dl[4] = {-1, 1, 0, 0};
        const int dc[4] = {0, 0, -1, 1};

        while (!pilha.vazio()) {
            int atual = pilha.top();
            int r = atual / w, c = atual % w;

            int ordem[4] = {0, 1, 2, 3};
            embaralhar4(ordem);

            bool avancou = false;
            for (int k = 0; k < 4; ++k) {
                int d = ordem[k];
                int nr = r + dl[d], nc = c + dc[d];
                if (nr >= 0 && nr < h && nc >= 0 && nc < w && !vis[nr * w + nc]) {
                    vis[nr * w + nc] = true;
                    abrirParede(m, r, c, nr, nc);
                    abrirCelula(m, nr, nc);
                    pilha.empilha(nr * w + nc);
                    avancou = true;
                    break;
                }
            }
            if (!avancou) pilha.desempilha();
        }
    }

    // ============================================================
    //  2) Prim aleatorizado (cresce a partir de uma célula)
    // ============================================================
    struct Borda { int r, c, pr, pc; }; // célula nova (r,c) acessível de (pr,pc)

    static void gerarPrim(Labirinto& m, int h, int w) {
        preencherParedes(m);
        VetorDinamico<bool> vis(static_cast<std::size_t>(h) * w, false);
        VetorDinamico<Borda> fronteira;

        const int dl[4] = {-1, 1, 0, 0};
        const int dc[4] = {0, 0, -1, 1};

        vis[0] = true;
        abrirCelula(m, 0, 0);
        for (int k = 0; k < 4; ++k) {
            int nr = 0 + dl[k], nc = 0 + dc[k];
            if (nr >= 0 && nr < h && nc >= 0 && nc < w) {
                Borda b{nr, nc, 0, 0};
                fronteira.insereElemento(b);
            }
        }

        while (fronteira.size() > 0) {
            int idx = std::rand() % static_cast<int>(fronteira.size());
            Borda b = fronteira[idx];
            // remoção O(1): troca pelo último e descarta
            fronteira[static_cast<std::size_t>(idx)] = fronteira[fronteira.size() - 1];
            fronteira.retiraElemento();

            if (vis[b.r * w + b.c]) continue;
            vis[b.r * w + b.c] = true;
            abrirParede(m, b.pr, b.pc, b.r, b.c);
            abrirCelula(m, b.r, b.c);

            for (int k = 0; k < 4; ++k) {
                int nr = b.r + dl[k], nc = b.c + dc[k];
                if (nr >= 0 && nr < h && nc >= 0 && nc < w && !vis[nr * w + nc]) {
                    Borda nb{nr, nc, b.r, b.c};
                    fronteira.insereElemento(nb);
                }
            }
        }
    }

    // ============================================================
    //  3) Kruskal aleatorizado (union-find sobre as células)
    // ============================================================
    struct ArestaK { int r1, c1, r2, c2; };

    static int achar(VetorDinamico<int>& pai, int x) {
        while (pai[static_cast<std::size_t>(x)] != x) {
            pai[static_cast<std::size_t>(x)] =
                pai[static_cast<std::size_t>(pai[static_cast<std::size_t>(x)])];
            x = pai[static_cast<std::size_t>(x)];
        }
        return x;
    }

    static void gerarKruskal(Labirinto& m, int h, int w) {
        preencherParedes(m);
        VetorDinamico<int> pai(static_cast<std::size_t>(h) * w, 0);
        for (int i = 0; i < h * w; ++i) pai[static_cast<std::size_t>(i)] = i;

        VetorDinamico<ArestaK> arestas;
        for (int r = 0; r < h; ++r) {
            for (int c = 0; c < w; ++c) {
                abrirCelula(m, r, c);
                if (c + 1 < w) { ArestaK a{r, c, r, c + 1}; arestas.insereElemento(a); }
                if (r + 1 < h) { ArestaK a{r, c, r + 1, c}; arestas.insereElemento(a); }
            }
        }
        // embaralha as arestas
        for (int i = static_cast<int>(arestas.size()) - 1; i > 0; --i) {
            int j = std::rand() % (i + 1);
            ArestaK t = arestas[static_cast<std::size_t>(i)];
            arestas[static_cast<std::size_t>(i)] = arestas[static_cast<std::size_t>(j)];
            arestas[static_cast<std::size_t>(j)] = t;
        }
        for (std::size_t i = 0; i < arestas.size(); ++i) {
            ArestaK a = arestas[i];
            int x = achar(pai, a.r1 * w + a.c1);
            int y = achar(pai, a.r2 * w + a.c2);
            if (x != y) {
                pai[static_cast<std::size_t>(x)] = y;
                abrirParede(m, a.r1, a.c1, a.r2, a.c2);
            }
        }
    }

    // ============================================================
    //  4) Divisão Recursiva (começa aberto e adiciona paredes)
    // ============================================================
    static void gerarDivisao(Labirinto& m, int h, int w) {
        (void)h; (void)w; // dimensões vêm de m.rows()/m.cols()
        int R = m.linhas(), C = m.colunas();
        // tudo aberto...
        for (int r = 0; r < R; ++r)
            for (int c = 0; c < C; ++c)
                m.setCell(r, c, TiposDeCelula::Path);
        // ...exceto a borda externa
        for (int c = 0; c < C; ++c) { m.setCell(0, c, TiposDeCelula::Wall); m.setCell(R - 1, c, TiposDeCelula::Wall); }
        for (int r = 0; r < R; ++r) { m.setCell(r, 0, TiposDeCelula::Wall); m.setCell(r, C - 1, TiposDeCelula::Wall); }

        divisaoRec(m, 1, 1, R - 2, C - 2);
    }

    // Divide a câmara delimitada pelas células físicas (r0,c0)-(r1,c1) (ímpares).
    static void divisaoRec(Labirinto& m, int r0, int c0, int r1, int c1) {
        int alturaCels = (r1 - r0) / 2 + 1;
        int largCels   = (c1 - c0) / 2 + 1;
        if (alturaCels < 2 && largCels < 2) return;

        bool horizontal;
        if (largCels < alturaCels)      horizontal = true;
        else if (alturaCels < largCels) horizontal = false;
        else                            horizontal = (std::rand() % 2 == 0);
        if (alturaCels < 2) horizontal = false; // não dá pra dividir na horizontal
        if (largCels < 2)   horizontal = true;  // não dá pra dividir na vertical

        if (horizontal) {
            int nOpc = (r1 - 1 - (r0 + 1)) / 2 + 1;      // linhas-parede pares candidatas
            int wr   = r0 + 1 + 2 * (std::rand() % nOpc);
            int nGap = (c1 - c0) / 2 + 1;                // colunas ímpares para a passagem
            int gap  = c0 + 2 * (std::rand() % nGap);
            for (int c = c0 - 1; c <= c1 + 1; ++c) m.setCell(wr, c, TiposDeCelula::Wall);
            m.setCell(wr, gap, TiposDeCelula::Path);
            divisaoRec(m, r0, c0, wr - 1, c1);
            divisaoRec(m, wr + 1, c0, r1, c1);
        } else {
            int nOpc = (c1 - 1 - (c0 + 1)) / 2 + 1;
            int wc   = c0 + 1 + 2 * (std::rand() % nOpc);
            int nGap = (r1 - r0) / 2 + 1;
            int gap  = r0 + 2 * (std::rand() % nGap);
            for (int r = r0 - 1; r <= r1 + 1; ++r) m.setCell(r, wc, TiposDeCelula::Wall);
            m.setCell(gap, wc, TiposDeCelula::Path);
            divisaoRec(m, r0, c0, r1, wc - 1);
            divisaoRec(m, r0, wc + 1, r1, c1);
        }
    }

    // ============================================================
    //  5) Árvore Binária (para cada célula, abre Norte OU Leste)
    // ============================================================
    static void gerarArvoreBinaria(Labirinto& m, int h, int w) {
        preencherParedes(m);
        for (int r = 0; r < h; ++r) {
            for (int c = 0; c < w; ++c) {
                abrirCelula(m, r, c);
                bool podeNorte = (r > 0);
                bool podeLeste = (c < w - 1);
                if (podeNorte && podeLeste) {
                    if (std::rand() % 2) abrirParede(m, r, c, r - 1, c);
                    else                 abrirParede(m, r, c, r, c + 1);
                } else if (podeNorte) {
                    abrirParede(m, r, c, r - 1, c);
                } else if (podeLeste) {
                    abrirParede(m, r, c, r, c + 1);
                }
            }
        }
    }

    // ============================================================
    //  6) Sidewinder (agrupa em "runs" horizontais; sobe uma por run)
    // ============================================================
    static void gerarSidewinder(Labirinto& m, int h, int w) {
        preencherParedes(m);
        for (int r = 0; r < h; ++r) {
            int inicioRun = 0;
            for (int c = 0; c < w; ++c) {
                abrirCelula(m, r, c);
                bool naBordaLeste = (c == w - 1);
                bool naBordaNorte = (r == 0);
                bool fecharRun = naBordaLeste || (!naBordaNorte && (std::rand() % 2 == 0));
                if (fecharRun) {
                    if (!naBordaNorte) {
                        int colNorte = inicioRun + std::rand() % (c - inicioRun + 1);
                        abrirParede(m, r, colNorte, r - 1, colNorte);
                    }
                    inicioRun = c + 1;
                } else {
                    abrirParede(m, r, c, r, c + 1); // estende a run para leste
                }
            }
        }
    }

    // ============================================================
    //  7) Hunt and Kill (caminhada aleatória + "caça" por células novas)
    // ============================================================
    static void gerarHuntKill(Labirinto& m, int h, int w) {
        preencherParedes(m);
        VetorDinamico<bool> vis(static_cast<std::size_t>(h) * w, false);
        const int dl[4] = {-1, 1, 0, 0};
        const int dc[4] = {0, 0, -1, 1};

        int r = 0, c = 0;
        vis[0] = true;
        abrirCelula(m, 0, 0);

        while (true) {
            int ordem[4] = {0, 1, 2, 3};
            embaralhar4(ordem);
            bool moveu = false;
            for (int k = 0; k < 4; ++k) {
                int d = ordem[k];
                int nr = r + dl[d], nc = c + dc[d];
                if (nr >= 0 && nr < h && nc >= 0 && nc < w && !vis[nr * w + nc]) {
                    vis[nr * w + nc] = true;
                    abrirParede(m, r, c, nr, nc);
                    abrirCelula(m, nr, nc);
                    r = nr; c = nc;
                    moveu = true;
                    break;
                }
            }
            if (moveu) continue;

            // "Caça": primeira célula não visitada com algum vizinho visitado.
            bool achou = false;
            for (int rr = 0; rr < h && !achou; ++rr) {
                for (int cc = 0; cc < w && !achou; ++cc) {
                    if (vis[rr * w + cc]) continue;
                    int ordem2[4] = {0, 1, 2, 3};
                    embaralhar4(ordem2);
                    for (int k = 0; k < 4; ++k) {
                        int d = ordem2[k];
                        int nr = rr + dl[d], nc = cc + dc[d];
                        if (nr >= 0 && nr < h && nc >= 0 && nc < w && vis[nr * w + nc]) {
                            vis[rr * w + cc] = true;
                            abrirCelula(m, rr, cc);
                            abrirParede(m, rr, cc, nr, nc);
                            r = rr; c = cc;
                            achou = true;
                            break;
                        }
                    }
                }
            }
            if (!achou) break; // não há mais células a visitar
        }
    }

    // ============================================================
    //  8) Wilson (passeio aleatório com apagamento de laços)
    // ============================================================
    static void gerarWilson(Labirinto& m, int h, int w) {
        preencherParedes(m);
        int total = h * w;
        VetorDinamico<bool> dentro(static_cast<std::size_t>(total), false);
        VetorDinamico<int>  dir(static_cast<std::size_t>(total), -1);
        const int dl[4] = {-1, 1, 0, 0};
        const int dc[4] = {0, 0, -1, 1};

        int inicial = std::rand() % total;
        dentro[static_cast<std::size_t>(inicial)] = true;
        abrirCelula(m, inicial / w, inicial % w);
        int restantes = total - 1;

        while (restantes > 0) {
            // escolhe uma célula ainda fora da árvore
            int off = std::rand() % total;
            int inicio = -1;
            for (int i = 0; i < total; ++i) {
                int idx = (off + i) % total;
                if (!dentro[static_cast<std::size_t>(idx)]) { inicio = idx; break; }
            }

            // caminhada aleatória até tocar a árvore, gravando a direção
            int atual = inicio;
            while (!dentro[static_cast<std::size_t>(atual)]) {
                int r = atual / w, c = atual % w;
                int d, nr, nc;
                do {
                    d = std::rand() % 4;
                    nr = r + dl[d];
                    nc = c + dc[d];
                } while (!(nr >= 0 && nr < h && nc >= 0 && nc < w));
                dir[static_cast<std::size_t>(atual)] = d;
                atual = nr * w + nc;
            }

            // percorre de novo a partir de 'inicio' seguindo dir[]:
            // laços são automaticamente apagados (direção foi sobrescrita).
            atual = inicio;
            while (!dentro[static_cast<std::size_t>(atual)]) {
                int r = atual / w, c = atual % w;
                int d = dir[static_cast<std::size_t>(atual)];
                int nr = r + dl[d], nc = c + dc[d];
                dentro[static_cast<std::size_t>(atual)] = true;
                abrirCelula(m, r, c);
                abrirParede(m, r, c, nr, nc);
                --restantes;
                atual = nr * w + nc;
            }
        }
    }

    // ============================================================
    //  9) Eller (gera o labirinto linha a linha usando conjuntos)
    // ============================================================
    static void gerarEller(Labirinto& m, int h, int w) {
        preencherParedes(m);
        VetorDinamico<int> conj(static_cast<std::size_t>(w), 0);
        int proxId = 1;
        for (int c = 0; c < w; ++c) conj[static_cast<std::size_t>(c)] = proxId++;

        for (int r = 0; r < h; ++r) {
            for (int c = 0; c < w; ++c) abrirCelula(m, r, c);
            bool ultimaLinha = (r == h - 1);

            // 1) une vizinhos horizontais de conjuntos diferentes
            for (int c = 0; c + 1 < w; ++c) {
                bool unir = ultimaLinha ? true : (std::rand() % 2 == 0);
                if (conj[static_cast<std::size_t>(c)] != conj[static_cast<std::size_t>(c + 1)] && unir) {
                    int velho = conj[static_cast<std::size_t>(c + 1)];
                    int novo  = conj[static_cast<std::size_t>(c)];
                    for (int k = 0; k < w; ++k)
                        if (conj[static_cast<std::size_t>(k)] == velho)
                            conj[static_cast<std::size_t>(k)] = novo;
                    abrirParede(m, r, c, r, c + 1);
                }
            }

            if (ultimaLinha) break;

            // 2) para cada conjunto, garante PELO MENOS uma descida
            VetorDinamico<int>  desce(static_cast<std::size_t>(w), 0);
            VetorDinamico<bool> tratado(static_cast<std::size_t>(w), false);
            for (int a = 0; a < w; ++a) {
                if (tratado[static_cast<std::size_t>(a)]) continue;
                int idc = conj[static_cast<std::size_t>(a)];
                VetorDinamico<int> cols;
                for (int b = 0; b < w; ++b)
                    if (conj[static_cast<std::size_t>(b)] == idc) {
                        cols.insereElemento(b);
                        tratado[static_cast<std::size_t>(b)] = true;
                    }
                int qtd = 1 + std::rand() % static_cast<int>(cols.size());
                for (int i = static_cast<int>(cols.size()) - 1; i > 0; --i) {
                    int j = std::rand() % (i + 1);
                    int t = cols[static_cast<std::size_t>(i)];
                    cols[static_cast<std::size_t>(i)] = cols[static_cast<std::size_t>(j)];
                    cols[static_cast<std::size_t>(j)] = t;
                }
                for (int i = 0; i < qtd; ++i)
                    desce[static_cast<std::size_t>(cols[static_cast<std::size_t>(i)])] = 1;
            }

            // 3) aplica as descidas e prepara a próxima linha
            VetorDinamico<int> novoConj(static_cast<std::size_t>(w), 0);
            for (int c = 0; c < w; ++c) {
                if (desce[static_cast<std::size_t>(c)]) {
                    abrirParede(m, r, c, r + 1, c);
                    novoConj[static_cast<std::size_t>(c)] = conj[static_cast<std::size_t>(c)];
                } else {
                    novoConj[static_cast<std::size_t>(c)] = proxId++;
                }
            }
            conj = novoConj;
        }
    }

    // ============================================================
    // 10) Braided (parte de um perfeito e remove becos -> ciclos)
    // ============================================================
    static void gerarBraided(Labirinto& m, int h, int w) {
        gerarBacktracker(m, h, w);
        const int dl[4] = {-1, 1, 0, 0};
        const int dc[4] = {0, 0, -1, 1};

        for (int r = 0; r < h; ++r) {
            for (int c = 0; c < w; ++c) {
                int pr = 2 * r + 1, pc = 2 * c + 1;
                int aberturas = 0;
                for (int k = 0; k < 4; ++k)
                    if (m.isWalkable(pr + dl[k], pc + dc[k])) ++aberturas;

                if (aberturas <= 1) { // beco sem saída
                    int ordem[4] = {0, 1, 2, 3};
                    embaralhar4(ordem);
                    for (int k = 0; k < 4; ++k) {
                        int d = ordem[k];
                        int nr = r + dl[d], nc = c + dc[d];
                        if (nr >= 0 && nr < h && nc >= 0 && nc < w) {
                            int wl = pr + dl[d], wc = pc + dc[d];
                            if (!m.isWalkable(wl, wc)) { // havia parede aqui
                                m.setCell(wl, wc, TiposDeCelula::Path);
                                break;
                            }
                        }
                    }
                }
            }
        }
    }

    // ============================================================
    // 11) Terreno com lama (custo alto) -> Dijkstra diverge do BFS
    //     Usa base BRAIDED (com ciclos) de propósito: assim existem
    //     rotas alternativas e o Dijkstra pode CONTORNAR a lama,
    //     enquanto o BFS atravessa por ter menos arestas.
    // ============================================================
    static void gerarTerrenoPesado(Labirinto& m, int h, int w) {
        gerarBraided(m, h, w);
        for (int r = 0; r < h; ++r) {
            for (int c = 0; c < w; ++c) {
                if ((r == 0 && c == 0) || (r == h - 1 && c == w - 1)) continue; // poupa início/fim
                if (std::rand() % 100 < 25) {
                    int pr = 2 * r + 1, pc = 2 * c + 1;
                    int custo = 10 + std::rand() % 41; // 10..50
                    m.setCell(pr, pc, TiposDeCelula::Mud);
                    m.setCost(pr, pc, custo);
                }
            }
        }
    }

    // ============================================================
    // 12) Salas (masmorra): retângulos abertos ligados por corredores
    // ============================================================
    struct Sala { int r0, c0, r1, c1, cr, cc; };

    static void carvarRet(Labirinto& m, int r0, int c0, int r1, int c1) {
        for (int r = r0; r <= r1; ++r)
            for (int c = c0; c <= c1; ++c)
                if (r > 0 && c > 0 && r < m.linhas() - 1 && c < m.colunas() - 1)
                    m.setCell(r, c, TiposDeCelula::Path);
    }
    static void corredorH(Labirinto& m, int r, int c1, int c2) {
        int a = std::min(c1, c2), b = std::max(c1, c2);
        for (int c = a; c <= b; ++c)
            if (r > 0 && c > 0 && r < m.linhas() - 1 && c < m.colunas() - 1)
                m.setCell(r, c, TiposDeCelula::Path);
    }
    static void corredorV(Labirinto& m, int c, int r1, int r2) {
        int a = std::min(r1, r2), b = std::max(r1, r2);
        for (int r = a; r <= b; ++r)
            if (r > 0 && c > 0 && r < m.linhas() - 1 && c < m.colunas() - 1)
                m.setCell(r, c, TiposDeCelula::Path);
    }

    static void gerarSalas(Labirinto& m, int h, int w) {
        preencherParedes(m);
        int R = m.linhas(), C = m.colunas();
        VetorDinamico<Sala> salas;

        int tentativas = (h * w) / 6 + 4;
        for (int t = 0; t < tentativas; ++t) {
            int sh = 3 + std::rand() % 4; // altura 3..6
            int sw = 3 + std::rand() % 5; // largura 3..7
            int maxR0 = std::max(1, R - 2 - sh);
            int maxC0 = std::max(1, C - 2 - sw);
            int r0 = 1 + std::rand() % maxR0;
            int c0 = 1 + std::rand() % maxC0;
            int r1 = std::min(R - 2, r0 + sh);
            int c1 = std::min(C - 2, c0 + sw);
            Sala s{r0, c0, r1, c1, (r0 + r1) / 2, (c0 + c1) / 2};
            carvarRet(m, r0, c0, r1, c1);
            salas.insereElemento(s);
        }

        // conecta as salas em sequência (garante labirinto conexo)
        for (std::size_t i = 1; i < salas.size(); ++i) {
            int cr = salas[i].cr, cc = salas[i].cc;
            int pr = salas[i - 1].cr, pc = salas[i - 1].cc;
            if (std::rand() % 2) { corredorH(m, pr, pc, cc); corredorV(m, cc, pr, cr); }
            else                 { corredorV(m, pc, pr, cr); corredorH(m, cr, pc, cc); }
        }

        // garante início e fim ligados à rede de salas
        m.setCell(1, 1, TiposDeCelula::Path);
        m.setCell(R - 2, C - 2, TiposDeCelula::Path);
        if (salas.size() > 0) {
            corredorH(m, 1, 1, salas[0].cc);
            corredorV(m, salas[0].cc, 1, salas[0].cr);
            std::size_t u = salas.size() - 1;
            corredorH(m, R - 2, salas[u].cc, C - 2);
            corredorV(m, salas[u].cc, salas[u].cr, R - 2);
        }
    }
};
