/* Programa principal  NFA a DFA.
 *
 *   ./nfa_to_dfa ejemplos/automata.nfa (formato de texto)
 *   ./nfa_to_dfa ejemplos/automata.json (JSON de la practica 1)
 *
 * El formato se deduce de la extension. 
 */

#include "algoritmos.hpp"
#include "io.hpp"

#include <iostream>
#include <string>

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "Uso: " << argv[0] << " <archivo.nfa | archivo.json>\n";
        return 1;
    }

    const std::string ruta = argv[1];
    NFA               nfa;
    bool              cargado = false;

    /*Entrada: se elige el cargador segun la extension*/
    if (ruta.size() > 5 && ruta.substr(ruta.size() - 5) == ".json")
    {
        cargado = loadNFAFromJSON(ruta, nfa);
        if (!cargado)
        {
            std::cerr << "Error: no se pudo leer el JSON '" << ruta << "'\n";
        }
    }
    else
    {
        std::string error;
        cargado = loadNFAFromText(ruta, nfa, error);
        if (!cargado)
        {
            std::cerr << "Error: " << error << "\n";
        }
    }

    if (!cargado)
    {
        return 1;
    }

    /* El automata antes de la conversion */
    imprimirNFA(nfa);

    /* Conversion */
    DFA dfa = subconjuntos(nfa);

    /* El automata despues de la conversion  */
    imprimirDFA(dfa);

    return 0;
}