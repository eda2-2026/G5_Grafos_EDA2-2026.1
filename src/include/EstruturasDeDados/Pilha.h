#pragma once
#include <cstddef>

/**
 * Stack<T>  (Pilha — LIFO)
 * ------------------------
 * Implementada do zero com lista ligada simples.
 * É a estrutura que define o comportamento da BUSCA EM PROFUNDIDADE (DFS):
 * o último nó descoberto é o primeiro a ser expandido.
 *
 * Todas as operações são O(1).
 */
template <typename T>
class Pilha {
private:
    struct No {
        T dado;
        No* proximo;
        No(const T& d, No* p) : dado(d), proximo(p) {}
    };

    No* topo_;
    std::size_t tamanho_;

public:
    Pilha() : topo_(nullptr), tamanho_(0) {}

    ~Pilha() {
        while (!vazio()) desempilha();
    }

    // Empilha um elemento no topo
    void empilha(const T& valor) {
        topo_ = new No(valor, topo_);
        ++tamanho_;
    }

    // Remove o elemento do topo
    void desempilha() {
        if (topo_ == nullptr) return;
        No* tmp = topo_;
        topo_ = topo_->proximo;
        delete tmp;
        --tamanho_;
    }

    // Acessa o elemento do topo (sem remover)
    T& top() { return topo_->dado; }
    const T& top() const { return topo_->dado; }

    bool vazio() const { return topo_ == nullptr; }
    std::size_t size() const { return tamanho_; }
};
