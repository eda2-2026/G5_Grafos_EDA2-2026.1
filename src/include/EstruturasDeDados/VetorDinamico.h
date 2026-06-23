#pragma once
#include <cstddef>

/**
 * VetorDinamico<T>
 * ----------------
 * Vetor dinâmico implementado do zero (sem usar std::vector).
 * Serve de base para a PriorityQueue e para os vetores auxiliares
 * usados pelos algoritmos (visitados, pais, distâncias, etc.).
 *
 * Crescimento amortizado O(1): dobra a capacidade quando enche.
 * Implementa a "regra dos cinco" (destrutor, cópia e move) para
 * poder ser copiado/retornado por valor com segurança.
 */
template <typename T>
class VetorDinamico {
private:
    T* dados_;
    std::size_t capacidade_;
    std::size_t tamanho_;

    void reserve(std::size_t novaCap) {
        if (novaCap <= capacidade_) return;
        T* novo = new T[novaCap];
        for (std::size_t i = 0; i < tamanho_; ++i) novo[i] = dados_[i];
        delete[] dados_;
        dados_ = novo;
        capacidade_ = novaCap;
    }

public:
    // Vetor vazio
    VetorDinamico() : dados_(new T[1]), capacidade_(1), tamanho_(0) {}

    // Vetor com n elementos, todos iguais a 'valor'
    VetorDinamico(std::size_t n, const T& valor)
        : dados_(new T[n > 0 ? n : 1]), capacidade_(n > 0 ? n : 1), tamanho_(n) {
        for (std::size_t i = 0; i < n; ++i) dados_[i] = valor;
    }

    // Construtor de cópia
    VetorDinamico(const VetorDinamico& outro)
        : dados_(new T[outro.capacidade_]), capacidade_(outro.capacidade_), tamanho_(outro.tamanho_) {
        for (std::size_t i = 0; i < tamanho_; ++i) dados_[i] = outro.dados_[i];
    }

    // Atribuição por cópia
    VetorDinamico& operator=(const VetorDinamico& outro) {
        if (this != &outro) {
            delete[] dados_;
            capacidade_ = outro.capacidade_;
            tamanho_ = outro.tamanho_;
            dados_ = new T[capacidade_];
            for (std::size_t i = 0; i < tamanho_; ++i) dados_[i] = outro.dados_[i];
        }
        return *this;
    }

    // Construtor de move
    VetorDinamico(VetorDinamico&& outro) noexcept
        : dados_(outro.dados_), capacidade_(outro.capacidade_), tamanho_(outro.tamanho_) {
        outro.dados_ = nullptr;
        outro.capacidade_ = 0;
        outro.tamanho_ = 0;
    }

    // Atribuição por move
    VetorDinamico& operator=(VetorDinamico&& outro) noexcept {
        if (this != &outro) {
            delete[] dados_;
            dados_ = outro.dados_;
            capacidade_ = outro.capacidade_;
            tamanho_ = outro.tamanho_;
            outro.dados_ = nullptr;
            outro.capacidade_ = 0;
            outro.tamanho_ = 0;
        }
        return *this;
    }

    ~VetorDinamico() { delete[] dados_; }

    void insereElemento(const T& valor) {
        if (tamanho_ == capacidade_) reserve(capacidade_ * 2);
        dados_[tamanho_++] = valor;
    }

    void retiraElemento() {
        if (tamanho_ > 0) --tamanho_;
    }

    T&       operator[](std::size_t i)       { return dados_[i]; }
    const T& operator[](std::size_t i) const { return dados_[i]; }

    std::size_t size() const { return tamanho_; }
    bool vazio() const { return tamanho_ == 0; }
    void esvaziar() { tamanho_ = 0; }
};
