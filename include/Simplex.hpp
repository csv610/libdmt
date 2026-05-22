#pragma once

#include <vector>
#include <algorithm>
#include <iostream>

namespace dmt {

/**
 * @brief Represents a simplex in a simplicial complex.
 * 
 * A simplex is defined by its set of vertices. We maintain them in sorted order
 * to facilitate unique identification and comparison.
 */
class Simplex {
public:
    Simplex() : id_(-1) {}
    
    explicit Simplex(const std::vector<int>& vertices) : vertices_(vertices), id_(-1) {
        std::sort(vertices_.begin(), vertices_.end());
    }

    int dimension() const {
        return static_cast<int>(vertices_.size()) - 1;
    }

    const std::vector<int>& vertices() const {
        return vertices_;
    }

    int id() const { return id_; }
    void set_id(int id) { id_ = id; }

    bool operator==(const Simplex& other) const {
        return vertices_ == other.vertices_;
    }

    bool operator<(const Simplex& other) const {
        if (vertices_.size() != other.vertices_.size()) {
            return vertices_.size() < other.vertices_.size();
        }
        return vertices_ < other.vertices_;
    }

    /**
     * @brief Generates all faces of codimension 1.
     */
    std::vector<Simplex> faces() const {
        std::vector<Simplex> result;
        if (dimension() <= 0) return result;

        for (size_t i = 0; i < vertices_.size(); ++i) {
            std::vector<int> face_vertices;
            face_vertices.reserve(vertices_.size() - 1);
            for (size_t j = 0; j < vertices_.size(); ++j) {
                if (i == j) continue;
                face_vertices.push_back(vertices_[j]);
            }
            result.emplace_back(face_vertices);
        }
        return result;
    }

    int vertex_index(int v) const {
        for (size_t i = 0; i < vertices_.size(); ++i) {
            if (vertices_[i] == v) return static_cast<int>(i);
        }
        return -1;
    }

    void print() const {
        std::cout << "(";
        for (size_t i = 0; i < vertices_.size(); ++i) {
            std::cout << vertices_[i] << (i == vertices_.size() - 1 ? "" : ", ");
        }
        std::cout << ")";
    }

private:
    std::vector<int> vertices_;
    int id_;
};

} // namespace dmt
