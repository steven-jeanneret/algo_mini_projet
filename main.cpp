#include <iostream>
#include "MiniProjet.hpp"

/**
 * Le fichier .dot avec le quel les tests ont été effectués se trouve dans l'archive, les autres fichiers doivent respecter cette convention
 * Si les noms de fichier par défaut sont conservés, la commande suivante permet de transformer les 4 graphes .dot en .pdf
 * dot -Tpdf graph_circo_state_1.dot -o circo_1.pdf ; dot -Tpdf graph_circo_state_2.dot -o circo_2.pdf; dot -Tpdf graph_circo_state_3.dot -o circo_3.pdf; dot -Tpdf graph_circo_state_4.dot -o circo_4.pdf
 * @return
 */

int main() {
    auto projet = new MiniProjet();
    projet->readFileToGraph(); //Phase 1
    projet->minimalRecoverGraph(); //Phase 2
    projet->selectShortestPath(); //Phase 3
    projet->bestShortestPath(); //Phase 4
    return 0;
}