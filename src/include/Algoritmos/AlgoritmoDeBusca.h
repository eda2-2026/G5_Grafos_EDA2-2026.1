#pragma once
#include "../EstruturasDeDados/VetorDinamico.h"
#include "../Grafo/Grafo.h"
#include <cstddef>

/**
 * ResultadoDaBusca * ------------
 * Tudo o que uma busca produz e que precisamos para análise e animação:
 *   - ordemDeVisita: ordem em que os nós foram EXPANDIDOS (para a animação);
 *   - caminho:       caminho final do início ao fim (sequência de nós);
 *   - encontrado:      se o destino foi alcançado;
 *   - nosExplorados: quantidade de nós expandidos (métrica de eficiência);
 *   - tempoEmMs:     tempo de execução em milissegundos (preenchido fora);
 *   - custoCaminho:   custo total do caminho (relevante no Dijkstra).
 */
struct ResultadoDaBusca {
    VetorDinamico<int> ordemDeVisita;
    VetorDinamico<int> caminho;
    bool encontrado = false;
    std::size_t nosExplorados = 0;
    double tempoEmMs = 0.0;
    int custoCaminho
    = 0;
};

/**
 * AlgoritmoDeBusca (classe abstrata)
 * ---------------------------------
 * Define a INTERFACE comum a BFS, DFS e Dijkstra. Graças ao método
 * virtual puro solve(), podemos tratar qualquer algoritmo de forma
 * polimórfica (ex.: medir tempo de todos com a mesma função). Esse é o
 * ponto central de orientação a objetos do projeto.
 */
class AlgoritmoDeBusca {
protected:
    const Grafo& grafo_;

    /**
     * Reconstrói o caminho do início ao fim a partir do vetor de "pais".
     * Sobe de 'fim' até 'inicio' seguindo parent[] e inverte o resultado.
     */
    void construirCaminho(const VetorDinamico<int>& parent,
                   int inicio, int fim, ResultadoDaBusca& res) const {
        if (!res.encontrado) return;
        VetorDinamico<int> reverso;
        int atual = fim;
        while (atual != -1) {
            reverso.insereElemento(atual);
            if (atual == inicio) break;
            atual = parent[atual];
        }
        for (std::size_t i = reverso.size(); i > 0; --i)
            res.caminho.insereElemento(reverso[i - 1]);
    }

public:
    explicit AlgoritmoDeBusca(const Grafo& g) : grafo_(g) {}
    virtual ~AlgoritmoDeBusca() = default;

    // Resolve do nó 'inicio' ao nó 'fim'
    virtual ResultadoDaBusca solve(int inicio, int fim) = 0;

    // Nome do algoritmo (para relatórios e rótulos)
    virtual const char* nome() const = 0;
};
