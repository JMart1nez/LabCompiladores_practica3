#include "algoritmos.hpp"
#include <queue>

/*
 * Algoritmo 3 Construccion de subconjuntos
 */
DFA subconjuntos(const NFA &nfa)
{
    DFA dfa;
    dfa.alphabet = nfa.alphabet;

    // Se nombra a cada subconjunto con un indice
    StateSet s0 = epsilonClosure(nfa, StateSet{nfa.start});
    dfa.subsets.push_back(s0);
    dfa.start = 0;

    // Se utiliza un diccionario para recordar los subconjuntos descubiertos
    std::map<StateSet, int> indices;
    indices[s0] = dfa.start;
    std::queue<int> cola;
    cola.push(dfa.start);

    // Empieza la exploración de los estados
    while (!cola.empty())
    {
        int origen = cola.front();
        cola.pop();

        // Se hace una copia por valor
        StateSet U = dfa.subsets[origen];
        for (char a : dfa.alphabet)
        {
            StateSet V = epsilonClosure(nfa, move(nfa, U, a));

            //  Se omite al conjunto vacio
            if (V.empty())
            {
                continue;
            }

            auto encontrado = indices.find(V);
            int destino;

            // Se marca como nuevo conjunto si es que no esta en el mapa de indices
            if (encontrado == indices.end())
            {
                destino = dfa.numStates();
                dfa.subsets.push_back(V);
                indices[V] = destino;
                cola.push(destino);
            }
            else
            {
                destino = encontrado->second;
            }

            dfa.delta[origen][a] = destino;
        }
    }

    // Un subconjunto acepta si contiene algun estado final del NFA
    for (int i = 0; i < dfa.numStates(); i++)
    {
        for (int estado : dfa.subsets[i])
        {
            if (nfa.accept.count(estado) != 0)
            {
                dfa.accept.insert(i);
                break;
            }
        }
    }

    return dfa;
}
