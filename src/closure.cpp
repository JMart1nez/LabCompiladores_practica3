#include "algoritmos.hpp"
#include <vector>

/* 
 * Algoritmo 2 epsilon-Closure(T)
 */
StateSet epsilonClosure(const NFA &nfa, const StateSet &T)
{
    /*
    * Inicializamos una pila con los elementos de T y C 
    * inicializado como T
    */
   StateSet C = T;
   std::vector<int> pila(T.begin(), T.end());

   /*
   * mientras la pila no es vacia
   */
   while (!pila.empty())
   {
        /*
        * Desapilar
        */
        int t = pila.back();
        pila.pop_back();

        /*
        * Para toda u en transicion N(t, epsilon)
        */
        auto transicion = nfa.epsilon.find(t);
        if (transicion != nfa.epsilon.end())
        {
            for (int u : transicion -> second)
            {
                /*
                * Para toda u en transicion N(t, epsilon)
                */
                
                if (C.find(u) == C.end())
                {
                    /*
                    * C <- C union {u}
                    */
                    C.insert(u);
                    
                    
                    /*
                    * apilar(Pila, u)
                    */
                    pila.push_back(u);
                }
            }
        }
   }
   
   return C;
}