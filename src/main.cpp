#include <iostream>
#include <cstdio>
#include <string>

#include "include/Labirinto/Labirinto.h"
#include "include/Labirinto/MazeGenerator.h"
#include "include/Grafo/Grafo.h"
#include "include/Algoritmos/BFS.h"
#include "include/Algoritmos/DFS.h"
#include "include/Algoritmos/Dijkstra.h"
#include "include/Algoritmos/ComponentesConectados.h"
#include "include/Algoritmos/GrafoReverso.h"
#include "include/Utils/Timer.h"

#ifdef WITH_SFML
#include "Visualizacao/MazeRenderer.h"
#endif

static ResultadoDaBusca runTimed(AlgoritmoDeBusca& algoritmo, int inicio, int fim) {
    Timer t;
    t.start();
    ResultadoDaBusca res = algoritmo.solve(inicio, fim);
    res.tempoEmMs = t.elapsedMs();
    return res;
}

static long custoReal(const Labirinto& m, const VetorDinamico<int>& caminho) {
    long soma = 0;
    for (std::size_t i = 1; i < caminho.size(); ++i)
        soma += m.costAt(caminho[i]);
    return soma;
}

int main() {
    const int H = 12, W = 20;
    const unsigned SEED = 2025;

    std::cout << "==========================================================\n";
    std::cout << "  Labirintos como grafos:  BFS  x  DFS  x  Dijkstra\n";
    std::cout << "==========================================================\n";
    std::printf("Gerando e resolvendo %d tipos de labirinto (%dx%d), seed %u\n\n",
                MazeGenerator::typeCount(), 2 * H + 1, 2 * W + 1, SEED);

    std::printf("%-3s %-22s | %-13s | %-13s | %-13s\n",
                "#", "TIPO", "BFS", "DFS", "DIJKSTRA");
    std::printf("%-3s %-22s | %-13s | %-13s | %-13s\n",
                "", "", "passos custo", "passos custo", "passos custo");
    for (int i = 0; i < 78; ++i) std::cout << '-';
    std::cout << "\n";

#ifdef WITH_SFML
    MazeRenderer renderer(24);
#endif

    for (int i = 0; i < MazeGenerator::typeCount(); ++i) {
        MazeType tipo = MazeGenerator::typeAt(i);
        Labirinto maze = MazeGenerator::generate(tipo, H, W, SEED);

        Grafo graph = maze.toGraph();
        int inicio = maze.startId();
        int fim    = maze.endId();

        BFS      bfs(graph);
        DFS      dfs(graph);
        Dijkstra dij(graph);

        ResultadoDaBusca rBfs = runTimed(bfs, inicio, fim);
        ResultadoDaBusca rDfs = runTimed(dfs, inicio, fim);
        ResultadoDaBusca rDij = runTimed(dij, inicio, fim);

        long realBfs = custoReal(maze, rBfs.caminho);
        long realDfs = custoReal(maze, rDfs.caminho);
        long realDij = custoReal(maze, rDij.caminho);

        std::printf("%-3d %-22s | %5zu %6ld | %5zu %6ld | %5zu %6ld\n",
                    i + 1, MazeGenerator::typeName(tipo),
                    rBfs.caminho.size(), realBfs,
                    rDfs.caminho.size(), realDfs,
                    rDij.caminho.size(), realDij);

#ifdef WITH_SFML
        int cena = renderer.addMaze(MazeGenerator::typeName(tipo), maze);
        renderer.addResult(cena, "BFS", rBfs);
        renderer.addResult(cena, "DFS", rDfs);
        renderer.addResult(cena, "Dijkstra", rDij);
#endif
    }

    for (int i = 0; i < 78; ++i) std::cout << '-';
    std::cout << "\n\n";

    std::cout << "==========================================================\n";
    std::cout << "  Componentes Conectados e Grafo Reverso\n";
    std::cout << "==========================================================\n";

    Labirinto mazeExemplo = MazeGenerator::generate(MazeType::RecursiveBacktracker, H, W, SEED);
    Grafo grafoExemplo    = mazeExemplo.toGraph();

    // --- Componentes Conectados ---
    ComponentesConectados cc(grafoExemplo);
    int totalComponentes = cc.contarComponentes();
    std::cout << "Labirinto Recursive Backtracker (" << 2*H+1 << "x" << 2*W+1 << "):\n";
    std::cout << "  Total de nos no grafo: " << grafoExemplo.contagemDeNos() << "\n";
    std::cout << "  Componentes encontrados: " << totalComponentes << "\n";
    std::cout << "  (O grafo inclui celulas de parede como nos isolados --\n";
    std::cout << "   cada parede vira um componente proprio, por isso o numero\n";
    std::cout << "   alto. Os nos caminhaveis formam 1 componente conectado.)\n\n";

    VetorDinamico<int> rotulo = cc.calcular();
    int componenteCaminhavel = rotulo[mazeExemplo.startId()];
    int nosCaminhaveis = 0;
    for (std::size_t i = 0; i < rotulo.size(); ++i)
        if (rotulo[i] == componenteCaminhavel) ++nosCaminhaveis;
    std::cout << "  Nos no mesmo componente que o inicio: " << nosCaminhaveis << "\n";
    std::cout << "  (Todos os caminhos do labirinto se conectam -- confirma\n";
    std::cout << "   que o labirinto e solucavel de qualquer ponto.)\n\n";

    // --- Grafo Reverso ---
    Grafo reverso = GrafoReverso::construir(grafoExemplo);
    std::cout << "Grafo Reverso do Backtracker:\n";
    std::cout << "  Nos no original: " << grafoExemplo.contagemDeNos() << "\n";
    std::cout << "  Nos no reverso:  " << reverso.contagemDeNos() << "\n";

    int noTeste = mazeExemplo.startId();
    std::cout << "  No de inicio (id=" << noTeste << "):\n";
    std::cout << "    Vizinhos no original: ";
    const VetorDinamico<Aresta>& vizOriginal = grafoExemplo.vizinhos(noTeste);
    for (std::size_t i = 0; i < vizOriginal.size(); ++i)
        std::cout << vizOriginal[i].destino << " ";
    std::cout << "\n";
    std::cout << "    Vizinhos no reverso:  ";
    const VetorDinamico<Aresta>& vizReverso = reverso.vizinhos(noTeste);
    for (std::size_t i = 0; i < vizReverso.size(); ++i)
        std::cout << vizReverso[i].destino << " ";
    std::cout << "\n";
    std::cout << "  (No reverso, as arestas que SAIAM do no agora CHEGAM nele.)\n\n";

    std::cout 
        "Leitura: 'custo' = custo REAL do caminho (peso do terreno somado).\n"
        "Sem lama, custo == passos-1, e os tres coincidem em custo (BFS=Dijkstra).\n"
        "Em 'Braided' e 'Salas', o DFS acha um caminho bem mais LONGO (ha varias\n"
        "rotas e ele nao busca a mais curta). Em 'Terreno com Lama', o BFS faz\n"
        "menos passos mas ATRAVESSA a lama (custo alto); o Dijkstra DESVIA: mais\n"
        "passos, porem custo real menor.\n";

#ifdef WITH_SFML
    std::cout 
        "\nAbrindo a janela grafica...\n"
        "  SETAS <- / ->  : troca o TIPO de labirinto\n"
        "  1 / 2 / 3      : BFS / DFS / Dijkstra\n"
        "  R              : reinicia a animacao\n"
        "  + / -          : aumenta / diminui a velocidade\n"
        "  ESC            : sair\n";
    renderer.run();
#else
    std::cout 
        "\n(Visualizacao grafica desabilitada.\n"
        " Compile com -DWITH_SFML=ON no CLion/CMake para ver a animacao.)\n";
#endif

    return 0;
}