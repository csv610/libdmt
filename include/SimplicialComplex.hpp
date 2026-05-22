#pragma once

#include "Simplex.hpp"
#include <vector>
#include <map>
#include <set>
#include <memory>
#include <stdexcept>

namespace dmt {

/**
 * @brief Manages a collection of simplices forming a simplicial complex.
 */
class SimplicialComplex {
public:
    SimplicialComplex() {}

    /**
     * @brief Adds a simplex to the complex. Automatically adds all its faces.
     * @return The ID of the added (or existing) simplex.
     */
    int add_simplex(const std::vector<int>& vertices) {
        Simplex s(vertices);
        auto it = simplex_to_id_.find(s.vertices());
        if (it != simplex_to_id_.end()) {
            return it->second;
        }

        // Assign new ID
        int id = static_cast<int>(simplices_.size());
        s.set_id(id);
        simplices_.push_back(s);
        simplex_to_id_[s.vertices()] = id;

        // Ensure all faces are added
        if (s.dimension() > 0) {
            for (const auto& face : s.faces()) {
                int face_id = add_simplex(face.vertices());
                faces_[id].insert(face_id);
                cofaces_[face_id].insert(id);
            }
        }

        return id;
    }

    const Simplex& get_simplex(int id) const {
        if (id < 0 || id >= static_cast<int>(simplices_.size())) {
            throw std::out_of_range("Simplex ID out of range");
        }
        return simplices_[id];
    }

    int num_simplices() const {
        return static_cast<int>(simplices_.size());
    }

    const std::set<int>& get_faces(int id) const {
        auto it = faces_.find(id);
        if (it == faces_.end()) {
            static const std::set<int> empty;
            return empty;
        }
        return it->second;
    }

    const std::set<int>& get_cofaces(int id) const {
        auto it = cofaces_.find(id);
        if (it == cofaces_.end()) {
            static const std::set<int> empty;
            return empty;
        }
        return it->second;
    }

    /**
     * @brief Computes the incidence sign <boundary(coface), face>
     */
    int incidence_sign(int face_id, int coface_id) const {
        const auto& face = get_simplex(face_id);
        const auto& coface = get_simplex(coface_id);

        if (coface.dimension() != face.dimension() + 1) return 0;

        // Find the missing vertex
        int missing_v = -1;
        const auto& f_v = face.vertices();
        const auto& c_v = coface.vertices();
        
        // Since they are sorted, we can find it easily
        size_t f_idx = 0;
        for (size_t c_idx = 0; c_idx < c_v.size(); ++c_idx) {
            if (f_idx < f_v.size() && f_v[f_idx] == c_v[c_idx]) {
                f_idx++;
            } else {
                missing_v = c_v[c_idx];
                // The index in coface determines the sign (-1)^i
                return (c_idx % 2 == 0) ? 1 : -1;
            }
        }
        return 0;
    }

    void remove_simplex(int id) {
        if (id < 0 || id >= static_cast<int>(simplices_.size())) return;
        
        // Remove from Hasse diagram
        for (int face_id : faces_[id]) {
            cofaces_[face_id].erase(id);
        }
        for (int coface_id : cofaces_[id]) {
            faces_[coface_id].erase(id);
        }
        
        faces_.erase(id);
        cofaces_.erase(id);
        
        // We don't remove from simplices_ vector to keep IDs stable,
        // but we can mark it as "removed".
        removed_.insert(id);
    }

    bool is_removed(int id) const {
        return removed_.count(id);
    }

    int euler_characteristic() const {
        int chi = 0;
        for (int i = 0; i < num_simplices(); ++i) {
            if (is_removed(i)) continue;
            const auto& s = get_simplex(i);
            chi += (s.dimension() % 2 == 0) ? 1 : -1;
        }
        return chi;
    }

    void set_function_value(int id, double value) {
        if (id < 0 || id >= static_cast<int>(simplices_.size())) return;
        function_values_[id] = value;
    }

    double get_function_value(int id) const {
        auto it = function_values_.find(id);
        return (it != function_values_.end()) ? it->second : 0.0;
    }

    /**
     * @brief Validates if the current function values satisfy the 
     * Discrete Morse Function conditions.
     */
    bool is_morse_function() const {
        for (int i = 0; i < num_simplices(); ++i) {
            double val = get_function_value(i);
            
            int coface_count = 0;
            for (int co_id : get_cofaces(i)) {
                if (get_function_value(co_id) <= val) {
                    coface_count++;
                }
            }

            int face_count = 0;
            for (int f_id : get_faces(i)) {
                if (get_function_value(f_id) >= val) {
                    face_count++;
                }
            }

            if (coface_count > 1 || face_count > 1 || (coface_count == 1 && face_count == 1)) {
                return false;
            }
        }
        return true;
    }

    void print_info() const {
        std::cout << "Simplicial Complex Info:" << std::endl;
        std::cout << "  Num simplices: " << simplices_.size() << std::endl;
        std::cout << "  Euler characteristic: " << euler_characteristic() << std::endl;
        
        std::map<int, int> dim_counts;
        for (const auto& s : simplices_) {
            dim_counts[s.dimension()]++;
        }
        for (auto const& [dim, count] : dim_counts) {
            std::cout << "  Dim " << dim << ": " << count << std::endl;
        }
    }

private:
    std::vector<Simplex> simplices_;
    std::map<std::vector<int>, int> simplex_to_id_;
    
    // Hasse diagram relations: ID -> Set of IDs
    std::map<int, std::set<int>> faces_;   // codimension 1 faces
    std::map<int, std::set<int>> cofaces_; // codimension 1 cofaces

    std::map<int, double> function_values_;
    std::set<int> removed_;
};

} // namespace dmt
