#ifndef ALGORITMOS_HPP
#define ALGORITMOS_HPP

#include "automata.hpp"

/* 
 * Algoritmo 1 Move(T, a)
 * Devuelve los estados alcanzables desde T consumiendo exactamente el simbolo a.
 */
StateSet move(const NFA &nfa, const StateSet &T, char a);

/* 
 * Algoritmo 2 epsilon-Closure(T) 
 * Devuelve todos los estados alcanzables desde T sin consumir entrada incluidos los de T mismos.
 */
StateSet epsilonClosure(const NFA &nfa, const StateSet &T);

/* 
 * Algoritmo 3 Construccion de subconjuntos
 * Transforma el NFA en un DFA equivalente 
 */
DFA subconjuntos(const NFA &nfa);

#endif /* ALGORITMOS_HPP */