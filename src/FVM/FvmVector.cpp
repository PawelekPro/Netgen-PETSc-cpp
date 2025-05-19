#include "FvmVector.hpp"

FvmVector *FvmVector::_instance = nullptr;

FvmVector::FvmVector(const std::shared_ptr<FvmMeshContainer> &fvmMesh)
    : _ghostsNb(fvmMesh->ghostsNb),
      _ghostsVec(fvmMesh->ghosts) {
}


void FvmVector::V_SetCmp(const Vec *v, const int ind, const double value) {
    VecSetValue(*v, ind, value, INSERT_VALUES);
}

double FvmVector::V_GetCmp(const Vec *v, const int ind) {
    double value;
    VecGetValues(*v, 1, &ind, &value);
    return value;
}

void FvmVector::V_Constr(Vec *v, const int n, const int sequential) {
    if (sequential == 1) {
        VecCreateSeq(PETSC_COMM_SELF, n, v);
    } else {
        const auto &inst = FvmVector::Instance();
        VecCreateGhost(
            PETSC_COMM_WORLD,
            n,
            PETSC_DECIDE,
            inst._ghostsNb,
            inst._ghostsVec.data(), v);
    }

    VecSetFromOptions(*v);
}
