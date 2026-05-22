#include "SimplicialComplex.hpp"
#include "DiscreteVectorField.hpp"
#include <iostream>

using namespace dmt;

int main() {
    std::cout << "--- 3D Example: Tetrahedron ---" << std::endl;
    SimplicialComplex sc;
    
    // Tetrahedron
    int tet_id = sc.add_simplex({0, 1, 2, 3});
    
    sc.print_info();

    DiscreteVectorField dvf(sc);

    // Vertex pairings
    dvf.add_pair(sc.add_simplex({0}), sc.add_simplex({0, 1}));
    dvf.add_pair(sc.add_simplex({1}), sc.add_simplex({1, 2}));
    dvf.add_pair(sc.add_simplex({2}), sc.add_simplex({2, 3}));
    // Vertex {3} is critical

    // Edge pairings
    dvf.add_pair(sc.add_simplex({0, 2}), sc.add_simplex({0, 1, 2}));
    dvf.add_pair(sc.add_simplex({0, 3}), sc.add_simplex({0, 1, 3}));
    dvf.add_pair(sc.add_simplex({1, 3}), sc.add_simplex({1, 2, 3}));

    // Face pairing
    dvf.add_pair(sc.add_simplex({0, 2, 3}), tet_id);

    dvf.print_critical_cells();

    if (sc.euler_characteristic() == dvf.morse_euler_characteristic()) {
        std::cout << "SUCCESS: Euler characteristic matches!" << std::endl;
    } else {
        std::cout << "FAILURE: Euler characteristic mismatch!" << std::endl;
    }

    return 0;
}
