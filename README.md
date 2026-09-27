# Práctica 3 – NFA a DFA

Laboratorio de Compiladores. Programa en C++17 que convierte un autómata finito no determinista (NFA) en un DFA equivalente usando la construcción de subconjuntos, e incluye minimización del DFA.

## Algoritmos implementados

- `move(T, a)` – estados alcanzables desde T con el símbolo `a`
- `epsilonClosure(T)` – cerradura épsilon
- `subconjuntos(nfa)` – construcción de subconjuntos (NFA → DFA)
- `minimize_dfa(dfa)` – minimización del DFA

## Estructura

```
src/     código fuente (automata, algoritmos, io, main)
tests/   pruebas unitarias (test_main.cpp) y validador de minimización (validador.cpp)
```

## Compilar

Requiere CMake 3.16+ y un compilador con C++17.

```bash
cmake -S . -B build
cmake --build build
```

## Uso

```bash
./build/nfa_to_dfa archivo.nfa     # formato de texto
./build/nfa_to_dfa archivo.json    # JSON generado por la práctica 1
```

El formato se detecta por la extensión. El programa imprime la tabla del NFA y la del DFA resultante.

### Formato `.nfa`

```
estados 4        # opcional
inicio 0
acepta 3         # admite varios: acepta 2 3 5
0 a 1
0 eps 2          # transición épsilon
2 b 3
1 b 3
```

Las líneas vacías y lo que va después de `#` se ignoran.

## Pruebas

```bash
cd build
ctest
```
