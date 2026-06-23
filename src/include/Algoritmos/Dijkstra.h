#pragma once
#include "./AlgoritmoDeBusca.h"
#include "../EstruturasDeDados/FilaDePrioridade.h"

/**
 * Dijkstra — Caminho de menor custo
 * ---------------------------------
 * Encontra o caminho de menor custo total em grafos com pesos não
 * negativos. Usa a fila de prioridade (min-heap) para sempre expandir
 * o nó com a menor distância acumulada conhecida.
 *
 * Em um labirinto onde todos os pesos são 1, o resultado coincide com o
 * do BFS. A diferença aparece quando há terrenos com custos diferentes
 * (veja Maze::setCost) — aí o Dijkstra desvia da lama, enquanto o BFS
 * apenas conta arestas.
 *
 * Complexidade: O(E log V) com heap binário.
 */
class Dijkstra : public AlgoritmoDeBusca {
public:
    explicit Dijkstra(const Grafo& g) : AlgoritmoDeBusca(g) {}

    const char* nome() const override { return "Dijkstra"; }

    ResultadoDaBusca solve(int inicio, int fim) override {
        ResultadoDaBusca res;
        int n = grafo_.contagemDeNos();
        const int INF = 1000000000;

        VetorDinamico<int>  dist(n, INF);
        VetorDinamico<int>  parent(n, -1);
        VetorDinamico<bool> finalizado(n, false);
        FilaDePrioridade<int> fila_de_prioridade; // prioridade = distância acumulada

        dist[inicio] = 0;
        fila_de_prioridade.enfileira(0, inicio);



        while (!fila_de_prioridade.vazio()) {
            int atual = fila_de_prioridade.desenfileira();
            if (finalizado[atual]) continue; // descarta entradas obsoletas
            finalizado[atual] = true;
            res.ordemDeVisita.insereElemento(atual);
            ++res.nosExplorados;

            if (atual == fim) { res.encontrado = true; break; }

            const VetorDinamico<Aresta>& vizinhos = grafo_.vizinhos(atual);
            for (std::size_t i = 0; i < vizinhos.size(); ++i) {
                int prox = vizinhos[i].destino;
                int peso = vizinhos[i].peso;
                if (!finalizado[prox] && dist[atual] + peso < dist[prox]) {
                    dist[prox] = dist[atual] + peso;
                    parent[prox] = atual;
                    fila_de_prioridade.enfileira(dist[prox], prox);
                }
            }
        }

        construirCaminho(parent, inicio, fim, res);
        if (res.encontrado) res.custoCaminho = dist[fim];
        return res;
    }
};
