#include "SimplicialComplex.hpp"
#include "DiscreteVectorField.hpp"
#include <iostream>

using namespace dmt;

int main() {
    std::cout << "--- 2D Example: Triangle ---" << std::endl;
    SimplicialComplex sc;
    
    // Adding a 2-simplex (triangle) automatically adds its edges and vertices
    int tri_id = sc.add_simplex({0, 1, 2});
    
    sc.print_info();

    DiscreteVectorField dvf(sc);

    // Let's pair them:
    // Pair vertex 0 with edge (0,1)
    // Pair vertex 1 with edge (1,2)
    // Pair edge (0,2) with triangle (0,1,2)
    // Vertex 2 should be critical
    
    // We need to find the IDs
    int v0 = sc.add_simplex({0});
    int v1 = sc.add_simplex({1});
    int v2 = sc.add_simplex({2});
    int e01 = sc.add_simplex({0, 1});
    int e12 = sc.add_simplex({1, 2});
    int e02 = sc.add_simplex({0, 2});

    dvf.add_pair(v0, e01);
    dvf.add_pair(v1, e12);
    dvf.add_pair(e02, tri_id);

    dvf.print_critical_cells();

    if (sc.euler_characteristic() == dvf.morse_euler_characteristic()) {
        std::cout << "SUCCESS: Euler characteristic matches!" << std::endl;
    } else {
        std::cout << "FAILURE: Euler characteristic mismatch!" << std::endl;
    }

    return 0;
}
