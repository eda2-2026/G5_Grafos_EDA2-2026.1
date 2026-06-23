#include <iostream>
#include <cstdio>
#include <string>

#include "include/Labirinto/Labirinto.h"
#include "include/Labirinto/MazeGenerator.h"
#include "include/Grafo/Grafo.h"
#include "include/Algoritmos/BFS.h"
#include "include/Algoritmos/DFS.h"
#include "include/Algoritmos/Dijkstra.h"
#include "include/Utils/Timer.h"

#ifdef WITH_SFML
#include "Visualizacao/MazeRenderer.h"
#endif

/**
 * Executa um algoritmo medindo o tempo. Recebe SearchAlgorithm& (polimorfismo):
 * a mesma função serve para BFS, DFS e Dijkstra.
 */
static ResultadoDaBusca runTimed(AlgoritmoDeBusca& algoritmo, int inicio, int fim) {
    Timer t;
    t.start();
    ResultadoDaBusca res = algoritmo.solve(inicio, fim);
    res.tempoEmMs = t.elapsedMs();
    return res;
}

/**
 * Custo REAL (ponderado) de um caminho: soma o peso de entrar em cada
 * célula, exceto a inicial. Usado para mostrar que o Dijkstra, mesmo
 * dando mais passos, sai mais barato quando há lama no caminho curto.
 */
static long custoReal(const Labirinto& m, const VetorDinamico<int>& caminho) {
    long soma = 0;
    for (std::size_t i = 1; i < caminho.size(); ++i)
        soma += m.costAt(caminho[i]);
    return soma;
}

int main() {
    const int H = 12, W = 20;          // 12x20 células -> grade 25x41
    const unsigned SEED = 2025;        // troque para gerar variações

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

        // custo REAL (ponderado) dos três -> comparação justa.
        // Em labirintos sem lama, custo real == (passos - 1).
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
    std::cout << "\n";
    std::cout <<
        "Leitura: 'custo' = custo REAL do caminho (peso do terreno somado).\n"
        "Sem lama, custo == passos-1, e os tres coincidem em custo (BFS=Dijkstra).\n"
        "Em 'Braided' e 'Salas', o DFS acha um caminho bem mais LONGO (ha varias\n"
        "rotas e ele nao busca a mais curta). Em 'Terreno com Lama', o BFS faz\n"
        "menos passos mas ATRAVESSA a lama (custo alto); o Dijkstra DESVIA: mais\n"
        "passos, porem custo real menor.\n";

#ifdef WITH_SFML
    std::cout <<
        "\nAbrindo a janela grafica...\n"
        "  SETAS <- / ->  : troca o TIPO de labirinto\n"
        "  1 / 2 / 3      : BFS / DFS / Dijkstra\n"
        "  R              : reinicia a animacao\n"
        "  + / -          : aumenta / diminui a velocidade\n"
        "  ESC            : sair\n";
    renderer.run();
#else
    std::cout <<
        "\n(Visualizacao grafica desabilitada.\n"
        " Compile com -DWITH_SFML=ON no CLion/CMake para ver a animacao.)\n";
#endif

    return 0;
}
