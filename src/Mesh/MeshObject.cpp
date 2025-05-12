#include "MeshObject.hpp"

#include "NetgenPluginLibWrapper.hpp"
#include "petscksp.h"

//----------------------------------------------------------------------------
MeshObject::MeshObject() : netgen::Mesh() {
}

//----------------------------------------------------------------------------
void MeshObject::DecomposeMesh(const int nProc) {
    if (nProc <= 1) {
        throw std::runtime_error("Error, number of processors must be greater than 1");
    }
    PetscPrintf(PETSC_COMM_WORLD, "Decomposing domain to %d partitions\n", nProc);
    NetgenPluginLibWrapper ngLib;
    this->ParallelMetis(nProc + 1);
    this->SetProcNumber(nProc);
}

//----------------------------------------------------------------------------
void MeshObject::SetPhysicalSurfaceRegionLabel(
    const int index, const std::string &label) {
    _physicalSurfaceRegions[index] = label;
}

//----------------------------------------------------------------------------
std::string MeshObject::GetPhysicalSurfaceRegionLabel(const int index) const {
    return _physicalSurfaceRegions.at(index);
}

//----------------------------------------------------------------------------
void MeshObject::SetPhysicalVolumeRegionLabel(
    const int index, const std::string &label) {
    _physicalVolumeRegions[index] = label;
}

//----------------------------------------------------------------------------
std::string MeshObject::GetPhysicalVolumeRegionLabel(const int index) const {
    return _physicalVolumeRegions.at(index);
}
