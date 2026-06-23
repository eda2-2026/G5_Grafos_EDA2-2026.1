cmake_minimum_required(VERSION 3.16)
project(MazeSolver LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---------------------------------------------------------------------------
# Opção: compilar com a visualização gráfica (SFML) ligada ou desligada.
#   ON  -> baixa e compila a SFML automaticamente (precisa de internet na
#          primeira vez) e habilita a janela de animação.
#   OFF -> compila só o núcleo (estruturas + grafo + algoritmos + benchmark
#          no console). Não precisa de SFML nem de internet.
# No CLion, dá pra definir em: Settings > Build > CMake > CMake options:
#   -DWITH_SFML=OFF
# ---------------------------------------------------------------------------
option(WITH_SFML "Compilar a visualizacao grafica com SFML" ON)

add_executable(MazeSolver src/main.cpp)
target_include_directories(MazeSolver PRIVATE src/include)

if (WITH_SFML)
    include(FetchContent)
    FetchContent_Declare(
            SFML
            GIT_REPOSITORY https://github.com/SFML/SFML.git
            GIT_TAG        2.6.1            # versão estável usada por este projeto
    )
    # Não compila os exemplos/doc da própria SFML (build mais rápido)
    set(SFML_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(SFML_BUILD_DOC      OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(SFML)

    target_sources(MazeSolver PRIVATE src/Visualization/MazeRenderer.cpp)
    target_link_libraries(MazeSolver PRIVATE sfml-graphics sfml-window sfml-system)
    target_compile_definitions(MazeSolver PRIVATE WITH_SFML)
endif()

# ---------------------------------------------------------------------------
# ALTERNATIVA (se você já tem a SFML instalada no sistema e prefere não
# baixar pelo FetchContent): comente o bloco acima e use:
#
#   find_package(SFML 2.6 COMPONENTS graphics window system REQUIRED)
#   target_sources(MazeSolver PRIVATE src/Visualization/MazeRenderer.cpp)
#   target_link_libraries(MazeSolver PRIVATE sfml-graphics sfml-window sfml-system)
#   target_compile_definitions(MazeSolver PRIVATE WITH_SFML)
# ---------------------------------------------------------------------------
