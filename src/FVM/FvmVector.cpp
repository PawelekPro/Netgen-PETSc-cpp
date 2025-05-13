#include "FvmVector.hpp"

FvmVector::FvmVector(const int ghostsNb, const std::vector<int> &ghostsVec)
    : _ghostsNb(ghostsNb),
      _ghostsVec(ghostsVec) {
}

void FvmVector::V_SetCmp(const Vec *v, const int ind, const double value) {
    VecSetValue(*v, ind, value, INSERT_VALUES);
}
