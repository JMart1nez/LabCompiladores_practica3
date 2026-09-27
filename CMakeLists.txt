cmake_minimum_required(VERSION 3.16)
project(nfa_to_dfa CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CORE_SOURCES
    src/move.cpp
    src/closure.cpp
    src/subconjuntos.cpp
    src/minimizacion.cpp
    src/io.cpp
)

add_executable(nfa_to_dfa src/main.cpp ${CORE_SOURCES})
target_include_directories(nfa_to_dfa PRIVATE src)

# Pruebas unitarias de move, epsilon-closure y la construccion de subconjuntos
add_executable(unit_tests tests/test_main.cpp ${CORE_SOURCES})
target_include_directories(unit_tests PRIVATE src)

# Suite de validacion de la minimizacion (3 regex, 20 cadenas cada una)
add_executable(validador tests/validador.cpp ${CORE_SOURCES})
target_include_directories(validador PRIVATE src)

enable_testing()
add_test(NAME unit_tests COMMAND unit_tests)
add_test(NAME validador COMMAND validador)