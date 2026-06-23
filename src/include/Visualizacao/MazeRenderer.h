#pragma once
#include "Labirinto/Labirinto.h"
#include "Algoritmos/AlgoritmoDeBusca.h"
#include "EstruturasDeDados/VetorDinamico.h"
#include <SFML/Graphics.hpp>
#include <string>

/**
 * NamedResult
 * -----------
 * Associa um resultado de busca ao nome do algoritmo, para que o
 * renderizador possa alternar entre BFS, DFS e Dijkstra com as teclas.
 */
struct NamedResult {
    std::string name;
    ResultadoDaBusca result;
};

/**
 * Cena
 * ----
 * Um labirinto (de um certo TIPO) junto com os resultados dos algoritmos
 * executados sobre ele. O renderizador guarda várias cenas e permite
 * alternar entre os tipos de labirinto com as setas do teclado.
 */
struct Cena {
    std::string nome;                     // nome do tipo de labirinto
    Labirinto maze;                            // o labirinto (cópia própria)
    VetorDinamico<NamedResult> resultados; // BFS, DFS, Dijkstra...
};

/**
 * MazeRenderer
 * ------------
 * Janela SFML que desenha o labirinto e ANIMA a busca em tempo real:
 *   - células exploradas vão sendo pintadas na ordem de expansão;
 *   - ao terminar, o caminho-solução é destacado.
 *
 * Controles:
 *   setas <- / -> : troca o TIPO de labirinto (cena)
 *   1, 2, 3 ...   : escolhe qual algoritmo visualizar
 *   R             : reinicia a animação atual
 *   + / -         : acelera / desacelera a animação
 *   Esc           : fecha
 *
 * A separação é proposital: o renderizador NÃO executa os algoritmos,
 * apenas recebe os SearchResult prontos (calculados no main). Isso mantém
 * a lógica de busca independente da interface gráfica.
 */
class MazeRenderer {
private:
    int cellSize_;
    VetorDinamico<Cena> cenas_;

    // estado: 0 = livre, 1 = explorado, 2 = caminho final
    sf::Color colorFor(const Labirinto& maze, int id, int estado) const;
    void drawMaze(sf::RenderWindow& window, const Labirinto& maze,
                  const VetorDinamico<int>& estado) const;

public:
    explicit MazeRenderer(int cellSize = 24);

    // Adiciona um novo labirinto (cena) e devolve seu índice.
    int addMaze(const std::string& nomeTipo, const Labirinto& maze);

    // Adiciona o resultado de um algoritmo à cena de índice 'cena'.
    void addResult(int cena, const std::string& nomeAlgo, const ResultadoDaBusca& res);

    void run();
};
