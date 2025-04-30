#ifndef PETSCNAVIERSTOKES_HPP
#define PETSCNAVIERSTOKES_HPP

#include "NetgenToDMPlex.hpp"
#include <petscdmplex.h>
#include <petscsnes.h>
#include <petscds.h>


class PETScNavierStokes {
public:
    PETScNavierStokes(std::shared_ptr<MeshObject> mesh, std::shared_ptr<MeshAlgorithm> algo)
        : _netgenMesh(mesh), _netgenAlgo(algo) {}

    PetscErrorCode Run(MPI_Comm comm);

private:
    static PetscErrorCode RHSFunction(TS ts, PetscReal t, Vec U, Vec R, void *ctx);
    static PetscErrorCode SetupProblem(DM dm);

    DM _dm;
    std::shared_ptr<MeshObject> _netgenMesh;
    std::shared_ptr<MeshAlgorithm> _netgenAlgo;
};



#endif
