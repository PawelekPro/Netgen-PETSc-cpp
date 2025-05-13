#include "BndCond.hpp"

BoundaryConditions::BoundaryConditions() = default;

void BoundaryConditions::RegisterBoundaryCondition(const BcdSurface &bcs) {
    _surfaceRegions.push_back(bcs);
}

void BoundaryConditions::RegisterBoundaryCondition(const BcdVolume &bcs) {
    _volumeRegions.push_back(bcs);
};

void BoundaryConditions::Clear() {
    _surfaceRegions.clear();
    _volumeRegions.clear();
}
