#ifndef AUTOMATA_HPP
#define AUTOMATA_HPP

#include <map>
#include <set>
#include <string>
#include <vector>

/*  Un subconjunto de estados del NFA se representa con std::set<int> */

using StateSet = std::set<int>;

/* 
 * NFA
 * Las transiciones por simbolo y las transiciones epsilon se guardan por separado */
struct NFA
{
    int numStates = 0;
    std::set<char> alphabet; /* Sigma, sin epsilon */
    std::map<int, std::map<char, StateSet>> delta; /* delta(estado, simbolo) */
    std::map<int, StateSet> epsilon; /* delta(estado, epsilon) */
    int start = -1;
    StateSet accept; /* conjunto, no un solo estado */

    /* Registra una transicion que consume un simbolo. */
    void addTransition(int from, char symbol, int to)
    {
        delta[from][symbol].insert(to);
        alphabet.insert(symbol);
    }

    /* Registra una transicion epsilon. */
    void addEpsilon(int from, int to)
    {
        epsilon[from].insert(to);
    }
};

/* DFA
 *
 * Cada estado del DFA es un subconjunto de estados del NFA, pero
 * guardarlo asi en la tabla de transiciones seria incomodo de imprimir
 * y de recorrer, por eso se numeran, subsets[i] es el subconjunto que
 * dio origen al estado i y la tabla trabaja con enteros.  */
struct DFA
{
    std::vector<StateSet> subsets; /* estado i -> subconjunto */
    std::set<char> alphabet;
    std::map<int, std::map<char, int>> delta; /* delta(estado, simbolo) */
    int start = -1;
    std::set<int> accept;

    int numStates() const { return static_cast<int>(subsets.size()); }
};

#endif /* AUTOMATA_HPP */
