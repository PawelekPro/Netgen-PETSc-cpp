#include "FvmSetup.hpp"
#include "FvmMesh.hpp"
#include "BndCond.hpp"
#include "FvmVar.hpp"
#include "FvmVector.hpp"

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
    for (const auto element: _fvmMesh->elements) {
        FvmVector::V_SetCmp(&FvmVar::cex, element.index, element.cVec.x);
        FvmVector::V_SetCmp(&FvmVar::cey, element.index, element.cVec.y);
        FvmVector::V_SetCmp(&FvmVar::cez, element.index, element.cVec.z);
    }

    VecAssemblyBegin(FvmVar::cex);
    VecAssemblyEnd(FvmVar::cex);
    VecAssemblyBegin(FvmVar::cey);
    VecAssemblyEnd(FvmVar::cey);
    VecAssemblyBegin(FvmVar::cez);
    VecAssemblyEnd(FvmVar::cez);

    VecGhostUpdateBegin(FvmVar::cex, INSERT_VALUES, SCATTER_FORWARD);
    VecGhostUpdateEnd(FvmVar::cex, INSERT_VALUES, SCATTER_FORWARD);
    VecGhostUpdateBegin(FvmVar::cey, INSERT_VALUES, SCATTER_FORWARD);
    VecGhostUpdateEnd(FvmVar::cey, INSERT_VALUES, SCATTER_FORWARD);
    VecGhostUpdateBegin(FvmVar::cez, INSERT_VALUES, SCATTER_FORWARD);
    VecGhostUpdateEnd(FvmVar::cez, INSERT_VALUES, SCATTER_FORWARD);
}

void FvmSetup::SetInitialConditions() const {
    for (auto element: _fvmMesh->elements) {
        element.bc = BndCondType::NONE;

        FvmVector::V_SetCmp(&FvmVar::xu, element.index, 0.0);
        FvmVector::V_SetCmp(&FvmVar::xv, element.index, 0.0);
        FvmVector::V_SetCmp(&FvmVar::xw, element.index, 0.0);
        FvmVector::V_SetCmp(&FvmVar::xp, element.index, 0.0);
        FvmVector::V_SetCmp(&FvmVar::xT, element.index, 0.0);
        FvmVector::V_SetCmp(&FvmVar::xs, element.index, 0.0);
    }

    for (const auto bndCnd: _fvmBndCnd->GetVolumeRegions()) {
        for (auto element: _fvmMesh->elements) {
            if (element.phyReg == bndCnd.physReg) {
                element.bc = bndCnd.bc;

                FvmVector::V_SetCmp(&FvmVar::xu, element.index, bndCnd.fu);
                FvmVector::V_SetCmp(&FvmVar::xv, element.index, bndCnd.fv);
                FvmVector::V_SetCmp(&FvmVar::xw, element.index, bndCnd.fw);
                FvmVector::V_SetCmp(&FvmVar::xp, element.index, bndCnd.fp);
                FvmVector::V_SetCmp(&FvmVar::xT, element.index, bndCnd.fT);
                FvmVector::V_SetCmp(&FvmVar::xs, element.index, bndCnd.fs);
            }
        }
    }

    VecAssemblyBegin(FvmVar::xu);
    VecAssemblyEnd(FvmVar::xu);
    VecAssemblyBegin(FvmVar::xv);
    VecAssemblyEnd(FvmVar::xv);
    VecAssemblyBegin(FvmVar::xw);
    VecAssemblyEnd(FvmVar::xw);
    VecAssemblyBegin(FvmVar::xp);
    VecAssemblyEnd(FvmVar::xp);
    VecAssemblyBegin(FvmVar::xT);
    VecAssemblyEnd(FvmVar::xT);
    VecAssemblyBegin(FvmVar::xs);
    VecAssemblyEnd(FvmVar::xs);

    VecGhostUpdateBegin(FvmVar::xu, INSERT_VALUES, SCATTER_FORWARD);
    VecGhostUpdateEnd(FvmVar::xu, INSERT_VALUES, SCATTER_FORWARD);
    VecGhostUpdateBegin(FvmVar::xv, INSERT_VALUES, SCATTER_FORWARD);
    VecGhostUpdateEnd(FvmVar::xv, INSERT_VALUES, SCATTER_FORWARD);
    VecGhostUpdateBegin(FvmVar::xw, INSERT_VALUES, SCATTER_FORWARD);
    VecGhostUpdateEnd(FvmVar::xw, INSERT_VALUES, SCATTER_FORWARD);
    VecGhostUpdateBegin(FvmVar::xp, INSERT_VALUES, SCATTER_FORWARD);
    VecGhostUpdateEnd(FvmVar::xp, INSERT_VALUES, SCATTER_FORWARD);
    VecGhostUpdateBegin(FvmVar::xT, INSERT_VALUES, SCATTER_FORWARD);
    VecGhostUpdateEnd(FvmVar::xT, INSERT_VALUES, SCATTER_FORWARD);
    VecGhostUpdateBegin(FvmVar::xs, INSERT_VALUES, SCATTER_FORWARD);
    VecGhostUpdateEnd(FvmVar::xs, INSERT_VALUES, SCATTER_FORWARD);

    VecCopy(FvmVar::xu, FvmVar::xu0);
    VecCopy(FvmVar::xv, FvmVar::xv0);
    VecCopy(FvmVar::xw, FvmVar::xw0);
    VecCopy(FvmVar::xp, FvmVar::xp0);
    VecCopy(FvmVar::xT, FvmVar::xT0);
    VecCopy(FvmVar::xs, FvmVar::xs0);
}
