
#include "algoritmos.hpp"
#include "io.hpp"

#include <cassert>
#include <iostream>
#include <string>
#include <vector>

/* Simula una cadena sobre el DFA. Devuelve true si termina en un
   estado de aceptacion, alse si se agota la cadena fuera de ellos
   o si encuentra una transicion indefinida. */
static bool test_string(const DFA &dfa, const std::string &input)
{
    int actual = dfa.start;
    for (char c : input)
    {
        auto porEstado = dfa.delta.find(actual);
        if (porEstado == dfa.delta.end())
        {
            return false;
        }
        auto porSimbolo = porEstado->second.find(c);
        if (porSimbolo == porEstado->second.end())
        {
            return false;
        }
        actual = porSimbolo->second;
    }
    return dfa.accept.count(actual) > 0;
}

/* Corre los dos vectores de cadenas y reporta el conteo. */
static int run_test_suite(const DFA &dfa,
                          const std::vector<std::string> &accept_tests,
                          const std::vector<std::string> &reject_tests)
{
    int pasadas = 0;
    int total   = static_cast<int>(accept_tests.size() + reject_tests.size());

    std::cout << "\n[Casos de aceptacion]\n";
    for (const auto &s : accept_tests)
    {
        bool res = test_string(dfa, s);
        std::cout << "  \"" << (s.empty() ? "(vacia)" : s) << "\": "
                  << (res ? "PASS" : "FAIL") << "\n";
        if (res) pasadas++;
    }

    std::cout << "\n[Casos de rechazo]\n";
    for (const auto &s : reject_tests)
    {
        bool res = !test_string(dfa, s);
        std::cout << "  \"" << (s.empty() ? "(vacia)" : s) << "\": "
                  << (res ? "PASS" : "FAIL") << "\n";
        if (res) pasadas++;
    }

    std::cout << "\nResultado: " << pasadas << "/" << total << " pruebas superadas.\n";
    return pasadas == total ? 0 : 1;
}

/* NFA de (a|b)*abb */
static NFA nfa_abb()
{
    NFA n;
    n.start = 0;
    /* (a|b)* : el bucle regresa al 0 */
    n.addEpsilon(0, 1);
    n.addEpsilon(0, 3);
    n.addTransition(1, 'a', 2);
    n.addTransition(3, 'b', 4);
    n.addEpsilon(2, 5);
    n.addEpsilon(4, 5);
    n.addEpsilon(5, 0);
    /* abb */
    n.addEpsilon(0, 7);
    n.addTransition(7, 'a', 8);
    n.addTransition(8, 'b', 9);
    n.addTransition(9, 'b', 10);
    n.accept.insert(10);
    n.numStates = 11;
    return n;
}

/* NFA de a(ba)*b */
static NFA nfa_baba()
{
    NFA n;
    n.start = 0;
    n.addTransition(0, 'a', 1);
    n.addEpsilon(1, 2);
    n.addEpsilon(1, 6);
    n.addTransition(2, 'b', 3);
    n.addTransition(3, 'a', 4);
    n.addEpsilon(4, 1);
    n.addTransition(6, 'b', 7);
    n.accept.insert(7);
    n.numStates = 8;
    return n;
}

/* NFA de (0|10*1)*, cadenas binarias con un numero par de unos. */
static NFA nfa_paridad()
{
    NFA n;
    n.start = 0;
    n.addEpsilon(0, 1);   /* rama del 0     */
    n.addEpsilon(0, 3);   /* rama del 10*1  */
    n.addEpsilon(0, 9);   /* salida (acepta)*/
    n.addTransition(1, '0', 2);
    n.addEpsilon(2, 8);
    n.addTransition(3, '1', 4);
    n.addEpsilon(4, 5);
    n.addTransition(5, '0', 6);
    n.addEpsilon(6, 5);
    n.addEpsilon(5, 7);
    n.addTransition(7, '1', 8);
    n.addEpsilon(8, 0);
    n.accept.insert(9);
    n.numStates = 10;
    return n;
}


static int evaluar(const std::string &nombre,
                   const NFA &nfa,
                   const std::vector<std::string> &acepta,
                   const std::vector<std::string> &rechaza)
{
    
    std::cout << " Expresion regular: " << nombre << "\n";
    

    DFA dfa     = subconjuntos(nfa);
    DFA dfa_min = minimize_dfa(dfa);

    imprimirDFA(dfa);
    std::cout << "--- despues de minimizar ---\n";
    imprimirDFA(dfa_min);

    assert(dfa.numStates() >= dfa_min.numStates());
    std::cout << "Comprobacion |Q| >= |Q'|: "
              << dfa.numStates() << " >= " << dfa_min.numStates() << "  OK\n";

    /* Comprobacion extra: minimizar no debe cambiar ninguna decision. */
    int discrepancias = 0;
    for (const auto &s : acepta)
        if (test_string(dfa, s) != test_string(dfa_min, s)) discrepancias++;
    for (const auto &s : rechaza)
        if (test_string(dfa, s) != test_string(dfa_min, s)) discrepancias++;
    std::cout << "diferencias entre el DFA original y el minimizado: "
              << discrepancias << "\n";

    int fallos = run_test_suite(dfa_min, acepta, rechaza);
    return fallos + discrepancias;
}

int main()
{
    int fallos = 0;

    /*  1. (a|b)*abb : cadenas que terminan en abb  */
    fallos += evaluar(
        "(a|b)*abb",
        nfa_abb(),
        /* aceptan: el caso minimo, prefijos con a, con b, y el patron repetido */
        {"abb", "aabb", "babb", "bbabb", "ababb",
         "abbabb", "aaabb", "bababb", "abbbabb", "bbbabb"},
        /* rechazan: vacia, simbolos sueltos, sufijos parecidos pero incorrectos */
        {"", "a", "b", "ab", "ba",
         "aab", "abba", "abbb", "bbb", "ababa"});

    /*  2. a(ba)*b : alternancia estricta */
    fallos += evaluar(
        "a(ba)*b",
        nfa_baba(),
        /* aceptan: de cero a nueve repeticiones del bloque ba */
        {"ab", "abab", "ababab", "abababab", "ababababab",
         "abababababab", "ababababababab", "abababababababab",
         "ababababababababab", "abababababababababab"},
        /* rechazan: vacia, letras sueltas, orden invertido, sobra o falta un simbolo */
        {"", "a", "b", "ba", "aa",
         "bb", "aab", "abb", "aba", "ababa"});

    
    fallos += evaluar(
        "(0|10*1)*",
        nfa_paridad(),
        /* aceptan: la vacia (cero unos es par), puros ceros, y pares de unos */
        {"", "0", "00", "11", "101",
         "1001", "0110", "000", "1111", "010010"},
        /* rechazan: todas tienen un numero impar de unos */
        {"1", "111", "01", "10", "0001",
         "11111", "0100", "10101", "1000", "0111"});

    
    if (fallos == 0)
        std::cout << " Todas las pruebas pasaron\n";
    else
        std::cout << " HUBO " << fallos << " FALLO(S)\n";
    

    return fallos == 0 ? 0 : 1;
}
