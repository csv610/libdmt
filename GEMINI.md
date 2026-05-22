# Discrete Morse Theory in C++

This project provides a comprehensive implementation of Robin Forman's Discrete Morse Theory (DMT), as described in his foundational paper *"A User's Guide to Discrete Morse Theory"*.

## Implemented Concepts

### 1. Simplicial Complexes & Hasse Diagrams
- Represent complexes of any dimension.
- Automatically builds incidence relations (faces and cofaces) to form a Hasse diagram.
- Support for sorted vertex representation to ensure unique simplex identification.

### 2. Discrete Morse Functions (DMF)
- Assign scalar values to every simplex in the complex.
- **Validator**: Ensures function values satisfy Forman's conditions:
  - Each simplex has at most one coface with a smaller/equal value.
  - Each simplex has at most one face with a larger/equal value.
  - Lemma: Only one of the above can be true for any given simplex.

### 3. Gradient Vector Fields (GVF)
- **Automatic Generation**: Extract the unique gradient vector field from a valid DMF.
- **Acyclicity Check**: Explicitly verifies that the vector field contains no closed V-paths (cycles), a requirement for it to be a true gradient field.

### 4. Critical Cells & Euler Characteristic
- Identification of "unpaired" simplices (critical cells).
- Verification of Morse-Euler equality: $\chi(K) = \sum (-1)^p m_p$.

### 5. V-Paths & Morse Complex
- **Path Tracing**: Follow the gradient flow from $(p+1)$-cells to $p$-cells.
- **Signed Boundary Coefficients**: Uses standard orientations and incidence signs to compute the boundary operator for the reduced Morse Complex:
  $\langle \partial_M \beta, \alpha \rangle = \sum_{\rho \in \mathcal{V}(\beta, \alpha)} w(\rho)$

### 6. Topological Collapsing
- Simplified the original complex by performing elementary collapses based on the vector field.
- Demonstrates how the complex can be reduced to just its critical cells while preserving its homotopy type.

## Building and Running
```bash
mkdir build && cd build
cmake .. && make
./example_2d
./example_3d
./example_surface
./example_dmf
```

## Examples
- `example_2d`: Basic triangle properties.
- `example_3d`: Tetrahedron volume properties.
- `example_surface`: Triangulated sphere (boundary of tetrahedron) with V-path analysis.
- `example_dmf`: Defining a Discrete Morse Function, validating it, generating a field, and collapsing the complex.
