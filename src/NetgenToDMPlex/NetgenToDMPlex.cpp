#include "NetgenToDMPlex.hpp"
#include "MeshAlgorithm.hpp"
#include "MeshObject.hpp"


//----------------------------------------------------------------------------
NetgenToDMPlex::NetgenToDMPlex(
    const std::shared_ptr<MeshObject> &mesh,
    const std::shared_ptr<MeshAlgorithm> &algorithm)
    : _mesh(mesh)
      , _algorithm(algorithm) {
}

//----------------------------------------------------------------------------
PetscErrorCode NetgenToDMPlex::ConvertToDMPlex(MPI_Comm comm, DM *dm) {
    PetscFunctionBeginUser;

    int dim = _algorithm->GetDim();
    const int numPoints = _mesh->GetNP();
    int numCells = _mesh->GetNE();

    std::vector<double> coords(3 * numPoints);
    for (int i = 1; i <= numPoints; ++i) {
        const auto &p = _mesh->Point(i);
        coords[3 * (i - 1) + 0] = p(0);
        coords[3 * (i - 1) + 1] = p(1);
        coords[3 * (i - 1) + 2] = p(2);
    }

    std::vector<int> cells(4 * numCells);
    for (int i = 1; i <= numCells; ++i) {
        const netgen::Element &elem = _mesh->VolumeElement(i);
        const int elementType = elem.GetType();
        const auto vertices = elem.PNums();

        if (elementType == netgen::TET) {
            //! Tetrahedral element
            for (int j = 0; j < 4; ++j)
                cells[4 * (i - 1) + j] = vertices[j] - 1;
        }
    }

    PetscCall(DMPlexCreateFromCellListPetsc(comm, dim, numCells, numPoints, 4,
        PETSC_FALSE, cells.data(), 3, coords.data(), dm));

    PetscFunctionReturn(PETSC_SUCCESS);
}
