# libdmt: Discrete Morse Theory in C++

`libdmt` is a C++17 header-only library implementing Robin Forman's Discrete Morse Theory (DMT) for simplicial complexes. It provides tools for topological analysis, gradient flow tracing, and complex simplification via elementary collapses.

## Core Features

*   **Simplicial Complex Management**: Efficient representation of simplices and their incidence relations (Hasse diagram).
*   **Discrete Morse Functions (DMF)**: Tools to define and validate scalar functions satisfying Forman's conditions.
*   **Gradient Vector Fields (GVF)**: Automatic generation of gradient fields from DMFs and explicit acyclicity verification.
*   **Morse Complex Analysis**: 
    *   Identification of critical cells.
    *   V-path (gradient flow) tracing between simplices of different dimensions.
    *   Calculation of signed boundary operator coefficients for the reduced Morse complex.
*   **Topological Simplification**: Support for elementary collapses to reduce a complex to its critical cells while preserving homotopy type.

## Mathematical Foundation

The implementation follows the combinatorial framework defined in Robin Forman's *"A User's Guide to Discrete Morse Theory"*. It verifies the Morse-Euler characteristic equality:
$$\chi(K) = \sum_{p=0}^{\dim K} (-1)^p m_p$$
where $m_p$ is the number of critical $p$-simplices.

## Getting Started

### Prerequisites

*   C++17 compatible compiler (e.g., GCC 9+, Clang 10+, MSVC 2019+)
*   CMake 3.10+
*   Git (for fetching Catch2 during tests)

### Installation

`libdmt` is header-only. Copy the contents of the `include/` directory to your project, or add it to your include paths.

### Building Examples and Tests

```bash
git clone git@github.com:csv610/libdmt.git
cd libdmt
mkdir build && cd build
cmake ..
make
```

## Usage

### Defining a Simplicial Complex
```cpp
#include "SimplicialComplex.hpp"

dmt::SimplicialComplex sc;
// Adding a 2-simplex automatically adds its edges and vertices
sc.add_simplex({0, 1, 2}); 
```

### Working with Discrete Vector Fields
```cpp
#include "DiscreteVectorField.hpp"

dmt::DiscreteVectorField dvf(sc);
dvf.add_pair(face_id, coface_id);

if (dvf.is_acyclic()) {
    auto critical = dvf.critical_cells();
}
```

## Testing

The library uses Catch2 for unit testing. After building, run the tests with:

```bash
./unit_tests
```

## References

*   Forman, Robin. "A user's guide to discrete Morse theory." *Sém. Lothar. Combin* 48 (2002): 35pp.
