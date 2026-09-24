#ifndef IO_HPP
#define IO_HPP

#include "automata.hpp"

#include <string>

/* 
 * io.hpp
 *
 * Para entrada y salida. 
 *
 * El NFA se puede construir de tres maneras, segun lo que convenga:
 *
 * 1. A mano en codigo, con addTransition y addEpsilon. 
 *
 * 2. Desde texto (archivo o cadena). Sirve para los vectores de prueba.
 *
 * 3. Desde el JSON que produce save_nfa de la practica 1 para automatas grandes generados a partir de una regex 
 * (es necesario haber ejecutado primero ejecutar la práctica y copiar el .json a esta práctica)
 *
 * Las tres dan la misma estructura asi que el resto del programa no distingue de donde vino el automata.
 */

/* 
 * Formato de texto. Las lineas en blanco y lo que sigue a # se ignoran. Ejemplo:
 *
 *     estados 4 (opcional, si falta se deduce del mayor indice)
 *     inicio  0
 *     acepta  3 (admite varios, "acepta 2 3 5")
 * 
 *     Ejemplo:
 *
 *     0 a 1
 *     0 a 2
 *     1 eps 2
*/
bool loadNFAFromText(const std::string &path, NFA &out, std::string &error);

/* Igual que el anterior pero desde una cadena en memoria para poder
 * escribir el automata dentro del propio codigo de pruebas. */
bool parseNFAFromString(const std::string &texto, NFA &out, std::string &error);

/* Carga un NFA desde el JSON que produce save_nfa de la practica 1.
 * Aquel formato es especifico de Thompson y aqui se traduce a la representacion general. */
bool loadNFAFromJSON(const std::string &path, NFA &out);

/* Representacion en texto de un conjunto, por ejemplo "{0, 1, 2}". */
std::string setToString(const StateSet &s);

/* Tabla de transiciones del NFA */
void imprimirNFA(const NFA &nfa);

/* Tabla de transiciones del DFA */
void imprimirDFA(const DFA &dfa);

#endif /* IO_HPP */