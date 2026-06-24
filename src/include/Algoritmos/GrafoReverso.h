#pragma once
#include "../Grafo/Grafo.h"

/**
 * GrafoReverso
 * ------------
 * Cria uma copia do grafo com todas as arestas invertidas.
 * Se no grafo original existe a aresta A -> B com peso P,
 * no grafo reverso existira B -> A com o mesmo peso P.
 *
 * Para que serve?
 * Em grafos NAO-DIRECIONADOS, o reverso e identico ao original
 * (cada aresta ja vai nos dois sentidos). A utilidade aparece em
 * grafos DIRECIONADOS, onde o reverso e usado por algoritmos como
 * Kosaraju para encontrar componentes FORTEMENTE conectados:
 * grupos onde todo no alcanca todos os outros.
 *
 * Exemplo:
 *   Original:  0->1  1->2  2->0  2->3
 *   Reverso:   1->0  2->1  0->2  3->2
 *   Componentes fortemente conectados: {0,1,2} e {3}
 *
 * Complexidade: O(V + E) -- percorre todos os nos e arestas uma vez.
 */
class GrafoReverso {
public:
    /**
     * Recebe o grafo original e devolve um novo grafo com as
     * arestas invertidas. O numero de nos e o mesmo.
     */
    static Grafo construir(const Grafo& original) {
        int n = original.contagemDeNos();
        Grafo reverso(n);

        for (int origem = 0; origem < n; ++origem) {
            const VetorDinamico<Aresta>& vizinhos = original.vizinhos(origem);
            for (std::size_t i = 0; i < vizinhos.size(); ++i) {
                int destino = vizinhos[i].destino;
                int peso    = vizinhos[i].peso;
                // aresta original: origem -> destino
                // aresta reversa:  destino -> origem
                reverso.adicionarArestaDirecionada(destino, origem, peso);
            }
        }

        return reverso;
    }
};