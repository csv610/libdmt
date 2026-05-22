#pragma once

#include "SimplicialComplex.hpp"
#include <map>
#include <set>
#include <vector>

namespace dmt {

/**
 * @brief Represents a Discrete Vector Field on a simplicial complex.
 * 
 * A DVF is a collection of pairs (face, coface) such that each simplex
 * appears in at most one pair.
 */
class DiscreteVectorField {
public:
    explicit DiscreteVectorField(const SimplicialComplex& complex) : complex_(complex) {}

    /**
     * @brief Generates the gradient vector field from the function values
     * defined in the complex.
     * @return true if the function was a valid Morse function.
     */
    bool from_morse_function() {
        if (!complex_.is_morse_function()) return false;

        pair_up_.clear();
        pair_down_.clear();

        for (int i = 0; i < complex_.num_simplices(); ++i) {
            double val = complex_.get_function_value(i);
            
            // Check cofaces for pairing (alpha < beta and f(beta) <= f(alpha))
            for (int co_id : complex_.get_cofaces(i)) {
                if (complex_.get_function_value(co_id) <= val) {
                    add_pair(i, co_id);
                    break; // Forman guarantees at most one
                }
            }
        }
        return true;
    }

    /**
     * @brief Adds a pair to the vector field.
     * @param face_id ID of the p-simplex.
     * @param coface_id ID of the (p+1)-simplex.
     * @return true if the pair was successfully added.
     */
    bool add_pair(int face_id, int coface_id) {
        const auto& face = complex_.get_simplex(face_id);
        const auto& coface = complex_.get_simplex(coface_id);

        // Check dimension
        if (coface.dimension() != face.dimension() + 1) return false;

        // Check if face is actually a face of coface
        const auto& coface_faces = complex_.get_faces(coface_id);
        if (coface_faces.find(face_id) == coface_faces.end()) return false;

        // Check if either simplex is already paired
        if (is_paired(face_id) || is_paired(coface_id)) return false;

        pair_up_[face_id] = coface_id;
        pair_down_[coface_id] = face_id;
        return true;
    }

    bool is_paired(int id) const {
        return pair_up_.count(id) || pair_down_.count(id);
    }

    bool is_critical(int id) const {
        return !is_paired(id);
    }

    int get_paired_coface(int face_id) const {
        auto it = pair_up_.find(face_id);
        return (it != pair_up_.end()) ? it->second : -1;
    }

    int get_paired_face(int coface_id) const {
        auto it = pair_down_.find(coface_id);
        return (it != pair_down_.end()) ? it->second : -1;
    }

    std::vector<int> critical_cells() const {
        std::vector<int> critical;
        for (int i = 0; i < complex_.num_simplices(); ++i) {
            if (is_critical(i)) {
                critical.push_back(i);
            }
        }
        return critical;
    }

    int morse_euler_characteristic() const {
        int chi = 0;
        for (int id : critical_cells()) {
            const auto& s = complex_.get_simplex(id);
            chi += (s.dimension() % 2 == 0) ? 1 : -1;
        }
        return chi;
    }

    /**
     * @brief Traces all V-paths from a (p+1)-simplex to p-simplices with SIGNS.
     */
    std::map<int, int> trace_v_paths_with_signs(int start_id) const {
        std::map<int, int> reached_critical;
        
        for (int alpha_1 : complex_.get_faces(start_id)) {
            int sign = complex_.incidence_sign(alpha_1, start_id);
            if (is_critical(alpha_1)) {
                reached_critical[alpha_1] += sign;
            } else {
                int beta_1 = get_paired_coface(alpha_1);
                if (beta_1 != -1 && beta_1 != start_id) {
                    std::set<int> visited;
                    // Formula: sign * (-sgn(beta_1, alpha_1))
                    int path_sign = sign * (-complex_.incidence_sign(alpha_1, beta_1));
                    trace_v_paths_recursive_signed(beta_1, alpha_1, path_sign, reached_critical, visited);
                }
            }
        }
        return reached_critical;
    }

    /**
     * @brief Traces all V-paths from a (p+1)-simplex to p-simplices.
     */
    std::map<int, int> trace_v_paths(int start_id) const {
        std::map<int, int> reached_critical;
        
        // Initial boundary of the (p+1)-simplex
        for (int face_id : complex_.get_faces(start_id)) {
            if (is_critical(face_id)) {
                reached_critical[face_id]++;
            } else {
                int paired_coface = get_paired_coface(face_id);
                // In DMT, we look for a path beta_0 -> alpha_1 -> beta_1 -> ... -> alpha_k
                // where alpha_1 is a face of beta_0 (start_id) and paired with beta_1.
                // My recursive function expects (beta, entering_face).
                if (paired_coface != -1 && paired_coface != start_id) {
                    std::set<int> visited;
                    trace_v_paths_recursive(paired_coface, face_id, reached_critical, visited);
                }
            }
        }
        return reached_critical;
    }

    void print_pairs() const {
        std::cout << "Pairs (face -> coface):" << std::endl;
        for (auto const& [f, c] : pair_up_) {
            std::cout << "  ";
            complex_.get_simplex(f).print();
            std::cout << " -> ";
            complex_.get_simplex(c).print();
            std::cout << std::endl;
        }
    }

    void print_critical_cells() const {
        std::cout << "Critical Cells:" << std::endl;
        auto critical = critical_cells();
        std::map<int, int> counts;
        for (int id : critical) {
            const auto& s = complex_.get_simplex(id);
            counts[s.dimension()]++;
            std::cout << "  Dim " << s.dimension() << " simplex ID " << id << ": ";
            s.print();
            std::cout << std::endl;
        }
        std::cout << "Summary of Critical Cells:" << std::endl;
        for (auto const& [dim, count] : counts) {
            std::cout << "  Dim " << dim << ": " << count << std::endl;
        }
        std::cout << "  Morse Euler Characteristic: " << morse_euler_characteristic() << std::endl;
    }

    /**
     * @brief Performs all possible elementary collapses based on the DVF.
     * Leaves only critical cells in the complex.
     */
    void collapse_complex(SimplicialComplex& complex) {
        for (auto const& [face_id, coface_id] : pair_up_) {
            complex.remove_simplex(face_id);
            complex.remove_simplex(coface_id);
        }
    }

    /**
     * @brief Checks if the discrete vector field is acyclic (a gradient field).
     */
    bool is_acyclic() const {
        std::set<int> visited;
        std::set<int> on_stack;

        for (int i = 0; i < complex_.num_simplices(); ++i) {
            const auto& s = complex_.get_simplex(i);
            // We only need to start from (p+1)-simplices that are paired down
            if (s.dimension() > 0 && is_paired(i) && get_paired_face(i) != -1) {
                if (has_cycle(i, visited, on_stack)) return false;
            }
        }
        return true;
    }

private:
    void trace_v_paths_recursive_signed(int beta_id, int entering_face_id, int current_sign,
                                        std::map<int, int>& reached_critical,
                                        std::set<int>& visited) const {
        if (visited.count(beta_id)) return;
        visited.insert(beta_id);

        for (int alpha_next : complex_.get_faces(beta_id)) {
            if (alpha_next == entering_face_id) continue;

            int edge_sign = complex_.incidence_sign(alpha_next, beta_id);
            int new_sign = current_sign * edge_sign;

            if (is_critical(alpha_next)) {
                reached_critical[alpha_next] += new_sign;
            } else {
                int beta_next = get_paired_coface(alpha_next);
                if (beta_next != -1) {
                    // Formula: new_sign * (-sgn(beta_next, alpha_next))
                    int next_sign = new_sign * (-complex_.incidence_sign(alpha_next, beta_next));
                    trace_v_paths_recursive_signed(beta_next, alpha_next, next_sign, reached_critical, visited);
                }
            }
        }
        visited.erase(beta_id);
    }

    void trace_v_paths_recursive(int beta_id, int entering_face_id, 
                                 std::map<int, int>& reached_critical,
                                 std::set<int>& visited) const {
        if (visited.count(beta_id)) return;
        visited.insert(beta_id);

        for (int alpha_id : complex_.get_faces(beta_id)) {
            if (alpha_id == entering_face_id) continue;

            if (is_critical(alpha_id)) {
                reached_critical[alpha_id]++;
            } else {
                int beta_next = get_paired_coface(alpha_id);
                if (beta_next != -1) {
                    trace_v_paths_recursive(beta_next, alpha_id, reached_critical, visited);
                }
            }
        }
        visited.erase(beta_id);
    }

    bool has_cycle(int beta_id, std::set<int>& visited, std::set<int>& on_stack) const {
        if (on_stack.count(beta_id)) return true;
        if (visited.count(beta_id)) return false;

        visited.insert(beta_id);
        on_stack.insert(beta_id);

        int alpha_id = get_paired_face(beta_id);
        if (alpha_id != -1) {
            // A V-path goes beta_i -> alpha_i+1 where alpha_i+1 is a face of beta_i
            // and alpha_i+1 is paired with beta_i+1.
            for (int face_id : complex_.get_faces(beta_id)) {
                if (face_id == alpha_id) continue;
                
                int beta_next = get_paired_coface(face_id);
                if (beta_next != -1) {
                    if (has_cycle(beta_next, visited, on_stack)) return true;
                }
            }
        }

        on_stack.erase(beta_id);
        return false;
    }

    const SimplicialComplex& complex_;
    std::map<int, int> pair_up_;   // p -> p+1
    std::map<int, int> pair_down_; // p+1 -> p
};

} // namespace dmt
