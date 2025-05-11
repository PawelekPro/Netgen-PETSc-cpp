#include "BndCond.hpp"

void BoundaryConditions::RegisterBoundaryCondition(const BcdSurface &bcs) {
    surfaceRegions.push_back(bcs);
}

void BoundaryConditions::RegisterBoundaryCondition(const BcdVolume &bcs) {
    volumeRegions.push_back(bcs);
};
