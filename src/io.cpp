#include "io.hpp"
#include "algoritmos.hpp"

#include <cctype>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>


std::string setToString(const StateSet &s)
{
    std::string resultado = "{";
    bool        primero   = true;

    for (int estado : s)
    {
        if (!primero)
        {
            resultado += ",";
        }
        resultado += std::to_string(estado);
        primero = false;
    }

    resultado += "}";
    return resultado;
}

/* 
 * Carga desde texto
 * Cada linea es (estados, inicio, acepta) o una transicion "origen simbolo destino". El simbolo eps designa una
 * transicion vacia.
 */

static bool esEpsilon(const std::string &token)
{
    return token == "eps" || token == "EPS" || token == "E" || token == "epsilon";
}

bool parseNFAFromString(const std::string &texto, NFA &out, std::string &error)
{
    out = NFA();
    error.clear();

    std::istringstream entrada(texto);
    std::string linea;
    int numLinea = 0;
    bool tieneInicio = false;

    while (std::getline(entrada, linea))
    {
        numLinea++;

        /* Se recorta el comentario y se ignoran las lineas vacias. */
        size_t comentario = linea.find('#');
        if (comentario != std::string::npos)
        {
            linea = linea.substr(0, comentario);
        }

        std::istringstream campos(linea);
        std::string primero;
        if (!(campos >> primero))
        {
            continue;
        }

        if (primero == "estados")
        {
            if (!(campos >> out.numStates) || out.numStates <= 0)
            {
                error = "linea " + std::to_string(numLinea) + ": 'estados' necesita un entero positivo";
                return false;
            }
        }
        else if (primero == "inicio")
        {
            if (!(campos >> out.start))
            {
                error = "linea " + std::to_string(numLinea) + ": 'inicio' necesita un estado";
                return false;
            }
            tieneInicio = true;
        }
        else if (primero == "acepta")
        {
            int estado;
            bool alguno = false;
            while (campos >> estado)
            {
                out.accept.insert(estado);
                alguno = true;
            }
            if (!alguno)
            {
                error = "linea " + std::to_string(numLinea) + ": 'acepta' necesita al menos un estado";
                return false;
            }
        }
        else
        {
            /* Transicion: origen simbolo destino */
            int origen;
            std::string simbolo;
            int destino;

            std::istringstream otraVez(linea);
            if (!(otraVez >> origen >> simbolo >> destino))
            {
                error = "linea " + std::to_string(numLinea) + ": se esperaba 'origen simbolo destino'";
                return false;
            }

            if (esEpsilon(simbolo))
            {
                out.addEpsilon(origen, destino);
            }
            else if (simbolo.size() == 1)
            {
                out.addTransition(origen, simbolo[0], destino);
            }
            else
            {
                error = "linea " + std::to_string(numLinea) + ": el simbolo debe ser un solo caracter o eps";
                return false;
            }
        }
    }

    if (!tieneInicio)
    {
        error = "falta 'inicio'";
        return false;
    }

    if (out.accept.empty())
    {
        error = "falta 'acepta'";
        return false;
    }

    /* Si no se declaro el numero de estados se deduce del mayor indice mencionado. */
    if (out.numStates == 0)
    {
        int mayor = out.start;
        for (int s : out.accept) { if (s > mayor) mayor = s; }
        for (const auto &porEstado : out.delta)
        {
            if (porEstado.first > mayor) mayor = porEstado.first;
            for (const auto &porSimbolo : porEstado.second)
            {
                for (int d : porSimbolo.second) { if (d > mayor) mayor = d; }
            }
        }
        for (const auto &porEstado : out.epsilon)
        {
            if (porEstado.first > mayor) mayor = porEstado.first;
            for (int d : porEstado.second) { if (d > mayor) mayor = d; }
        }
        out.numStates = mayor + 1;
    }

    return true;
}

bool loadNFAFromText(const std::string &path, NFA &out, std::string &error)
{
    std::ifstream archivo(path);
    if (!archivo)
    {
        error = "no se pudo abrir '" + path + "'";
        return false;
    }

    std::stringstream buffer;
    buffer << archivo.rdbuf();

    return parseNFAFromString(buffer.str(), out, error);
}

/* 
 * Carga del JSON de la practica 1
 *
 * El formato es especifico de la construccion de Thompson donde cada
 * estado tiene un campo symbol y hasta dos destinos fijos (out1, out2),
 * con symbol == -1 para epsilon.
 *
 * Aqui se traduce a la representacion general, un estado con symbol
 * distinto de -1 aporta una transicion por simbolo  y uno con -1
 * aporta hasta dos transiciones epsilon 
 */

/* Lee el entero que sigue a una clave dada */
static bool leerCampo(const std::string &texto, size_t &pos, const std::string &clave, int &valor)
{
    size_t encontrado = texto.find(clave, pos);
    if (encontrado == std::string::npos)
    {
        return false;
    }

    size_t i = encontrado + clave.size();
    while (i < texto.size() && (texto[i] == ' ' || texto[i] == ':' || texto[i] == '"' || texto[i] == '\t'))
    {
        i++;
    }

    size_t inicio = i;
    if (i < texto.size() && (texto[i] == '-' || texto[i] == '+'))
    {
        i++;
    }
    while (i < texto.size() && std::isdigit(static_cast<unsigned char>(texto[i])))
    {
        i++;
    }

    if (i == inicio)
    {
        return false;
    }

    valor = std::atoi(texto.substr(inicio, i - inicio).c_str());
    pos   = i;
    return true;
}

bool loadNFAFromJSON(const std::string &path, NFA &out)
{
    std::ifstream archivo(path);
    if (!archivo)
    {
        return false;
    }

    std::stringstream buffer;
    buffer << archivo.rdbuf();
    std::string texto = buffer.str();

    size_t pos = 0;
    int size = 0, start = 0, accept = 0;

    if (!leerCampo(texto, pos, "\"size\"", size) ||
        !leerCampo(texto, pos, "\"start\"", start) ||
        !leerCampo(texto, pos, "\"accept\"", accept) || size <= 0)
    {
        return false;
    }

    out = NFA();
    out.numStates = size;
    out.start     = start;
    out.accept.insert(accept);

    for (int i = 0; i < size; i++)
    {
        int id = 0, symbol = 0, out1 = 0, out2 = 0;

        if (!leerCampo(texto, pos, "\"id\"", id) ||
            !leerCampo(texto, pos, "\"symbol\"", symbol) ||
            !leerCampo(texto, pos, "\"out1\"", out1) ||
            !leerCampo(texto, pos, "\"out2\"", out2))
        {
            return false;
        }

        if (symbol == -1)
        {
            /* Transiciones epsilon: hasta dos destinos. */
            if (out1 != -1) { out.addEpsilon(id, out1); }
            if (out2 != -1) { out.addEpsilon(id, out2); }
        }
        else
        {
            /* Transicion que consume un simbolo: solo out1 es valido. */
            if (out1 != -1)
            {
                out.addTransition(id, static_cast<char>(symbol), out1);
            }
        }
    }

    return true;
}

 /* 
 * Muestra las tablas de transiciones en consola.
 */

void imprimirNFA(const NFA &nfa)
{
    std::cout << "=== NFA ===\n";
    std::cout << "Estados: " << nfa.numStates << "\n";
    std::cout << "Inicio:  " << nfa.start << "\n";
    std::cout << "Acepta:  " << setToString(nfa.accept) << "\n";
    std::cout << "Transiciones normales:\n";
    for (const auto &estado : nfa.delta)
    {
        for (const auto &simbolo : estado.second)
        {
            std::cout << "  " << estado.first << " --" << simbolo.first << "--> " << setToString(simbolo.second) << "\n";
        }
    }
    std::cout << "Transiciones epsilon (eps):\n";
    for (const auto &estado : nfa.epsilon)
    {
        std::cout << "  " << estado.first << " --eps--> " << setToString(estado.second) << "\n";
    }
    std::cout << "===========\n";
}

void imprimirDFA(const DFA &dfa)
{
    std::cout << "=== DFA ===\n";
    std::cout << "Estados: " << dfa.numStates() << "\n";
    std::cout << "Inicio:  " << dfa.start << "\n";
    std::cout << "Acepta:  " << setToString(dfa.accept) << "\n";
    std::cout << "Transiciones:\n";
    for (const auto &estado : dfa.delta)
    {
        for (const auto &simbolo : estado.second)
        {
            std::cout << "  " << estado.first << " --" << simbolo.first << "--> " << simbolo.second << "\n";
        }
    }
    std::cout << "===========\n";
}


