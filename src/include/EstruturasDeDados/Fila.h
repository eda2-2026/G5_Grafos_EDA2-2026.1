#pragma once
#include <cstddef>

/**
 * Queue<T>  (Fila — FIFO)
 * -----------------------
 * Implementada do zero com lista ligada (ponteiros para início e fim).
 * É a estrutura que define o comportamento da BUSCA EM LARGURA (BFS):
 * o primeiro nó descoberto é o primeiro a ser expandido, o que garante
 * encontrar o caminho com MENOS ARESTAS em grafos não ponderados.
 *
 * Todas as operações são O(1).
 */
template <typename T>
class Fila {
private:
    struct No {
        T dado;
        No* proximo;
        explicit No(const T& d) : dado(d), proximo(nullptr) {}
    };

    No* inicio_;
    No* fim_;
    std::size_t tamanho_;

public:
    Fila() : inicio_(nullptr), fim_(nullptr), tamanho_(0) {}

    ~Fila() {
        while (!vazio()) desenfileira();
    }

    // Insere no fim da fila
    void enfileira(const T& valor) {
        No* novo = new No(valor);
        if (fim_ != nullptr) fim_->proximo = novo;
        else                 inicio_ = novo;
        fim_ = novo;
        ++tamanho_;
    }

    // Remove do início da fila
    void desenfileira() {
        if (inicio_ == nullptr) return;
        No* tmp = inicio_;
        inicio_ = inicio_->proximo;
        if (inicio_ == nullptr) fim_ = nullptr;
        delete tmp;
        --tamanho_;
    }

    // Acessa o elemento do início (sem remover)
    T& front() { return inicio_->dado; }
    const T& front() const { return inicio_->dado; }

    bool vazio() const { return inicio_ == nullptr; }
    std::size_t size() const { return tamanho_; }
};
