#pragma once
#include "./AlgoritmoDeBusca.h"
#include "../EstruturasDeDados/Pilha.h"

/**
 * DFS — Busca em Profundidade
 * ---------------------------
 * Avança o máximo possível por um caminho antes de retroceder. Usa a
 * PILHA (Stack). Implementação ITERATIVA: empilhamos pares (nó, pai) e,
 * ao desempilhar, registramos o pai que realmente levou àquele nó — assim
 * o caminho reconstruído é sempre válido.
 *
 * Observação: o DFS NÃO garante o caminho mais curto; ele encontra
 * *um* caminho. É ótimo para comparar com BFS/Dijkstra no relatório.
 *
 * Complexidade: O(V + E).
 */
class DFS : public AlgoritmoDeBusca {
public:
    explicit DFS(const Grafo& g) : AlgoritmoDeBusca(g) {}

    const char* nome() const override { return "DFS"; }

    ResultadoDaBusca solve(int inicio, int fim) override {
        ResultadoDaBusca res;
        int n = grafo_.contagemDeNos();
        VetorDinamico<bool> visitado(n, false);
        VetorDinamico<int>  parent(n, -1);

        struct Frame { int no; int pai; };
        Pilha<Frame> pilha;

        Frame inicial; inicial.no = inicio; inicial.pai = -1;
        pilha.empilha(inicial);

        while (!pilha.vazio()) {
            Frame f = pilha.top();
            pilha.desempilha();

            if (visitado[f.no]) continue;
            visitado[f.no] = true;
            parent[f.no] = f.pai;
            res.ordemDeVisita.insereElemento(f.no);
            ++res.nosExplorados;

            if (f.no == fim) { res.encontrado = true; break; }

            const VetorDinamico<Aresta>& vizinhos = grafo_.vizinhos(f.no);
            for (std::size_t i = 0; i < vizinhos.size(); ++i) {
                int prox = vizinhos[i].destino;
                if (!visitado[prox]) {
                    Frame nf; nf.no = prox; nf.pai = f.no;
                    pilha.empilha(nf);
                }
            }
        }

        construirCaminho(parent, inicio, fim, res);
        if (res.encontrado) res.custoCaminho = static_cast<int>(res.caminho.size()) - 1;
        return res;
    }
};
