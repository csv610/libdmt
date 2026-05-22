#include "SimplicialComplex.hpp"
#include "DiscreteVectorField.hpp"
#include <iostream>

using namespace dmt;

int main() {
    std::cout << "--- Surface Example: Triangulated Sphere (Boundary of Tetrahedron) ---" << std::endl;
    SimplicialComplex sc;
    
    // Boundary of a tetrahedron {0,1,2,3} consists of 4 triangles
    sc.add_simplex({0, 1, 2});
    sc.add_simplex({0, 1, 3});
    sc.add_simplex({0, 2, 3});
    sc.add_simplex({1, 2, 3});
    
    sc.print_info();

    DiscreteVectorField dvf(sc);

    // Let's adjust to ensure a path.
    // Vertex pairings:
    dvf.add_pair(sc.add_simplex({0}), sc.add_simplex({0, 3}));
    dvf.add_pair(sc.add_simplex({1}), sc.add_simplex({1, 3}));
    dvf.add_pair(sc.add_simplex({2}), sc.add_simplex({2, 3}));
    // Vertex {3} is critical.
    
    // Edge pairings:
    dvf.add_pair(sc.add_simplex({1, 2}), sc.add_simplex({1, 2, 3}));
    dvf.add_pair(sc.add_simplex({0, 2}), sc.add_simplex({0, 2, 3}));
    // Edge {0, 1} and Face {0, 1, 2} and Face {0, 1, 3} are critical.
    // Wait, this might change the Euler characteristic if I have too many.
    // Let's just make {0,1} critical and pair {0,1,3} with something else.
    // Actually, if {0,1,3} is unpaired, it's critical.
    
    dvf.print_pairs();
    dvf.print_critical_cells();

    std::cout << "\n--- Morse Complex Analysis ---" << std::endl;
    auto criticals = dvf.critical_cells();
    for (int id : criticals) {
        const auto& s = sc.get_simplex(id);
        std::cout << "Analyzing critical simplex ";
        s.print();
        std::cout << " (ID " << id << ", Dim " << s.dimension() << "):" << std::endl;
        
        if (s.dimension() == 2) {
            std::cout << "  Boundary faces of ID " << id << ": ";
            for (int f_id : sc.get_faces(id)) {
                sc.get_simplex(f_id).print();
                std::cout << " (ID " << f_id << ") ";
            }
            std::cout << std::endl;

            // Find all 1-simplices reached from this 2-simplex
            auto reached_1 = dvf.trace_v_paths_with_signs(id);
            if (reached_1.empty()) {
                std::cout << "  - No critical 1-simplices reached." << std::endl;
            }
            for (auto const& [e_id, count_e] : reached_1) {
                std::cout << "  - Reaches 1-simplex ";
                sc.get_simplex(e_id).print();
                std::cout << " (ID " << e_id << ") with coefficient " << count_e << std::endl;
                
                auto reached_0 = dvf.trace_v_paths_with_signs(e_id);
                for (auto const& [v_id, count_v] : reached_0) {
                    std::cout << "    * Reaches 0-simplex ";
                    sc.get_simplex(v_id).print();
                    std::cout << " (ID " << v_id << ") with coefficient " << count_v << std::endl;
                }
            }
        } else if (s.dimension() == 0) {
            std::cout << "  (Critical vertex, endpoint of paths)" << std::endl;
        }
    }

    if (sc.euler_characteristic() == dvf.morse_euler_characteristic()) {
        std::cout << "SUCCESS: Euler characteristic matches!" << std::endl;
    } else {
        std::cout << "FAILURE: Euler characteristic mismatch!" << std::endl;
    }

    return 0;
}
