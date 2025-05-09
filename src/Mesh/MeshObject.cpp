#include "MeshObject.hpp"

//----------------------------------------------------------------------------
MeshObject::MeshObject() : netgen::Mesh() {
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
