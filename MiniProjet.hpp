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
#include <map>
#include <limits>

template<class T>
class MiniProjet {
private:
    Graph<std::string, int> *graph;
public:
    MiniProjet() {
        graph = new Graph<std::string, int>(true);
        readFileToGraph(graph); //Phase 1
        minimalRecoverGraph(graph); //Phase 2
    }

    void readFileToGraph(Graph<std::string, int> *graph) {
        std::ifstream myFile;
        std::string line;

        std::string fileName = getInputFileName();

        auto setOfElements = new std::set<std::string>();
        //auto vertex = new std::vector<Graph<std::string,std::string>::Vertex *>();
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
            auto it = setOfElements->begin();
            while (it != setOfElements->end()) {
                std::cout << *it << ",";
                it++;
            }
            myFile.close();
            exportCircoGraph(1);
        }
    }

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

    void minimalRecoverGraph(Graph<std::string, int> *g) {
        auto vertexInTree = new std::vector<Graph<std::string, int>::Vertex *>();
        auto edgeInTree = new std::set<Graph<std::string, int>::Edge *>();
        vertexInTree->push_back(g->get_vertex(0));
        while (vertexInTree->size() < g->num_vertices()) {
            int min = std::numeric_limits<int>::max();
            Graph<std::string, int>::Vertex *v;
            Graph<std::string, int>::Edge *e;
            for (auto vi = g->vertex_iterator(); vi.has_next(); vi.next()) {
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

    void affiche(Graph<std::string, int> *g) {
        std::string lien = g->is_directed() ? " -> " : " -- ";
        for (auto vi = g->vertex_iterator(); vi.has_next(); vi.next()) {
            auto curr = vi.current();
            std::cout << "vertex [" << curr->get_value() << "]" << std::endl;
            for (auto ni = curr->neighbor_iterator(); ni.has_next(); ni.next()) {
                auto neighbor_edge = ni.current();
                auto neighbor = neighbor_edge->get_destination(curr);
                std::cout << "  edge " << curr->get_value() << lien << neighbor->get_value() << " ["
                          << neighbor_edge->get_weight() << "] " << std::endl;
            }
        }
    }

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
            for (auto vi = graph->vertex_iterator(); vi.has_next(); vi.next()) {
                auto curr = vi.current();
                for (auto ni = curr->neighbor_iterator(); ni.has_next(); ni.next()) {
                    auto neighbor_edge = ni.current();
                    auto neighbor = neighbor_edge->get_destination(curr);
                    myFile << "\t" << curr->get_value() << lien << neighbor->get_value() << " [label="
                           << neighbor_edge->get_weight();

                    if (state == 2) {
                        if (std::find(edges->begin(), edges->end(), neighbor_edge) != edges->end()) {
                            myFile << ",penwith=3,color=\"red\"";
                        }
                    }

                    myFile << "]\n";
                }
            }
            myFile << "}";
            myFile.close();

            std::cout << "File write successful!" << std::endl;
        }
    }

    bool isMinimalEdge(Graph<std::string, int>::Vertex *src, Graph<std::string, int>::Vertex *dest) {
        int minimalValue = src->get_edge(0)->get_weight();
        int actualValue;
        for (auto ni = src->neighbor_iterator(); ni.has_next(); ni.next()) {
            if (minimalValue > ni.current()->get_weight()) {
                minimalValue = ni.current()->get_weight();
            }
            if (ni.current()->get_destination() == dest) {
                actualValue = ni.current()->get_weight();
            }
        }
        return minimalValue == actualValue;
    }

    std::string getFileName(std::string s = "test") {
        //std::string fileName = "/home/steven/Documents/2/Algorithme/miniproject/" + s;
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

    std::string getCurrentPath() {
        char buffer[2048];
        getcwd(buffer, 2048);
        std::string fileName(buffer);
        return fileName;
    }

    std::string getInputFileName(std::string s = "test") {
        s += ".dot";
        return getFileName(s);
    }

    std::string getOutputFileName(int state, std::string s = "graph_circo") {
        s += "_state_" + std::to_string(state) + ".dot";
        return getFileName(s);
    }

};
