#pragma once
#include "../EstruturasDeDados/VetorDinamico.h"
#include <cstddef>

/**
 * PriorityQueue<T>  (Fila de prioridade de mínimo)
 * -------------------------------------------------
 * Implementada do zero como um MIN-HEAP BINÁRIO sobre o VetorDinamico.
 * É a estrutura que torna o algoritmo de DIJKSTRA eficiente: sempre
 * retorna o elemento de MENOR prioridade (menor distância acumulada).
 *
 * Em um heap representado por array (índices base 0):
 *   - pai de i ........ (i - 1) / 2
 *   - filho esquerdo .. 2*i + 1
 *   - filho direito ... 2*i + 2
 *
 * Complexidade: push e pop em O(log n); top em O(1).
 */
template <typename T>
class FilaDePrioridade {
private:
    struct Item {
        int prioridade; // menor = mais prioritário
        T   valor;
    };

    VetorDinamico<Item> heap_;

    void trocar(std::size_t a, std::size_t b) {
        Item tmp = heap_[a];
        heap_[a] = heap_[b];
        heap_[b] = tmp;
    }

    // Sobe o elemento até restaurar a propriedade de heap
    void subir(std::size_t i) {
        while (i > 0) {
            std::size_t pai = (i - 1) / 2;
            if (heap_[i].prioridade < heap_[pai].prioridade) {
                trocar(i, pai);
                i = pai;
            } else {
                break;
            }
        }
    }

    // Desce o elemento até restaurar a propriedade de heap
    void descer(std::size_t i) {
        std::size_t n = heap_.size();
        while (true) {
            std::size_t menor = i;
            std::size_t esq = 2 * i + 1;
            std::size_t dir = 2 * i + 2;
            if (esq < n && heap_[esq].prioridade < heap_[menor].prioridade) menor = esq;
            if (dir < n && heap_[dir].prioridade < heap_[menor].prioridade) menor = dir;
            if (menor != i) {
                trocar(i, menor);
                i = menor;
            } else {
                break;
            }
        }
    }

public:
    // Insere 'valor' com a prioridade dada
    void enfileira(int prioridade, const T& valor) {
        Item item;
        item.prioridade = prioridade;
        item.valor = valor;
        heap_.insereElemento(item);
        subir(heap_.size() - 1);
    }

    // Remove e retorna o valor de MENOR prioridade
    T desenfileira() {
        Item topo = heap_[0];
        heap_[0] = heap_[heap_.size() - 1];
        heap_.retiraElemento();
        if (!heap_.vazio()) descer(0);
        return topo.valor;
    }

    bool vazio() const { return heap_.vazio(); }
    std::size_t tamanho() const { return heap_.size(); }
};
