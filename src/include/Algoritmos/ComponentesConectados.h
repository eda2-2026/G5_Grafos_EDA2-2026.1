#pragma once
#include "../Grafo/Grafo.h"
#include "../EstruturasDeDados/VetorDinamico.h"
#include "../EstruturasDeDados/Fila.h"

/**
 * ComponentesConectados
 * ---------------------
 * Encontra todos os grupos de nos que se alcancam entre si num grafo.
 * Um componente conectado e um conjunto de nos onde existe caminho
 * entre qualquer par. Nos isolados formam um componente sozinhos.
 *
 * Algoritmo: BFS repetido. Comecamos pelo no 0 e fazemos BFS,
 * marcando todos os nos alcancados como parte do componente 1.
 * Se ainda restar algum no nao visitado, fazemos BFS a partir dele
 * (componente 2), e assim por diante.
 *
 * Cada vez que precisamos comecar um BFS novo, e porque encontramos
 * um grupo novo que nao se conecta aos anteriores.
 *
 * Complexidade: O(V + E) -- cada no e aresta e visitado uma vez.
 */
class ComponentesConectados {
private:
    const Grafo& grafo_;

    // Faz BFS a partir de 'inicio', marcando todos os nos alcancados
    // com o numero do componente atual em 'rotulo[]'
    void bfsComponente(int inicio, int componente,
                       VetorDinamico<int>& rotulo) const {
        Fila<int> fila;
        fila.enfileira(inicio);
        rotulo[inicio] = componente;

        while (!fila.vazio()) {
            int atual = fila.front();
            fila.desenfileira();

            const VetorDinamico<Aresta>& vizinhos = grafo_.vizinhos(atual);
            for (std::size_t i = 0; i < vizinhos.size(); ++i) {
                int prox = vizinhos[i].destino;
                if (rotulo[prox] == -1) {       // ainda nao visitado
                    rotulo[prox] = componente;
                    fila.enfileira(prox);
                }
            }
        }
    }

public:
    explicit ComponentesConectados(const Grafo& g) : grafo_(g) {}

    /**
     * Executa o algoritmo e devolve um vetor onde cada posicao i
     * contem o numero do componente ao qual o no i pertence.
     * Os componentes sao numerados a partir de 0.
     *
     * Exemplo: rotulo = [0, 0, 0, 1, 1, 2]
     *   nos 0,1,2 -> componente 0
     *   nos 3,4   -> componente 1
     *   no  5     -> componente 2
     */
    VetorDinamico<int> calcular() const {
        int n = grafo_.contagemDeNos();
        VetorDinamico<int> rotulo(n, -1);  // -1 = nao visitado
        int componente = 0;

        for (int i = 0; i < n; ++i) {
            if (rotulo[i] == -1) {          // no ainda nao visitado
                bfsComponente(i, componente, rotulo);
                ++componente;               // proximo no novo = componente novo
            }
        }

        return rotulo;
    }

    /**
     * Conta quantos componentes existem no grafo.
     * Atalho: pega o maior rotulo + 1.
     */
    int contarComponentes() const {
        VetorDinamico<int> rotulo = calcular();
        int maior = 0;
        for (std::size_t i = 0; i < rotulo.size(); ++i)
            if (rotulo[i] > maior) maior = rotulo[i];
        return maior + 1;
    }
};