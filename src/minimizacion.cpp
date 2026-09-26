#include "algoritmos.hpp"
#include <queue>
#include <algorithm>
#include <list>
#include <iterator>

/*
 * Algoritmo de minimización
 */
DFA minimize_dfa(const DFA &dfa)
{
    // Eliminamos estados inalcanzables
    std::set<int> alcanzables;
    std::queue<int> cola_alcanzables;

    if (dfa.start != -1)
    {
        alcanzables.insert(dfa.start);
        cola_alcanzables.push(dfa.start);
    }

    while (!cola_alcanzables.empty())
    {
        int q = cola_alcanzables.front();
        cola_alcanzables.pop();

        auto it_estado = dfa.delta.find(q);
        if (it_estado != dfa.delta.end())
        {
            for (const auto &transicion : it_estado->second)
            {
                int destino = transicion.second;
                if (alcanzables.find(destino) == alcanzables.end())
                {
                    alcanzables.insert(destino);
                    cola_alcanzables.push(destino);
                }
            }
        }
    }

    // Inicializamos grupos P y W
    std::set<int> F, Q_minus_F;
    for (int q : alcanzables)
    {
        if (dfa.accept.count(q)) F.insert(q);
        else Q_minus_F.insert(q);
    }

    std::list<std::set<int>> P;
    std::list<std::set<int>> W;

    if (!F.empty()) { P.push_back(F); W.push_back(F); }
    if (!Q_minus_F.empty()) { P.push_back(Q_minus_F); W.push_back(Q_minus_F); }

    while (!W.empty())
    {
        std::set<int> A = W.front();
        W.pop_front();

        for (char c : dfa.alphabet)
        {
            std::set<int> X;
            for (int q : alcanzables)
            {
                auto it_q = dfa.delta.find(q);
                if (it_q != dfa.delta.end())
                {
                    auto it_trans = it_q->second.find(c);
                    if (it_trans != it_q->second.end() && A.count(it_trans->second))
                    {
                        X.insert(q);
                    }
                }
            }

            if (X.empty()) continue;

            // Revisamos Y en P
            auto it_Y = P.begin();
            while (it_Y != P.end())
            {
                std::set<int> Y = *it_Y;
                std::set<int> Y1, Y2;

                // Y1 = Y intersección X  |  Y2 = Y diferencia X
                std::set_intersection(Y.begin(), Y.end(), X.begin(), X.end(), std::inserter(Y1, Y1.begin()));
                std::set_difference(Y.begin(), Y.end(), X.begin(), X.end(), std::inserter(Y2, Y2.begin()));

                if (!Y1.empty() && !Y2.empty())
                {
                    it_Y = P.erase(it_Y);
                    P.insert(it_Y, Y1);
                    P.insert(it_Y, Y2);

                    auto it_W = std::find(W.begin(), W.end(), Y);
                    if (it_W != W.end())
                    {
                        // Si Y está en W, reemplazamos por Y1 y Y2
                        it_W = W.erase(it_W);
                        W.insert(it_W, Y1);
                        W.insert(it_W, Y2);
                    }
                    else
                    {
                        if (Y1.size() <= Y2.size()) W.push_back(Y1);
                        else W.push_back(Y2);
                    }
                }
                else
                {
                    ++it_Y;
                }
            }
        }
    }

    // Construcción del DFA
    DFA dfa_min;
    dfa_min.alphabet = dfa.alphabet;
    dfa_min.subsets.resize(P.size()); 

    std::map<int, int> mapa_estados;
    int nuevo_id = 0;

    for (const auto &grupo : P)
    {
        for (int q : grupo)
        {
            mapa_estados[q] = nuevo_id;
        }

        // Si el grupo contiene el inicio original, este grupo es el nuevo inicio
        if (grupo.count(dfa.start)) 
        {
            dfa_min.start = nuevo_id;
        }

        // Si el grupo contiene un estado de aceptación, el nuevo estado es de aceptación
        for (int q : grupo)
        {
            if (dfa.accept.count(q))
            {
                dfa_min.accept.insert(nuevo_id);
                break;
            }
        }
        nuevo_id++;
    }

    // Reconstruimos transiciones con un representante de cada grupo
    for (const auto &grupo : P)
    {
        int representante = *grupo.begin();
        int id_origen = mapa_estados[representante];

        auto it_estado = dfa.delta.find(representante);
        if (it_estado != dfa.delta.end())
        {
            for (const auto &transicion : it_estado->second)
            {
                char simbolo = transicion.first;
                int destino_original = transicion.second;
                
                if (mapa_estados.count(destino_original))
                {
                    dfa_min.delta[id_origen][simbolo] = mapa_estados[destino_original];
                }
            }
        }
    }

    return dfa_min;
}