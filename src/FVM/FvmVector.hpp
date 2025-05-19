#ifndef FVMVECTOR_HPP
#define FVMVECTOR_HPP

#include "FvmMesh.hpp"

#include <vector>
#include <memory>


#include "petscksp.h"


class FvmVector {
public:
    static void Init(const std::shared_ptr<FvmMeshContainer> &fvmMesh) {
        if (!_instance) {
            _instance = new FvmVector(fvmMesh);
        }
    }

    static FvmVector &Instance() {
        if (!_instance) {
            throw std::runtime_error("FvmVector not initialized. Call Init first.");
        }
        return *_instance;
    }

    static void V_Constr(Vec *v, int n, int sequential);

    static void V_SetCmp(const Vec *v, int ind, double value);

    // static void V_SetAllCmp(Vec *v, double value);

    static double V_GetCmp(const Vec *v, int ind);

private:
    explicit FvmVector(const std::shared_ptr<FvmMeshContainer> &fvmMesh);

private:
    int _ghostsNb = 0;
    std::vector<int> _ghostsVec;

    static FvmVector *_instance;
};


#endif
