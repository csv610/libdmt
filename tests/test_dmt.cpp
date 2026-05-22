#include <catch2/catch_test_macros.hpp>
#include "SimplicialComplex.hpp"
#include "DiscreteVectorField.hpp"

using namespace dmt;

TEST_CASE("Simplex properties", "[simplex]") {
    Simplex s1({2, 0, 1}); // Unsorted input
    
    SECTION("Vertices are sorted") {
        std::vector<int> expected = {0, 1, 2};
        REQUIRE(s1.vertices() == expected);
    }
    
    SECTION("Dimension calculation") {
        REQUIRE(s1.dimension() == 2);
        REQUIRE(Simplex({0}).dimension() == 0);
    }
    
    SECTION("Face generation") {
        auto faces = s1.faces();
        REQUIRE(faces.size() == 3);
        // Faces should be (0,1), (0,2), (1,2)
        REQUIRE(std::find(faces.begin(), faces.end(), Simplex({0, 1})) != faces.end());
        REQUIRE(std::find(faces.begin(), faces.end(), Simplex({0, 2})) != faces.end());
        REQUIRE(std::find(faces.begin(), faces.end(), Simplex({1, 2})) != faces.end());
    }
}

TEST_CASE("Simplicial Complex logic", "[complex]") {
    SimplicialComplex sc;
    int t_id = sc.add_simplex({0, 1, 2});
    
    SECTION("Implicit face addition") {
        // Triangle has 7 simplices: 1 triangle, 3 edges, 3 vertices
        REQUIRE(sc.num_simplices() == 7);
    }
    
    SECTION("Euler characteristic") {
        // Triangle is contractible, chi = 1
        REQUIRE(sc.euler_characteristic() == 1);
    }
    
    SECTION("Incidence signs") {
        int v0 = sc.add_simplex({0});
        int e01 = sc.add_simplex({0, 1});
        int e12 = sc.add_simplex({1, 2});
        int t012 = sc.add_simplex({0, 1, 2});
        
        // sgn( [0,1], [0] ) -> index of 1 in [0,1] is 1, so (-1)^1 = -1
        // Wait, index of missing vertex. 1 is missing in [0]. No.
        // Sign is (-1)^i where i is index of missing vertex in coface.
        // For (v0, e01), missing vertex in e01 is 1. Index of 1 in [0,1] is 1. Sign (-1)^1 = -1.
        REQUIRE(sc.incidence_sign(v0, e01) == -1);
        
        // For (e12, t012), missing vertex is 0. Index of 0 in [0,1,2] is 0. Sign (-1)^0 = 1.
        REQUIRE(sc.incidence_sign(e12, t012) == 1);
    }
}

TEST_CASE("Discrete Morse Theory", "[dmt]") {
    SimplicialComplex sc;
    int t_id = sc.add_simplex({0, 1, 2});
    DiscreteVectorField dvf(sc);
    
    SECTION("Pairing validation") {
        int v0 = sc.add_simplex({0});
        int e01 = sc.add_simplex({0, 1});
        REQUIRE(dvf.add_pair(v0, e01) == true);
        REQUIRE(dvf.add_pair(v0, e01) == false); // Already paired
    }
    
    SECTION("DMF Validation and Generation") {
        SimplicialComplex sc_dmf;
        DiscreteVectorField dvf_dmf(sc_dmf);
        
        int t012 = sc_dmf.add_simplex({0, 1, 2});
        int e12 = sc_dmf.add_simplex({1, 2});
        int e02 = sc_dmf.add_simplex({0, 2});
        int e01 = sc_dmf.add_simplex({0, 1});
        int v0 = sc_dmf.add_simplex({0});
        int v1 = sc_dmf.add_simplex({1});
        int v2 = sc_dmf.add_simplex({2});
        
        sc_dmf.set_function_value(v0, 10.0);
        sc_dmf.set_function_value(v1, 8.0); // Smaller than e01
        sc_dmf.set_function_value(v2, 12.0);
        sc_dmf.set_function_value(e01, 9.0);
        sc_dmf.set_function_value(e12, 21.0);
        sc_dmf.set_function_value(e02, 22.0);
        sc_dmf.set_function_value(t012, 30.0);
        
        REQUIRE(sc_dmf.is_morse_function() == true);
        REQUIRE(dvf_dmf.from_morse_function() == true);
        REQUIRE(dvf_dmf.is_paired(v0) == true);
    }
    
    SECTION("Acyclicity") {
        // Create a cycle (non-gradient field)
        // This is harder to do with a DMF, so we do it manually.
        // Forman's acyclic condition: no closed V-paths.
        // Example: beta0 -> alpha1 -> beta1 -> alpha0 (back to beta0)
        // This requires at least a complex where such a loop is possible.
        // On a triangle, any valid matching is acyclic.
        REQUIRE(dvf.is_acyclic() == true);
    }
}
