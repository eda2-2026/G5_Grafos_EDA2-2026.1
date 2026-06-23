#pragma once
#include "./AlgoritmoDeBusca.h"
#include "../EstruturasDeDados/VetorDinamico.h"
#include "../EstruturasDeDados/Fila.h"

/**
 * BFS — Busca em Largura
 * ----------------------
 * Explora o grafo em "ondas", visitando todos os vizinhos de distância 1,
 * depois 2, e assim por diante. Usa a fila.
 *
 * Complexidade: O(V + E).
 */
class BFS : public AlgoritmoDeBusca {
public:
    explicit BFS(const Grafo& g) : AlgoritmoDeBusca(g) {}

    const char* nome() const override { return "BFS"; }

    ResultadoDaBusca solve(int inicio, int fim) override {
        ResultadoDaBusca res;
        int n = grafo_.contagemDeNos();
        VetorDinamico<bool> visitado(n, false);
        VetorDinamico<int>  parent(n, -1);
        Fila<int> fila;

        fila.enfileira(inicio);
        visitado[inicio] = true;

        while (!fila.vazio()) {
            int atual = fila.front();
            fila.desenfileira();
            res.ordemDeVisita.insereElemento(atual);
            ++res.nosExplorados;

            if (atual == fim) { res.encontrado = true; break; }

            const VetorDinamico<Aresta>& vizinhos = grafo_.vizinhos(atual);
            for (std::size_t i = 0; i < vizinhos.size(); ++i) {
                int prox = vizinhos[i].destino;
                if (!visitado[prox]) {
                    visitado[prox] = true;
                    parent[prox] = atual;
                    fila.enfileira(prox);
                }
            }
        }

        construirCaminho(parent, inicio, fim, res);
        if (res.encontrado) res.custoCaminho = static_cast<int>(res.caminho.size()) - 1;
        return res;
    }
};
