#include "ParallelRecursiveMerge.hpp"
#include "Metrics.hpp"
#include <vector>
#include <numeric>
#include <iostream>
#include <sstream>
#include <chrono>
#include <cstdlib>
#include <tbb/global_control.h>

/**
 * Programme principal.
 *
 * @param[in] argc le nombre d'arguments de la ligne de commandes.
 * @param[in] argv les arguments de la ligne de commandes.
 * @return @c EXIT_SUCCESS en cas d'exécution réussie ou @c EXIT_FAILURE en cas
 *   de problèmes.
 */
int main(int argc, char* argv[]) {
    // La ligne de commandes est vide : l'utilisateur demande de l'aide.
    if (argc == 1) {
        std::cout << "Usage: " << argv[0] << " nb_iterations" << std::endl;
        return EXIT_SUCCESS;
    }

    // Le nombre d'arguments est différent de 1 : l'utilisateur fait n'importe quoi.
    if (argc != 2) {
        std::cerr << "Nombre d'argument(s) incorrect." << std::endl;
        return EXIT_FAILURE;
    }

    // Tentative d'extraction du nombre d'itérations.
    size_t iters;
    {
        std::istringstream entree(argv[1]);
        entree >> iters;
        if (!entree || !entree.eof()) {
            std::cerr << "Argument incorrect." << std::endl;
            return EXIT_FAILURE;
        }
    }

    // Synonyme du type des éléments à fusionner.
    typedef int Type;

    // Relation d'ordre utilisée : strictement inférieur à.
    const auto comp = std::less<const Type&>();

    // Deux tableaux d'entiers de taille différentes à fusionner.
    std::vector<Type> lhs(128 * 1024), rhs(lhs.size() + 211);
    std::iota(lhs.begin(), lhs.end(), 19);
    std::iota(rhs.begin(), rhs.end(), 5);

    // Conteneur accueillant le résultat de la fusion.
    std::vector<Type> result(lhs.size() + rhs.size());

    // Durée d'exécution de l'algorithme merge de la bibliothèque standard.
    double seq;
    {
        auto start = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i != iters; i++) {
            std::merge(lhs.begin(),
                       lhs.end(),
                       rhs.begin(),
                       rhs.end(),
                       result.begin(),
                       comp);
        }
        auto stop = std::chrono::high_resolution_clock::now();
        seq = std::chrono::duration<double>(stop - start).count();
    }

    // Affichage des performances de la version séquentielle.
    std::cout << "--[ merge: begin ]--" << std::endl;
    std::cout << "\tDurée:\t\t" << seq << " sec." << std::endl;
    std::cout << "\tVerdict:\t\t"
              << std::boolalpha
              << std::is_sorted(result.begin(), result.end(), comp)
              << std::endl;
    std::cout << "--[ merge: end ]--" << std::endl;
    std::cout << std::endl;

    // Obtention du nombre de threads disponibles via TBB.
    const int threads = tbb::global_control::active_value(tbb::global_control::max_allowed_parallelism);
    tbb::global_control control(tbb::global_control::max_allowed_parallelism, threads);

    // Durées d'exécution de l'algorithme ParallelRecursiveMerge. Nous 
    // allons utiliser plusieurs valeurs du cutoff.
    for (size_t cutoff = 1024;
         cutoff < result.size();
         cutoff += 1024) {
        // Durée d'exécution de notre version parallèle.
        auto start = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i != iters; i++) {
            merging::ParallelRecursiveMerge::apply(lhs.begin(),
                                                   lhs.end(),
                                                   rhs.begin(),
                                                   rhs.end(),
                                                   result.begin(),
                                                   comp,
                                                   cutoff);
        }
        auto stop = std::chrono::high_resolution_clock::now();
        double par = std::chrono::duration<double>(stop - start).count();

        // Affichage des performances de notre version parallèle.
        std::cout << "--[ ParallelRecursiveMerge("
                  << cutoff
                  << "): begin ]--" << std::endl;
        std::cout << "\tThread(s):\t" << threads << std::endl;
        std::cout << "\tDurée:\t\t" << par << " sec." << std::endl;
        std::cout << "\tVerdict:\t\t"
                  << std::boolalpha
                  << std::is_sorted(result.begin(), result.end(), comp)
                  << std::endl;
        std::cout << "\tSpeedup:\t"
                  << Metrics::speedup(seq, par)
                  << std::endl;
        std::cout << "\tEfficiency:\t"
                  << Metrics::efficiency(seq, par, threads)
                  << std::endl;
        std::cout << "--[ ParallelRecursiveMerge("
                  << cutoff
                  << "): end ]--" << std::endl;
        std::cout << std::endl;
    }

    // Tout s'est bien passé.
    return EXIT_SUCCESS;
}