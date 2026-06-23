#pragma once
#include "../EstruturasDeDados/VetorDinamico.h"

/**
 * Edge (Aresta)
 * -------------
 * Representa uma ligação direcionada para o nó 'destino' com um 'peso'.
 * Em um labirinto sem terrenos especiais, todos os pesos valem 1.
 */
struct Aresta {
    int destino;
    int peso;
};

/**
 * Graph (Grafo)
 * -------------
 * Grafo representado por LISTA DE ADJACÊNCIAS.
 * Cada nó é identificado por um inteiro de 0 a (nodeCount - 1).
 *
 * No nosso projeto, cada célula caminhável do labirinto vira um nó, e
 * células vizinhas (cima/baixo/esquerda/direita) ficam ligadas por arestas.
 * É sobre este grafo que DFS, BFS e Dijkstra operam — deixando explícita
 * a ideia central do trabalho: "resolver o labirinto = percorrer o grafo".
 */
class Grafo {
private:
    VetorDinamico<VetorDinamico<Aresta>> adjacencia_;
    int totalNos_;

public:
    explicit Grafo(int n) : totalNos_(n) {
        for (int i = 0; i < n; ++i)
            adjacencia_.insereElemento(VetorDinamico<Aresta>());
    }

    // Adiciona uma aresta direcionada origem -> destino
    void adicionarArestaDirecionada(int origem, int destino, int peso = 1) {
        Aresta e;
        e.destino = destino;
        e.peso = peso;
        adjacencia_[origem].insereElemento(e);
    }

    // Adiciona aresta nos dois sentidos (grafo não direcionado)
    void adicionarArestaNaoDirecionada(int a, int b, int peso = 1) {
        adicionarArestaDirecionada(a, b, peso);
        adicionarArestaDirecionada(b, a, peso);
    }

    // Lista de vizinhos de um nó
    const VetorDinamico<Aresta>& vizinhos(int no) const {
        return adjacencia_[no];
    }

    int contagemDeNos() const { return totalNos_; }
};
