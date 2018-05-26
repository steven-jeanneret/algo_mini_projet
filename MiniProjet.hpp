//
// Created by steven on 30.04.18.
//

#pragma once

#include "graph.hpp"
#include <string>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <set>
#include <list>
#include <map>
#include <limits>

/**
 * La classe n'utilise pas de template car la consigne dit qu'il s'agit d'un nom comme clé et d'un point en entier
 */
class MiniProjet {
private:
    Graph<std::string, int> *graph;
public:


    MiniProjet() {
        graph = new Graph<std::string, int>(false);
    }

    /**
     * Phase 1
     * Permet la lecture du fichier saisi par l'utilisateur
     * Créer le graphe adapté
     * Exporte le graphe au format circo en demandant le chemin du fichier désiré
     * @param f
     */
    void readFileToGraph(std::string f = "") {
        std::ifstream myFile;
        std::string line;
        std::string fileName = (f == "") ? getInputFileName() : f;

        auto vertex = std::map<std::string, Graph<std::string, int>::Vertex *>();
        try {
            myFile.open(fileName);
        } catch (const std::exception &e) {
            std::cout << e.what();
            std::cout << "Erreur de lecture du fichier";
        }
        if (myFile.is_open()) {
            int nbLigneIgnored = 2;
            while (getline(myFile, line)) {
                if (nbLigneIgnored <= 0) {
                    if (line != "}") {
                        std::istringstream buf(line); //Source : http://www.cplusplus.com/forum/beginner/87238/
                        std::istream_iterator<std::string> beg(buf), end;
                        std::vector<std::string> tokens(beg, end); // done!

                        Graph<std::string, int>::Vertex *person1 = nullptr;
                        Graph<std::string, int>::Vertex *person2 = nullptr;
                        int nbConversation = 0;
                        for (std::string s: tokens) {
                            if (s != "--") {
                                if (s[0] != '[') {
                                    if (vertex.count(s) <= 0) {
                                        vertex[s] = graph->add_vertex(s);
                                    }
                                    if (person1 == nullptr) {
                                        person1 = vertex[s];
                                    } else {
                                        person2 = vertex[s];
                                    }
                                } else {
                                    std::string nbTemp = "";
                                    for (int i = 7; i < s.length(); i++) {
                                        nbTemp += s[i];
                                    }
                                    nbConversation = atoi(nbTemp.c_str());
                                }
                            }
                        }
                        graph->add_edge(nbConversation, person1, person2);
                    }
                } else {
                    nbLigneIgnored--;
                }
            }

            myFile.close();
            std::cout << "File read successful!\n";
            exportCircoGraph(1);
        }
    }

    /* Phase 2
     * Implémentation personnelle du graphe minimal recouvrant faite avant distribution de la version du prof
     * D'après l'algorithme de Prim, source : https://www.youtube.com/watch?v=I0uiQyAs5G4 */
    void minimalRecoverGraph() {
        auto vertexInTree = new std::vector<Graph<std::string, int>::Vertex *>();
        auto edgeInTree = new std::set<Graph<std::string, int>::Edge *>();
        vertexInTree->push_back(graph->get_vertex(0));
        while (vertexInTree->size() < graph->num_vertices()) {
            int min = std::numeric_limits<int>::max();
            Graph<std::string, int>::Vertex *v;
            Graph<std::string, int>::Edge *e;
            for (auto vi = graph->vertex_iterator(); vi.has_next(); vi.next()) {
                auto vert = vi.current();
                for (auto ni = vert->neighbor_iterator(); ni.has_next(); ni.next()) {
                    int iTemp = isOnlyOne(vertexInTree, ni.current());
                    if (iTemp >= 0) {
                        if (ni.current()->get_weight() < min) {
                            min = ni.current()->get_weight();
                            e = ni.current();
                            v = (iTemp == 1) ? ni.current()->get_source() : v = ni.current()->get_destination();
                        }
                    }
                }
            }
            vertexInTree->push_back(v);
            edgeInTree->insert(e);
        }
        exportCircoGraph(2, edgeInTree);
    }

    /**
     * Affiche tous les vertex et demande d'en choisir un pour l'arbre minimal recouvrant
     */
    void selectShortestPath() {
        int iCpt = 0;
        for (auto vi = graph->vertex_iterator(); vi.has_next(); vi.next()) {
            std::cout << vi.current()->get_value() << " [" << iCpt << "]\t";
            if ((iCpt + 1) % 3 == 0) {
                std::cout << "\n";
            }
            iCpt++;
        }
        std::cout << "\n";
        int i = 0;
        std::cout << "Enter the number of the vertex for the ACPC\n";
        std::cin >> i;
        shortestPath(graph->get_vertex(i),3);
    }

    /**
     * Calcul le pivot le plus rapide
     */
    void bestShortestPath() {
        int iShortestVertex = 0;
        int BestWeight = getWeightVertex(graph->get_vertex(iShortestVertex));
        for (int i = 1; i < graph->num_vertices(); i++) {
            int ActualWeight = getWeightVertex(graph->get_vertex(i));
            if (ActualWeight < BestWeight) {
                iShortestVertex = i;
                BestWeight = ActualWeight;
            }
        }
        std::cout << graph->get_vertex(iShortestVertex)->get_value()
                  << " is the head of the AMP with the total weight : " << BestWeight << "\n";
        shortestPath(graph->get_vertex(iShortestVertex), 4);
    }

private:
    /**
     * Utilisation de la fonction shortest_path_tree_iterator
     * On stocke les côtés de l'arbre retourné dans un set qu'on marquera en rouge à l'export
     * @param v
     * @param state
     */
    void shortestPath(Graph<std::string, int>::Vertex *v = nullptr, int state = 3) {
        auto vertex = (v == nullptr) ? graph->get_vertex(0) : v;
        auto it = graph->shortest_path_tree_iterator(vertex);
        auto s = graph->subgraph(&it);
        auto setEdge = new std::set<Graph<std::string, int>::Edge *>();
        auto i = s.edge_iterator();
        while (i.has_next()) {
            setEdge->insert(i.current());
            i.next();
        }
        exportCircoGraph(state, setEdge);
    }

    /**
     * Permet de calculer le poids d'un ACPC pour un vertex
     * @param pVertex
     * @return
     */
    int getWeightVertex(Graph<std::string, int>::Vertex *pVertex) {
        int i = 0;
        auto it = graph->shortest_path_tree_iterator(pVertex);
        while (it.has_next()) {
            i += it.current()->get_weight();
            it.next();
        }
        return i;
    }

    /**
     * Permet de savoir si 2 edge sont identiques
     * Si les vertex à l'extrémité sont identiques, ils sont considérés comme identiques
     * @param pSet
     * @param pEdge
     * @return
     */
    bool isEdgeInSet(std::set<Graph<std::string, int>::Edge *> *pSet, Graph<std::string, int>::Edge *pEdge) {
        std::string name1 = pEdge->get_source()->get_value();
        std::string name2 = pEdge->get_destination()->get_value();
        for (auto e = pSet->begin(); e != pSet->end(); e++) {
            if ((*e)->get_source()->get_value() == name1 || (*e)->get_source()->get_value() ==name2) {
                if ((*e)->get_destination()->get_value()==name1 ||
                    (*e)->get_destination()->get_value()==name2) {
                    return true;
                }
            }
        }
        return false;
    }

    /**
     * Export du graphe en fichier .dot format circo
     * Si nous sommes à plus que la phase 1, nous prenons en compte le set qui contient les edges a marquer en rouge
     * Comme il est spécifé dans la consigne que c'est un arbre non dirigé, ceci n'est pas pris en compte via le graphe
     * @param state
     * @param edges
     */
    void exportCircoGraph(int state, std::set<Graph<std::string, int>::Edge *> *edges = nullptr) {
        std::string lien = " -- ";
        std::ofstream myFile;
        std::string fileName = getOutputFileName(state);
        try {
            myFile.open(fileName);
        } catch (const std::exception &e) {
            std::cout << e.what();
            std::cout << "Erreur de lecture du fichier";
        }
        if (myFile.is_open()) {
            myFile << "graph {\n";
            myFile << "\tlabelloc=\"t\";labeljust=\"l\";label=\"np=8 l=2-4 m=20-40\"\n";
            myFile << "\tlayout=\"circo\"\n";
            auto setEdge = new std::set<Graph<std::string, int>::Edge *>();
            for (auto vi = graph->vertex_iterator(); vi.has_next(); vi.next()) {
                auto curr = vi.current();
                for (auto ni = curr->neighbor_iterator(); ni.has_next(); ni.next()) {
                    if (setEdge->count(ni.current()) == 0) {
                        setEdge->insert(ni.current());
                        auto neighbor_edge = ni.current();
                        auto neighbor = neighbor_edge->get_destination(curr);
                        myFile << "\t" << curr->get_value() << lien << neighbor->get_value() << " [label="
                               << neighbor_edge->get_weight();
                        if (state >= 2) {
                            if (isEdgeInSet(edges, ni.current())) {
                                myFile << ",penwith=3,color=\"red\"";
                            }
                        }
                        myFile << "]\n";
                    }
                }
            }
        }
        myFile << "}";
        myFile.close();
        std::cout << "File write successful!" << std::endl;
    }

    /**
     * On s'assure que l'arrête passé en paramètre à un sommet dans le graph minimal et un sommet à l'extérieur
     * @param pVector
     * @param pEdge
     * @return
     */
    int isOnlyOne(std::vector<Graph<std::string, int>::Vertex *> *pVector, Graph<std::string, int>::Edge *pEdge) {
        int verif = 0;
        int index = -1;
        for (int i = 0; i < pVector->size(); i++) {
            if (pVector->at(i) == pEdge->get_source()) {
                verif++;
                index = 0;
            }
            if (pVector->at(i) == pEdge->get_destination()) {
                verif++;
                index = 1;
            }
        }
        if (verif == 1) {
            return index;
        }
        return -1;
    }


    /**
     * Permet de demander un chemin de fichier à l'utilisateur
     * La réponses doit être plus longue que 3 sinon on conserve les paramètres par défaut
     * @param s
     * @return
     */
    std::string getFileName(std::string s = "test") {
        std::string fileName = getCurrentPath() + "/../" + s;
        std::string input = "";
        std::cout << "Default file :" << fileName << std::endl;
        std::cout << "Enter y to keep this file or enter new file name : " << std::endl;
        std::cin >> input;
        if (input.length() > 3) { //Si moins de 3 caractères ce n'est pas un chemin de fichier
            return input;
        }
        return fileName;
    }

    /**
     * Permet d'obtenir le chemin actuel (utilisation de cmake, d'ou l'ajout des .. dans le path)
     * @return
     */
    std::string getCurrentPath() {
        char buffer[2048];
        getcwd(buffer, 2048);
        std::string fileName(buffer);
        return fileName;
    }

    /**
     * Obtiens le nom de fichier pour l'entrée
     * @param s
     * @return
     */
    std::string getInputFileName(std::string s = "test") {
        s += ".dot";
        return getFileName(s);
    }

    /**
     * Obtiens le nom de fichier pour la sortie
     * @param state
     * @param s
     * @return
     */
    std::string getOutputFileName(int state, std::string s = "graph_circo") {
        s += "_state_" + std::to_string(state) + ".dot";
        return getFileName(s);
    }

};
