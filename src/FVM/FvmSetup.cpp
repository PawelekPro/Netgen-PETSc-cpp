#include "FvmSetup.hpp"
#include "FvmMesh.hpp"
#include "BndCond.hpp"

FvmSetup::FvmSetup(
    const std::shared_ptr<FvmMeshContainer> &fvmMesh,
    const std::shared_ptr<BoundaryConditions> &fvmBndCnd,
    const std::shared_ptr<MaterialsBase> &materialsBase)
    : _fvmMesh(fvmMesh)
      , _fvmBndCnd(fvmBndCnd)
      , _materialsBase(materialsBase) {
};

void FvmSetup::SetGhosts() const {
    int ghostsNb = 0;
    for (auto &face: _fvmMesh->faces) {
        if (face.pair != -1)
            continue;

        if (face.bc == BndCondType::PROCESSOR) {
            face.ghost = _fvmMesh->elementsNb + ghostsNb;
            ++ghostsNb;
        }
    }

    _fvmMesh->ghosts.clear();
    _fvmMesh->ghosts.reserve(ghostsNb);

    for (const auto &face: _fvmMesh->faces) {
        if (face.pair != -1)
            continue;

        if (face.bc == BndCondType::PROCESSOR) {
            _fvmMesh->ghosts.push_back(face.physReg);
        }
    }
}

void FvmSetup::SetCenters() const {
    for (auto element: _fvmMesh->elements) {
        V_
    }
}
