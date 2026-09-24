#include "algoritmos.hpp"

/*
 * Algoritmo 1: Move(T, a)
*/
StateSet move(const NFA &nfa, const StateSet &T, char a)
{
    StateSet R;

    for (int s : T)
    {
        /* El estado puede no tener ninguna transicion registrada. */
        auto porEstado = nfa.delta.find(s);
        if (porEstado == nfa.delta.end())
        {
            continue;
        }

        /* O tenerlas, pero no para este simbolo. */
        auto porSimbolo = porEstado->second.find(a);
        if (porSimbolo == porEstado->second.end())
        {
            continue;
        }

        /* Union, insert sobre un set ignora los repetidos. */
        R.insert(porSimbolo->second.begin(), porSimbolo->second.end());
    }

    return R;
}