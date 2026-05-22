#include "SimplicialComplex.hpp"
#include "DiscreteVectorField.hpp"
#include <iostream>

using namespace dmt;

int main() {
    std::cout << "--- 2D Example: Morse Function on a Triangle ---" << std::endl;
    SimplicialComplex sc;
    
    // Triangle {0, 1, 2}
    int t012 = sc.add_simplex({0, 1, 2});
    int e01 = sc.add_simplex({0, 1});
    int e12 = sc.add_simplex({1, 2});
    int e02 = sc.add_simplex({0, 2});
    int v0 = sc.add_simplex({0});
    int v1 = sc.add_simplex({1});
    int v2 = sc.add_simplex({2});

    std::cout << "Simplex IDs:" << std::endl;
    for (int i = 0; i < sc.num_simplices(); ++i) {
        std::cout << "  ID " << i << ": ";
        sc.get_simplex(i).print();
        std::cout << std::endl;
    }

    /**
     * Set ALL values to avoid default 0.0 interfering.
     * Natural order: f(v) < f(e) < f(t)
     */
    sc.set_function_value(v0, 10.0);
    sc.set_function_value(v1, 8.0); // Changed to be smaller than e01
    sc.set_function_value(v2, 30.0);
    sc.set_function_value(e01, 9.0); // Pair (v0, e01) because f(v0) >= f(e01)
    sc.set_function_value(e12, 21.0);
    sc.set_function_value(e02, 31.0);
    sc.set_function_value(t012, 40.0);

    if (sc.is_morse_function()) {
        std::cout << "SUCCESS: Function is a valid Discrete Morse Function." << std::endl;
    } else {
        std::cout << "FAILURE: Function is NOT a valid Discrete Morse Function." << std::endl;
    }

    DiscreteVectorField dvf(sc);
    if (dvf.from_morse_function()) {
        std::cout << "Successfully generated Gradient Vector Field from DMF." << std::endl;
        std::cout << "Is acyclic: " << (dvf.is_acyclic() ? "Yes" : "No") << std::endl;
        dvf.print_pairs();
        dvf.print_critical_cells();

        std::cout << "\n--- Collapsing Complex ---" << std::endl;
        std::cout << "Euler characteristic before collapse: " << sc.euler_characteristic() << std::endl;
        dvf.collapse_complex(sc);
        std::cout << "Euler characteristic after collapse: " << sc.euler_characteristic() << std::endl;
        std::cout << "Remaining simplices:" << std::endl;
        for (int i = 0; i < sc.num_simplices(); ++i) {
            if (!sc.is_removed(i)) {
                std::cout << "  ";
                sc.get_simplex(i).print();
                std::cout << std::endl;
            }
        }
    }
 else {
        std::cout << "Failed to generate GVF." << std::endl;
    }

    return 0;
}
