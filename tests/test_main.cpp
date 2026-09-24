#include "../src/algoritmos.hpp"
#include "../src/io.hpp"

#include <cstdio>

static int pruebas_totales = 0;
static int pruebas_ok = 0;

#define CHECA(cond, descripcion) \
    do { \
        pruebas_totales++; \
        if (cond) { pruebas_ok++; } \
        else { std::printf("  [FALLO] %s (linea %d)\n", descripcion, __LINE__); } \
    } while (0)

/*
 * Vector de prueba tomado del manual, seccion 2.2.2.
 * Estado 0 con 0-a->1, 0-a->2, 0-b->3. Move({0}, 'a') debe ser {1, 2}.
 */
static void prueba_move()
{
    std::printf("Move(T, a)\n");

    NFA nfa;
    nfa.numStates = 4;
    nfa.start = 0;
    nfa.accept = {3};
    nfa.addTransition(0, 'a', 1);
    nfa.addTransition(0, 'a', 2);
    nfa.addTransition(0, 'b', 3);

    StateSet resultadoA = move(nfa, {0}, 'a');
    CHECA(resultadoA == StateSet({1, 2}), "Move({0}, 'a') debe ser {1, 2}");

    StateSet resultadoB = move(nfa, {0}, 'b');
    CHECA(resultadoB == StateSet({3}), "Move({0}, 'b') debe ser {3}");

    StateSet resultadoC = move(nfa, {0}, 'c');
    CHECA(resultadoC.empty(), "Move con un simbolo sin transiciones registradas debe ser vacio");
}

/*
 * Vector de prueba tomado del manual, seccion 2.2.1.
 * Estados {0,1,2,3}, transiciones 0-eps->1, 1-eps->2, ciclo 2-eps->1.
 * epsilon-Closure({0}) debe ser {0,1,2} sin caer en bucle infinito.
 */
static void prueba_epsilon_closure()
{
    std::printf("epsilon-Closure(T)\n");

    NFA nfa;
    nfa.numStates = 4;
    nfa.start = 0;
    nfa.accept = {3};
    nfa.addEpsilon(0, 1);
    nfa.addEpsilon(1, 2);
    nfa.addEpsilon(2, 1); /* ciclo de vuelta a 1, no a 0 */

    StateSet resultado = epsilonClosure(nfa, {0});
    CHECA(resultado == StateSet({0, 1, 2}), "epsilon-Closure({0}) debe ser {0,1,2}");
}

/*
 * NFA de Thompson para la expresion (a|b)*abb, el ejemplo clasico de
 * construccion de subconjuntos (Aho, Sethi, Ullman). Se eligio porque
 * su DFA resultante es un resultado conocido y facil de verificar a mano,
 * y porque tiene ciclos de epsilon y varios simbolos por estado.
 *
 *   0 --eps--> 1        1 --eps--> 2       2 --a--> 3
 *   0 --eps--> 7        1 --eps--> 4       3 --eps--> 6
 *                       6 --eps--> 1       4 --b--> 5
 *                       6 --eps--> 7       5 --eps--> 6
 *   7 --a--> 8          8 --b--> 9         9 --b--> 10 (aceptacion)
 */
static NFA construirNFA_abb()
{
    NFA nfa;
    nfa.numStates = 11;
    nfa.start = 0;
    nfa.accept = {10};

    nfa.addEpsilon(0, 1);
    nfa.addEpsilon(0, 7);
    nfa.addEpsilon(1, 2);
    nfa.addEpsilon(1, 4);
    nfa.addTransition(2, 'a', 3);
    nfa.addEpsilon(3, 6);
    nfa.addTransition(4, 'b', 5);
    nfa.addEpsilon(5, 6);
    nfa.addEpsilon(6, 1);
    nfa.addEpsilon(6, 7);
    nfa.addTransition(7, 'a', 8);
    nfa.addTransition(8, 'b', 9);
    nfa.addTransition(9, 'b', 10);

    return nfa;
}

static int buscarEstado(const DFA &dfa, const StateSet &subconjunto)
{
    for (size_t i = 0; i < dfa.subsets.size(); i++)
    {
        if (dfa.subsets[i] == subconjunto)
        {
            return static_cast<int>(i);
        }
    }
    return -1;
}

/*
 * Prueba obligatoria del manual (seccion 2.2.3): construccion de
 * subconjuntos completa, NFA -> DFA. Se compara contra los 5 subconjuntos
 * y la tabla de transiciones conocidos del ejemplo (a|b)*abb.
 */
static void prueba_subconjuntos()
{
    std::printf("Construccion de subconjuntos (NFA a DFA)\n");

    NFA nfa = construirNFA_abb();
    imprimirNFA(nfa);

    DFA dfa = subconjuntos(nfa);
    imprimirDFA(dfa);

    const StateSet A = {0, 1, 2, 4, 7};
    const StateSet B = {1, 2, 3, 4, 6, 7, 8};
    const StateSet C = {1, 2, 4, 5, 6, 7};
    const StateSet D = {1, 2, 4, 5, 6, 7, 9};
    const StateSet E = {1, 2, 4, 5, 6, 7, 10}; /* unico subconjunto de aceptacion, contiene al 10 */

    CHECA(dfa.numStates() == 5, "el DFA debe tener exactamente 5 estados alcanzables");

    int a = buscarEstado(dfa, A);
    int b = buscarEstado(dfa, B);
    int c = buscarEstado(dfa, C);
    int d = buscarEstado(dfa, D);
    int e = buscarEstado(dfa, E);

    CHECA(a != -1 && b != -1 && c != -1 && d != -1 && e != -1,
          "los 5 subconjuntos esperados deben existir en QD");
    CHECA(a != -1 && dfa.start == a, "el estado inicial debe ser epsilon-Closure({0})");
    CHECA(e != -1 && dfa.accept.size() == 1 && dfa.accept.count(e) == 1,
          "solo el subconjunto que contiene al estado 10 debe ser de aceptacion");

    if (a != -1 && b != -1 && c != -1 && d != -1 && e != -1)
    {
        CHECA(dfa.delta.count(a) && dfa.delta.at(a).count('a') && dfa.delta.at(a).at('a') == b, "A --a--> B");
        CHECA(dfa.delta.count(a) && dfa.delta.at(a).count('b') && dfa.delta.at(a).at('b') == c, "A --b--> C");
        CHECA(dfa.delta.count(b) && dfa.delta.at(b).count('a') && dfa.delta.at(b).at('a') == b, "B --a--> B");
        CHECA(dfa.delta.count(b) && dfa.delta.at(b).count('b') && dfa.delta.at(b).at('b') == d, "B --b--> D");
        CHECA(dfa.delta.count(c) && dfa.delta.at(c).count('a') && dfa.delta.at(c).at('a') == b, "C --a--> B");
        CHECA(dfa.delta.count(c) && dfa.delta.at(c).count('b') && dfa.delta.at(c).at('b') == c, "C --b--> C");
        CHECA(dfa.delta.count(d) && dfa.delta.at(d).count('a') && dfa.delta.at(d).at('a') == b, "D --a--> B");
        CHECA(dfa.delta.count(d) && dfa.delta.at(d).count('b') && dfa.delta.at(d).at('b') == e, "D --b--> E");
        CHECA(dfa.delta.count(e) && dfa.delta.at(e).count('a') && dfa.delta.at(e).at('a') == b, "E --a--> B");
        CHECA(dfa.delta.count(e) && dfa.delta.at(e).count('b') && dfa.delta.at(e).at('b') == c, "E --b--> C");
    }

    CHECA(dfa.alphabet == nfa.alphabet, "el alfabeto del DFA debe ser el mismo que el del NFA, sin epsilon");
}

int main()
{
    prueba_move();
    prueba_epsilon_closure();
    prueba_subconjuntos();

    std::printf("\n%d/%d pruebas exitosas\n", pruebas_ok, pruebas_totales);
    return (pruebas_ok == pruebas_totales) ? 0 : 1;
}
