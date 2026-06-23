#include "Visualizacao/MazeRenderer.h"
#include <string>

MazeRenderer::MazeRenderer(int cellSize)
    : cellSize_(cellSize) {}

int MazeRenderer::addMaze(const std::string& nomeTipo, const Labirinto& maze) {
    Cena c;
    c.nome = nomeTipo;
    c.maze = maze;
    cenas_.insereElemento(c);
    return static_cast<int>(cenas_.size()) - 1;
}

void MazeRenderer::addResult(int cena, const std::string& nomeAlgo,
                             const ResultadoDaBusca& res) {
    NamedResult nr;
    nr.name = nomeAlgo;
    nr.result = res;
    cenas_[static_cast<std::size_t>(cena)].resultados.insereElemento(nr);
}

sf::Color MazeRenderer::colorFor(const Labirinto& maze, int id, int estado) const {
    TiposDeCelula t = maze.cellAt(id);
    if (t == TiposDeCelula::Wall)  return sf::Color(40, 42, 54);    // parede
    if (t == TiposDeCelula::Start) return sf::Color(80, 250, 123);  // início (verde)
    if (t == TiposDeCelula::End)   return sf::Color(255, 85, 85);   // fim (vermelho)
    if (estado == 2)          return sf::Color(255, 184, 108); // caminho (laranja)
    if (estado == 1)          return sf::Color(98, 114, 164);  // explorado (azul)
    if (t == TiposDeCelula::Mud)   return sf::Color(122, 85, 58);   // lama (marrom)
    return sf::Color(248, 248, 242);                           // livre (branco)
}

void MazeRenderer::drawMaze(sf::RenderWindow& window, const Labirinto& maze,
                            const VetorDinamico<int>& estado) const {
    sf::RectangleShape celula(
        sf::Vector2f(static_cast<float>(cellSize_) - 1.f,
                     static_cast<float>(cellSize_) - 1.f));
    for (int l = 0; l < maze.linhas(); ++l) {
        for (int c = 0; c < maze.colunas(); ++c) {
            int id = maze.id(l, c);
            celula.setPosition(static_cast<float>(c) * cellSize_,
                               static_cast<float>(l) * cellSize_);
            celula.setFillColor(colorFor(maze, id, estado[id]));
            window.draw(celula);
        }
    }
}

void MazeRenderer::run() {
    if (cenas_.vazio()) return;

    // Todos os labirintos têm o mesmo tamanho (mesmos H e W), então a
    // janela é dimensionada pela primeira cena.
    int total = cenas_[0].maze.linhas() * cenas_[0].maze.colunas();
    unsigned w = static_cast<unsigned>(cenas_[0].maze.colunas() * cellSize_);
    unsigned h = static_cast<unsigned>(cenas_[0].maze.linhas() * cellSize_);
    sf::RenderWindow window(sf::VideoMode(w, h),
                            "Maze Solver - DFS / BFS / Dijkstra");
    window.setFramerateLimit(60);

    std::size_t cenaAtual = 0;
    std::size_t algoAtual = 0;
    double atrasoPassoMs = 12.0; // menor = mais rápido

    VetorDinamico<int> estado(total, 0);
    std::size_t idxExplorar = 0;
    std::size_t idxCaminho = 0;
    bool explorando = true;
    sf::Clock relogio;
    double acumulado = 0.0;

    auto reiniciarAnim = [&]() {
        for (int i = 0; i < total; ++i) estado[i] = 0;
        idxExplorar = 0;
        idxCaminho = 0;
        explorando = true;
        acumulado = 0.0;
        relogio.restart();
    };

    auto atualizarTitulo = [&]() {
        const Cena& cena = cenas_[cenaAtual];
        std::string s = "[" + std::to_string(cenaAtual + 1) + "/" +
                        std::to_string(cenas_.size()) + "] " + cena.nome;
        if (cena.resultados.size() > 0) {
            const NamedResult& nr = cena.resultados[algoAtual];
            s += "  |  " + nr.name;
            s += "  |  nos: " + std::to_string(nr.result.nosExplorados);
            s += "  |  passos: " + std::to_string(nr.result.caminho.size());
            s += "  |  custo: " + std::to_string(nr.result.custoCaminho);
            if (!nr.result.encontrado) s += "  |  SEM SOLUCAO";
        }
        s += "   [setas: labirinto | 1-9: algoritmo | R | +/-]";
        window.setTitle(s);
    };

    reiniciarAnim();
    atualizarTitulo();

    while (window.isOpen()) {
        sf::Event e;
        while (window.pollEvent(e)) {
            if (e.type == sf::Event::Closed) window.close();
            if (e.type == sf::Event::KeyPressed) {
                if (e.key.code == sf::Keyboard::Escape) {
                    window.close();
                } else if (e.key.code == sf::Keyboard::R) {
                    reiniciarAnim();
                } else if (e.key.code == sf::Keyboard::Left) {
                    if (cenaAtual == 0) cenaAtual = cenas_.size() - 1;
                    else --cenaAtual;
                    algoAtual = 0;
                    reiniciarAnim();
                    atualizarTitulo();
                } else if (e.key.code == sf::Keyboard::Right) {
                    cenaAtual = (cenaAtual + 1) % cenas_.size();
                    algoAtual = 0;
                    reiniciarAnim();
                    atualizarTitulo();
                } else if (e.key.code == sf::Keyboard::Add ||
                           e.key.code == sf::Keyboard::Equal) {
                    atrasoPassoMs *= 0.5;
                    if (atrasoPassoMs < 0.5) atrasoPassoMs = 0.5;
                } else if (e.key.code == sf::Keyboard::Subtract ||
                           e.key.code == sf::Keyboard::Hyphen) {
                    atrasoPassoMs *= 2.0;
                    if (atrasoPassoMs > 200.0) atrasoPassoMs = 200.0;
                } else {
                    int sel = -1;
                    if (e.key.code >= sf::Keyboard::Num1 &&
                        e.key.code <= sf::Keyboard::Num9)
                        sel = e.key.code - sf::Keyboard::Num1;
                    if (sel >= 0 &&
                        static_cast<std::size_t>(sel) < cenas_[cenaAtual].resultados.size()) {
                        algoAtual = static_cast<std::size_t>(sel);
                        reiniciarAnim();
                        atualizarTitulo();
                    }
                }
            }
        }

        const Cena& cena = cenas_[cenaAtual];

        // Cena sem resultados: só desenha o labirinto.
        if (cena.resultados.size() == 0) {
            window.clear(sf::Color(20, 21, 28));
            drawMaze(window, cena.maze, estado);
            window.display();
            continue;
        }

        const ResultadoDaBusca& res = cena.resultados[algoAtual].result;

        // Avança a animação com base no tempo real decorrido.
        acumulado += relogio.restart().asMicroseconds() / 1000.0;
        while (acumulado >= atrasoPassoMs) {
            acumulado -= atrasoPassoMs;
            if (explorando) {
                if (idxExplorar < res.ordemDeVisita.size()) {
                    int id = res.ordemDeVisita[idxExplorar];
                    if (estado[id] == 0) estado[id] = 1;
                    ++idxExplorar;
                } else {
                    explorando = false;
                }
            } else {
                if (idxCaminho < res.caminho.size()) {
                    int id = res.caminho[idxCaminho];
                    estado[id] = 2;
                    ++idxCaminho;
                }
            }
        }

        window.clear(sf::Color(20, 21, 28));
        drawMaze(window, cena.maze, estado);
        window.display();
    }
}
