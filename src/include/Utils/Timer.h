#pragma once
#include <chrono>

/**
 * Timer
 * -----
 * Cronômetro de alta resolução para medir o tempo de execução das buscas.
 * Uso:
 *     Timer t;
 *     t.start();
 *     ... código a medir ...
 *     double ms = t.elapsedMs();
 *
 * Usa std::chrono (biblioteca padrão), com resolução de microssegundos.
 */
class Timer {
private:
    std::chrono::high_resolution_clock::time_point inicio_;

public:
    void start() {
        inicio_ = std::chrono::high_resolution_clock::now();
    }

    double elapsedMs() const {
        auto agora = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double, std::milli>(agora - inicio_).count();
    }
};
